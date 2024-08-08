/**
 * @file
 *
 * IPv6 layer.
 */

/*
 * Copyright (c) 2010 Inico Technologies Ltd.
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
 * Author: Ivan Delamer <delamer@inicotech.com>
 *
 *
 * Please coordinate changes and requests with Ivan Delamer
 * <delamer@inicotech.com>
 */

#include "lwip/opt.h"

#if LWIP_IPV6  /* don't build if not configured for use in lwipopts.h */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/ip6_frag.h"
#include "lwip/icmp6.h"
#include "lwip/priv/raw_priv.h"
#include "lwip/udp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/dhcp6.h"
#include "lwip/nd6.h"
#include "lwip/mld6.h"
#include "lwip/debug.h"
#include "lwip/stats.h"

#if LWIP_NAT64
#include "lwip/nat64.h"
#endif

#if LWIP_RIPPLE
#include "lwip/lwip_rpl.h"
#endif

#if LWIP_IP6IN4
#include "lwip/ip6in4.h"
#endif
#if LWIP_MPL
#include "mcast6.h"
#ifndef MCAST6_IS_FROM_CONN_PEER
#define MCAST6_IS_FROM_CONN_PEER mcast6_esmrf_from_conn_peer
#endif /* MCAST6_IS_FROM_CONN_PEER */

#ifndef MCAST6_FORWARD
#define MCAST6_FORWARD mcast6_esmrf_in
#endif /* MCAST6_FORWARD */
#endif /* LWIP_MPL */

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif

#if LWIP_IPV6_FILTER
/* This function is called when an ip packet received. The return value of this function will
 * decide accept or drop a ip packet when LWIP_IPV6_FILTER is Enabled and ip_filter function has been set
 */
static ip_filter_fn ip6_filter = NULL;
#endif /* LWIP_IPV6_FILTER */

#ifndef LWIP_RPI_LEN
#define LWIP_RPI_LEN 6
#endif

#if LWIP_RIPPLE
typedef struct {
  u32_t route_up:1;
  u32_t next_hop_nonmesh:1;
} pkt_rte_status;
static pkt_rte_status g_pkt_rte_stat;

int
lwip_get_pkt_route_status(void)
{
  return (int)g_pkt_rte_stat.route_up;
}

void
lwip_set_pkt_route_status(int up)
{
  g_pkt_rte_stat.route_up = !!up;
}

int
lwip_get_rte_nexthop_nonmesh(void)
{
  return (int)g_pkt_rte_stat.next_hop_nonmesh;
}

void
lwip_set_rte_nexthop_nonmesh(int stat)
{
  g_pkt_rte_stat.next_hop_nonmesh = !!stat;
}

/*
 * Hardcoded lenghth of HBH,
 * providing function for future enhancement.
 */
u16_t
lwip_hbh_len(struct pbuf *p)
{
  u16_t hbh_len = 0;
  /* pbuf alloc will pass NULL as pbuf. */
  if (p == NULL) {
  /* 2: add for 8 byte Aligned */
    hbh_len = IP6_HBH_HLEN + LWIP_RPI_LEN;
  }

  return hbh_len;
}

static err_t
ip6_process_hbh_exth_options(struct pbuf *p, const struct netif *iface)
{
  u16_t ext_len;
  u16_t hbh_opt_offset = IP6_OPT_HLEN; /* skipping first 2 header bytes */
  u16_t tmp_hbh_opt_offset; /* Used two iterator variables to check overflow. */
  const struct ip6_hdr *ip6hdr = (struct ip6_hdr *)((char *)(p->payload) - IP6_HLEN); /* for accessing dest */

  struct ip6_opt_hdr *opt_tlv = NULL;
  struct ip6_hbh_hdr  *ext_hdr = (struct ip6_hbh_hdr *)(p->payload);
  (void)iface;

  LWIP_DEBUGF(IP6_DEBUG, ("Next header is [%x]\n", IP6H_NEXTH(ip6hdr)));
  if (IP6H_NEXTH(ip6hdr) != IP6_NEXTH_HOPBYHOP) {
    return 0;
  }

  LWIP_DEBUGF(IP6_DEBUG, ("nexth %u, hlen %u\n", ext_hdr->_nexth, ext_hdr->_hlen));
#if LWIP_RIPPLE
  /* do not handle non mesh interface packet. */
  if (lwip_rpl_is_rpl_netif(iface) == lwIP_FALSE) {
    return ERR_OK;
  }
#endif
  if (p->len < IP6_HBH_HLEN) {
    LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                ("IP6_NEXTH_HOPBYHOP: pbuf (len %"U16_F") is less than 2.\n", p->len));
    /* free (drop) packet pbufs */
    IP6_STATS_INC(ip6.lenerr);
    return -1;
  }

  /*
   * max value can be stored in hbh_ext_len(8 bits) is 255
   * so max value after multilication and addition will be
   * ((255 * 8) + 8) : 2048 and local variable u16_t ext_len can
   * store 65535 implies -> No Overflow.
   */
  /* hbh hlen is saved in units of octets excluding first octet. */
  ext_len = (ext_hdr->_hlen << 3) + 8;

  /* Validate the received IP packet length gainst the hbh total length */
  if (ext_len > p->len) {
    LWIP_ERROR("Received malformed packet\n", 0, return -1);
  }

  while (hbh_opt_offset < ext_len) {
    opt_tlv = (struct ip6_opt_hdr *)((char *)(p->payload) + hbh_opt_offset);
    switch (opt_tlv->_opt_type) {
      case LWIP_EXT_HDR_OPT_PAD1:
        LWIP_DEBUGF(IP6_DEBUG, ("processing PAD1 option\n"));
        tmp_hbh_opt_offset = hbh_opt_offset + 1;
        LWIP_ERROR("overflow occured in ext len\n", tmp_hbh_opt_offset > hbh_opt_offset, return -1);
        hbh_opt_offset = tmp_hbh_opt_offset;

        break;
      case LWIP_EXT_HDR_OPT_PADN:
        LWIP_DEBUGF(IP6_DEBUG, ("processing PADN option\n"));
        tmp_hbh_opt_offset = (u16_t)(hbh_opt_offset + IP6_HBH_HLEN + opt_tlv->_opt_dlen);
        LWIP_ERROR("overflow occured in ext len\n", tmp_hbh_opt_offset > hbh_opt_offset, return -1);
        hbh_opt_offset = tmp_hbh_opt_offset;
        break;
      case LWIP_RPL_RPI_TYPE_NEW:
      case LWIP_RPL_RPI_TYPE_OLD:
        /*
         * Fixes situation when a node that is not using RPL
         * joins a network which does. The received packages will include the
         * RPL header and processed by the "default" case of the switch
         * (0x63 & 0xC0 = 0x40). Hence, the packet is discarded as the header
         * is considered invalid.
         * Using this fix, the header is ignored, and the next header (if
         * present) is processed.
         */
#if LWIP_RIPPLE
        LWIP_DEBUGF(IP6_DEBUG, ("Processing RPL opt\n"));
        if (lwip_verify_rplext_header((void *)p, hbh_opt_offset)) {
          LWIP_ERROR("RPL Opt Error: Dropping Pkt\n", 0, ;);
          return -1;
        }
#endif
        tmp_hbh_opt_offset = (u16_t)(hbh_opt_offset + IP6_HBH_HLEN + opt_tlv->_opt_dlen);
        LWIP_ERROR("overflow occured in ext len\n", tmp_hbh_opt_offset > hbh_opt_offset, return -1);
        hbh_opt_offset = tmp_hbh_opt_offset;
        p->flags |= PBUF_FLAG_RPI;
        /* Should we return or process rest of the options */
        return 0;
      default:
        /*
         * check the two highest order bits of the option
         * - 00 skip over this option and continue processing the header.
         * - 01 discard the packet.
         * - 10 discard the packet and, regardless of whether or not the
         * - packet's Destination Address was a multicast address, send an
         * - ICMP Parameter Problem, Code 2, message to the packet's
         * - Source Address, pointing to the unrecognized Option Type.
         * - 11 discard the packet and, only if the packet's Destination
         * - Address was not a multicast address, send an ICMP Parameter
         * - Problem, Code 2, message to the packet's Source Address,
         * - pointing to the unrecognized Option Type.
         */
        LWIP_DEBUGF(IP6_DEBUG, ("MSB %x\n", opt_tlv->_opt_type));
        switch (opt_tlv->_opt_type & 0xC0) {
          case 0:
            break;
          case 0x40:
            return -1;
          case 0xC0:
            if (ip6_addr_ismulticast(&ip6hdr->dest)) {
              return -1;
            }
          /* fall-through */
          case 0x80:
            icmp6_param_problem(p, ICMP6_PP_OPTION, opt_tlv);
            return -1;
          default:
            break;
        }

        tmp_hbh_opt_offset = (u16_t)(hbh_opt_offset + IP6_HBH_HLEN + opt_tlv->_opt_dlen);
        LWIP_ERROR("overflow occured in ext len\n", tmp_hbh_opt_offset > hbh_opt_offset, return -1);
        hbh_opt_offset = tmp_hbh_opt_offset;
        break;
    }
  }

  return ERR_OK;
}

#ifdef LWIP_HOOK_IP6_ROUTE
static err_t
lwip_rpl_same_prefix(const ip6_addr_t *dest)
{
  ip6_addr_t prefix;
  uint8_t len;
  err_t ret;

  ret = lwip_rpl_get_default_prefix(&prefix, &len);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }
  /* now our prefix len is 64, this will be ok. */
  if (memcmp(dest, &prefix, (len >> 3)) != 0) {
    return ERR_VAL;
  }
  return ERR_OK;
}
#endif
#endif /* LWIP_RIPPLE */
/**
 * Finds the appropriate network interface for a given IPv6 address. It tries to select
 * a netif following a sequence of heuristics:
 * 1) if there is only 1 netif, return it
 * 2) if the destination is a zoned address, match its zone to a netif
 * 3) if the either the source or destination address is a scoped address,
 *    match the source address's zone (if set) or address (if not) to a netif
 * 4) tries to match the destination subnet to a configured address
 * 5) tries to find a router-announced route
 * 6) tries to match the (unscoped) source address to the netif
 * 7) returns the default netif, if configured
 *
 * Note that each of the two given addresses may or may not be properly zoned.
 *
 * @param src the source IPv6 address, if known
 * @param dest the destination IPv6 address for which to find the route
 * @return the netif on which to send to reach dest
 */
