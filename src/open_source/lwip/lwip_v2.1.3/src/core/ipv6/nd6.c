/**
 * @file
 *
 * Neighbor discovery and stateless address autoconfiguration for IPv6.
 * Aims to be compliant with RFC 4861 (Neighbor discovery) and RFC 4862
 * (Address autoconfiguration).
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
#include "lwip/sys.h"
#include "lwip/nd6.h"
#include "lwip/priv/nd6_priv.h"
#include "lwip/prot/nd6.h"
#include "lwip/prot/icmp6.h"
#include "lwip/pbuf.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"
#include "lwip/icmp6.h"
#include "lwip/mld6.h"
#include "lwip/dhcp6.h"
#include "lwip/ip.h"
#include "lwip/stats.h"
#include "lwip/dns.h"

#include "lwip/lwip_rpl.h"
#include <string.h>

#if LWIP_NAT64
#include "lwip/nat64.h"
#endif

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif

#if LWIP_IPV6_DUP_DETECT_ATTEMPTS > IP6_ADDR_TENTATIVE_COUNT_MASK
#error LWIP_IPV6_DUP_DETECT_ATTEMPTS > IP6_ADDR_TENTATIVE_COUNT_MASK
#endif

#ifndef ND6_RA_MAX_INTERVAL
#define ND6_RA_MAX_INTERVAL 600
#endif

#ifndef ND6_RA_ROUTER_LIFETIME
#define ND6_RA_ROUTER_LIFETIME (3 * (ND6_RA_MAX_INTERVAL))
#endif

#define EUI_MAC_BROADCAST_MASK 0xff000000ul

u32_t is_dup_detect_initialized = 0;
sys_sem_t dup_addr_detect;
/* Router tables. */
struct nd6_neighbor_cache_entry neighbor_cache[LWIP_ND6_NUM_NEIGHBORS];
struct nd6_destination_cache_entry destination_cache[LWIP_ND6_NUM_DESTINATIONS];
struct nd6_prefix_list_entry prefix_list[LWIP_ND6_NUM_PREFIXES];
struct nd6_router_list_entry default_router_list[LWIP_ND6_NUM_ROUTERS];

/* Default values, can be updated by a RA message. */
u32_t reachable_time = LWIP_ND6_REACHABLE_TIME;
u32_t retrans_timer = LWIP_ND6_RETRANS_TIMER; /* @todo implement this value in timer */

/* Index for cache entries. */
static u8_t nd6_cached_neighbor_index;
static netif_addr_idx_t nd6_cached_destination_index;

/* Multicast address holder. */
static ip6_addr_t multicast_address;

#if LWIP_IPV6_SEND_ROUTER_SOLICIT
static u8_t nd6_tmr_rs_reduction;
#endif

/* Static buffer to parse RA packet options */
union ra_options {
  struct lladdr_option  lladdr;
  struct mtu_option     mtu;
  struct prefix_option  prefix;
#if LWIP_ND6_RDNSS_MAX_DNS_SERVERS
  struct rdnss_option   rdnss;
#endif
};
static union ra_options nd6_ra_buffer;
/* Static buffer to parse RD packet options (size of a minium lladdr as biggest option) */
static u8_t rd6_ra_buffer[sizeof(struct lladdr_option)];

/* Forward declarations. */
static s8_t nd6_find_neighbor_cache_entry(const ip6_addr_t *ip6addr, const struct netif *netif);
static s8_t nd6_new_neighbor_cache_entry(void);
static void nd6_free_neighbor_cache_entry(s8_t i);
static s16_t nd6_find_destination_cache_entry(const ip6_addr_t *ip6addr);
static s16_t nd6_new_destination_cache_entry(void);
static s8_t nd6_select_router(const ip6_addr_t *ip6addr, struct netif *netif);
static s8_t nd6_get_router(const ip6_addr_t *router_addr, struct netif *netif);
static s8_t nd6_new_router(const ip6_addr_t *router_addr, struct netif *netif);
static s8_t nd6_get_onlink_prefix(const ip6_addr_t *prefix, struct netif *netif);
static s8_t nd6_new_onlink_prefix(const ip6_addr_t *prefix, struct netif *netif);
static s8_t nd6_get_next_hop_entry(const ip6_addr_t *ip6addr, struct netif *netif);
static err_t nd6_queue_packet(s8_t neighbor_index, struct pbuf *q);
static void  nd6_free_router(s8_t i);
static int   nd6_neighbor_update(struct netif *inp, const struct lladdr_option *lladdr_opt, s8_t i, u32_t flags);
#if defined(LWIP_RA_PREFIX_DYNAMIC) && LWIP_RA_PREFIX_DYNAMIC
static s8_t nd6_get_onlink_prefix_with_len(const ip6_addr_t *prefix, u8_t len, struct netif *netif);
#endif
static void nd6_send_ns(struct netif *netif, const ip6_addr_t *target_addr, u8_t flags);
static void nd6_send_neighbor_cache_probe(struct nd6_neighbor_cache_entry *entry, u8_t flags);

#if LWIP_ND6_QUEUEING
static void nd6_free_q(struct nd6_q_entry *q);
#else /* LWIP_ND6_QUEUEING */
#define nd6_free_q(q) pbuf_free(q)
#endif /* LWIP_ND6_QUEUEING */
static void nd6_send_q(s8_t i);

/* Codenomicon for ICMPv6 fails due to redirect. */
struct nd6_opt_hdr {
  u8_t nd_opt_type;
  u8_t nd_opt_len;
} nd6_opt_hdr_t;

#ifdef LWIP_TESTBED
void nd6_remove_netif_neighbor_cache_entries(struct netif *netif)
{
  int i;

  /* iterate through neighbour cache */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (neighbor_cache[i].netif == netif) {
      neighbor_cache[i].isrouter = 0;
      nd6_free_neighbor_cache_entry(i);
    }
  }
}
#endif

#if LWIP_CONF_UPDATE_BEACON_PRIORITY
u16_t
lwip_nbr_cache_free_count(void)
{
  int index;
  u16_t cnt = 0;
  for (index = 0; index < LWIP_ND6_NUM_NEIGHBORS; index++) {
    if (neighbor_cache[index].state == ND6_NO_ENTRY) {
      cnt++;
    }
  }

  return cnt;
}
#endif

#if LWIP_RIPPLE
void *
nd6_add_default_router(const ip6_addr_t *router_addr,
                       u32_t lifetime, /* in seconds mostly */
                       struct netif *inp)
{
  /* query if entry already exists */
  s16_t idx;

  /* Get the matching default router entry. */
  idx = nd6_get_router(router_addr, inp);
  if (idx < 0) {
    /* Create a new router entry. */
    idx = nd6_new_router(router_addr, inp);
    if (idx < 0) {
      /* Could not create a new router entry. */
      ND6_STATS_INC(nd6.cache_full);
      return NULL;
    }
  }

  /* Re-set invalidation timer. */
  if (lifetime != 0) {
    default_router_list[idx].invalidation_timer = lifetime;
  }
  default_router_list[idx].flags = 0;
  return (void *)&default_router_list[idx];
}

void
nd6_refresh_destination_cache(const ip6_addr_t *nexthop_addr)
{
  int idx;
  if (nexthop_addr == NULL) {
    for (idx = 0; idx < LWIP_ND6_NUM_DESTINATIONS; idx++) {
      destination_cache[idx].age = 0;
      destination_cache[idx].pmtu = 0;
      ip6_addr_set_any(&destination_cache[idx].destination_addr);
      ip6_addr_set_any(&destination_cache[idx].next_hop_addr);
    }
    return;
  }

  for (idx = 0; idx < LWIP_ND6_NUM_DESTINATIONS; idx++) {
    if (ip6_addr_cmp(nexthop_addr, &(destination_cache[idx].next_hop_addr))) {
      ip6_addr_set_any(&destination_cache[idx].destination_addr);
      ip6_addr_set_any(&destination_cache[idx].next_hop_addr);
    }
  }
}

void
nd6_refresh_destination_cache_by_dest(const ip6_addr_t *dest)
{
  int idx;
  if (dest == NULL) {
    return;
  }

  for (idx = 0; idx < LWIP_ND6_NUM_DESTINATIONS; idx++) {
    if (ip6_addr_cmp(dest, &(destination_cache[idx].destination_addr))) {
      ip6_addr_set_any(&destination_cache[idx].destination_addr);
      ip6_addr_set_any(&destination_cache[idx].next_hop_addr);
    }
  }
}

void
nd6_refresh_destination_cache_by_prefix_dest(const ip6_addr_t *dest, uint8_t prefix_len)
{
  int idx;
  if (dest == NULL) {
    return;
  }

  for (idx = 0; idx < LWIP_ND6_NUM_DESTINATIONS; idx++) {
    if (ip6_addr_prefix_netcmp(dest, &(destination_cache[idx].destination_addr), prefix_len)) {
      ip6_addr_set_any(&destination_cache[idx].destination_addr);
      ip6_addr_set_any(&destination_cache[idx].next_hop_addr);
    }
  }
}

#if LWIP_ND6_DESTINATIONS_OLDTIME
static void
nd6_refresh_destination_cache_by_age(s8_t i)
{
  LWIP_ASSERT("invalid nd6 destination_cache index\n", (i >= 0 && i < LWIP_ND6_NUM_DESTINATIONS));
  s32_t k;
  if (destination_cache[i].age > LWIP_ND6_DESTINATIONS_OLDTIME) {
    for (k = 0; k < LWIP_ND6_NUM_NEIGHBORS; k++) {
      if (ip6_addr_cmp(&neighbor_cache[k].next_hop_address, &destination_cache[i].next_hop_addr)) {
        ip6_addr_set_any(&destination_cache[i].destination_addr);
        ip6_addr_set_any(&destination_cache[i].next_hop_addr);
        destination_cache[i].age = 0;
      }
    }
  }
}
#endif /* LWIP_ND6_DESTINATIONS_OLDTIME */

void
nd6_remove_default_router(void *default_route)
{
  struct nd6_router_list_entry *entry = NULL;

  if (default_route == NULL) {
    return;
  }

  entry = (struct nd6_router_list_entry *)default_route;

  if (entry->neighbor_entry != NULL) {
    nd6_refresh_destination_cache(&(entry->neighbor_entry->next_hop_address));
    entry->neighbor_entry->isrouter = 0;
    entry->neighbor_entry = NULL;
    entry->invalidation_timer = 0;
    entry->flags = 0;
  }
}

err_t
nd6_get_default_router_ip(ip6_addr_t *router_addr, void *default_route)
{
  struct nd6_router_list_entry *entry = NULL;

  if ((router_addr == NULL) || (default_route == NULL)) {
    return ERR_VAL;
  }

  entry = (struct nd6_router_list_entry *)default_route;

  if (entry->neighbor_entry == NULL) {
    return ERR_VAL;
  }

  ip6_addr_copy_ptr(router_addr, &entry->neighbor_entry->next_hop_address);

  return ERR_OK;
}
#endif /* LWIP_RIPPLE */
#if LWIP_RIPPLE || (defined LWIP_TESTBED)

err_t
nd6_add_neighbor_cache_entry(struct netif *netif,
                             const ip6_addr_t *nbr_addr, const u8_t *addr,
                             const u8_t addrlen, u8_t uc_reason2_add, void *pv_data)
{
  int idx;
  LWIP_UNUSED_ARG(uc_reason2_add);
  LWIP_UNUSED_ARG(pv_data);

  if ((addr == NULL) || (nbr_addr == NULL)) {
    return ERR_VAL;
  }
  idx = nd6_find_neighbor_cache_entry(nbr_addr, netif);
  if (idx >= 0) {
    /* We already have a record for the solicitor. */
    if (neighbor_cache[idx].state == ND6_INCOMPLETE) {
      neighbor_cache[idx].netif = netif;
      if (memcpy_s(neighbor_cache[idx].lladdr, NETIF_MAX_HWADDR_LEN, addr, addrlen) != EOK) {
        ND6_STATS_INC(nd6.memerr);
        return ERR_MEM;
      }
      neighbor_cache[idx].lladdrlen = addrlen;

      /* Delay probe in case we get confirmation of reachability from upper layer (TCP). */
      neighbor_cache[idx].state = ND6_DELAY;
      neighbor_cache[idx].counter.delay_time = LWIP_ND6_DELAY_FIRST_PROBE_TIME / ND6_TMR_INTERVAL;
    }

    LWIP_DEBUGF_LOG0(IP6_DEBUG, "ND6: Found NBR \n");
  } else {
    /*
     * Add their IPv6 address and link-layer address to neighbor cache.
     * We will need it at least to send a unicast NA message, but most
     * likely we will also be communicating with this node soon.
     */
    idx = nd6_new_neighbor_cache_entry();
    if (idx < 0) {
      /*
       * We couldn't assign a cache entry for this neighbor.
       * we won't be able to reply. drop it.
       */
      ND6_STATS_INC(nd6.cache_full);
      return ERR_MEM;
    }

    neighbor_cache[idx].netif = netif;
    if (memcpy_s(neighbor_cache[idx].lladdr, NETIF_MAX_HWADDR_LEN, addr, addrlen) != EOK) {
      ND6_STATS_INC(nd6.memerr);
      return ERR_MEM;
    }
    neighbor_cache[idx].lladdrlen = addrlen;
    ip6_addr_set(&(neighbor_cache[idx].next_hop_address), nbr_addr);

    /* Neighbors added by the RPL module we consider to be reachable */
    neighbor_cache[idx].state = ND6_REACHABLE;
    neighbor_cache[idx].counter.reachable_time = reachable_time;
  }

  return ERR_OK;
}
#endif /* LWIP_RIPPLE || (defined LWIP_TESTBED) */

int nd6_validate_options(u8_t *opt, int opt_len)
{
  struct nd6_opt_hdr *nd_opt = (struct nd6_opt_hdr *)opt;
  int cnt = 0;
  if ((nd_opt == NULL) || (opt_len < 0)) {
    return -1;
  }
  while (opt_len) {
    int l;
    if ((u32_t)opt_len < sizeof(struct nd6_opt_hdr)) {
      return -1;
    }
    l = nd_opt->nd_opt_len << 3;  // multiply by 8 bytes
    if ((opt_len < l) || (l == 0)) {
      return -1;
    }
    opt_len -= l;
    nd_opt = (struct nd6_opt_hdr *)(((char *)nd_opt) + l);
    cnt++;
  }
  return cnt;
}

static int
nd6_neighbor_update(struct netif *inp, const struct lladdr_option *lladdr_opt, s8_t i, u32_t flags)
{
  s32_t is_llchange = 0; /* Link layer address changes */
  u8_t is_solicited;     /* Solicited ND message */
  u8_t is_override;      /* Override LLADDR message */
  u8_t is_router;        /* Is Router Message */
  s8_t idx;              /* Router Index */

  /* Get solicited flag */
  is_solicited = (flags & ND6_FLAG_SOLICITED);
  /* Get override flag */
  is_override  = (flags & ND6_FLAG_OVERRIDE);

  /*
   * If the target's Neighbor Cache entry is in the INCOMPLETE state when
   * the advertisement is received, one of two things happens.  If the
   * link layer has addresses and no Target Link-Layer Address option is
   * included, the receiving node SHOULD silently discard the received
   * advertisement.  Otherwise, the receiving node performs the following
   * steps:

   - It records the link-layer address in the Neighbor Cache entry.

   - If the advertisement's Solicited flag is set, the state of the
     entry is set to REACHABLE; otherwise, it is set to STALE.

   - It sets the IsRouter flag in the cache entry based on the Router
     flag in the received advertisement.

   - It sends any packets queued for the neighbor awaiting address
     resolution.
  */
  if (neighbor_cache[i].state == ND6_INCOMPLETE) {
    if (lladdr_opt == NULL) {
      /* Not a valid message */
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ICMPv6 NA: Invalid Option.\n"));
      ND6_STATS_INC(nd6.proterr);
      return -1;
    }

    /* - It records the link-layer address in the Neighbor Cache entry. */
    if (memcpy_s(neighbor_cache[i].lladdr, NETIF_MAX_HWADDR_LEN, lladdr_opt->addr, inp->hwaddr_len) != EOK) {
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ICMPv6 NA: memcpy_s error.\n"));
      ND6_STATS_INC(nd6.memerr);
      return -1;
    }
    neighbor_cache[i].lladdrlen = inp->hwaddr_len;

    /*
     * - If the advertisement's Solicited flag is set, the state of the entry is set to REACHABLE;
     *   otherwise, it is set to STALE.
     */
    if (is_solicited) {
      neighbor_cache[i].state = ND6_REACHABLE;
      neighbor_cache[i].counter.reachable_time = reachable_time;
    } else {
      neighbor_cache[i].state = ND6_STALE;
    }
    /* save the interface */
    neighbor_cache[i].netif = inp;
  } else {
    /* Check is there LLADDR change. */
    if (lladdr_opt != NULL) {
      is_llchange = !lwip_lladdr_cmp(neighbor_cache[i].lladdr, lladdr_opt->addr, neighbor_cache[i].lladdrlen);
    }
    /*
     * If the Override flag is clear and the supplied link-layer address differs from that in the cache,
     * then one of two actions takes place:
     *    a. If the state of the entry is REACHABLE, set it to STALE, but do not update the entry in any other way.
     */
    if (!is_override && is_llchange) {
      if (neighbor_cache[i].state == ND6_REACHABLE) {
        neighbor_cache[i].state = ND6_STALE;
      }
      /* b. Otherwise, the received advertisement should be ignored and MUST NOT update the cache. */
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING,
                  ("ICMPv6 NA: Override flag is clear and LLADDR differs from the cache.\n"));
      return -1;
    } else {
      /*
       *  - The link-layer address in the Target Link-Layer Address option MUST be inserted in the cache
       *    (if one is supplied and differs from the already recorded address)
       */
      if (is_override || ((lladdr_opt != NULL) && !is_llchange) || (lladdr_opt == NULL)) {
        /* If target LLADDR option present, insert to cache */
        if (lladdr_opt != NULL) {
          if (memcpy_s(neighbor_cache[i].lladdr, NETIF_MAX_HWADDR_LEN, lladdr_opt->addr, inp->hwaddr_len) != EOK) {
            LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ICMPv6 NA: memcpy_s error.\n"));
            ND6_STATS_INC(nd6.memerr);
            return -1;
          }
          neighbor_cache[i].lladdrlen = inp->hwaddr_len;
        }
        /*
         * - If the advertisement's Solicited flag is set, the state of the entry is set to REACHABLE;
         *   otherwise, it is set to STALE.
         */
        if (is_solicited) {
          neighbor_cache[i].state = ND6_REACHABLE;
          neighbor_cache[i].counter.reachable_time = reachable_time;
        } else if (is_llchange) {
          neighbor_cache[i].state = ND6_STALE;
        }
      }
    }
  }

  /* Get router flag */
  is_router = (flags & ND6_FLAG_ROUTER) ? 1 : 0;
#if LWIP_RIPPLE && LWIP_ND6_ROUTER
  /* the router should not record other router */
  if (inp->forwarding != 0) {
    is_router = 0;
  }
#endif

  /* In those cases where the IsRouter flag changes from TRUE to FALSE as a result of this update,
   * the node MUST remove that router from the Default Router List and update the Destination Cache entries
   * for all destinations using that neighbor as a router as specified in RFC4861 Section 7.3.3.
   */
  if (neighbor_cache[i].isrouter && !is_router) {
    idx = nd6_get_router(ip6_current_src_addr(), inp);
    /* Remove from Default Router List */
    nd6_free_router(idx);
    neighbor_cache[i].isrouter = is_router;
  }

  /* Send queued packets, if any. */
  if (neighbor_cache[i].q != NULL) {
    nd6_send_q(i);
  }

  return 1;
}

/**
 * A local address has been determined to be a duplicate. Take the appropriate
 * action(s) on the address and the interface as a whole.
 *
 * @param netif the netif that owns the address
 * @param addr_idx the index of the address detected to be a duplicate
 */
static void
nd6_duplicate_addr_detected(struct netif *netif, s8_t addr_idx)
{

  /* Mark the address as duplicate, but leave its lifetimes alone. If this was
   * a manually assigned address, it will remain in existence as duplicate, and
   * as such be unusable for any practical purposes until manual intervention.
   * If this was an autogenerated address, the address will follow normal
   * expiration rules, and thus disappear once its valid lifetime expires. */
  netif_ip6_addr_set_state(netif, addr_idx, IP6_ADDR_DUPLICATED);

#if LWIP_IPV6_AUTOCONFIG
  /* If the affected address was the link-local address that we use to generate
   * all other addresses, then we should not continue to use those derived
   * addresses either, so mark them as duplicate as well. For autoconfig-only
   * setups, this will make the interface effectively unusable, approaching the
   * intention of RFC 4862 Sec. 5.4.5. @todo implement the full requirements */
  if (addr_idx == 0) {
    s8_t i;
    for (i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (!ip6_addr_isinvalid(netif_ip6_addr_state(netif, i)) &&
          !netif_ip6_addr_isstatic(netif, i)) {
        netif_ip6_addr_set_state(netif, i, IP6_ADDR_DUPLICATED);
      }
    }
  }
#endif /* LWIP_IPV6_AUTOCONFIG */
}

