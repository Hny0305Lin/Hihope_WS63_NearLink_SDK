/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: implementation for ip6 in ip4
 * Author: NA
 * Create: 2020
 */

#include "lwip/opt.h"

#if LWIP_IP6IN4
#include "lwip/ip6in4.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/icmp.h"
#include "lwip/stats.h"
#include "lwip/lwip_rpl.h"
#include "lwip/etharp.h"
#include "lwip/netifapi.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/inet_chksum.h"
#if LWIP_LOWPOWER
#include "lwip/lowpower.h"
#endif

#include "lwip/ip6in4_api.h"

#if (LWIP_IPV4 == 0) || (LWIP_IPV6 == 0) || (LWIP_RIPPLE == 0)
#error "it should enable the ipv4, ipv6 and ripple module before using ip6in4."
#endif

#define IP6IN4_PKT_HANDLED       (1)
#define IP6IN4_PKT_NOT_HANDLED   (0)

#define IP6IN4_RET_FAIL  (-1)
#define IP6IN4_RET_OK    (0)
#define IP6IN4_RET_ERR   (1)

#define IP6IN4_V6_VERSION 6
#define IP6IN4_NETIF_NAME_LEN_MIN 2

#define LWIP_IP6IN4_ENTRY_ADD_UPDATE 1
#define LWIP_IP6IN4_ENTRY_DELETE 2
#define LWIP_IP6IN4_ENTRY_SYNC 3

typedef struct {
  const char *name;
  uint8_t len;
  uint8_t mbr_cnt;
} ip6in4_init_arg;

typedef struct {
  const lwip_ip6in4_entry_t *entry;
  uint8_t cnt;
  uint8_t evt;
} ip6in4_entry_update_arg;

typedef struct {
  lwip_ip6in4_entry_t *entry;
  uint8_t cnt;
  uint8_t ret_cnt;
} ip6in4_entry_all_arg;

typedef struct {
  lwip_ip6in4_entry_t *entry;
  const char *mac;
  uint8_t len;
} ip6in4_entry_mac_arg;

static struct netif *g_ip6in4_nif;
static ip6in4_entry_t *g_ip6in4_entry;
static uint8_t g_ip6in4_entry_size;
static uint8_t g_ip6in4_entry_cnt;
static uint8_t g_ip6in4_inited;

static void
ip6in4_entry_on_remove(ip6in4_entry_t *entry)
{
  /* arp clear */
  (void)etharp_delete_arp_entry(g_ip6in4_nif, &entry->ip4);
}

static err_t
ip6in4_entry_remove(ip6in4_entry_t *entry)
{
  if ((entry == NULL) || (entry->in_use == lwIP_FALSE)) {
    return ERR_ARG;
  }

  /* here trigger the remove event */
  ip6in4_entry_on_remove(entry);

  if (g_ip6in4_entry_cnt > 0) {
    g_ip6in4_entry_cnt--;
  }

  (void)memset_s(entry, sizeof(ip6in4_entry_t), 0, sizeof(ip6in4_entry_t));
  return ERR_OK;
}

static void
ip6in4_entry_all_remove(void)
{
  uint8_t i;
  ip6in4_entry_t *entry = NULL;

  if (g_ip6in4_entry == NULL) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4 is not init\n"));
    return;
  }

  for (i = 0; i < g_ip6in4_entry_size; i++) {
    entry = &g_ip6in4_entry[i];
    if (entry->in_use != lwIP_TRUE) {
      continue;
    }

    (void)ip6in4_entry_remove(entry);
  }
}

static ip6in4_entry_t *
ip6in4_entry_new(void)
{
  uint32_t i;
  ip6in4_entry_t *entry = NULL;

  if (g_ip6in4_entry == NULL) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4 is not init\n"));
    return NULL;
  }

  for (i = 0; i < g_ip6in4_entry_size; i++) {
    entry = &g_ip6in4_entry[i];
    if (entry->in_use == lwIP_TRUE) {
      continue;
    }

    if (g_ip6in4_entry_cnt < g_ip6in4_entry_size) {
      g_ip6in4_entry_cnt++;
    }

    entry->in_use = lwIP_TRUE;
    return entry;
  }

  return NULL;
}

