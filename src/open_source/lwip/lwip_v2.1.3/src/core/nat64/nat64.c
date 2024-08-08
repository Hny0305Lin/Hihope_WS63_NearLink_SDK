/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: implementation for NAT64
 * Author: NA
 * Create: 2019
 */
#include "lwip/opt.h"

#if LWIP_NAT64
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/icmp.h"
#include "lwip/dhcp.h"
#include "lwip/nd6.h"
#include "lwip/nat64.h"
#include "lwip/nat64_addr.h"
#include "lwip/nat64_dns64.h"
#include "lwip/nat64_v4_dhcpc.h"
#include "lwip/stats.h"
#include "lwip/lwip_rpl.h"
#include "lwip/netifapi.h"
#include "lwip/ip6in4.h"
#include "rpl_common.h"
#include "rpl_event_api.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip6_frag.h"
#include "lwip/ip4_frag.h"
#if LWIP_NA_PROXY_UNSOLICITED
#include "lwip/prot/nd6.h"
#endif /* LWIP_NA_PROXY_UNSOLICITED */
#if LWIP_LOWPOWER
#include "lwip/lowpower.h"
#endif
#if RPL_CONF_SWITCH_MBR_BY_AUTOLINK
#include "mesh_route_api.h"
#endif
#define NAT64_NETIF_NAME "wlan"
#define NAT64_NETIF_NAME_LEN_MIN 4

#define NAT64_WAIT_DHCP_RELEASE_PERIOD 2

#if LWIP_ICMP6
#define NAT64_ICMP6_NO_ROUTE(iphdr, p)   do {    \
  if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {    \
    icmp6_dest_unreach((p), ICMP6_DUR_NO_ROUTE); \
  } \
} while (0)
#else
#define NAT64_ICMP6_NO_ROUTE(iphdr, p)
#endif /* LWIP_ICMP6 */

typedef struct {
  ip4_addr_t nhop;
  struct ip_hdr ip4hdr;
  u16_t ip4_hdr_len;
  u16_t ip6_hdr_len;
  u16_t ip_pbuf_data_len;
  u8_t nexth;
} nat64_ip6_data_t;

#if RPL_CONF_SWITCH_MBR_BY_AUTOLINK
typedef struct {
  linklayer_addr_t mac; /* the mg node mac address */
  u32_t local_ip; /* the sender msg mbr ip address */
  u8_t type; /* the event type */
} lwip_autolink_event_t;

enum lwip_mbr_msg_type_e {
  MSG_NODE_CHANGE_MBR = 0,
  MSG_NODE_CHANGE_MBR_ACK,
  MSG_MAX
};
#endif

/*
 * the memory MAY be alloc dynamicly in the init function,
 * when we want to support stateful dynamicly.
 */
static nat64_entry_t *g_nat64_table = NULL;
static struct netif *g_nat64_netif = NULL;

static void nat64_dhcp_proxy_stop(nat64_entry_t *entry);
static nat64_entry_t *nat64_entry_new(const linklayer_addr_t *lladdr, u8_t dao_sn, u8_t mnid, u32_t lifetime,
                                      u8_t nat64_sync, u32_t conn_time);

err_t
nat64_entry_add_new(nat64_entry_t *entry)
{
  int i;
  err_t ret;
  nat64_entry_t *nate_new = NULL;
  if (g_nat64_table == NULL) {
    return ERR_VAL;
  }

  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    if (g_nat64_table[i].state == NAT64_STATE_INIT) {
      continue;
    }
    if (lwip_lladdr_cmp(&g_nat64_table[i].mac, &entry->mac, sizeof(linklayer_addr_t))) {
      if (!ip4_addr_isany_val(g_nat64_table[i].ip)) {
        return ERR_OK;
      }
      /* when mg change in diffrent mbr, can't add nat entry from other mbr if it has not got ipv4 addr yet */
      if (g_nat64_table[i].nat64_sync == lwIP_FALSE) {
        ret = nat64_dhcp_stop(g_nat64_netif, &g_nat64_table[i].mac, lwIP_FALSE);
        if (ret != ERR_OK) {
          LWIP_DEBUGF(NAT64_DEBUG, ("stop nat64 dhcp proxy fail, mac:\n"));
        }
      }
      (void)nat64_entry_update(entry, lwIP_FALSE);
      return ERR_OK;
    }
  }

  nate_new = nat64_entry_new(&entry->mac, entry->dao_sn, entry->mnid, entry->lifetime, lwIP_TRUE, entry->conn_time);
  if (nate_new != NULL) {
    ip4_addr_copy(nate_new->ip, entry->ip);
    nate_new->orig_mnid = entry->orig_mnid;
    nate_new->state = entry->state;
  } else {
    return ERR_VAL;
  }
  return ERR_OK;
}


err_t
nat64_entry_mac_to_idx(const u8_t *hwaddr, u8_t hwaddr_len, dhcp_num_t *mac_idx)
{
  u16_t i;
  nat64_entry_t *entry = NULL;
  if (g_nat64_table == NULL) {
    return ERR_VAL;
  }
  if ((hwaddr == NULL) || (mac_idx == NULL)) {
    return ERR_ARG;
  }
  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    entry = &g_nat64_table[i];
    if (entry->state == NAT64_STATE_INIT) {
      continue;
    }
    if ((entry->mac.addrlen == hwaddr_len) &&
        (memcmp(entry->mac.addr, hwaddr, entry->mac.addrlen) == 0)) {
      *mac_idx = i + 1;
      return ERR_OK;
    }
  }

  return ERR_VAL;
}

nat64_entry_t *
nat64_get_next_inuse_entry(int *state)
{
  nat64_entry_t *nate = NULL;
  uint16_t i;
  if ((g_nat64_table == NULL) || (state == NULL)) {
    return NULL;
  }

  if (*state < 0) {
    *state = 0;
  }
  i = (uint16_t)*state;
  for (; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    nate = &g_nat64_table[i];
    if (nate->state != NAT64_STATE_INIT) {
      (*state) = i + 1;
      return nate;
    }
  }
  return NULL;
}

u8_t
nat64_entry_traverse(rpl_nate_cb cb, void *arg)
{
  int state = 0;
  nat64_entry_t *nate = NULL;
  if (cb == NULL) {
    return ERR_OK;
  }

  while ((nate = nat64_get_next_inuse_entry(&state)) != NULL) {
    if (cb(nate, arg) != ERR_OK) {
      continue;
    }
  }

  return ERR_OK;
}

err_t
nat64_entry_idx_to_mac(dhcp_num_t mac_idx, u8_t *hwaddr, u8_t *hwaddr_len)
{
  int ret;
  u16_t i = mac_idx;
  nat64_entry_t *entry = NULL;
  /* hwaddr_len should not be less than half of the max len */
  if ((i < 1) || (i > LWIP_NAT64_ENTRY_SIZE) || (g_nat64_table == NULL) ||
      (hwaddr == NULL) || (hwaddr_len == NULL) || (*hwaddr_len < (NETIF_MAX_HWADDR_LEN / 2))) {
    return ERR_ARG;
  }

  entry = &g_nat64_table[i - 1];
  if (entry->state == NAT64_STATE_INIT) {
    return ERR_VAL;
  }

  ret = memcpy_s(hwaddr, *hwaddr_len, entry->mac.addr, entry->mac.addrlen);
  if (ret != EOK) {
    return ERR_VAL;
  }

  *hwaddr_len = entry->mac.addrlen;
  return ERR_OK;
}

err_t
nat64_entry_idx_to_ip6addr(dhcp_num_t mac_idx, ip6_addr_t *ip6addr)
{
  u16_t i = mac_idx;
  nat64_entry_t *entry = NULL;
  if ((i < 1) || (i > LWIP_NAT64_ENTRY_SIZE) || (ip6addr == NULL) || (g_nat64_table == NULL)) {
    return ERR_ARG;
  }

  entry = &g_nat64_table[i - 1];
  if (entry->state == NAT64_STATE_INIT) {
    return ERR_VAL;
  }

  if (nat64_entry_to6(entry, ip6addr) != 0) {
    return ERR_VAL;
  }
  return ERR_OK;
}

static nat64_entry_t *
nat64_entry_lookup_by_ip4addr(const ip4_addr_t *ip4addr)
{
  s16_t i;
#if LWIP_NAT64_MIN_SUBSTITUTE
  ip4_addr_t ip;
#endif
  nat64_entry_t *entry = NULL;
  if ((g_nat64_table == NULL) || (ip4addr == NULL)) {
    return NULL;
  }

#if LWIP_NAT64_MIN_SUBSTITUTE
  (void)memset_s(&ip, sizeof(ip4_addr_t), 0, sizeof(ip4_addr_t));
#endif
  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    entry = &g_nat64_table[i];
    if ((entry->state == NAT64_STATE_ESTABLISH) &&
#if !LWIP_NAT64_MIN_SUBSTITUTE
        ip4_addr_cmp(&entry->ip, ip4addr)
#elif LWIP_DHCP_SUBSTITUTE
        (dhcp_substitute_idx_to_ip(g_nat64_netif, i + 1, &ip) == ERR_OK) &&
        ip4_addr_cmp(&ip, ip4addr)
#else
        (lwIP_FALSE)
#endif
       ) {
      return entry;
    }
  }
  return NULL;
}

nat64_entry_t *
nat64_entry_lookup_by_mac(const linklayer_addr_t *mac)
{
  s16_t i;
  nat64_entry_t *entry = NULL;
  if ((mac == NULL) || (g_nat64_table == NULL)) {
    return NULL;
  }

  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    entry = &g_nat64_table[i];
    if (memcmp(&entry->mac, mac, sizeof(linklayer_addr_t)) == 0) {
      return entry;
    }
  }
  return NULL;
}

err_t
nat64_entry_remove(nat64_entry_t *entry, u8_t evt_flag)
{
  if ((entry == NULL) || (entry->state == NAT64_STATE_INIT)) {
    return ERR_ARG;
  }

#if LWIP_MMBR && LWIP_NAT64_CHANGE_MSG
  if ((evt_flag == lwIP_TRUE) && (entry->nat64_sync == lwIP_FALSE) && (!ip4_addr_isany_val(entry->ip))) {
    nat64_send_change_entry_msg(entry, RPL_EVT_NODE_NAT64_DEL);
  }
#else
  (void)evt_flag;
#endif /* LWIP_MMBR && LWIP_NAT64_CHANGE_MSG */

  (void)memset_s(entry, sizeof(nat64_entry_t), 0, sizeof(nat64_entry_t));
  entry->state = NAT64_STATE_INIT;
  return ERR_OK;
}

err_t
nat64_entry_update(nat64_entry_t *entry, u8_t evt_flag)
{
  if ((g_nat64_table == NULL) || (entry == NULL)) {
    return ERR_ARG;
  }

  nat64_entry_t *nate_exist = nat64_entry_lookup_by_mac(&entry->mac);
  if (nate_exist == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("%s:old nate has disappeared\n", __FUNCTION__));
    /*
     * the nat64 entry may delete for timeout for loss period synchronization packet,
     * let to nat64 entry inconsistency issue.
     */
    return nat64_entry_add_new(entry);
  }
#if !LWIP_NAT64_MIN_SUBSTITUTE
  ip4_addr_copy(nate_exist->ip, entry->ip);
#endif
  nate_exist->lifetime = entry->lifetime;
  nate_exist->mnid = entry->mnid;
  nate_exist->orig_mnid = entry->orig_mnid;
  nate_exist->state = entry->state;
  nate_exist->nat64_sync = evt_flag;
  nate_exist->conn_time = entry->conn_time;
  nate_exist->dao_sn = entry->dao_sn;
  return ERR_OK;
}

