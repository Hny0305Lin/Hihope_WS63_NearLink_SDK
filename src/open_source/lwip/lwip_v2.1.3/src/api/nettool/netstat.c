/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: netstat shell cmd API implementation
 * Author: none
 * Create: 2020
 */

#include "lwip/nettool/netstat.h"
#include "lwip/sockets.h"
#include "lwip/priv/nd6_priv.h"
#include "lwip/ip.h"
#include "lwip/tcpip.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/api.h"
#include "lwip/prot/udp.h"
#include "lwip/udp.h"
#include "lwip/etharp.h"
#include "lwip/raw.h"
#include "lwip/priv/api_msg.h"
#include "lwip/nettool/utility.h"
#if LWIP_LITEOS_COMPAT
#include "los_config.h"
#endif

#ifdef CUSTOM_AT_COMMAND
#include "soc_at.h"
typedef enum netstat_trans_type {
  TCP_IP6,
  TCP,
  UDP_IP6,
  UDP,
  RAW,
  PKT_RAW,
} netstat_trans_type;
#endif

#if LWIP_ENABLE_BASIC_SHELL_CMD

#define MAX_PRINT_SIZE 128

#define NETSTAT_ENTRY_SIZE 120
#define MAX_NETSTAT_ENTRY (NETSTAT_ENTRY_SIZE * (MEMP_NUM_TCP_PCB + MEMP_NUM_UDP_PCB + MEMP_NUM_TCP_PCB_LISTEN + 1))

struct netstat_data {
  s8_t *netstat_out_buf;
  u32_t netstat_out_buf_len;
  u32_t netstat_out_buf_updated_len;
  sys_sem_t cb_completed;
};

#if LWIP_IPV6

extern struct nd6_neighbor_cache_entry neighbor_cache[LWIP_ND6_NUM_NEIGHBORS];

int
netstat_get_udp_sendq_len6(struct udp_pcb *udppcb, struct pbuf *udpbuf)
{
  int send_len = -1;
  u16_t offset = 0;
  u16_t len;
  struct ip6_hdr *iphdr = NULL;
  struct udp_hdr *udphdr = NULL;
  struct ip6_dest_hdr *dest_hdr = NULL;
  struct ip6_frag_hdr *frag_hdr = NULL;
  u8_t nexth;
  u16_t hlen = 0;

  LWIP_ERROR("netstat_get_udp6_sendQLen: NULL pcb received"CRLF, (udppcb != NULL), return -1);
  LWIP_ERROR("netstat_get_udp6_sendQLen: NULL pbuf received"CRLF, (udpbuf != NULL), return -1);

  iphdr = (struct ip6_hdr *)udpbuf->payload;
  LWIP_ERROR("netstat_get_udp6_sendQLen: NULL iphdr received"CRLF, (iphdr != NULL), return -1);
  if (!(ip6_addr_cmp_zoneless(&iphdr->dest, ip_2_ip6(&udppcb->remote_ip)) &&
        (ip_addr_isany(&udppcb->local_ip) ||
         ip_get_option(udppcb, SOF_BINDNONUNICAST) ||
         ip6_addr_cmp_zoneless(&iphdr->src, ip_2_ip6(&udppcb->local_ip))))) {
    goto FUNC_OUT;
  }

  len = IP6_HLEN;
  if (pbuf_header(udpbuf, (s16_t)(-(s16_t)(len)))) {
    goto FUNC_OUT;
  }

  offset = len;

  nexth = IP6H_NEXTH(iphdr);
  while (offset < udpbuf->tot_len) {
    if ((nexth == IP6_NEXTH_NONE) || (nexth == IP6_NEXTH_UDP) || (nexth == IP6_NEXTH_UDPLITE)) {
      break;
    }
    switch (nexth) {
      case IP6_NEXTH_HOPBYHOP:
      case IP6_NEXTH_ROUTING:
        nexth = *((u8_t *)udpbuf->payload);
        /* 8 : hlen will Multiply by 8 in ipv6 option segment */
        hlen = 8 * (1 + (u16_t)*((u8_t *)udpbuf->payload + 1));
        break;
      case IP6_NEXTH_DESTOPTS:
        nexth = *((u8_t *)udpbuf->payload);
        dest_hdr = (struct ip6_dest_hdr *)udpbuf->payload;
        /* 8 : hlen will Multiply by 8 in ipv6 option segment */
        hlen = 8 * (1 + (u16_t)dest_hdr->_hlen);
        break;
      case IP6_NEXTH_FRAGMENT:
        frag_hdr = (struct ip6_frag_hdr *)udpbuf->payload;
        nexth = frag_hdr->_nexth;
        hlen = IP6_FRAG_HLEN;
        break;
      default:
        /* Unknown next_header */
        goto FUNC_OUT;
    }

    (void)pbuf_header(udpbuf, (s16_t)(-(s16_t)hlen));
    offset = (u16_t)(offset + hlen);
  }

  /* If the while loop test condition failed , then revert the last offset change */
  if (offset >= udpbuf->tot_len) {
    offset = (u16_t)(offset - hlen);
    goto FUNC_OUT;
  }

  LWIP_ERROR("Transport option should be UDP", (nexth == IP6_NEXTH_UDP || nexth == IP6_NEXTH_UDPLITE), goto FUNC_OUT);

  if (offset > iphdr->_plen) {
    goto FUNC_OUT;
  }

  /* check if there is enough space for atleast udp header available */
  if (udpbuf->tot_len < UDP_HLEN) {
    goto FUNC_OUT;
  }

  udphdr = (struct udp_hdr *)udpbuf->payload;
  if ((ntohs(udphdr->dest) == udppcb->remote_port) && (ntohs(udphdr->src) == udppcb->local_port)) {
    if (ntohs(udphdr->len) > UDP_HLEN) {
      send_len = ntohs(udphdr->len) - UDP_HLEN;
    } else {
      send_len = udpbuf->tot_len - UDP_HLEN;
    }
  }

FUNC_OUT:
  (void)pbuf_header(udpbuf, (s16_t)offset); // canot not cross max limit of s16_t
  return send_len;
}
#endif

