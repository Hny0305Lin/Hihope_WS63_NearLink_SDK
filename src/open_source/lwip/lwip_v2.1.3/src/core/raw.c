/**
 * @file
 * Implementation of raw protocol PCBs for low-level handling of
 * different types of protocols besides (or overriding) those
 * already available in lwIP.\n
 * See also @ref raw_raw
 *
 * @defgroup raw_raw RAW
 * @ingroup callbackstyle_api
 * Implementation of raw protocol PCBs for low-level handling of
 * different types of protocols besides (or overriding) those
 * already available in lwIP.\n
 * @see @ref api
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"

#if LWIP_RAW /* don't build if not configured for use in lwipopts.h */

#include "lwip/def.h"
#include "lwip/memp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/raw.h"
#include "lwip/priv/raw_priv.h"
#include "lwip/stats.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/inet_chksum.h"
#ifdef LWIP_IPV6
#include "lwip/icmp6.h"
#include "lwip/prot/udp.h"
#include "lwip/prot/tcp.h"
#include "lwip/api.h"
#endif
#include "lwip/etharp.h"
#include "lwip/prot/ethernet.h"
#include <string.h>

#if PF_PKT_SUPPORT
const struct eth_hdr *g_lwip_current_eth_hdr;
const struct netif *g_lwip_current_netif;
#endif

/** The list of RAW PCBs */
struct raw_pcb *raw_pcbs;
#if PF_PKT_SUPPORT
struct raw_pcb *pkt_raw_pcbs;
struct raw_pcb *all_pkt_raw_pcbs;
#endif

static u8_t
raw_input_local_match(struct raw_pcb *pcb, u8_t broadcast)
{
  LWIP_UNUSED_ARG(broadcast); /* in IPv6 only case */

  /* check if PCB is bound to specific netif */
  if ((pcb->netif_idx != NETIF_NO_INDEX) &&
      (pcb->netif_idx != netif_get_index(ip_data.current_input_netif))) {
    return 0;
  }

#if LWIP_IPV4 && LWIP_IPV6
  /* Dual-stack: PCBs listening to any IP type also listen to any IP address */
  if (IP_IS_ANY_TYPE_VAL(pcb->local_ip)) {
#if IP_SOF_BROADCAST_RECV
    if ((broadcast != 0) && !ip_get_option(pcb, SOF_BROADCAST)) {
      return 0;
    }
#endif /* IP_SOF_BROADCAST_RECV */
    return 1;
  }
#endif /* LWIP_IPV4 && LWIP_IPV6 */

  /* Only need to check PCB if incoming IP version matches PCB IP version */
  if (IP_ADDR_PCB_VERSION_MATCH_EXACT(pcb, ip_current_dest_addr())) {
#if LWIP_IPV4
    /* Special case: IPv4 broadcast: receive all broadcasts
     * Note: broadcast variable can only be 1 if it is an IPv4 broadcast */
    if (broadcast != 0) {
#if IP_SOF_BROADCAST_RECV
      if (ip_get_option(pcb, SOF_BROADCAST))
#endif /* IP_SOF_BROADCAST_RECV */
      {
        if (ip4_addr_isany(ip_2_ip4(&pcb->local_ip))) {
          return 1;
        }
      }
    } else
#endif /* LWIP_IPV4 */
      /* Handle IPv4 and IPv6: catch all or exact match */
      if (ip_addr_isany(&pcb->local_ip) ||
          ip_addr_cmp(&pcb->local_ip, ip_current_dest_addr())) {
        return 1;
      }
  }

  return 0;
}

#if LWIP_IPV6
#if LWIP_SOCK_OPT_ICMP6_FILTER
static u32_t icmpv6_filter_check(struct pbuf *p, struct raw_pcb *pcb, s16_t proto, u16_t *typep)
{
  u8_t type;
  struct icmpv6_hdr *icmp6_tmphdr = NULL;
  /*
   * extract the icmp6 header type and check if it is present in icmp6_filter
   * filter structure.Use the ICMP6_FILTER_WILLBLOCK macros to check
   * if this icmpv6 message need to be blocked/filtered at application.
   * The current Macros are slightly reversed to rfc2292 macros. Macros are in compliance
   * with the litos linux header files.
   */
  if (proto == IPPROTO_ICMPV6) {
    u32_t *data = &pcb->icmp6_filter.icmp6_filt[0];
    icmp6_tmphdr = (struct icmpv6_hdr *)(p->payload);
    type = icmp6_tmphdr->type;
    *typep = type;
    return (u32_t)((data[(type) >> ICMP6_FILTER_VAL]) & (1U << ((type) & ICMP6_FILTER_INTVAL)));
  }

  return 0;
}
#endif /* LWIP_SOCK_OPT_ICMP6_FILTER */

#if LWIP_IPV6_PER_PROTO_CHKSUM
static u32_t lwip_ipv6checksum_validate(struct pbuf *p, const struct raw_pcb *pcb, s16_t proto)
{
  u32_t ret = 0;

  if (proto == IP6_NEXTH_ICMP6) {
    /* checksum will be from 3rd byte. so */
    if (p->len < sizeof(struct icmpv6_hdr)) {
      /* drop short packets and dont give to application */
      LWIP_DEBUGF(RAW_DEBUG, ("icmp6_input: length mismatch failed .\n"));
      return 1;
    }

    /* if ret value is 0 it mean checksum is Ok. */
    ret = ip6_chksum_pseudo(p, pcb->raw_proto,  p->tot_len, ip6_current_src_addr(), ip6_current_dest_addr());
  } else {
    if (pcb->chksum_reqd == 0) {
      /* returning 0 , as the checksum validation is not enabled so need to give to app layer */
      return 0;
    }

    if ((proto == IP6_NEXTH_UDP) && (p->len < UDP_HLEN)) {
      /*
       * In this case it will be given to recv callback
       * in raw_input() if length is not proper
       * drop short packets
       */
      LWIP_DEBUGF(RAW_DEBUG,
                  ("udp_input: short UDP datagram (%"U16_F" bytes) discarded\n", p->tot_len));
      return 1;
    } else if ((proto == IP6_NEXTH_TCP) && (p->len < TCP_HLEN)) {
      /* drop short packets */
      LWIP_DEBUGF(RAW_DEBUG, ("tcp_input: short packet (%"U16_F" bytes) discarded\n", p->tot_len));
      return 1;
    }

    /* if ret value is 0 it mean checksum is Ok. */
    ret = ip6_chksum_pseudo(p, (u8_t)proto, p->tot_len,
                            ip6_current_src_addr(),
                            ip6_current_dest_addr());
  }
  return ret;
}
#endif /* LWIP_IPV6_PER_PROTO_CHKSUM */
#endif