static void
nat64_table_init(void)
{
  g_nat64_table = (nat64_entry_t *)mem_malloc(sizeof(nat64_entry_t) * LWIP_NAT64_ENTRY_SIZE);
  if (g_nat64_table == NULL) {
    return;
  }
  (void)memset_s(g_nat64_table, sizeof(nat64_entry_t) * LWIP_NAT64_ENTRY_SIZE,
                 0, sizeof(nat64_entry_t) * LWIP_NAT64_ENTRY_SIZE);
}

static void
nat64_table_deinit(void)
{
  s16_t i;
  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    if (g_nat64_table[i].state == NAT64_STATE_INIT) {
      continue;
    }

    if (g_nat64_table[i].nat64_sync == lwIP_FALSE) {
      (void)nat64_dhcp_stop(g_nat64_netif, &(g_nat64_table[i].mac), lwIP_TRUE);
    }
    /* when mbr exit, we just remove nat64 entry */
    (void)nat64_entry_remove(&g_nat64_table[i], lwIP_TRUE);
  }
  mem_free(g_nat64_table);
  g_nat64_table = NULL;
}

void
nat64_set_statful_enable(void)
{
  if (g_nat64_table != NULL) {
    return;
  }
  /* init the table */
  nat64_table_init();
  lwip_rpl_trigger_global_dao();
  lwip_rpl_trigger_msta();
}

void
nat64_set_statful_disable(void)
{
  nat64_table_deinit();
}

static nat64_entry_t *
nat64_entry_new(const linklayer_addr_t *lladdr, u8_t dao_sn, u8_t mnid, u32_t lifetime, u8_t nat64_sync,
                u32_t conn_time)
{
  s16_t i;
  if (g_nat64_table == NULL) {
    return NULL;
  }
  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    if (g_nat64_table[i].state == NAT64_STATE_INIT) {
      g_nat64_table[i].mac = *lladdr;
      g_nat64_table[i].dao_sn = dao_sn;
      g_nat64_table[i].mnid = mnid;
      g_nat64_table[i].orig_mnid = mnid;
      g_nat64_table[i].lifetime = lifetime;
      g_nat64_table[i].conn_time = conn_time;
      g_nat64_table[i].nat64_sync = nat64_sync;
      g_nat64_table[i].state = NAT64_STATE_CREATED;
      return &g_nat64_table[i];
    }
  }

  LWIP_DEBUGF(NAT64_DEBUG, ("no available entry\r\n"));
  return NULL;
}

static err_t
nat64_ip4_dest_addr_exist(const ip4_addr_t *ip4addr)
{
  ip6_addr_t ip6addr;
  nat64_entry_t *entry = NULL;

  if (ip4addr == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("%s:ip4addr is NULL\n", __FUNCTION__));
    return ERR_ARG;
  }
  (void)memset_s(&ip6addr, sizeof(ip6_addr_t), 0, sizeof(ip6_addr_t));

  /* stateful checkout */
  entry = nat64_entry_lookup_by_ip4addr(ip4addr);
  if (entry != NULL) {
    if (entry->nat64_sync == lwIP_TRUE) {
      return ERR_VAL;
    }
    return ERR_OK;
  }
#ifdef LWIP_ARP_PROXY_BY_ROUTE_ENTRY
#if LWIP_RIPPLE
  if (nat64_stateless_addr_4to6(ip4addr, &ip6addr) != 0) {
    return ERR_ARG;
  }
  /* stateless checkout */
  if (lwip_rpl_route_entry_lookup(&ip6addr) != NULL) {
    return ERR_OK;
  }
#endif
#endif
  return ERR_VAL;
}

u8_t
nat64_arp_ip4_is_proxy(ip4_addr_t sipaddr, ip4_addr_t dipaddr)
{
#if LWIP_RIPPLE
  if ((lwip_rpl_is_br()) && !(ip4_addr_isany_val(sipaddr)) && (nat64_ip4_dest_addr_exist(&dipaddr) == ERR_OK)) {
    return lwIP_TRUE;
  }
  if (!(lwip_rpl_is_br()) && lwip_rpl_is_router() && (sipaddr.addr != dipaddr.addr) &&
      !(ip4_addr_isany_val(sipaddr)) && (nat64_ip4_dest_addr_exist(&dipaddr) == ERR_VAL)) {
    return lwIP_TRUE;
  }
#endif
  return lwIP_FALSE;
}

static int
nat64_ip4_dest_addr_check(const struct pbuf *p)
{
  u32_t addr = lwip_htonl(ip4_addr_get_u32(ip4_current_dest_addr()));

  if (ip4_addr_islinklocal(ip4_current_dest_addr())) {
    return 0;
  }

  if (p->flags & PBUF_FLAG_LLBCAST) {
    /* don't route link-layer broadcasts */
    return 0;
  }

  if ((p->flags & PBUF_FLAG_LLMCAST) || IP_MULTICAST(addr)) {
    /* don't route link-layer multicasts (use LWIP_HOOK_IP4_CANFORWARD instead) */
    return 0;
  }

  if (IP_EXPERIMENTAL(addr)) {
    return 0;
  }

  if (IP_CLASSA(addr)) {
    u32_t net = addr & IP_CLASSA_NET;
    if ((net == 0) || (net == ((u32_t)IP_LOOPBACKNET << IP_CLASSA_NSHIFT))) {
      /* don't route loopback packets */
      return 0;
    }
  }
  return 1;
}

static u8_t
nat64_ip4_forward(struct pbuf *p, struct ip_hdr *iphdr, struct netif *inp)
{
  /* decrement TTL */
  if (IPH_TTL(iphdr)) {
    IPH_TTL_SET(iphdr, IPH_TTL(iphdr) - 1);
  }

  /* send ICMP if TTL == 0 */
  if (IPH_TTL(iphdr) == 0) {
    MIB2_STATS_INC(mib2.ipinhdrerrors);
#if LWIP_ICMP
    /* Don't send ICMP messages in response to ICMP error messages */
    if (IPH_PROTO(iphdr) == IP_PROTO_ICMP) {
      (void)pbuf_header(p, -IP_HLEN);
      if ((((u8_t *)p->payload)[0] == ICMP_ER) || (((u8_t *)p->payload)[0] == ICMP_ECHO) ||
          (((u8_t *)p->payload)[0] == ICMP_TS) || (((u8_t *)p->payload)[0] == ICMP_TSR)) {
        (void)pbuf_header(p, IP_HLEN);
        icmp_time_exceeded(p, ICMP_TE_TTL);
      } else {
        (void)pbuf_header(p, IP_HLEN);
      }
    } else {
      icmp_time_exceeded(p, ICMP_TE_TTL);
    }
#endif /* LWIP_ICMP */
    return 1;
  }

  /* Incrementally update the IP checksum. */
  if (IPH_CHKSUM(iphdr) >= PP_HTONS(0xffffU - 0x100)) {
    IPH_CHKSUM_SET(iphdr, (u16_t)(IPH_CHKSUM(iphdr) + PP_HTONS(0x100) + 1));
  } else {
    IPH_CHKSUM_SET(iphdr, (u16_t)(IPH_CHKSUM(iphdr) + PP_HTONS(0x100)));
  }

  LWIP_DEBUGF(NAT64_DEBUG, ("ip4_forward: forwarding packet to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
                            ip4_addr1_16(ip4_current_dest_addr()), ip4_addr2_16(ip4_current_dest_addr()),
                            ip4_addr3_16(ip4_current_dest_addr()), ip4_addr4_16(ip4_current_dest_addr())));

  IP_STATS_INC(ip.fw);
  MIB2_STATS_INC(mib2.ipforwdatagrams);
  IP_STATS_INC(ip.xmit);

  /* transmit pbuf on chosen interface */
  (void)inp->output(inp, p, ip4_current_dest_addr());
  return 1;
}

int
nat64_status_check(void)
{
  /* we need start the nat64 dynamicly */
  if (g_nat64_table == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_status_check: nat64 is not init\n"));
    return NAT64_RET_FAIL;
  }
#if LWIP_RIPPLE
  /* the node must be mesh node */
  if (lwip_rpl_is_router() == lwIP_FALSE) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_status_check: not mesh node\n"));
    return NAT64_RET_FAIL;
  }
#endif
  return NAT64_RET_OK;
}
static int
nat64_ip4_packet_check(struct pbuf *p, const struct ip_hdr *iphdr, const struct netif *inp)
{
  /* do not handle the broadcast packet */
  if (ip4_addr_isbroadcast(ip4_current_dest_addr(), inp)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip4_packet_check: broadcast address\n"));
    return NAT64_RET_FAIL;
  }
  if (nat64_ip4_dest_addr_check(p) == 0) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip4_packet_check: nat64_ip4_dest_addr_check fail\n"));
    return NAT64_RET_FAIL;
  }
  if ((IPH_TTL(iphdr) == 0) || (IPH_TTL(iphdr) == 1)) {
#if LWIP_ICMP
    /* Don't send ICMP messages in response to ICMP error messages */
    if (IPH_PROTO(iphdr) == IP_PROTO_ICMP) {
      (void)pbuf_header(p, -IP_HLEN);
      if ((((u8_t *)p->payload)[0] == ICMP_ER) || (((u8_t *)p->payload)[0] == ICMP_ECHO) ||
          (((u8_t *)p->payload)[0] == ICMP_TS) || (((u8_t *)p->payload)[0] == ICMP_TSR)) {
        (void)pbuf_header(p, IP_HLEN);
        icmp_time_exceeded(p, ICMP_TE_TTL);
      } else {
        (void)pbuf_header(p, IP_HLEN);
      }
    } else {
      icmp_time_exceeded(p, ICMP_TE_TTL);
    }
#endif /* LWIP_ICMP */
    IP_STATS_INC(ip.natdrop);
    LWIP_DEBUGF(NAT64_DEBUG, ("ttl exceeded\n"));
    return NAT64_RET_ERR;
  }
  return NAT64_RET_OK;
}

static int
nat64_ip4_no_route(struct pbuf *p, struct ip_hdr *iphdr,
                   const struct netif *inp, struct netif *outp)
{
  if (lwip_rpl_is_rpl_netif(inp) == lwIP_TRUE) {
    /* use the default router */
    if (lwip_rpl_is_br() == lwIP_TRUE) {
      (void)nat64_ip4_forward(p, iphdr, outp);
      LWIP_DEBUGF(NAT64_DEBUG, ("%s:%d: ip6_route default forward.\n", __FUNCTION__, __LINE__));
      return NAT64_RET_ERR;
    }
  } else {
    /* the pkt should be drop */
    IP_STATS_INC(ip.natdrop);
    LWIP_DEBUGF(NAT64_DEBUG, ("%s:%d: ip6_route drop pkt.\n", __FUNCTION__, __LINE__));
    return NAT64_RET_ERR;
  }

  return NAT64_RET_OK;
}

static int
nat64_ip4_translate_to_ip6src(ip6_addr_t *ip6src, const struct netif *inp)
{
  int ret;
  nat64_entry_t *entry = NULL;
  if ((lwip_rpl_is_br() == lwIP_TRUE) &&
      (lwip_rpl_is_rpl_netif(inp) == lwIP_FALSE)) {
    entry = nat64_entry_lookup_by_ip4addr(ip4_current_src_addr());
    if ((entry != NULL) && (entry->nat64_sync == lwIP_TRUE)) {
      ret = nat64_entry_to6(entry, ip6src);
    } else {
      ret = nat64_stateless_addr_4to6(ip4_current_src_addr(), ip6src);
    }
  } else {
    ret = nat64_stateless_addr_4to6(ip4_current_src_addr(), ip6src);
  }
  return ret;
}