#if LWIP_IPV6_AUTOCONFIG
/**
 * We received a router advertisement that contains a prefix with the
 * autoconfiguration flag set. Add or update an associated autogenerated
 * address.
 *
 * @param netif the netif on which the router advertisement arrived
 * @param prefix_opt a pointer to the prefix option data
 * @param prefix_addr an aligned copy of the prefix address
 */
static void
nd6_process_autoconfig_prefix(struct netif *netif,
  struct prefix_option *prefix_opt, const ip6_addr_t *prefix_addr)
{
  ip6_addr_t ip6addr;
  u32_t valid_life, pref_life;
  u8_t addr_state;
  s8_t i, free_idx;

  /* The caller already checks RFC 4862 Sec. 5.5.3 points (a) and (b). We do
   * the rest, starting with checks for (c) and (d) here. */
  valid_life = lwip_htonl(prefix_opt->valid_lifetime);
  pref_life = lwip_htonl(prefix_opt->preferred_lifetime);
  if (pref_life > valid_life || ((prefix_opt->prefix_length != 64)
#if defined(LWIP_RA_PREFIX_DYNAMIC) && LWIP_RA_PREFIX_DYNAMIC
                                   && (prefix_opt->prefix_length != LWIP_RA_PREFIX_LONG)
#endif
                                  )) {
    return; /* silently ignore this prefix for autoconfiguration purposes */
  }

  /* If an autogenerated address already exists for this prefix, update its
   * lifetimes. An address is considered autogenerated if 1) it is not static
   * (i.e., manually assigned), and 2) there is an advertised autoconfiguration
   * prefix for it (the one we are processing here). This does not necessarily
   * exclude the possibility that the address was actually assigned by, say,
   * DHCPv6. If that distinction becomes important in the future, more state
   * must be kept. As explained elsewhere we also update lifetimes of tentative
   * and duplicate addresses. Skip address slot 0 (the link-local address). */
  for (i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    addr_state = netif_ip6_addr_state(netif, i);
    if (!ip6_addr_isinvalid(addr_state) && !netif_ip6_addr_isstatic(netif, i) &&
        !netif_ip6_addr_isdhcp6(netif, i) && ip6_addr_netcmp(prefix_addr, netif_ip6_addr(netif, i))) {
      /* Update the valid lifetime, as per RFC 4862 Sec. 5.5.3 point (e).
       * The valid lifetime will never drop to zero as a result of this. */
      u32_t remaining_life = netif_ip6_addr_valid_life(netif, i);
      if (valid_life > ND6_2HRS || valid_life > remaining_life) {
        netif_ip6_addr_set_valid_life(netif, i, valid_life);
      } else if (remaining_life > ND6_2HRS) {
        netif_ip6_addr_set_valid_life(netif, i, ND6_2HRS);
      }
      LWIP_ASSERT("bad valid lifetime", !netif_ip6_addr_isstatic(netif, i));
      /* Update the preferred lifetime. No bounds checks are needed here. In
       * rare cases the advertisement may un-deprecate the address, though.
       * Deprecation is left to the timer code where it is handled anyway. */
      if (pref_life > 0 && addr_state == IP6_ADDR_DEPRECATED) {
        netif_ip6_addr_set_state(netif, i, IP6_ADDR_PREFERRED);
      }
      netif_ip6_addr_set_pref_life(netif, i, pref_life);
      return; /* there should be at most one matching address */
    }
  }

  /* No autogenerated address exists for this prefix yet. See if we can add a
   * new one. However, if IPv6 autoconfiguration is administratively disabled,
   * do not generate new addresses, but do keep updating lifetimes for existing
   * addresses. Also, when adding new addresses, we must protect explicitly
   * against a valid lifetime of zero, because again, we use that as a special
   * value. The generated address would otherwise expire immediately anyway.
   * Finally, the original link-local address must be usable at all. We start
   * creating addresses even if the link-local address is still in tentative
   * state though, and deal with the fallout of that upon DAD collision. */
  addr_state = netif_ip6_addr_state(netif, 0);
  if (!netif->ip6_autoconfig_enabled || valid_life == IP6_ADDR_LIFE_STATIC ||
      ip6_addr_isinvalid(addr_state) || ip6_addr_isduplicated(addr_state)) {
    return;
  }

  /* Construct the new address that we intend to use, and then see if that
   * address really does not exist. It might have been added manually, after
   * all. As a side effect, find a free slot. Note that we cannot use
   * netif_add_ip6_address() here, as it would return ERR_OK if the address
   * already did exist, resulting in that address being given lifetimes. */
#if defined(LWIP_RA_PREFIX_DYNAMIC) && LWIP_RA_PREFIX_DYNAMIC
  if (prefix_opt->prefix_length == LWIP_RA_PREFIX_LONG) {
    netif_create_ip6_address_80bit_prefix(netif, prefix_addr, &ip6addr);
  } else
#endif
  {
    IP6_ADDR(&ip6addr, prefix_addr->addr[0], prefix_addr->addr[1],
             netif_ip6_addr(netif, 0)->addr[2], netif_ip6_addr(netif, 0)->addr[3]);
    ip6_addr_assign_zone(&ip6addr, IP6_UNICAST, netif);
  }

  free_idx = 0;
  for (i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (!ip6_addr_isinvalid(netif_ip6_addr_state(netif, i))) {
      if (ip6_addr_cmp(&ip6addr, netif_ip6_addr(netif, i))) {
        return; /* formed address already exists */
      }
    } else if ((free_idx == 0) && !netif_ip6_addr_isdhcp6(netif, i)) {
      free_idx = i;
    }
  }
  if (free_idx == 0) {
    return; /* no address slots available, try again on next advertisement */
  }

  /* Assign the new address to the interface. */
  ip_addr_copy_from_ip6(netif->ip6_addr[free_idx], ip6addr);
#if defined(LWIP_IP6_DYNAMIC_PREFIX) && LWIP_IP6_DYNAMIC_PREFIX
  netif->ip6_prefix_len[free_idx] = prefix_opt->prefix_length;
#endif
  netif_ip6_addr_set_valid_life(netif, free_idx, valid_life);
  netif_ip6_addr_set_pref_life(netif, free_idx, pref_life);
  /* Mark it as tentative (DAD will be performed if configured). */
  if (LWIP_IS_DAD_ENABLED(netif)) {
    netif_ip6_addr_set_state(netif, free_idx, IP6_ADDR_TENTATIVE);
  } else {
    netif_ip6_addr_set_state(netif, free_idx, IP6_ADDR_PREFERRED);
  }
}
#endif /* LWIP_IPV6_AUTOCONFIG */

#if LWIP_ND6_ROUTER
static inline struct lladdr_option *
nd6_extract_lladdr_option(struct pbuf *p, u8_t opt_type, u16_t offset)
{
  struct lladdr_option *lladdr_opt = (struct lladdr_option *)((u8_t *)p->payload + offset);
  if ((lladdr_opt->length == 0) || (lladdr_opt->type != opt_type) ||
      (p->len < (u16_t)(offset + (unsigned int)(lladdr_opt->length << 3)))) {
    lladdr_opt = NULL;
  }
  return lladdr_opt;
}

#ifdef LWIP_ND6_NETIF_PREFIX
static err_t
nd6_get_ra_prefix_from_netif(struct netif *nif, ip6_addr_t *prefix)
{
  int i;
  /* Get prefix of the first global address */
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (!ip6_addr_isvalid(netif_ip6_addr_state(nif, i))) {
      continue;
    }

    if (ip6_addr_islinklocal(netif_ip6_addr(nif, i))) {
      continue;
    }
    /* using fixed prefix len: 64 */
    prefix->addr[0] = netif_ip6_addr(nif, i)->addr[0];
    prefix->addr[1] = netif_ip6_addr(nif, i)->addr[1];
    prefix->addr[2] = 0;
    prefix->addr[3] = 0;
    break;
  }

  if (i == LWIP_IPV6_NUM_ADDRESSES) {
    return ERR_IF;
  }

  return ERR_OK;
}
#endif
#if LWIP_RIPPLE && defined(LWIP_ND6_DAG_PREFIX)
static err_t
nd6_get_ra_prefix_from_dag(const struct netif *nif, ip6_addr_t *prefix)
{
  if (lwip_rpl_get_ra_prefix(nif, prefix) != ERR_OK) {
    return ERR_VAL;
  }

  return ERR_OK;
}
#endif

static err_t
nd6_get_ra_prefix(const struct netif *nif, ip6_addr_t *prefix)
{
#if LWIP_RIPPLE && defined(LWIP_ND6_DAG_PREFIX)
  return nd6_get_ra_prefix_from_dag(nif, prefix);
#elif defined(LWIP_ND6_NETIF_PREFIX)
  return nd6_get_ra_prefix_from_netif(nif, prefix);
#else
  (void)nif;
  (void)prefix;
  return ERR_ARG;
#endif
}

#if LWIP_ND6_RDNSS_MAX_DNS_SERVERS
#define ND6_RA_RDNSS_MAX 16
#define ND6_OPTION_RDNSS_MAX_LEN  (1 + (2 * (LWIP_ND6_RDNSS_MAX_DNS_SERVERS)))
static u8_t
nd6_ra_dns_option_octets(const struct netif *nif)
{
  u8_t cnt;
  u8_t ip4cnt = 0;
  u8_t ip6cnt = 0;
  u8_t optlen = 0;

  cnt = lwip_dns_get_server_cnt(&ip4cnt, &ip6cnt);
  if ((cnt == 0) || (cnt > ND6_RA_RDNSS_MAX)) {
    return 0;
  }
  /* 2 : base 8 bytes, each ipv6 addr 16 bytes */
  optlen += 1 + (2 * ip6cnt);

#if LWIP_RIPPLE && LWIP_DNS64 && LWIP_NAT64
  if ((lwip_rpl_is_rpl_netif(nif) == lwIP_TRUE) && (lwip_rpl_is_br() == lwIP_TRUE) &&
      (nat64_status_check() == NAT64_RET_OK)) {
    /* 2 : ipv4 convert to ipv6, 16 bytes */
    optlen += 2 * ip4cnt;
  }
#else
  (void)nif;
#endif

  if (optlen > ND6_OPTION_RDNSS_MAX_LEN) {
    optlen = ND6_OPTION_RDNSS_MAX_LEN;
  }

  return optlen;
}
#endif

static u16_t
nd6_ra_option_octets(const struct netif *nif)
{
  u16_t opt_len;
  u16_t single_opt_len;
  single_opt_len = (u16_t)(((sizeof(struct prefix_option)) >> 3) +
                           (((sizeof(struct prefix_option)) & 0x07) ? 1 : 0));
  opt_len = single_opt_len;

  single_opt_len = (u16_t)(((sizeof(struct mtu_option)) >> 3) +
                           (((sizeof(struct mtu_option)) & 0x07) ? 1 : 0));
  opt_len += single_opt_len;

#if LWIP_ND6_RDNSS_MAX_DNS_SERVERS
  single_opt_len = nd6_ra_dns_option_octets(nif);
  opt_len += single_opt_len;
#else
  (void)nif;
#endif

  return opt_len;
}

#if LWIP_ND6_RDNSS_MAX_DNS_SERVERS
#define ND6_RDNSS_LIFETIME_INFINITY 0xFFFFFFFFU
static err_t
nd6_ra_fill_dns(const struct netif *nif, struct pbuf *p, u16_t *offset)
{
  err_t ret;
  u8_t dns_opt_len;
  struct rdnss_option *dns_opt = NULL;

  dns_opt_len = nd6_ra_dns_option_octets(nif);
  if (dns_opt_len <= 1) {
    return ERR_VAL;
  }

  dns_opt = (struct rdnss_option *)((u8_t *)p->payload + *offset);
  dns_opt->type = ND6_OPTION_TYPE_RDNSS;
  dns_opt->length = dns_opt_len;
  dns_opt->reserved = 0;
  dns_opt->lifetime = ND6_RDNSS_LIFETIME_INFINITY;

  ret = lwip_dns_copy_ip6server_addr(nif, (ip6_addr_t *)dns_opt->rdnss_address, (dns_opt_len - 1) >> 1);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }

  *offset += (dns_opt_len << 3);
  return ERR_OK;
}
#endif

static err_t
nd6_ra_fill_mtu(struct netif *nif, struct pbuf *p, u16_t *offset)
{
  u16_t opt_len;
  u32_t mtu;
  struct mtu_option *mtu_opt = NULL;

  opt_len = (u16_t)(((sizeof(struct mtu_option)) >> 3) +
                    (((sizeof(struct mtu_option)) & 0x07) ? 1 : 0));
  mtu_opt = (struct mtu_option *)((u8_t *)p->payload + *offset);
  mtu_opt->type = ND6_OPTION_TYPE_MTU;
  mtu_opt->length = (u8_t)opt_len;
  mtu_opt->reserved = 0;
  mtu = nif->mtu6;
#if LWIP_RIPPLE
  mtu -= lwip_hbh_len(NULL);
#endif
  mtu_opt->mtu = lwip_htonl(mtu);

  *offset += (opt_len << 3);
  return ERR_OK;
}

static err_t
nd6_ra_fill_prefix(ip6_addr_t *prefix, struct pbuf *p, u16_t *offset)
{
  struct prefix_option *prefix_opt = NULL;
  u16_t prefix_opt_len;
  prefix_opt_len = (u16_t)(((sizeof(struct prefix_option)) >> 3) +
                           (((sizeof(struct prefix_option)) & 0x07) ? 1 : 0));

  prefix_opt = (struct prefix_option *)((u8_t *)p->payload + *offset);

  prefix_opt->type = ND6_OPTION_TYPE_PREFIX_INFO;
  prefix_opt->length = prefix_opt_len;
#if LWIP_RIPPLE && defined(LWIP_RA_PREFIX_DYNAMIC) && LWIP_RA_PREFIX_DYNAMIC
  if ((lwip_ntohl(prefix->addr[2]) & EUI_MAC_BROADCAST_MASK) == EUI_MAC_BROADCAST_MASK) {
    prefix_opt->prefix_length = LWIP_RA_PREFIX_LONG;
  } else
#endif
  {
    prefix_opt->prefix_length = ND6_PREFIX_FIXED_LEN;
  }
  prefix_opt->flags = ND6_PREFIX_FLAG_ON_LINK | ND6_PREFIX_FLAG_AUTONOMOUS;
  prefix_opt->valid_lifetime = ND6_PREFIX_LIFETIME_INFINITY;
  prefix_opt->preferred_lifetime = ND6_PREFIX_LIFETIME_INFINITY;
  prefix_opt->reserved2[0] = 0;
  prefix_opt->reserved2[1] = 0;
  prefix_opt->reserved2[2] = 0;
  prefix_opt->reserved2[3] = 0;
  prefix_opt->prefix.addr[0] = prefix->addr[0];
  prefix_opt->prefix.addr[1] = prefix->addr[1];
#if LWIP_RIPPLE && defined(LWIP_RA_PREFIX_DYNAMIC) && LWIP_RA_PREFIX_DYNAMIC
  if ((lwip_ntohl(prefix->addr[2]) & EUI_MAC_BROADCAST_MASK) == EUI_MAC_BROADCAST_MASK) {
    /* 0xffff0000ul: take highest bits only */
    prefix_opt->prefix.addr[2] = lwip_htonl(lwip_ntohl(prefix->addr[2]) & 0xffff0000ul);
  } else
#endif
  {
    prefix_opt->prefix.addr[2] = 0;
  }
  prefix_opt->prefix.addr[3] = 0;

  *offset += (prefix_opt_len << 3);

  return ERR_OK;
}

static err_t
nd6_ra_fill(struct netif *nif, u8_t flags, ip6_addr_t *prefix, struct pbuf *p)
{
  u16_t offset;
  struct ra_header *ra_hdr = NULL;

  /* Set fields. */
  ra_hdr = (struct ra_header *)p->payload;

  ra_hdr->type = ICMP6_TYPE_RA;
  ra_hdr->code = 0;
  ra_hdr->chksum = 0;
  ra_hdr->current_hop_limit = 0; /* 0: unspecified */
  ra_hdr->flags = 0; /* M and O flag, 0: not managed by DHCPv6 or Other */
  ra_hdr->router_lifetime = lwip_htons(ND6_RA_ROUTER_LIFETIME);
  ra_hdr->reachable_time = 0; /* 0: unspecified, used by NUD */
  ra_hdr->retrans_timer = 0; /* 0: unspecified, used by address resolution and NUD */

  offset = (u16_t)sizeof(struct ra_header);
  if (nd6_ra_fill_prefix(prefix, p, &offset) != ERR_OK) {
    LWIP_DEBUGF_LOG0(IP6_DEBUG, "ICMPv6 RA send fail, fill ra prefix error.\n");
    return ERR_VAL;
  }

  if (nd6_ra_fill_mtu(nif, p, &offset) != ERR_OK) {
    LWIP_DEBUGF(IP6_DEBUG, ("ICMPv6 RA send fail, fill mtu error.\n"));
    return ERR_VAL;
  }

#if LWIP_ND6_RDNSS_MAX_DNS_SERVERS
  if (nd6_ra_fill_dns(nif, p, &offset) != ERR_OK) {
    LWIP_DEBUGF_LOG0(IP6_DEBUG, "ICMPv6 RA send, without filling ra dns.\n");
  }
#else
  (void)nif;
#endif

  (void)flags;

  return ERR_OK;
}

err_t
nd6_send_ra(struct netif *netif, const ip6_addr_t *target_addr, u8_t flags)
{
  struct pbuf *p = NULL;
  const ip6_addr_t *src_addr = NULL;
  const ip6_addr_t *dest_addr = NULL;
  err_t err;
  ip6_addr_t prefix;
  u16_t opt_len;
  struct ra_header *ra_hdr = NULL;

  if ((netif == NULL) || (netif->ra_enable != lwIP_TRUE) || ((netif->flags & NETIF_FLAG_UP) == 0) ||
      (ip6_addr_isinvalid(netif_ip6_addr_state(netif, 0)))) {
    LWIP_DEBUGF(IP6_DEBUG, ("ICMPv6 RA send fail, not enable.\n"));
    return ERR_IF;
  }

  if (nd6_get_ra_prefix(netif, &prefix) != ERR_OK) {
    LWIP_DEBUGF(IP6_DEBUG, ("ICMPv6 RA send fail, not prefix.\n"));
    return ERR_IF;
  }

  /*
   * RFC-4861
   * Section 4.2 : Router Advertisement Message Format
   * Source Address MUST be the link-local address assigned to the interface
   * from which this message is sent
   */
  if (!ip6_addr_isvalid(netif_ip6_addr_state(netif, 0))) {
    LWIP_DEBUGF(IP6_DEBUG, ("ICMPv6 RA send fail, no link local addr.\n"));
    return ERR_IF;
  }
  src_addr = netif_ip6_addr(netif, 0);

  /* The dest address default should be the all nodes target address. */
  if (target_addr == NULL) {
    ip6_addr_set_allnodes_linklocal(&multicast_address);
    dest_addr = &multicast_address;
  } else {
    dest_addr = target_addr;
  }

  opt_len = nd6_ra_option_octets(netif);
  p = pbuf_alloc(PBUF_IP, sizeof(struct ra_header) + (unsigned int)(opt_len << 3), PBUF_RAM);
  if (p == NULL) {
    ND6_STATS_INC(nd6.memerr);
    return ERR_BUF;
  }

  ra_hdr = (struct ra_header *)p->payload;
  if (nd6_ra_fill(netif, flags, &prefix, p) != ERR_OK) {
    LWIP_DEBUGF(IP6_DEBUG, ("ICMPv6 RA send fail, fill ra error.\n"));
    ND6_STATS_INC(nd6.err);
    (void)pbuf_free(p);
    return ERR_VAL;
  }

#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    ra_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr, dest_addr);
  }
#endif /* CHECKSUM_GEN_ICMP6 */

  ND6_STATS_INC(nd6.xmit);
  err = ip6_output_if(p, src_addr, dest_addr, LWIP_ICMP6_HL, 0, IP6_NEXTH_ICMP6, netif);
  (void)pbuf_free(p);

  return err;
}