#if LWIP_IPV4
int
netstat_get_udp_sendq_len(struct udp_pcb *udppcb, struct pbuf *udpbuf)
{
  int send_len = -1;
  u16_t offset = 0;
  u16_t len;
  struct ip_hdr *iphdr = NULL;
  struct udp_hdr *udphdr = NULL;

  LWIP_ERROR("netstat_get_udp_sendQLen: NULL pcb received"CRLF, (udppcb != NULL), return -1);
  LWIP_ERROR("netstat_get_udp_sendQLen: NULL pbuf received"CRLF, (udpbuf != NULL), return -1);

  iphdr = (struct ip_hdr *)udpbuf->payload;

  if (!(ip4_addr_cmp(&iphdr->dest, ip_2_ip4(&udppcb->remote_ip))
        && (ip_addr_isany(&udppcb->local_ip) ||
            ip_get_option(udppcb, SOF_BINDNONUNICAST) ||
            ip4_addr_cmp(&iphdr->src, ip_2_ip4(&udppcb->local_ip))))) {
    goto FUNC_OUT;
  }
#if LWIP_UDPLITE
  if ((IPH_PROTO(iphdr) != IP_PROTO_UDP) && (IPH_PROTO(iphdr) != IP_PROTO_UDPLITE))
#else
  if (IPH_PROTO(iphdr) != IP_PROTO_UDP)
#endif
  {
    goto FUNC_OUT;
  }

  if ((ntohs(IPH_OFFSET(iphdr)) & IP_OFFMASK) != 0) {
    goto FUNC_OUT;
  }

  len = (u16_t)(IPH_HL(iphdr) * 4); /* 4: IP Header Length Multiply by 4 */
  if (pbuf_header(udpbuf, (s16_t)(-len))) {
    goto FUNC_OUT;
  }

  offset = (u16_t)(offset + len);

  udphdr = (struct udp_hdr *)udpbuf->payload;
  if ((ntohs(udphdr->dest) == udppcb->remote_port) && (ntohs(udphdr->src) == udppcb->local_port)) {
    send_len = ntohs(udphdr->len) - UDP_HLEN;
  }

FUNC_OUT:
  (void)pbuf_header(udpbuf, (s16_t)offset);
  return send_len;
}
#endif