/**
 * Determine if in incoming IP packet is covered by a RAW PCB
 * and if so, pass it to a user-provided receive callback function.
 *
 * Given an incoming IP datagram (as a chain of pbufs) this function
 * finds a corresponding RAW PCB and calls the corresponding receive
 * callback function.
 *
 * @param p pbuf to be demultiplexed to a RAW PCB.
 * @param inp network interface on which the datagram was received.
 * @return - RAW_INPUT_EATEN if the packet has been processed by a RAW PCB receive
 *           callback function.
 * @return - RAW_INPUT_DELIVERED if the packet has been processed by a RAW PCB receive
 *           callback function but fail to delivered to upper layer.
 * @return - RAW_INPUT_NONE if packet is not been processed.
 *
 */
raw_input_state_t
raw_input(struct pbuf *p, struct netif *inp)
{
  struct raw_pcb *pcb;
  s16_t proto;
  raw_input_state_t ret = RAW_INPUT_NONE;
  u8_t broadcast = ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif());

  LWIP_UNUSED_ARG(inp);

#if LWIP_IPV6
#if LWIP_IPV4
  if (IP_HDR_GET_VERSION(p->payload) == 6)
#endif /* LWIP_IPV4 */
  {
    struct ip6_hdr *ip6hdr = (struct ip6_hdr *)p->payload;
    proto = IP6H_NEXTH(ip6hdr);
  }
#if LWIP_IPV4
  else
#endif /* LWIP_IPV4 */
#endif /* LWIP_IPV6 */
#if LWIP_IPV4
  {
    proto = IPH_PROTO((struct ip_hdr *)p->payload);
  }
#endif /* LWIP_IPV4 */

  pcb = raw_pcbs;
  /* loop through all raw pcbs until the packet is eaten by one */
  /* this allows multiple pcbs to match against the packet by design */
  while (pcb != NULL) {
    if ((pcb->raw_proto == proto) && raw_input_local_match(pcb, broadcast) &&
        (((pcb->flags & RAW_FLAGS_CONNECTED) == 0) ||
         ip_addr_cmp(&pcb->remote_ip, ip_current_src_addr()))) {
      /* receive callback function available? */
      if (pcb->recv != NULL) {
        u8_t eaten;

#if LWIP_IPV4 && LWIP_IPV6
        struct netconn *conn;
        conn = (struct netconn *)pcb->recv_arg;
        if (NETCONNTYPE_ISIPV6(NETCONN_TYPE(conn)) && IP_IS_V4_VAL(*ip_current_src_addr())) {
          pcb = pcb->next;
          continue;
        }
#endif
        /* the receive callback function did not eat the packet? */
        eaten = pcb->recv(pcb->recv_arg, pcb, p, ip_current_src_addr());
        if (eaten != 0) {
          /* receive function ate the packet */
          ret = RAW_INPUT_EATEN;
        } else {
          if (ret == RAW_INPUT_NONE) {
            ret = RAW_INPUT_DELIVERED;
          }
        }
      }
      /* no receive callback function was set for this raw PCB */
    }
    /* drop the packet */
    pcb = pcb->next;
  }
  return ret;
}

#if LWIP_IPV6
/*
 * Determine if in incoming IP packet is covered by a RAW PCB
 * and if so, pass it to a user-provided receive callback function.
 *
 * Given an incoming IP datagram (as a chain of pbufs) this function
 * finds a corresponding RAW PCB and calls the corresponding receive
 * callback function.
 *
 * @param p pbuf to be demultiplexed to a RAW PCB.
 * @param inp network interface on which the datagram was received.
 * @return - RAW_INPUT_EATEN if the packet has been processed by a RAW PCB receive
 *           callback function.
 * @return - RAW_INPUT_DELIVERED if the packet has been processed by a RAW PCB receive
 *           callback function but fail to delivered to upper layer.
 * @return - RAW_INPUT_NONE if packet is not been processed.
 *
 */