struct netif *
ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest)
{
#if LWIP_SINGLE_NETIF
  LWIP_UNUSED_ARG(src);
  LWIP_UNUSED_ARG(dest);
#else /* LWIP_SINGLE_NETIF */
  struct netif *netif;
  s8_t i;

  LWIP_ASSERT_CORE_LOCKED();

#if !LWIP_SO_DONTROUTE
  /* If single netif configuration, fast return. */
  if ((netif_list != NULL) && (netif_list->next == NULL)) {
    if (!netif_is_up(netif_list) || !netif_is_link_up(netif_list) ||
        (ip6_addr_has_zone(dest) && !ip6_addr_test_zone(dest, netif_list))) {
      return NULL;
    }
    return netif_list;
  }
#endif

#if LWIP_IPV6_SCOPES
  /* Special processing for zoned destination addresses. This includes link-
   * local unicast addresses and interface/link-local multicast addresses. Use
   * the zone to find a matching netif. If the address is not zoned, then there
   * is technically no "wrong" netif to choose, and we leave routing to other
   * rules; in most cases this should be the scoped-source rule below. */
  if (ip6_addr_has_zone(dest)) {
    IP6_ADDR_ZONECHECK(dest);
    /* Find a netif based on the zone. For custom mappings, one zone may map
     * to multiple netifs, so find one that can actually send a packet. */
    NETIF_FOREACH(netif) {
      if (ip6_addr_test_zone(dest, netif) &&
          netif_is_up(netif) && netif_is_link_up(netif)) {
        NETIF_SET_SCOPE(netif, RT_SCOPE_LINK);
        return netif;
      }
    }
    /* No matching netif found. Do no try to route to a different netif,
     * as that would be a zone violation, resulting in any packets sent to
     * that netif being dropped on output. */
    return NULL;
  }
#endif /* LWIP_IPV6_SCOPES */

  /* Special processing for scoped source and destination addresses. If we get
   * here, the destination address does not have a zone, so either way we need
   * to look at the source address, which may or may not have a zone. If it
   * does, the zone is restrictive: there is (typically) only one matching
   * netif for it, and we should avoid routing to any other netif as that would
   * result in guaranteed zone violations. For scoped source addresses that do
   * not have a zone, use (only) a netif that has that source address locally
   * assigned. This case also applies to the loopback source address, which has
   * an implied link-local scope. If only the destination address is scoped
   * (but, again, not zoned), we still want to use only the source address to
   * determine its zone because that's most likely what the user/application
   * wants, regardless of whether the source address is scoped. Finally, some
   * of this story also applies if scoping is disabled altogether. */
#if LWIP_IPV6_SCOPES
  if (ip6_addr_has_scope(dest, IP6_UNKNOWN) ||
      ip6_addr_has_scope(src, IP6_UNICAST) ||
#else /* LWIP_IPV6_SCOPES */
  if (ip6_addr_islinklocal(dest) || ip6_addr_ismulticast_iflocal(dest) ||
      ip6_addr_ismulticast_linklocal(dest) || ip6_addr_islinklocal(src) ||
#endif /* LWIP_IPV6_SCOPES */
      ip6_addr_isloopback(src)) {
#if LWIP_IPV6_SCOPES
    if (ip6_addr_has_zone(src)) {
      /* Find a netif matching the source zone (relatively cheap). */
      NETIF_FOREACH(netif) {
        if (netif_is_up(netif) && netif_is_link_up(netif) &&
            ip6_addr_test_zone(src, netif)) {
          NETIF_SET_SCOPE(netif, RT_SCOPE_LINK);
          return netif;
        }
      }
    } else
#endif /* LWIP_IPV6_SCOPES */
    {
      if (ip6_addr_isany(src)) {
        /* Use default netif, if Up. */
        if (netif_default == NULL || !netif_is_up(netif_default) ||
            !netif_is_link_up(netif_default)) {
          return NULL;
        }
        return netif_default;
      }
      /* Find a netif matching the source address (relatively expensive). */
      NETIF_FOREACH(netif) {
        if (!netif_is_up(netif) || !netif_is_link_up(netif)) {
          continue;
        }
        for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
          if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
              ip6_addr_cmp_zoneless(src, netif_ip6_addr(netif, i))) {
            NETIF_SET_SCOPE(netif, RT_SCOPE_HOST);
            return netif;
          }
        }
      }
    }
    /* Again, do not use any other netif in this case, as that could result in
     * zone boundary violations. */
    return NULL;
  }

  /* We come here only if neither source nor destination is scoped. */
  IP6_ADDR_ZONECHECK(src);

#ifdef LWIP_HOOK_IP6_ROUTE
  netif = LWIP_HOOK_IP6_ROUTE(src, dest);
  if (netif != NULL) {
    NETIF_SET_SCOPE(netif, RT_SCOPE_UNIVERSAL);
    return netif;
  }
#endif

#if LWIP_RIPPLE
  u8_t dest_same_prefix = lwIP_FALSE;
  if (lwip_rpl_same_prefix(dest) == ERR_OK) {
    dest_same_prefix = lwIP_TRUE;
  }

  /* for the mbr node, first to check the default route */
  netif = nd6_find_route(dest);
  if ((netif != NULL) && netif_is_up(netif) && netif_is_link_up(netif) &&
      (dest_same_prefix == lwIP_TRUE) && (lwip_rpl_is_br() == lwIP_TRUE) &&
      (lwip_rpl_is_rpl_netif(netif) == lwIP_FALSE)) {
    return netif;
  }

  if ((netif_default != NULL) && netif_is_up(netif_default) && netif_is_link_up(netif_default) &&
      (dest_same_prefix == lwIP_TRUE) && (lwip_rpl_is_br() == lwIP_TRUE) &&
      (lwip_rpl_is_rpl_netif(netif_default) == lwIP_FALSE)) {
    return netif_default;
  }
#endif /* LWIP_RIPPLE */


  /* See if the destination subnet matches a configured address. In accordance
   * with RFC 5942, dynamically configured addresses do not have an implied
   * local subnet, and thus should be considered /128 assignments. However, as
   * such, the destination address may still match a local address, and so we
   * still need to check for exact matches here. By (lwIP) policy, statically
   * configured addresses do always have an implied local /64 subnet. */
  NETIF_FOREACH(netif) {
    if (!netif_is_up(netif) || !netif_is_link_up(netif)) {
      continue;
    }
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_netcmp(dest, netif_ip6_addr(netif, i)) &&
          (netif_ip6_addr_isstatic(netif, i) ||
          ip6_addr_nethostcmp(dest, netif_ip6_addr(netif, i)))) {
        NETIF_SET_SCOPE(netif, RT_SCOPE_UNIVERSAL);
        return netif;
      }
    }
  }

  /* Get the netif for a suitable router-announced route. */
  netif = nd6_find_route(dest);
  if (netif != NULL) {
    NETIF_SET_SCOPE(netif, RT_SCOPE_UNIVERSAL);
    return netif;
  }

  /* Try with the netif that matches the source address. Given the earlier rule
   * for scoped source addresses, this applies to unscoped addresses only. */
  if (!ip6_addr_isany(src)) {
    NETIF_FOREACH(netif) {
      if (!netif_is_up(netif) || !netif_is_link_up(netif)) {
        continue;
      }
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
            ip6_addr_cmp(src, netif_ip6_addr(netif, i))) {
          NETIF_SET_SCOPE(netif, RT_SCOPE_HOST);
          return netif;
        }
      }
    }
  }

#if LWIP_NETIF_LOOPBACK && !LWIP_HAVE_LOOPIF
  /* loopif is disabled, loopback traffic is passed through any netif */
  if (ip6_addr_isloopback(dest)) {
    /* don't check for link on loopback traffic */
    if (netif_default != NULL && netif_is_up(netif_default)) {
#if LWIP_SO_DONTROUTE
      netif_default->scope = RT_SCOPE_HOST;
#endif
      return netif_default;
    }
    /* default netif is not up, just use any netif for loopback traffic */
    NETIF_FOREACH(netif) {
      if (netif_is_up(netif)) {
        NETIF_SET_SCOPE(netif, RT_SCOPE_HOST);
        return netif;
      }
    }
    return NULL;
  }
#endif /* LWIP_NETIF_LOOPBACK && !LWIP_HAVE_LOOPIF */
#endif /* !LWIP_SINGLE_NETIF */

  /* no matching netif found, use default netif, if up */
  if ((netif_default == NULL) || !netif_is_up(netif_default) || !netif_is_link_up(netif_default)) {
    return NULL;
  }
  NETIF_SET_SCOPE(netif_default, RT_SCOPE_UNIVERSAL);
  return netif_default;
}

/**
 * @ingroup ip6
 * Select the best IPv6 source address for a given destination
 * IPv6 address. Loosely follows RFC 3484. "Strong host" behavior
 * is assumed.
 *
 * @param addr1 IPv6 Address 1
 * @param addr2 IPv6 Address 2
 * @param prefix_length Length of network prefix(in bits)
 * @return
 * Number of common prefix bits in addr1 & addr2 : On success
 * -1 : Invalid parameters
 */
int ip6_common_prefix_length(const ip6_addr_t *addr1, const ip6_addr_t *addr2, u8_t prefix_length)
{
  int common_prefix_length = 0;
  int i = 0;
  int bit_index;
  u8_t *addr1_char = NULL;
  u8_t *addr2_char = NULL;

  LWIP_ERROR("ip6_common_prefix_length: addr1 != NULL", (addr1 != NULL), return -1);
  LWIP_ERROR("ip6_common_prefix_length: addr2 != NULL", (addr2 != NULL), return -1);

  if ((prefix_length < 1) || (prefix_length > IP6_ADDR_LEN)) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_common_prefix_length: Prefix length = %"X8_F": is invalid", prefix_length));
    return -1;
  }

  // Checking Word-By-Word
  while ((common_prefix_length < prefix_length) && (i < IP6_ADDR_U32_ARR_SIZE)) {
    if (addr1->addr[i] == addr2->addr[i]) {
      common_prefix_length += 32;  // u32_t occupy 32 bits
      ++i;
      continue;
    }
    break;
  }

  // If Already "prefix_length" number of bits are matched, then skip remaining part of the function
  if (common_prefix_length >= prefix_length) {
    goto exit;
  }

  // Checking Byte-By-Byte
  addr1_char = (u8_t *)(addr1);
  addr2_char = (u8_t *)(addr2);

  // Advancing i To Proper value so that it now indexes the next byte for comparison
  i *= 4;  // u32_t occupy 4 u8_t

  while ((common_prefix_length < prefix_length)) {
    if (addr1_char[i] == addr2_char[i]) {
      common_prefix_length += 8;  // u8_t occupy 8 bits
      ++i;
      continue;
    }
    break;
  }

  // If Already "prefix_length" number of bits are matched, then skip remaining part of the function
  if (common_prefix_length >= prefix_length) {
    goto exit;
  }

  // Checking Bit by Bit
  // bit_index is set to 7 so that the entire eight bits has to be checked
  bit_index = 7;

  // Checking whether the first nibble of the byte matches
  // This is done so as to reduce the number of bit-by-bit comparisons that may be done afterwards
  if ((u8_t)(addr1_char[i] >> 4) == (u8_t)(addr2_char[i] >> 4)) {
    common_prefix_length += 4;
    // bit_index is set to 3 so that only the last 4 bits has to be checked
    // (because the first 4 bits have already proved to be equal)
    bit_index = 3;
  }

  // If already "prefix_length" number of bits are matched, then skip remaining part of the function
  if (common_prefix_length >= prefix_length) {
    goto exit;
  }

  while (bit_index >= 0) {
    if ((u8_t)(addr1_char[i] >> (u32_t)bit_index) == (u8_t)(addr2_char[i] >> (u32_t)bit_index)) {
      ++common_prefix_length;
      --bit_index;
      continue;
    }
    break;
  }

exit:
  // Placing an upper bound so that the prefix length matched does not go beyond "prefix_length"
  common_prefix_length = LWIP_MIN(common_prefix_length, prefix_length);

  return common_prefix_length;
}

/**
 * @ingroup ip6
 * Select the best IPv6 source address for a given destination IPv6 address.
 *
 * This implementation follows RFC 6724 Sec. 5 to the following extent:
 * - Rules 1, 2, 3: fully implemented
 * - Rules 4, 5, 5.5: not applicable
 * - Rule 6: not implemented
 * - Rule 7: not applicable
 * - Rule 8: limited to "prefer /64 subnet match over non-match"
 *
 * For Rule 2, we deliberately deviate from RFC 6724 Sec. 3.1 by considering
 * ULAs to be of smaller scope than global addresses, to avoid that a preferred
 * ULA is picked over a deprecated global address when given a global address
 * as destination, as that would likely result in broken two-way communication.
 *
 * As long as temporary addresses are not supported (as used in Rule 7), a
 * proper implementation of Rule 8 would obviate the need to implement Rule 6.
 *
 * @param netif the netif on which to send a packet
 * @param dest the destination we are trying to reach (possibly not properly
 *             zoned)
 * @return the most suitable source address to use, or NULL if no suitable
 *         source address is found
 */
const ip_addr_t *
ip6_select_source_address(struct netif *netif, const ip6_addr_t *dest)
{
  const ip_addr_t *best_addr;
  const ip6_addr_t *cand_addr;
  s8_t dest_scope, cand_scope;
  s8_t best_scope = IP6_MULTICAST_SCOPE_RESERVED;
  u8_t i, cand_pref;
  u8_t best_pref = 0;
  int best_bits = 0;
  u8_t prefix_length = 64;
  int cand_bits;

  /* Start by determining the scope of the given destination address. These
   * tests are hopefully (roughly) in order of likeliness to match. */
  if (ip6_addr_isglobal(dest)) {
    dest_scope = IP6_MULTICAST_SCOPE_GLOBAL;
  } else if (ip6_addr_islinklocal(dest) || ip6_addr_isloopback(dest)) {
    dest_scope = IP6_MULTICAST_SCOPE_LINK_LOCAL;
  } else if (ip6_addr_isuniquelocal(dest)) {
    dest_scope = IP6_MULTICAST_SCOPE_ORGANIZATION_LOCAL;
  } else if (ip6_addr_ismulticast(dest)) {
    dest_scope = ip6_addr_multicast_scope(dest);
  } else if (ip6_addr_issitelocal(dest)) {
    dest_scope = IP6_MULTICAST_SCOPE_SITE_LOCAL;
  } else {
    /* no match, consider scope global */
    dest_scope = IP6_MULTICAST_SCOPE_RESERVEDF;
  }

  best_addr = NULL;

  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    /* Consider only valid (= preferred and deprecated) addresses. */
    if (!ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
      continue;
    }
    /* Determine the scope of this candidate address. Same ordering idea. */
    cand_addr = netif_ip6_addr(netif, i);
    if (ip6_addr_isglobal(cand_addr)) {
      cand_scope = IP6_MULTICAST_SCOPE_GLOBAL;
    } else if (ip6_addr_islinklocal(cand_addr)) {
      cand_scope = IP6_MULTICAST_SCOPE_LINK_LOCAL;
    } else if (ip6_addr_isuniquelocal(cand_addr)) {
      cand_scope = IP6_MULTICAST_SCOPE_ORGANIZATION_LOCAL;
    } else if (ip6_addr_issitelocal(cand_addr)) {
      cand_scope = IP6_MULTICAST_SCOPE_SITE_LOCAL;
    } else {
      /* no match, treat as low-priority global scope */
      cand_scope = IP6_MULTICAST_SCOPE_RESERVEDF;
    }
    cand_pref = ip6_addr_ispreferred(netif_ip6_addr_state(netif, i));
    cand_bits = ip6_common_prefix_length(cand_addr, dest, prefix_length);
    if (cand_bits == -1) {
      cand_bits = 0;
    }

    if ((best_addr == NULL) || /* no alternative yet */
        ((cand_scope < best_scope) && (cand_scope >= dest_scope)) ||
        ((cand_scope > best_scope) && (best_scope < dest_scope)) || /* Rule 2 */
        ((cand_scope == best_scope) && ((cand_pref > best_pref) || /* Rule 3 */
        ((cand_pref == best_pref) && (cand_bits > best_bits))))) { /* Rule 8 */
      /* We found a new "winning" candidate. */
      best_addr = netif_ip_addr6(netif, i);
      best_scope = cand_scope;
      best_pref = cand_pref;
      best_bits = cand_bits;
    }
  }

  return best_addr; /* may be NULL */
}