static err_t
ip6in4_entry_update(ip6in4_entry_t *entry, ip6_addr_t *ip6addr, ip4_addr_t *ip4addr, uint32_t lifetime)
{
  if ((entry == NULL) || (entry->in_use == lwIP_FALSE)) {
    return ERR_ARG;
  }

  if (ip6addr != NULL) {
    entry->ip6 = *ip6addr;
  }

  if (ip4addr != NULL) {
    entry->ip4 = *ip4addr;
  }

  if (lifetime > 0) {
    entry->lifetime = lifetime;
  }

  return ERR_OK;
}

err_t
ip6in4_entry_add(ip6_addr_t *ip6addr, ip4_addr_t *ip4addr, uint32_t lifetime)
{
  err_t ret;
  ip6in4_entry_t *entry = NULL;

  if ((ip6addr == NULL) || (ip4addr == NULL) || (lifetime == 0)) {
    return ERR_ARG;
  }

  entry = ip6in4_entry_new();
  if (entry == NULL) {
    return ERR_VAL;
  }

  ret = ip6in4_entry_update(entry, ip6addr, ip4addr, lifetime);
  if (ret != ERR_OK) {
    (void)ip6in4_entry_remove(entry);
    return ERR_VAL;
  }

  return ERR_OK;
}

static ip6in4_entry_t *
ip6in4_entry_find_by_ip6addr(const ip6_addr_t *ip6addr)
{
  uint32_t i;
  ip6in4_entry_t *entry = NULL;

  if (ip6addr == NULL) {
    return NULL;
  }

  if (g_ip6in4_entry == NULL) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4 is not init\n"));
    return NULL;
  }

  for (i = 0; i < g_ip6in4_entry_size; i++) {
    entry = &g_ip6in4_entry[i];
    if (entry->in_use != lwIP_TRUE) {
      continue;
    }

    if (memcmp(ip6addr, &entry->ip6, sizeof(ip6_addr_t)) == 0) {
      return entry;
    }
  }

  return NULL;
}

static ip6in4_entry_t *
ip6in4_entry_find_by_ip4addr(const ip4_addr_t *ip4addr)
{
  uint32_t i;
  ip6in4_entry_t *entry = NULL;

  if (ip4addr == NULL) {
    return NULL;
  }

  if (g_ip6in4_entry == NULL) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4 is not init\n"));
    return NULL;
  }

  for (i = 0; i < g_ip6in4_entry_size; i++) {
    entry = &g_ip6in4_entry[i];
    if (entry->in_use != lwIP_TRUE) {
      continue;
    }

    if (memcmp(ip4addr, &entry->ip4, sizeof(ip4_addr_t)) == 0) {
      return entry;
    }
  }

  return NULL;
}

static int
ip6in4_ip4_output(struct pbuf *p, struct ip6_hdr *ip6hdr, const ip6in4_entry_t *entry)
{
  err_t ret;

  p->ip6in4_ip6 = lwIP_TRUE;
  LWIP_UNUSED_ARG(ip6hdr);
  ret = ip4_output_if(p, NULL, &entry->ip4, IP_DEFAULT_TTL, 0, IP_PROTO_IP6IN4, g_ip6in4_nif);
  if (ret != ERR_OK) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4_ip4_output: ip6in4 out fail\n"));
  }
  (void)pbuf_free(p);
  return IP6IN4_PKT_HANDLED;
}

static int
ip6in4_status_check()
{
  if (g_ip6in4_inited == lwIP_FALSE) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4_status_check: ip6in4 is not init\n"));
    return IP6IN4_RET_FAIL;
  }
  /* the node must be MBR */
  if (lwip_rpl_is_br() == lwIP_FALSE) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4_status_check: not mbr node\n"));
    return IP6IN4_RET_FAIL;
  }
  return IP6IN4_RET_OK;
}