raw_input_state_t
raw_input6(struct pbuf *p, s16_t proto, s8_t *is_checksuminvalid)
{
  struct raw_pcb *pcb = NULL;
#if LWIP_SOCK_OPT_ICMP6_FILTER
  u16_t type;
#endif
  raw_input_state_t eaten = RAW_INPUT_NONE;

  u8_t broadcast = (u8_t)(ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif()));
  *is_checksuminvalid = 0;

  pcb = raw_pcbs;
  /* loop through all raw pcbs */
  /* this allows multiple pcbs to match against the packet by design */
  while (pcb != NULL) {
    if ((pcb->raw_proto == proto) && raw_input_local_match(pcb, broadcast) &&
        (((pcb->flags & RAW_FLAGS_CONNECTED) == 0))) {
      /* receive callback function available? */
      if (pcb->recv != NULL) {
#if LWIP_IPV6_PER_PROTO_CHKSUM
        if (lwip_ipv6checksum_validate(p, pcb, proto)) {
          LWIP_DEBUGF(RAW_DEBUG, ("checksum validation failed for proto = %"U16_F"\n", proto));
          *is_checksuminvalid = 1;
          pcb = pcb->next;
          continue;
        }
#endif
#if LWIP_SOCK_OPT_ICMP6_FILTER
        type = 0;
        if (icmpv6_filter_check(p, pcb, proto, &type) != 0) {
          LWIP_DEBUGF(RAW_DEBUG, ("packet filtered of icmp6 type = %"U16_F"\n", type));
          pcb = pcb->next;
          continue;
        }
#endif
        /* the receive callback function did not eat the packet? */
        if (pcb->recv(pcb->recv_arg, pcb, p, ip_current_src_addr())) {
          eaten = RAW_INPUT_EATEN;
        } else {
          if (eaten == RAW_INPUT_NONE) {
            eaten = RAW_INPUT_DELIVERED;
          }
        }
      }
      /* no receive callback function was set for this raw PCB */
    }

    pcb = pcb->next;
  }
  return eaten;
}

#endif

#if PF_PKT_SUPPORT
/*
 * Determine if in incoming IP packet is covered by a RAW PCB
 * and if so, pass it to a user-provided receive callback function.
 *
 * Given an incoming IP datagram (as a chain of pbufs) this function
 * finds a corresponding RAW PCB and calls the corresponding receive
 * callback function.
 *
 * @param p pbuf to be demultiplexed to a RAW PCB.
 * @param inp network interface on which the datagram was received.
  * @param from the pbuf is from which NETCONN_PKT_RAW type raw_pcb,
 *           otherwise it should be NULL.
 * @return- void
 *
 */
void raw_pkt_input(struct pbuf *p, const struct netif *inp, const struct raw_pcb *from)
{
  struct raw_pcb *pcb = NULL;
  struct eth_hdr *ethhdr = NULL;
  u16_t proto;

  LWIP_UNUSED_ARG(inp);

  ethhdr = (struct eth_hdr *)p->payload;
  proto = ethhdr->type;

  g_lwip_current_eth_hdr = ethhdr;
  g_lwip_current_netif = inp;

  pcb = pkt_raw_pcbs;
  /* loop through all raw pcbs until the packet is eaten by one */
  /* this allows multiple pcbs to match against the packet by design */
  while (pcb != NULL) {
    if (((pcb->proto.eth_proto == htons(ETHTYPE_ALL)) || ((p != NULL) && !(p->flags & PBUF_FLAG_OUTGOING) &&
        pcb->proto.eth_proto == proto)) &&
        ((!pcb->netifindex) || (pcb->netifindex == inp->ifindex)) && (pcb != from)) {
      /* receive callback function available? */
      if (pcb->recv != NULL) {
        /* the receive callback function did not eat the packet? */
        if (pcb->recv(pcb->recv_arg, pcb, p, NULL) != 0) {
          LWIP_DEBUGF(RAW_DEBUG, ("raw_pkt_input: packets recved failed \n"));
        }
      }
      /* no receive callback function was set for this raw PCB */
    }

    pcb = pcb->next;
  }

  g_lwip_current_eth_hdr = NULL;
  return;
}
#endif

/**
 * @ingroup raw_raw
 * Bind a RAW PCB.
 *
 * @param pcb RAW PCB to be bound with a local address ipaddr.
 * @param ipaddr local IP address to bind with. Use IP4_ADDR_ANY to
 * bind to all local interfaces.
 *
 * @return lwIP error code.
 * - ERR_OK. Successful. No error occurred.
 * - ERR_USE. The specified IP address is already bound to by
 * another RAW PCB.
 *
 * @see raw_disconnect()
 */
err_t
raw_bind(struct raw_pcb *pcb, const ip_addr_t *ipaddr)
{
  LWIP_ASSERT_CORE_LOCKED();
  if ((pcb == NULL) || (ipaddr == NULL)) {
    return ERR_VAL;
  }
  ip_addr_set_ipaddr(&pcb->local_ip, ipaddr);
#if LWIP_IPV6 && LWIP_IPV6_SCOPES
  /* If the given IP address should have a zone but doesn't, assign one now.
   * This is legacy support: scope-aware callers should always provide properly
   * zoned source addresses. */
  if (IP_IS_V6(&pcb->local_ip) &&
      ip6_addr_lacks_zone(ip_2_ip6(&pcb->local_ip), IP6_UNKNOWN)) {
    ip6_addr_select_zone(ip_2_ip6(&pcb->local_ip), ip_2_ip6(&pcb->local_ip));
  }
#endif /* LWIP_IPV6 && LWIP_IPV6_SCOPES */
  if (netif_ipaddr_isbrdcast(ipaddr) || ip_addr_ismulticast(ipaddr)) {
    ip_set_option(pcb, SOF_BINDNONUNICAST);
  } else {
    ip_reset_option(pcb, SOF_BINDNONUNICAST);
  }
  return ERR_OK;
}

#if PF_PKT_SUPPORT
/*
 * Bind a RAW PCB for Packet family.
 *
 * @param pcb RAW PCB to be bound with a local address ipaddr.
 * @param ifindex Interface Index to bind with. Use IP_ADDR_ANY to
 * bind to all local interfaces.
 *
 * @return lwIP error code.
 * - ERR_OK. Successful. No error occured.
 *
 * @see raw_disconnect()
 */