void
nd6_rs_input(struct pbuf *p, struct netif *inp, u8_t flags)
{
  struct rs_header *rs_hdr = NULL;
  struct lladdr_option *lladdr_opt = NULL;
  u8_t naflags = 0;
  s8_t i;

  ND6_STATS_INC(nd6.recv);

  /* Check that RS header fits in packet. */
  if (p->len < sizeof(struct rs_header)) {
    (void)pbuf_free(p);
    ND6_STATS_INC(nd6.lenerr);
    ND6_STATS_INC(nd6.drop);
    return;
  }
  (void)flags;
  /* Check that RS header HL is 255. */
  if (IP6H_HOPLIM(ip6_current_header()) != ND6_HOPLIM) {
    LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ICMPv6 RS header Hop Limit is not 255, packet dropped.\n"));
    (void)pbuf_free(p);
    ND6_STATS_INC(nd6.drop);
    return;
  }
#if LWIP_ND6_ROUTER
  if (inp->forwarding != lwIP_TRUE) {
    LWIP_DEBUGF(IP6_DEBUG, ("ICMPv6 RS forwarding false, packet dropped.\n"));
    (void)pbuf_free(p);
    ND6_STATS_INC(nd6.drop);
    return;
  }
#endif
#if LWIP_RIPPLE
  if (lwip_rpl_is_rpl_netif(inp) == lwIP_FALSE) {
    LWIP_DEBUGF(IP6_DEBUG, ("not mesh netif, all rs should drop.\n"));
    (void)pbuf_free(p);
    ND6_STATS_INC(nd6.drop);
    return;
  }
#endif
  rs_hdr = (struct rs_header *)p->payload;
  if (rs_hdr->code != 0) {
    LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                ("ICMPv6 RS header (code %"X8_F") is not zero, packet dropped.\n", rs_hdr->code));
    (void)pbuf_free(p);
    ND6_STATS_INC(nd6.drop);
    return;
  }

  /* 2 : Check if there is a link-layer address provided. Only point to it if in this buffer. */
  if (p->len >= (sizeof(struct rs_header) + 2)) {
    lladdr_opt = nd6_extract_lladdr_option(p, ND6_OPTION_TYPE_SOURCE_LLADDR, (u16_t)sizeof(struct rs_header));
  } else {
    lladdr_opt = NULL;
  }

  /* just drop packets that the src addr is any, or in the rpl network, the multicast ra is not required. */
  if (ip6_addr_isany_val((*ip6_current_src_addr())) || (lladdr_opt == NULL)) {
    (void)pbuf_free(p);
    ND6_STATS_INC(nd6.drop);
    return;
  }

  /* Codenomicon for ICMPv6 fails due to redirect. */
  /* Validate incoming packet - Do not add neighbor cache entry for self */
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
    if (!ip6_addr_isinvalid(netif_ip6_addr_state(inp, i)) &&
        ip6_addr_cmp(ip6_current_src_addr(), netif_ip6_addr(inp, i))) {
      /* Not a valid message. */
      (void)pbuf_free(p);
      ND6_STATS_INC(nd6.proterr);
      ND6_STATS_INC(nd6.drop);
      return;
    }
  }
#if LWIP_RIPPLE && LWIP_RPL_RS_DAO
  if (flags == ND6_RS_FLAG_DAO) {
    if (ip6_addr_isallrouters_linklocal(ip6_current_dest_addr())) {
      /* send DAO message */
      lwip_rpl_dao_proxy(ip6_current_src_addr(), lladdr_opt->addr, inp->hwaddr_len);
      (void)pbuf_free(p);
    } else {
      /* Not a valid message. */
      (void)pbuf_free(p);
      ND6_STATS_INC(nd6.proterr);
      ND6_STATS_INC(nd6.drop);
    }
    return;
  } else
#endif
  {
    LWIP_DEBUGF(IP6_DEBUG, ("Will send RA flags[%x]\n", naflags));
    (void)nd6_send_ra(inp, ip6_current_src_addr(), naflags);
  }

  (void)pbuf_free(p);
  return;
}
#endif

#if LWIP_RIPPLE && defined(LWIP_NA_PROXY) && LWIP_NA_PROXY
static err_t
nd6_na_proxy(ip6_addr_t *src, ip6_addr_t *target, struct netif *inp)
{
  if (ip6_addr_isany_val(*src) || ip6_addr_islinklocal(target)) {
    return ERR_ARG;
  }

  if (lwip_rpl_is_rpl_netif(inp)) {
    /* only the non-mesh node be proxy */
    return lwip_rpl_nonmesh_node(src);
  } else if (lwip_rpl_is_br()) {
    /* the node behind the MBR will be proxy */
    return lwip_rpl_behind_mbr_node(target);
  }

  return ERR_VAL;
}
#endif

/**
 * Process an incoming neighbor discovery message
 *
 * @param p the nd packet, p->payload pointing to the icmpv6 header
 * @param inp the netif on which this packet was received
 */