static int
ip6in4_ip6_packet_check(struct pbuf *p, const struct ip6_hdr *iphdr)
{
  /* do not handle the linklocal/loopback/multicast packet */
  if (ip6_addr_islinklocal(ip6_current_dest_addr()) ||
      ip6_addr_isloopback(ip6_current_dest_addr()) ||
      ip6_addr_ismulticast(ip6_current_dest_addr())) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4_ip6_input:pkt not handle\n"));
    return IP6IN4_RET_FAIL;
  }

  /* send ICMP6 if HL <= 1 */
  if (IP6H_HOPLIM(iphdr) <= 1) {
#if LWIP_ICMP6
    /* Don't send ICMP messages in response to ICMP messages */
    if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {
      icmp6_time_exceeded(p, ICMP6_TE_HL);
    }
#endif /* LWIP_ICMP6 */
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4_ip6_input:hop limit drop\n"));
    IP6_STATS_INC(ip6.drop);
    (void)pbuf_free(p);
    return IP6IN4_RET_ERR;
  }

  return IP6IN4_RET_OK;
}

int
ip6in4_ip6_input(struct pbuf *p, struct ip6_hdr *iphdr, const struct netif *inp, const ip6_addr_t *tunnel_end)
{
  int ret;
  uint8_t rc;
  ip6in4_entry_t *entry = NULL;
  ip6_addr_t nhop = {0};
  ip6_addr_t dest_addr;

  if ((p == NULL) || (iphdr == NULL) || (inp == NULL)) {
    return IP6IN4_PKT_NOT_HANDLED;
  }

  ret = ip6in4_status_check();
  if (ret != IP6IN4_RET_OK) {
    return IP6IN4_PKT_NOT_HANDLED;
  }

  ret = ip6in4_ip6_packet_check(p, iphdr);
  if (ret == IP6IN4_RET_FAIL) {
    return IP6IN4_PKT_NOT_HANDLED;
  } else if (ret == IP6IN4_RET_ERR) {
    return IP6IN4_PKT_HANDLED;
  }

  rc = lwip_rpl_is_rpl_netif(inp);
  if (rc != lwIP_TRUE) {
    return IP6IN4_PKT_NOT_HANDLED;
  }

  /* now it is allowed to set the tunnel endpoint info outside of ip6in4. */
  if (tunnel_end == NULL) {
    ip6_addr_copy_from_packed(dest_addr, iphdr->dest);
    ret = lwip_rpl_route_entry_nhop_get(&dest_addr, &nhop);
    if (ret != ERR_OK) {
      return IP6IN4_PKT_NOT_HANDLED;
    }
    tunnel_end = &nhop;
  }

  entry = ip6in4_entry_find_by_ip6addr(tunnel_end);
  if (entry == NULL) {
    return IP6IN4_PKT_NOT_HANDLED;
  }

  /* send the whole packet, using the ip4 */
  return ip6in4_ip4_output(p, iphdr, entry);
}

void
ip6in4_ip4_input(struct pbuf *p, struct netif *inp, const ip4_addr_t *src, const ip4_addr_t *dst)
{
  int ret;
  ip6in4_entry_t *entry = NULL;
  struct ip6_hdr *ip6hdr = NULL;

  if ((p == NULL) || (inp == NULL) || (src == NULL) || (dst == NULL)) {
    if (p != NULL) {
      (void)pbuf_free(p);
    }
    return;
  }

  if (ip4_addr_ismulticast(dst) || ip4_addr_isbroadcast(dst, inp) ||
      (inp != g_ip6in4_nif)) {
    (void)pbuf_free(p);
    return;
  }

  ret = ip6in4_status_check();
  if (ret != IP6IN4_RET_OK) {
    (void)pbuf_free(p);
    return;
  }

  entry = ip6in4_entry_find_by_ip4addr(src);
  if (entry == NULL) {
    (void)pbuf_free(p);
    return;
  }

  if (p->len < IP6_HLEN) {
    (void)pbuf_free(p);
    return;
  }

  ip6hdr = (struct ip6_hdr *)p->payload;
  if (IP6H_V(ip6hdr) != IP6IN4_V6_VERSION) {
    (void)pbuf_free(p);
    return;
  }

  /* it should check the route entry, when receive the ip6-in-ip4 packet.
   * the ip6 may send the icmpv6 packet to the dest. */
#if defined(LWIP_IP6IN4_CHECK_IP6ADDR)
  if (lwip_rpl_route_entry_lookup((ip6_addr_t *)&ip6hdr->dest) == NULL) {
    (void)pbuf_free(p);
    return;
  }
#endif

  /* here just use ip6_input, if it need to handle the error message,
   * the input function should be restructed, but it will take too much text size. */
  (void)ip6_input(p, inp);
}