#if LWIP_IPV6_FORWARD
#if LWIP_RIPPLE
#define IP6_FORWARD_BIG_DROP 1
#define IP6_FORWARD_BIG_HANDLED 2
#define IP6_FORWARD_BIG_ASSEMBLED 3
static int
ip6_forward_big_packet(struct pbuf **q, struct ip6_hdr **oiphdr, const struct netif *outp, u8_t *big_packet)
{
  u16_t ip6_hdr_len;
  u8_t nexth;
  u8_t flags;
  u16_t optlen;
  struct pbuf *p = *q;
  struct ip6_hdr *iphdr = *oiphdr;

  /* send ICMP6 if HL == 0 */
  if ((IP6H_HOPLIM(iphdr) == 0) || ((IP6H_HOPLIM(iphdr) - 1) == 0)) {
#if LWIP_ICMP6
    /* Don't send ICMP messages in response to ICMP messages */
    if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {
      icmp6_time_exceeded(p, ICMP6_TE_HL);
    }
#endif /* LWIP_ICMP6 */
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward_big_packet:hop limit drop\n"));
    IP6_STATS_INC(ip6.drop);
    return IP6_FORWARD_BIG_DROP;
  }
  LWIP_UNUSED_ARG(outp);
  nexth = IP6H_NEXTH(iphdr);
  ip6_hdr_len = IP6_HLEN;

  /* find the data in ip6 pkt */
  /* Move to payload. */
  (void)pbuf_header(p, -IP6_HLEN);
  *big_packet = 1;
  flags = 0;
  /* Process option extension headers, if present. */
  while (nexth != IP6_NEXTH_NONE) {
    if ((nexth == IP6_NEXTH_TCP) || (nexth == IP6_NEXTH_UDP) ||
        (nexth == IP6_NEXTH_ICMP6) || (flags != 0)) {
      break;
    }

    switch (nexth) {
      case IP6_NEXTH_HOPBYHOP:
      case IP6_NEXTH_ENCAPS:
      case IP6_NEXTH_ROUTING:
      case IP6_NEXTH_DESTOPTS:
        /* Get next header type. */
        nexth = *((u8_t *)p->payload);

        if (p->len < IP6_HBH_HLEN) {
          LWIP_DEBUGF(IP6_DEBUG, ("pbuf (len %"U16_F") is less than 2.\n", p->len));
          IP_STATS_INC(ip6.drop);
          return IP6_FORWARD_BIG_DROP;
        }
        /* Get the header length. */
        /* 8: go to option length segment and multily by eight */
        optlen = 8 * (1 + (u16_t)*((u8_t *)p->payload + 1));

        /* Skip over this header. */
        if (optlen > p->len) {
          LWIP_DEBUGF(IP6_DEBUG,
                      ("IPv6 opt header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 pac dropped.\n",
                       optlen, p->len));
          IP_STATS_INC(ip6.drop);
          return IP6_FORWARD_BIG_DROP;
        }

        ip6_hdr_len += optlen;
        (void)pbuf_header(p, (s16_t)(-(s16_t)optlen));
        break;
      case IP6_NEXTH_FRAGMENT: {
        struct ip6_frag_hdr *frag_hdr = NULL;
        LWIP_DEBUGF(IP6_DEBUG, ("nat64_ip6_input: packet with Fragment header\n"));

        frag_hdr = (struct ip6_frag_hdr *)p->payload;

        if (p->len < IP6_HBH_HLEN) {
          LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                      ("IP6_NEXTH_FRAGMENT:(pbuf len %"U16_F" is less than 2), IPv6 packet dropped.\n",
                       p->len));
          IP6_STATS_INC(ip6.drop);
          return IP6_FORWARD_BIG_DROP;
        }

        /* Get next header type. */
        nexth = frag_hdr->_nexth;

        /* 8 : Fragment Header length. */
        optlen = 8;
        ip6_hdr_len += optlen;

        /* Make sure this header fits in current pbuf. */
        if (optlen > p->len) {
          LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                      ("IPv6 opt header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 pac dropped.\n",
                       optlen, p->len));
          IP6_STATS_INC(ip6.drop);
          return IP6_FORWARD_BIG_DROP;
        }

        /* check payload length is multiple of 8 octets when mbit is set */
        if (IP6_FRAG_MBIT(frag_hdr) && ((IP6H_PLEN(iphdr) & 0x7) != 0)) {
          /* ipv6 payload length is not multiple of 8 octets */
          icmp6_param_problem(p, ICMP6_PP_FIELD, frag_hdr);
          LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward_big_packet: packet with invalid payload length dropped\n"));
          IP6_STATS_INC(ip6.drop);
          return IP6_FORWARD_BIG_DROP;
        }

        /* Offset == 0 and more_fragments == 0? */
        if ((frag_hdr->_fragment_offset &
             PP_HTONS(IP6_FRAG_OFFSET_MASK | IP6_FRAG_MORE_FLAG)) == 0) {
          /*
           * This is a 1-fragment packet, usually a packet that we have
           * already reassembled. Skip this header anc continue.
           */
          (void)pbuf_header(p, (s16_t)(-(s16_t)optlen));
        } else {
#if LWIP_IPV6_REASS
          /* reassemble the packet */
          p = ip6_reass(p);
          /* packet not fully reassembled yet? */
          if (p == NULL) {
            LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward_big_packet:fragment is not fully reassembled yet\n"));
            /* note: the pkt has been free in the ip6_reass */
            return IP6_FORWARD_BIG_ASSEMBLED;
          }

          /*
           * Returned p point to IPv6 header.
           * Update all our variables and pointers and continue.
           */
          iphdr = (struct ip6_hdr *)p->payload;
          nexth = IP6H_NEXTH(iphdr);
          optlen = 0;
          ip6_hdr_len = 0;
          *q = p;
          *oiphdr = iphdr;
          (void)pbuf_header(p, -IP6_HLEN);
          ip6_hdr_len = IP6_HLEN;

#else /* LWIP_IPV6_REASS */
          /* free (drop) packet pbufs */
          LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward_big_packet: packet with Fragment header dropped.\n"));
          IP6_STATS_INC(ip6.drop);
          return IP6_FORWARD_BIG_DROP;
#endif /* LWIP_IPV6_REASS */
        }
        break;
      }
      default:
        flags = 1;
        break;
    }
  }

  if ((nexth != IP6_NEXTH_TCP) && (nexth != IP6_NEXTH_UDP) &&
      (nexth != IP6_NEXTH_ICMP6)) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward_big_packet: nexth is not correct.\n"));
    IP6_STATS_INC(ip6.drop);
    return IP6_FORWARD_BIG_DROP;
  }

  (void)pbuf_header(p, (s16_t)ip6_hdr_len);
  return IP6_FORWARD_BIG_HANDLED;
}
#endif
/**
 * Forwards an IPv6 packet. It finds an appropriate route for the
 * packet, decrements the HL value of the packet, and outputs
 * the packet on the appropriate interface.
 *
 * @param p the packet to forward (p->payload points to IP header)
 * @param iphdr the IPv6 header of the input packet
 * @param inp the netif on which this packet was received
 */