static int
nat64_ip4_translate_to_ip6dst(ip6_addr_t *ip6dest, const struct netif *inp)
{
  int ret;
  nat64_entry_t *entry = NULL;
  if ((lwip_rpl_is_br() == lwIP_TRUE) &&
      (lwip_rpl_is_rpl_netif(inp) == lwIP_FALSE)) {
    entry = nat64_entry_lookup_by_ip4addr(ip4_current_dest_addr());
    if (entry != NULL) {
      ret = nat64_entry_to6(entry, ip6dest);
    } else {
      /* just assume stateless, maybe drop the pkt later */
      ret = nat64_stateless_addr_4to6(ip4_current_dest_addr(), ip6dest);
    }
  } else {
    /* stateless ip address convert */
    ret = nat64_stateless_addr_4to6(ip4_current_dest_addr(), ip6dest);
  }
  return ret;
}


static int
nat64_ip4_route(struct pbuf *p, struct ip_hdr *iphdr, const struct netif *inp,
                ip6_addr_t *ip6src, ip6_addr_t *ip6dest, struct netif **outp)
{
  void *route = NULL;
  int ret;
  /* handle the two ipv4 sta connecting on the same MG */
  /* here we convert the address */
  ret = nat64_ip4_translate_to_ip6src(ip6src, inp);
  if (ret != 0) {
    IP_STATS_INC(ip.natdrop);
    return NAT64_RET_ERR;
  }
  ret = nat64_ip4_translate_to_ip6dst(ip6dest, inp);
  if (ret != 0) {
    IP_STATS_INC(ip.natdrop);
    return NAT64_RET_ERR;
  }
  /* here we can */
  *outp = ip6_route(ip6src, ip6dest);
  if (*outp == NULL) {
    IP_STATS_INC(ip.natdrop);
#if LWIP_ICMP
    icmp_dest_unreach(p, ICMP_DUR_SR);
#endif
    LWIP_DEBUGF(NAT64_DEBUG, ("%s:%d: ip6_route is null\n", __FUNCTION__, __LINE__));
    return NAT64_RET_ERR;
  }

  route = lwip_rpl_route_entry_lookup(ip6dest);
  if (route != NULL) {
    if (lwip_rpl_route_is_ipv4sta(route) == 1) {
      /* ipv4 forward the packet */
      LWIP_DEBUGF(NAT64_DEBUG, ("%s:%d: ip6_route forward.\n", __FUNCTION__, __LINE__));
      (void)nat64_ip4_forward(p, iphdr, *outp);
      return NAT64_RET_ERR;
    }
  } else {
    return nat64_ip4_no_route(p, iphdr, inp, *outp);
  }
  return NAT64_RET_OK;
}

static int
nat64_ip4_reass_check(struct pbuf **p, struct ip_hdr **iphdr, const struct netif *inp,
                      const ip6_addr_t *nexthop, const struct netif *outp)
{
  u16_t ip4_hdr_len;
  u16_t ip6_hdr_len;
  u16_t ip_pbuf_data_len;
  if ((IPH_OFFSET(*iphdr) & lwip_htons(IP_OFFMASK | IP_MF)) != 0) {
#if IP_REASSEMBLY /* packet fragment reassembly code present? */
    /* reassemble the packet */
    *p = ip4_reass(*p);
    /* packet not fully reassembled yet? */
    if (*p == NULL) {
      LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip4_input:fragment is not fully reassembled yet\n"));
      /* note: the pkt has been free in the ip4_reass */
      return NAT64_RET_ASSEMBLE;
    }
    *iphdr = (struct ip_hdr *)(*p)->payload;
#else /* IP_REASSEMBLY == 0, no packet fragment reassembly code present */
    (void)p;
    LWIP_DEBUGF(NAT64_DEBUG, ("IP packet dropped since it was fragmented (0x%"X16_F
                              ") (while IP_REASSEMBLY == 0).\n", lwip_ntohs(IPH_OFFSET(*iphdr))));
    return NAT64_RET_FAIL;
#endif /* IP_REASSEMBLY */
  }

  /*
   * MAY convert the ipv4 header options into ipv6 options, the ip_pbuf_data_len should include the options.
   * now we just drop the options.
   */
  ip6_hdr_len = IP6_HLEN;
  ip4_hdr_len = IPH_HL(*iphdr) << 2; /* ipv4 header len is equal to HL * 4 */
  ip_pbuf_data_len = (u16_t)(lwip_ntohs(IPH_LEN(*iphdr)) - ip4_hdr_len);
#if !LWIP_IPV6_FRAG
  /*
   * we can calculate the total len, cmp with the mtu.
   * now we donot support the ip fragment, so just drop it.
   */
  if (netif_mtu6(outp) && ((ip6_hdr_len + ip_pbuf_data_len) > nd6_get_destination_mtu(nexthop, outp))) {
    IP_STATS_INC(ip.natdrop);
    return NAT64_RET_ERR;
  }
#else
  (void)ip6_hdr_len;
  (void)ip4_hdr_len;
  (void)ip_pbuf_data_len;
  (void)nexthop;
  (void)outp;
#endif
  (void)inp;
  return NAT64_RET_OK;
}
#if LWIP_DNS64
static u16_t
nat64_dns64_extra_size(struct pbuf *p, const struct ip_hdr *iphdr)
{
  struct udp_hdr *udphdr = NULL;
  u16_t ip4_hdr_len;
  u16_t count = 0;

  if (IPH_PROTO(iphdr) != IP_PROTO_UDP) {
    return 0;
  }

  /* ipv4 header len is equal to HL * 4 */
  ip4_hdr_len = IPH_HL(iphdr) << 2;
  (void)pbuf_header(p, -((s16_t)(ip4_hdr_len)));
  udphdr = (struct udp_hdr *)p->payload;
  if (udphdr->src != lwip_ntohs(NAT64_DNS_PORT)) {
    (void)pbuf_header(p, (s16_t)(ip4_hdr_len));
    return 0;
  }

  (void)pbuf_header(p, -((s16_t)(sizeof(struct udp_hdr))));

  if (nat64_dns64_extra_count(p, &count) != ERR_OK) {
    (void)pbuf_header(p, (s16_t)(sizeof(struct udp_hdr)));
    (void)pbuf_header(p, (s16_t)(ip4_hdr_len));
    return 0;
  }

  (void)pbuf_header(p, (s16_t)(sizeof(struct udp_hdr)));
  (void)pbuf_header(p, (s16_t)(ip4_hdr_len));
  return count * (sizeof(ip6_addr_t) - sizeof(ip4_addr_t) + DNS_MAX_NAME_LENGTH);
}
#endif
static int
nat64_ip4_send(struct pbuf *new_buf, const struct ip6_hdr *ip6hdr,
               struct netif *outp, const ip6_addr_t *ip6dest)
{
  int ret;
  u8_t *ip6hdr_payload = NULL;
  const ip6_addr_t *nexthop = ip6dest;

  (void)pbuf_header(new_buf, IP6_HLEN);

  ip6hdr_payload = (u8_t *)new_buf->payload;
  /* the endian convert */
  ret = memcpy_s(ip6hdr_payload, sizeof(struct ip6_hdr), ip6hdr, sizeof(struct ip6_hdr));
  if (ret != EOK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("%s:memcpy_s fail(%d)\n", __FUNCTION__, ret));
    (void)pbuf_free(new_buf);
    return NAT64_PKT_HANDLED;
  }
#if LWIP_RIPPLE
  if (new_buf->flags & PBUF_FLAG_HBH_SPACE) {
    struct pbuf *new_p = NULL;
    /*
     * add Hop by Hop header for rpl. If space for HBH is not allocated then pbuf
     * will be expanded.
     */
    if (lwip_get_pkt_route_status()) {
      new_buf->pkt_up = lwIP_TRUE;
    } else {
      new_buf->pkt_up = lwIP_FALSE;
    }
    new_p = lwip_add_rpi_hdr(new_buf, IP6H_NEXTH(ip6hdr), lwip_hbh_len(new_buf), 0);
    if (new_p == NULL) {
      LWIP_ERROR("Could not add HBH header.\n", 0, ;);
      (void)pbuf_free(new_buf);
      return NAT64_PKT_HANDLED;
    } else {
      new_buf = new_p;
    }
  }
#endif

#if LWIP_IPV6_FRAG
  if (netif_mtu6(outp) && (new_buf->tot_len > nd6_get_destination_mtu(nexthop, outp))) {
    (void)ip6_frag(new_buf, outp, nexthop);
    (void)pbuf_free(new_buf);
    return NAT64_PKT_HANDLED;
  }
#endif

  outp->output_ip6(outp, new_buf, ip6dest);
  IP6_STATS_INC(ip.natfw);
  IP6_STATS_INC(ip6.xmit);
  (void)pbuf_free(new_buf);
  return NAT64_PKT_HANDLED;
}

static int
nat64_ip4_translate_udp(struct pbuf *p, struct pbuf *new_buf, struct ip6_hdr *ip6hdr,
                        const ip6_addr_t *ip6src, const ip6_addr_t *ip6dest)
{
  struct udp_hdr *udphdr = (struct udp_hdr *)new_buf->payload;
#if LWIP_DNS64
  if (udphdr->src == lwip_ntohs(NAT64_DNS_PORT)) {
    (void)pbuf_header(p, -((s16_t)(sizeof(struct udp_hdr))));
    (void)pbuf_header(new_buf, -((s16_t)(sizeof(struct udp_hdr))));
    if (nat64_dns64_4to6(p, new_buf) != ERR_OK) {
      IP_STATS_INC(ip.natdrop);
#if LWIP_ICMP
      icmp_dest_unreach(p, ICMP_DUR_SR);
#endif
      (void)pbuf_free(new_buf);
      return NAT64_RET_ERR;
    }
    udphdr->len = lwip_htons(new_buf->tot_len);
    (void)pbuf_header(p, (s16_t)(sizeof(struct udp_hdr)));
    (void)pbuf_header(new_buf, (s16_t)(sizeof(struct udp_hdr)));
    IP6H_PLEN_SET(ip6hdr, new_buf->tot_len);
  }
#else
  (void)p;
  (void)ip6hdr;
#endif
  udphdr->chksum = 0;
  udphdr->chksum = ip6_chksum_pseudo(new_buf, IP_PROTO_UDP, new_buf->tot_len,
                                     ip6src, ip6dest);
  return NAT64_RET_OK;
}

static void
nat64_ip4_translate_tcp(struct pbuf *new_buf, const ip6_addr_t *ip6src, const ip6_addr_t *ip6dest)
{
  struct tcp_hdr *tcphdr = (struct tcp_hdr *)new_buf->payload;
  tcphdr->chksum = 0;
  tcphdr->chksum = ip6_chksum_pseudo(new_buf, IP_PROTO_TCP, new_buf->tot_len, ip6src, ip6dest);
}

static int
nat64_ip4_translate_icmp(struct pbuf *new_buf, const ip6_addr_t *ip6src, const ip6_addr_t *ip6dest)
{
  struct icmp_echo_hdr *icmp4hdr = (struct icmp_echo_hdr *)new_buf->payload;
  struct icmpv6_hdr *icmp6hdr = (struct icmpv6_hdr *)new_buf->payload;
  /* just handle the echo icmp */
  if (ICMPH_TYPE(icmp4hdr) == ICMP_ECHO) {
    icmp6hdr->type = ICMP6_TYPE_EREQ;
  } else if (ICMPH_TYPE(icmp4hdr) == ICMP_ER) {
    icmp6hdr->type = ICMP6_TYPE_EREP;
  } else {
    LWIP_DEBUGF(NAT64_DEBUG, ("icmp4 not handle %x\n", ICMPH_TYPE(icmp4hdr)));
    (void)pbuf_free(new_buf);
    IP_STATS_INC(ip.natdrop);
    return NAT64_RET_ERR;
  }

  icmp6hdr->chksum = 0;
#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    icmp6hdr->chksum = ip6_chksum_pseudo(new_buf, IP6_NEXTH_ICMP6, new_buf->tot_len,
                                         ip6src, ip6dest);
  }