int
netstat_tcp_recvq(struct tcp_pcb *tpcb)
{
  unsigned int ret_val = 0;
#if LWIP_SO_RCVBUF
  struct netconn *conn = NULL;
#endif

  LWIP_ERROR("netstat_tcp_recvq: Received NULL pcb"CRLF, (tpcb != NULL), return 0);

#if LWIP_SO_RCVBUF
  conn = (struct netconn *)tpcb->callback_arg;
  if (conn != NULL) {
    switch (conn->type) {
      case NETCONN_TCP:
      case NETCONN_RAW:
#if LWIP_IPV6
      case NETCONN_RAW_IPV6:
      case NETCONN_UDP_IPV6:
#endif
      case NETCONN_UDP:
        SYS_ARCH_GET(((unsigned int)conn->recv_avail + conn->lrcv_left), ret_val);
        break;
      default:
        ret_val = 0; /* ur... very ugly, damn DHCP DNS and SNTP */
    }
  }
#endif

  return (int)ret_val;
}

int
netstat_tcp_sendq(struct tcp_pcb *tpcb)
{
  int ret_val = 0;
  struct tcp_seg *useg = NULL;

  LWIP_ERROR("netstat_tcp_sendq: Received NULL pcb"CRLF, (tpcb != NULL), return 0);

  for (useg = tpcb->unacked; useg != NULL; useg = useg->next) {
    ret_val = ret_val + useg->len;
  }

  return ret_val;
}

#if LWIP_IPV6
int
netstat_udp_sendq6(struct udp_pcb *upcb)
{
  int ret_len = 0;
  int idx = 0;
  int i, ret;
#if LWIP_ND6_QUEUEING
  struct nd6_q_entry *neibq = NULL;
#endif

  LWIP_ERROR("netstat_udp_sendq6: Received NULL pcb"CRLF, (upcb != NULL), return 0);

  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (neighbor_cache[i].state != ND6_NO_ENTRY) {
      if (ip6_addr_cmp(&upcb->remote_ip.u_addr.ip6, &neighbor_cache[i].next_hop_address)) {
        idx = i;
        break;
      }
    }
  }
#if LWIP_ND6_QUEUEING
  for (neibq = neighbor_cache[idx].q; neibq != NULL; neibq = neibq->next) {
    if (neibq->p == NULL) {
      return 0;
    }
    ret = netstat_get_udp_sendq_len6(upcb, neibq->p);
    if (ret >= 0) {
      ret_len += ret;
    }
  }
#else
  ret = netstat_get_udp_sendq_len6(upcb, neighbor_cache[idx].q);
  if (ret >= 0) {
    ret_len += ret;
  }
#endif
  return ret_len;
}
#endif