err_t
ip6in4_entry_ip4_get(const ip6_addr_t *ip6, ip4_addr_t *ip4)
{
  int ret;
  ip6in4_entry_t *entry = NULL;
  if ((ip6 == NULL) || (ip4 == NULL)) {
    return ERR_ARG;
  }

  ret = ip6in4_status_check();
  if (ret != IP6IN4_RET_OK) {
    return ERR_VAL;
  }

  entry = ip6in4_entry_find_by_ip6addr(ip6);
  if (entry == NULL) {
    return ERR_VAL;
  }

  *ip4 = entry->ip4;
  return ERR_OK;
}

#if LWIP_LOWPOWER
u32_t
ip6in4_tmr_tick(void)
{
  s16_t i;
  u32_t tick = 0;
  ip6in4_entry_t *entry = NULL;

  if ((g_ip6in4_inited != lwIP_TRUE) || (g_ip6in4_entry == NULL)) {
    LOWPOWER_DEBUG(("%s tmr tick: 0\n", __func__));
    return 0;
  }

  for (i = 0; i < g_ip6in4_entry_size; i++) {
    entry = &g_ip6in4_entry[i];
    if (entry->in_use == lwIP_FALSE) {
      continue;
    }

    SET_TMR_TICK(tick, entry->lifetime);
  }

  LOWPOWER_DEBUG(("%s tmr tick: %u\n", __func__, tick));
  return tick;
}
#endif

void
ip6in4_tmr(void)
{
  uint32_t i;
  ip6in4_entry_t *entry = NULL;

  if ((g_ip6in4_inited != lwIP_TRUE) || (g_ip6in4_entry == NULL)) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4 tmr: ip6in4 is not init\n"));
    return;
  }

  for (i = 0; i < g_ip6in4_entry_size; i++) {
    entry = &g_ip6in4_entry[i];
    if ((entry->in_use == lwIP_FALSE) || (entry->lifetime == LWIP_IP6IN4_LIFETIME_INFINITE)) {
      continue;
    }

    if (entry->lifetime <= (IP6IN4_TMR_INTERVAL / MS_PER_SECOND)) {
      /* entry timeout, remove it. */
      (void)ip6in4_entry_remove(entry);
    } else {
      entry->lifetime -= (IP6IN4_TMR_INTERVAL / MS_PER_SECOND);
    }
  }
}

static err_t
ip6in4_entry_get(struct netif *nif, void *data)
{
  uint8_t i;
  int rc;
  const ip6_addr_t *ip6 = NULL;
  lwip_ip6in4_entry_t *entry = (lwip_ip6in4_entry_t *)data;

  if ((g_ip6in4_inited != lwIP_TRUE) || (g_ip6in4_entry == NULL) ||
      (g_ip6in4_nif == NULL)) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4 is not init\n"));
    return ERR_VAL;
  }

  if (!IP_IS_V4_VAL(g_ip6in4_nif->ip_addr) || (ip4_addr_isany(ip_2_ip4(&g_ip6in4_nif->ip_addr)) ||
        (ip4_addr_isnone(ip_2_ip4(&g_ip6in4_nif->ip_addr))))) {
    return ERR_VAL;
  }

  entry->ip4 = ip_addr_get_ip4_u32(&g_ip6in4_nif->ip_addr);

  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    ip6 = netif_ip6_addr(g_ip6in4_nif, i);
    if (!ip6_addr_islinklocal(ip6)) {
      continue;
    }

    rc = memcpy_s(entry->ip6, sizeof(entry->ip6), ip6->addr, sizeof(ip6->addr));
    if (rc != EOK) {
      return ERR_VAL;
    }

    break;
  }

  if (i == LWIP_IPV6_NUM_ADDRESSES) {
    return ERR_VAL;
  }