#endif /* CHECKSUM_GEN_ICMP6 */
  return NAT64_RET_OK;
}

static int
nat64_ip4_translate_default(struct pbuf *p, struct pbuf *new_buf,
                            const struct netif *inp, u16_t ip4_hdr_len)
{
#if LWIP_ICMP
  /* send ICMP destination protocol unreachable unless is was a broadcast */
  if (!ip4_addr_isbroadcast(ip4_current_dest_addr(), inp) &&
      !ip4_addr_ismulticast(ip4_current_dest_addr())) {
    (void)pbuf_header_force(p, (s16_t)(ip4_hdr_len));
    icmp_dest_unreach(p, ICMP_DUR_PROTO);
  }
#endif /* LWIP_ICMP */
  LWIP_DEBUGF(NAT64_DEBUG, ("pkt not handle\n"));
  (void)pbuf_free(new_buf);
  IP_STATS_INC(ip.natdrop);
  return NAT64_PKT_HANDLED;
}

static struct pbuf *
nat64_ip4_new_buf(struct pbuf *p, const struct ip_hdr *iphdr, struct ip6_hdr *ip6hdr, u16_t *ip4_hdr_len)
{
  u16_t ip_pbuf_data_len;
  u16_t dns64_extra_size;
  struct pbuf *new_buf = NULL;
  /* ipv4 header len is equal to HL * 4 */
  *ip4_hdr_len = IPH_HL(iphdr) << 2;
  ip_pbuf_data_len = (u16_t)(p->tot_len - *ip4_hdr_len);
#if LWIP_DNS64
  dns64_extra_size = nat64_dns64_extra_size(p, iphdr);
#else
  dns64_extra_size = 0;
#endif
  ip_pbuf_data_len += dns64_extra_size;
  /* here PBUF_LINK should be used */
  new_buf = pbuf_alloc(PBUF_IP, ip_pbuf_data_len, PBUF_RAM);
  if (new_buf == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("pbuf alloc fail:no MEMORY\r\n"));
    return NULL;
  }

#if defined(LWIP_NAT64_PRIORITY_KEEP) && LWIP_NAT64_PRIORITY_KEEP
  IP6H_VTCFL_SET(ip6hdr, IP_PROTO_VERSION_6, IPH_TOS(iphdr), 0);
#else
  IP6H_VTCFL_SET(ip6hdr, IP_PROTO_VERSION_6, 0, 0);
#endif

  IP6H_PLEN_SET(ip6hdr, ip_pbuf_data_len);
  IP6H_HOPLIM_SET(ip6hdr, (IPH_TTL(iphdr) - 1));

  return new_buf;
}

static int
nat64_ip4_translate(struct pbuf *p, const struct ip_hdr *iphdr, const struct netif *inp,
                    const ip6_addr_t *ip6src, const ip6_addr_t *ip6dest, struct netif *outp)
{
  int ret;
  struct ip6_hdr ip6hdr;
  u16_t ip4_hdr_len;
  struct pbuf *new_buf = NULL;

  (void)memset_s(&ip6hdr, sizeof(ip6hdr), 0, sizeof(ip6hdr));

  new_buf = nat64_ip4_new_buf(p, iphdr, &ip6hdr, &ip4_hdr_len);
  if (new_buf == NULL) {
    return NAT64_PKT_NOT_HANDLED;
  }

  ip6_addr_copy_to_packed(ip6hdr.dest, *ip6dest);
  ip6_addr_copy_to_packed(ip6hdr.src, *ip6src);

  (void)pbuf_header(p, -((s16_t)(ip4_hdr_len)));
  if (pbuf_copy(new_buf, p) != ERR_OK) {
    (void)pbuf_free(new_buf);
    return NAT64_PKT_HANDLED;
  }

  switch (IPH_PROTO(iphdr)) {
#if LWIP_UDP
    case IP_PROTO_UDP:
      IP6H_NEXTH_SET(&ip6hdr, IP6_NEXTH_UDP);
      ret = nat64_ip4_translate_udp(p, new_buf, &ip6hdr, ip6src, ip6dest);
      if (ret != NAT64_RET_OK) {
        return NAT64_PKT_HANDLED;
      }
      break;
#endif /* LWIP_UDP */
#if LWIP_TCP
    case IP_PROTO_TCP:
      IP6H_NEXTH_SET(&ip6hdr, IP6_NEXTH_TCP);
      nat64_ip4_translate_tcp(new_buf, ip6src, ip6dest);
      break;
#endif /* LWIP_TCP */
#if LWIP_ICMP
    case IP_PROTO_ICMP:
      IP6H_NEXTH_SET(&ip6hdr, IP6_NEXTH_ICMP6);
      ret = nat64_ip4_translate_icmp(new_buf, ip6src, ip6dest);
      if (ret != NAT64_RET_OK) {
        return NAT64_PKT_HANDLED;
      }
      break;
#endif /* LWIP_ICMP */
    default:
      return nat64_ip4_translate_default(p, new_buf, inp, ip4_hdr_len);
  }

  return nat64_ip4_send(new_buf, &ip6hdr, outp, ip6dest);
}

/* we should handle the stateless & stateful */
int
nat64_ip4_input(struct pbuf *p, struct ip_hdr *iphdr, const struct netif *inp)
{
  struct netif *outp = NULL;
  ip6_addr_t ip6src;
  ip6_addr_t ip6dest;
  int ret;

  (void)memset_s(&ip6src, sizeof(ip6src), 0, sizeof(ip6src));
  (void)memset_s(&ip6dest, sizeof(ip6dest), 0, sizeof(ip6dest));

  if (nat64_status_check() != NAT64_RET_OK) {
    return NAT64_PKT_NOT_HANDLED;
  }

  ret = nat64_ip4_packet_check(p, iphdr, inp);
  if (ret == NAT64_RET_FAIL) {
    return NAT64_PKT_NOT_HANDLED;
  } else if (ret == NAT64_RET_ERR) {
    (void)pbuf_free(p);
    return NAT64_PKT_HANDLED;
  }

  ret = nat64_ip4_route(p, iphdr, inp, &ip6src, &ip6dest, &outp);
  if (ret == NAT64_RET_FAIL) {
    return NAT64_PKT_NOT_HANDLED;
  } else if (ret == NAT64_RET_ERR) {
    (void)pbuf_free(p);
    return NAT64_PKT_HANDLED;
  }

  if (outp == NULL) {
    (void)pbuf_free(p);
    return NAT64_PKT_HANDLED;
  }

  ret = nat64_ip4_reass_check(&p, &iphdr, inp, &ip6dest, outp);
  if (ret == NAT64_RET_FAIL) {
    return NAT64_PKT_NOT_HANDLED;
  } else if (ret == NAT64_RET_ERR) {
    (void)pbuf_free(p);
    return NAT64_PKT_HANDLED;
  } else if (ret == NAT64_RET_ASSEMBLE) {
    return NAT64_PKT_HANDLED;
  }

  /* the function will never return fail, but the return value must be checked */
  ret = nat64_ip4_translate(p, iphdr, inp, &ip6src, &ip6dest, outp);
  if (ret == NAT64_RET_FAIL) {
    return NAT64_PKT_NOT_HANDLED;
  }

  (void)pbuf_free(p);
  return NAT64_PKT_HANDLED;
}

static int
nat64_ip6_addr_convert(struct pbuf *p, const struct ip6_hdr *iphdr, struct ip_hdr *ip4hdr)
{
  int ip4src;
  int ip4dst;

  /* here we convert the address */
  if ((nat64_addr_6to4(ip6_current_dest_addr(), (ip4_addr_t *)&ip4hdr->dest) != 0) ||
      (nat64_addr_6to4(ip6_current_src_addr(), (ip4_addr_t *)&ip4hdr->src) != 0)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input: the ip6 address is not ip4-converted.\n"));
    ip4dst = nat64_addr_is_ip4(ip6_current_dest_addr());
    ip4src = nat64_addr_is_ip4(ip6_current_src_addr());
    if ((ip4src == lwIP_TRUE) || (ip4dst == lwIP_TRUE)) {
      /* the pkt should be drop */
      IP_STATS_INC(ip6.natdrop);
      NAT64_ICMP6_NO_ROUTE(iphdr, p);
      (void)pbuf_free(p);
      return NAT64_RET_ERR;
    }

    return NAT64_RET_FAIL;
  }

  return NAT64_RET_OK;
}

static int
nat64_ip6_packet_check(struct pbuf *p, const struct ip6_hdr *iphdr)
{
  /* do not handle the linklocal/loopback/multicast packet */
  if (ip6_addr_islinklocal(ip6_current_dest_addr()) ||
      ip6_addr_isloopback(ip6_current_dest_addr()) ||
      ip6_addr_ismulticast(ip6_current_dest_addr())) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input:pkt not handle\n"));
    return NAT64_RET_FAIL;
  }

  /* send ICMP6 if HL == 0 */
  if ((IP6H_HOPLIM(iphdr) == 0) || (IP6H_HOPLIM(iphdr) == 1)) {
#if LWIP_ICMP6
    /* Don't send ICMP messages in response to ICMP messages */
    if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {
      icmp6_time_exceeded(p, ICMP6_TE_HL);
    }
#endif /* LWIP_ICMP6 */
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input:hop limit drop\n"));
    IP6_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }

  return NAT64_RET_OK;
}

static int
nat64_ip6_route(struct pbuf *p, const struct ip6_hdr *iphdr, const struct netif *inp,
                nat64_ip6_data_t *data, struct netif **outp)
{
  int ret;
  struct ip_hdr *ip4hdr = &data->ip4hdr;
#if LWIP_IP6IN4 && LWIP_NAT64_IP6IN4
  ip6_addr_t nhop = {0};
#endif
  void *route = NULL;
  route = lwip_rpl_route_entry_lookup(ip6_current_dest_addr());
  if (route != NULL) {
    if ((lwip_rpl_route_is_sync(route) == 0) && (lwip_rpl_route_is_ipv4sta(route) == 0)) {
      /* ipv6 forward the packet */
      return NAT64_RET_FAIL;
    }
#if LWIP_IP6IN4 && LWIP_NAT64_IP6IN4
    ret = lwip_rpl_route_nhop(route, &nhop);
    if (ret == ERR_OK) {
      (void)ip6in4_entry_ip4_get(&nhop, &data->nhop);
    }
#endif
  } else {
    if (lwip_rpl_is_rpl_netif(inp) == lwIP_TRUE) {
      /* connect to the ipv4 router */
      if (lwip_rpl_is_br() == lwIP_FALSE) {
        LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input:no netif\n"));
        return NAT64_RET_FAIL;
      }
    } else {
      /* the pkt should be drop */
      IP_STATS_INC(ip6.natdrop);
      NAT64_ICMP6_NO_ROUTE(iphdr, p);
      LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input:no router\n"));
      (void)pbuf_free(p);
      return NAT64_RET_ERR;
    }
  }

  ret = nat64_ip6_addr_convert(p, iphdr, ip4hdr);
  if (ret != NAT64_RET_OK) {
    return ret;
  }

  *outp = ip4_route((ip4_addr_t *)&ip4hdr->dest);
  if (*outp == NULL) {
    IP_STATS_INC(ip6.natdrop);
    NAT64_ICMP6_NO_ROUTE(iphdr, p);
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input:no netif\n"));
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }

  return NAT64_RET_OK;
}