void
nd6_input(struct pbuf *p, struct netif *inp)
{
  u8_t msg_type;
  s8_t i;
  s16_t dest_idx;

  ND6_STATS_INC(nd6.recv);

  msg_type = *((u8_t *)p->payload);
#if LWIP_RIPPLE && defined(LWIP_NA_PROXY) && LWIP_NA_PROXY
  if ((p->na_proxy == lwIP_TRUE) && (msg_type != ICMP6_TYPE_NS)) {
    (void)pbuf_free(p);
    return;
  }
#endif
  switch (msg_type) {
  case ICMP6_TYPE_NA: /* Neighbor Advertisement. */
  {
    struct na_header *na_hdr;
    struct lladdr_option *lladdr_opt;
    ip6_addr_t target_address;
    int ret;

    /* Check that na header fits in packet. */
    if (p->len < (sizeof(struct na_header))) {
      /* @todo debug message */
      pbuf_free(p);
      ND6_STATS_INC(nd6.lenerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    na_hdr = (struct na_header *)p->payload;

    /* Create an aligned, zoned copy of the target address. */
    ip6_addr_copy_from_packed(target_address, na_hdr->target_address);
    ip6_addr_assign_zone(&target_address, IP6_UNICAST, inp);

    /* Check a subset of the other RFC 4861 Sec. 7.1.2 requirements. */
    if (IP6H_HOPLIM(ip6_current_header()) != ND6_HOPLIM || na_hdr->code != 0 ||
        ip6_addr_ismulticast(&target_address)) {
      pbuf_free(p);
      ND6_STATS_INC(nd6.proterr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    /* @todo RFC MUST: if IP destination is multicast, Solicited flag is zero */
    /* @todo RFC MUST: all included options have a length greater than zero */

    /* Unsolicited NA?*/
    if (ip6_addr_ismulticast(ip6_current_dest_addr())) {
      /* This is an unsolicited NA.
       * link-layer changed?
       * part of DAD mechanism? */

      /*
       * Check RFC 4861 Sec. 7.1.2 requirements: If the IP Destination Address is a multicast address the
       * Solicited flag is zero, Silently discard any received Neighbor Advertisement.
       */
      if (na_hdr->flags & ND6_FLAG_SOLICITED) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ICMPv6 NA: solicited NA is multicasted.\n"));
        (void)pbuf_free(p);
        ND6_STATS_INC(nd6.proterr);
        ND6_STATS_INC(nd6.drop);
        return;
      }
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS
      /* If the target address matches this netif, it is a DAD response. */
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (!ip6_addr_isinvalid(netif_ip6_addr_state(inp, i)) &&
            !ip6_addr_isduplicated(netif_ip6_addr_state(inp, i)) &&
            ip6_addr_cmp(&target_address, netif_ip6_addr(inp, i))) {
          /* [Ignore the NA tgt address same as stack address ] */
          if (ip6_addr_isvalid(netif_ip6_addr_state(inp, i))) {
            /* just ignore the packet and do not process it further */
            (void)pbuf_free(p);
            ND6_STATS_INC(nd6.drop);
            return;
          }
          /* We are using a duplicate address. */
          nd6_duplicate_addr_detected(inp, i);

          if (inp->ipv6_addr_event_cb != NULL) {
            inp->ipv6_addr_event_cb(inp, *netif_ip6_addr(inp, i), netif_ip6_addr_state(inp, i));
          }

          if (is_dup_detect_initialized) {
            sys_sem_signal(&dup_addr_detect);
          }
          /* Once DAD on link local fails, it is not supposed to send/recv anything */
          if (ip6_addr_islinklocal(&target_address) && (i == 0)) {
            (void)netif_set_down(inp);
          } else {
#if LWIP_IPV6_DHCP6 && LWIP_IPV6_DHCP6_STATEFUL
            dhcp6_dad_handle(inp, &target_address);
#endif
          }
          pbuf_free(p);
          return;
        }
      }
#endif /* LWIP_IPV6_DUP_DETECT_ATTEMPTS */

      /* Check that link-layer address option also fits in packet. */
      if (p->len < (sizeof(struct na_header) + 2)) {
        /* @todo debug message */
        pbuf_free(p);
        ND6_STATS_INC(nd6.lenerr);
        ND6_STATS_INC(nd6.drop);
        return;
      }

      lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct na_header));

      if ((lladdr_opt->length == 0) || (lladdr_opt->type != ND6_OPTION_TYPE_TARGET_LLADDR) ||
          (p->len < (sizeof(struct na_header) + (lladdr_opt->length << 3)))) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ICMPv6 NA: Invalid Option.\n"));
        pbuf_free(p);
        ND6_STATS_INC(nd6.lenerr);
        ND6_STATS_INC(nd6.drop);
        return;
      }

      /* This is an unsolicited NA, most likely there was a LLADDR change. */
      i = nd6_find_neighbor_cache_entry(&target_address, inp);
      if (i < 0) {
        /* Entry is not present in the Cache,drop it. */
        ND6_STATS_INC(nd6.cache_miss);
        (void)pbuf_free(p);
        return;
      }
    } else {
      /* This is a solicited NA.
       * neighbor address resolution response?
       * neighbor unreachability detection response? */

      /* Find the cache entry corresponding to this na. */
      i = nd6_find_neighbor_cache_entry(&target_address, inp);
      if (i < 0) {
        /* We no longer care about this target address. drop it. */
        ND6_STATS_INC(nd6.cache_miss);
        pbuf_free(p);
        return;
      }

      /* Check if there is a link-layer address provided. Only point to it if in this buffer. */
      /* 2 is struct lladdr_option's type and code size */
      if (p->len >= (sizeof(struct na_header) + 2)) {
        lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct na_header));

        if ((!lladdr_opt->length) || (lladdr_opt->type != ND6_OPTION_TYPE_TARGET_LLADDR) ||
            p->len < (sizeof(struct na_header) + (lladdr_opt->length << 3))) {
          lladdr_opt = NULL;
        }
        /* Yes Option is valid */
      } else {
        /* No LLADDR option or Invalid */
        lladdr_opt = NULL;
      }
    }

    /* Update Neighbor cache entries */
    ret = nd6_neighbor_update(inp, lladdr_opt, i, na_hdr->flags);
    if (ret < 0) {
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ICMPv6 neighbor update failed ret(%"S32_F").\n", ret));
      ND6_STATS_INC(nd6.drop);
      (void)pbuf_free(p);
      return;
    }

    break; /* ICMP6_TYPE_NA */
  }
  case ICMP6_TYPE_NS: /* Neighbor solicitation. */
  {
    struct ns_header *ns_hdr;
    struct lladdr_option *lladdr_opt;
    ip6_addr_t target_address;
    u8_t accepted;
    u8_t naflags = 0;

    /* Check that ns header fits in packet. */
    if (p->len < sizeof(struct ns_header)) {
      /* @todo debug message */
      pbuf_free(p);
      ND6_STATS_INC(nd6.lenerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    ns_hdr = (struct ns_header *)p->payload;

    /* Create an aligned, zoned copy of the target address. */
    ip6_addr_copy_from_packed(target_address, ns_hdr->target_address);
    ip6_addr_assign_zone(&target_address, IP6_UNICAST, inp);

    /* Check a subset of the other RFC 4861 Sec. 7.1.1 requirements. */
    if (IP6H_HOPLIM(ip6_current_header()) != ND6_HOPLIM || ns_hdr->code != 0 ||
       ip6_addr_ismulticast(&target_address)) {
      pbuf_free(p);
      ND6_STATS_INC(nd6.proterr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    /* @todo RFC MUST: all included options have a length greater than zero */
    /* @todo RFC MUST: if IP source is 'any', destination is solicited-node multicast address */
    /* @todo RFC MUST: if IP source is 'any', there is no source LL address option */

    /* Check if there is a link-layer address provided. Only point to it if in this buffer. */
    if (p->len >= (sizeof(struct ns_header) + 2)) {
      lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct ns_header));
      if ((lladdr_opt == NULL) || (!lladdr_opt->length) || (lladdr_opt->type != ND6_OPTION_TYPE_SOURCE_LLADDR) ||
          p->len < (sizeof(struct ns_header) + (lladdr_opt->length << 3))) {
        lladdr_opt = NULL;
      } else {
        /*
         * Check RFC 4861 Sec. 7.1.1 requirements: If the IP source address is the unspecified address,
         * there is no source link-layer address option in the message.
         */
        if (ip6_addr_isany_val(*(ip6_current_src_addr()))) {
          (void)pbuf_free(p);
          ND6_STATS_INC(nd6.proterr);
          ND6_STATS_INC(nd6.drop);
          return;
        }
      }
    } else {
      lladdr_opt = NULL;
    }

    /* Check if the target address is configured on the receiving netif. */
    accepted = 0;
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
      if ((ip6_addr_isvalid(netif_ip6_addr_state(inp, i)) ||
           (ip6_addr_istentative(netif_ip6_addr_state(inp, i)) &&
           ip6_addr_isany_val((*ip6_current_src_addr())))) &&
          ip6_addr_cmp(&target_address, netif_ip6_addr(inp, i))) {
        accepted = 1;
        break;
      }
    }

#if LWIP_RIPPLE && defined(LWIP_NA_PROXY) && LWIP_NA_PROXY
    if ((accepted == 0) &&
        (nd6_na_proxy(ip6_current_src_addr(), (ip6_addr_t *)&ns_hdr->target_address, inp) == ERR_OK)) {
      accepted = 1;
    }
#endif

    /* NS not for us? */
    if (!accepted) {
      pbuf_free(p);
      return;
    }

    /* Check for ANY address in src (DAD algorithm). */
    if (ip6_addr_isany_val((*ip6_current_src_addr()))) {
      /* Sender is validating this address. */
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
        if (!ip6_addr_isinvalid(netif_ip6_addr_state(inp, i)) &&
            ip6_addr_cmp(&target_address, netif_ip6_addr(inp, i))) {
            naflags = (ND6_FLAG_OVERRIDE | ND6_SEND_FLAG_ALLNODES_DEST);
          if (ip6_addr_istentative(netif_ip6_addr_state(inp, i))) {
            /* We shouldn't use this address either. */
            nd6_duplicate_addr_detected(inp, i);
            /* do not use any of address on this interface */
            /* Once DAD on link local fails, it is not supposed to send/recv anything */
            if (ip6_addr_islinklocal(netif_ip6_addr(inp, i)) && (i == 0)) {
              (void)netif_set_down(inp);
              (void)pbuf_free(p);
              return;
            } else {
#if LWIP_IPV6_DHCP6 && LWIP_IPV6_DHCP6_STATEFUL
              dhcp6_dad_handle(inp, &target_address);
#endif
            }
          } else {
            /* Send a NA back so that the sender does not use this address. */
            nd6_send_na(inp, netif_ip6_addr(inp, i), naflags);
          }
        }
      }
    } else {
      /* Sender is trying to resolve our address. */
      /* Verify that they included their own link-layer address. */
      if (lladdr_opt == NULL) {
        /* Not a valid message. */
        pbuf_free(p);
        ND6_STATS_INC(nd6.proterr);
        ND6_STATS_INC(nd6.drop);
        return;
      }

      /* Codenomicon for ICMPv6 fails due to redirect. */
      /* Validate incoming packet - Do not add neighbor cache entry for self */
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
        if (!ip6_addr_isinvalid(netif_ip6_addr_state(inp, i)) &&
            ip6_addr_cmp(ip6_current_src_addr(), netif_ip6_addr(inp, i))) {
            /* Not a valid message. */
            (void)pbuf_free(p);
            ND6_STATS_INC(nd6.proterr);
            ND6_STATS_INC(nd6.drop);
            return;
        }
      }
      i = nd6_find_neighbor_cache_entry(ip6_current_src_addr(), inp);
      if (i>= 0) {
        /* We already have a record for the solicitor. */
        if (neighbor_cache[i].state == ND6_INCOMPLETE) {
          neighbor_cache[i].netif = inp;
          MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
          neighbor_cache[i].lladdrlen = inp->hwaddr_len;

          /* Delay probe in case we get confirmation of reachability from upper layer (TCP). */
          neighbor_cache[i].state = ND6_DELAY;
          neighbor_cache[i].counter.delay_time = LWIP_ND6_DELAY_FIRST_PROBE_TIME / ND6_TMR_INTERVAL;
        }
      } else {
        /* Add their IPv6 address and link-layer address to neighbor cache.
         * We will need it at least to send a unicast NA message, but most
         * likely we will also be communicating with this node soon. */
        i = nd6_new_neighbor_cache_entry();
        if (i < 0) {
          /* We couldn't assign a cache entry for this neighbor.
           * we won't be able to reply. drop it. */
          pbuf_free(p);
          ND6_STATS_INC(nd6.cache_full);
          return;
        }
        neighbor_cache[i].netif = inp;
        MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
        neighbor_cache[i].lladdrlen = inp->hwaddr_len;
        ip6_addr_set(&(neighbor_cache[i].next_hop_address), ip6_current_src_addr());

        /* Receiving a message does not prove reachability: only in one direction.
         * Delay probe in case we get confirmation of reachability from upper layer (TCP). */
        neighbor_cache[i].state = ND6_DELAY;
        neighbor_cache[i].counter.delay_time = LWIP_ND6_DELAY_FIRST_PROBE_TIME / ND6_TMR_INTERVAL;
      }

      /* Send back a NA for us. Allocate the reply pbuf. */
      naflags = ND6_FLAG_SOLICITED | ND6_FLAG_OVERRIDE;

      /* If the node acts as a router in the RPL mesh network it can set the ND6_FLAG_ROUTER in the NA message */
#if LWIP_RIPPLE && LWIP_ND6_ROUTER
      if ((inp->forwarding != lwIP_FALSE)
#if defined(LWIP_NA_PROXY) && LWIP_NA_PROXY
          && (p->na_proxy != lwIP_TRUE)
#endif
         ) {
        naflags |= ND6_FLAG_ROUTER;
      }
#endif

      LWIP_DEBUGF(IP6_DEBUG, ("Will send NA flags[%x]", naflags));
      nd6_send_na(inp, &target_address, naflags);
    }

    break; /* ICMP6_TYPE_NS */
  }
  case ICMP6_TYPE_RA: /* Router Advertisement. */
  {
    struct ra_header *ra_hdr;
    u8_t *buffer; /* Used to copy options. */
    u16_t offset;
    /* flag indicated that whether there was already one default router cache or need to create a new one. */
    u8_t exist_or_new;
#if LWIP_ND6_RDNSS_MAX_DNS_SERVERS
    /* There can be multiple RDNSS options per RA */
    u8_t rdnss_server_idx = 0;
#endif /* LWIP_ND6_RDNSS_MAX_DNS_SERVERS */
#if LWIP_ND6_ROUTER
    if ((inp->accept_ra != lwIP_TRUE) || (inp->forwarding != lwIP_FALSE)) {
      LWIP_DEBUGF(IP6_DEBUG, ("ICMPv6 RA not accept, packet dropped.\n"));
      (void)pbuf_free(p);
      ND6_STATS_INC(nd6.drop);
      return;
    }
#endif
    /* Check that RA header fits in packet. */
    if (p->len < sizeof(struct ra_header)) {
      /* @todo debug message */
      pbuf_free(p);
      ND6_STATS_INC(nd6.lenerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    ra_hdr = (struct ra_header *)p->payload;

    /* Check a subset of the other RFC 4861 Sec. 6.1.2 requirements. */
    if (!ip6_addr_islinklocal(ip6_current_src_addr()) ||
        IP6H_HOPLIM(ip6_current_header()) != ND6_HOPLIM || ra_hdr->code != 0) {
      pbuf_free(p);
      ND6_STATS_INC(nd6.proterr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    /* @todo RFC MUST: all included options have a length greater than zero */

    /* If we are sending RS messages, stop. */
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
    /* ensure at least one solicitation is sent (see RFC 4861, ch. 6.3.7) */
    if ((inp->rs_count < LWIP_ND6_MAX_MULTICAST_SOLICIT) ||
        (nd6_send_rs(inp) == ERR_OK)) {
      inp->rs_count = 0;
    } else {
      inp->rs_count = 1;
    }
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */

    /* Get the matching default router entry. */
    i = nd6_get_router(ip6_current_src_addr(), inp);
    if (i < 0) {
      if (lwip_htons(ra_hdr->router_lifetime) != 0) {
        /* Create a new router entry. */
        i = nd6_new_router(ip6_current_src_addr(), inp);

        if (i < 0) {
          /* Could not create a new router entry. */
          pbuf_free(p);
          ND6_STATS_INC(nd6.cache_full);
          return;
        } else {
          exist_or_new = 1;
        }
      } else {
        exist_or_new = 0;
      }
    } else {
      exist_or_new = 1;
    }

    if (exist_or_new) {
      /* RA update: Remove the entry from default router list immediately rather than waiting for timer expiry */
      if (ra_hdr->router_lifetime == 0) {
        nd6_free_router(i);
      } else {
        /* Re-set invalidation timer. */
        default_router_list[i].invalidation_timer = lwip_htons(ra_hdr->router_lifetime);
        /* Update flags in local entry (incl. preference). */
        default_router_list[i].flags = ra_hdr->flags;
      }
    }
    /* Re-set default timer values. */
#if LWIP_ND6_ALLOW_RA_UPDATES
    if (ra_hdr->retrans_timer > 0) {
      retrans_timer = lwip_htonl(ra_hdr->retrans_timer);
    }
    if (ra_hdr->reachable_time > 0) {
      reachable_time = lwip_htonl(ra_hdr->reachable_time);
    }
#endif /* LWIP_ND6_ALLOW_RA_UPDATES */

#if LWIP_IPV6_DHCP6
    /* Trigger DHCPv6 if enabled */
    dhcp6_nd6_ra_trigger(inp, ra_hdr->flags & ND6_RA_FLAG_MANAGED_ADDR_CONFIG,
      ra_hdr->flags & ND6_RA_FLAG_OTHER_CONFIG);
#endif

    /* Offset to options. */
    offset = sizeof(struct ra_header);

    /* Validate option. */
    while ((p->tot_len - offset) >= 2) {
      u8_t option_type;
      u16_t option_len;
      int option_len8 = pbuf_try_get_at(p, offset + 1);
      if (option_len8 <= 0) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                    ("IPv6 options header (optlen %d) is not specified, packet dropped.\n",
                     option_len8));
        /* read beyond end or zero length */
        goto lenerr_drop_free_return;
      }
      option_len = ((u8_t)option_len8) << 3;
      if (option_len > p->tot_len - offset) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("IPv6 options header (optlen %d) does not fit in \
          (total len %"U16_F") and (offset %"U16_F"), packet dropped.\n",
           option_len8, p->tot_len, offset));
        /* short packet (option does not fit in) */
        goto lenerr_drop_free_return;
      }
      if (p->len == p->tot_len) {
        /* no need to copy from contiguous pbuf */
        buffer = &((u8_t*)p->payload)[offset];
      } else {
        /* check if this option fits into our buffer */
        if (option_len > sizeof(nd6_ra_buffer)) {
          option_type = pbuf_get_at(p, offset);
          /* invalid option length */
          if (option_type != ND6_OPTION_TYPE_RDNSS) {
            goto lenerr_drop_free_return;
          }
          /* we allow RDNSS option to be longer - we'll just drop some servers */
          option_len = sizeof(nd6_ra_buffer);
        }
        buffer = (u8_t*)&nd6_ra_buffer;
        option_len = pbuf_copy_partial(p, &nd6_ra_buffer, option_len, offset);
      }
      option_type = buffer[0];
      switch (option_type) {
      case ND6_OPTION_TYPE_SOURCE_LLADDR:
      {
        struct lladdr_option *lladdr_opt;
        if (option_len < sizeof(struct lladdr_option)) {
          LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                      ("ND6_OPTION_TYPE_SOURCE_LLADDR: (optlen %"U16_F") is less than \
                      struct lladdr_option or not same as HWADDR length, packet dropped.\n", option_len));
          goto lenerr_drop_free_return;
        }
        lladdr_opt = (struct lladdr_option *)buffer;
        if (exist_or_new &&
            (default_router_list[i].neighbor_entry != NULL) &&
            (default_router_list[i].neighbor_entry->state == ND6_INCOMPLETE)) {
          SMEMCPY(default_router_list[i].neighbor_entry->lladdr, lladdr_opt->addr, inp->hwaddr_len);
          default_router_list[i].neighbor_entry->lladdrlen = inp->hwaddr_len;
          default_router_list[i].neighbor_entry->state = ND6_REACHABLE;
          default_router_list[i].neighbor_entry->counter.reachable_time = reachable_time;
        }
        break;
      }
      case ND6_OPTION_TYPE_MTU:
      {
        struct mtu_option *mtu_opt;
        u32_t mtu32;
        if (option_len < sizeof(struct mtu_option)) {
          goto lenerr_drop_free_return;
        }
        mtu_opt = (struct mtu_option *)buffer;
        mtu32 = lwip_htonl(mtu_opt->mtu);
        if ((mtu32 >= IP6_MIN_MTU_LENGTH) && (mtu32 <= 0xffff)) {
#if LWIP_ND6_ALLOW_RA_UPDATES
          if (inp->mtu) {
            /* don't set the mtu for IPv6 higher than the netif driver supports */
            inp->mtu6 = LWIP_MIN(LWIP_MIN(inp->mtu, inp->mtu6), (u16_t)mtu32);
          } else {
            inp->mtu6 = (u16_t)mtu32;
          }
#endif /* LWIP_ND6_ALLOW_RA_UPDATES */
        }
        break;
      }
      case ND6_OPTION_TYPE_PREFIX_INFO:
      {
        struct prefix_option *prefix_opt;
        ip6_addr_t prefix_addr;
        u8_t prefix_len;
        if (option_len < sizeof(struct prefix_option)) {
          LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                      ("ND6_OPTION_TYPE_PREFIX_INFO: (optlen %"U16_F") is less than \
                      struct prefix_option, packet dropped.\n", option_len));
          goto lenerr_drop_free_return;
        }

        prefix_opt = (struct prefix_option *)buffer;

        /* Get a memory-aligned copy of the prefix. */
        ip6_addr_copy_from_packed(prefix_addr, prefix_opt->prefix);
        ip6_addr_assign_zone(&prefix_addr, IP6_UNICAST, inp);
        prefix_len = prefix_opt->prefix_length;

        if (!ip6_addr_islinklocal(&prefix_addr) && !ip6_addr_isany(&(prefix_opt->prefix))) {
          if (prefix_opt->flags & ND6_PREFIX_FLAG_ON_LINK) {
            /* Add to on-link prefix list. */
            u32_t valid_life;
            s8_t prefix = 0;
            valid_life = lwip_htonl(prefix_opt->valid_lifetime);

            /* find cache entry for this prefix. */
            if (prefix_opt->prefix_length == 64) {
              prefix = nd6_get_onlink_prefix(&prefix_addr, inp);
            } else {
#if defined(LWIP_RA_PREFIX_DYNAMIC) && LWIP_RA_PREFIX_DYNAMIC
              prefix = nd6_get_onlink_prefix_with_len(&prefix_addr, prefix_len, inp);
#else
              LWIP_DEBUGF(IP6_DEBUG, ("on-link prefix length %"U8_F" unrecognized\n",
                                      prefix_opt->prefix_length));
              valid_life = 0; /* mark negative so that cache entry is not updated */
#endif
            }
            if (prefix < 0 && valid_life > 0) {
              /* Create a new cache entry. */
              prefix = nd6_new_onlink_prefix(&prefix_addr, inp);
            }
            if (prefix >= 0) {
              prefix_list[prefix].invalidation_timer = valid_life;
              prefix_list[prefix].prefix_len = prefix_len;
            }
          }

#if LWIP_IPV6_AUTOCONFIG
          if (prefix_opt->flags & ND6_PREFIX_FLAG_AUTONOMOUS) {
            /* Perform processing for autoconfiguration. */
            nd6_process_autoconfig_prefix(inp, prefix_opt, &prefix_addr);
          }
#endif /* LWIP_IPV6_AUTOCONFIG */
        }

        break;
      }
      case ND6_OPTION_TYPE_ROUTE_INFO:
        /* @todo implement preferred routes.
        struct route_option * route_opt;
        route_opt = (struct route_option *)buffer;*/

        break;
#if LWIP_ND6_RDNSS_MAX_DNS_SERVERS
      case ND6_OPTION_TYPE_RDNSS:
      {
        u8_t num, n;
        u16_t copy_offset = offset + SIZEOF_RDNSS_OPTION_BASE;
        struct rdnss_option * rdnss_opt;
        if (option_len < SIZEOF_RDNSS_OPTION_BASE) {
          goto lenerr_drop_free_return;
        }

        rdnss_opt = (struct rdnss_option *)buffer;
        num = (rdnss_opt->length - 1) / 2;
        for (n = 0; (rdnss_server_idx < DNS_MAX_SERVERS) && (n < num); n++, copy_offset += sizeof(ip6_addr_p_t)) {
          ip_addr_t rdnss_address;

          /* Copy directly from pbuf to get an aligned, zoned copy of the prefix. */
          if (pbuf_copy_partial(p, &rdnss_address, sizeof(ip6_addr_p_t), copy_offset) == sizeof(ip6_addr_p_t)) {
            IP_SET_TYPE_VAL(rdnss_address, IPADDR_TYPE_V6);
            ip6_addr_assign_zone(ip_2_ip6(&rdnss_address), IP6_UNKNOWN, inp);

            if (htonl(rdnss_opt->lifetime) > 0) {
              /* TODO implement Lifetime > 0 */
              dns_setserver(rdnss_server_idx++, &rdnss_address);
            } else {
              /* TODO implement DNS removal in dns.c */
              u8_t s;
              for (s = 0; s < DNS_MAX_SERVERS; s++) {
                const ip_addr_t *addr = dns_getserver(s);
                if(ip_addr_cmp(addr, &rdnss_address)) {
                  dns_setserver(s, NULL);
                }
              }
            }
          }
        }
        break;
      }
#endif /* LWIP_ND6_RDNSS_MAX_DNS_SERVERS */
      default:
        /* Unrecognized option, abort. */
        ND6_STATS_INC(nd6.proterr);
        break;
      }
      /* option length is checked earlier to be non-zero to make sure loop ends */
      offset += 8 * (u8_t)option_len8;
    }

    break; /* ICMP6_TYPE_RA */
  }
  case ICMP6_TYPE_RD: /* Redirect */
  {
    struct redirect_header *redir_hdr;
    struct lladdr_option *lladdr_opt;
    ip6_addr_t tmp;
    ip6_addr_t tmp_dest;
    ip6_addr_t destination_address, target_address;
    u8_t *buffer = NULL; /* Used to copy options. */
    u16_t offset;
    u8_t dup = 0;

    /* Check that Redir header fits in packet. */
    if (p->len < sizeof(struct redirect_header)) {
      /* @todo debug message */
      pbuf_free(p);
      ND6_STATS_INC(nd6.lenerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    redir_hdr = (struct redirect_header *)p->payload;

    /* Create an aligned, zoned copy of the destination address. */
    ip6_addr_copy_from_packed(destination_address, redir_hdr->destination_address);
    ip6_addr_assign_zone(&destination_address, IP6_UNICAST, inp);

    /* Check a subset of the other RFC 4861 Sec. 8.1 requirements. */
    if (!ip6_addr_islinklocal(ip6_current_src_addr()) ||
        IP6H_HOPLIM(ip6_current_header()) != ND6_HOPLIM ||
        redir_hdr->code != 0 || ip6_addr_ismulticast(&destination_address)) {
      pbuf_free(p);
      ND6_STATS_INC(nd6.proterr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    /**
    @page RFC-4861 RFC-4861
    @par Compliant Section
    Section 4.5.  Redirect Message Format
    @par Behavior Description
    An IP address that is a better first hop to use for the ICMP Destination Address.
    When the target is the actual endpoint of communication, i.e., the destination is a neighbor,
    the Target Address field MUST contain the same value as the ICMP Destination Address field.
    Otherwise, the target is a better first-hop router and the Target Address MUST be the router's
    link-local address so that hosts can uniquely identify routers.
    Behavior:  Silently discard any received Router Advertisement
    */
    /**
    @page RFC-4861 RFC-4861
    @par Compliant Sections
    Section 8.1.  Validation of Redirect Messages
    @par Behavior Description
    The ICMP Target Address is either a link-local address (when redirected to a router) or
    the same as the ICMP Destination Address (when redirected to the on-link destination.
    Behavior:  Silently discard any received Router Advertisement
    */
    ip6_addr_copy_from_packed(tmp, redir_hdr->target_address);
    ip6_addr_assign_zone(&tmp, IP6_UNICAST, inp);
    ip6_addr_copy_from_packed(tmp_dest, redir_hdr->destination_address);
    ip6_addr_assign_zone(&tmp_dest, IP6_UNICAST, inp);
    if (!ip6_addr_cmp(&(tmp), &(tmp_dest))) {
      if (!ip6_addr_islinklocal(&tmp)) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                    ("ICMPv6 RD header target address is not link local address .\n"));
        (void)pbuf_free(p);
        ND6_STATS_INC(nd6.proterr);
        ND6_STATS_INC(nd6.drop);
        return;
      }
    }

    if (nd6_validate_options((u8_t*)p->payload + sizeof(struct redirect_header),
                             (int)(p->len - (sizeof(struct redirect_header)))) < 0) {
        pbuf_free(p);
        ND6_STATS_INC(nd6.lenerr);
        ND6_STATS_INC(nd6.drop);
        return;
    }

    /* Offset to options. */
    offset = sizeof(struct redirect_header);

    /* Process each option. */
    while ((p->tot_len - offset) >= 2) {
      u16_t option_len;
      int opt_type;
      int option_len8 = pbuf_try_get_at(p, (u16_t)(offset + 1));
      if (option_len8 <= 0) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                    ("IPv6 options header (optlen %"S32_F") is not specified, packet dropped.\n",
                    option_len8));
        /* read beyond end or zero length */
        goto lenerr_drop_free_return;
      }
      option_len = (u16_t)(((u8_t)option_len8) << 3);  // multiply by 8 bytes
      if (option_len > p->tot_len - offset) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                    ("IPv6 options header (optlen %"S32_F") does not fit in (total len %"U16_F") \
                    and (offset %"U16_F"), packet dropped.\n",
                    option_len8, p->tot_len, offset));
        /* short packet (option does not fit in) */
        goto lenerr_drop_free_return;
      }

      opt_type =  pbuf_try_get_at(p, offset);
      LWIP_ASSERT("opt_type >= 0", opt_type >= 0);

      switch (opt_type) {
        case ND6_OPTION_TYPE_TARGET_LLADDR:
          if (option_len != sizeof(struct lladdr_option)) {
            LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                        ("ND6_OPTION_TYPE_TARGET_LLADDR: (optlen %"U16_F") is less than struct lladdr_option, \
                        packet dropped.\n",
                         option_len));
            goto lenerr_drop_free_return;
          }
          break;

        case ND6_OPTION_TYPE_REDIRECT_HEADER:
          LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING,
                      ("ND6_OPTION_TYPE_REDIRECT_HEADER: option not supported so skipping .\n"));
          break;

        default:
          break;
      } /* End of Switch */

      offset = (u16_t)(offset + (u16_t)option_len);
    } /* End of While */
    /* Codenomicon for ICMPv6 fails due to redirect. */
    /* check if target address is not same as stack's address, it will create loopback, treat such RD packet as invalid
      packets and ignore thema.
    */
    ip6_addr_copy_from_packed(tmp, redir_hdr->destination_address);
    ip6_addr_assign_zone(&tmp, IP6_UNICAST, inp);
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
      if (!ip6_addr_isinvalid(netif_ip6_addr_state(inp, i)) &&
          ip6_addr_cmp(&tmp, netif_ip6_addr(inp, i))) {
          /* Not a valid message. */
          pbuf_free(p);
          ND6_STATS_INC(nd6.proterr);
          ND6_STATS_INC(nd6.drop);
          return;
      }
    }

    /* Copy original destination address to current source address, to have an aligned copy. */
    ip6_addr_copy_from_packed(tmp, redir_hdr->destination_address);
    ip6_addr_assign_zone(&tmp, IP6_UNICAST, inp);

    /* Find dest address in cache */
    dest_idx = nd6_find_destination_cache_entry(&tmp);
    if (dest_idx < 0) {
      /* Destination not in cache, drop packet. */
      pbuf_free(p);
      ND6_STATS_INC(nd6.drop);
      return;
    }
    /**
    @page RFC-4861 RFC-4861
    @par Compliant Sections
    Section 8.1. Validation of Redirect Messages
    @par Behavior Description
    The IP source address of the Redirect is the same as the current first-hop router for the specified
     ICMP Destination Address. Behavior:Silently discard any received Router Advertisement
    */
    if (!ip6_addr_cmp(ip6_current_src_addr(), &(destination_cache[dest_idx].next_hop_addr))) {
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
                  ("ICMPv6 RD msg,IP source addr is not same as the dest first-hop/next-hop addr.\n"));
      (void)pbuf_free(p);
      ND6_STATS_INC(nd6.proterr);
      ND6_STATS_INC(nd6.drop);
      return;
    }
    /* Create an aligned, zoned copy of the target address. */
    ip6_addr_copy_from_packed(target_address, redir_hdr->target_address);
    ip6_addr_assign_zone(&target_address, IP6_UNICAST, inp);

    /* Set the new target address. */
    ip6_addr_copy(destination_cache[dest_idx].next_hop_addr, target_address);

    /* Offset to options. */
    offset = sizeof(struct redirect_header);

    /* Process each option. */
    while ((p->tot_len - offset) >= 2) {
      if (p->len == p->tot_len) {
        /* no need to copy from contiguous pbuf */
        buffer = &((u8_t*)p->payload)[offset];
      } else {
        buffer = rd6_ra_buffer;
        /* already validation is done at the above loop so now just perform buffer parsing */
        (void)pbuf_copy_partial(p, buffer, sizeof(struct lladdr_option), offset);
      }

      switch (buffer[0]) {
        case ND6_OPTION_TYPE_TARGET_LLADDR:
          if (dup) {
            LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING,
                        ("ND6_OPTION_TYPE_TARGET_LLADDR: option is present repeatedly \
                         so taking only the first and ignoring the next items \n"));
            break;
          } else {
            dup = 1; /* to avoid updating repeatedly. */
          }
          lladdr_opt = (struct lladdr_option *)buffer;

          /* If Link-layer address of other router is given, try to add to neighbor cache. */
          if (lladdr_opt != NULL) {
            i = nd6_find_neighbor_cache_entry(&target_address, inp);
            if (i < 0) {
              i = nd6_new_neighbor_cache_entry();
              if (i >= 0) {
                neighbor_cache[i].netif = inp;
                MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
                neighbor_cache[i].lladdrlen = inp->hwaddr_len;
                ip6_addr_copy(neighbor_cache[i].next_hop_address, target_address);

                /* Receiving a message does not prove reachability: only in one direction.
                 * Delay probe in case we get confirmation of reachability from upper layer (TCP). */
                neighbor_cache[i].state = ND6_DELAY;
                neighbor_cache[i].counter.delay_time = LWIP_ND6_DELAY_FIRST_PROBE_TIME / ND6_TMR_INTERVAL;
              }
            } else {
              if (neighbor_cache[i].state == ND6_INCOMPLETE) {
                MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
                neighbor_cache[i].lladdrlen = inp->hwaddr_len;
                /* Receiving a message does not prove reachability: only in one direction.
                 * Delay probe in case we get confirmation of reachability from upper layer (TCP). */
                neighbor_cache[i].state = ND6_DELAY;
                neighbor_cache[i].counter.delay_time = LWIP_ND6_DELAY_FIRST_PROBE_TIME / ND6_TMR_INTERVAL;
              } else if (neighbor_cache[i].state > ND6_INCOMPLETE) {
                if (!lwip_lladdr_cmp(neighbor_cache[i].lladdr, lladdr_opt->addr, neighbor_cache[i].lladdrlen)) {
                  (void)memcpy(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
                  neighbor_cache[i].lladdrlen = inp->hwaddr_len;
                  neighbor_cache[i].state = ND6_STALE;
                  neighbor_cache[i].counter.stale_time = 0;
                }
              }
            }
          }
          break; /* ND6_OPTION_TYPE_TARGET_LLADDR */

        case ND6_OPTION_TYPE_REDIRECT_HEADER:
          LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING,
                      ("ND6_OPTION_TYPE_REDIRECT_HEADER: option not supported so skipping .\n"));
          break;

        default:
          break;
      } /* End of Switch */
      offset = (u16_t)(offset + (u16_t)(((u16_t)buffer[1]) << 3));
    } /* End of While */
    break;
  }
  case ICMP6_TYPE_PTB: /* Packet too big */
  {
    struct icmpv6_hdr *icmp6hdr; /* Packet too big message */
    struct ip6_hdr *ip6hdr; /* IPv6 header of the packet which caused the error */
    u32_t pmtu;
    ip6_addr_t destination_address;

    /* Check that ICMPv6 header + IPv6 header fit in payload */
    if (p->len < (sizeof(struct icmpv6_hdr) + IP6_HLEN)) {
      /* drop short packets */
      pbuf_free(p);
      ND6_STATS_INC(nd6.lenerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    icmp6hdr = (struct icmpv6_hdr *)p->payload;
    ip6hdr = (struct ip6_hdr *)((u8_t*)p->payload + sizeof(struct icmpv6_hdr));

    /* Create an aligned, zoned copy of the destination address. */
    ip6_addr_copy_from_packed(destination_address, ip6hdr->dest);
    ip6_addr_assign_zone(&destination_address, IP6_UNKNOWN, inp);

    /* Look for entry in destination cache. */
    dest_idx = nd6_find_destination_cache_entry(&destination_address);
    if (dest_idx < 0) {
      /* Destination not in cache, drop packet. */
      pbuf_free(p);
      return;
    }

    /* Change the Path MTU. */
    pmtu = lwip_htonl(icmp6hdr->data);
    if (pmtu < NETIF_MTU_MIN) {
      destination_cache[dest_idx].pmtu = NETIF_MTU_MIN;
    } else {
      destination_cache[dest_idx].pmtu = (u16_t)LWIP_MIN(pmtu, 0xFFFF);
    }

    break; /* ICMP6_TYPE_PTB */
  }

  default:
    ND6_STATS_INC(nd6.proterr);
    ND6_STATS_INC(nd6.drop);
    break; /* default */
  }

  pbuf_free(p);
  return;
lenerr_drop_free_return:
  ND6_STATS_INC(nd6.lenerr);
  ND6_STATS_INC(nd6.drop);
  pbuf_free(p);
}