#ifndef LWIP_IP6IN4_LIFETIME_TIMER
  entry->lifetime = LWIP_IP6IN4_LIFETIME_INFINITE;
#else
  entry->lifetime = LWIP_IP6IN4_LIFETIME_DEFAULT;
#endif

  LWIP_UNUSED_ARG(nif);
  return ERR_OK;
}

int
lwip_ip6in4_entry_get(lwip_ip6in4_entry_t *entry)
{
  err_t ret;

  if (entry == NULL) {
    return -1;
  }

  ret = netifapi_call_argcb(ip6in4_entry_get, entry);
  if (ret != ERR_OK) {
    return -1;
  }

  return 0;
}

static err_t
ip6in4_entry_copy(const ip6in4_entry_t *ent, lwip_ip6in4_entry_t *entry)
{
  int ret;

  ret = memcpy_s(entry->ip6, sizeof(entry->ip6), ent->ip6.addr, sizeof(ent->ip6.addr));
  if (ret != EOK) {
    return ERR_VAL;
  }

  entry->ip4 = ent->ip4.addr;
  entry->lifetime = ent->lifetime;
  return ERR_OK;
}

static err_t
ip6in4_entry_all_get(struct netif *nif, void *data)
{
  err_t ret;
  uint8_t i;
  uint8_t j;
  uint8_t cnt = 0;
  lwip_ip6in4_entry_t *entry = NULL;
  ip6in4_entry_t *ent = NULL;
  ip6in4_entry_all_arg *arg = (ip6in4_entry_all_arg *)data;

  if ((g_ip6in4_inited != lwIP_TRUE) || (g_ip6in4_entry == NULL) ||
      (g_ip6in4_nif == NULL)) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4 is not init\n"));
    return ERR_VAL;
  }
  /* the first entry is the local config */
  ret = ip6in4_entry_get(NULL, &arg->entry[0]);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }

  j = 0;
  cnt++;
  for (i = 1; i < arg->cnt; i++) {
    entry = &arg->entry[i];
    while (j < g_ip6in4_entry_size) {
      if (g_ip6in4_entry[j].in_use == lwIP_TRUE) {
        break;
      }
      j++;
    }

    if (j == g_ip6in4_entry_size) {
      break;
    }

    ent = &g_ip6in4_entry[j];
    ret = ip6in4_entry_copy(ent, entry);
    if (ret != ERR_OK) {
      return ERR_VAL;
    }
    cnt++;
    j++;
  }
  LWIP_UNUSED_ARG(nif);
  arg->ret_cnt = cnt;
  return ERR_OK;
}

int
lwip_ip6in4_entry_all_get(lwip_ip6in4_entry_t *entry, uint8_t cnt)
{
  err_t ret;
  ip6in4_entry_all_arg arg = {0};

  if ((entry == NULL) || (cnt == 0)) {
    return -1;
  }

  arg.entry = entry;
  arg.cnt = cnt;

  ret = netifapi_call_argcb(ip6in4_entry_all_get, &arg);
  if (ret != ERR_OK) {
    return -1;
  }

  return arg.ret_cnt;
}