static void
ip6_forward(struct pbuf *p, struct ip6_hdr *iphdr, struct netif *inp, u8_t *free_flag)
{
  struct netif *netif;
  u8_t big_packet = 0;

  /* do not forward link-local or loopback addresses */
  if (ip6_addr_islinklocal(ip6_current_dest_addr()) ||
      ip6_addr_isloopback(ip6_current_dest_addr())) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: not forwarding link-local address.\n"));
    IP6_STATS_INC(ip6.rterr);
    IP6_STATS_INC(ip6.drop);
    return;
  }

  /* Find network interface where to forward this IP packet to. */
  netif = ip6_route(IP6_ADDR_ANY6, ip6_current_dest_addr());
  if (netif == NULL) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: no route for %"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F"\n",
        IP6_ADDR_BLOCK1(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK2(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK3(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK4(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK5(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK6(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK7(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK8(ip6_current_dest_addr())));
#if LWIP_ICMP6
    /* Don't send ICMP messages in response to ICMP messages */
    if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {
      icmp6_dest_unreach(p, ICMP6_DUR_NO_ROUTE);
    }
#endif /* LWIP_ICMP6 */
    IP6_STATS_INC(ip6.rterr);
    IP6_STATS_INC(ip6.drop);
    return;
  }
#if LWIP_IPV6_SCOPES
  /* Do not forward packets with a zoned (e.g., link-local) source address
   * outside of their zone. We determined the zone a bit earlier, so we know
   * that the address is properly zoned here, so we can safely use has_zone.
   * Also skip packets with a loopback source address (link-local implied). */
  if ((ip6_addr_has_zone(ip6_current_src_addr()) &&
      !ip6_addr_test_zone(ip6_current_src_addr(), netif)) ||
      ip6_addr_isloopback(ip6_current_src_addr())) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: not forwarding packet beyond its source address zone.\n"));
    IP6_STATS_INC(ip6.rterr);
    IP6_STATS_INC(ip6.drop);
    return;
  }
#endif /* LWIP_IPV6_SCOPES */
  /* Do not forward packets onto the same network interface on which
   * they arrived. */
  if ((netif == inp)
#if LWIP_RIPPLE
      && (lwip_rpl_is_rpl_netif(inp) == lwIP_FALSE)
#endif
     ) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: not bouncing packets back on incoming interface.\n"));
    IP6_STATS_INC(ip6.rterr);
    IP6_STATS_INC(ip6.drop);
    return;
  }

#if LWIP_RIPPLE
  int ret;
  if (lwip_rpl_is_br() && (lwip_rpl_is_rpl_netif(inp) == lwIP_FALSE) &&
      (lwip_rpl_is_rpl_netif(netif) == lwIP_TRUE)) {
    /* should handle the big packet. */
    ret = ip6_forward_big_packet(&p, &iphdr, netif, &big_packet);
    if (ret == IP6_FORWARD_BIG_DROP) {
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: DROP\n"));
      IP6_STATS_INC(ip6.drop);
      return;
    } else if (ret == IP6_FORWARD_BIG_ASSEMBLED) {
      *free_flag = lwIP_FALSE;
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: ressa\n"));
      return;
    }
  }
#else
  LWIP_UNUSED_ARG(big_packet);
  LWIP_UNUSED_ARG(free_flag);
#endif
  /* decrement HL */
  if (IP6H_HOPLIM(iphdr)) {
    IP6H_HOPLIM_SET(iphdr, IP6H_HOPLIM(iphdr) - 1);
  }
  /* send ICMP6 if HL == 0 */
  if (IP6H_HOPLIM(iphdr) == 0) {
#if LWIP_ICMP6
    /* Don't send ICMP messages in response to ICMP messages */
    if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {
      icmp6_time_exceeded(p, ICMP6_TE_HL);
    }
#endif /* LWIP_ICMP6 */
    IP6_STATS_INC(ip6.drop);
    return;
  }

  if (netif->mtu && (p->tot_len > netif->mtu) && (big_packet == 0)) {
#if LWIP_ICMP6
    /* Don't send ICMP messages in response to ICMP messages */
    if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {
      icmp6_packet_too_big(p, netif->mtu);
    }
#endif /* LWIP_ICMP6 */
    IP6_STATS_INC(ip6.drop);
    return;
  }

  LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: forwarding packet to %"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F"\n",
      IP6_ADDR_BLOCK1(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK2(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK3(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK4(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK5(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK6(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK7(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK8(ip6_current_dest_addr())));

#if LWIP_RIPPLE
  /* Update the RPI extended header before forwarding */
  if ((p->flags & PBUF_FLAG_HBH_SPACE) != 0) {
    if (lwip_get_pkt_route_status()) {
      p->pkt_up = lwIP_TRUE;
    } else {
      p->pkt_up = lwIP_FALSE;
    }
    if (ip6_update_rpi(p, netif) != ERR_OK) {
      IP6_STATS_INC(ip6.rterr);
      IP6_STATS_INC(ip6.drop);
      return;
    }
  }
#endif
  LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: %c%c%"U16_F"\n", netif->name[0], netif->name[1], (u16_t)netif->num));
  ip6_debug_print(p);

#if LWIP_IPV6_FRAG
  /* don't fragment if interface has mtu set to 0 [loopif] */
  if ((big_packet != 0) && netif_mtu6(netif) &&
      (p->tot_len > nd6_get_destination_mtu(ip6_current_dest_addr(), netif))) {
    (void)ip6_frag(p, netif, ip6_current_dest_addr());
    *free_flag = lwIP_FALSE;
    (void)pbuf_free(p);
    return;
  }
#endif /* LWIP_IPV6_FRAG */

  /* transmit pbuf on chosen interface */
  netif->output_ip6(netif, p, ip6_current_dest_addr());
  IP6_STATS_INC(ip6.fw);
  IP6_STATS_INC(ip6.xmit);
  return;
}
#endif /* LWIP_IPV6_FORWARD */

static u8_t
ip6_process_destination_header_extension_unknown_options(struct pbuf *p, const struct ip6_opt_hdr *opt_hdr)
{
  u8_t clear_resource = 0;
  /* Check 2 MSB of Destination header type. */
  switch (IP6_OPT_TYPE_ACTION(opt_hdr)) {
    case 1:
      /* Discard the packet. */
      LWIP_DEBUGF(IP6_DEBUG,
                  ("ip6_input: packet with invalid destination option type dropped, \
                  highest order 2 bits value is 1\n"));
      clear_resource = 1;
      break;
    case 2:
      /* Send ICMP Parameter Problem */
      LWIP_DEBUGF(IP6_DEBUG,
                  ("ip6_input: packet with invalid destination option type dropped, \
                   highest order 2 bits value is 2\n"));
      /* move payload pointer back to ip header */
      (void)pbuf_header_force(p, (s16_t)((u8_t*)p->payload - (const u8_t *)ip6_current_header()));
      icmp6_param_problem(p, ICMP6_PP_OPTION, (const void *)opt_hdr);
      clear_resource = 1;
      break;
    case 3:
      /* Send ICMP Parameter Problem if destination address is not a multicast address */
      LWIP_DEBUGF(IP6_DEBUG,
                  ("ip6_input: packet with invalid destination option type dropped, \
                   highest order 2 bits value is 3\n"));
      if (!ip6_addr_ismulticast(ip6_current_dest_addr())) {
        /* move payload pointer back to ip header */
        (void)pbuf_header_force(p, (s16_t)((u8_t*)p->payload - (const u8_t *)ip6_current_header()));
        icmp6_param_problem(p, ICMP6_PP_OPTION, (const void *)opt_hdr);
      }
      clear_resource = 1;
      break;

    default:
      break;
  }

  return clear_resource;
}
/*
 * This function is called when there is extension options to be processed in destination header in IPV6 packet
 * @param p points to the pbuf buffer
 * @param dest_hdr points to the destination header in the IPV6
 * @param hlen header lenght of the destination option
 * @return need_ip6_input_cleanup = 1 means discard, 0 means continue process the packet
 */
void ip6_process_destination_header_extension_options(struct pbuf *p, struct ip6_dest_hdr *dest_hdr,
                                                      u16_t hlen, u8_t *need_ip6_input_cleanup)
{
  s32_t opt_offset;
  struct ip6_opt_hdr *opt_hdr = NULL;
  if ((p == NULL) || (dest_hdr == NULL) || (need_ip6_input_cleanup == NULL)) {
    return;
  }
  *need_ip6_input_cleanup = 0;

  /* The extended option header starts right after Destination header. */
  opt_offset = IP6_DEST_HLEN;
  while (opt_offset < (s32_t)hlen) {
    s32_t opt_dlen = 0;
    u8_t clear_resource = 0;

    opt_hdr = (struct ip6_opt_hdr *)((u8_t *)dest_hdr + opt_offset);

    /*
     * @page RFC-2460 RFC-2460
     * @par  RFC-2460 Compliance Information
     * @par Compliant Section
     * IPv6 Extension Headers. Test v6LC.1.2.8: Option Processing, Destination Options Header
     * @par Behavior Description
     *
     * Our node conforms to RFC-2460. \n
     *  RFC-2460 does not support below extension options and the features corresponding to it. \n
     * @verbatim
     *   RFC-3775 --> IPV6 Mobility Support       - IP6_HOME_ADDRESS_OPTION (Option_Type = 201)
     *   RFC-2711 --> IPv6 Router Alert Option    - IP6_ROUTER_ALERT_OPTION (Option_Type = 5)
     *   RFC-2675 --> IPV6 Jumbo Payload Option   - IP6_JUMBO_OPTION        (Option_Type = 194)
     * @endverbatim
     * \n
     *   For these options and other extension header options, our node action conforms to RFC-2460: \n
     *   If the IPv6 node does not recognize the Option_Type, then the action it should take depends
     *   on the highest order two bits of the Option_Type.
     */
    switch (IP6_OPT_TYPE(opt_hdr)) {
      case IP6_PAD1_OPTION:
        /* PAD1 option deosn't have length and value field */
        opt_dlen = -1;
        break;

      case IP6_PADN_OPTION:
        if ((hlen - opt_offset) >= IP6_OPT_HLEN) { /* malformed packet */
          opt_dlen = IP6_OPT_DLEN(opt_hdr);
        } else {
          /* Discard the packet. */
          LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                      ("ip6_input: malformed packet detected in PADN option rocessing, discarding it \n"));
          clear_resource = 1;
        }
        break;

      default:
        clear_resource = ip6_process_destination_header_extension_unknown_options(p, opt_hdr);
        if (clear_resource == 0) {
          /* Skip over this option. */
          if ((hlen - opt_offset) >= IP6_OPT_HLEN) { /* malformed packet */
            opt_dlen = IP6_OPT_DLEN(opt_hdr);
          } else {
            /* Discard the packet. */
            LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                        ("ip6_input: malformed packet detected in Unknown option processing, discarding it \n"));
            clear_resource = 1;
          }
        }
        break;
    }

    if (clear_resource != 0) {
      (void)pbuf_free(p);
      IP6_STATS_INC(ip6.drop);
      *need_ip6_input_cleanup = 1;
      break;
    }
    /* Adjust the offset to move to the next extended option header */
    opt_offset = opt_offset + IP6_OPT_HLEN + opt_dlen;
  }

  return;
}
#if LWIP_IPV6_MLD_QUERIER
static u8_t
ip6_mld6_check(struct pbuf *p, struct ip6_hdr *ip6hdr)
{
  u16_t *opt_data;
  u32_t offset = 0;
  struct ip6_hbh_hdr *hbh_hdr = NULL;
  struct ip6_opt_hdr *opt_hdr = NULL;

  if (IP6H_NEXTH(ip6hdr) != IP6_NEXTH_HOPBYHOP) {
    return lwIP_FALSE;
  }

  /* Move to payload. */
  (void)pbuf_header(p, -IP6_HLEN);
  if (p->len < sizeof(struct ip6_hbh_hdr)) {
    return lwIP_FALSE;
  }
  hbh_hdr = (struct ip6_hbh_hdr *)(p->payload);
  if (hbh_hdr->_nexth != IP6_NEXTH_ICMP6) {
    return lwIP_FALSE;
  }
  offset = IP6_HBH_HLEN;

  opt_hdr = (struct ip6_opt_hdr *)((u8_t *)hbh_hdr + offset);
  if (IP6_OPT_TYPE(opt_hdr) != IP6_ROUTER_ALERT_OPTION) {
    return lwIP_FALSE;
  }
  offset += IP6_OPT_HLEN;

  /* Set router alert option data */
  opt_data = (u16_t *)((u8_t *)hbh_hdr + offset);
  if (*opt_data != IP6_ROUTER_ALERT_VALUE_MLD) {
    return lwIP_FALSE;
  }

  /* Move to ip6 header */
  (void)pbuf_header(p, IP6_HLEN);
  return lwIP_TRUE;
}
#endif /* LWIP_IPV6_MLD_QUERIER */

#if LWIP_IPV6_MLD && LWIP_MPL
static u8_t
ip6_mld6_mpl_check(struct pbuf *p, struct netif *inp)
{
  LWIP_ASSERT("invalid pointer\n", (p != NULL && inp != NULL));
  if ((inp->flags & NETIF_IS_RPL_UP) == NETIF_IS_RPL_UP) {
    if ((ip6_addr_multicast_scope(ip6_current_dest_addr()) > IP6_MULTICAST_SCOPE_LINK_LOCAL) &&
        (MCAST6_IS_FROM_CONN_PEER(p) == MCAST6_FALSE)) {
      return lwIP_FALSE;
    }
  } else {
    u8_t mesh_ap;
    if ((is_connect_to_ap(&mesh_ap) == lwIP_TRUE) && (mesh_ap == lwIP_TRUE) &&
        (is_connected_ap(p->mac_address, sizeof(p->mac_address)) == lwIP_FALSE)) {
      return lwIP_FALSE;
    }
  }
  return lwIP_TRUE;
}
#endif /* LWIP_IPV6_MLD && LWIP_MPL */
#if LWIP_RIPPLE && defined(LWIP_NA_PROXY) && LWIP_NA_PROXY
static err_t
ip6_na_proxy(ip6_addr_t *src, ip6_addr_t *target, struct netif *inp)
{
  if (ip6_addr_isany_val(*src)) {
    return ERR_ARG;
  }

  if (lwip_rpl_is_rpl_netif(inp)) {
    /* only the non-mesh node be proxy */
    return lwip_rpl_nonmesh_solicited_node(src);
  } else if (lwip_rpl_is_br()) {
    /* the node behind the MBR will be proxy */
    return lwip_rpl_behind_mbr_solicited_node(target);
  }

  return ERR_VAL;
}
#endif

/** Return true if the current input packet should be accepted on this netif */
static int
ip6_input_accept(struct netif *netif)
{
  /* interface is up? */
  if (netif_is_up(netif)) {
    u8_t i;
    /* unicast to this interface address? address configured? */
    /* If custom scopes are used, the destination zone will be tested as
      * part of the local-address comparison, but we need to test the source
      * scope as well (e.g., is this interface on the same link?). */
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_cmp(ip6_current_dest_addr(), netif_ip6_addr(netif, i))
#if IPV6_CUSTOM_SCOPES
          && (!ip6_addr_has_zone(ip6_current_src_addr()) ||
              ip6_addr_test_zone(ip6_current_src_addr(), netif))
#endif /* IPV6_CUSTOM_SCOPES */
      ) {
        /* accept on this netif */
        return 1;
      }
    }
  }
  return 0;
}

/**
 * This function is called by the network interface device driver when
 * an IPv6 packet is received. The function does the basic checks of the
 * IP header such as packet size being at least larger than the header
 * size etc. If the packet was not destined for us, the packet is
 * forwarded (using ip6_forward).
 *
 * Finally, the packet is sent to the upper layer protocol input function.
 *
 * @param p the received IPv6 packet (p->payload points to IPv6 header)
 * @param inp the netif on which this packet was received
 * @return ERR_OK if the packet was processed (could return ERR_* if it wasn't
 *         processed, but currently always returns ERR_OK)
 */
err_t
ip6_input(struct pbuf *p, struct netif *inp)
{
  struct ip6_hdr *ip6hdr;
  struct netif *netif;
  const u8_t *nexth;
  u16_t hlen, hlen_tot; /* the current header length */
#if LWIP_RAW
  raw_input_state_t raw_status;
#endif /* LWIP_RAW */
  u8_t need_ip6_input_cleanup = 0;
  u8_t assemble_flag;

  LWIP_ASSERT_CORE_LOCKED();

  IP6_STATS_INC(ip6.recv);

#if LWIP_IPV6_FILTER
  if (ip6_filter != NULL && ip6_filter(p, inp) != ERR_OK) {
    (void)pbuf_free(p);
    IP6_STATS_INC(ip6.drop);
    return ERR_OK;
  }
#endif /* LWIP_IPV6_FILTER */

  if ((IP6_HLEN > p->len)) {
    LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                ("IPv6 header (len %"U16_F") does not fit in first pbuf (len %"U16_F"), IP packet dropped.\n",
                (u16_t)IP6_HLEN, p->len));

    /* free (drop) packet pbufs */
    (void)pbuf_free(p);
    IP6_STATS_INC(ip6.lenerr);
    IP6_STATS_INC(ip6.drop);
    return ERR_OK;
  }

  /* identify the IP header */
  ip6hdr = (struct ip6_hdr *)p->payload;
  if (IP6H_V(ip6hdr) != 6) {
    LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING, ("IPv6 packet dropped due to bad version number %"U32_F"\n",
        IP6H_V(ip6hdr)));
    pbuf_free(p);
    IP6_STATS_INC(ip6.err);
    IP6_STATS_INC(ip6.drop);
    return ERR_OK;
  }

#ifdef LWIP_HOOK_IP6_INPUT
  if (LWIP_HOOK_IP6_INPUT(p, inp)) {
    /* the packet has been eaten */
    return ERR_OK;
  }
#endif

  /* header length exceeds first pbuf length, or ip length exceeds total pbuf length? */

  if ((IP6H_PLEN(ip6hdr) + IP6_HLEN) > p->tot_len) {
    LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
      ("IPv6 (plen %"U16_F") is longer than pbuf (len %"U16_F"), IP packet dropped.\n",
          (u16_t)(IP6H_PLEN(ip6hdr) + IP6_HLEN), p->tot_len));

    /* free (drop) packet pbufs */
    pbuf_free(p);
    IP6_STATS_INC(ip6.lenerr);
    IP6_STATS_INC(ip6.drop);
    return ERR_OK;
  }

  /* Trim pbuf. This should have been done at the netif layer,
   * but we'll do it anyway just to be sure that its done. */
  pbuf_realloc(p, (u16_t)(IP6_HLEN + IP6H_PLEN(ip6hdr)));

  /* copy IP addresses to aligned ip6_addr_t */
  ip_addr_copy_from_ip6_packed(ip_data.current_iphdr_dest, ip6hdr->dest);
  ip_addr_copy_from_ip6_packed(ip_data.current_iphdr_src, ip6hdr->src);

  /* Don't accept virtual IPv4 mapped IPv6 addresses.
   * Don't accept multicast source addresses. */
  if (ip6_addr_isipv4mappedipv6(ip_2_ip6(&ip_data.current_iphdr_dest)) ||
     ip6_addr_isipv4mappedipv6(ip_2_ip6(&ip_data.current_iphdr_src)) ||
     ip6_addr_ismulticast(ip_2_ip6(&ip_data.current_iphdr_src))) {
    /* free (drop) packet pbufs */
    pbuf_free(p);
    IP6_STATS_INC(ip6.err);
    IP6_STATS_INC(ip6.drop);
    return ERR_OK;
  }

  /* Set the appropriate zone identifier on the addresses. */
  ip6_addr_assign_zone(ip_2_ip6(&ip_data.current_iphdr_dest), IP6_UNKNOWN, inp);
  ip6_addr_assign_zone(ip_2_ip6(&ip_data.current_iphdr_src), IP6_UNICAST, inp);