err_t
raw_pkt_bind(struct raw_pcb *pcb, u8_t ifindex, u16_t proto)
{
  struct netif *loc_netif = NULL;

  if (ifindex != 0) {
    for (loc_netif = netif_list; loc_netif != NULL; loc_netif = loc_netif->next) {
      if (ifindex == loc_netif->ifindex) {
        break;
      }
    }

    /* Return if no matching netifs to bind */
    if (loc_netif == NULL) {
      LWIP_DEBUGF(RAW_DEBUG, ("raw_pkt_bind: No matching netif found for ifindex(%u)\n", ifindex));
      return ERR_NODEV;
    }
  } else {
    return ERR_NODEV;
  }

#if DRIVER_STATUS_CHECK
  if (!netif_is_up(loc_netif) || !netif_is_ready(loc_netif))
#else
  if (!netif_is_up(loc_netif))
#endif
  {
    LWIP_DEBUGF(RAW_DEBUG, ("raw_pkt_bind: bind failed as netif (index %u) was down\n", ifindex));
    return ERR_NETDOWN;
  }

  pcb->netifindex = ifindex;
  pcb->proto.eth_proto = proto;

  return ERR_OK;
}
#endif

/**
 * @ingroup raw_raw
 * Bind an RAW PCB to a specific netif.
 * After calling this function, all packets received via this PCB
 * are guaranteed to have come in via the specified netif, and all
 * outgoing packets will go out via the specified netif.
 *
 * @param pcb RAW PCB to be bound with netif.
 * @param netif netif to bind to. Can be NULL.
 *
 * @see raw_disconnect()
 */
void
raw_bind_netif(struct raw_pcb *pcb, const struct netif *netif)
{
  LWIP_ASSERT_CORE_LOCKED();
  if (netif != NULL) {
    pcb->netif_idx = netif_get_index(netif);
  } else {
    pcb->netif_idx = NETIF_NO_INDEX;
  }
}

/**
 * @ingroup raw_raw
 * Connect an RAW PCB. This function is required by upper layers
 * of lwip. Using the raw api you could use raw_sendto() instead
 *
 * This will associate the RAW PCB with the remote address.
 *
 * @param pcb RAW PCB to be connected with remote address ipaddr and port.
 * @param ipaddr remote IP address to connect with.
 *
 * @return lwIP error code
 *
 * @see raw_disconnect() and raw_sendto()
 */
err_t
raw_connect(struct raw_pcb *pcb, const ip_addr_t *ipaddr)
{
  struct netif *netif = NULL;
  LWIP_ASSERT_CORE_LOCKED();
  if ((pcb == NULL) || (ipaddr == NULL)) {
    return ERR_VAL;
  }

  netif = ip_route_pcb(ipaddr, (struct ip_pcb*)pcb);
  if (netif == NULL) {
    return ERR_NETUNREACH;
  }

  if (!ip_get_option(pcb, SOF_BROADCAST) && ip_addr_isbroadcast(ipaddr, netif)) {
    return ERR_ACCESS;
  }

  ip_addr_set_ipaddr(&pcb->remote_ip, ipaddr);
#if LWIP_IPV6 && LWIP_IPV6_SCOPES
  /* If the given IP address should have a zone but doesn't, assign one now,
   * using the bound address to make a more informed decision when possible. */
  if (IP_IS_V6(&pcb->remote_ip) &&
      ip6_addr_lacks_zone(ip_2_ip6(&pcb->remote_ip), IP6_UNKNOWN)) {
    ip6_addr_select_zone(ip_2_ip6(&pcb->remote_ip), ip_2_ip6(&pcb->local_ip));
  }
#endif /* LWIP_IPV6 && LWIP_IPV6_SCOPES */
  raw_set_flags(pcb, RAW_FLAGS_CONNECTED);
  return ERR_OK;
}

/**
 * @ingroup raw_raw
 * Disconnect a RAW PCB.
 *
 * @param pcb the raw pcb to disconnect.
 */
void
raw_disconnect(struct raw_pcb *pcb)
{
  LWIP_ASSERT_CORE_LOCKED();
  /* reset remote address association */
#if LWIP_IPV4 && LWIP_IPV6
  if (IP_IS_ANY_TYPE_VAL(pcb->local_ip)) {
    ip_addr_copy(pcb->remote_ip, *IP_ANY_TYPE);
  } else {
#endif
    ip_addr_set_any(IP_IS_V6_VAL(pcb->remote_ip), &pcb->remote_ip);
#if LWIP_IPV4 && LWIP_IPV6
  }
#endif
  pcb->netif_idx = NETIF_NO_INDEX;
  /* mark PCB as unconnected */
  raw_clear_flags(pcb, RAW_FLAGS_CONNECTED);
}

/**
 * @ingroup raw_raw
 * Set the callback function for received packets that match the
 * raw PCB's protocol and binding.
 *
 * The callback function MUST either
 * - eat the packet by calling pbuf_free() and returning non-zero. The
 *   packet will not be passed to other raw PCBs or other protocol layers.
 * - not free the packet, and return zero. The packet will be matched
 *   against further PCBs and/or forwarded to another protocol layers.
 */
void
raw_recv(struct raw_pcb *pcb, raw_recv_fn recv, void *recv_arg)
{
  LWIP_ASSERT_CORE_LOCKED();
  /* remember recv() callback and user data */
  pcb->recv = recv;
  pcb->recv_arg = recv_arg;
}

#if PF_PKT_SUPPORT
/*
 * Send the raw IP packet through the given netif driver. Note that actually you cannot
 * modify the link layer header here. Packet need to be sent to driver as it is through the
 * given netif
 * @param pcb the raw pcb which to send
 * @param p the ethernet packet to send
 * @param ifindex the Interface index of the netif through which packet needs to be sent
 */