#if LWIP_IPV4
int
netstat_udp_sendq(struct udp_pcb *upcb)
{
  int ret_len = 0;
  int arpidx = -1;
  int i, ret;
#if ARP_QUEUEING
  struct etharp_q_entry *arpq = NULL;
#endif

  LWIP_ERROR("netstat_udp_sendq: Received NULL pcb"CRLF, (upcb != NULL), return 0);

  for (i = 0; i < ARP_TABLE_SIZE; ++i) {
    if (arp_table[i].state != ETHARP_STATE_EMPTY) {
      if (ip4_addr_cmp(ip_2_ip4(&upcb->remote_ip), &arp_table[i].ipaddr)) {
        arpidx = i;
        break;
      }
    }
  }

  if (arpidx >= 0) {
#if ARP_QUEUEING
    for (arpq = arp_table[arpidx].q; arpq != NULL; arpq = arpq->next) {
      ret = netstat_get_udp_sendq_len(upcb, arpq->p);
      if (ret <= 0) {
        continue;
      }
      ret_len += ret;
      if (ret_len <= 0) { // overflow, set rteLen = -1 to indicate
        ret_len = -1;
        break;
      }
    }
#else
    ret = netstat_get_udp_sendq_len(upcb, arp_table[arpidx].q);
    if (ret > 0) {
      ret_len += ret;
      if (ret_len <= 0) { // overflow, set rteLen = -1 to indicate
        ret_len = -1;
      }
    }
#endif
  }
  return ret_len;
}
#endif
int
netstat_netconn_recvq(const struct netconn *conn)
{
  unsigned int ret_val = 0;

#if LWIP_SO_RCVBUF
  if (conn == NULL) {
    return 0;
  }

  switch (NETCONNTYPE_GROUP(conn->type)) {
    case NETCONN_TCP:
    case NETCONN_RAW:
#if PF_PKT_SUPPORT
    case NETCONN_PKT_RAW:
#endif
    case NETCONN_UDP:
      SYS_ARCH_GET(((unsigned int)conn->recv_avail + conn->lrcv_left), ret_val);
      break;
    default:
      ret_val = 0; /* ur... very ugly, damn DHCP DNS and SNTP */
  }
#else
  (void)conn;
#endif
  return (int)ret_val;
}
int
netstat_netconn_sendq(struct netconn *conn)
{
  int ret_val;

  if (conn == NULL) {
    return 0;
  }

  switch (NETCONNTYPE_GROUP(conn->type)) {
    case NETCONN_TCP:
      ret_val = netstat_tcp_sendq(conn->pcb.tcp);
      break;
    case NETCONN_RAW:
      ret_val = 0;
      break;
#if PF_PKT_SUPPORT
    case NETCONN_PKT_RAW:
      ret_val = 0; /* always be 0 as frame send to driver directly */
      break;
#endif
    case NETCONN_UDP:
      ret_val = netstat_udp_sendq(conn->pcb.udp);
      break;
    default:
      ret_val = 0; /* ur... very ugly, damn DHCP DNS and SNTP */
  }

  return ret_val;
}

#ifndef CUSTOM_AT_COMMAND
#if PF_PKT_SUPPORT
static s32_t
print_netstat_pkt_raw(struct netstat_data *ndata, u32_t *entry_buf_offset)
{
  u8_t netif_name[NETIF_NAMESIZE];
  struct netif *netif = NULL;
  s8_t *entry_buf = ndata->netstat_out_buf;
  u32_t entry_buf_len = ndata->netstat_out_buf_len;
  int recv_qlen, send_qlen, i_ret;
  u_int proto;
  struct raw_pcb *rpcb = NULL;
  if (entry_buf == NULL) {
    return ERR_VAL;
  }
  if (pkt_raw_pcbs != NULL) {
    i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                       "\n%-12s%-12s%-12s%-16s%-12s"CRLF, "Type", "Recv-Q", "Send-Q", "Protocol", "netif");
    if ((i_ret <= 0) || ((u32_t)(i_ret) >= entry_buf_len)) {
      return ERR_VAL;
    }
    entry_buf_len -= (u32_t)(i_ret);
    (*entry_buf_offset) += (u32_t)(i_ret);

    for (rpcb = pkt_raw_pcbs; rpcb != NULL; rpcb = rpcb->next) {
      recv_qlen = netstat_netconn_recvq(rpcb->recv_arg);
      send_qlen = netstat_netconn_sendq(rpcb->recv_arg);
      for (netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif->ifindex != rpcb->netifindex) {
          continue;
        }
        i_ret = snprintf_s((char *)netif_name, NETIF_NAMESIZE, NETIF_NAMESIZE - 1, "%s%u", netif->name, netif->num);
        if ((i_ret <= 0) || ((u32_t)(i_ret) >= NETIF_NAMESIZE)) {
          return ERR_VAL;
        }
        break;
      }

      if (netif == NULL) {
        (void)snprintf_s((char *)netif_name, NETIF_NAMESIZE, NETIF_NAMESIZE - 1, "%s", "None");
      }

      proto = ntohs(rpcb->proto.eth_proto);

      i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                         "%-12s%-12d%-12d%-16x%-12s"CRLF, "pkt-raw", recv_qlen, send_qlen, proto, netif_name);
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= entry_buf_len)) {
        return ERR_VAL;
      }
      entry_buf_len -= (u32_t)(i_ret);
      (*entry_buf_offset) += (u32_t)(i_ret);
    }
  }
  ndata->netstat_out_buf_len = entry_buf_len;
  return ERR_OK;
}
#endif