#if LWIP_MPL
  /*
   * Don't accept the packet which was sent by us.
   */
  for (netif = netif_list; netif != NULL; netif = netif->next) {
    u8_t i;
    if (netif->flags & NETIF_FLAG_LOOPBACK) {
      continue;
    }
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_cmp(ip6_current_src_addr(), netif_ip6_addr(netif, i))) {
        /* free (drop) packet pbufs */
        (void)pbuf_free(p);
        IP6_STATS_INC(ip6.err);
        IP6_STATS_INC(ip6.drop);
        return ERR_OK;
      }
    }
  }
#endif

#if LWIP_RIPPLE && defined(LWIP_NA_PROXY) && LWIP_NA_PROXY
  p->na_proxy = lwIP_FALSE;
#endif
  /* current header pointer. */
  ip_data.current_ip6_header = ip6hdr;

  /* In netif, used in case we need to send ICMPv6 packets back. */
  ip_data.current_netif = inp;
  ip_data.current_input_netif = inp;

  /* match packet against an interface, i.e. is this packet for us? */
  if (ip6_addr_ismulticast(ip6_current_dest_addr())) {
    /* Always joined to multicast if-local and link-local all-nodes group. */
    if (ip6_addr_isallnodes_iflocal(ip6_current_dest_addr()) ||
        ip6_addr_isallnodes_linklocal(ip6_current_dest_addr())
#if LWIP_RIPPLE
        || ip6_addr_isallrpl_nodes_linklocal(ip6_current_dest_addr())
        || ip6_addr_isallrouters_linklocal(ip6_current_dest_addr())
#endif
       ) {
      netif = inp;
    }
#if LWIP_IPV6_MLD
    else if (mld6_lookfor_group(inp, ip6_current_dest_addr())
#if LWIP_MPL_IPV4_BCAST
             || ip6_addr_is_mpl_ip4_bcast(ip6_current_dest_addr())
#endif
            ) {
      netif = inp;
#if LWIP_MPL
      if (ip6_mld6_mpl_check(p, inp) == lwIP_FALSE) {
        /* free (drop) packet pbufs */
        (void)pbuf_free(p);
        IP6_STATS_INC(ip6.err);
        IP6_STATS_INC(ip6.drop);
        return ERR_OK;
      }
#endif
    }
#else /* LWIP_IPV6_MLD */
    else if (ip6_addr_issolicitednode(ip6_current_dest_addr())) {
      u8_t i;
      /* Filter solicited node packets when MLD is not enabled
       * (for Neighbor discovery). */
      netif = NULL;
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (!ip6_addr_isinvalid(netif_ip6_addr_state(inp, i)) &&
            ip6_addr_cmp_solicitednode(ip6_current_dest_addr(), netif_ip6_addr(inp, i))) {
          netif = inp;
          LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: solicited node packet accepted on interface %s\n",
              netif->name));
          break;
        }
      }
#if LWIP_RIPPLE && defined(LWIP_NA_PROXY) && LWIP_NA_PROXY
      if ((netif == NULL) &&
          (ip6_na_proxy(ip6_current_src_addr(), ip6_current_dest_addr(), inp) == ERR_OK)) {
        netif = inp;
        p->na_proxy = lwIP_TRUE;
      }
#endif
    }
#endif /* LWIP_IPV6_MLD */
#if LWIP_IPV6_MLD_QUERIER
    else if (
#if LWIP_MPL
      (ip6_addr_multicast_scope(ip6_current_dest_addr()) > IP6_MULTICAST_SCOPE_LINK_LOCAL) &&
       (MCAST6_IS_FROM_CONN_PEER(p) == MCAST6_TRUE) &&
#endif /* LWIP_MPL */
       (ip6_addr_isallrouters_linklocal(ip6_current_dest_addr()) ||
       (ip6_mld6_check(p, ip6hdr) == lwIP_TRUE))) {
      netif = inp;
    }
#endif
#if LWIP_RIPPLE && defined(LWIP_NA_PROXY) && LWIP_NA_PROXY
    else if (ip6_addr_issolicitednode(ip6_current_dest_addr())) {
      u8_t i = 0;
      /*
       * Filter solicited node packets when MLD is not enabled
       * (for Neighbor discovery).
       */
      netif = NULL;
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (!ip6_addr_isinvalid(netif_ip6_addr_state(inp, i)) &&
            ip6_addr_cmp_solicitednode(ip6_current_dest_addr(), netif_ip6_addr(inp, i))) {
          netif = inp;
          LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: solicited node packet accepted on interface %c%c\n",
                                  netif->name[0], netif->name[1]));
          break;
        }
      }

      if ((netif == NULL) &&
          (ip6_na_proxy(ip6_current_src_addr(), ip6_current_dest_addr(), inp) == ERR_OK)) {
        netif = inp;
        p->na_proxy = lwIP_TRUE;
      }
    }
#endif
    else {
      netif = NULL;
    }
#if LWIP_MPL
    if (ip6_addr_multicast_scope(ip6_current_dest_addr()) > IP6_MULTICAST_SCOPE_LINK_LOCAL) {
      MCAST6_FORWARD(p, ip6hdr);
    }
#endif /* LWIP_MPL */
  } else {
    /* start trying with inp. if that's not acceptable, start walking the
       list of configured netifs. */
    if (ip6_input_accept(inp)) {
      netif = inp;
    } else {
      netif = NULL;
#if !IPV6_CUSTOM_SCOPES
      /* Shortcut: stop looking for other interfaces if either the source or
        * the destination has a scope constrained to this interface. Custom
        * scopes may break the 1:1 link/interface mapping, however. */
      if (ip6_addr_islinklocal(ip6_current_dest_addr()) ||
          ip6_addr_islinklocal(ip6_current_src_addr())) {
        goto netif_found;
      }
#endif /* !IPV6_CUSTOM_SCOPES */
#if !LWIP_NETIF_LOOPBACK || LWIP_HAVE_LOOPIF
      /* The loopback address is to be considered link-local. Packets to it
        * should be dropped on other interfaces, as per RFC 4291 Sec. 2.5.3.
        * Its implied scope means packets *from* the loopback address should
        * not be accepted on other interfaces, either. These requirements
        * cannot be implemented in the case that loopback traffic is sent
        * across a non-loopback interface, however. */
      if (ip6_addr_isloopback(ip6_current_dest_addr()) ||
          ip6_addr_isloopback(ip6_current_src_addr())) {
        goto netif_found;
      }
#endif /* !LWIP_NETIF_LOOPBACK || LWIP_HAVE_LOOPIF */
#if !LWIP_SINGLE_NETIF
      NETIF_FOREACH(netif) {
        if (netif == inp) {
          /* we checked that before already */
          continue;
        }
        if (ip6_input_accept(netif)) {
          break;
        }
      }
#endif /* !LWIP_SINGLE_NETIF */
    }
netif_found:
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet accepted on interface %s\n",
        netif ? netif->name : "XX"));
  }

  /* "::" packet source address? (used in duplicate address detection) */
  if (ip6_addr_isany_val(*ip6_current_src_addr()) &&
      (!ip6_addr_issolicitednode(ip6_current_dest_addr()))) {
    /* packet source is not valid */
    /* free (drop) packet pbufs */
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with src ANY_ADDRESS dropped\n"));
    pbuf_free(p);
    IP6_STATS_INC(ip6.drop);
    goto ip6_input_cleanup;
  }

  /* packet not for us? */
  if (netif == NULL) {
    u8_t free_flag = lwIP_TRUE;
    /* packet not for us, route or discard */
    LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_TRACE, ("ip6_input: packet not for us.\n"));
#if LWIP_IP6IN4 && LWIP_IP6IN4_PRIOR_NAT64
    if (ip6in4_ip6_input(p, ip6hdr, inp, NULL)) {
      LWIP_DEBUGF(IP_DEBUG | LWIP_DBG_TRACE, ("ip6in4_ip6_input: packet handle.\n"));
      goto ip6_input_cleanup;
    }
#endif
#if LWIP_NAT64
    if (nat64_ip6_input(p, ip6hdr, inp)) {
      LWIP_DEBUGF(IP_DEBUG | LWIP_DBG_TRACE, ("nat64_ip6_input: packet handle.\n"));
      goto ip6_input_cleanup;
    }
#endif /* LWIP_NAT64 */
#if LWIP_IP6IN4 && !LWIP_IP6IN4_PRIOR_NAT64 && !LWIP_NAT64_IP6IN4
    if (ip6in4_ip6_input(p, ip6hdr, inp, NULL)) {
      LWIP_DEBUGF(IP_DEBUG | LWIP_DBG_TRACE, ("ip6in4_ip6_input: packet handle.\n"));
      goto ip6_input_cleanup;
    }
#endif
#if LWIP_IPV6_FORWARD
    /* non-multicast packet? */
    if (!ip6_addr_ismulticast(ip6_current_dest_addr())) {
      /* try to forward IP packet on (other) interfaces */
#if LWIP_RIPPLE
      (void)pbuf_header(p, -IP6_HLEN);
      if (ip6_process_hbh_exth_options(p, inp) == ERR_OK) {
        (void)pbuf_header(p, IP6_HLEN);
        LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: RPI validation Success\n"));
        ip6_forward(p, ip6hdr, inp, &free_flag);
      }
#else
      ip6_forward(p, ip6hdr, inp, &free_flag);
#endif
    }