err_t
raw_pkt_sendto(const struct raw_pcb *pcb, struct pbuf *p, u8_t ifindex)
{
  struct netif *netif = NULL;
  u8_t netifindex;
  LWIP_UNUSED_ARG(pcb);

  LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("raw_pkt_sendto: ifindex=%d\n", ifindex));
  LWIP_ASSERT("p != NULL", p != NULL);

  netifindex = ifindex;

  if (ifindex == 0) {
    if (pcb->netifindex != 0) {
      netifindex = pcb->netifindex;
    } else {
      return ERR_NODEVADDR;
    }
  }

  /* Find the netif corresponding to the interface index */
  netif = netif_find_by_ifindex(netifindex);
  if (netif == NULL) {
    LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("netif not found for given ifindex (%u)\n", ifindex));
    return ERR_NODEVADDR;
  }

#if DRIVER_STATUS_CHECK
  if ((!netif_is_up(netif)) || (!netif_is_ready(netif)))
#else
  if ((!netif_is_up(netif)))
#endif
  {
    LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("netif was down for given ifindex (%u)\n", ifindex));
    return ERR_NETDOWN;
  }

  if ((p->tot_len - (SIZEOF_ETH_HDR - ETH_PAD_SIZE)) > netif->mtu) {
    LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("Message too long (%u)\n", p->tot_len));
    return ERR_MSGSIZE;
  }

  if (pbuf_header(p, ETH_PAD_SIZE) == 0) {
    p->flags = (u16_t)(p->flags & ~(PBUF_FLAG_LLMCAST | PBUF_FLAG_LLBCAST | PBUF_FLAG_HOST));
    p->flags |= PBUF_FLAG_OUTGOING;
    raw_pkt_input(p, netif, pcb);
    (void)pbuf_header(p, -ETH_PAD_SIZE);
  }

  /* For RAW packets of PF_PACKET family, do not modify the packets as it is
     already supposed to contain the link layer header. So send directly to the driver */
  netif->drv_send(netif, p);
  LINK_STATS_INC(link.xmit);
  return ERR_OK;
}
#endif

/**
 * @ingroup raw_raw
 * Send the raw IP packet to the given address. An IP header will be prepended
 * to the packet, unless the RAW_FLAGS_HDRINCL flag is set on the PCB. In that
 * case, the packet must include an IP header, which will then be sent as is.
 *
 * @param pcb the raw pcb which to send
 * @param p the IP payload to send
 * @param ipaddr the destination address of the IP packet
 *
 */
err_t
raw_sendto(struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *ipaddr)
{
  struct netif *netif;
  const ip_addr_t *src_ip;

  if ((pcb == NULL) || (ipaddr == NULL) || !IP_ADDR_PCB_VERSION_MATCH(pcb, ipaddr)) {
    return ERR_VAL;
  }

  LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("raw_sendto\n"));

  if (pcb->netif_idx != NETIF_NO_INDEX) {
    netif = netif_get_by_index(pcb->netif_idx);
  } else {
#if LWIP_MULTICAST_TX_OPTIONS
    netif = NULL;
    if (ip_addr_ismulticast(ipaddr)) {
      /* For multicast-destined packets, use the user-provided interface index to
       * determine the outgoing interface, if an interface index is set and a
       * matching netif can be found. Otherwise, fall back to regular routing. */
      netif = netif_get_by_index(pcb->mcast_ifindex);
    }

    if (netif == NULL)
#endif /* LWIP_MULTICAST_TX_OPTIONS */
    {
      netif = ip_route_pcb(ipaddr, (struct ip_pcb*)pcb);
    }
  }

  if (netif == NULL) {
    LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_LEVEL_WARNING, ("raw_sendto: No route to "));
    ip_addr_debug_print(RAW_DEBUG | LWIP_DBG_LEVEL_WARNING, ipaddr);
    return ERR_RTE;
  }

  if (ip_addr_isany(&pcb->local_ip) || ip_get_option(pcb, SOF_BINDNONUNICAST)) {
    /* use outgoing network interface IP address as source address */
    src_ip = ip_netif_get_local_ip(netif, ipaddr);
#if LWIP_IPV6
    if (src_ip == NULL) {
      return ERR_RTE;
    }
#endif /* LWIP_IPV6 */
  } else {
    /* use RAW PCB local IP address as source address */
    src_ip = &pcb->local_ip;
  }

  return raw_sendto_if_src(pcb, p, ipaddr, netif, src_ip);
}

/**
 * @ingroup raw_raw
 * Send the raw IP packet to the given address, using a particular outgoing
 * netif and source IP address. An IP header will be prepended to the packet,
 * unless the RAW_FLAGS_HDRINCL flag is set on the PCB. In that case, the
 * packet must include an IP header, which will then be sent as is.
 *
 * @param pcb RAW PCB used to send the data
 * @param p chain of pbufs to be sent
 * @param dst_ip destination IP address
 * @param netif the netif used for sending
 * @param src_ip source IP address
 */
err_t
raw_sendto_if_src(struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *dst_ip,
                  struct netif *netif, const ip_addr_t *src_ip)
{
  err_t err;
  struct pbuf *q; /* q will be sent down the stack */
  u16_t header_size;
  u8_t ttl;

  LWIP_ASSERT_CORE_LOCKED();

  if ((pcb == NULL) || (dst_ip == NULL) || (netif == NULL) || (src_ip == NULL) ||
      !IP_ADDR_PCB_VERSION_MATCH(pcb, src_ip) || !IP_ADDR_PCB_VERSION_MATCH(pcb, dst_ip)) {
    return ERR_VAL;
  }

#if LWIP_IPV4 && IP_SOF_BROADCAST
  /* broadcast filter? */
  if (!ip_get_option(pcb, SOF_BROADCAST) &&
#if LWIP_IPV6
      IP_IS_V4(dst_ip) &&
#endif /* LWIP_IPV6 */
      ip_addr_isbroadcast(dst_ip, netif)) {
    LWIP_DEBUGF(UDP_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                ("raw_sendto: SOF_BROADCAST not enabled on pcb %p\n", (void *)pcb));
    return ERR_ACCESS;
  }
#endif /* LWIP_IPV4 && IP_SOF_BROADCAST */