#if LWIP_RAW
static s32_t
print_netstat_raw(struct netstat_data *ndata, u32_t *entry_buf_offset)
{
  struct raw_pcb *rpcb = NULL;
  s8_t local_ip_port[64] = {0};
  s8_t remote_ip_port[64] = {0};
  s8_t *entry_buf = ndata->netstat_out_buf;
  u32_t entry_buf_len = ndata->netstat_out_buf_len;
  int recv_qlen, send_qlen, i_ret;
  if (entry_buf == NULL) {
    return ERR_VAL;
  }
  if (raw_pcbs != NULL) {
    char buf[IPADDR_STRLEN_MAX];
    i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                       "\n%-8s%-12s%-12s%-20s%-20s%-16s%-16s"CRLF,
                       "Type", "Recv-Q", "Send-Q", "Local Address", "Foreign Address", "Protocol", "HDRINCL");
    if ((i_ret <= 0) || ((u32_t)(i_ret) >= entry_buf_len)) {
      return ERR_VAL;
    }
    entry_buf_len -= (u32_t)(i_ret);
    (*entry_buf_offset) += (u32_t)(i_ret);

    for (rpcb = raw_pcbs; rpcb != NULL; rpcb = rpcb->next) {
      (void)ipaddr_ntoa_r(&rpcb->local_ip, buf, IPADDR_STRLEN_MAX);
      i_ret = snprintf_s((char *)local_ip_port, sizeof(local_ip_port), (sizeof(local_ip_port) - 1), "%s",
                         buf);
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= sizeof(local_ip_port))) {
        return ERR_VAL;
      }
      (void)ipaddr_ntoa_r(&rpcb->remote_ip, buf, IPADDR_STRLEN_MAX);
      i_ret = snprintf_s((char *)remote_ip_port, sizeof(remote_ip_port), (sizeof(remote_ip_port) - 1), "%s",
                         buf);
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= sizeof(remote_ip_port))) {
        return ERR_VAL;
      }

      recv_qlen = netstat_netconn_recvq(rpcb->recv_arg);
      send_qlen = netstat_netconn_sendq(rpcb->recv_arg);
      i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                         "%-8s%-12d%-12d%-20s%-20s%-16u%-16d"CRLF, "raw",
                         recv_qlen, send_qlen, local_ip_port, remote_ip_port,
                         rpcb->raw_proto, raw_is_flag_set(rpcb, RAW_FLAGS_HDRINCL));
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= entry_buf_len)) {
        return ERR_VAL;
      }
      entry_buf_len -= (u32_t)(i_ret);
      (*entry_buf_offset) += (u32_t)(i_ret);
    }
  }
  ndata->netstat_out_buf_len = entry_buf_len;
#if PF_PKT_SUPPORT
  s32_t pkt_ret = print_netstat_pkt_raw(ndata, entry_buf_offset);
  if (pkt_ret == ERR_VAL) {
    return ERR_VAL;
  }
#endif
  return ERR_OK;
}
#endif
#endif

#if LWIP_UDP
static s32_t
print_netstat_udp(struct netstat_data *ndata, u32_t *entry_buf_offset)
{
  s8_t local_ip_port[64] = {0};
  s8_t remote_ip_port[64] = {0};
  struct udp_pcb *upcb = NULL;
  s8_t *entry_buf = ndata->netstat_out_buf;
  u32_t entry_buf_len = ndata->netstat_out_buf_len;
  int recv_qlen, send_qlen, i_ret;
  if (entry_buf == NULL) {
    return ERR_VAL;
  }
  if (udp_pcbs != NULL) {
    char buf[IPADDR_STRLEN_MAX];
#ifndef CUSTOM_AT_COMMAND
    i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                       "\n%-8s%-12s%-12s%-24s%-24s"CRLF, "Proto", "Recv-Q", "Send-Q",
                       "Local Address", "Foreign Address");
    if ((i_ret <= 0) || ((u32_t)(i_ret) >= entry_buf_len)) {
      return ERR_VAL;
    }
    entry_buf_len -= (u32_t)(i_ret);
    (*entry_buf_offset) += (u32_t)(i_ret);
#endif

    for (upcb = udp_pcbs; upcb != NULL; upcb = upcb->next) {
      (void)ipaddr_ntoa_r(&upcb->local_ip, buf, IPADDR_STRLEN_MAX);
      i_ret = snprintf_s((char *)local_ip_port, sizeof(local_ip_port), (sizeof(local_ip_port) - 1), "%s:%d",
                         buf, upcb->local_port);
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= sizeof(local_ip_port))) {
        return ERR_VAL;
      }

      (void)ipaddr_ntoa_r(&upcb->remote_ip, buf, IPADDR_STRLEN_MAX);
      i_ret = snprintf_s((char *)remote_ip_port, sizeof(remote_ip_port), (sizeof(remote_ip_port) - 1), "%s:%d",
                         buf, upcb->remote_port);
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= sizeof(remote_ip_port))) {
        return ERR_VAL;
      }

      recv_qlen = (upcb->recv == recv_udp) ? netstat_netconn_recvq(upcb->recv_arg) : 0;