#endif /* LWIP_IPV6_FORWARD */
    if (free_flag == lwIP_TRUE) {
      (void)pbuf_free(p);
    }
    IP6_STATS_INC(ip6.drop);
    goto ip6_input_cleanup;
  }

  /* current netif pointer. */
  ip_data.current_netif = netif;

  /* Save next header type. */
  nexth = &IP6H_NEXTH(ip6hdr);

  /* Init header length. */
  hlen = hlen_tot = IP6_HLEN;

  /* Move to payload. */
  pbuf_remove_header(p, IP6_HLEN);

  /* Process known option extension headers, if present. */
  while (*nexth != IP6_NEXTH_NONE)
  {
    assemble_flag = 0;
    switch (*nexth) {
    case IP6_NEXTH_HOPBYHOP:
    {
      s32_t opt_offset;
      struct ip6_hbh_hdr *hbh_hdr;
      struct ip6_opt_hdr *opt_hdr;
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Hop-by-Hop options header\n"));

      /* Get and check the header length, while staying in packet bounds. */
      hbh_hdr = (struct ip6_hbh_hdr *)p->payload;

      /* Get next header type. */
#if LWIP_RIPPLE
      if (ip6_process_hbh_exth_options(p, netif) != ERR_OK) {
        LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: RPI validation failed\n"));
        (void)pbuf_free(p);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }
#endif
      nexth = &IP6_HBH_NEXTH(hbh_hdr);
      /*
       *@page RFC-2460 RFC-2460
       *@par Compliant Section
       *Section 4.  IPv6 Extension Headers. Test v6LC.1.2.1: Next Header Zero
       *@par Behavior Description
       *The lwIP stack will not send back the ICMP Parameter Problem message and discard
       *the packet when a packet has Next Header field of zero in a header other than an IPv6 header.
       *Also the processing of Hop-by-Hop Options is not performed.
       *The exception referred to in the preceding paragraph is the Hop-by-
       *Hop Options header, which carries information that must be examined
       *and processed by every node along a packet's delivery path, including
       *the source and destination nodes.  The Hop-by-Hop Options header,
       *when present, must immediately follow the IPv6 header.  Its presence
       *is indicated by the value zero in the Next Header field of the IPv6
       *header. \n
       *If, as a result of processing a header, a node is required to proceed
       *to the next header but the Next Header value in the current header is
       *unrecognized by the node, it should discard the packet and send an
       *ICMP Parameter Problem message to the source of the packet, with an
       *ICMP Code value of 1 ("unrecognized Next Header type encountered")
       *and the ICMP Pointer field containing the offset of the unrecognized
       *value within the original packet.  The same action should be taken if
       *a node encounters a Next Header value of zero in any header other \,
       *than an IPv6 header. \n
       *[RFC 8200 4. IPv6 Extension Headers] Modified the above behavior. By default no need to send
       *back the ICMP Parameter Problem message and discard the packet when a packet has Next
       *Header field of zero in a header other than an IPv6 header. Also the processing of Hop-by-Hop
       *Options header is changed from 'must' to 'may' by every node in the path \n
       * \n
       *NOTE: While RFC-2460 required that all nodes must examine and
       *process the Hop-by-Hop Options header, it is now expected that nodes
       *along a packet's delivery path only examine and process the
       *Hop-by-Hop Options header if explicitly configured to do so.
       */
      if (p->len < IP6_OPT_HLEN) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IP6_NEXTH_HOPBYHOP: pbuf (len %"U16_F") is less than 2.\n", p->len));
        /* free (drop) packet pbufs */
        (void)pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }

      /* Get the header length. */
      hlen = (u16_t)(8 * (1 + hbh_hdr->_hlen));

      if ((p->len < 8) || (hlen > p->len)) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IPv6 options header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 packet dropped.\n",
              hlen, p->len));
        /* free (drop) packet pbufs */
        pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }

      hlen_tot = (u16_t)(hlen_tot + hlen);

      /* The extended option header starts right after Hop-by-Hop header. */
      opt_offset = IP6_HBH_HLEN;
      while (opt_offset < hlen)
      {
        s32_t opt_dlen = 0;

        opt_hdr = (struct ip6_opt_hdr *)((u8_t *)hbh_hdr + opt_offset);

        switch (IP6_OPT_TYPE(opt_hdr)) {
        /* @todo: process IPV6 Hop-by-Hop option data */
        case IP6_PAD1_OPTION:
          /* PAD1 option doesn't have length and value field */
          opt_dlen = -1;
          break;
        case IP6_PADN_OPTION:
          opt_dlen = IP6_OPT_DLEN(opt_hdr);
          break;
        case IP6_ROUTER_ALERT_OPTION:
          opt_dlen = IP6_OPT_DLEN(opt_hdr);
          break;
        case IP6_JUMBO_OPTION:
          opt_dlen = IP6_OPT_DLEN(opt_hdr);
          break;
        default:
          /* Check 2 MSB of Hop-by-Hop header type. */
          switch (IP6_OPT_TYPE_ACTION(opt_hdr)) {
          case 1:
            /* Discard the packet. */
            LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with invalid Hop-by-Hop option type dropped.\n"));
            pbuf_free(p);
            IP6_STATS_INC(ip6.drop);
            goto ip6_input_cleanup;
          case 2:
            /* Send ICMP Parameter Problem */
            /* move payload pointer back to ip header */
            (void)pbuf_header_force(p, (s16_t)((u8_t*)p->payload - (const u8_t *)ip6_current_header()));
            icmp6_param_problem(p, ICMP6_PP_OPTION, (const void *)opt_hdr);
            LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with invalid Hop-by-Hop option type dropped.\n"));
            pbuf_free(p);
            IP6_STATS_INC(ip6.drop);
            goto ip6_input_cleanup;
          case 3:
            /* Send ICMP Parameter Problem if destination address is not a multicast address */
            if (!ip6_addr_ismulticast(ip6_current_dest_addr())) {
              /* move payload pointer back to ip header */
              (void)pbuf_header_force(p, (s16_t)((u8_t*)p->payload - (const u8_t *)ip6_current_header()));
              icmp6_param_problem(p, ICMP6_PP_OPTION, (const void *)opt_hdr);
            }
            LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with invalid Hop-by-Hop option type dropped.\n"));
            pbuf_free(p);
            IP6_STATS_INC(ip6.drop);
            goto ip6_input_cleanup;
          default:
            /* Skip over this option. */
            opt_dlen = IP6_OPT_DLEN(opt_hdr);
            break;
          }
          break;
        }

        /* Adjust the offset to move to the next extended option header */
        opt_offset = opt_offset + IP6_OPT_HLEN + opt_dlen;
      }
      pbuf_remove_header(p, hlen);
      break;
    }
    case IP6_NEXTH_DESTOPTS:
    {
      struct ip6_dest_hdr *dest_hdr;
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Destination options header\n"));

      dest_hdr = (struct ip6_dest_hdr *)p->payload;

      /* Get next header type. */
      nexth = &IP6_DEST_NEXTH(dest_hdr);
      if (p->len < IP6_DEST_HLEN) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IP6_NEXTH_DESTOPTS: pbuf (len %"U16_F") is less than 2.\n", p->len));
        /* free (drop) packet pbufs */
        (void)pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }

      /* Get the header length. */
      hlen = 8 * (1 + dest_hdr->_hlen);
      if ((p->len < 8) || (hlen > p->len)) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IPv6 options header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 packet dropped.\n",
              hlen, p->len));
        /* free (drop) packet pbufs */
        pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }

      hlen_tot = (u16_t)(hlen_tot + hlen);
      /*
       *@page RFC-2460 RFC-2460
       *@par Compliant Section
       *Section 4.  IPv6 Extension Headers. Test v6LC.1.2.8: Option Processing, Destination Options Header
       *@par Behavior Description
       *Our code conforms to RFC-2460. \n
       *   RFC-2460 does not support the following extension options and the features corresponding to it. \n
       *@verbatim
       *   RFC-3775 --> IPV6 Mobility Support       - IP6_HOME_ADDRESS_OPTION (Option_Type = 201)
       *   RFC-2711 --> IPv6 Router Alert Option    - IP6_ROUTER_ALERT_OPTION (Option_Type = 5)
       *   RFC-2675 --> IPV6 Jumbo Payload Option   - IP6_JUMBO_OPTION        (Option Type = 194)
       *   @endverbatim
       *   For these options and other extension header options, our node action will conform to RFC-2460: \n
       *   If the IPv6 node does not recognize the Option_Type, then the action it should take depends
       *  on the highest order two bits of the Option_Type.
       */
      /* The extended option header starts right after Destination header. */
      ip6_process_destination_header_extension_options(p, dest_hdr, hlen, &need_ip6_input_cleanup);
      if (need_ip6_input_cleanup) {
        goto ip6_input_cleanup;
      }

      pbuf_remove_header(p, hlen);
      break;
    }
    case IP6_NEXTH_ROUTING:
    {
      struct ip6_rout_hdr *rout_hdr;
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Routing header\n"));

      rout_hdr = (struct ip6_rout_hdr *)p->payload;

      /* Get next header type. */
      nexth = &IP6_ROUT_NEXTH(rout_hdr);
      if (p->len < IP6_OPT_HLEN) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IP6_NEXTH_ROUTING: pbuf (len %"U16_F") is less than 2.\n", p->len));
        /* free (drop) packet pbufs */
        (void)pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }

      /* Get the header length. */
      hlen = 8 * (1 + rout_hdr->_hlen);

      if ((p->len < 8) || (hlen > p->len)) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IPv6 options header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 packet dropped.\n",
              hlen, p->len));
        /* free (drop) packet pbufs */
        pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }

      /* Skip over this header. */
      hlen_tot = (u16_t)(hlen_tot + hlen);

      /* if segment left value is 0 in routing header, ignore the option */
      if (IP6_ROUT_SEG_LEFT(rout_hdr)) {
        /* The length field of routing option header must be even */
        if (rout_hdr->_hlen & 0x1) {
          /* Discard and send parameter field error */
          icmp6_param_problem(p, ICMP6_PP_FIELD, &rout_hdr->_hlen);
          LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with invalid routing type dropped\n"));
          pbuf_free(p);
          IP6_STATS_INC(ip6.drop);
          goto ip6_input_cleanup;
        }

        switch (IP6_ROUT_TYPE(rout_hdr))
        {
        /* TODO: process routing by the type */
        case IP6_ROUT_TYPE2:
          break;
        case IP6_ROUT_RPL:
          break;
        default:
          /* Discard unrecognized routing type and send parameter field error */
          icmp6_param_problem(p, ICMP6_PP_FIELD, &IP6_ROUT_TYPE(rout_hdr));
          LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with invalid routing type dropped\n"));
          pbuf_free(p);
          IP6_STATS_INC(ip6.drop);
          goto ip6_input_cleanup;
        }
      }

      pbuf_remove_header(p, hlen);
      break;
    }
    case IP6_NEXTH_FRAGMENT:
    {
      struct ip6_frag_hdr *frag_hdr;
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Fragment header\n"));

      frag_hdr = (struct ip6_frag_hdr *)p->payload;

      if (p->len < IP6_OPT_HLEN) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IP6_NEXTH_FRAGMENT:(pbuf len %"U16_F" is less than 2), IPv6 packet dropped.\n",
            p->len));
        /* free (drop) packet pbufs */
        (void)pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }
      /* Get next header type. */
      nexth = &IP6_FRAG_NEXTH(frag_hdr);

      /* Fragment Header length. */
      hlen = 8;

      /* Make sure this header fits in current pbuf. */
      if (hlen > p->len) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IPv6 options header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 packet dropped.\n",
              hlen, p->len));
        /* free (drop) packet pbufs */
        pbuf_free(p);
        IP6_FRAG_STATS_INC(ip6_frag.lenerr);
        IP6_FRAG_STATS_INC(ip6_frag.drop);
        goto ip6_input_cleanup;
      }

      hlen_tot = (u16_t)(hlen_tot + hlen);

      /* check payload length is multiple of 8 octets when mbit is set */
      if (IP6_FRAG_MBIT(frag_hdr) && (IP6H_PLEN(ip6hdr) & 0x7)) {
        /* move payload pointer back to ip header */
        (void)pbuf_header_force(p, (s16_t)((u8_t*)p->payload - (const u8_t *)ip6_current_header()));
        /* ipv6 payload length is not multiple of 8 octets */
        icmp6_param_problem(p, ICMP6_PP_FIELD, LWIP_PACKED_CAST(const void *, &ip6hdr->_plen));
        LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with invalid payload length dropped\n"));
        pbuf_free(p);
        IP6_FRAG_STATS_INC(ip6_frag.lenerr);
        IP6_FRAG_STATS_INC(ip6_frag.drop);
        goto ip6_input_cleanup;
      }

      /* Offset == 0 and more_fragments == 0? */
      if ((frag_hdr->_fragment_offset &
           PP_HTONS(IP6_FRAG_OFFSET_MASK | IP6_FRAG_MORE_FLAG)) == 0) {
        /* This is a 1-fragment packet. Skip this header and continue. */
        pbuf_remove_header(p, hlen);
      } else {
#if LWIP_IPV6_REASS
        /* reassemble the packet */
        ip_data.current_ip_header_tot_len = hlen_tot;
        p = ip6_reass(p);
        /* packet not fully reassembled yet? */
        if (p == NULL) {
          goto ip6_input_cleanup;
        }

        /* Returned p point to IPv6 header.
         * Update all our variables and pointers and continue. */
        ip6hdr = (struct ip6_hdr *)p->payload;
        nexth = &IP6H_NEXTH(ip6hdr);
        hlen = hlen_tot = IP6_HLEN;
        assemble_flag = 1;
        pbuf_remove_header(p, IP6_HLEN);

#else /* LWIP_IPV6_REASS */
        /* free (drop) packet pbufs */
        LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Fragment header dropped (with LWIP_IPV6_REASS==0)\n"));
        pbuf_free(p);
        IP6_STATS_INC(ip6.opterr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
#endif /* LWIP_IPV6_REASS */
      }
      break;
    }
    default:
      goto options_done;
    }

    if ((assemble_flag == 0) && (*nexth == IP6_NEXTH_HOPBYHOP)) {
      /* move payload pointer back to ip header */
      (void)pbuf_header_force(p, (s16_t)((u8_t*)p->payload - (const u8_t *)ip6_current_header()));
      /* Hop-by-Hop header comes only as a first option */
      icmp6_param_problem(p, ICMP6_PP_HEADER, nexth);
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Hop-by-Hop options header dropped (only valid as a first option)\n"));
      pbuf_free(p);
      IP6_STATS_INC(ip6.opterr);
      IP6_STATS_INC(ip6.drop);
      goto ip6_input_cleanup;
    }
  }