  header_size = (
#if LWIP_IPV4 && LWIP_IPV6
                  IP_IS_V6(dst_ip) ? IP6_HLEN : IP_HLEN);
#elif LWIP_IPV4
                  IP_HLEN);
#else
                  IP6_HLEN);
#endif

  /* Handle the HDRINCL option as an exception: none of the code below applies
   * to this case, and sending the packet needs to be done differently too. */
  if (pcb->flags & RAW_FLAGS_HDRINCL) {
    if (netif->mtu && (p->tot_len > netif->mtu)) {
      return ERR_VAL;
    }

    /* A full header *must* be present in the first pbuf of the chain, as the
     * output routines may access its fields directly. */
    if (p->len < header_size) {
      return ERR_VAL;
    }

  /* Multicast Loop? */
#if LWIP_MULTICAST_TX_OPTIONS
    if (((pcb->flags & RAW_FLAGS_MULTICAST_LOOP) != 0) && ip_addr_ismulticast(dst_ip)) {
      p->flags |= PBUF_FLAG_MCASTLOOP;
    }
#endif /* LWIP_MULTICAST_TX_OPTIONS */

#if LWIP_SO_DONTROUTE
    if (ip_get_option((struct ip_pcb *)pcb, SOF_DONTROUTE)) {
      p->flags |= PBUF_FLAG_IS_LINK_ONLY;
    }
#endif /* LWIP_SO_DONTROUTE */

    if (IP_IS_V4(dst_ip)) {
      struct ip_hdr *iphdr = (struct ip_hdr *)p->payload;
      iphdr->dest.addr = ip_2_ip4(dst_ip)->addr;
    }
#if LWIP_IPV6
    else {
      struct ip6_hdr *ip6hdr = (struct ip6_hdr *)p->payload;
      ip6_addr_copy_to_packed(ip6hdr->dest, *ip_2_ip6(dst_ip));
    }
#endif
    /* @todo multicast loop support, if at all desired for this scenario.. */
    NETIF_SET_HINTS(netif, &pcb->netif_hints);
    err = ip_output_if_hdrincl(p, src_ip, dst_ip, netif);
    NETIF_RESET_HINTS(netif);
    return err;
  }

  /* packet too large to add an IP header without causing an overflow? */
  if ((u16_t)(p->tot_len + header_size) < p->tot_len) {
    return ERR_MEM;
  }
  /* not enough space to add an IP header to first pbuf in given p chain? */
  if (pbuf_add_header(p, header_size)) {
    /* allocate header in new pbuf */
    q = pbuf_alloc(PBUF_IP, 0, PBUF_RAM);
    /* new header pbuf could not be allocated? */
    if (q == NULL) {
      LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("raw_sendto: could not allocate header\n"));
      return ERR_MEM;
    }
    if (p->tot_len != 0) {
#if LWIP_SO_PRIORITY
      q->priority = p->priority;
#endif /* LWIP_SO_PRIORITY */
      /* chain header q in front of given pbuf p */
      pbuf_chain(q, p);
    }
    /* { first pbuf q points to header pbuf } */
    LWIP_DEBUGF(RAW_DEBUG, ("raw_sendto: added header pbuf %p before given pbuf %p\n", (void *)q, (void *)p));
  } else {
    /* first pbuf q equals given pbuf */
    q = p;
    if (pbuf_remove_header(q, header_size)) {
      LWIP_ASSERT("Can't restore header we just removed!", 0);
      return ERR_MEM;
    }
  }

#if IP_SOF_BROADCAST
  if (IP_IS_V4(dst_ip)) {
    /* broadcast filter? */
    if (!ip_get_option(pcb, SOF_BROADCAST) && ip_addr_isbroadcast(dst_ip, netif)) {
      LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_LEVEL_WARNING, ("raw_sendto: SOF_BROADCAST not enabled on pcb %p\n", (void *)pcb));
      /* free any temporary header pbuf allocated by pbuf_header() */
      if (q != p) {
        pbuf_free(q);
      }
      return ERR_VAL;
    }
  }
#endif /* IP_SOF_BROADCAST */

  /* Multicast Loop? */
#if LWIP_MULTICAST_TX_OPTIONS
  if (((pcb->flags & RAW_FLAGS_MULTICAST_LOOP) != 0) && ip_addr_ismulticast(dst_ip)) {
    q->flags |= PBUF_FLAG_MCASTLOOP;
  }
#endif /* LWIP_MULTICAST_TX_OPTIONS */

#if LWIP_SO_DONTROUTE
  if (ip_get_option((struct ip_pcb *)pcb, SOF_DONTROUTE)) {
    q->flags |= PBUF_FLAG_IS_LINK_ONLY;
  }
#endif /* LWIP_SO_DONTROUTE */