static void
nat64_ip6_assemble(struct pbuf *p, struct ip6_hdr **iphdr, nat64_ip6_data_t *data)
{
  /*
   * Returned p point to IPv6 header.
   * Update all our variables and pointers and continue.
   */
  *iphdr = (struct ip6_hdr *)p->payload;
  data->nexth = IP6H_NEXTH(*iphdr);
  data->ip6_hdr_len = 0;
  data->ip_pbuf_data_len = IP6H_PLEN(*iphdr);
  (void)pbuf_header(p, -IP6_HLEN);
  data->ip6_hdr_len = IP6_HLEN;
}

static int
nat64_ip6_option_frag(struct pbuf **q, struct ip6_hdr **iphdr, nat64_ip6_data_t *data)
{
  u16_t optlen;
  struct pbuf *p = *q;
  struct ip6_frag_hdr *frag_hdr = (struct ip6_frag_hdr *)p->payload;

  if (p->len < IP6_EXTENSION_HEADER_MIN_LEN) {
    LWIP_DEBUGF(NAT64_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                ("IP6_NEXTH_FRAGMENT:(pbuf len %"U16_F" is less than 2), IPv6 packet dropped.\n", p->len));
    IP6_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }

  /* Get next header type. */
  data->nexth = frag_hdr->_nexth;

  /* 8 : Fragment Header length. */
  optlen = 8;
  data->ip6_hdr_len += optlen;

  /* Make sure this header fits in current pbuf. */
  if (optlen > p->len) {
    LWIP_DEBUGF(NAT64_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                ("IPv6 opt header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 pac dropped.\n",
                 optlen, p->len));
    IP6_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }

  /* check payload length is multiple of 8 octets when mbit is set */
  if (IP6_FRAG_MBIT(frag_hdr) && (IP6H_PLEN(*iphdr) & 0x7)) {
    /* ipv6 payload length is not multiple of 8 octets */
    icmp6_param_problem(p, ICMP6_PP_FIELD, (u8_t *)(*iphdr) + (u32_t)(data->ip6_hdr_len - optlen));
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input: packet with invalid payload length dropped\n"));
    IP6_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }

  /* Offset == 0 and more_fragments == 0? */
  if ((frag_hdr->_fragment_offset &
       PP_HTONS(IP6_FRAG_OFFSET_MASK | IP6_FRAG_MORE_FLAG)) == 0) {
    /*
     * This is a 1-fragment packet, usually a packet that we have
     * already reassembled. Skip this header anc continue.
     */
    (void)pbuf_header(p, -(s16_t)(optlen));
  } else {
#if LWIP_IPV6_REASS
    /* reassemble the packet */
    p = ip6_reass(p);
    *q = p;
    /* packet not fully reassembled yet? */
    LWIP_ERROR("nat64_ip6_input:fragment is not fully reassembled yet\n", (p != NULL), return NAT64_RET_ASSEMBLE);
    nat64_ip6_assemble(p, iphdr, data);
#else /* LWIP_IPV6_REASS */
    /* free (drop) packet pbufs */
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input: packet with Fragment header dropped.\n"));
    IP6_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
#endif /* LWIP_IPV6_REASS */
  }
  return NAT64_RET_OK;
}

static int
nat64_ip6_option_normal(struct pbuf *p, nat64_ip6_data_t *data)
{
  u16_t optlen;
  /* Get next header type. */
  data->nexth = *((u8_t *)p->payload);

  if (p->len < IP6_EXTENSION_HEADER_MIN_LEN) {
    LWIP_DEBUGF(NAT64_DEBUG, ("pbuf (len %"U16_F") is less than 2.\n", p->len));
    IP_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }
  /* Get the header length. */
  if ((1 + *((u8_t *)p->payload + 1)) > (p->len / IP6_HDR_EXT_LEN_UNIT)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("pbuf option (len %"U8_F") is too big.\n", *((u8_t *)p->payload + 1)));
    IP_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }
  optlen = (u16_t)(IP6_HDR_EXT_LEN_UNIT * (1 + *((u8_t *)p->payload + 1)));
  /* Skip over this header. */
  if (optlen > p->len) {
    LWIP_DEBUGF(NAT64_DEBUG,
                ("IPv6 opt header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 pac dropped.\n",
                 optlen, p->len));
    IP_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }

  data->ip6_hdr_len += optlen;
  (void)pbuf_header(p, -((s16_t)(optlen)));
  return NAT64_RET_OK;
}

static int
nat64_ip6_option(struct pbuf **q, struct ip6_hdr **iphdr, nat64_ip6_data_t *data)
{
  int ret;
  u8_t flags;
  struct pbuf *p = *q;
  /*
   * convert the ipv6 header options into ipv4 options, the ip_pbuf_data_len should include the options.
   * now we just drop the options.
   */
  data->nexth = IP6H_NEXTH(*iphdr);
  data->ip4_hdr_len = IP_HLEN;
  data->ip6_hdr_len = IP6_HLEN;
  data->ip_pbuf_data_len = IP6H_PLEN(*iphdr);

  /* find the data in ip6 pkt */
  /* Move to payload. */
  (void)pbuf_header(p, -IP6_HLEN);

  flags = 0;
  /* Process option extension headers, if present. */
  while (data->nexth != IP6_NEXTH_NONE) {
    if ((data->nexth == IP6_NEXTH_TCP) || (data->nexth == IP6_NEXTH_UDP) ||
        (data->nexth == IP6_NEXTH_ICMP6) || (flags != 0)) {
      break;
    }

    /* maybe the pbuf assembled */
    if (p != *q) {
      p = *q;
    }

    switch (data->nexth) {
      case IP6_NEXTH_HOPBYHOP:
      case IP6_NEXTH_ENCAPS:
      case IP6_NEXTH_ROUTING:
      case IP6_NEXTH_DESTOPTS:
        ret = nat64_ip6_option_normal(p, data);
        if (ret != NAT64_RET_OK) {
          return ret;
        }
        break;

      case IP6_NEXTH_FRAGMENT:
        ret = nat64_ip6_option_frag(q, iphdr, data);
        if (ret != NAT64_RET_OK) {
          return ret;
        }
        break;
      default:
        flags = 1;
        break;
    }
  }
  return NAT64_RET_OK;
}

static int
nat64_ip6_send(struct pbuf *p, struct pbuf *new_buf,
               struct ip_hdr *ip4hdr, struct netif *outp, const ip4_addr_t *nhop)
{
  u16_t chksum = 0;
  struct ip_hdr *ip4hdr_payload = NULL;
  const ip4_addr_t *target = NULL;

  (void)ip4_iphdr_chksum(ip4hdr, outp, &chksum);

  IF__NETIF_CHECKSUM_ENABLED(outp, NETIF_CHECKSUM_GEN_IP) {
    IPH_CHKSUM_SET(ip4hdr, chksum);
  }
#if LWIP_CHECKSUM_CTRL_PER_NETIF
  else {
    IPH_CHKSUM_SET(ip4hdr, 0);
  }
#endif
  ip4hdr_payload = new_buf->payload;
  *ip4hdr_payload = *ip4hdr;

#if IP_FRAG
  if ((outp->mtu != 0) && (new_buf->tot_len > outp->mtu)) {
    (void)ip4_frag(new_buf, outp, (ip4_addr_t *)&ip4hdr->dest);
    (void)pbuf_free(p);
    (void)pbuf_free(new_buf);
    return NAT64_RET_OK;
  }
#endif /* IP_FRAG */

  if (nhop == NULL) {
    target = (ip4_addr_t *)&ip4hdr->dest;
  } else {
    target = nhop;
  }

  LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input ipv4 output\n"));
  outp->output(outp, new_buf, target);
  IP6_STATS_INC(ip6.natfw);
  IP6_STATS_INC(ip.xmit);
  (void)pbuf_free(p);
  (void)pbuf_free(new_buf);
  return NAT64_RET_OK;
}

static int
nat64_ip6_translate_udp(struct pbuf *p, const struct ip6_hdr *iphdr,
                        struct pbuf *new_buf, const struct ip_hdr *ip4hdr)
{
  struct udp_hdr *udphdr = (struct udp_hdr *)new_buf->payload;
#if LWIP_DNS64
  if (udphdr->dest == lwip_ntohs(NAT64_DNS_PORT)) {
    (void)pbuf_header(p, -(s16_t)(sizeof(struct udp_hdr)));
    (void)pbuf_header(new_buf, -(s16_t)(sizeof(struct udp_hdr)));
    if (nat64_dns64_6to4(new_buf) != ERR_OK) {
      IP_STATS_INC(ip6.natdrop);
#if LWIP_ICMP6
      LWIP_ERROR("send icmp\n", (IP6H_NEXTH(iphdr) == IP6_NEXTH_ICMP6),
                 icmp6_dest_unreach(p, ICMP6_DUR_PORT));
#endif /* LWIP_ICMP6 */
      (void)pbuf_free(p);
      (void)pbuf_free(new_buf);
      return NAT64_RET_ERR;
    }
    (void)pbuf_header(p, (s16_t)(sizeof(struct udp_hdr)));
    (void)pbuf_header(new_buf, (s16_t)(sizeof(struct udp_hdr)));
  }
#else
  (void)p;
  (void)iphdr;
#endif
  udphdr->chksum = 0;
  udphdr->chksum = inet_chksum_pseudo(new_buf, IP_PROTO_UDP, new_buf->tot_len,
                                      (ip4_addr_t *)&ip4hdr->src, (ip4_addr_t *)&ip4hdr->dest);
  return NAT64_RET_OK;
}

static void
nat64_ip6_translate_tcp(struct pbuf *new_buf, const struct ip_hdr *ip4hdr)
{
  struct tcp_hdr *tcphdr = (struct tcp_hdr *)new_buf->payload;
  tcphdr->chksum = 0;
  tcphdr->chksum = inet_chksum_pseudo(new_buf, IP_PROTO_TCP, new_buf->tot_len,
                                      (ip4_addr_t *)&ip4hdr->src, (ip4_addr_t *)&ip4hdr->dest);
}

static int
nat64_ip6_translate_icmp(struct pbuf *p, struct netif *outp, struct pbuf *new_buf)
{
  struct icmp_echo_hdr *icmp4hdr = (struct icmp_echo_hdr *)new_buf->payload;
  struct icmpv6_hdr *icmp6hdr = (struct icmpv6_hdr *)new_buf->payload;
  /* just handle the echo icmp */
  if (icmp6hdr->type == ICMP6_TYPE_EREP) {
    ICMPH_TYPE_SET(icmp4hdr, ICMP_ER);
  } else if (icmp6hdr->type == ICMP6_TYPE_EREQ) {
    ICMPH_TYPE_SET(icmp4hdr, ICMP_ECHO);
  } else {
    LWIP_DEBUGF(NAT64_DEBUG, ("icmp6 not handle\n"));
    (void)pbuf_free(new_buf);
    IP_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }

  icmp4hdr->chksum = 0;
#if CHECKSUM_GEN_ICMP
  IF__NETIF_CHECKSUM_ENABLED(outp, NETIF_CHECKSUM_GEN_ICMP) {
    icmp4hdr->chksum = inet_chksum(icmp4hdr, new_buf->len);
  }
#endif
  LWIP_UNUSED_ARG(outp);
  return NAT64_RET_OK;
}

static int
nat64_ip6_translate_default(struct pbuf *p, const struct ip6_hdr *iphdr, struct pbuf *new_buf)
{
#if LWIP_ICMP6
  /* send ICMP parameter problem unless it was a multicast or ICMPv6 */
  if ((!ip6_addr_ismulticast(ip6_current_dest_addr())) &&
      (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6)) {
    icmp6_dest_unreach(p, ICMP6_DUR_NO_ROUTE);
  }
#endif /* LWIP_ICMP6 */
  LWIP_DEBUGF(NAT64_DEBUG, ("pkt not handle\n"));
  (void)pbuf_free(new_buf);
  IP_STATS_INC(ip6.natdrop);
  (void)pbuf_free(p);
  return NAT64_RET_ERR;
}