#if (LWIP_RIPPLE && LWIP_CONF_UPDATE_BEACON_PRIORITY)
extern void rpl_core_route_table_entry_cnt(uint16_t *const cnt_max, uint16_t *const cnt_inuse);

static void
beacon_priority_set(u8_t prio)
{
  struct netif *netif = NULL;

  for (netif = netif_list; netif != NULL; netif = netif->next) {
    if (netif->flags & NETIF_FLAG_LOOPBACK) {
      continue;
    }
    (void)netif_set_beacon_prio(netif, prio);
  }
}

void
beacon_priority_update(u8_t is_clear)
{
  static u8_t prio_old = NETIF_BEACON_PRIORITY_MAX;
  static u8_t netif_cnt_old = 0;
  u8_t netif_cnt;
  u8_t prio_new;
  u8_t parent_cnt;
  u16_t nbr_remaining, route_inuse, route_total;

  if (is_clear != 0) {
    LWIP_DEBUGF_LOG0(IP6_DEBUG, "bp clr\n");
    prio_old = NETIF_BEACON_PRIORITY_MAX;
    netif_cnt_old = 0;
    beacon_priority_set(0);
    return;
  }

  if (lwip_is_rpl_running() == lwIP_FALSE) {
    LWIP_DEBUGF_LOG0(IP6_DEBUG, "bp n rpl\n");
    prio_old = NETIF_BEACON_PRIORITY_MAX;
    netif_cnt_old = 0;
    return;
  }

  parent_cnt = lwip_rpl_dag_parent_count();
  if (parent_cnt == 0) {
    prio_new = 0;
  } else {
    nbr_remaining = lwip_nbr_cache_free_count();
    rpl_core_route_table_entry_cnt(&route_total, &route_inuse);
    prio_new = (u8_t)LWIP_MIN((((u32_t)nbr_remaining) * NETIF_BEACON_PRIORITY_MAX) / LWIP_ND6_NUM_NEIGHBORS,
                              (((u32_t)(route_total - route_inuse)) * NETIF_BEACON_PRIORITY_MAX) / route_total);
  }
  /* if a new netif is added, need to set it's priority, no matter the priority changed or not */
  netif_cnt = netif_count();
  if ((prio_new != prio_old) || (netif_cnt > netif_cnt_old)) {
    beacon_priority_set(prio_new);
    prio_old = prio_new;
    netif_cnt_old = netif_cnt;
  }

  return;
}
#endif

/**
 * Periodic timer for Neighbor discovery functions:
 *
 * - Update neighbor reachability states
 * - Update destination cache entries age
 * - Update invalidation timers of default routers and on-link prefixes
 * - Update lifetimes of our addresses
 * - Perform duplicate address detection (DAD) for our addresses
 * - Send router solicitations
 */
void
nd6_tmr(void)
{
  s8_t i;
  struct netif *netif = NULL;

#if (LWIP_RIPPLE && LWIP_CONF_UPDATE_BEACON_PRIORITY)
  beacon_priority_update(0);
#endif

  /* Process neighbor entries. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    switch (neighbor_cache[i].state) {
      case ND6_INCOMPLETE:
        if ((neighbor_cache[i].counter.probes_sent >= LWIP_ND6_MAX_MULTICAST_SOLICIT) &&
            (!neighbor_cache[i].isrouter)) {
          /* Retries exceeded. */
          nd6_free_neighbor_cache_entry(i);
#if (LWIP_RIPPLE && LWIP_CONF_UPDATE_BEACON_PRIORITY)
          beacon_priority_update(0);
#endif
        } else {
          /* Send a NS for this entry. */
          neighbor_cache[i].counter.probes_sent++;
          nd6_send_neighbor_cache_probe(&neighbor_cache[i], ND6_SEND_FLAG_MULTICAST_DEST);
        }
        break;
      case ND6_REACHABLE:
        /* Send queued packets, if any are left. Should have been sent already. */
        if (neighbor_cache[i].q != NULL) {
          nd6_send_q(i);
        }
        if (neighbor_cache[i].counter.reachable_time <= ND6_TMR_INTERVAL) {
          /* Change to stale state. */
#if LWIP_RIPPLE
          if (neighbor_cache[i].isrouter != 0) {
            LWIP_DEBUGF(IP6_DEBUG,
                        ("Need to do immediate probe for the default router.\n"));
            neighbor_cache[i].state = ND6_DELAY;
            neighbor_cache[i].counter.delay_time = LWIP_ND6_DELAY_FIRST_PROBE_TIME / ND6_TMR_INTERVAL;
          } else
#endif
          {
            neighbor_cache[i].state = ND6_STALE;
            neighbor_cache[i].counter.stale_time = 0;
          }
        } else {
          neighbor_cache[i].counter.reachable_time -= ND6_TMR_INTERVAL;
        }
        break;
      case ND6_STALE:
        neighbor_cache[i].counter.stale_time++;
        break;
      case ND6_DELAY:
        if (neighbor_cache[i].counter.delay_time <= 1) {
          /* Change to PROBE state. */
          neighbor_cache[i].state = ND6_PROBE;
          neighbor_cache[i].counter.probes_sent = 0;
        } else {
          neighbor_cache[i].counter.delay_time--;
        }
        break;
      case ND6_PROBE:
        if ((neighbor_cache[i].counter.probes_sent >= LWIP_ND6_MAX_MULTICAST_SOLICIT) &&
            (!neighbor_cache[i].isrouter)) {
          /* Retries exceeded. */
          nd6_free_neighbor_cache_entry(i);
#if (LWIP_RIPPLE && LWIP_CONF_UPDATE_BEACON_PRIORITY)
          beacon_priority_update(0);
#endif
        } else {
          /* Send a NS for this entry. */
          neighbor_cache[i].counter.probes_sent++;
          nd6_send_neighbor_cache_probe(&neighbor_cache[i], 0);
        }
        break;
      case ND6_NO_ENTRY:
#if LWIP_ND6_STATIC_NBR
      case ND6_PERMANENT:
#endif
      default:
        /* Do nothing. */
        break;
    }
  }

  /* Process destination entries. */
  for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) {
    destination_cache[i].age++;
#if LWIP_RIPPLE && LWIP_ND6_DESTINATIONS_OLDTIME
    nd6_refresh_destination_cache_by_age(i);
#endif
  }

  /* Process router entries. */
  for (i = 0; i < LWIP_ND6_NUM_ROUTERS; i++) {
    if (default_router_list[i].neighbor_entry != NULL) {
      /* Active entry. */
      if (default_router_list[i].invalidation_timer <= ND6_TMR_INTERVAL / 1000) {
        /* No more than 1 second remaining. Clear this entry. Also clear any of
         * its destination cache entries, as per RFC 4861 Sec. 5.3 and 6.3.5. */
        s8_t j;
        for (j = 0; j < LWIP_ND6_NUM_DESTINATIONS; j++) {
          if (ip6_addr_cmp(&destination_cache[j].next_hop_addr,
              &default_router_list[i].neighbor_entry->next_hop_address)) {
            ip6_addr_set_any(&destination_cache[j].destination_addr);
          }
        }
        default_router_list[i].neighbor_entry->isrouter = 0;
        default_router_list[i].neighbor_entry = NULL;
        default_router_list[i].invalidation_timer = 0;
        default_router_list[i].flags = 0;
      } else {
        default_router_list[i].invalidation_timer -= ND6_TMR_INTERVAL / 1000;
      }
    }
  }

  /* Process prefix entries. */
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; i++) {
    if (prefix_list[i].netif != NULL) {
      if (prefix_list[i].invalidation_timer <= ND6_TMR_INTERVAL / 1000) {
        /* Entry timed out, remove it */
        prefix_list[i].invalidation_timer = 0;
        prefix_list[i].netif = NULL;
      } else {
        prefix_list[i].invalidation_timer -= ND6_TMR_INTERVAL / 1000;
      }
    }
  }

  /* Process our own addresses, updating address lifetimes and/or DAD state. */
  NETIF_FOREACH(netif) {
    if (netif->flags & NETIF_FLAG_LOOPBACK) {
      continue;
    }

    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
      u8_t addr_state;
#if LWIP_IPV6_ADDRESS_LIFETIMES
      /* Step 1: update address lifetimes (valid and preferred). */
      addr_state = netif_ip6_addr_state(netif, i);
      /* RFC 4862 is not entirely clear as to whether address lifetimes affect
       * tentative addresses, and is even less clear as to what should happen
       * with duplicate addresses. We choose to track and update lifetimes for
       * both those types, although for different reasons:
       * - for tentative addresses, the line of thought of Sec. 5.7 combined
       *   with the potentially long period that an address may be in tentative
       *   state (due to the interface being down) suggests that lifetimes
       *   should be independent of external factors which would include DAD;
       * - for duplicate addresses, retiring them early could result in a new
       *   but unwanted attempt at marking them as valid, while retiring them
       *   late/never could clog up address slots on the netif.
       * As a result, we may end up expiring addresses of either type here.
       */
      if (!ip6_addr_isinvalid(addr_state) &&
          !netif_ip6_addr_isstatic(netif, i)) {
        u32_t life = netif_ip6_addr_valid_life(netif, i);
        if (life <= ND6_TMR_INTERVAL / 1000) {
          /* The address has expired. */
          netif_ip6_addr_set_valid_life(netif, i, 0);
          netif_ip6_addr_set_pref_life(netif, i, 0);
          netif_ip6_addr_set_state(netif, i, IP6_ADDR_INVALID);
        } else {
          if (!ip6_addr_life_isinfinite(life)) {
            life -= ND6_TMR_INTERVAL / 1000;
            LWIP_ASSERT("bad valid lifetime", life != IP6_ADDR_LIFE_STATIC);
            netif_ip6_addr_set_valid_life(netif, i, life);
          }
          /* The address is still here. Update the preferred lifetime too. */
          life = netif_ip6_addr_pref_life(netif, i);
          if (life <= ND6_TMR_INTERVAL / 1000) {
            /* This case must also trigger if 'life' was already zero, so as to
             * deal correctly with advertised preferred-lifetime reductions. */
            netif_ip6_addr_set_pref_life(netif, i, 0);
            if (addr_state == IP6_ADDR_PREFERRED)
              netif_ip6_addr_set_state(netif, i, IP6_ADDR_DEPRECATED);
          } else if (!ip6_addr_life_isinfinite(life)) {
            life -= ND6_TMR_INTERVAL / 1000;
            netif_ip6_addr_set_pref_life(netif, i, life);
          }
        }
      }
      /* The address state may now have changed, so reobtain it next. */
#endif /* LWIP_IPV6_ADDRESS_LIFETIMES */
      /* Step 2: update DAD state. */
      addr_state = netif_ip6_addr_state(netif, i);
      if (ip6_addr_istentative(addr_state)) {
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS
        if (!LWIP_IS_DAD_ENABLED(netif)) {
          netif_ip6_addr_set_state(netif, i, IP6_ADDR_PREFERRED);
          continue;
        }

        if ((addr_state & IP6_ADDR_TENTATIVE_COUNT_MASK) >= LWIP_IPV6_DUP_DETECT_ATTEMPTS) {
          /* No NA received in response. Mark address as valid. For dynamic
           * addresses with an expired preferred lifetime, the state is set to
           * deprecated right away. That should almost never happen, though. */
          addr_state = IP6_ADDR_PREFERRED;
#if LWIP_IPV6_ADDRESS_LIFETIMES
          if (!netif_ip6_addr_isstatic(netif, i) &&
              (netif_ip6_addr_pref_life(netif, i) == 0)) {
            addr_state = IP6_ADDR_DEPRECATED;
          }
#endif /* LWIP_IPV6_ADDRESS_LIFETIMES */
          netif_ip6_addr_set_state(netif, i, addr_state);
        } else if (netif_is_up(netif) && netif_is_link_up(netif)) {
          /* tentative: set next state by increasing by one */
          netif_ip6_addr_set_state(netif, i, addr_state + 1);
          /* Send a NS for this address. Use the unspecified address as source
           * address in all cases (RFC 4862 Sec. 5.4.2), not in the least
           * because as it is, we only consider multicast replies for DAD. */
          nd6_send_ns(netif, netif_ip6_addr(netif, i),
                      ND6_SEND_FLAG_MULTICAST_DEST | ND6_SEND_FLAG_ANY_SRC);
#if LWIP_RIPPLE && LWIP_ND6_DESTINATIONS_OLDTIME
          if (!ip6_addr_islinklocal(netif_ip6_addr(netif, i))) {
            nd6_refresh_destination_cache(netif_ip6_addr(netif, i));
          }
#endif
        }
#else
        netif_ip6_addr_set_state(netif, i, IP6_ADDR_PREFERRED);
#endif /* LWIP_IPV6_DUP_DETECT_ATTEMPTS */
      }
    }
  }

#if LWIP_IPV6_SEND_ROUTER_SOLICIT
  /* Send router solicitation messages, if necessary. */
  if (!nd6_tmr_rs_reduction) {
    nd6_tmr_rs_reduction = (ND6_RTR_SOLICITATION_INTERVAL / ND6_TMR_INTERVAL) - 1;
    NETIF_FOREACH(netif) {
      /* Do not send NS/NA/RS/RA packets to loopback interface */
      if (netif->flags & NETIF_FLAG_LOOPBACK) {
        continue;
      }
      if ((netif->rs_count > 0) && netif_is_up(netif) &&
          netif_is_link_up(netif) &&
          !ip6_addr_isinvalid(netif_ip6_addr_state(netif, 0)) &&
          !ip6_addr_isduplicated(netif_ip6_addr_state(netif, 0))) {
        if (nd6_send_rs(netif) == ERR_OK) {
          netif->rs_count--;
        }
      }
    }
  } else {
    nd6_tmr_rs_reduction--;
  }
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */

#if LWIP_ND6_ROUTER
  for (netif = netif_list; netif != NULL; netif = netif->next) {
    /* Do not send NS/NA/RS/RA packets to loopback interface */
    if ((netif->flags & NETIF_FLAG_LOOPBACK) != 0) {
      continue;
    }

    if ((netif->ra_enable != lwIP_TRUE) || !(netif->flags & NETIF_FLAG_UP) ||
        (ip6_addr_isinvalid(netif_ip6_addr_state(netif, 0)))) {
      continue;
    }

    if (netif->ra_init_cnt > 0) {
      if (netif->ra_timer > 0) {
        netif->ra_timer--;
      } else {
        netif->ra_init_cnt--;
        netif->ra_timer = (netif->ra_init_cnt > 0) ? ND6_RA_INIT_INTERVAL : ND6_RA_NORMAL_INTERVAL;
        (void)nd6_send_ra(netif, NULL, 0);
      }
    } else {
      if (netif->ra_timer > 0) {
        netif->ra_timer--;
      } else {
        netif->ra_timer = ND6_RA_NORMAL_INTERVAL;
        (void)nd6_send_ra(netif, NULL, 0);
      }
    }
  }
#endif

#if LWIP_ICMP6_ERR_RT_LMT
  icmp6_err_rate_calc();
#endif /* LWIP_ICMP6_ERR_RT_LMT */
}

#if LWIP_LOWPOWER
#include "lwip/lowpower.h"
u32_t
nd6_tmr_tick(void)
{
  s8_t i;
  struct netif *netif = NULL;
  u32_t tick = 0;
  u32_t val = 0;

  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    switch (neighbor_cache[i].state) {
      case ND6_PROBE:
      case ND6_INCOMPLETE: /* state PROBE and INCOMPLETE return 1 */
        return 1;
      case ND6_REACHABLE:
        /* Send queued packets, if any are left. Should have been sent already. */
        if (neighbor_cache[i].q != NULL) {
          return 1;
        }
        if (neighbor_cache[i].counter.reachable_time >= ND6_TMR_INTERVAL) {
          val = neighbor_cache[i].counter.reachable_time / ND6_TMR_INTERVAL;
          SET_TMR_TICK(tick, val);
        }
        break;
      case ND6_DELAY:
        val = neighbor_cache[i].counter.delay_time;
        SET_TMR_TICK(tick, val);
        break;
      default:
        /* Do nothing. */
        break;
    }
  }

  /* Process router entries. */
  for (i = 0; i < LWIP_ND6_NUM_ROUTERS; i++) {
    if (default_router_list[i].neighbor_entry != NULL) {
      val = default_router_list[i].invalidation_timer;
      SET_TMR_TICK(tick, val);
    }
  }

  /* Process prefix entries. */
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; i++) {
    if (prefix_list[i].netif != NULL) {
      val = prefix_list[i].invalidation_timer;
      SET_TMR_TICK(tick, val);
    }
  }

  /* Process our own addresses, updating address lifetimes and/or DAD state. */
  NETIF_FOREACH(netif) {
    if ((netif->flags & NETIF_FLAG_LOOPBACK) != 0) {
      continue;
    }
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
      u8_t addr_state;
#if LWIP_IPV6_ADDRESS_LIFETIMES
      /* Step 1: update address lifetimes (valid and preferred). */
      addr_state = netif_ip6_addr_state(netif, i);
      if (!ip6_addr_isinvalid(addr_state) &&
          !netif_ip6_addr_isstatic(netif, i)) {
        u32_t life = netif_ip6_addr_valid_life(netif, i);
        if (!ip6_addr_life_isinfinite(life)) {
          SET_TMR_TICK(tick, life);
        }

        life = netif_ip6_addr_pref_life(netif, i);
        if (!ip6_addr_life_isinfinite(life)) {
          SET_TMR_TICK(tick, life);
        }
      }
      /* The address state may now have changed, so reobtain it next. */
#endif /* LWIP_IPV6_ADDRESS_LIFETIMES */
      /* Step 2: update DAD state. */
      addr_state = netif_ip6_addr_state(netif, i);
      if (ip6_addr_istentative(addr_state)) {
        LOWPOWER_DEBUG(("%s tmr tick: 1\n", __func__));
        return 1;
      }
    }
  }

  /* Router solicitations are sent in 4 second intervals (see RFC 4861, ch. 6.3.7) */
  /* ND6_RTR_SOLICITATION_INTERVAL */
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
  if (nd6_tmr_rs_reduction > 0) {
    val = nd6_tmr_rs_reduction;
    SET_TMR_TICK(tick, val);
  }
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */

#if LWIP_ND6_ROUTER
  for (netif = netif_list; netif != NULL; netif = netif->next) {
    /* Do not send NS/NA/RS/RA packets to loopback interface */
    if ((netif->flags & NETIF_FLAG_LOOPBACK) != 0) {
      continue;
    }

    if ((netif->ra_enable != lwIP_TRUE) || ((netif->flags & NETIF_FLAG_UP) == 0) ||
        (ip6_addr_isinvalid(netif_ip6_addr_state(netif, 0)))) {
      continue;
    }
    val = netif->ra_timer;
    SET_TMR_TICK(tick, val);
  }
#endif

  LOWPOWER_DEBUG(("%s tmr tick: %u\n", __func__, tick));
  return tick;
}
#endif /* LWIP_LOWPOWER */

/** Send a neighbor solicitation message for a specific neighbor cache entry
 *
 * @param entry the neightbor cache entry for wich to send the message
 * @param flags one of ND6_SEND_FLAG_*
 */
static void
nd6_send_neighbor_cache_probe(struct nd6_neighbor_cache_entry *entry, u8_t flags)
{
  nd6_send_ns(entry->netif, &entry->next_hop_address, flags);
}