static err_t
ip6in4_entry_get_by_mac(struct netif *nif, void *data)
{
  int rc;
  err_t ret;
  ip6in4_entry_t *ent = NULL;
  ip6_addr_t ip6 = {0};
  linklayer_addr_t mac = {0};
  ip6in4_entry_mac_arg *arg = (ip6in4_entry_mac_arg *)data;

  if (g_ip6in4_inited != lwIP_TRUE) {
    return ERR_VAL;
  }

  rc = memcpy_s(mac.addr, sizeof(mac.addr), arg->mac, arg->len);
  if (rc != EOK) {
    return ERR_VAL;
  }
  mac.addrlen = arg->len;

  ret = netif_create_ip6_linklocal_address_from_mac(&mac, &ip6);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }

  ent = ip6in4_entry_find_by_ip6addr(&ip6);
  if (ent == NULL) {
    return ERR_VAL;
  }

  ret = ip6in4_entry_copy(ent, arg->entry);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }

  LWIP_UNUSED_ARG(nif);
  return ERR_OK;
}

int
lwip_ip6in4_entry_get_by_mac(const char *mac, uint8_t len, lwip_ip6in4_entry_t *entry)
{
  err_t ret;
  ip6in4_entry_mac_arg arg = {0};

  if ((entry == NULL) || (mac == NULL) || (len != LWIP_IP6IN4_MAC_LEN)) {
    return -1;
  }

  arg.entry = entry;
  arg.mac = mac;
  arg.len = len;

  ret = netifapi_call_argcb(ip6in4_entry_get_by_mac, &arg);
  if (ret != ERR_OK) {
    return -1;
  }

  return 0;
}

ip6in4_entry_t *
ip6in4_entry_get_next_inuse(uint32_t *state)
{
  uint32_t i;
  ip6in4_entry_t *entry = NULL;

  if (state == NULL) {
    return NULL;
  }

  if ((g_ip6in4_inited != lwIP_TRUE) || (g_ip6in4_entry == NULL) ||
      (g_ip6in4_nif == NULL)) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4 is not init\n"));
    return NULL;
  }

  for (i = *state; i < g_ip6in4_entry_size; i++) {
    entry = &g_ip6in4_entry[i];
    if (entry->in_use != lwIP_TRUE) {
      continue;
    }

    *state = i + 1;
    return entry;
  }

  return NULL;
}

static err_t
ip6in4_entry_sync(const lwip_ip6in4_entry_t *entry, uint8_t cnt)
{
  uint8_t i;
  int rc;
  err_t ret;
  ip6_addr_t ip6;
  ip4_addr_t ip4;
  const ip6_addr_t *linklocal = NULL;
  const lwip_ip6in4_entry_t *ent = NULL;

  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    linklocal = netif_ip6_addr(g_ip6in4_nif, i);
    if (ip6_addr_islinklocal(linklocal)) {
      break;
    }
  }

  if (i == LWIP_IPV6_NUM_ADDRESSES) {
    return ERR_VAL;
  }

  ip6in4_entry_all_remove();

  for (i = 0; i < cnt; i++) {
    ent = &entry[i];
    /* filter the local entry */
    if (memcmp(ent->ip6, linklocal->addr, sizeof(linklocal->addr)) == 0) {
      continue;
    }

    (void)memset_s(&ip6, sizeof(ip6), 0, sizeof(ip6));
    rc = memcpy_s(ip6.addr, sizeof(ip6.addr), ent->ip6, sizeof(ent->ip6));
    if (rc != EOK) {
      return ERR_VAL;
    }

    ip4.addr = ent->ip4;
    ret = ip6in4_entry_add(&ip6, &ip4, ent->lifetime);
    if (ret != ERR_OK) {
      return ERR_VAL;
    }
  }

  return ERR_OK;
}