static void
nat64_ip6_set_ip4hdr(const struct ip6_hdr *iphdr, const nat64_ip6_data_t *data,
                     struct ip_hdr *ip4hdr)
{
  /* ipv4 version and len */
  IPH_VHL_SET(ip4hdr, 4, data->ip4_hdr_len >> 2);

#if defined(LWIP_NAT64_PRIORITY_KEEP) && LWIP_NAT64_PRIORITY_KEEP
  IPH_TOS_SET(ip4hdr, IP6H_TC(iphdr));
#else
  IPH_TOS_SET(ip4hdr, 0);
#endif

  IPH_LEN_SET(ip4hdr, lwip_htons(data->ip4_hdr_len + data->ip_pbuf_data_len));
  IPH_ID_SET(ip4hdr, lwip_htons(ip4_get_ip_id()));
  IPH_OFFSET_SET(ip4hdr, 0);
  ip4_inc_ip_id();

  IPH_TTL_SET(ip4hdr, (IP6H_HOPLIM(iphdr) - 1));
}

static int
nat64_ip6_translate(struct pbuf *p, const struct ip6_hdr *iphdr, const nat64_ip6_data_t *data,
                    struct ip_hdr *ip4hdr, struct netif *outp)
{
  int ret;
  struct pbuf *new_buf = NULL;

  /* MAY here PBUF_LINK should be used */
  new_buf = pbuf_alloc(PBUF_IP, data->ip_pbuf_data_len, PBUF_RAM);
  if (new_buf == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("pbuf alloc fail:no MEMORY\r\n"));
    (void)pbuf_header(p, (s16_t)(data->ip6_hdr_len));
    return NAT64_RET_FAIL;
  }

  nat64_ip6_set_ip4hdr(iphdr, data, ip4hdr);

  if (pbuf_copy(new_buf, p) != ERR_OK) {
    (void)pbuf_free(p);
    (void)pbuf_free(new_buf);
    return NAT64_RET_ERR;
  }
  LWIP_DEBUGF(NAT64_DEBUG, ("%s:%d\n", __FUNCTION__, __LINE__));
  switch (data->nexth) {
#if LWIP_UDP
    case IP6_NEXTH_UDP:
      IPH_PROTO_SET(ip4hdr, IP_PROTO_UDP);
      ret = nat64_ip6_translate_udp(p, iphdr, new_buf, ip4hdr);
      if (ret != NAT64_RET_OK) {
        return NAT64_RET_ERR;
      }
      break;
#endif /* LWIP_UDP */
#if LWIP_TCP
    case IP6_NEXTH_TCP:
      IPH_PROTO_SET(ip4hdr, IP_PROTO_TCP);
      nat64_ip6_translate_tcp(new_buf, ip4hdr);
      break;
#endif /* LWIP_TCP */
#if LWIP_ICMP
    case IP6_NEXTH_ICMP6:
      IPH_PROTO_SET(ip4hdr, IP_PROTO_ICMP);
      ret = nat64_ip6_translate_icmp(p, outp, new_buf);
      if (ret != NAT64_RET_OK) {
        return NAT64_RET_ERR;
      }
      break;
#endif /* LWIP_ICMP */
    default:
      return nat64_ip6_translate_default(p, iphdr, new_buf);
  }

  (void)pbuf_header(new_buf, (s16_t)data->ip4_hdr_len);

  return nat64_ip6_send(p, new_buf, ip4hdr, outp, ip4_addr_isany(&data->nhop) ? NULL : &data->nhop);
}

static int
nat64_ip6_frag(struct pbuf *p, const struct netif *outp, nat64_ip6_data_t *data)
{
  data->ip_pbuf_data_len -= (data->ip6_hdr_len - IP6_HLEN);
#if !IP_FRAG
  /*
   * we can calculate the total len, cmp with the mtu.
   * now we donot support the ip fragment, so just drop it.
   */
  if ((outp->mtu != 0) && ((data->ip4_hdr_len + data->ip_pbuf_data_len) > outp->mtu)) {
    IP_STATS_INC(ip6.natdrop);
    (void)pbuf_free(p);
    return NAT64_RET_ERR;
  }
#else
  LWIP_UNUSED_ARG(p);
  LWIP_UNUSED_ARG(outp);
#endif
  return NAT64_RET_OK;
}

int
nat64_ip6_input(struct pbuf *p, struct ip6_hdr *iphdr, const struct netif *inp)
{
  int ret;
  struct netif *outp = NULL;
  nat64_ip6_data_t data;

  (void)memset_s(&data, sizeof(data), 0, sizeof(data));

  if (nat64_status_check() != NAT64_RET_OK) {
    return NAT64_PKT_NOT_HANDLED;
  }

  ret = nat64_ip6_packet_check(p, iphdr);
  if (ret == NAT64_RET_FAIL) {
    return NAT64_PKT_NOT_HANDLED;
  } else if (ret == NAT64_RET_ERR) {
    return NAT64_PKT_HANDLED;
  }

  ret = nat64_ip6_route(p, iphdr, inp, &data, &outp);
  if (ret == NAT64_RET_FAIL) {
    return NAT64_PKT_NOT_HANDLED;
  } else if (ret == NAT64_RET_ERR) {
    return NAT64_PKT_HANDLED;
  }

  ret = nat64_ip6_option(&p, &iphdr, &data);
  if (ret == NAT64_RET_ERR) {
    return NAT64_PKT_HANDLED;
  } else if (ret == NAT64_RET_ASSEMBLE) {
    return NAT64_PKT_HANDLED;
  }

  if ((data.nexth != IP6_NEXTH_TCP) && (data.nexth != IP6_NEXTH_UDP) &&
      (data.nexth != IP6_NEXTH_ICMP6)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64_ip6_input: nexth is not correct.\n"));
    (void)pbuf_free(p);
    return NAT64_PKT_HANDLED;
  }

  ret = nat64_ip6_frag(p, outp, &data);
  if (ret != NAT64_RET_OK) {
    return NAT64_PKT_HANDLED;
  }

  ret = nat64_ip6_translate(p, iphdr, &data, &data.ip4hdr, outp);
  if (ret == NAT64_RET_FAIL) {
    (void)pbuf_free(p);
    return NAT64_PKT_HANDLED;
  }

  return NAT64_PKT_HANDLED;
}

static void
nat64_dhcp_proxy_start(nat64_entry_t *entry)
{
  err_t ret;
  /* we need start the nat64 dynamicly */
  if (g_nat64_table == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64 stateful is not init\n"));
    return;
  }
  if ((entry->state == NAT64_STATE_DHCP_REQUEST) || (entry->nat64_sync == lwIP_TRUE)) {
    return;
  }
  ret = nat64_dhcp_request_ip(g_nat64_netif, &(entry->mac), &(entry->ip));
  if (ret != ERR_OK) {
    entry->state = NAT64_STATE_DHCP_FAIL;
    LWIP_DEBUGF(NAT64_DEBUG, ("start nat64 dhcp proxy fail, mac:\n"));
    return;
  }

  LWIP_DEBUGF(NAT64_DEBUG, ("start nat64 dhcp proxy.\n"));
  entry->state = NAT64_STATE_DHCP_REQUEST;
  return;
}

#define NAT64_DELAY_RELEASE_PERIOD 10 /* the max time for mg switch mbr to do the join succ */

static void
nat64_dhcp_proxy_stop(nat64_entry_t *entry)
{
  /* we need start the nat64 dynamicly */
  if ((g_nat64_table == NULL) || (entry == NULL)) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64 stateful is not init\n"));
    return;
  }
  switch (entry->state) {
    case NAT64_STATE_ESTABLISH:
#if LWIP_MMBR
      entry->state = NAT64_STATE_DELAY_RELEASE;
      entry->lifetime = NAT64_DELAY_RELEASE_PERIOD;
      break;
#endif
    case NAT64_STATE_CREATED:
    case NAT64_STATE_DHCP_REQUEST:
    case NAT64_STATE_DHCP_FAIL:
      entry->state = NAT64_STATE_DIRECT_RELEASE;
      entry->lifetime = 0;
      break;
    /*
     * this behind state should not handle
     * tmr handle : NAT64_STATE_DELAY_RELEASE / NAT64_STATE_DIRECT_RELEASE
     * dao & linkdao handle : NAT64_STATE_TIMEOUT_FOR_MBR
     */
    default:
      break;
  }
  return;
}

static void
handle_dhcp_and_nat64_timeout(nat64_entry_t *entry)
{
  err_t ret;
  /* we need start the nat64 dynamicly */
  if (g_nat64_table == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64 stateful is not init\n"));
    return;
  }

  switch (entry->state) {
    case NAT64_STATE_ESTABLISH:
#if LWIP_MMBR
      entry->state = NAT64_STATE_DELAY_RELEASE;
      entry->lifetime = NAT64_DELAY_RELEASE_PERIOD;
      break;
#endif
    case NAT64_STATE_CREATED:
    case NAT64_STATE_DHCP_REQUEST:
    case NAT64_STATE_DIRECT_RELEASE:
      if (entry->nat64_sync == lwIP_FALSE) {
        ret = nat64_dhcp_stop(g_nat64_netif, &(entry->mac), lwIP_FALSE);
        if (ret == ERR_OK) {
          LWIP_DEBUGF(NAT64_DEBUG, ("nat64 stop ok\n"));
        }
      }
      (void)nat64_entry_remove(entry, lwIP_TRUE);
      break;
#if LWIP_MMBR
    case NAT64_STATE_DELAY_RELEASE:
      LWIP_DEBUGF(NAT64_DEBUG, ("stop nat64 dhcp proxy.\n"));
      ret = nat64_dhcp_stop(g_nat64_netif, &(entry->mac), lwIP_FALSE);
      if (ret != ERR_OK) {
        LWIP_DEBUGF(NAT64_DEBUG, ("stop nat64 dhcp proxy fail, mac:\n"));
        return;
      }
      entry->lifetime = NAT64_WAIT_DHCP_RELEASE_PERIOD;
      entry->state = NAT64_STATE_DIRECT_RELEASE;
      break;
#endif
    default:
      LWIP_DEBUGF(NAT64_DEBUG, ("unexpected state = %d.\n", (int)(entry->state)));
      break;
  }
}

int
nat64_delete_entry_by_mnid(u8_t mnid)
{
  s16_t i;
  nat64_entry_t *entry = NULL;

  if (g_nat64_table == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64 is not init\n"));
    return ERR_OK;
  }

  if (mnid == 0) {
    return ERR_ARG;
  }

  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    if (g_nat64_table[i].mnid != mnid) {
      continue;
    }

    entry = &g_nat64_table[i];
    if (entry->nat64_sync == lwIP_FALSE) {
      nat64_dhcp_proxy_stop(entry);
    }
  }

  return ERR_OK;
}

int
nat64_delete_ipv4_addr(linklayer_addr_t *lladdr, u8_t mnid)
{
  s16_t i;
  nat64_entry_t *entry = NULL;

  if (g_nat64_table == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64 is not init\n"));
    return ERR_OK;
  }

  if (lladdr == NULL) {
    return ERR_ARG;
  }

  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    if (lwip_lladdr_cmp(&g_nat64_table[i].mac, lladdr, sizeof(linklayer_addr_t)) &&
        ((mnid == 0) || (g_nat64_table[i].mnid == 0) || (g_nat64_table[i].mnid == mnid))) {
      break;
    }
  }

  if (i == LWIP_NAT64_ENTRY_SIZE) {
    return ERR_OK;
  }

  entry = &g_nat64_table[i];
  nat64_dhcp_proxy_stop(entry);
  return ERR_OK;
}