#if LWIP_IPV6
      send_qlen = IP_IS_V6(&upcb->local_ip) ? netstat_udp_sendq6(upcb) : netstat_udp_sendq(upcb);
#else
      send_qlen = netstat_udp_sendq(upcb);
#endif
      if (entry_buf_len <= 1) {
        return ERR_VAL;
      }
#ifdef CUSTOM_AT_COMMAND
      /* Proto 2:udp-ip6; 3:udp */
      i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                         "+NETSTAT:%d,%d,%d,%s,%s,%d"CRLF,
                         (IP_IS_V6(&upcb->local_ip) ? UDP_IP6 : UDP), recv_qlen, send_qlen, local_ip_port,
                         remote_ip_port, 0);
#else
      i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                         (IP_IS_V6(&upcb->local_ip) ? "%-8s%-12d%-12d%-39s%-39s%-16s\n" :
                          "%-8s%-12d%-12d%-24s%-24s%-16s"CRLF),
                         IP_IS_V6(&upcb->local_ip) ? "udp-ip6" : "udp",
                         recv_qlen, send_qlen, local_ip_port, remote_ip_port, " ");
#endif
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= entry_buf_len)) {
        return ERR_VAL;
      }
      entry_buf_len -= (u32_t)(i_ret);
      (*entry_buf_offset) += (u32_t)(i_ret);
    }
  }
  ndata->netstat_out_buf_len = entry_buf_len;
  return ERR_OK;
}
#endif

#if LWIP_TCP
static s32_t
print_netstat_tcp(struct netstat_data *ndata, u32_t *entry_buf_offset)
{
  s8_t local_ip_port[64] = {0};
  s8_t remote_ip_port[64] = {0};
  struct tcp_pcb *tpcb = NULL;
  u16_t remote_port;
  s8_t *entry_buf = ndata->netstat_out_buf;
  u32_t entry_buf_len = ndata->netstat_out_buf_len;
  int recv_qlen, send_qlen, i_ret;
  if (entry_buf == NULL) {
    return ERR_VAL;
  }
  if ((tcp_active_pcbs == NULL) && (tcp_bound_pcbs == NULL) &&
      (tcp_tw_pcbs == NULL) && (tcp_listen_pcbs.pcbs == NULL)) {
    return ERR_OK;
  }
#ifndef CUSTOM_AT_COMMAND
  i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                     "%-8s%-12s%-12s%-24s%-24s%-16s"CRLF,
                     "Proto", "Recv-Q", "Send-Q", "Local Address", "Foreign Address", "State");
  if ((i_ret <= 0) || ((u32_t)(i_ret) >= entry_buf_len)) {
    return ERR_VAL;
  }
  entry_buf_len -= (u32_t)(i_ret);
  (*entry_buf_offset) += (u32_t)(i_ret);