options_done:

    /* p points to IPv6 header again for raw_input. */
  pbuf_add_header_force(p, hlen_tot);
  /* send to upper layers */
  LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: \n"));
  ip6_debug_print(p);
  LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: p->len %"U16_F" p->tot_len %"U16_F"\n", p->len, p->tot_len));

  ip_data.current_ip_header_tot_len = hlen_tot;
#if IP_STATS
  /* Successfully received bytes */
  STATS_INC_NUM(ip6.ip_rx_bytes, p->tot_len);
#endif

  struct ip6_hdr *ip6hdrtmp = (struct ip6_hdr *)p->payload;
  u8_t proto = IP6H_NEXTH(ip6hdrtmp);
  s8_t is_chksum_invalid = 0;
  /* fragmentation data all merged will be given app */
  if ((proto == IP6_NEXTH_FRAGMENT) || (proto == IP6_NEXTH_HOPBYHOP)) {
    proto = *nexth;
  }
#if LWIP_RIPPLE && defined(LWIP_NA_PROXY) && LWIP_NA_PROXY
  if ((p->na_proxy == lwIP_TRUE) && (*nexth != IP6_NEXTH_ICMP6)) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_input:not icmp6, na_proxy return..\n"));
    (void)pbuf_free(p);
    goto ip6_input_cleanup;
  }
#endif
  /* Point to payload. */
  pbuf_remove_header(p, hlen_tot);
#if LWIP_RAW
  /*
   * @page RFC-2292 RFC-2292
   * @par Compliant Sections
   * Section 3. IPv6 Raw Sockets
   * Section 3.1. Checksums
   * Section 3.2. ICMPv6 Type Filtering
   * @par Behavior Description
   *      Support IPv6 raw sockets.
   *      Support IPv6 raw checksums.
   *      Support ICMPv6 Type Filtering.
   * @par Non-Compliant Section
   * Section 4. Ancillary Data
   * @par Behavior Description
   *      Ancillary data is not supported.
   */
  /* raw input did not eat the packet? */
  raw_status = raw_input6(p, proto, &is_chksum_invalid);
  if (is_chksum_invalid) {
    /* checksum validation is failed */
    /* It means packet is not given to application either due to checksum failure or some
     * or some other recv failure at the application side.
     */
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_input:Failed in checksum validation proto = %"U16_F" \n", proto));
    (void)pbuf_free(p);
    IP6_STATS_INC(ip6.chkerr);
    IP6_STATS_INC(ip6.drop);
    goto ip6_input_cleanup;
  }

#endif /* LWIP_RAW */
  switch (*nexth) {
  case IP6_NEXTH_NONE:
    pbuf_free(p);
    break;
#if LWIP_UDP
  case IP6_NEXTH_UDP:
#if LWIP_UDPLITE
  case IP6_NEXTH_UDPLITE:
#endif /* LWIP_UDPLITE */
    udp_input(p, inp);
    break;
#endif /* LWIP_UDP */
#if LWIP_TCP
  case IP6_NEXTH_TCP:
    tcp_input(p, inp);
    break;
#endif /* LWIP_TCP */
#if LWIP_ICMP6
  case IP6_NEXTH_ICMP6:
    icmp6_input(p, inp);
    break;
#endif /* LWIP_ICMP */
  default:
#if LWIP_RAW
      if (raw_status != RAW_INPUT_NONE) {
        /* @todo: ipv6 mib in-delivers? */
      } else
#endif /* LWIP_RAW */
      {
#if LWIP_ICMP6
      /* p points to IPv6 header again for raw_input. */
      pbuf_add_header_force(p, hlen_tot);
      /* send ICMP parameter problem unless it was a multicast or ICMPv6 */
      if ((!ip6_addr_ismulticast(ip6_current_dest_addr())) &&
          (IP6H_NEXTH(ip6hdr) != IP6_NEXTH_ICMP6)) {
        icmp6_param_problem(p, ICMP6_PP_HEADER, nexth);
      }
#endif /* LWIP_ICMP */
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip6_input: Unsupported transport protocol %"U16_F"\n", (u16_t)IP6H_NEXTH(ip6hdr)));
      IP6_STATS_INC(ip6.proterr);
      IP6_STATS_INC(ip6.drop);
    }
    pbuf_free(p);
    break;
  }

ip6_input_cleanup:
  ip_data.current_netif = NULL;
  ip_data.current_input_netif = NULL;
  ip_data.current_ip6_header = NULL;
  ip_data.current_ip_header_tot_len = 0;
  ip6_addr_set_zero(ip6_current_src_addr());
  ip6_addr_set_zero(ip6_current_dest_addr());

  return ERR_OK;
}


/**
 * Sends an IPv6 packet on a network interface. This function constructs
 * the IPv6 header. If the source IPv6 address is NULL, the IPv6 "ANY" address is
 * used as source (usually during network startup). If the source IPv6 address it
 * IP6_ADDR_ANY, the most appropriate IPv6 address of the outgoing network
 * interface is filled in as source address. If the destination IPv6 address is
 * LWIP_IP_HDRINCL, p is assumed to already include an IPv6 header and
 * p->payload points to it instead of the data.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
            protocol header; if dest == LWIP_IP_HDRINCL, p already includes an
            IPv6 header and p->payload points to that IPv6 header)
 * @param src the source IPv6 address to send from (if src == IP6_ADDR_ANY, an
 *         IP address of the netif is selected and used as source address.
 *         if src == NULL, IP6_ADDR_ANY is used as source) (src is possibly not
 *         properly zoned)
 * @param dest the destination IPv6 address to send the packet to (possibly not
 *             properly zoned)
 * @param hl the Hop Limit value to be set in the IPv6 header
 * @param tc the Traffic Class value to be set in the IPv6 header
 * @param nexth the Next Header to be set in the IPv6 header
 * @param netif the netif on which to send this packet
 * @return ERR_OK if the packet was sent OK
 *         ERR_BUF if p doesn't have enough space for IPv6/LINK headers
 *         returns errors returned by netif->output_ip6
 */
err_t
ip6_output_if(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
              u8_t hl, u8_t tc,
              u8_t nexth, struct netif *netif)
{
  const ip6_addr_t *src_used = src;
  if (dest != LWIP_IP_HDRINCL) {
    if (src != NULL && ip6_addr_isany(src)) {
      src_used = ip_2_ip6(ip6_select_source_address(netif, dest));
      if ((src_used == NULL) || ip6_addr_isany(src_used)) {
        /* No appropriate source address was found for this packet. */
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip6_output: No suitable source address for packet.\n"));
        IP6_STATS_INC(ip6.rterr);
        return ERR_RTE;
      }
    }
  }
  return ip6_output_if_src(p, src_used, dest, hl, tc, nexth, netif);
}

/**
 * Same as ip6_output_if() but 'src' address is not replaced by netif address
 * when it is 'any'.
 */
err_t
ip6_output_if_src(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
                  u8_t hl, u8_t tc,
                  u8_t nexth, struct netif *netif)
{
  struct ip6_hdr *ip6hdr;
  ip6_addr_t dest_addr;

  LWIP_ASSERT_CORE_LOCKED();
  LWIP_IP_CHECK_PBUF_REF_COUNT_FOR_TX(p);

  /* Should the IPv6 header be generated or is it already included in p? */
  if (dest != LWIP_IP_HDRINCL) {
#if LWIP_IPV6_SCOPES
    /* If the destination address is scoped but lacks a zone, add a zone now,
     * based on the outgoing interface. The lower layers (e.g., nd6) absolutely
     * require addresses to be properly zoned for correctness. In some cases,
     * earlier attempts will have been made to add a zone to the destination,
     * but this function is the only one that is called in all (other) cases,
     * so we must do this here. */
    if (ip6_addr_lacks_zone(dest, IP6_UNKNOWN)) {
      ip6_addr_copy(dest_addr, *dest);
      ip6_addr_assign_zone(&dest_addr, IP6_UNKNOWN, netif);
      dest = &dest_addr;
    }
#endif /* LWIP_IPV6_SCOPES */

#if LWIP_RIPPLE
    if (nexth == IP6_NEXTH_ICMP6) {
      /* check ICMP type is neither Echo request nor Echo reply */
      if ((*((u8_t *)p->payload) != ICMP6_TYPE_EREQ) && (*((u8_t *)p->payload) != ICMP6_TYPE_EREP)) {
        p->flags |= PBUF_FLAG_CTRL_PKT;
#if LWIP_SO_PRIORITY
        p->priority = LWIP_PKT_PRIORITY_CTRL;
#endif /* LWIP_SO_PRIORITY */
      }
    }
#endif /* LWIP_RIPPLE */
    /* generate IPv6 header */
    if (pbuf_add_header(p, IP6_HLEN)) {
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip6_output: not enough room for IPv6 header in pbuf\n"));
      IP6_STATS_INC(ip6.err);
      return ERR_BUF;
    }

    ip6hdr = (struct ip6_hdr *)p->payload;
    LWIP_ASSERT("check that first pbuf can hold struct ip6_hdr",
               (p->len >= sizeof(struct ip6_hdr)));

    IP6H_HOPLIM_SET(ip6hdr, hl);
    IP6H_NEXTH_SET(ip6hdr, nexth);

    /* dest cannot be NULL here */
    ip6_addr_copy_to_packed(ip6hdr->dest, *dest);

    IP6H_VTCFL_SET(ip6hdr, 6, tc, 0);
    IP6H_PLEN_SET(ip6hdr, (u16_t)(p->tot_len - IP6_HLEN));

    if (src == NULL) {
      src = IP6_ADDR_ANY6;
    }
    /* src cannot be NULL here */
    ip6_addr_copy_to_packed(ip6hdr->src, *src);

  } else {
    /* IP header already included in p */
    ip6hdr = (struct ip6_hdr *)p->payload;
    ip6_addr_copy_from_packed(dest_addr, ip6hdr->dest);
    ip6_addr_assign_zone(&dest_addr, IP6_UNKNOWN, netif);
    dest = &dest_addr;
  }

  IP6_STATS_INC(ip6.xmit);
#if IP_STATS
  STATS_INC_NUM(ip6.ip_tx_bytes, p->tot_len);
#endif

  LWIP_DEBUGF(IP6_DEBUG, ("ip6_output_if: %s%"U16_F"\n", netif->name, (u16_t)netif->num));
  ip6_debug_print(p);

#if ENABLE_LOOPBACK
  {
    int i;
#if !LWIP_HAVE_LOOPIF
    if (ip6_addr_isloopback(dest)) {
      return netif_loop_output(netif, p);
    }
#endif /* !LWIP_HAVE_LOOPIF */
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_cmp(dest, netif_ip6_addr(netif, i))) {
        /* Packet to self, enqueue it for loopback */
        LWIP_DEBUGF(IP6_DEBUG, ("netif_loop_output()\n"));
        return netif_loop_output(netif, p);
      }
    }
  }