/**
 * Send a neighbor solicitation message
 *
 * @param netif the netif on which to send the message
 * @param target_addr the IPv6 target address for the ND message
 * @param flags one of ND6_SEND_FLAG_*
 */
static void
nd6_send_ns(struct netif *netif, const ip6_addr_t *target_addr, u8_t flags)
{
  struct ns_header *ns_hdr = NULL;
  struct pbuf *p = NULL;
  const ip6_addr_t *src_addr = NULL;
  u16_t lladdr_opt_len;

  /*
   * RFC4862 section 7.7.2: If the source address of the packet prompting the solicitation is the same
   * as one of the addresses assigned to the outgoing interface, that address SHOULD be placed in the IP
   * Source Address of the outgoing solicitation. Otherwise, any one of the addresses assigned to the
   * interface should be used. Using the prompting packet's source address when possible ensures that
   * the recipient of the Neighbor Solicitation installs in its Neighbor Cache the IP address that is highly
   * likely to be used in subsequent return traffic belonging to the prompting packet's "connection".
   */
  /*
   * lwip does NOT comply with this specification as the source address of NA is choosen based on the
   * target address. In most of cases, if users don't declare the specific source address by binding socket to
   * one specific address, the source address of IPV6 packets is also choosen based on target address, so the
   * source address of NS is same as the prompting IPv6 packet.
   */
  if (!(flags & ND6_SEND_FLAG_ANY_SRC)) {
    src_addr = ip_2_ip6(ip6_select_source_address(netif, target_addr));
    if (src_addr == NULL) {
      int i;
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
              ip6_addr_netcmp(target_addr, netif_ip6_addr(netif, i))) {
          src_addr = netif_ip6_addr(netif, i);
          break;
        }
      }

      if (i == LWIP_IPV6_NUM_ADDRESSES) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING, ("ICMPv6 NS: no available src address\n"));
        ND6_STATS_INC(nd6.err);
        return;
      }
    }
    /* calculate option length (in 8-byte-blocks) */
    lladdr_opt_len = (u16_t)(((netif->hwaddr_len + 2) + 7) >> 3);
  } else {
    src_addr = IP6_ADDR_ANY6;
    /* Option "MUST NOT be included when the source IP address is the unspecified address." */
    lladdr_opt_len = 0;
  }

  /* Allocate a packet. */
  p = pbuf_alloc(PBUF_IP, (u16_t)(sizeof(struct ns_header) + (unsigned int)(lladdr_opt_len << 3)), PBUF_RAM);
  if (p == NULL) {
    ND6_STATS_INC(nd6.memerr);
    return;
  }

  /* Set fields. */
  ns_hdr = (struct ns_header *)p->payload;

  ns_hdr->type = ICMP6_TYPE_NS;
  ns_hdr->code = 0;
  ns_hdr->chksum = 0;
  ns_hdr->reserved = 0;
  ip6_addr_copy_to_packed(ns_hdr->target_address, *target_addr);

  if (lladdr_opt_len != 0) {
    struct lladdr_option *lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct ns_header));
    lladdr_opt->type = ND6_OPTION_TYPE_SOURCE_LLADDR;
    lladdr_opt->length = (u8_t)lladdr_opt_len;
    SMEMCPY(lladdr_opt->addr, netif->hwaddr, netif->hwaddr_len);
  }

  /* Generate the solicited node address for the target address. */
  if (flags & ND6_SEND_FLAG_MULTICAST_DEST) {
    ip6_addr_set_solicitednode(&multicast_address, target_addr->addr[3]);
    ip6_addr_assign_zone(&multicast_address, IP6_MULTICAST, netif);
    target_addr = &multicast_address;
  }

#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    ns_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr,
      target_addr);
  }
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Send the packet out. */
  ND6_STATS_INC(nd6.xmit);
  ip6_output_if(p, (src_addr == IP6_ADDR_ANY6) ? NULL : src_addr, target_addr,
      ND6_HOPLIM, 0, IP6_NEXTH_ICMP6, netif);
  pbuf_free(p);
}

/**
 * Send a neighbor advertisement message
 *
 * @param netif the netif on which to send the message
 * @param target_addr the IPv6 target address for the ND message
 * @param flags one of ND6_SEND_FLAG_*
 */
void
nd6_send_na(struct netif *netif, const ip6_addr_t *target_addr, u8_t flags)
{
  struct na_header *na_hdr;
  struct lladdr_option *lladdr_opt;
  struct pbuf *p;
  const ip6_addr_t *src_addr;
  const ip6_addr_t *dest_addr;
  u16_t lladdr_opt_len;

  LWIP_ASSERT("target address is required", target_addr != NULL);

  /* Use link-local address as source address. */
  /* src_addr = netif_ip6_addr(netif, 0); */
  /* Use target address as source address. */
  src_addr = target_addr;

  /* Allocate a packet. */
  lladdr_opt_len = ((netif->hwaddr_len + 2) >> 3) + (((netif->hwaddr_len + 2) & 0x07) ? 1 : 0);
  p = pbuf_alloc(PBUF_IP, sizeof(struct na_header) + (lladdr_opt_len << 3), PBUF_RAM);
  if (p == NULL) {
    ND6_STATS_INC(nd6.memerr);
    return;
  }

  /* Set fields. */
  na_hdr = (struct na_header *)p->payload;
  lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct na_header));

  na_hdr->type = ICMP6_TYPE_NA;
  na_hdr->code = 0;
  na_hdr->chksum = 0;
  na_hdr->flags = flags & 0xf0;
  na_hdr->reserved[0] = 0;
  na_hdr->reserved[1] = 0;
  na_hdr->reserved[2] = 0;
  ip6_addr_copy_to_packed(na_hdr->target_address, *target_addr);

  lladdr_opt->type = ND6_OPTION_TYPE_TARGET_LLADDR;
  lladdr_opt->length = (u8_t)lladdr_opt_len;
  SMEMCPY(lladdr_opt->addr, netif->hwaddr, netif->hwaddr_len);

  /* Generate the solicited node address for the target address. */
  if (flags & ND6_SEND_FLAG_MULTICAST_DEST) {
    ip6_addr_set_solicitednode(&multicast_address, target_addr->addr[3]);
    ip6_addr_assign_zone(&multicast_address, IP6_MULTICAST, netif);
    dest_addr = &multicast_address;
  } else if (flags & ND6_SEND_FLAG_ALLNODES_DEST) {
    ip6_addr_set_allnodes_linklocal(&multicast_address);
    ip6_addr_assign_zone(&multicast_address, IP6_MULTICAST, netif);
    dest_addr = &multicast_address;
  } else {
    dest_addr = ip6_current_src_addr();
  }

#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    na_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr,
      dest_addr);
  }
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Send the packet out. */
  ND6_STATS_INC(nd6.xmit);
  ip6_output_if(p, src_addr, dest_addr,
      ND6_HOPLIM, 0, IP6_NEXTH_ICMP6, netif);
  pbuf_free(p);
}

#if LWIP_IPV6_SEND_ROUTER_SOLICIT
/**
 * Send a router solicitation message
 *
 * @param netif the netif on which to send the message
 */
err_t
nd6_send_rs(struct netif *netif)
{
  struct rs_header *rs_hdr;
  struct lladdr_option *lladdr_opt;
  struct pbuf *p;
  const ip6_addr_t *src_addr;
  err_t err;
  u16_t lladdr_opt_len = 0;
  u16_t lladdr_opt_data_len = 0;

  /* Link-local source address, or unspecified address? */
  if (ip6_addr_isvalid(netif_ip6_addr_state(netif, 0))) {
    src_addr = netif_ip6_addr(netif, 0);
  } else {
    src_addr = IP6_ADDR_ANY6;
  }

  /* Generate the all routers target address. */
  ip6_addr_set_allrouters_linklocal(&multicast_address);
  ip6_addr_assign_zone(&multicast_address, IP6_MULTICAST, netif);

  /* Allocate a packet. */
  if (src_addr != IP6_ADDR_ANY6) {
    lladdr_opt_len = ((netif->hwaddr_len + 2) >> 3) + (((netif->hwaddr_len + 2) & 0x07) ? 1 : 0);
  }
  p = pbuf_alloc(PBUF_IP, sizeof(struct rs_header) + (lladdr_opt_len << 3), PBUF_RAM);
  if (p == NULL) {
    ND6_STATS_INC(nd6.memerr);
    return ERR_BUF;
  }

  /* Set fields. */
  rs_hdr = (struct rs_header *)p->payload;

  rs_hdr->type = ICMP6_TYPE_RS;
  rs_hdr->code = 0;
  rs_hdr->chksum = 0;
  rs_hdr->reserved = 0;

  if (src_addr != IP6_ADDR_ANY6) {
    /* Include our hw address. */
    lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct rs_header));
    lladdr_opt->type = ND6_OPTION_TYPE_SOURCE_LLADDR;
    lladdr_opt->length = (u8_t)lladdr_opt_len;
    /* the padding bytes must be set to zero */
    lladdr_opt_data_len = (lladdr_opt_len << 3) - RS_LLADDR_OPTION_HEADER_LEN;
    if (lladdr_opt_data_len > netif->hwaddr_len) {
      (void)memset_s(lladdr_opt->addr, lladdr_opt_data_len, 0, lladdr_opt_data_len);
    }

    if (memcpy_s(lladdr_opt->addr, lladdr_opt_data_len, netif->hwaddr, netif->hwaddr_len) != EOK) {
      (void)pbuf_free(p);
      return ERR_MEM;
    }
  }

#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    rs_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr,
      &multicast_address);
  }
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Send the packet out. */
  ND6_STATS_INC(nd6.xmit);

  err = ip6_output_if(p, (src_addr == IP6_ADDR_ANY6) ? NULL : src_addr, &multicast_address,
      ND6_HOPLIM, 0, IP6_NEXTH_ICMP6, netif);
  pbuf_free(p);

  return err;
}
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */

#if LWIP_IPV6 && LWIP_NETIF_NBR_CACHE_API
err_t
nd6_del_neighbor_cache_entry_manually(struct netif *netif, struct ip6_addr *nbrip)
{
  int idx;
  idx = nd6_find_neighbor_cache_entry(nbrip, netif);
  if ((idx >= 0) && (neighbor_cache[idx].netif == netif)) {
    /* Check if its the router in that case first remove the router entry and then clear the nbr cache */
    if (neighbor_cache[idx].isrouter != 0) {
      s8_t router_index;
      router_index = nd6_get_router(nbrip, netif);
      if ((router_index >= 0) && (default_router_list[router_index].neighbor_entry == &(neighbor_cache[idx]))) {
        default_router_list[router_index].neighbor_entry = NULL;
        default_router_list[router_index].flags = 0;
        neighbor_cache[idx].isrouter = 0;
      }
    }

    nd6_free_neighbor_cache_entry(idx);
#if (LWIP_RIPPLE && LWIP_CONF_UPDATE_BEACON_PRIORITY)
    beacon_priority_update(0);
#endif
  } else {
    LWIP_DEBUGF(IP6_DEBUG, ("ND6: No NBR found \n"));
    return ERR_NBR_NOTFOUND;
  }

  return ERR_OK;
}

err_t
nd6_add_neighbor_cache_entry_manually(struct netif *netif, struct nd6_neighbor_info *nbrinfo)
{
  int idx;
  idx = nd6_find_neighbor_cache_entry(&(nbrinfo->nbripaddr), netif);
  if (idx >= 0) {
    /*
     * When a neighbor already exist for this node we will not allow to modify
     * it unless until its a static neighbor entry
     */
#if LWIP_ND6_STATIC_NBR
    if ((neighbor_cache[idx].state == ND6_PERMANENT)) {
      neighbor_cache[idx].netif = netif;
      if (memcpy_s(neighbor_cache[idx].lladdr, NETIF_MAX_HWADDR_LEN, nbrinfo->hwaddr, nbrinfo->hwlen) != EOK) {
        ND6_STATS_INC(nd6.memerr);
        return ERR_MEM;
      }
      neighbor_cache[idx].lladdrlen = nbrinfo->hwlen;
    } else
#endif
    {
      LWIP_DEBUGF(IP6_DEBUG, ("ND6: Overwritting non-static entry is forbidden"));
      return ERR_NBR_OVERWRITEFORBIDDEN;
    }
  } else {
    idx = nd6_new_neighbor_cache_entry();
    if (idx < 0) {
      /*
       * We couldn't assign a cache entry for this neighbor.
       * we won't be able to reply. drop it.
       */
      ND6_STATS_INC(nd6.cache_full);
      return ERR_MEM;
    }

    neighbor_cache[idx].netif = netif;
    if (memcpy_s(neighbor_cache[idx].lladdr, NETIF_MAX_HWADDR_LEN, nbrinfo->hwaddr, nbrinfo->hwlen) != EOK) {
      ND6_STATS_INC(nd6.memerr);
      return ERR_MEM;
    }
    neighbor_cache[idx].lladdrlen = nbrinfo->hwlen;

    ip6_addr_set(&(neighbor_cache[idx].next_hop_address), &(nbrinfo->nbripaddr));

    /*
     * Receiving a message does not prove reachability: only in one direction.
     * Delay probe in case we get confirmation of reachability from upper layer (TCP).
     */
    neighbor_cache[idx].state = nbrinfo->curstate;
    neighbor_cache[idx].counter.reachable_time = nbrinfo->reachabletime;
  }

  return ERR_OK;
}

err_t
nd6_get_neighbor_cache_info(struct netif *netif, struct ip6_addr *nbrip,
                            struct nd6_neighbor_info *nbrinfo)
{
  int idx;
  idx = nd6_find_neighbor_cache_entry(nbrip, netif);
  if ((idx >= 0) && (neighbor_cache[idx].netif == netif)) {
    if (memcpy_s(nbrinfo->hwaddr, NETIF_MAX_HWADDR_LEN, neighbor_cache[idx].lladdr, NETIF_MAX_HWADDR_LEN) != EOK) {
      ND6_STATS_INC(nd6.memerr);
      return ERR_MEM;
    }
    nbrinfo->hwlen = neighbor_cache[idx].lladdrlen;
    ip6_addr_set(&(nbrinfo->nbripaddr), &(neighbor_cache[idx].next_hop_address));
    nbrinfo->curstate = neighbor_cache[idx].state;
    /* The rechable time will be same as the user configured */
    nbrinfo->reachabletime = reachable_time;
#if LWIP_ND6_STATIC_NBR
    /* If its a static entry it will not have any rechablility test so time will be set to 0 */
    if (nbrinfo->curstate == ND6_PERMANENT) {
      nbrinfo->reachabletime = 0;
    }
#endif
  } else {
    LWIP_DEBUGF(IP6_DEBUG, ("ND6: No NBR found \n"));
    return ERR_NBR_NOTFOUND;
  }

  return ERR_OK;
}
#endif /* LWIP_IPV6 && LWIP_NETIF_NBR_CACHE_API */

/**
 * Search for a neighbor cache entry
 *
 * @param ip6addr the IPv6 address of the neighbor
 * @return The neighbor cache entry index that matched, -1 if no
 * entry is found
 */
static s8_t
nd6_find_neighbor_cache_entry(const ip6_addr_t *ip6addr, const struct netif *netif)
{
  s8_t i;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].netif == netif) &&
        (ip6_addr_cmp(ip6addr, &(neighbor_cache[i].next_hop_address)))) {
      return i;
    }
  }
  return -1;
}

#if LWIP_RIPPLE
struct nd6_neighbor_cache_entry *
nd6_get_neighbor_byindex(s8_t nbrindex)
{
  if ((nbrindex < 0) || (nbrindex >= LWIP_ND6_NUM_NEIGHBORS)) {
    return NULL;
  }

  /* If no neighbor exists at the given index */
  if (neighbor_cache[nbrindex].state == ND6_NO_ENTRY) {
    return NULL;
  }

  return &(neighbor_cache[nbrindex]);
}

struct nd6_neighbor_cache_entry *
nd6_neighbor_cache_lookup(const ip6_addr_t *ip6addr, struct netif *iface)
{
  s8_t i;
  if ((ip6addr == NULL) || (iface == NULL)) {
    return NULL;
  }
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (ip6_addr_cmp(ip6addr, &(neighbor_cache[i].next_hop_address)) && (neighbor_cache[i].netif == iface)) {
      return &(neighbor_cache[i]);
    }
  }

  return NULL;
}
#endif /* LWIP_RIPPLE */

#if LWIP_ENABLE_ROUTER && LWIP_RIPPLE
void
nd6_remove_neighbor_cache_entry(struct netif *netif,
                                const ip6_addr_t *nbr_addr)
{
  s8_t index;

  index = nd6_find_neighbor_cache_entry(nbr_addr, netif);
  if (index >= 0) {
    nd6_free_neighbor_cache_entry(index);
  }
}

void
nd6_remove_neighbor_by_mac(struct netif *netif, const u8_t *mac, u8_t maclen)
{
  s8_t i;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].netif == netif) && (maclen == neighbor_cache[i].lladdrlen) &&
        (memcmp(mac, neighbor_cache[i].lladdr, maclen) == 0)) {
      nd6_free_neighbor_cache_entry(i);
    }
  }
#if (LWIP_RIPPLE && LWIP_CONF_UPDATE_BEACON_PRIORITY)
  beacon_priority_update(0);
#endif
}

#endif /* LWIP_ENABLE_ROUTER && LWIP_RIPPLE */

/**
 * Create a new neighbor cache entry.
 *
 * If no unused entry is found, will try to recycle an old entry
 * according to ad-hoc "age" heuristic.
 *
 * @return The neighbor cache entry index that was created, -1 if no
 * entry could be created
 */
static s8_t
nd6_new_neighbor_cache_entry(void)
{
  s8_t i;
  s8_t j;
  u32_t time;

  /* First, try to find an empty entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (neighbor_cache[i].state == ND6_NO_ENTRY) {
      return i;
    }
  }

  /* We need to recycle an entry. in general, do not recycle if it is a router. */

  /* Next, try to find a Stale entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_STALE) &&
        (!neighbor_cache[i].isrouter)) {
      nd6_free_neighbor_cache_entry(i);
      return i;
    }
  }

  /* Next, try to find a Probe entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_PROBE) &&
        (!neighbor_cache[i].isrouter)) {
      nd6_free_neighbor_cache_entry(i);
      return i;
    }
  }

  /* Next, try to find a Delayed entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_DELAY) &&
        (!neighbor_cache[i].isrouter)) {
      nd6_free_neighbor_cache_entry(i);
      return i;
    }
  }

  /* Next, try to find the oldest reachable entry. */
  time = 0xfffffffful;
  j = -1;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_REACHABLE) &&
        (!neighbor_cache[i].isrouter)) {
      if (neighbor_cache[i].counter.reachable_time < time) {
        j = i;
        time = neighbor_cache[i].counter.reachable_time;
      }
    }
  }
  if (j >= 0) {
    nd6_free_neighbor_cache_entry(j);
    return j;
  }

  /* Next, find oldest incomplete entry without queued packets. */
  time = 0;
  j = -1;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (
        (neighbor_cache[i].q == NULL) &&
        (neighbor_cache[i].state == ND6_INCOMPLETE) &&
        (!neighbor_cache[i].isrouter)) {
      if (neighbor_cache[i].counter.probes_sent >= time) {
        j = i;
        time = neighbor_cache[i].counter.probes_sent;
      }
    }
  }
  if (j >= 0) {
    nd6_free_neighbor_cache_entry(j);
    return j;
  }

  /* Next, find oldest incomplete entry with queued packets. */
  time = 0;
  j = -1;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_INCOMPLETE) &&
        (!neighbor_cache[i].isrouter)) {
      if (neighbor_cache[i].counter.probes_sent >= time) {
        j = i;
        time = neighbor_cache[i].counter.probes_sent;
      }
    }
  }
  if (j >= 0) {
    nd6_free_neighbor_cache_entry(j);
    return j;
  }

  /* No more entries to try. */
  return -1;
}

/**
 * Will free any resources associated with a neighbor cache
 * entry, and will mark it as unused.
 *
 * @param i the neighbor cache entry index to free
 */