static err_t
ip6in4_entry_add_update(const lwip_ip6in4_entry_t *entry, uint8_t cnt)
{
  uint8_t i;
  int rc;
  err_t ret;
  ip6_addr_t ip6;
  ip4_addr_t ip4;
  ip6in4_entry_t *entry64 = NULL;
  const lwip_ip6in4_entry_t *ent = NULL;
  const ip6_addr_t *linklocal = NULL;

  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    linklocal = netif_ip6_addr(g_ip6in4_nif, i);
    if (ip6_addr_islinklocal(linklocal)) {
      break;
    }
  }

  if (i == LWIP_IPV6_NUM_ADDRESSES) {
    return ERR_VAL;
  }

  for (i = 0; i < cnt; i++) {
    ent = &entry[i];
    /* filter the local entry */
    if (memcmp(ent->ip6, linklocal->addr, sizeof(linklocal->addr)) == 0) {
      continue;
    }

    (void)memset_s(&ip6, sizeof(ip6), 0, sizeof(ip6));
    rc = memcpy_s(ip6.addr, sizeof(ip6.addr), ent->ip6, sizeof(ent->ip6));
    if (rc != EOK) {
      return ERR_VAL;
    }

    ip4.addr = ent->ip4;
    entry64 = ip6in4_entry_find_by_ip6addr(&ip6);
    if (entry64 == NULL) {
      ret = ip6in4_entry_add(&ip6, &ip4, ent->lifetime);
      if (ret != ERR_OK) {
        return ERR_VAL;
      }
      continue;
    }

    ret = ip6in4_entry_update(entry64, NULL, &ip4, ent->lifetime);
    if (ret != ERR_OK) {
      return ERR_VAL;
    }
  }

  return ERR_OK;
}

static err_t
ip6in4_entry_delete(const lwip_ip6in4_entry_t *entry, uint8_t cnt)
{
  uint8_t i;
  int rc;
  err_t ret;
  ip6_addr_t ip6;
  ip6in4_entry_t *entry64 = NULL;
  const lwip_ip6in4_entry_t *ent = NULL;

  for (i = 0; i < cnt; i++) {
    ent = &entry[i];
    (void)memset_s(&ip6, sizeof(ip6), 0, sizeof(ip6));
    rc = memcpy_s(ip6.addr, sizeof(ip6.addr), ent->ip6, sizeof(ent->ip6));
    if (rc != EOK) {
      return ERR_VAL;
    }

    entry64 = ip6in4_entry_find_by_ip6addr(&ip6);
    if (entry64 == NULL) {
      continue;
    }

    ret = ip6in4_entry_remove(entry64);
    if (ret != ERR_OK) {
      return ERR_VAL;
    }
  }

  return ERR_OK;
}

static err_t
ip6in4_entry_set(struct netif *nif, void *data)
{
  ip6in4_entry_update_arg *arg = (ip6in4_entry_update_arg *)data;

  if ((g_ip6in4_inited != lwIP_TRUE) || (g_ip6in4_entry == NULL) ||
      (g_ip6in4_nif == NULL)) {
    LWIP_DEBUGF(IP6IN4_DEBUG, ("ip6in4 is not init\n"));
    return ERR_VAL;
  }

  if (arg->evt == LWIP_IP6IN4_ENTRY_SYNC) {
    return ip6in4_entry_sync(arg->entry, arg->cnt);
  }

  if (arg->evt == LWIP_IP6IN4_ENTRY_ADD_UPDATE) {
    return ip6in4_entry_add_update(arg->entry, arg->cnt);
  }

  if (arg->evt == LWIP_IP6IN4_ENTRY_DELETE) {
    return ip6in4_entry_delete(arg->entry, arg->cnt);
  }
  LWIP_UNUSED_ARG(nif);
  return ERR_ARG;
}

static int
lwip_ip6in4_entry_set(const lwip_ip6in4_entry_t *entry, uint8_t cnt, uint8_t evt)
{
  err_t ret;
  ip6in4_entry_update_arg arg;

  if ((entry == NULL) || (cnt == 0)) {
    return -1;
  }

  (void)memset_s(&arg, sizeof(arg), 0, sizeof(arg));
  arg.entry = entry;
  arg.cnt = cnt;
  arg.evt = evt;

  ret = netifapi_call_argcb(ip6in4_entry_set, &arg);
  if (ret != ERR_OK) {
    return -1;
  }

  return 0;
}