#endif

  int i;
  char buf[IPADDR_STRLEN_MAX];
  for (i = 0; i < NUM_TCP_PCB_LISTS; i++) {
    struct tcp_pcb *pcblist = *tcp_pcb_lists[i];
    for (tpcb = pcblist; tpcb != NULL; tpcb = tpcb->next) {
      /* DON'T access a tcp_pcb's remote_port if it's casted from a tcp_pcb_listen */
      remote_port = (pcblist == tcp_listen_pcbs.pcbs) ? 0 : tpcb->remote_port;
      (void)ipaddr_ntoa_r(&tpcb->local_ip, buf, IPADDR_STRLEN_MAX);
      i_ret = snprintf_s((char *)local_ip_port, sizeof(local_ip_port), (sizeof(local_ip_port) - 1), "%s:%u",
                         buf, tpcb->local_port);
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= sizeof(local_ip_port))) {
        return ERR_VAL;
      }
      (void)ipaddr_ntoa_r(&tpcb->remote_ip, buf, IPADDR_STRLEN_MAX);
      i_ret = snprintf_s((char *)remote_ip_port, sizeof(remote_ip_port), (sizeof(remote_ip_port) - 1), "%s:%u",
                         buf, remote_port);
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= sizeof(remote_ip_port))) {
        return ERR_VAL;
      }

      if (pcblist == tcp_bound_pcbs) {
        send_qlen = 0;
        recv_qlen = 0;
      } else if (pcblist == tcp_listen_pcbs.pcbs) {
        recv_qlen = netstat_netconn_recvq(tpcb->callback_arg);
        send_qlen = 0;
      } else {
        recv_qlen = (tpcb->state == SYN_RCVD) ? 0 : netstat_netconn_recvq(tpcb->callback_arg);
        send_qlen = (tpcb->state == SYN_RCVD) ? 0 : netstat_netconn_sendq(tpcb->callback_arg);
      }

#ifdef CUSTOM_AT_COMMAND
      /* Proto 0:tcp-ip6; 1:tcp .State for tcp_state_str */
      i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                         "+NETSTAT:%d,%d,%d,%s,%s,%d"CRLF,
                         (IP_IS_V6(&tpcb->local_ip) ? TCP_IP6 : TCP), recv_qlen, send_qlen, local_ip_port,
                         remote_ip_port, tpcb->state);
#else
      i_ret = snprintf_s((char *)(entry_buf + (*entry_buf_offset)), entry_buf_len, entry_buf_len - 1,
                         (IP_IS_V6(&tpcb->local_ip) ? "%-8s%-12d%-12d%-39s%-39s%-16s"CRLF :
                          "%-8s%-12d%-12d%-24s%-24s%-16s"CRLF),
                         IP_IS_V6(&tpcb->local_ip) ? "tcp-ip6" : "tcp",
                         recv_qlen, send_qlen, local_ip_port, remote_ip_port, tcp_state_str[tpcb->state]);
#endif
      if ((i_ret <= 0) || ((u32_t)(i_ret) >= entry_buf_len)) {
        return ERR_VAL;
      }
      entry_buf_len -= (u32_t)(i_ret);
      (*entry_buf_offset) += (u32_t)(i_ret);
    }
  }

  ndata->netstat_out_buf_len = entry_buf_len;
  return ERR_OK;
}
#endif

void
netstat_internal(void *ctx)
{
  s8_t *entry_buf = NULL;
  u32_t entry_buf_offset = 0;
  struct netstat_data *ndata = (struct netstat_data *)ctx;
  entry_buf = ndata->netstat_out_buf;

  if (entry_buf == NULL) {
    goto out;
  }

#ifndef CUSTOM_AT_COMMAND
  int i_ret;
  i_ret = snprintf_s((char *)(entry_buf), ndata->netstat_out_buf_len, ndata->netstat_out_buf_len - 1,
                     "========== total sockets %d ======  unused sockets %d =========="CRLF, LWIP_CONFIG_NUM_SOCKETS,
                     get_unused_socket_num());
  if ((i_ret <= 0) || ((u32_t)(i_ret) >= ndata->netstat_out_buf_len)) {
    goto out;
  }
  ndata->netstat_out_buf_len -= (u32_t)(i_ret);
  entry_buf_offset = (u32_t)(i_ret);
#endif
#if LWIP_TCP
  s32_t tcp_ret = print_netstat_tcp(ndata, &entry_buf_offset);
  if (tcp_ret == ERR_VAL) {
    goto out;
  }
#endif
#if LWIP_UDP
  s32_t udp_ret = print_netstat_udp(ndata, &entry_buf_offset);
  if (udp_ret == ERR_VAL) {
    goto out;
  }
#endif
#ifndef CUSTOM_AT_COMMAND
#if LWIP_RAW
  s32_t raw_ret = print_netstat_raw(ndata, &entry_buf_offset);
  if (raw_ret == ERR_VAL) {
    goto out;
  }
#endif
#endif
out:
  ndata->netstat_out_buf_updated_len = entry_buf_offset;
  sys_sem_signal(&ndata->cb_completed);
  return;
}