static void
nd6_free_neighbor_cache_entry(s8_t i)
{
  struct nd6_neighbor_cache_entry entry;

  if ((i < 0) || (i >= LWIP_ND6_NUM_NEIGHBORS)) {
    return;
  }

#if !LWIP_RIPPLE
  if (neighbor_cache[i].isrouter) {
    /* isrouter needs to be cleared before deleting a neighbor cache entry */
    return;
  }
#endif

#if LWIP_RIPPLE && LWIP_ND6_ROUTER
  if ((neighbor_cache[i].isrouter != 0) && (neighbor_cache[i].netif != NULL) &&
      (neighbor_cache[i].netif->forwarding == 0)) {
    /* isrouter needs to be cleared before deleting a neighbor cache entry */
    return;
  }
#endif

  /* Free any queued packets. */
  if (neighbor_cache[i].q != NULL) {
    nd6_free_q(neighbor_cache[i].q);
    neighbor_cache[i].q = NULL;
  }

  entry = neighbor_cache[i];
  neighbor_cache[i].state = ND6_NO_ENTRY;
  neighbor_cache[i].isrouter = 0;
  neighbor_cache[i].netif = NULL;
  /* set the lladdr to 0, for removing the entry by mac */
  neighbor_cache[i].lladdrlen = 0;
  (void)memset_s(neighbor_cache[i].lladdr, NETIF_MAX_HWADDR_LEN, 0, NETIF_MAX_HWADDR_LEN);
  neighbor_cache[i].counter.reachable_time = 0;
  ip6_addr_set_zero(&(neighbor_cache[i].next_hop_address));

#if LWIP_RIPPLE
  /* Indicate the RPL Layer that this neighbor is getting removed */
  LWIP_CONF_ND6_NEIGHBOR_STATE_CHANGED(&entry);

  /* Clean the Destination cache */
  nd6_refresh_destination_cache(&(entry.next_hop_address));
#else
  (void)entry;
#endif
}

/**
 * Search for a destination cache entry
 *
 * @param ip6addr the IPv6 address of the destination
 * @return The destination cache entry index that matched, -1 if no
 * entry is found
 */
static s16_t
nd6_find_destination_cache_entry(const ip6_addr_t *ip6addr)
{
  s16_t i;

  IP6_ADDR_ZONECHECK(ip6addr);

  for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) {
    if (ip6_addr_cmp(ip6addr, &(destination_cache[i].destination_addr))) {
      return i;
    }
  }
  return -1;
}

/**
 * Create a new destination cache entry. If no unused entry is found,
 * will recycle oldest entry.
 *
 * @return The destination cache entry index that was created, -1 if no
 * entry was created
 */
static s16_t
nd6_new_destination_cache_entry(void)
{
  s16_t i, j;
  u32_t age;

  /* Find an empty entry. */
  for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) {
    if (ip6_addr_isany(&(destination_cache[i].destination_addr))) {
      return i;
    }
  }

  /* Find oldest entry. */
  age = 0;
  j = LWIP_ND6_NUM_DESTINATIONS - 1;
  for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) {
    if (destination_cache[i].age > age) {
      j = i;
    }
  }

  return j;
}

/**
 * Clear the destination cache.
 *
 * This operation may be necessary for consistency in the light of changing
 * local addresses and/or use of the gateway hook.
 */
void
nd6_clear_destination_cache(void)
{
  int i;

  for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) {
    ip6_addr_set_any(&destination_cache[i].destination_addr);
  }
}

#if !LWIP_RIPPLE || LWIP_ND6_ROUTER
/**
 * Determine whether an address matches an on-link prefix or the subnet of a
 * statically assigned address.
 *
 * @param ip6addr the IPv6 address to match
 * @return 1 if the address is on-link, 0 otherwise
 */
static int
nd6_is_prefix_in_netif(const ip6_addr_t *ip6addr, struct netif *netif)
{
  s8_t i;

  /* Check to see if the address matches an on-link prefix. */
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; i++) {
    if ((prefix_list[i].netif == netif) &&
        (prefix_list[i].invalidation_timer > 0) &&
        ip6_addr_prefix_netcmp(ip6addr, &(prefix_list[i].prefix), (prefix_list[i].prefix_len >> 3))) {
      return 1;
    }
  }

#if !LWIP_RIPPLE
  /* Check to see if address prefix matches a manually configured (= static)
   * address. Static addresses have an implied /64 subnet assignment. Dynamic
   * addresses (from autoconfiguration) have no implied subnet assignment, and
   * are thus effectively /128 assignments. See RFC 5942 for more on this. */
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
        ip6_addr_netcmp(ip6addr, netif_ip6_addr(netif, i))) {
      return 1;
    }
  }
#endif
  return 0;
}
#endif

/**
 * get  an adressing matching prefix on netif
 *
 * @param ip6addr the IPv6 address to match
 * @return 1 if the address is on-link, 0 otherwise
 */
const ip6_addr_t *
nd6_get_prefix_addr(const ip6_addr_t *ip6addr, const struct netif *netif)
{
  if ((ip6addr == NULL) || (netif == NULL)) {
    return NULL;
  }
  s8_t i;
  /* Check to see if address prefix matches a (manually?) configured address. */
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
        ip6_addr_netcmp(ip6addr, netif_ip6_addr(netif, i))) {
      return netif_ip6_addr(netif, i);
    }
  }
  return NULL;
}

/**
 * Select a default router for a destination.
 *
 * This function is used both for routing and for finding a next-hop target for
 * a packet. In the former case, the given netif is NULL, and the returned
 * router entry must be for a netif suitable for sending packets (up, link up).
 * In the latter case, the given netif is not NULL and restricts router choice.
 *
 * @param ip6addr the destination address
 * @param netif the netif for the outgoing packet, if known
 * @return the default router entry index, or -1 if no suitable
 *         router is found
 */
static s8_t
nd6_select_router(const ip6_addr_t *ip6addr, struct netif *netif)
{
  struct netif *router_netif;
  s8_t i, j, valid_router;
  static s8_t last_router;

  LWIP_UNUSED_ARG(ip6addr); /* @todo match preferred routes!! (must implement ND6_OPTION_TYPE_ROUTE_INFO) */

  /* @todo: implement default router preference */

  /* Look for valid routers. A reachable router is preferred. */
  valid_router = -1;
  for (i = 0; i < LWIP_ND6_NUM_ROUTERS; i++) {
    /* Is the router netif both set and apppropriate? */
    if (default_router_list[i].neighbor_entry != NULL) {
      router_netif = default_router_list[i].neighbor_entry->netif;
      if ((router_netif != NULL) && (netif != NULL ? netif == router_netif :
          (netif_is_up(router_netif) && netif_is_link_up(router_netif)))) {
        /* Is the router valid, i.e., reachable or probably reachable as per
         * RFC 4861 Sec. 6.3.6? Note that we will never return a router that
         * has no neighbor cache entry, due to the netif association tests. */
        if (default_router_list[i].neighbor_entry->state != ND6_INCOMPLETE) {
          /* Is the router known to be reachable? */
          if ((default_router_list[i].neighbor_entry->state == ND6_REACHABLE)
#if LWIP_ND6_STATIC_NBR
              || (default_router_list[i].neighbor_entry->state == ND6_PERMANENT)
#endif
             ) {
            return i; /* valid and reachable - done! */
          } else if (valid_router < 0) {
            valid_router = i; /* valid but not known to be reachable */
          }
        }
      }
    }
  }
  if (valid_router >= 0) {
    return valid_router;
  }

  /* Look for any router for which we have any information at all. */
  /* last_router is used for round-robin selection of incomplete routers, as
   * recommended in RFC 4861 Sec. 6.3.6 point (2). Advance only when picking a
   * route, to select the same router as next-hop target in the common case. */
  if ((netif == NULL) && (++last_router >= LWIP_ND6_NUM_ROUTERS)) {
    last_router = 0;
  }
  i = last_router;
  for (j = 0; j < LWIP_ND6_NUM_ROUTERS; j++) {
    if (default_router_list[i].neighbor_entry != NULL) {
      router_netif = default_router_list[i].neighbor_entry->netif;
      if ((router_netif != NULL) && (netif != NULL ? netif == router_netif :
          (netif_is_up(router_netif) && netif_is_link_up(router_netif)))) {
        return i;
      }
    }
    if (++i >= LWIP_ND6_NUM_ROUTERS) {
      i = 0;
    }
  }

  /* no suitable router found. */
  return -1;
}

/**
 * Find a router-announced route to the given destination. This route may be
 * based on an on-link prefix or a default router.
 *
 * If a suitable route is found, the returned netif is guaranteed to be in a
 * suitable state (up, link up) to be used for packet transmission.
 *
 * @param ip6addr the destination IPv6 address
 * @return the netif to use for the destination, or NULL if none found
 */
struct netif *
nd6_find_route(const ip6_addr_t *ip6addr)
{
  struct netif *netif;
  s8_t i;

  /* @todo decide if it makes sense to check the destination cache first */

  /* Check if there is a matching on-link prefix. There may be multiple
   * matches. Pick the first one that is associated with a suitable netif. */
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; ++i) {
    netif = prefix_list[i].netif;
    if ((netif != NULL) && ip6_addr_netcmp(&prefix_list[i].prefix, ip6addr) &&
        netif_is_up(netif) && netif_is_link_up(netif)) {
      return netif;
    }
  }

  /* No on-link prefix match. Find a router that can forward the packet. */
  i = nd6_select_router(ip6addr, NULL);
  if (i >= 0) {
    LWIP_ASSERT("selected router must have a neighbor entry",
      default_router_list[i].neighbor_entry != NULL);
    return default_router_list[i].neighbor_entry->netif;
  }

  return NULL;
}

/**
 * Find an entry for a default router.
 *
 * @param router_addr the IPv6 address of the router
 * @param netif the netif on which the router is found, if known
 * @return the index of the router entry, or -1 if not found
 */
static s8_t
nd6_get_router(const ip6_addr_t *router_addr, struct netif *netif)
{
  s8_t i;

  IP6_ADDR_ZONECHECK_NETIF(router_addr, netif);

  /* Look for router. */
  for (i = 0; i < LWIP_ND6_NUM_ROUTERS; i++) {
    if ((default_router_list[i].neighbor_entry != NULL) &&
        ((netif != NULL) ? netif == default_router_list[i].neighbor_entry->netif : 1) &&
        ip6_addr_cmp(router_addr, &(default_router_list[i].neighbor_entry->next_hop_address))) {
      return i;
    }
  }

  /* router not found. */
  return -1;
}

/**
 * Create a new entry for a default router.
 *
 * @param router_addr the IPv6 address of the router
 * @param netif the netif on which the router is connected, if known
 * @return the index on the router table, or -1 if could not be created
 */
static s8_t
nd6_new_router(const ip6_addr_t *router_addr, struct netif *netif)
{
  s8_t router_index;
  s8_t free_router_index;
  s8_t neighbor_index;

  IP6_ADDR_ZONECHECK_NETIF(router_addr, netif);

  /* Do we have a neighbor entry for this router? */
  neighbor_index = nd6_find_neighbor_cache_entry(router_addr, netif);
  if (neighbor_index < 0) {
    /* Create a neighbor entry for this router. */
    neighbor_index = nd6_new_neighbor_cache_entry();
    if (neighbor_index < 0) {
      /* Could not create neighbor entry for this router. */
      ND6_STATS_INC(nd6.cache_full);
      return -1;
    }
    ip6_addr_set(&(neighbor_cache[neighbor_index].next_hop_address), router_addr);
    neighbor_cache[neighbor_index].netif = netif;
    neighbor_cache[neighbor_index].q = NULL;
    neighbor_cache[neighbor_index].state = ND6_INCOMPLETE;
    neighbor_cache[neighbor_index].counter.probes_sent = 1;
    nd6_send_neighbor_cache_probe(&neighbor_cache[neighbor_index], ND6_SEND_FLAG_MULTICAST_DEST);
  }

  /* Mark neighbor as router. */
  neighbor_cache[neighbor_index].isrouter = 1;

  /* Look for empty entry. */
  free_router_index = LWIP_ND6_NUM_ROUTERS;
  for (router_index = LWIP_ND6_NUM_ROUTERS - 1; router_index >= 0; router_index--) {
    /* check if router already exists (this is a special case for 2 netifs on the same subnet
       - e.g. wifi and cable) */
    if(default_router_list[router_index].neighbor_entry == &(neighbor_cache[neighbor_index])){
      return router_index;
    }
    if (default_router_list[router_index].neighbor_entry == NULL) {
      /* remember lowest free index to create a new entry */
      free_router_index = router_index;
    }
  }
  if (free_router_index < LWIP_ND6_NUM_ROUTERS) {
    default_router_list[free_router_index].neighbor_entry = &(neighbor_cache[neighbor_index]);
    return free_router_index;
  }

  /* Could not create a router entry. */

  /* Mark neighbor entry as not-router. Entry might be useful as neighbor still. */
  neighbor_cache[neighbor_index].isrouter = 0;

  /* router not found. */
  return -1;
}

/*
 * Will free any resources associated with a default router list
 * entry, and will mark it as unused.
 *
 * @param i the default entry index to free
 */
static void
nd6_free_router(s8_t i)
{
  if ((i < 0) || (i >= LWIP_ND6_NUM_ROUTERS)) {
    return;
  }

  /*
   * the node MUST remove that router from the Default Router List and update
   * the Destination Cache entries for all destinations using that neighbor as
   * a router as specified in Section 7.3.3.
   */
  if (default_router_list[i].neighbor_entry != NULL) {
    s8_t j;
    for (j = 0; j < LWIP_ND6_NUM_DESTINATIONS; j++) {
      if (ip6_addr_cmp(&destination_cache[j].next_hop_addr,
          &default_router_list[i].neighbor_entry->next_hop_address)) {
        ip6_addr_set_any(&destination_cache[j].destination_addr);
      }
    }
    default_router_list[i].neighbor_entry->isrouter = 0;
    default_router_list[i].neighbor_entry = NULL;
    default_router_list[i].invalidation_timer = 0;
    default_router_list[i].flags = 0;
  }
  return;
}

/**
 * Find the cached entry for an on-link prefix.
 *
 * @param prefix the IPv6 prefix that is on-link
 * @param netif the netif on which the prefix is on-link
 * @return the index on the prefix table, or -1 if not found
 */
static s8_t
nd6_get_onlink_prefix(const ip6_addr_t *prefix, struct netif *netif)
{
  s8_t i;

  /* Look for prefix in list. */
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; ++i) {
    if ((ip6_addr_netcmp(&(prefix_list[i].prefix), prefix)) &&
        (prefix_list[i].netif == netif)) {
      return i;
    }
  }

  /* Entry not available. */
  return -1;
}
#if defined(LWIP_RA_PREFIX_DYNAMIC) && LWIP_RA_PREFIX_DYNAMIC
static s8_t
nd6_get_onlink_prefix_with_len(const ip6_addr_t *prefix, u8_t len, struct netif *netif)
{
  s8_t i;

  /* Look for prefix in list. */
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; ++i) {
    if ((ip6_addr_prefix_netcmp(&(prefix_list[i].prefix), prefix, len >> 3)) &&
        (prefix_list[i].prefix_len == len) &&
        (prefix_list[i].netif == netif)) {
      return i;
    }
  }

  /* Entry not available. */
  return -1;
}
#endif

/**
 * Creates a new entry for an on-link prefix.
 *
 * @param prefix the IPv6 prefix that is on-link
 * @param netif the netif on which the prefix is on-link
 * @return the index on the prefix table, or -1 if not created
 */
static s8_t
nd6_new_onlink_prefix(const ip6_addr_t *prefix, struct netif *netif)
{
  s8_t i;

  /* Create new entry. */
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; ++i) {
    if ((prefix_list[i].netif == NULL) ||
        (prefix_list[i].invalidation_timer == 0)) {
      /* Found empty prefix entry. */
      prefix_list[i].netif = netif;
      ip6_addr_set(&(prefix_list[i].prefix), prefix);
      return i;
    }
  }

  /* Entry not available. */
  return -1;
}

/**
 * Determine the next hop for a destination. Will determine if the
 * destination is on-link, else a suitable on-link router is selected.
 *
 * The last entry index is cached for fast entry search.
 *
 * @param ip6addr the destination address
 * @param netif the netif on which the packet will be sent
 * @return the neighbor cache entry for the next hop, ERR_RTE if no
 *         suitable next hop was found, ERR_MEM if no cache entry
 *         could be created
 */
static s8_t
nd6_get_next_hop_entry(const ip6_addr_t *ip6addr, struct netif *netif)
{
#ifdef LWIP_HOOK_ND6_GET_GW
  const ip6_addr_t *next_hop_addr;
#endif /* LWIP_HOOK_ND6_GET_GW */
  s8_t i;
  s16_t dst_idx;

  IP6_ADDR_ZONECHECK_NETIF(ip6addr, netif);

#if LWIP_NETIF_HWADDRHINT
  if (netif->hints != NULL) {
    /* per-pcb cached entry was given */
    netif_addr_idx_t addr_hint = netif->hints->addr_hint;
    if (addr_hint < LWIP_ND6_NUM_DESTINATIONS) {
      nd6_cached_destination_index = addr_hint;
    }
  }
#endif /* LWIP_NETIF_HWADDRHINT */

  /* Look for ip6addr in destination cache. */
  if (ip6_addr_cmp(ip6addr, &(destination_cache[nd6_cached_destination_index].destination_addr))) {
    /* the cached entry index is the right one! */
    /* do nothing. */
    ND6_STATS_INC(nd6.cachehit);
  } else {
    /* Search destination cache. */
    dst_idx = nd6_find_destination_cache_entry(ip6addr);
    if (dst_idx >= 0) {
      /* found destination entry. make it our new cached index. */
      LWIP_ASSERT("type overflow", (size_t)dst_idx < NETIF_ADDR_IDX_MAX);
      nd6_cached_destination_index = (netif_addr_idx_t)dst_idx;
    } else {
      /* Not found. Create a new destination entry. */
      dst_idx = nd6_new_destination_cache_entry();
      if (dst_idx >= 0) {
        /* got new destination entry. make it our new cached index. */
        LWIP_ASSERT("type overflow", (size_t)dst_idx < NETIF_ADDR_IDX_MAX);
        nd6_cached_destination_index = (netif_addr_idx_t)dst_idx;
      } else {
        /* Could not create a destination cache entry. */
        ND6_STATS_INC(nd6.cache_full);
        return ERR_MEM;
      }

      /* Copy dest address to destination cache. */
      ip6_addr_set(&(destination_cache[nd6_cached_destination_index].destination_addr), ip6addr);

      /* Now find the next hop. is it a neighbor? */
      if (ip6_addr_islinklocal(ip6addr)
#if !LWIP_RIPPLE
         || nd6_is_prefix_in_netif(ip6addr, netif)
#endif
         ) {
        /* Destination in local link. */
        destination_cache[nd6_cached_destination_index].pmtu = netif_mtu6(netif);
        ip6_addr_copy(destination_cache[nd6_cached_destination_index].next_hop_addr, destination_cache[nd6_cached_destination_index].destination_addr);
#ifdef LWIP_HOOK_ND6_GET_GW
      } else if ((next_hop_addr = LWIP_HOOK_ND6_GET_GW(netif, ip6addr)) != NULL) {
        /* Next hop for destination provided by hook function. */
        destination_cache[nd6_cached_destination_index].pmtu = netif->mtu;
        ip6_addr_set(&destination_cache[nd6_cached_destination_index].next_hop_addr, next_hop_addr);
#endif /* LWIP_HOOK_ND6_GET_GW */
#if LWIP_ND6_ROUTER
      /* should try the same prefix addr if the node is a sta */
      } else if ((netif->forwarding == 0) && nd6_is_prefix_in_netif(ip6addr, netif)) {
        destination_cache[nd6_cached_destination_index].pmtu = netif_mtu6(netif);
        ip6_addr_copy(destination_cache[nd6_cached_destination_index].next_hop_addr,
                      destination_cache[nd6_cached_destination_index].destination_addr);
#endif
      } else {
        /* We need to select a router. */
        i = nd6_select_router(ip6addr, netif);
        if (i < 0) {
          /* No router found. */
          ip6_addr_set_any(&(destination_cache[nd6_cached_destination_index].destination_addr));
          return ERR_RTE;
        }
        destination_cache[nd6_cached_destination_index].pmtu = netif_mtu6(netif); /* Start with netif mtu, correct through ICMPv6 if necessary */
        ip6_addr_copy(destination_cache[nd6_cached_destination_index].next_hop_addr, default_router_list[i].neighbor_entry->next_hop_address);
      }
    }
  }

#if LWIP_NETIF_HWADDRHINT
  if (netif->hints != NULL) {
    /* per-pcb cached entry was given */
    netif->hints->addr_hint = nd6_cached_destination_index;
  }
#endif /* LWIP_NETIF_HWADDRHINT */

  /* Look in neighbor cache for the next-hop address. */
  if (ip6_addr_cmp(&(destination_cache[nd6_cached_destination_index].next_hop_addr),
                   &(neighbor_cache[nd6_cached_neighbor_index].next_hop_address))) {
    /* Cache hit. */
    /* Do nothing. */
    ND6_STATS_INC(nd6.cachehit);
  } else {
    i = nd6_find_neighbor_cache_entry(&(destination_cache[nd6_cached_destination_index].next_hop_addr), netif);
    if (i >= 0) {
      /* Found a matching record, make it new cached entry. */
      nd6_cached_neighbor_index = i;
    } else {
      /* Neighbor not in cache. Make a new entry. */
      i = nd6_new_neighbor_cache_entry();
      if (i >= 0) {
        /* got new neighbor entry. make it our new cached index. */
        nd6_cached_neighbor_index = i;
      } else {
        /* Could not create a neighbor cache entry. */
        ND6_STATS_INC(nd6.cache_full);
        return ERR_MEM;
      }

      /* Initialize fields. */
      ip6_addr_copy(neighbor_cache[i].next_hop_address,
                   destination_cache[nd6_cached_destination_index].next_hop_addr);
      neighbor_cache[i].isrouter = 0;
      neighbor_cache[i].netif = netif;
      neighbor_cache[i].state = ND6_INCOMPLETE;
      neighbor_cache[i].counter.probes_sent = 1;
      nd6_send_neighbor_cache_probe(&neighbor_cache[i], ND6_SEND_FLAG_MULTICAST_DEST);
    }
  }

  /* Reset this destination's age. */
  destination_cache[nd6_cached_destination_index].age = 0;

  return nd6_cached_neighbor_index;
}