static int
nat64_get_ipv4_addr(s16_t index, uint32_t *ipv4)
{
#if LWIP_NAT64_MIN_SUBSTITUTE
  ip4_addr_t ip;
  (void)memset_s(&ip, sizeof(ip4_addr_t), 0, sizeof(ip4_addr_t));
#endif
  if (g_nat64_table == NULL) {
    return NAT64_RET_ERR;
  }
  if ((g_nat64_table[index].state == NAT64_STATE_ESTABLISH) &&
#if !LWIP_NAT64_MIN_SUBSTITUTE
      (!ip4_addr_isany_val(g_nat64_table[index].ip))
#elif LWIP_DHCP_SUBSTITUTE
      (dhcp_substitute_idx_to_ip(g_nat64_netif, index + 1, &ip) == ERR_OK) &&
      (!ip4_addr_isany_val(ip))
#else
      (lwIP_FALSE)
#endif
     ) {
#if !LWIP_NAT64_MIN_SUBSTITUTE
    *ipv4 = g_nat64_table[index].ip.addr;
#elif LWIP_DHCP_SUBSTITUTE
    *ipv4 = ip.addr;
#else
    *ipv4 = 0;
#endif
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64: the ipv4 is ready. ip: %u\n", *ipv4));
    return NAT64_RET_OK;
  }
  LWIP_DEBUGF(NAT64_DEBUG, ("nat64: the ipv4 is not ready. ip is null\n"));
  return NAT64_RET_ERR;
}

#if LWIP_NA_PROXY_UNSOLICITED
static void
nat64_send_unsolicited_na(nat64_entry_t *entry)
{
  ip6_addr_t addr;
  int ret;
  u16_t msecs;

  msecs = ND6_RETRANS_TIMER;
  entry->na_to = (u8_t)((msecs + NAT64_TMR_INTERVAL - 1) / NAT64_TMR_INTERVAL);
  if (g_nat64_netif == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("uns na nul if\n"));
    return;
  }

  ret = nat64_entry_to6((const nat64_entry_t *)entry, &addr);
  if (ret != 0) {
    LWIP_DEBUGF(NAT64_DEBUG, ("uns na addr fail\n"));
    return;
  }

  nd6_send_na(g_nat64_netif, (const ip6_addr_t *)&addr, ND6_FLAG_OVERRIDE | ND6_SEND_FLAG_ALLNODES_DEST);
  if (entry->na_tries < ND6_MAX_NEIGHBOR_ADVERTISEMENT) {
    entry->na_tries++;
  }
}

void
nat64_proxy_unsolicited_na(const struct netif *netif)
{
  int i;
  nat64_entry_t *entry = NULL;

  if (g_nat64_table == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("proxy na: nat64 statful is not init\n"));
    return;
  }
  if (g_nat64_netif != netif) {
    LWIP_DEBUGF(NAT64_DEBUG, ("proxy na: no nat64 netif\n"));
    return;
  }
  if (!(netif_is_up(netif) && netif_is_link_up(netif))) {
    return;
  }

  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    entry = &g_nat64_table[i];
    if (entry->state == NAT64_STATE_INIT) {
      continue;
    }
    entry->na_tries = 0;
    nat64_send_unsolicited_na(entry);
  }

  return;
}
#endif /* LWIP_NA_PROXY_UNSOLICITED */

#if LWIP_MMBR && LWIP_NAT64_CHANGE_MSG
void
nat64_send_change_entry_msg(nat64_entry_t *entry, u8_t type)
{
  nat64_entry_t *entry_transmit = NULL;
  if (entry->nat64_sync == lwIP_TRUE) {
    return;
  }
  entry_transmit = (nat64_entry_t *)mem_malloc(sizeof(nat64_entry_t));
  if (entry_transmit == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("mem_malloc fail!\n"));
    return;
  }

  (void)memcpy_s(entry_transmit, sizeof(nat64_entry_t), entry, sizeof(nat64_entry_t));
  entry_transmit->mnid = rpl_get_mbr_mnid();
  entry_transmit->nat64_sync = lwIP_TRUE;

  rpl_event_indicate(type, 0, entry_transmit);
}
#endif /* LWIP_MMBR && LWIP_NAT64_CHANGE_MSG */

#if RPL_CONF_SWITCH_MBR_BY_AUTOLINK
static err_t
lwip_mmbr_autolink_event_do(struct tcpip_api_call_data *m)
{
  nat64_entry_t *entry = NULL;
  lwip_autolink_event_t *reply_ack_msg = NULL;
  err_t ret;
  struct netifapi_msg *msg = (struct netifapi_msg *)(void *)m;
  lwip_autolink_event_t *autolink_event = (lwip_autolink_event_t *)(msg->msg.arg_cb.arg);
  entry = nat64_entry_lookup_by_mac(&(autolink_event->mac));
  if (entry == NULL) {
    return ERR_ARG;
  }
  switch (autolink_event->type) {
    case MSG_NODE_CHANGE_MBR:
      reply_ack_msg = (lwip_autolink_event_t *)mem_malloc(sizeof(lwip_autolink_event_t));
      if (reply_ack_msg == NULL) {
        LWIP_DEBUGF(NAT64_DEBUG, ("mem_malloc fail!\n"));
        return ERR_MEM;
      }
      (void)memset_s(reply_ack_msg, 0, sizeof(lwip_autolink_event_t), 0);
      reply_ack_msg->type = MSG_NODE_CHANGE_MBR_ACK;
      reply_ack_msg->mac = autolink_event->mac;
      /* here we should send this ack info to the peer MBR */
      mesh_lwip_send_msg(autolink_event->local_ip, (u8_t *)reply_ack_msg, sizeof(lwip_autolink_event_t));
      mem_free(reply_ack_msg);
      /* when receive the MSG_NODE_CHANGE_MBR, we should stop dhcp and release nat64 anyway */
      if ((entry->nat64_sync == lwIP_FALSE) && ((entry->state == NAT64_STATE_DHCP_REQUEST) ||
          (entry->state == NAT64_STATE_ESTABLISH))) {
        ret = nat64_dhcp_stop(nat64_netif_get(), &(entry->mac), lwIP_FALSE);
        if (ret != ERR_OK) {
          LWIP_DEBUGF(NAT64_DEBUG, ("stop nat64 dhcp proxy fail, mac:\n"));
          return ERR_ARG;
        }
        entry->nat64_sync = lwIP_TRUE;
      }
      break;
    case MSG_NODE_CHANGE_MBR_ACK:
      entry->timeouts = 0;
      if ((entry->nat64_sync == lwIP_FALSE) && (entry->state == NAT64_STATE_TIMEOUT_FOR_MBR)) {
        nat64_dhcp_proxy_start(entry);
      }
      break;
    default:
      break;
  }
  return ERR_OK;
}

static void
lwip_mmbr_autolink_event(const u8_t *data, u32_t data_lenth)
{
  if ((data == NULL) || (data_lenth < sizeof(lwip_autolink_event_t))) {
    LWIP_DEBUGF(NAT64_DEBUG, ("invalid param!\n"));
  }
  lwip_autolink_event_t *autolink_event = (lwip_autolink_event_t *)data;
  LWIP_API_VAR_DECLARE(msg);
  LWIP_API_VAR_ALLOC(msg);
  LWIP_API_VAR_REF(msg).netif = NULL;
  LWIP_API_VAR_REF(msg).msg.arg_cb.argfunc = NULL;
  LWIP_API_VAR_REF(msg).msg.arg_cb.arg = (void *)autolink_event;
  (void)tcpip_linklayer_event_call(lwip_mmbr_autolink_event_do, &API_VAR_REF(msg).call);
  LWIP_API_VAR_FREE(msg);
}

static void
lwip_send_autolink_event_othermbr(nat64_entry_t *entry, u32_t peer_mbr_ipv4)
{
  struct netif *netif = nat64_netif_get();
  if (netif == NULL) {
    return;
  }
  lwip_autolink_event_t *autolink_event = (lwip_autolink_event_t *)mem_malloc(sizeof(lwip_autolink_event_t));
  if (autolink_event == NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("mem_malloc fail!\n"));
    return;
  }
  (void)memcpy_s(&(autolink_event->mac), sizeof(linklayer_addr_t), &(entry->mac), sizeof(linklayer_addr_t));
  (void)netif_get_addr(netif, (ip4_addr_t *)&autolink_event->local_ip, NULL, NULL);
  autolink_event->type = MSG_NODE_CHANGE_MBR;
  mesh_lwip_send_msg(peer_mbr_ipv4, (u8_t *)(autolink_event), sizeof(lwip_autolink_event_t));
  entry->state = NAT64_STATE_TIMEOUT_FOR_MBR;
  mem_free(autolink_event);
}

uint8_t
lwip_mmbr_autolink_event_init(void)
{
  mesh_set_lwip_msg_call_back(lwip_mmbr_autolink_event);
  return RPL_OK;
}

void
lwip_mmbr_autolink_event_deinit(void)
{
  mesh_set_lwip_msg_call_back(NULL);
  return;
}
#endif

static void
update_entry_from_dao(nat64_entry_t *nate, const nat64_ipv4_rqst_t *ipv4_rqst)
{
  /* should renew the daoSn when the ip is not ready */
  nate->dao_sn = ipv4_rqst->dao_sn;
  nate->mnid = ipv4_rqst->mnid;
  nate->orig_mnid = ipv4_rqst->mnid;
  nate->lifetime = ipv4_rqst->lifetime;
  nate->conn_time = ipv4_rqst->conn_time;
  nate->nat64_sync = lwIP_FALSE;
}