int
lwip_ip6in4_entry_add_update(const lwip_ip6in4_entry_t *entry, uint8_t cnt)
{
  return lwip_ip6in4_entry_set(entry, cnt, LWIP_IP6IN4_ENTRY_ADD_UPDATE);
}

int
lwip_ip6in4_entry_delete(const lwip_ip6in4_entry_t *entry, uint8_t cnt)
{
  return lwip_ip6in4_entry_set(entry, cnt, LWIP_IP6IN4_ENTRY_DELETE);
}

int
lwip_ip6in4_entry_sync(const lwip_ip6in4_entry_t *entry, uint8_t cnt)
{
  return lwip_ip6in4_entry_set(entry, cnt, LWIP_IP6IN4_ENTRY_SYNC);
}

err_t
ip6in4_init(struct netif *inp, uint8_t mbr_cnt)
{
  size_t entry_size;

  if ((inp == NULL) || (mbr_cnt == 0)) {
    return ERR_ARG;
  }

  if (g_ip6in4_inited == lwIP_TRUE) {
    return ERR_OK;
  }

  entry_size = mbr_cnt * sizeof(ip6in4_entry_t);
  g_ip6in4_entry = (ip6in4_entry_t *)mem_malloc(entry_size);
  if (g_ip6in4_entry == NULL) {
    return ERR_MEM;
  }
  g_ip6in4_entry_size = mbr_cnt;
  g_ip6in4_entry_cnt = 0;
  (void)memset_s(g_ip6in4_entry, entry_size, 0, entry_size);
  g_ip6in4_nif = inp;
  g_ip6in4_inited = lwIP_TRUE;
  return ERR_OK;
}

err_t
ip6in4_deinit(void)
{
  if (g_ip6in4_inited == lwIP_FALSE) {
    return ERR_OK;
  }

  if (g_ip6in4_entry != NULL) {
    mem_free(g_ip6in4_entry);
    g_ip6in4_entry_cnt = 0;
    g_ip6in4_entry = NULL;
  }
  g_ip6in4_entry_size = 0;
  g_ip6in4_nif = NULL;
  g_ip6in4_inited = lwIP_FALSE;
  return ERR_OK;
}

err_t
ip6in4_deinit_netif(const struct netif *ntf)
{
  if ((ntf == NULL) || (g_ip6in4_inited == lwIP_FALSE) || (ntf != g_ip6in4_nif)) {
    return ERR_ARG;
  }

  return ip6in4_deinit();
}

static err_t
ip6in4_init_with_name(struct netif *nif, void *data)
{
  ip6in4_init_arg *arg = (ip6in4_init_arg *)data;
  nif = netif_find(arg->name);
  if (nif == NULL) {
    return ERR_ARG;
  }

  return ip6in4_init(nif, arg->mbr_cnt);
}

int
lwip_ip6in4_init(const char *name, uint8_t len, uint8_t mbr_cnt)
{
  err_t ret;
  ip6in4_init_arg arg = {0};

  if ((name == NULL) || (len >= NETIF_NAMESIZE) || (len < IP6IN4_NETIF_NAME_LEN_MIN) || (mbr_cnt == 0)) {
    return -1;
  }

  arg.name = name;
  arg.len = len;
  arg.mbr_cnt = mbr_cnt;

  ret = netifapi_call_argcb(ip6in4_init_with_name, &arg);
  if (ret != ERR_OK) {
    return -1;
  }

  return 0;
}

static err_t
ip6in4_exit(struct netif *nif)
{
  LWIP_UNUSED_ARG(nif);
  return ip6in4_deinit();
}

int
lwip_ip6in4_deinit(void)
{
  err_t ret;
  ret = netifapi_netif_common(NULL, NULL, ip6in4_exit);
  if (ret != ERR_OK) {
    return -1;
  }

  return 0;
}

#endif /* LWIP_IP6IN4 */