static void
print_netstat_out_buf_updated_len(struct netstat_data *ndata)
{
  u32_t print_len = 0;
  char print_out_buff[MAX_PRINT_SIZE] = {0};
  if (ndata->netstat_out_buf_updated_len < MAX_PRINT_SIZE) {
#ifdef CUSTOM_AT_COMMAND
    (void)uapi_at_printf("%s", (char *)(ndata->netstat_out_buf));
#else
    LWIP_PLATFORM_PRINT("%s"CRLF, (char *)(ndata->netstat_out_buf));
#endif
  } else {
    do {
      (void)memset_s(print_out_buff, sizeof(print_out_buff), 0x0, sizeof(print_out_buff));
      (void)memcpy_s(print_out_buff, sizeof(print_out_buff) - 1, ndata->netstat_out_buf + print_len,
                     sizeof(print_out_buff) - 1);
#ifdef CUSTOM_AT_COMMAND
      (void)uapi_at_printf("%s", print_out_buff);
#else
       LWIP_PLATFORM_PRINT("%s", print_out_buff);
#endif
       ndata->netstat_out_buf_updated_len -= sizeof(print_out_buff) - 1;
       print_len += sizeof(print_out_buff) - 1;
    } while (ndata->netstat_out_buf_updated_len >= (MAX_PRINT_SIZE - 1));

    if (ndata->netstat_out_buf_updated_len > 0) {
#ifdef CUSTOM_AT_COMMAND
      (void)uapi_at_printf("%s", (char *)(ndata->netstat_out_buf + print_len));
#else
       LWIP_PLATFORM_PRINT("%s", (char *)(ndata->netstat_out_buf + print_len));
#endif
    }
    LWIP_PLATFORM_PRINT(CRLF);
  }
#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("OK"CRLF);
#endif
}

u32_t
os_shell_netstat(int argc, const char **argv)
{
  struct netstat_data ndata;
  err_t err;

  if (argc > 0) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_USAGE);
    return OS_NOK;
  }

  if (tcpip_init_finish == 0) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_TCPIP_UNINTED);
    return OS_NOK;
  }
  (void)memset_s(&ndata, sizeof(struct netstat_data), 0, sizeof(struct netstat_data));
  ndata.netstat_out_buf = mem_malloc(MAX_NETSTAT_ENTRY);
  if (ndata.netstat_out_buf == NULL) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_MEM_ERR);
    return OS_NOK;
  }
  ndata.netstat_out_buf_len = MAX_NETSTAT_ENTRY;
  ndata.netstat_out_buf_updated_len = 0;

  if (sys_sem_new(&ndata.cb_completed, 0) != ERR_OK) {
    goto err_hand;
  }

  err = tcpip_callback(netstat_internal, &ndata);
  if (err != ERR_OK) {
    sys_sem_free(&ndata.cb_completed);
    goto err_hand;
  }

  (void)sys_arch_sem_wait(&ndata.cb_completed, 0);
  sys_sem_free(&ndata.cb_completed);
  if ((ndata.netstat_out_buf_updated_len > 0) && (ndata.netstat_out_buf_updated_len < MAX_NETSTAT_ENTRY)) {
    print_netstat_out_buf_updated_len(&ndata);
    mem_free(ndata.netstat_out_buf);
    return OS_OK;
  } else if (ndata.netstat_out_buf_updated_len >= MAX_NETSTAT_ENTRY) {
    goto err_hand;
  } else {
#ifdef CUSTOM_AT_COMMAND
    (void)uapi_at_printf("OK"CRLF);
#endif
    mem_free(ndata.netstat_out_buf);
    return OS_OK;
  }

err_hand:
  mem_free(ndata.netstat_out_buf);
  ndata.netstat_out_buf = NULL;
  (void)(argv);
  return OS_NOK;
}

#endif /* LWIP_ENABLE_BASIC_SHELL_CMD */