int
nat64_request_ipv4_addr(const nat64_ipv4_rqst_t *ipv4_rqst)
{
  s16_t i;
  u32_t pref_ipv4 = 0;
#if RPL_CONF_SWITCH_MBR_BY_AUTOLINK
  u32_t peer_mbr_ipv4 = 0;
#endif
  nat64_entry_t *entry = NULL;
#if LWIP_NAT64_MIN_SUBSTITUTE
  ip4_addr_t ip;
#endif

  if ((ipv4_rqst == NULL) || (ipv4_rqst->lladdr == NULL) || (ipv4_rqst->ipv4 == NULL) || (g_nat64_table == NULL)) {
    return NAT64_RET_FAIL;
  }

  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    if (lwip_lladdr_cmp(&g_nat64_table[i].mac, ipv4_rqst->lladdr, sizeof(linklayer_addr_t))) {
      break;
    }
  }

  /* find exist nat64 */
  if (i != LWIP_NAT64_ENTRY_SIZE) {
    switch (g_nat64_table[i].state) {
      case NAT64_STATE_ESTABLISH:
        if (g_nat64_table[i].nat64_sync == lwIP_FALSE) {
          if ((ipv4_rqst->mnid == g_nat64_table[i].mnid) ||
              (g_nat64_table[i].lifetime == NAT64_TMR_INFINITY)) {
#if !LWIP_NAT64_MIN_SUBSTITUTE
            /* the lifetime is NAT64_TMR_INFINITY can't send update event when sta behind mbr */
            if (!ip4_addr_isany_val(g_nat64_table[i].ip) && (g_nat64_table[i].lifetime != NAT64_TMR_INFINITY)) {
#endif
              update_entry_from_dao(&g_nat64_table[i], ipv4_rqst);
#if LWIP_MMBR && LWIP_NAT64_CHANGE_MSG
              nat64_send_change_entry_msg(&g_nat64_table[i], RPL_EVT_NODE_NAT64_UPDATE);
#endif /* LWIP_MMBR && LWIP_NAT64_CHANGE_MSG */
              return nat64_get_ipv4_addr(i, ipv4_rqst->ipv4);
            }
          }
          /* handle sta exchange diffrent mg in one mbr */
          if ((ipv4_rqst->mnid != g_nat64_table[i].mnid) && (g_nat64_table[i].mnid != 0) && (ipv4_rqst->mnid != 0)) {
#if !LWIP_NAT64_MIN_SUBSTITUTE
            if (!ip4_addr_isany_val(g_nat64_table[i].ip)) {
              /* remove old dao proxy in one mbr */
              if (ipv4_rqst->conn_time > g_nat64_table[i].conn_time) {
                return NAT64_RET_ERR;
              }
              update_entry_from_dao(&g_nat64_table[i], ipv4_rqst);
#if LWIP_MMBR && LWIP_NAT64_CHANGE_MSG
              nat64_send_change_entry_msg(&g_nat64_table[i], RPL_EVT_NODE_NAT64_ADD);
#endif /* LWIP_MMBR && LWIP_NAT64_CHANGE_MSG */
              return nat64_get_ipv4_addr(i, ipv4_rqst->ipv4);
            }
#endif
          }
        } else if (g_nat64_table[i].nat64_sync == lwIP_TRUE) {
          if (ipv4_rqst->conn_time > g_nat64_table[i].conn_time) {
            return NAT64_RET_ERR;
          }
#if !LWIP_NAT64_MIN_SUBSTITUTE
          if (!ip4_addr_isany_val(g_nat64_table[i].ip)) {
            pref_ipv4 = g_nat64_table[i].ip.addr;
          }
#endif

#if RPL_CONF_SWITCH_MBR_BY_AUTOLINK
            (void)mesh_get_mbr_ip(g_nat64_table[i].mnid, &peer_mbr_ipv4);
#endif
          /*
           * in sync state, dhcp is not exist, if we go to here, this could Frequent switching,
           * we just stop dhcp and remove sync nat64, then create nat64 entry and start dhcp.
           */
          (void)nat64_dhcp_stop(nat64_netif_get(), &(g_nat64_table[i].mac), lwIP_FALSE);
          update_entry_from_dao(&g_nat64_table[i], ipv4_rqst);
          if (pref_ipv4 != 0) {
#if RPL_CONF_SWITCH_MBR_BY_AUTOLINK
            lwip_send_autolink_event_othermbr(&g_nat64_table[i], peer_mbr_ipv4);
#endif
          }
          return NAT64_RET_OK;
        }
        break;
#if LWIP_MMBR
      case NAT64_STATE_DELAY_RELEASE:
        if (g_nat64_table[i].nat64_sync == lwIP_FALSE) {
          update_entry_from_dao(&g_nat64_table[i], ipv4_rqst);
          g_nat64_table[i].state = NAT64_STATE_ESTABLISH;
#if LWIP_NAT64_CHANGE_MSG
          if (!ip4_addr_isany_val(g_nat64_table[i].ip)) {
            nat64_send_change_entry_msg(&g_nat64_table[i], RPL_EVT_NODE_NAT64_ADD);
          }
#endif
          return nat64_get_ipv4_addr(i, ipv4_rqst->ipv4);
        } else {
          return NAT64_RET_ERR;
        }
#endif
      case NAT64_STATE_CREATED:
      case NAT64_STATE_DHCP_FAIL:
        if ((g_nat64_table[i].state == NAT64_STATE_CREATED) && (g_nat64_table[i].nat64_sync == lwIP_TRUE)) {
          /*
           * Normally we expect a synced nat64 entry not to have an active dhcp proxy.
           * Reaching here implies MG switching form an MBR to one another frequently,
           * so we restart the dhcp proxy to avoid inconsistency between dhcp and nat64 tables.
           */
          (void)nat64_dhcp_stop(nat64_netif_get(), &(g_nat64_table[i].mac), lwIP_FALSE);
        }
        if (g_nat64_table[i].nat64_sync == lwIP_FALSE || (g_nat64_table[i].state == NAT64_STATE_CREATED)) {
          update_entry_from_dao(&g_nat64_table[i], ipv4_rqst);
          nat64_dhcp_proxy_start(&g_nat64_table[i]);
        }
        return NAT64_RET_ERR;
      case NAT64_STATE_DHCP_REQUEST:
        return NAT64_RET_ERR;
      case NAT64_STATE_DIRECT_RELEASE:
        /*
         * when sta parent from mg switch to mbr, when connect to mbr,
         * this sta current nat64 state is NAT64_STATE_DIRECT_RELEASE,
         * we will delete its nat64 entry for timeout.
         * in this case, we should update dao and start dhcp.
         */
        if ((ipv4_rqst->lifetime == MAX_UINT32) && (g_nat64_table[i].nat64_sync == lwIP_FALSE)) {
          update_entry_from_dao(&g_nat64_table[i], ipv4_rqst);
          nat64_dhcp_proxy_start(&g_nat64_table[i]);
          return NAT64_RET_OK;
        }
        return NAT64_RET_ERR;
      default:
        return NAT64_RET_ERR;
    }
  }

  /* not find exist nat64 entry or mg change one mbr to other mbr */
  if (i == LWIP_NAT64_ENTRY_SIZE) {
    entry = nat64_entry_new(ipv4_rqst->lladdr, ipv4_rqst->dao_sn, ipv4_rqst->mnid, ipv4_rqst->lifetime, lwIP_FALSE,
                            ipv4_rqst->conn_time);
    if (entry != NULL) {
#if LWIP_NA_PROXY_UNSOLICITED
      nat64_send_unsolicited_na(entry);
#endif /* LWIP_NA_PROXY_UNSOLICITED */
      /* triger the dhcp to get ip */
      LWIP_DEBUGF(NAT64_DEBUG, ("nat64_dhcp_proxy_start\n"));
      nat64_dhcp_proxy_start(entry); /* NAT64_STATE_INIT change to NAT64_STATE_DHCP_REQUEST */
      return NAT64_RET_ERR;
    }
  }
  return NAT64_RET_FAIL;
}

#if LWIP_LOWPOWER
u32_t
nat64_tmr_tick(void)
{
  s16_t i;
  u32_t tick = 0;
  nat64_entry_t *entry = NULL;

  if (g_nat64_table == NULL) {
    LOWPOWER_DEBUG(("%s tmr tick: 0\n", __func__));
    return 0;
  }

  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    entry = &g_nat64_table[i];
    if (entry->state == NAT64_STATE_INIT) {
      continue;
    }
#if LWIP_NA_PROXY_UNSOLICITED
    SET_TMR_TICK(tick, entry->na_to);
#endif /* LWIP_NA_PROXY_UNSOLICITED */

    if (entry->lifetime == NAT64_TMR_INFINITY) {
      continue;
    }
    SET_TMR_TICK(tick, entry->lifetime);
  }

  LOWPOWER_DEBUG(("%s tmr tick: %u\n", __func__, tick));
  return tick;
}
#endif

#if LWIP_NA_PROXY_UNSOLICITED
static void
nat64_na_tmr(nat64_entry_t *entry)
{
  if (entry->na_to > 1) {
    entry->na_to--;
  } else if (entry->na_to == 1) {
    entry->na_to--;
    if (entry->na_tries < ND6_MAX_NEIGHBOR_ADVERTISEMENT) {
      nat64_send_unsolicited_na(entry);
    } else {
      entry->na_tries = 0;
    }
  } else {
    /* do nothing if na_to is 0 */
  }
}
#endif /* LWIP_NA_PROXY_UNSOLICITED */

void
nat64_tmr(void)
{
  int i;
  nat64_entry_t *entry = NULL;
  if (g_nat64_table == NULL) {
    return;
  }
  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    entry = &g_nat64_table[i];
    if (entry->state == NAT64_STATE_INIT) {
      continue;
    }
    /* the mbr should increase every sta/mg conn_time  */
    entry->conn_time++; /* the max time is 136 years */
#if RPL_CONF_SWITCH_MBR_BY_AUTOLINK
    if (entry->state == NAT64_STATE_TIMEOUT_FOR_MBR) {
      entry->timeouts++;
      if (entry->timeouts == NAT64_MBR_EXIT_WAIT_ACK_TIME) {
        nat64_dhcp_proxy_start(entry);
        entry->timeouts = 0;
      }
    }
#endif
#if LWIP_NA_PROXY_UNSOLICITED
    nat64_na_tmr(entry);
#endif /* LWIP_NA_PROXY_UNSOLICITED */

    if (entry->lifetime == NAT64_TMR_INFINITY) {
      /* when sta connect MBR, if start dhcp fail, we should start dhcp */
      if (entry->state == NAT64_STATE_DHCP_FAIL) {
        nat64_dhcp_proxy_start(entry);
      }
      continue;
    }

    if (entry->lifetime <= (NAT64_TMR_INTERVAL / MS_PER_SECOND)) {
      handle_dhcp_and_nat64_timeout(entry);
    } else {
      entry->lifetime -= (NAT64_TMR_INTERVAL / MS_PER_SECOND);
    }
  }
}

struct netif *
nat64_netif_get(void)
{
  return g_nat64_netif;
}

err_t
nat64_init(struct netif *ntf)
{
  if (g_nat64_table != NULL) {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64 already inited.\n"));
    return ERR_OK;
  }
  if (ntf != NULL) {
    g_nat64_netif = ntf;
    nat64_set_statful_enable();
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64 stateful..\n"));
  } else {
    LWIP_DEBUGF(NAT64_DEBUG, ("nat64 stateless..\n"));
  }
#if RPL_CONF_SWITCH_MBR_BY_AUTOLINK
  if (lwip_mmbr_autolink_event_init() != ERR_OK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("register mmbr autolink event fail!\n"));
  }
#endif
  return ERR_OK;
}

err_t
nat64_deinit(void)
{
  if (g_nat64_netif != NULL) {
    nat64_set_statful_disable();
    g_nat64_netif = NULL;
  }
#if RPL_CONF_SWITCH_MBR_BY_AUTOLINK
  lwip_mmbr_autolink_event_deinit();
#endif
  return ERR_OK;
}

err_t
nat64_deinit_netif(const struct netif *ntf)
{
  if ((ntf == NULL) || (g_nat64_table == NULL) || (ntf != g_nat64_netif)) {
    return ERR_ARG;
  }

  return nat64_deinit();
}

static err_t
nat64_init_with_name(struct netif *nif, void *data)
{
  nif = netif_find((const char *)data);
  if (nif == NULL) {
    return ERR_ARG;
  }

  return nat64_init(nif);
}

err_t
nat64_entry_remove_by_mnid(struct netif *nif, void *arg)
{
  int i;
  uint8_t *mnid = (uint8_t *)arg;
  if ((*mnid == 0) || (g_nat64_table == NULL)) {
    return ERR_VAL;
  }
  for (i = 0; i < LWIP_NAT64_ENTRY_SIZE; i++) {
    if ((g_nat64_table[i].mnid == *mnid) && (g_nat64_table[i].nat64_sync == lwIP_TRUE)) {
      /* delete all this mbr nat64 sync when mbr leave */
      (void)nat64_entry_remove(&g_nat64_table[i], lwIP_FALSE);
    }
  }
  LWIP_UNUSED_ARG(nif);
  return ERR_OK;
}

int
lwip_nat64_init(const char *name, uint8_t len)
{
  err_t ret;

  if ((name == NULL) || (len >= NETIF_NAMESIZE) || (len < NAT64_NETIF_NAME_LEN_MIN) ||
      (strncmp(name, NAT64_NETIF_NAME, NAT64_NETIF_NAME_LEN_MIN) != 0)) {
    return -1;
  }

  ret = netifapi_call_argcb(nat64_init_with_name, (void *)name);
  if (ret != ERR_OK) {
    return -1;
  }

  return 0;
}

static err_t
nat64_exit(struct netif *nif)
{
  LWIP_UNUSED_ARG(nif);
  return nat64_deinit();
}

int
lwip_nat64_deinit(void)
{
  err_t ret;
  ret = netifapi_netif_common(NULL, NULL, nat64_exit);
  if (ret != ERR_OK) {
    return -1;
  }

  return 0;
}
#endif