s8_t netif_find_dst_ip6addr_mac_addr(const ip_addr_t *ipaddr, ip_addr_t **dst_addr, struct eth_addr **eth_ret)
{
  struct netif *netif = NULL;

  int dst_index;
  int neighbour_index;

  netif = netif_find_by_ipaddr(ipaddr);
  if (netif == NULL) {
    return -1;
  }

  dst_index = nd6_find_destination_cache_entry(&((*dst_addr)->u_addr.ip6));
  if (dst_index < 0) {
    return -1;
  }
  neighbour_index = nd6_find_neighbor_cache_entry(&(destination_cache[dst_index].next_hop_addr), netif);
  if (neighbour_index < 0) {
    return -1;
  }

  if (memcpy_s((*eth_ret)->addr, sizeof(struct eth_addr), neighbor_cache[neighbour_index].lladdr,
               neighbor_cache[neighbour_index].lladdrlen) != EOK) {
    return -1;
  }

  return 0;
}

/**
 * Queue a packet for a neighbor.
 *
 * @param neighbor_index the index in the neighbor cache table
 * @param q packet to be queued
 * @return ERR_OK if succeeded, ERR_MEM if out of memory
 */
static err_t
nd6_queue_packet(s8_t neighbor_index, struct pbuf *q)
{
  err_t result = ERR_MEM;
  struct pbuf *p;
  int copy_needed = 0;
#if LWIP_ND6_QUEUEING
  struct nd6_q_entry *new_entry, *r;
#endif /* LWIP_ND6_QUEUEING */

  if ((neighbor_index < 0) || (neighbor_index >= LWIP_ND6_NUM_NEIGHBORS)) {
    return ERR_ARG;
  }

  /* IF q includes a pbuf that must be copied, we have to copy the whole chain
   * into a new PBUF_RAM. See the definition of PBUF_NEEDS_COPY for details. */
  p = q;
  while (p) {
    if (PBUF_NEEDS_COPY(p)) {
      copy_needed = 1;
      break;
    }
    p = p->next;
  }
  if (copy_needed) {
    /* copy the whole packet into new pbufs */
    p = pbuf_clone(PBUF_LINK, PBUF_RAM, q);
    while ((p == NULL) && (neighbor_cache[neighbor_index].q != NULL)) {
      /* Free oldest packet (as per RFC recommendation) */
#if LWIP_ND6_QUEUEING
      r = neighbor_cache[neighbor_index].q;
      neighbor_cache[neighbor_index].q = r->next;
      r->next = NULL;
      nd6_free_q(r);
#else /* LWIP_ND6_QUEUEING */
      pbuf_free(neighbor_cache[neighbor_index].q);
      neighbor_cache[neighbor_index].q = NULL;
#endif /* LWIP_ND6_QUEUEING */
      p = pbuf_clone(PBUF_LINK, PBUF_RAM, q);
    }
    if (p != NULL) {
      p->flags = q->flags;
#if LWIP_SO_PRIORITY
      p->priority = q->priority;
#endif /* LWIP_SO_PRIORITY */
    }
  } else {
    /* referencing the old pbuf is enough */
    p = q;
    pbuf_ref(p);
  }
  /* packet was copied/ref'd? */
  if (p != NULL) {
    /* queue packet ... */
#if LWIP_ND6_QUEUEING
    /* allocate a new nd6 queue entry */
    new_entry = (struct nd6_q_entry *)memp_malloc(MEMP_ND6_QUEUE);
    if ((new_entry == NULL) && (neighbor_cache[neighbor_index].q != NULL)) {
      /* Free oldest packet (as per RFC recommendation) */
      r = neighbor_cache[neighbor_index].q;
      neighbor_cache[neighbor_index].q = r->next;
      r->next = NULL;
      nd6_free_q(r);
      new_entry = (struct nd6_q_entry *)memp_malloc(MEMP_ND6_QUEUE);
    }
    if (new_entry != NULL) {
      new_entry->next = NULL;
      new_entry->p = p;
      if (neighbor_cache[neighbor_index].q != NULL) {
        /* queue was already existent, append the new entry to the end */
        r = neighbor_cache[neighbor_index].q;
        while (r->next != NULL) {
          r = r->next;
        }
        r->next = new_entry;
      } else {
        /* queue did not exist, first item in queue */
        neighbor_cache[neighbor_index].q = new_entry;
      }
      LWIP_DEBUGF(LWIP_DBG_TRACE, ("ipv6: queued packet %p on neighbor entry %"S16_F"\n", (void *)p, (s16_t)neighbor_index));
      result = ERR_OK;
    } else {
      /* the pool MEMP_ND6_QUEUE is empty */
      pbuf_free(p);
      LWIP_DEBUGF(LWIP_DBG_TRACE, ("ipv6: could not queue a copy of packet %p (out of memory)\n", (void *)p));
      /* { result == ERR_MEM } through initialization */
    }
#else /* LWIP_ND6_QUEUEING */
    /* Queue a single packet. If an older packet is already queued, free it as per RFC. */
    if (neighbor_cache[neighbor_index].q != NULL) {
      pbuf_free(neighbor_cache[neighbor_index].q);
    }
    neighbor_cache[neighbor_index].q = p;
    LWIP_DEBUGF(LWIP_DBG_TRACE, ("ipv6: queued packet %p on neighbor entry %"S16_F"\n", (void *)p, (s16_t)neighbor_index));
    result = ERR_OK;
#endif /* LWIP_ND6_QUEUEING */
  } else {
    LWIP_DEBUGF(LWIP_DBG_TRACE, ("ipv6: could not queue a copy of packet %p (out of memory)\n", (void *)q));
    /* { result == ERR_MEM } through initialization */
  }

  return result;
}

#if LWIP_ND6_QUEUEING
/**
 * Free a complete queue of nd6 q entries
 *
 * @param q a queue of nd6_q_entry to free
 */
static void
nd6_free_q(struct nd6_q_entry *q)
{
  struct nd6_q_entry *r;
  LWIP_ASSERT("q != NULL", q != NULL);
  LWIP_ASSERT("q->p != NULL", q->p != NULL);
  while (q) {
    r = q;
    q = q->next;
    LWIP_ASSERT("r->p != NULL", (r->p != NULL));
    pbuf_free(r->p);
    memp_free(MEMP_ND6_QUEUE, r);
  }
}
#endif /* LWIP_ND6_QUEUEING */

/**
 * Send queued packets for a neighbor
 *
 * @param i the neighbor to send packets to
 */
static void
nd6_send_q(s8_t i)
{
  struct ip6_hdr *ip6hdr;
  ip6_addr_t dest;
#if LWIP_ND6_QUEUEING
  struct nd6_q_entry *q;
#endif /* LWIP_ND6_QUEUEING */

  if ((i < 0) || (i >= LWIP_ND6_NUM_NEIGHBORS)) {
    return;
  }

#if LWIP_ND6_QUEUEING
  while (neighbor_cache[i].q != NULL) {
    /* remember first in queue */
    q = neighbor_cache[i].q;
    /* pop first item off the queue */
    neighbor_cache[i].q = q->next;
    /* Get ipv6 header. */
    ip6hdr = (struct ip6_hdr *)(q->p->payload);
    /* Create an aligned copy. */
    ip6_addr_copy_from_packed(dest, ip6hdr->dest);
    /* Restore the zone, if applicable. */
    ip6_addr_assign_zone(&dest, IP6_UNKNOWN, neighbor_cache[i].netif);
    LWIP_DEBUGF(LWIP_DBG_TRACE, ("ipv6: queued packet %p on neighbor entry %"S16_F"\n",
                (void *)q->p, (s16_t)i));
    /* send the queued IPv6 packet */
    (neighbor_cache[i].netif)->output_ip6(neighbor_cache[i].netif, q->p, &dest);
    /* free the queued IP packet */
    pbuf_free(q->p);
    /* now queue entry can be freed */
    memp_free(MEMP_ND6_QUEUE, q);
  }
#else /* LWIP_ND6_QUEUEING */
  if (neighbor_cache[i].q != NULL) {
    /* Get ipv6 header. */
    ip6hdr = (struct ip6_hdr *)(neighbor_cache[i].q->payload);
    /* Create an aligned copy. */
    ip6_addr_copy_from_packed(dest, ip6hdr->dest);
    /* Restore the zone, if applicable. */
    ip6_addr_assign_zone(&dest, IP6_UNKNOWN, neighbor_cache[i].netif);
    /* send the queued IPv6 packet */
    (neighbor_cache[i].netif)->output_ip6(neighbor_cache[i].netif, neighbor_cache[i].q, &dest);
    /* free the queued IP packet */
    pbuf_free(neighbor_cache[i].q);
    neighbor_cache[i].q = NULL;
  }
#endif /* LWIP_ND6_QUEUEING */
}

/**
 * A packet is to be transmitted to a specific IPv6 destination on a specific
 * interface. Check if we can find the hardware address of the next hop to use
 * for the packet. If so, give the hardware address to the caller, which should
 * use it to send the packet right away. Otherwise, enqueue the packet for
 * later transmission while looking up the hardware address, if possible.
 *
 * As such, this function returns one of three different possible results:
 *
 * - ERR_OK with a non-NULL 'hwaddrp': the caller should send the packet now.
 * - ERR_OK with a NULL 'hwaddrp': the packet has been enqueued for later.
 * - not ERR_OK: something went wrong; forward the error upward in the stack.
 *
 * @param netif The lwIP network interface on which the IP packet will be sent.
 * @param q The pbuf(s) containing the IP packet to be sent.
 * @param ip6addr The destination IPv6 address of the packet.
 * @param hwaddrp On success, filled with a pointer to a HW address or NULL (meaning
 *        the packet has been queued).
 * @return
 * - ERR_OK on success, ERR_RTE if no route was found for the packet,
 * or ERR_MEM if low memory conditions prohibit sending the packet at all.
 */
err_t
nd6_get_next_hop_addr_or_queue(struct netif *netif, struct pbuf *q, const ip6_addr_t *ip6addr, const u8_t **hwaddrp)
{
  s8_t i;

  /* Get next hop record. */
  i = nd6_get_next_hop_entry(ip6addr, netif);
  if (i < 0) {
    /* failed to get a next hop neighbor record. */
    return i;
  }

  /* Now that we have a destination record, send or queue the packet. */
  if (neighbor_cache[i].state == ND6_STALE) {
    /* Switch to delay state. */
    neighbor_cache[i].state = ND6_DELAY;
    neighbor_cache[i].counter.delay_time = LWIP_ND6_DELAY_FIRST_PROBE_TIME / ND6_TMR_INTERVAL;
  }
  /* @todo should we send or queue if PROBE? send for now, to let unicast NS pass. */
  if ((neighbor_cache[i].state == ND6_REACHABLE) ||
      (neighbor_cache[i].state == ND6_DELAY) ||
      (neighbor_cache[i].state == ND6_PROBE)
#if LWIP_ND6_STATIC_NBR
      || (neighbor_cache[i].state == ND6_PERMANENT)
#endif
     ) {
    /* Tell the caller to send out the packet now. */
    *hwaddrp = neighbor_cache[i].lladdr;
    return ERR_OK;
  }

  /* We should queue packet on this interface. */
  *hwaddrp = NULL;
  return nd6_queue_packet(i, q);
}


/**
 * Get the Path MTU for a destination.
 *
 * @param ip6addr the destination address
 * @param netif the netif on which the packet will be sent
 * @return the Path MTU, if known, or the netif default MTU
 */
u16_t
nd6_get_destination_mtu(const ip6_addr_t *ip6addr, struct netif *netif)
{
  s16_t i;

  i = nd6_find_destination_cache_entry(ip6addr);
  if (i >= 0) {
    if (destination_cache[i].pmtu > 0) {
      return destination_cache[i].pmtu;
    }
  }

  if (netif != NULL) {
    return netif_mtu6(netif);
  }

  return IP6_MIN_MTU_LENGTH; /* Minimum MTU */
}


#if LWIP_ND6_TCP_REACHABILITY_HINTS
/**
 * Provide the Neighbor discovery process with a hint that a
 * destination is reachable. Called by tcp_receive when ACKs are
 * received or sent (as per RFC). This is useful to avoid sending
 * NS messages every 30 seconds.
 *
 * @param ip6addr the destination address which is know to be reachable
 *                by an upper layer protocol (TCP)
 */
void
nd6_reachability_hint(const ip6_addr_t *ip6addr)
{
  s8_t i;
  s16_t dst_idx;
  struct netif *netif = NULL;

  /* Find destination in cache. */
  if (ip6_addr_cmp(ip6addr, &(destination_cache[nd6_cached_destination_index].destination_addr))) {
    dst_idx = nd6_cached_destination_index;
    ND6_STATS_INC(nd6.cachehit);
  } else {
    dst_idx = nd6_find_destination_cache_entry(ip6addr);
  }
  if (dst_idx < 0) {
    return;
  }

  /* Find next hop neighbor in cache. */
  if (ip6_addr_cmp(&(destination_cache[dst_idx].next_hop_addr), &(neighbor_cache[nd6_cached_neighbor_index].next_hop_address))) {
    i = nd6_cached_neighbor_index;
    ND6_STATS_INC(nd6.cachehit);
  } else {
    netif = netif_find_by_ipaddr(ip_current_dest_addr());
    i = nd6_find_neighbor_cache_entry(&(destination_cache[dst_idx].next_hop_addr), netif);
  }
  if (i < 0) {
    ND6_STATS_INC(nd6.cache_miss);
    return;
  }

  /* For safety: don't set as reachable if we don't have a LL address yet. Misuse protection. */
  if (neighbor_cache[i].state == ND6_INCOMPLETE || neighbor_cache[i].state == ND6_NO_ENTRY) {
    return;
  }

  /* Set reachability state. */
  neighbor_cache[i].state = ND6_REACHABLE;
  neighbor_cache[i].counter.reachable_time = reachable_time;
}
#endif /* LWIP_ND6_TCP_REACHABILITY_HINTS */

/**
 * Remove all prefix, neighbor_cache and router entries of the specified netif.
 *
 * @param netif points to a network interface
 */
void
nd6_cleanup_netif(struct netif *netif)
{
  u8_t i, j;
  s8_t router_index;
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; i++) {
    if (prefix_list[i].netif == netif) {
      prefix_list[i].netif = NULL;
    }
  }
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (neighbor_cache[i].netif == netif) {
      for (j = 0; j < LWIP_ND6_NUM_DESTINATIONS; j++) {
        if (!ip6_addr_isany(&destination_cache[j].destination_addr) &&
            ip6_addr_cmp(&(destination_cache[j].next_hop_addr), &(neighbor_cache[i].next_hop_address))) {
          ip6_addr_set_any(&destination_cache[j].destination_addr);
        }
      }

      for (router_index = 0; router_index < LWIP_ND6_NUM_ROUTERS; router_index++) {
        if (default_router_list[router_index].neighbor_entry == &neighbor_cache[i]) {
          default_router_list[router_index].neighbor_entry->isrouter = 0;
          default_router_list[router_index].neighbor_entry = NULL;
          default_router_list[router_index].invalidation_timer = 0;
          default_router_list[router_index].flags = 0;
        }
      }
      neighbor_cache[i].isrouter = 0;
      nd6_free_neighbor_cache_entry((s8_t)i);
    }
  }
#if (LWIP_RIPPLE && LWIP_CONF_UPDATE_BEACON_PRIORITY)
  beacon_priority_update(0);
#endif
}

#if LWIP_IPV6_MLD
/**
 * The state of a local IPv6 address entry is about to change. If needed, join
 * or leave the solicited-node multicast group for the address.
 *
 * @param netif The netif that owns the address.
 * @param addr_idx The index of the address.
 * @param new_state The new (IP6_ADDR_) state for the address.
 */
void
nd6_adjust_mld_membership(struct netif *netif, s8_t addr_idx, u8_t new_state)
{
  u8_t old_state, old_member, new_member;

  old_state = netif_ip6_addr_state(netif, addr_idx);

  /* Determine whether we were, and should be, a member of the solicited-node
   * multicast group for this address. For tentative addresses, the group is
   * not joined until the address enters the TENTATIVE_1 (or VALID) state. */
  old_member = (old_state != IP6_ADDR_INVALID && old_state != IP6_ADDR_DUPLICATED && old_state != IP6_ADDR_TENTATIVE);
  new_member = (new_state != IP6_ADDR_INVALID && new_state != IP6_ADDR_DUPLICATED && new_state != IP6_ADDR_TENTATIVE);

  if (old_member != new_member) {
    ip6_addr_set_solicitednode(&multicast_address, netif_ip6_addr(netif, addr_idx)->addr[3]);
    ip6_addr_assign_zone(&multicast_address, IP6_MULTICAST, netif);

    if (new_member) {
      mld6_joingroup_netif(netif, &multicast_address);
    } else {
      mld6_leavegroup_netif(netif, &multicast_address);
    }
  }
}
#endif /* LWIP_IPV6_MLD */

#if LWIP_IPV6_AUTOCONFIG
/*
 * clear the netif's IPV6 address which is autoconfiged from RA
 *
 * @param netif the netif to clear IPV6 address
 */
void
nd6_clear_netif_autoconf_address(struct netif *netif)
{
  s8_t i, j;

  if (netif == NULL) {
    return;
  }

  /* Skip 0 index (link-local address) */
  for (j = 1; j < LWIP_IPV6_NUM_ADDRESSES; j++) {
    if (netif_ip6_addr_state(netif, j) != IP6_ADDR_INVALID) {
      i = nd6_get_onlink_prefix((ip6_addr_t *)netif_ip6_addr(netif, j), netif);
      if ((i >= 0) && !netif_ip6_addr_isstatic(netif, j) && !netif_ip6_addr_isdhcp6(netif, j)) {
        netif_ip6_addr_set_state(netif, j, IP6_ADDR_INVALID);
      }
    }
  }

  return;
}
#endif

/** Netif was added, set up, or reconnected (link up) */
void
nd6_restart_netif(struct netif *netif)
{
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
  /* Send Router Solicitation messages (see RFC 4861, ch. 6.3.7). */
  netif->rs_count = LWIP_ND6_MAX_MULTICAST_SOLICIT;
#else
  LWIP_UNUSED_ARG(netif);
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */
}

/*
 * Report ND6 messages for this interface
 *
 * @param netif network interface on which report ND6 messages
 */
void
nd6_report_groups(struct netif *netif, s8_t addr_idx)
{
  int i;
  if ((netif->flags & (NETIF_FLAG_ETHARP)) == 0) {
    return;
  }
  /*
   * Publish all new address after hw addr changed, note : link local address remains same,
   * since it needs to be reconfigured by user.
   */
  /*
   * RFC4861 section 7.2.6: The Override flag MAY be set to either zero or one. In either case, neighboring
   * nodes will immediately change the state of their Neighbor Cache entries for the Target Address to STALE,
   * prompting them to verify the path for reachability. If the Override flag is set to one, neighboring nodes will
   * install the new link-layer address in their caches. Otherwise, they will ignore the new link-layer address,
   * choosing instead to probe the cached address.
   */
  /*
   * lwip sets Override flag to one to make the neighboring nodes update their neighbor cache immediately,
   * this would avoid network failure if the peer side still use the old hw address.
   */
  if (addr_idx == (s8_t)LWIP_INVALID_IPV6_IDX) {
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
        nd6_send_na(netif, netif_ip6_addr(netif, i), ND6_FLAG_OVERRIDE | ND6_SEND_FLAG_ALLNODES_DEST);
      }
    }
#if (LWIP_NAT64 && LWIP_NA_PROXY_UNSOLICITED)
    nat64_proxy_unsolicited_na(netif);
#endif
  } else {
    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, addr_idx))) {
      nd6_send_na(netif, netif_ip6_addr(netif, addr_idx), ND6_FLAG_OVERRIDE | ND6_SEND_FLAG_ALLNODES_DEST);
    }
  }
}

#endif /* LWIP_IPV6 */