#if LWIP_MULTICAST_TX_OPTIONS
  if ((p->flags & PBUF_FLAG_MCASTLOOP) != 0) {
    netif_loop_output(netif, p);
  }
#endif /* LWIP_MULTICAST_TX_OPTIONS */
#endif /* ENABLE_LOOPBACK */
#if LWIP_RIPPLE
  if ((!ip6_addr_islinklocal(dest)) &&
      (!ip6_addr_ismulticast(dest)) &&
      (p->flags & PBUF_FLAG_HBH_SPACE) &&
      lwip_rpl_is_rpl_netif(netif)) {
    struct pbuf *new_p = NULL;
    /* add Hop by Hop header for rpl. If space for HBH is not allocated then pbuf will be expanded. */
    (void)ip6_route(src, dest);

    if (lwip_get_pkt_route_status()) {
      p->pkt_up = lwIP_TRUE;
    } else {
      p->pkt_up = lwIP_FALSE;
    }
    new_p = lwip_add_rpi_hdr(p, nexth, lwip_hbh_len(p), 1);
    if (new_p == NULL) {
      LWIP_ERROR("Could not add HBH header.\n", 0, ;);
      IP6_STATS_INC(ip6.err);
      return ERR_BUF;
    } else {
      p = new_p;
    }
  }
#endif /* LWIP_RIPPLE */
#if LWIP_IPV6_FRAG
  /* don't fragment if interface has mtu set to 0 [loopif] */
  if (netif_mtu6(netif) && (p->tot_len > nd6_get_destination_mtu(dest, netif))) {
    return ip6_frag(p, netif, dest);
  }
#endif /* LWIP_IPV6_FRAG */

  LWIP_DEBUGF(IP6_DEBUG, ("netif->output_ip6()\n"));
  return netif->output_ip6(netif, p, dest);
}

/**
 * Simple interface to ip6_output_if. It finds the outgoing network
 * interface and calls upon ip6_output_if to do the actual work.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
            protocol header; if dest == LWIP_IP_HDRINCL, p already includes an
            IPv6 header and p->payload points to that IPv6 header)
 * @param src the source IPv6 address to send from (if src == IP6_ADDR_ANY, an
 *         IP address of the netif is selected and used as source address.
 *         if src == NULL, IP6_ADDR_ANY is used as source)
 * @param dest the destination IPv6 address to send the packet to
 * @param hl the Hop Limit value to be set in the IPv6 header
 * @param tc the Traffic Class value to be set in the IPv6 header
 * @param nexth the Next Header to be set in the IPv6 header
 *
 * @return ERR_RTE if no route is found
 *         see ip_output_if() for more return values
 */
err_t
ip6_output(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
           u8_t hl, u8_t tc, u8_t nexth)
{
  struct netif *netif;
  struct ip6_hdr *ip6hdr;
  ip6_addr_t src_addr, dest_addr;

  LWIP_IP_CHECK_PBUF_REF_COUNT_FOR_TX(p);

  IP6_ADDR(&dest_addr, 0, 0, 0, 0);

  if (dest != LWIP_IP_HDRINCL) {
    netif = ip6_route(src, dest);
  } else {
    /* IP header included in p, read addresses. */
    ip6hdr = (struct ip6_hdr *)p->payload;
    ip6_addr_copy_from_packed(src_addr, ip6hdr->src);
    ip6_addr_copy_from_packed(dest_addr, ip6hdr->dest);
    netif = ip6_route(&src_addr, &dest_addr);
    dest = &dest_addr;
  }

  if (netif == NULL) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_output: no route for %"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F"\n",
        IP6_ADDR_BLOCK1(dest),
        IP6_ADDR_BLOCK2(dest),
        IP6_ADDR_BLOCK3(dest),
        IP6_ADDR_BLOCK4(dest),
        IP6_ADDR_BLOCK5(dest),
        IP6_ADDR_BLOCK6(dest),
        IP6_ADDR_BLOCK7(dest),
        IP6_ADDR_BLOCK8(dest)));
    IP6_STATS_INC(ip6.rterr);
    return ERR_RTE;
  }

  return ip6_output_if(p, src, dest, hl, tc, nexth, netif);
}


#if LWIP_NETIF_USE_HINTS
/** Like ip6_output, but takes and addr_hint pointer that is passed on to netif->addr_hint
 *  before calling ip6_output_if.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
            protocol header; if dest == LWIP_IP_HDRINCL, p already includes an
            IPv6 header and p->payload points to that IPv6 header)
 * @param src the source IPv6 address to send from (if src == IP6_ADDR_ANY, an
 *         IP address of the netif is selected and used as source address.
 *         if src == NULL, IP6_ADDR_ANY is used as source)
 * @param dest the destination IPv6 address to send the packet to
 * @param hl the Hop Limit value to be set in the IPv6 header
 * @param tc the Traffic Class value to be set in the IPv6 header
 * @param nexth the Next Header to be set in the IPv6 header
 * @param netif_hint netif output hint pointer set to netif->hint before
 *        calling ip_output_if()
 *
 * @return ERR_RTE if no route is found
 *         see ip_output_if() for more return values
 */
err_t
ip6_output_hinted(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
                  u8_t hl, u8_t tc, u8_t nexth, struct netif_hint *netif_hint)
{
  struct netif *netif;
  struct ip6_hdr *ip6hdr;
  ip6_addr_t src_addr, dest_addr;
  err_t err;

  LWIP_IP_CHECK_PBUF_REF_COUNT_FOR_TX(p);

  if (dest != LWIP_IP_HDRINCL) {
    netif = ip6_route(src, dest);
  } else {
    /* IP header included in p, read addresses. */
    ip6hdr = (struct ip6_hdr *)p->payload;
    ip6_addr_copy_from_packed(src_addr, ip6hdr->src);
    ip6_addr_copy_from_packed(dest_addr, ip6hdr->dest);
    netif = ip6_route(&src_addr, &dest_addr);
    dest = &dest_addr;
  }

  if (netif == NULL) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_output: no route for %"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F"\n",
        IP6_ADDR_BLOCK1(dest),
        IP6_ADDR_BLOCK2(dest),
        IP6_ADDR_BLOCK3(dest),
        IP6_ADDR_BLOCK4(dest),
        IP6_ADDR_BLOCK5(dest),
        IP6_ADDR_BLOCK6(dest),
        IP6_ADDR_BLOCK7(dest),
        IP6_ADDR_BLOCK8(dest)));
    IP6_STATS_INC(ip6.rterr);
    return ERR_RTE;
  }

  NETIF_SET_HINTS(netif, netif_hint);
  err = ip6_output_if(p, src, dest, hl, tc, nexth, netif);
  NETIF_RESET_HINTS(netif);

  return err;
}
#endif /* LWIP_NETIF_USE_HINTS*/

#if LWIP_IPV6_MLD
/**
 * Add a hop-by-hop options header with a router alert option and padding.
 *
 * Used by MLD when sending a Multicast listener report/done message.
 *
 * @param p the packet to which we will prepend the options header
 * @param nexth the next header protocol number (e.g. IP6_NEXTH_ICMP6)
 * @param value the value of the router alert option data (e.g. IP6_ROUTER_ALERT_VALUE_MLD)
 * @return ERR_OK if hop-by-hop header was added, ERR_* otherwise
 */
err_t
ip6_options_add_hbh_ra(struct pbuf *p, u8_t nexth, u8_t value)
{
  u8_t *opt_data;
  u32_t offset = 0;
  struct ip6_hbh_hdr *hbh_hdr;
  struct ip6_opt_hdr *opt_hdr;

  /* fixed 4 bytes for router alert option and 2 bytes padding */
  const u8_t hlen = sizeof(struct ip6_hbh_hdr) + sizeof(struct ip6_opt_hdr) + IP6_ROUTER_ALERT_DLEN;
  /* Move pointer to make room for hop-by-hop options header. */
  if (pbuf_add_header(p, sizeof(struct ip6_hbh_hdr) + hlen)) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_options: no space for options header\n"));
    IP6_STATS_INC(ip6.err);
    return ERR_BUF;
  }

  /* Set fields of Hop-by-Hop header */
  hbh_hdr = (struct ip6_hbh_hdr *)p->payload;
  IP6_HBH_NEXTH(hbh_hdr) = nexth;
  hbh_hdr->_hlen = 0;
  offset = IP6_HBH_HLEN;

  /* Set router alert options to Hop-by-Hop extended option header */
  opt_hdr = (struct ip6_opt_hdr *)((u8_t *)hbh_hdr + offset);
  IP6_OPT_TYPE(opt_hdr) = IP6_ROUTER_ALERT_OPTION;
  IP6_OPT_DLEN(opt_hdr) = IP6_ROUTER_ALERT_DLEN;
  offset += IP6_OPT_HLEN;

  /* Set router alert option data */
  opt_data = (u8_t *)hbh_hdr + offset;
  opt_data[0] = value;
  opt_data[1] = 0;
  offset += IP6_OPT_DLEN(opt_hdr);

  /* add 2 bytes padding to make 8 bytes Hop-by-Hop header length */
  opt_hdr = (struct ip6_opt_hdr *)((u8_t *)hbh_hdr + offset);
  IP6_OPT_TYPE(opt_hdr) = IP6_PADN_OPTION;
  IP6_OPT_DLEN(opt_hdr) = 0;

  return ERR_OK;
}
#endif /* LWIP_IPV6_MLD */

#if LWIP_IPV6_FILTER
/*
 * Set ip filter for input packet.
 */
err_t set_ip6_filter(ip_filter_fn filter_fn)
{
    ip6_filter = filter_fn;
    return ERR_OK;
}
#endif /* LWIP_IPV6_FILTER */

#if IP6_DEBUG
/* Print an IPv6 header by using LWIP_DEBUGF
 * @param p an IPv6 packet, p->payload pointing to the IPv6 header
 */
void
ip6_debug_print(struct pbuf *p)
{
  struct ip6_hdr *ip6hdr = (struct ip6_hdr *)p->payload;

  LWIP_DEBUGF(IP6_DEBUG, ("IPv6 header:\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("| %2"U16_F" |  %3"U16_F"  |      %7"U32_F"     | (ver, class, flow)\n",
                    IP6H_V(ip6hdr),
                    IP6H_TC(ip6hdr),
                    IP6H_FL(ip6hdr)));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("|     %5"U16_F"     |  %3"U16_F"  |  %3"U16_F"  | (plen, nexth, hopl)\n",
                    IP6H_PLEN(ip6hdr),
                    IP6H_NEXTH(ip6hdr),
                    IP6H_HOPLIM(ip6hdr)));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("|  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |  %4"X32_F" | (src)\n",
                    IP6_ADDR_BLOCK1(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK2(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK3(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK4(&(ip6hdr->src))));
  LWIP_DEBUGF(IP6_DEBUG, ("|  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |\n",
                    IP6_ADDR_BLOCK5(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK6(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK7(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK8(&(ip6hdr->src))));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("|  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |  %4"X32_F" | (dest)\n",
                    IP6_ADDR_BLOCK1(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK2(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK3(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK4(&(ip6hdr->dest))));
  LWIP_DEBUGF(IP6_DEBUG, ("|  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |\n",
                    IP6_ADDR_BLOCK5(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK6(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK7(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK8(&(ip6hdr->dest))));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));

  if (IP6H_NEXTH(ip6hdr) == IP6_NEXTH_ICMP6)
  {
    struct icmpv6_hdr *icmp6hdr;
    pbuf_header(p, (s16_t)(-(s16_t)IP6_HLEN));
    icmp6hdr = (struct icmpv6_hdr *)p->payload;
    LWIP_DEBUGF(IP6_DEBUG, ("|     %8"U16_F"     |  %8"U16_F"  | (ICMPv6-Type, ICMPv6-Code)\n",
                    icmp6hdr->type,
                    icmp6hdr->code));
    pbuf_header(p, (s16_t)((s16_t)IP6_HLEN));
    LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));

  }
}
#endif /* IP6_DEBUG */

#endif /* LWIP_IPV6 */