#if LWIP_IPV6
  /* If requested, based on the IPV6_CHECKSUM socket option per RFC3542,
     compute the checksum and update the checksum in the payload. */
  if (IP_IS_V6(dst_ip) && pcb->chksum_reqd) {
    if (p->len >= (pcb->chksum_offset + LWIP_IPV6_CHKSUM_LEN)) {
      u16_t chksum;
      switch (pcb->raw_proto) {
        case IP6_NEXTH_ICMP6:
          if (pcb->chksum_offset != IPV6_ICMP_CHKSUM_OFFSET) {
            err = ERR_VAL;
            LWIP_DEBUGF(RAW_DEBUG,
                        ("raw_sendto: chksum offset = %"U16_F" value not matching to ICMP6 proto chksum offset = %"U16_F" \n",
                         pcb->chksum_offset, IPV6_ICMP_CHKSUM_OFFSET));
            goto check_and_free_header;
          }
          break;
        case IP6_NEXTH_UDP:
          if (pcb->chksum_offset != IPV6_UDP_CHKSUM_OFFSET) {
            err = ERR_VAL;
            LWIP_DEBUGF(RAW_DEBUG,
                        ("raw_sendto: chksum offset = %"U16_F" value not matching to UDP proto chksum offset = %"U16_F" \n",
                         pcb->chksum_offset, IPV6_UDP_CHKSUM_OFFSET));
            goto check_and_free_header;
          }
          break;
        case IP6_NEXTH_TCP:
          if (pcb->chksum_offset != IPV6_TCP_CHKSUM_OFFSET) {
            err = ERR_VAL;
            LWIP_DEBUGF(RAW_DEBUG,
                        ("raw_sendto: chksum offset = %"U16_F" value not matching to TCP proto chksum offset = %"U16_F" \n",
                         pcb->chksum_offset, IPV6_TCP_CHKSUM_OFFSET));
            goto check_and_free_header;
          }
          break;
        default:
          /* default proto will have to processed and the offset need to added */
          if (q != p) {
            (void)pbuf_free(q);
            return ERR_VAL;
          }
          break;
      }

      /* Clear the checksum field before inserting checksum */
      (void)memset_s(((u8_t *)p->payload) + pcb->chksum_offset, sizeof(u16_t), 0, sizeof(u16_t));
      chksum = ip6_chksum_pseudo(p, pcb->raw_proto, p->tot_len, ip_2_ip6(src_ip), ip_2_ip6(dst_ip));
      LWIP_ASSERT("Checksum must fit into first pbuf", p->len >= (pcb->chksum_offset + RAW_CHKSUM_OFFSET));
      SMEMCPY(((u8_t *)p->payload) + pcb->chksum_offset, &chksum, sizeof(u16_t));
    } else {
      LWIP_DEBUGF(RAW_DEBUG,
                  ("raw_sendto: chksum offset = %"U16_F" value is not within the packet length = %"U16_F" \n",
                   pcb->chksum_offset, p->len));
      err = ERR_VAL;
      goto check_and_free_header;
    }
  }
#endif
#if LWIP_SO_PRIORITY
  q->priority = pcb->priority;
#endif /* LWIP_SO_PRIORITY */

  /* Determine TTL to use */
#if LWIP_MULTICAST_TX_OPTIONS
  ttl = (ip_addr_ismulticast(dst_ip) ? raw_get_multicast_ttl(pcb) : pcb->ttl);
#else /* LWIP_MULTICAST_TX_OPTIONS */
  ttl = pcb->ttl;
#endif /* LWIP_MULTICAST_TX_OPTIONS */

  NETIF_SET_HINTS(netif, &pcb->netif_hints);

  err = ip_output_if(q, src_ip, dst_ip, ttl, pcb->tos, pcb->raw_proto, netif);
  NETIF_RESET_HINTS(netif);

#if LWIP_IPV6
check_and_free_header:
#endif
  /* did we chain a header earlier? */
  if (q != p) {
    /* free the header */
    (void)pbuf_free(q);
  }
  return err;
}

/**
 * @ingroup raw_raw
 * Send the raw IP packet to the address given by raw_connect()
 *
 * @param pcb the raw pcb which to send
 * @param p the IP payload to send
 *
 */
err_t
raw_send(struct raw_pcb *pcb, struct pbuf *p)
{
  return raw_sendto(pcb, p, &pcb->remote_ip);
}

/**
 * @ingroup raw_raw
 * Remove an RAW PCB.
 *
 * @param pcb RAW PCB to be removed. The PCB is removed from the list of
 * RAW PCB's and the data structure is freed from memory.
 *
 * @see raw_new()
 */
void
raw_remove(struct raw_pcb *pcb)
{
  struct raw_pcb *pcb2;
  LWIP_ASSERT_CORE_LOCKED();
  /* pcb to be removed is first in list? */
  if (raw_pcbs == pcb) {
    /* make list start at 2nd pcb */
    raw_pcbs = raw_pcbs->next;
    /* pcb not 1st in list */
  } else {
    for (pcb2 = raw_pcbs; pcb2 != NULL; pcb2 = pcb2->next) {
      /* find pcb in raw_pcbs list */
      if (pcb2->next != NULL && pcb2->next == pcb) {
        /* remove pcb from list */
        pcb2->next = pcb->next;
        break;
      }
    }
  }
  memp_free(MEMP_RAW_PCB, pcb);
}

/**
 * @ingroup raw_raw
 * Create a RAW PCB.
 *
 * @return The RAW PCB which was created. NULL if the PCB data structure
 * could not be allocated.
 *
 * @param proto the protocol number of the IPs payload (e.g. IP_PROTO_ICMP)
 *
 * @see raw_remove()
 */
struct raw_pcb *
raw_new(u8_t proto)
{
  struct raw_pcb *pcb;

  LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("raw_new\n"));
  LWIP_ASSERT_CORE_LOCKED();

  pcb = (struct raw_pcb *)memp_malloc(MEMP_RAW_PCB);
  /* could allocate RAW PCB? */
  if (pcb != NULL) {
    /* initialize PCB to all zeroes */
    memset(pcb, 0, sizeof(struct raw_pcb));
#if PF_PKT_SUPPORT
    pcb->proto.protocol = proto;
#else
    pcb->protocol = proto;
#endif

    pcb->ttl = RAW_TTL;
#if LWIP_MULTICAST_TX_OPTIONS
    raw_set_multicast_ttl(pcb, RAW_TTL);
#endif /* LWIP_MULTICAST_TX_OPTIONS */
    pcb->next = raw_pcbs;
    raw_pcbs = pcb;
  }
  return pcb;
}

#if PF_PKT_SUPPORT
/*
 * Create a RAW PCB for Packet family.
 *
 * @return The RAW PCB which was created. NULL if the PCB data structure
 * could not be allocated.
 *
 * @param proto the protocol number of the IPs payload (e.g. IP_PROTO_ICMP)
 *
 * @see raw_remove()
 */
struct raw_pcb *
raw_pkt_new(u16_t proto)
{
  struct raw_pcb *pcb = NULL;

  LWIP_DEBUGF(RAW_DEBUG | LWIP_DBG_TRACE, ("raw_pkt_new\n"));

  pcb = (struct raw_pcb *)memp_malloc(MEMP_RAW_PCB);
  /* could allocate RAW PCB? */
  if (pcb != NULL) {
    /* initialize PCB to all zeroes */
    (void)memset(pcb, 0, sizeof(struct raw_pcb));
    pcb->proto.eth_proto = proto;
    pcb->ttl = RAW_TTL;
    pcb->next = pkt_raw_pcbs;
    pkt_raw_pcbs = pcb;

    if (proto == htons(ETHTYPE_ALL)) {
      pcb->all_next = all_pkt_raw_pcbs;
      all_pkt_raw_pcbs = pcb;
    }

#if LWIP_NETIF_PROMISC
    netif_start_promisc_mode(pcb->netifindex);
#endif
  }
  return pcb;
}

/*
 * Remove an RAW PCB of packet family type
 *
 * @param pcb RAW PCB to be removed. The PCB is removed from the list of
 * RAW PCB's and the data structure is freed from memory.
 *
 * @see raw_pkt_new()
 */
void
raw_pkt_remove(struct raw_pcb *pcb)
{
  struct raw_pcb *pcb2 = NULL;

  /* NULL check */
  if (pcb == NULL) {
    return;
  }

  /* pcb to be removed is first in all_pkt list? */
  if (all_pkt_raw_pcbs == pcb) {
    /* make list start at 2nd pcb */
    all_pkt_raw_pcbs = all_pkt_raw_pcbs->all_next;
    /* pcb not 1st in list */
  } else {
    for (pcb2 = all_pkt_raw_pcbs; pcb2 != NULL; pcb2 = pcb2->all_next) {
      /* find pcb in all_pkt_raw_pcbs list */
      if (pcb2->all_next == pcb) {
        /* remove pcb from list */
        pcb2->all_next = pcb->all_next;
      }
    }
  }

  /* pcb to be removed is first in list? */
  if (pkt_raw_pcbs == pcb) {
    /* make list start at 2nd pcb */
    pkt_raw_pcbs = pkt_raw_pcbs->next;
    /* pcb not 1st in list */
  } else {
    for (pcb2 = pkt_raw_pcbs; pcb2 != NULL; pcb2 = pcb2->next) {
      /* find pcb in raw_pcbs list */
      if (pcb2->next == pcb) {
        /* remove pcb from list */
        pcb2->next = pcb->next;
      }
    }
  }

#if LWIP_NETIF_PROMISC
  netif_stop_promisc_mode(pcb->netifindex);
#endif  /* LWIP_NETIF_PROMISC */
  memp_free(MEMP_RAW_PCB, pcb);
}

#if LWIP_NETIF_PROMISC
/* provides the count of pkt_raw_pcbs using this netif */
u8_t pkt_raw_pcbs_using_netif(u8_t ifindex)
{
  struct raw_pcb *pcb = NULL;
  u8_t count = 0;

  for (pcb = pkt_raw_pcbs; pcb != NULL; pcb = pcb->next) {
    /* check for without bind and netif binded pakcet raw sockets */
    if (!pcb->netifindex || pcb->netifindex == ifindex) {
      count++;
    }
  }
  return count;
}
#endif /* LWIP_NETIF_PROMISC */
#endif /* PF_PKT_SUPPORT */

/**
 * @ingroup raw_raw
 * Create a RAW PCB for specific IP type.
 *
 * @return The RAW PCB which was created. NULL if the PCB data structure
 * could not be allocated.
 *
 * @param type IP address type, see @ref lwip_ip_addr_type definitions.
 * If you want to listen to IPv4 and IPv6 (dual-stack) packets,
 * supply @ref IPADDR_TYPE_ANY as argument and bind to @ref IP_ANY_TYPE.
 * @param proto the protocol number (next header) of the IPv6 packet payload
 *              (e.g. IP6_NEXTH_ICMP6)
 *
 * @see raw_remove()
 */
struct raw_pcb *
raw_new_ip_type(u8_t type, u8_t proto)
{
  struct raw_pcb *pcb;
  LWIP_ASSERT_CORE_LOCKED();
  pcb = raw_new(proto);
#if LWIP_IPV4 && LWIP_IPV6
  if (pcb != NULL) {
    IP_SET_TYPE_VAL(pcb->local_ip,  type);
    IP_SET_TYPE_VAL(pcb->remote_ip, type);
  }
#else /* LWIP_IPV4 && LWIP_IPV6 */
  LWIP_UNUSED_ARG(type);
#endif /* LWIP_IPV4 && LWIP_IPV6 */
  return pcb;
}

/** This function is called from netif.c when address is changed
 *
 * @param old_addr IP address of the netif before change
 * @param new_addr IP address of the netif after change
 */
void raw_netif_ip_addr_changed(const ip_addr_t *old_addr, const ip_addr_t *new_addr)
{
  struct raw_pcb *rpcb;

  if (!ip_addr_isany(old_addr) && !ip_addr_isany(new_addr)) {
    for (rpcb = raw_pcbs; rpcb != NULL; rpcb = rpcb->next) {
      /* PCB bound to current local interface address? */
      if (ip_addr_cmp(&rpcb->local_ip, old_addr)) {
        /* The PCB is bound to the old ipaddr and
         * is set to bound to the new one instead */
        ip_addr_copy(rpcb->local_ip, *new_addr);
      }
    }
  }
}

#endif /* LWIP_RAW */
