/**
 * @file
 * Multicast listener discovery
 *
 * @defgroup mld6 MLD6
 * @ingroup ip6
 * Multicast listener discovery for IPv6. Aims to be compliant with RFC 2710.
 * No support for MLDv2.\n
 * Note: The allnodes (ff01::1, ff02::1) group is assumed be received by your 
 * netif since it must always be received for correct IPv6 operation (e.g. SLAAC).
 * Ensure the netif filters are configured accordingly!\n
 * The netif flags also need NETIF_FLAG_MLD6 flag set to enable MLD6 on a
 * netif ("netif->flags |= NETIF_FLAG_MLD6;").\n
 * To be called from TCPIP thread.
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

/* Based on igmp.c implementation of igmp v2 protocol */

#include "lwip/opt.h"

#if LWIP_IPV6 && (LWIP_IPV6_MLD || LWIP_IPV6_MLD_QUERIER)  /* don't build if not configured for use in lwipopts.h */

#include "lwip/mld6.h"
#include "lwip/prot/mld6.h"
#include "lwip/icmp6.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/ip.h"
#include "lwip/inet_chksum.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/memp.h"
#include "lwip/stats.h"

#include <string.h>


/*
 * MLD constants
 */
#define MLD6_HL                           1
#define MLD6_JOIN_DELAYING_MEMBER_TMR_MS  (500)

#define MLD6_GROUP_NON_MEMBER             0
#define MLD6_GROUP_DELAYING_MEMBER        1
#define MLD6_GROUP_IDLE_MEMBER            2

/* Forward declarations. */
static struct mld_group *mld6_new_group(struct netif *ifp, const ip6_addr_t *addr);
static err_t mld6_remove_group(struct netif *netif, struct mld_group *group);
static void mld6_delayed_report(struct mld_group *group, u16_t maxresp);
static void mld6_send(struct netif *netif, struct mld_group *group, u8_t type);

#if LWIP_IPV6_MLD_QUERIER
#define MLD6_TIMER_SET(TIMER, MS) do { \
  (TIMER) = (u16_t)((MS) / (MLD6_TMR_INTERVAL)); \
  if ((TIMER) == 0) { \
    (TIMER) = 1; \
  } \
} while (0)

static void mld6_querier_tmr(void);
static void mld6_querier_input(struct mld_header *mld_hdr, struct netif *netif);
static void mld6_querier_listener_del(struct netif *netif, struct mld6_listener *listener);
#endif /* LWIP_IPV6_MLD_QUERIER */

/**
 * Stop MLD processing on interface
 *
 * @param netif network interface on which stop MLD processing
 */
err_t
mld6_stop(struct netif *netif)
{
  struct mld_group *group = netif_mld6_data(netif);

  netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_MLD6, NULL);

  while (group != NULL) {
    struct mld_group *next = group->next; /* avoid use-after-free below */

#if LWIP_LINK_MCAST_FILTER
    /* disable the group at the MAC level */
    if (netif->mld_mac_filter != NULL) {
      (void)netif->mld_mac_filter(netif, &(group->group_address), NETIF_DEL_MAC_FILTER);
    }
#endif /* LWIP_LINK_MCAST_FILTER */

    /* free group */
    memp_free(MEMP_MLD6_GROUP, group);

    /* move to "next" */
    group = next;
  }
  return ERR_OK;
}

/**
 * Report MLD memberships for this interface
 *
 * @param netif network interface on which report MLD memberships
 */
void
mld6_report_groups(struct netif *netif)
{
  struct mld_group *group = netif_mld6_data(netif);

  while (group != NULL) {
    /*
     * RFC 2710:MLD messages are never sent for multicast addresses whose scope is 0   (reserved) or 1 (node-local).
     * MLD messages ARE sent for multicast addresses whose scope is 2  (link-local), including Solicited-Node
     *   multicast addresses , except for the link-scope, all-nodes address (FF02::1)
     */
    if ((!(ip6_addr_ismulticast_iflocal(&(group->group_address)))) &&
        (!(ip6_addr_isallnodes_linklocal(&(group->group_address)))) &&
        (!(ip6_addr_ismulticast_reserved_scope(&(group->group_address))))) {
      mld6_delayed_report(group, MLD6_JOIN_DELAYING_MEMBER_TMR_MS);
    }
    group = group->next;
  }
}

/**
 * Search for a group that is joined on a netif
 *
 * @param ifp the network interface for which to look
 * @param addr the group ipv6 address to search for
 * @return a struct mld_group* if the group has been found,
 *         NULL if the group wasn't found.
 */
struct mld_group *
mld6_lookfor_group(struct netif *ifp, const ip6_addr_t *addr)
{
  struct mld_group *group = netif_mld6_data(ifp);

  while (group != NULL) {
    if (ip6_addr_cmp(&(group->group_address), addr)) {
      return group;
    }
    group = group->next;
  }

  return NULL;
}


/**
 * create a new group
 *
 * @param ifp the network interface for which to create
 * @param addr the new group ipv6
 * @return a struct mld_group*,
 *         NULL on memory error.
 */
static struct mld_group *
mld6_new_group(struct netif *ifp, const ip6_addr_t *addr)
{
  struct mld_group *group;

  group = (struct mld_group *)memp_malloc(MEMP_MLD6_GROUP);
  if (group != NULL) {
    ip6_addr_set(&(group->group_address), addr);
    group->timer              = 0; /* Not running */
    group->group_state        = MLD6_GROUP_IDLE_MEMBER;
    group->last_reporter_flag = 0;
    group->use                = 0;
    group->next               = netif_mld6_data(ifp);

    netif_set_client_data(ifp, LWIP_NETIF_CLIENT_DATA_INDEX_MLD6, group);
  }

  return group;
}

/**
 * Remove a group from the mld_group_list, but do not free it yet
 *
 * @param group the group to remove
 * @return ERR_OK if group was removed from the list, an err_t otherwise
 */
static err_t
mld6_remove_group(struct netif *netif, struct mld_group *group)
{
  err_t err = ERR_OK;

  /* Is it the first group? */
  if (netif_mld6_data(netif) == group) {
    netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_MLD6, group->next);
  } else {
    /* look for group further down the list */
    struct mld_group *tmpGroup;
    for (tmpGroup = netif_mld6_data(netif); tmpGroup != NULL; tmpGroup = tmpGroup->next) {
      if (tmpGroup->next == group) {
        tmpGroup->next = group->next;
        break;
      }
    }
    /* Group not find group */
    if (tmpGroup == NULL) {
      err = ERR_ARG;
    }
  }

  return err;
}


/**
 * Process an input MLD message. Called by icmp6_input.
 *
 * @param p the mld packet, p->payload pointing to the icmpv6 header
 * @param inp the netif on which this packet was received
 */
void
mld6_input(struct pbuf *p, struct netif *inp)
{
  struct mld_header *mld_hdr;
  struct mld_group *group;

  MLD6_STATS_INC(mld6.recv);

  /* Check that mld header fits in packet. */
  if (p->len < sizeof(struct mld_header)) {
    /* @todo debug message */
    pbuf_free(p);
    MLD6_STATS_INC(mld6.lenerr);
    MLD6_STATS_INC(mld6.drop);
    return;
  }

  mld_hdr = (struct mld_header *)p->payload;

  /**
      @page RFC-2710 RFC-2710
      @par Compliant Section
      Section 5.  Node State Transition Diagram
      @par Behavior Description
      MLD messages are never sent for multicast addresses whose scope is 0   (reserved) or 1 (node-local).
      MLD messages ARE sent for multicast addresses whose scope is 2  (link-local), including Solicited-Node
      multicast addresses , except for the link-scope, all-nodes address (FF02::1).
      MLD Report will not be sent for  multicast addresses whose scope is 0   (reserved) or 1 (node-local)
      and  link-scope, all-nodes address (FF02::1)  \n

      Send report for the address on the interface.
      The Report message is sent to the address being reported.
      Send done for the address on the interface.
      The Done message is sent to the link-scope all-routers address (FF02::2).
      Send Report to the multicast address which is being queried to
      Send Done to the link-scope all-routers address (FF02::2) \n

      - Report received occurs when the node receives a valid MLD Report message.
      A Report applies only to the address   identified in the Multicast Address field of the Report, on the
      interface from which the Report is received.  It is ignored in the  Non-Listener or Idle Listener state.
      All other events, such as receiving invalid MLD messages or MLD   message types other than Query or Report,
      are ignored in all states.
      Behavior:Stack will ignore the Report received from peer if the destination address is not same as the
      multicast address. \n

      - Query received  occurs when the node receives either a valid General Query message
      or a valid Multicast-Address-Specific Query message. To be valid, the Query message MUST
      come from a link local IPv6 Source Address
      Report received-occurs when the node receives a valid MLD Report message.
      To be valid, the Report message MUST come from a link-local IPv6 Source Address
      Behavior: Stack will validate the source address to be link local for Report and Query message\n
      For Done message there is no stack validation for source address as Done message is handled by
      Router. \n

      */

  /*  <Query received-  occurs when the node receives either a valid General Query message\n
   *   or a valid Multicast-Address-Specific Query message. To be valid, the Query message MUST\n
   *   come from a link local IPv6 Source Address\n
   *   Report recieved-occurs when the node receives a valid MLD Report message.\n
   *   To be valid, the Report message MUST come from a link-local IPv6 Source Address\n
   *   Behavior: Stack will validate the source address to be link local for Report and Query message\n
   *   For Done message there is no stack validation for source address as Done message is handled by\n
   *   Router>
   *
   */
  if (!ip6_addr_islinklocal(ip6_current_src_addr())) {
    (void)pbuf_free(p);
    MLD6_STATS_INC(mld6.proterr);
    MLD6_STATS_INC(mld6.drop);
    return;
  }

#if LWIP_IPV6_MLD_QUERIER
  mld6_querier_input(mld_hdr, inp);
#endif /* LWIP_IPV6_MLD_QUERIER */

  switch (mld_hdr->type) {
  case ICMP6_TYPE_MLQ: /* Multicast listener query. */
    /* Is it a general query? */
    if (ip6_addr_isallnodes_linklocal(ip6_current_dest_addr()) &&
        ip6_addr_isany(&(mld_hdr->multicast_address))) {
      MLD6_STATS_INC(mld6.rx_general);
      /* Report all groups, except all nodes group, and if-local groups. */
      group = netif_mld6_data(inp);
      while (group != NULL) {
        if ((!(ip6_addr_ismulticast_iflocal(&(group->group_address)))) &&
            (!(ip6_addr_isallnodes_linklocal(&(group->group_address)))) &&
            (!(ip6_addr_ismulticast_reserved_scope(&(group->group_address))))) {
          mld6_delayed_report(group, lwip_ntohs(mld_hdr->max_resp_delay));
        }
        group = group->next;
      }
    } else {
      /* Have we joined this group?
       * We use IP6 destination address to have a memory aligned copy.
       * mld_hdr->multicast_address should be the same. */
      /**
          @page RFC-2710 RFC-2710
          @par Compliant Section
          Section 6.  Router State Transition Diagram
          @par Behavior Description
          Send multicast-address-specific query" (Router) for the address on the  link.
          The Multicast-Address-Specific Query is sent to the address being queried
          and has a Maximum Response Delay of [Last Listener Query Interval].\n
          Behavior: Stack will ignore the Multicast specific query from peer if the Query destination address
          is not the multicast address which is being queried>
          */
      /**
          @page RFC-2710 RFC-2710
          @par Compliant Section
          Section 8.  Message Destinations
          @par Behavior Description
          @verbatim
          Message Type                                     IPv6 Destination Address
          ------------                                     ------------------------
          General Query                                    link-scope all-nodes (FF02::1)
          Multicast-Address-Specific Query                 the multicast address being queried
          Report                                           the multicast address being reported
          Done                                             link-scope all-routers (FF02::2)
          @endverbatim
          Behavior: When stack receives Query (General or Specific) stack will validate the destination address
          to be same as the multicast address, if not the message will be ignored.
          When sending out the Report or Done , the destination address is verified and send to the
          multicast address and link-scope all-routers respectively
          */
      if (ip6_addr_cmp_zoneless(ip6_current_dest_addr(), &(mld_hdr->multicast_address))) {
        MLD6_STATS_INC(mld6.rx_group);
        group = mld6_lookfor_group(inp, ip6_current_dest_addr());
        if (group != NULL) {
          /*
           * MLD messages are never sent for multicast addresses whose scope is 0 (reserved) or
           * 1 (node-local).
           * MLD report is not sent for the link-scope, all-nodes address (FF02::1).
           */
          if ((!(ip6_addr_ismulticast_iflocal(&(group->group_address)))) &&
              (!(ip6_addr_isallnodes_linklocal(&(group->group_address)))) &&
              (!(ip6_addr_ismulticast_reserved_scope(&(group->group_address))))) {
            /* Schedule a report. */
            mld6_delayed_report(group, lwip_ntohs(mld_hdr->max_resp_delay));
          }
        }
      }
    }
    break; /* ICMP6_TYPE_MLQ */
  case ICMP6_TYPE_MLR: /* Multicast listener report. */
    /* Have we joined this group?
     * We use IP6 destination address to have a memory aligned copy.
     * mld_hdr->multicast_address should be the same. */
    if (ip6_addr_cmp_zoneless(ip6_current_dest_addr(), &(mld_hdr->multicast_address))) {
      MLD6_STATS_INC(mld6.rx_report);
      group = mld6_lookfor_group(inp, ip6_current_dest_addr());
      if (group != NULL) {
        /* If we are waiting to report, cancel it. */
        if (group->group_state == MLD6_GROUP_DELAYING_MEMBER) {
          group->timer = 0; /* stopped */
          group->group_state = MLD6_GROUP_IDLE_MEMBER;
          group->last_reporter_flag = 0;
        }
      }
    }
    break; /* ICMP6_TYPE_MLR */
  case ICMP6_TYPE_MLD: /* Multicast listener done. */
    /* Do nothing, router will query us. */
    break; /* ICMP6_TYPE_MLD */
  default:
    MLD6_STATS_INC(mld6.proterr);
    MLD6_STATS_INC(mld6.drop);
    break;
  }

  pbuf_free(p);
}

/**
 * @ingroup mld6
 * Join a group on one or all network interfaces.
 *
 * If the group is to be joined on all interfaces, the given group address must
 * not have a zone set (i.e., it must have its zone index set to IP6_NO_ZONE).
 * If the group is to be joined on one particular interface, the given group
 * address may or may not have a zone set.
 *
 * @param srcaddr ipv6 address (zoned) of the network interface which should
 *                join a new group. If IP6_ADDR_ANY6, join on all netifs
 * @param groupaddr the ipv6 address of the group to join (possibly but not
 *                  necessarily zoned)
 * @return ERR_OK if group was joined on the netif(s), an err_t otherwise
 */
err_t
mld6_joingroup(const ip6_addr_t *srcaddr, const ip6_addr_t *groupaddr)
{
  err_t         err = ERR_VAL; /* no matching interface */
  struct netif *netif;

  LWIP_ASSERT_CORE_LOCKED();

  /* loop through netif's */
  NETIF_FOREACH(netif) {
    /* Should we join this interface ? */
    if (ip6_addr_isany(srcaddr) ||
        netif_get_ip6_addr_match(netif, srcaddr) >= 0) {
      err = mld6_joingroup_netif(netif, groupaddr);
      if (err != ERR_OK) {
        return err;
      }
    }
  }

  return err;
}

/**
 * @ingroup mld6
 * Join a group on a network interface.
 *
 * @param netif the network interface which should join a new group.
 * @param groupaddr the ipv6 address of the group to join (possibly but not
 *                  necessarily zoned)
 * @return ERR_OK if group was joined on the netif, an err_t otherwise
 */
err_t
mld6_join_staticgroup_netif(struct netif *netif, const ip6_addr_t *groupaddr)
{
  struct mld_group *group = NULL;

  /* Just find group */
  group = mld6_lookfor_group(netif, groupaddr);

  if (group == NULL) {
    /* Joining a new group. Create a new group entry. */
    group = mld6_new_group(netif, groupaddr);
    if (group == NULL) {
      return ERR_MEM;
    }

#if LWIP_LINK_MCAST_FILTER
    /* Activate this address on the MAC layer. */
    if (netif->mld_mac_filter != NULL) {
      (void)netif->mld_mac_filter(netif, groupaddr, NETIF_ADD_MAC_FILTER);
    }
#endif /* LWIP_LINK_MCAST_FILTER */
  }

  /* Increment group use */
  group->use++;
  return ERR_OK;
}

/**
 * @ingroup mld6
 * Join a group on a network interface.
 *
 * @param netif the network interface which should join a new group.
 * @param groupaddr the ipv6 address of the group to join (possibly but not
 *                  necessarily zoned)
 * @return ERR_OK if group was joined on the netif, an err_t otherwise
 */
err_t
mld6_joingroup_netif(struct netif *netif, const ip6_addr_t *groupaddr)
{
  struct mld_group *group;
#if LWIP_IPV6_SCOPES
  ip6_addr_t ip6addr;

  /* If the address has a particular scope but no zone set, use the netif to
   * set one now. Within the mld6 module, all addresses are properly zoned. */
  if (ip6_addr_lacks_zone(groupaddr, IP6_MULTICAST)) {
    ip6_addr_set(&ip6addr, groupaddr);
    ip6_addr_assign_zone(&ip6addr, IP6_MULTICAST, netif);
    groupaddr = &ip6addr;
  }
  IP6_ADDR_ZONECHECK_NETIF(groupaddr, netif);
#endif /* LWIP_IPV6_SCOPES */

  LWIP_ASSERT_CORE_LOCKED();

  /* find group or create a new one if not found */
  group = mld6_lookfor_group(netif, groupaddr);

  if (group == NULL) {
    /* Joining a new group. Create a new group entry. */
    group = mld6_new_group(netif, groupaddr);
    if (group == NULL) {
      return ERR_MEM;
    }

#if LWIP_LINK_MCAST_FILTER
    /* Activate this address on the MAC layer. */
    if (netif->mld_mac_filter != NULL) {
      (void)netif->mld_mac_filter(netif, groupaddr, NETIF_ADD_MAC_FILTER);
    }
#endif /* LWIP_LINK_MCAST_FILTER */
    /*
     * RFC 2710:MLD messages are never sent for multicast addresses whose scope is 0   (reserved) or 1 (node-local).
     * MLD messages ARE sent for multicast addresses whose scope is 2  (link-local), including Solicited-Node
     * multicast addresses , except for the link-scope, all-nodes address (FF02::1)
     */
    if ((!(ip6_addr_ismulticast_iflocal(&(group->group_address)))) &&
        (!(ip6_addr_isallnodes_linklocal(&(group->group_address)))) &&
        (!(ip6_addr_ismulticast_reserved_scope(&(group->group_address))))) {
      /* Report our membership. */
      MLD6_STATS_INC(mld6.tx_report);
      mld6_send(netif, group, ICMP6_TYPE_MLR);
      mld6_delayed_report(group, MLD6_JOIN_DELAYING_MEMBER_TMR_MS);
#if LWIP_MPL
      LWIP_CONF_MLD6_JOIN_GRP(groupaddr);
#endif
    }
  }

  /* Increment group use */
  group->use++;
  return ERR_OK;
}

/**
 * @ingroup mld6
 * Leave a group on a network interface.
 *
 * Zoning of address follows the same rules as @ref mld6_joingroup.
 *
 * @param srcaddr ipv6 address (zoned) of the network interface which should
 *                leave the group. If IP6_ADDR_ANY6, leave on all netifs
 * @param groupaddr the ipv6 address of the group to leave (possibly, but not
 *                  necessarily zoned)
 * @return ERR_OK if group was left on the netif(s), an err_t otherwise
 */
err_t
mld6_leavegroup(const ip6_addr_t *srcaddr, const ip6_addr_t *groupaddr)
{
  err_t         err = ERR_VAL; /* no matching interface */
  struct netif *netif;

  LWIP_ASSERT_CORE_LOCKED();

  /* loop through netif's */
  NETIF_FOREACH(netif) {
    /* Should we leave this interface ? */
    if (ip6_addr_isany(srcaddr) ||
        netif_get_ip6_addr_match(netif, srcaddr) >= 0) {
      err_t res = mld6_leavegroup_netif(netif, groupaddr);
      if (err != ERR_OK) {
        /* Store this result if we have not yet gotten a success */
        err = res;
      }
    }
  }

  return err;
}

/**
 * @ingroup mld6
 * Leave a group on a network interface.
 *
 * @param netif the network interface which should leave the group.
 * @param groupaddr the ipv6 address of the group to leave (possibly, but not
 *                  necessarily zoned)
 * @return ERR_OK if group was left on the netif, an err_t otherwise
 */
err_t
mld6_leavegroup_netif(struct netif *netif, const ip6_addr_t *groupaddr)
{
  struct mld_group *group;
  u8_t ulast_reporter;
#if LWIP_IPV6_SCOPES
  ip6_addr_t ip6addr;

  if (ip6_addr_lacks_zone(groupaddr, IP6_MULTICAST)) {
    ip6_addr_set(&ip6addr, groupaddr);
    ip6_addr_assign_zone(&ip6addr, IP6_MULTICAST, netif);
    groupaddr = &ip6addr;
  }
  IP6_ADDR_ZONECHECK_NETIF(groupaddr, netif);
#endif /* LWIP_IPV6_SCOPES */

  LWIP_ASSERT_CORE_LOCKED();

  /* find group */
  group = mld6_lookfor_group(netif, groupaddr);

  if (group != NULL) {
    /* Leave if there is no other use of the group */
    if (group->use <= 1) {
      /* Remove the group from the list */
      mld6_remove_group(netif, group);

      /* If we are the last reporter for this group */
      /**
      @page RFC-2710 RFC-2710

      @par Compliant Section
      Section 4.  Protocol Description
      @par Behavior Description
      If the node's most   recent Report message was suppressed by hearing another Report
      message, it MAY send nothing, as it is highly likely that there is   another listener for that
      address still present on the same link.  If   this optimization is implemented, it MUST be able
      to be turned off but SHOULD default to on.\n
      Behavior: Stack will not send Done if the recent Report message was suppressed by hearing another Report
      message, as it is highly likely that there is   another listener. This option is configurable, by default ON.
      */
#if LWIP_MLD6_DONE_ONLYFOR_LAST_REPORTER
      ulast_reporter =  group->last_reporter_flag;
#else
      /*
       * If group is not the last reporter
       * Done message will not be sent. By disabling the configuration, stack will send Done irrespective of the
       * last reporter flag
       */
      ulast_reporter = 1;
#endif

      if (ulast_reporter) {
        /*
         * MLD messages (host side )are never sent for multicast addresses whose scope is 0 (reserved) or
         * 1 (node-local).
         *  MLD messages is not sent for the link-scope, all-nodes address (FF02::1)
         */
        if ((!(ip6_addr_ismulticast_iflocal(&(group->group_address)))) &&
            (!(ip6_addr_isallnodes_linklocal(&(group->group_address)))) &&
            (!(ip6_addr_ismulticast_reserved_scope(&(group->group_address))))) {
          MLD6_STATS_INC(mld6.tx_leave);
          mld6_send(netif, group, ICMP6_TYPE_MLD);
        }
      }

#if LWIP_LINK_MCAST_FILTER
      /* Disable the group at the MAC level */
      if (netif->mld_mac_filter != NULL) {
        (void)netif->mld_mac_filter(netif, groupaddr, NETIF_DEL_MAC_FILTER);
      }
#endif /* LWIP_LINK_MCAST_FILTER */

      /* free group struct */
      memp_free(MEMP_MLD6_GROUP, group);
    } else {
      /* Decrement group use */
      group->use--;
    }

    /* Left group */
    return ERR_OK;
  }

  /* Group not found */
  return ERR_VAL;
}


/**
 * Periodic timer for mld processing. Must be called every
 * MLD6_TMR_INTERVAL milliseconds (100).
 *
 * When a delaying member expires, a membership report is sent.
 */
void
mld6_tmr(void)
{
  struct netif *netif;

  NETIF_FOREACH(netif) {
    struct mld_group *group = netif_mld6_data(netif);

    while (group != NULL) {
      if (group->timer > 0) {
        group->timer--;
        if (group->timer == 0) {
          /* If the state is MLD6_GROUP_DELAYING_MEMBER then we send a report for this group */
          if (group->group_state == MLD6_GROUP_DELAYING_MEMBER) {
            MLD6_STATS_INC(mld6.tx_report);
            mld6_send(netif, group, ICMP6_TYPE_MLR);
            group->group_state = MLD6_GROUP_IDLE_MEMBER;
          }
        }
      }
      group = group->next;
    }
  }
#if LWIP_IPV6_MLD_QUERIER
  mld6_querier_tmr();
#endif /* LWIP_IPV6_MLD_QUERIER */
}

#if LWIP_LOWPOWER
#include "lwip/lowpower.h"

#if LWIP_IPV6_MLD_QUERIER
static u32_t
get_mld6_querier_tmr_listener(struct mld6_listener *listener)
{
  u32_t tick = 0;
  u32_t val;

  if (listener->timer < 1) {
    return 0;
  }
  val = listener->timer;
  SET_TMR_TICK(tick, val);

  /* here timer is greater than 0 */
  if (listener->state != MLD6_LISTENER_STATE_CHECK) {
    return tick;
  }

  /* here state is MLD6_LISTENER_STATE_CHECK */
  if (listener->rexmt_timer > 0) {
    SET_TMR_TICK(tick, listener->rexmt_timer);
    return tick;
  }

  if (listener->query_count < MLD6_LAST_LISTENER_QUERY_COUNT) {
    val = (u32_t)MLD6_LAST_LISTENER_QUERY_COUNT - listener->query_count;
    SET_TMR_TICK(tick, val);
  }
  return tick;
}
#endif /* #if LWIP_IPV6_MLD_QUERIER */

u32_t
mld6_tmr_tick(void)
{
  struct netif *netif = netif_list;
  struct mld6_listener *listener = NULL;
  u32_t val = 0;
  u32_t tick = 0;

  while (netif != NULL) {
    struct mld_group *group = netif_mld6_data(netif);
    while (group != NULL) {
      if (group->timer > 0) {
        SET_TMR_TICK(tick, group->timer);
      }
      group = group->next;
    }

#if LWIP_IPV6_MLD_QUERIER
    struct mld6_querier *querier = netif_mld6_querier_data(netif);
    if (querier == NULL) {
      netif = netif->next;
      continue;
    }
    if (querier->timer > 0) {
      SET_TMR_TICK(tick, querier->timer);
    }

    listener = querier->listeners;
    while (listener != NULL) {
      val = get_mld6_querier_tmr_listener(listener);
      SET_TMR_TICK(tick, val);
      listener = listener->next;
    }
#endif /* LWIP_IPV6_MLD_QUERIER */
    netif = netif->next;
  }

  LOWPOWER_DEBUG(("%s tmr tick: %u\n", __func__, tick));
  return tick;
}
#endif

/**
 * Schedule a delayed membership report for a group
 *
 * @param group the mld_group for which "delaying" membership report
 *              should be sent
 * @param maxresp_in the max resp delay provided in the query
 */
static void
mld6_delayed_report(struct mld_group *group, u16_t maxresp_in)
{
  /* Convert maxresp from milliseconds to tmr ticks */
  u16_t maxresp = maxresp_in / MLD6_TMR_INTERVAL;
  if (maxresp == 0) {
    maxresp = 1;
  }

#ifdef LWIP_RAND
  /* Randomize maxresp. (if LWIP_RAND is supported) */
  maxresp = (u16_t)(LWIP_RAND() % maxresp);
  if (maxresp == 0) {
    maxresp = 1;
  }
#endif /* LWIP_RAND */

  /* Apply timer value if no report has been scheduled already. */
  if ((group->group_state == MLD6_GROUP_IDLE_MEMBER) ||
     ((group->group_state == MLD6_GROUP_DELAYING_MEMBER) &&
      ((group->timer == 0) || (maxresp < group->timer)))) {
    group->timer = maxresp;
    group->group_state = MLD6_GROUP_DELAYING_MEMBER;
  }
}

/**
 * Send a MLD message (report or done).
 *
 * An IPv6 hop-by-hop options header with a router alert option
 * is prepended.
 *
 * @param group the group to report or quit
 * @param type ICMP6_TYPE_MLR (report) or ICMP6_TYPE_MLD (done)
 */
static void
mld6_send(struct netif *netif, struct mld_group *group, u8_t type)
{
  struct mld_header *mld_hdr;
  struct pbuf *p;
  const ip6_addr_t *src_addr;
  ip6_addr_t dst_addr;

  /* Allocate a packet. Size is MLD header + IPv6 Hop-by-hop options header. */
  p = pbuf_alloc(PBUF_IP, sizeof(struct mld_header) + MLD6_HBH_HLEN, PBUF_RAM);
  if (p == NULL) {
    MLD6_STATS_INC(mld6.memerr);
    return;
  }

  /* Move to make room for Hop-by-hop options header. */
  if (pbuf_remove_header(p, MLD6_HBH_HLEN)) {
    pbuf_free(p);
    MLD6_STATS_INC(mld6.lenerr);
    return;
  }

  /* Select our source address. */
  if (!ip6_addr_isvalid(netif_ip6_addr_state(netif, 0))) {
    /* This is a special case, when we are performing duplicate address detection.
     * We must join the multicast group, but we don't have a valid address yet. */
    src_addr = IP6_ADDR_ANY6;
  } else {
    /* Use link-local address as source address. */
    src_addr = netif_ip6_addr(netif, 0);
  }

  /* MLD message header pointer. */
  mld_hdr = (struct mld_header *)p->payload;

  /* Set fields. */
  mld_hdr->type = type;
  mld_hdr->code = 0;
  mld_hdr->chksum = 0;
  mld_hdr->max_resp_delay = 0;
  mld_hdr->reserved = 0;
  ip6_addr_copy_to_packed(mld_hdr->multicast_address, group->group_address);

  /*
   * "send report" for the address on the interface.
   * The Report message is sent to the address being reported.
   * - "send done" for the address on the interface.
   * The Done message is sent to the link-scope all-routers address (FF02::2).
   * Behavior:Send Report to the multicast address which is being queried to
   * Send Done to the link-scope all-routers address (FF02::2)
   */
  if (type == ICMP6_TYPE_MLD) {
    /* Generate the all routers target address to send the done message . */
    ip6_addr_set_allrouters_linklocal(&dst_addr);
  } else {
    ip6_addr_copy(dst_addr, group->group_address);
  }

#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    mld_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len,
                                        src_addr, &dst_addr);
  }
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Add hop-by-hop headers options: router alert with MLD value. */
  ip6_options_add_hbh_ra(p, IP6_NEXTH_ICMP6, IP6_ROUTER_ALERT_VALUE_MLD);

  if (type == ICMP6_TYPE_MLR) {
    /* Remember we were the last to report */
    group->last_reporter_flag = 1;
  }

  /* Send the packet out. */
  MLD6_STATS_INC(mld6.xmit);
  ip6_output_if(p, (ip6_addr_isany(src_addr)) ? NULL : src_addr, &dst_addr,
      MLD6_HL, 0, IP6_NEXTH_HOPBYHOP, netif);
  pbuf_free(p);
}

#if LWIP_IPV6_MLD_QUERIER
static void
mld6_query_send(struct netif *netif, ip6_addr_t *group_address, u16_t max_resp_delay)
{
  struct mld_header *mld_hdr = NULL;
  struct pbuf *p = NULL;
  const ip6_addr_t *src_addr = NULL;
  ip6_addr_t dst_addr = {0};

  /* check link-local address */
  if (!ip6_addr_isvalid(netif_ip6_addr_state(netif, 0))) {
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_query_send: link-local address not ready\n"));
    return;
  }
  src_addr = netif_ip6_addr(netif, 0);

  /* Allocate a packet. Size is MLD header + IPv6 Hop-by-hop options header. */
  p = pbuf_alloc(PBUF_IP, sizeof(struct mld_header) + sizeof(struct ip6_hbh_hdr), PBUF_RAM);
  if (p == NULL) {
    MLD6_STATS_INC(mld6.memerr);
    return;
  }

  /* Move to make room for Hop-by-hop options header. */
  if (pbuf_header(p, -IP6_HBH_HLEN)) {
    (void)pbuf_free(p);
    MLD6_STATS_INC(mld6.lenerr);
    return;
  }

  /* MLD message header pointer. */
  mld_hdr = (struct mld_header *)p->payload;

  /* Set fields. */
  mld_hdr->type = ICMP6_TYPE_MLQ;
  mld_hdr->code = 0;
  mld_hdr->chksum = 0;
  mld_hdr->max_resp_delay = lwip_htons(max_resp_delay);
  mld_hdr->reserved = 0;
  if (group_address != NULL) {
    ip6_addr_copy_to_packed(mld_hdr->multicast_address, *group_address);
    ip6_addr_copy_to_packed(dst_addr, *group_address);
  } else {
    ip6_addr_copy_to_packed(mld_hdr->multicast_address, *IP6_ADDR_ANY6);
    ip6_addr_set_allnodes_linklocal(&dst_addr);
  }

#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    mld_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len,
                                        src_addr, &dst_addr);
  }
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Add hop-by-hop headers options: router alert with MLD value. */
  (void)ip6_options_add_hbh_ra(p, IP6_NEXTH_ICMP6, IP6_ROUTER_ALERT_VALUE_MLD);

  /* Send the packet out. */
  MLD6_STATS_INC(mld6.xmit);
  (void)ip6_output_if(p, src_addr, &dst_addr, MLD6_HL, 0, IP6_NEXTH_HOPBYHOP, netif);
  (void)pbuf_free(p);

  return;
}

static struct mld6_querier *
mld6_querier_get_struct(struct netif *netif)
{
  struct mld6_querier *querier = netif_mld6_querier_data(netif);

  if (querier != NULL) {
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                ("mld6_querier_get_struct: using existing mld6 querier\n"));
    return querier;
  }

  querier = (struct mld6_querier *)mem_malloc(sizeof(struct mld6_querier));
  if (querier == NULL) {
    DHCP6_STATS_INC(mld6.memerr);
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_get_struct: could not allocate mld6 querier\n"));
    return NULL;
  }

  (void)memset_s(querier, sizeof(struct mld6_querier), 0, sizeof(struct mld6_querier));

  netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_MLD6_QUERIER, querier);

  querier->querier_state = MLD6_QUERIER_STATE_STARTUP;
  MLD6_TIMER_SET(querier->timer, ((MLD6_STARTUP_QUERY_INTERVAL) * MLD6_MS_PER_SECOND));
  mld6_query_send(netif, NULL, MLD6_QUERY_RESPONSE_INTERVAL);
  querier->query_count = 0;

  return querier;
}

err_t
mld6_querier_start(struct netif *netif)
{
  struct mld6_querier *querier = NULL;

  LWIP_ERROR("mld6_querier_start: nul netif", (netif != NULL), return ERR_ARG);

  LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("mld6_querier_start(netif=%p) %s%"U16_F"\n", (void *)netif,
                                                             netif->name, (u16_t)netif->num));

  /* check link-local address */
  if (!ip6_addr_isvalid(netif_ip6_addr_state(netif, 0))) {
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_start: link-local address not ready\n"));
    return ERR_VAL;
  }

  querier = mld6_querier_get_struct(netif);
  if (querier == NULL) {
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_start(): get querier struct failed\n"));
    return ERR_MEM;
  }

  LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_start(): querier start success\n"));
  return ERR_OK;
}

static void
mld6_querier_listeners_free(struct mld6_querier *querier)
{
  struct mld6_listener *listener = querier->listeners;
  struct mld6_listener *listener_next = NULL;

  while (listener != NULL) {
    listener_next = listener->next;
    mem_free(listener);
    listener = listener_next;
  }

  querier->listeners = NULL;
  querier->listener_num = 0;

  return;
}

void
mld6_querier_stop(struct netif *netif)
{
  struct mld6_querier *querier = NULL;

  LWIP_ERROR("mld6_querier_stop: nul netif", (netif != NULL), return);

  LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("mld6_querier_stop(netif=%p) %s%"U16_F"\n", (void *)netif,
                                                             netif->name, (u16_t)netif->num));

  querier = netif_mld6_querier_data(netif);
  if (querier == NULL) {
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_stop(): querier not start\n"));
    return;
  }

  netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_MLD6_QUERIER, NULL);
  mld6_querier_listeners_free(querier);
  mem_free(querier);
  return;
}

static void
mld6_querier_startup_done(struct mld6_querier *querier)
{
  if (querier->listeners == NULL) {
    querier->querier_state = MLD6_QUERIER_STATE_NON_LISTENER;
    querier->query_count = 0;
    querier->timer = 0;
  } else {
    querier->querier_state = MLD6_QUERIER_STATE_HAVE_LISTENER;
    querier->query_count = 0;
    MLD6_TIMER_SET(querier->timer, ((MLD6_QUERY_INTERVAL) * MLD6_MS_PER_SECOND));
  }

  return;
}

static void
mld6_querier_tmr_query(struct netif *netif, struct mld6_querier *querier)
{
  if (querier->timer == 0) {
    return;
  }

  querier->timer--;
  if (querier->timer > 0) {
    return;
  }

  /* here timer is 0, send General Query */
  if (querier->querier_state == MLD6_QUERIER_STATE_STARTUP) {
    querier->query_count++;
    if (querier->query_count >= MLD6_STARTUP_QUERY_COUNT) {
      mld6_querier_startup_done(querier);
    } else {
      mld6_query_send(netif, NULL, MLD6_QUERY_RESPONSE_INTERVAL);
      MLD6_TIMER_SET(querier->timer, ((MLD6_STARTUP_QUERY_INTERVAL) * MLD6_MS_PER_SECOND));
    }
  } else if (querier->querier_state == MLD6_QUERIER_STATE_HAVE_LISTENER) {
    mld6_query_send(netif, NULL, MLD6_QUERY_RESPONSE_INTERVAL);
    MLD6_TIMER_SET(querier->timer, ((MLD6_QUERY_INTERVAL) * MLD6_MS_PER_SECOND));
  }

  return;
}

static void
mld6_querier_tmr_listener(struct netif *netif, struct mld6_listener *listener)
{
#ifdef LWIP_DEBUG
  char buf[IP6ADDR_STRLEN_MAX];
#endif
  if (listener->timer <= 1) {
#ifdef LWIP_DEBUG
    (void)ip6addr_ntoa_r(&(listener->group_address), buf, IP6ADDR_STRLEN_MAX);
#endif
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_tmr_listener: del listener %s\n",
                                              buf));
    mld6_querier_listener_del(netif, listener);
    mem_free(listener);
    listener = NULL;
    return;
  }

  listener->timer--;

  /* here timer is greater than 0 */
  if (listener->state != MLD6_LISTENER_STATE_CHECK) {
    return;
  }

  /* here state is MLD6_LISTENER_STATE_CHECK */
  listener->rexmt_timer--;
  if (listener->rexmt_timer > 0) {
    return;
  }
  listener->query_count++;
#ifdef LWIP_DEBUG
  (void)ip6addr_ntoa_r(&(listener->group_address), buf, IP6ADDR_STRLEN_MAX);
#endif
  LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_tmr_listener: check listener %s %hhu\n",
                                            buf, listener->query_count));
  if (listener->query_count < MLD6_LAST_LISTENER_QUERY_COUNT) {
    mld6_query_send(netif, &(listener->group_address), MLD6_LAST_LISTENER_QUERY_INTERVAL);
  }

  return;
}

static void
mld6_querier_tmr_listeners(struct netif *netif, struct mld6_querier *querier)
{
  struct mld6_listener *listener = querier->listeners;
  struct mld6_listener *listener_next = NULL;

  while (listener != NULL) {
    listener_next = listener->next;
    mld6_querier_tmr_listener(netif, listener);
    listener = listener_next;
  }

  return;
}

static void
mld6_querier_tmr(void)
{
  struct netif *netif = netif_list;

  while (netif != NULL) {
    struct mld6_querier *querier = netif_mld6_querier_data(netif);
    if (querier == NULL) {
      netif = netif->next;
      continue;
    }
    mld6_querier_tmr_query(netif, querier);
    mld6_querier_tmr_listeners(netif, querier);
    netif = netif->next;
  }
}

static struct mld6_listener *
mld6_querier_listener_new(ip6_addr_t *addr)
{
  struct mld6_listener *listener = NULL;

  listener = (struct mld6_listener *)mem_malloc(sizeof(struct mld6_listener));
  if (listener == NULL) {
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_listener_new: listener alloc failed\n"));
    return NULL;
  }

  listener->next = NULL;
  ip6_addr_copy_ptr(&(listener->group_address), addr);
  listener->state = MLD6_LISTENER_STATE_PRESENT;
  MLD6_TIMER_SET(listener->timer, (MLD6_LISTENER_INTERVAL * MLD6_MS_PER_SECOND));
  listener->rexmt_timer = 0;
  listener->query_count = 0;

  return listener;
}

static struct mld6_listener *
mld6_querier_listener_find(struct mld6_listener *listeners, ip6_addr_t *addr)
{
  struct mld6_listener *listener = listeners;

  while (listener != NULL) {
    if (ip6_addr_cmp(&(listener->group_address), addr)) {
      break;
    }
    listener = listener->next;
  }

  return listener;
}

struct mld6_listener *
mld6_querier_listener_lookfor(struct netif *netif, const ip6_addr_t *addr)
{
  struct mld6_querier *querier = NULL;
  struct mld6_listener *listener = NULL;

  if ((netif == NULL) || (addr == NULL)) {
    return NULL;
  }
  querier = netif_mld6_querier_data(netif);
  if (querier == NULL) {
    return NULL;
  }
  listener = querier->listeners;
  while (listener != NULL) {
    if (ip6_addr_cmp(&(listener->group_address), addr)) {
      break;
    }
    listener = listener->next;
  }

  return listener;
}

static void
mld6_querier_listener_del(struct netif *netif, struct mld6_listener *listener)
{
  struct mld6_querier *querier = netif_mld6_querier_data(netif);
  struct mld6_listener *listener_p = NULL;

  /* Is it the first listener? */
  if (querier->listeners == listener) {
    querier->listeners = listener->next;
    querier->listener_num--;
    if (querier->listeners == NULL) {
      querier->querier_state = MLD6_QUERIER_STATE_NON_LISTENER;
      querier->query_count = 0;
      querier->timer = 0;
    }
  } else {
    /* look for listener further down the list */
    for (listener_p = querier->listeners; listener_p != NULL; listener_p = listener_p->next) {
      if (listener_p->next == listener) {
        querier->listener_num--;
        listener_p->next = listener->next;
        break;
      }
    }
  }

  return;
}

static void
mld6_querier_handle_report(struct mld_header *mld_hdr, struct netif *netif)
{
  struct mld6_querier *querier = netif_mld6_querier_data(netif);
  struct mld6_listener *listener = NULL;
  ip6_addr_t group_address;

  if (querier == NULL) {
    MLD6_STATS_INC(mld6.drop);
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_handle_report: querier not start\n"));
    return;
  }

  if ((ip6_addr_isany_val(mld_hdr->multicast_address)) ||
      !(ip6_addr_cmp_zoneless(ip6_current_dest_addr(), &(mld_hdr->multicast_address)))) {
    MLD6_STATS_INC(mld6.drop);
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_handle_report: invalid report\n"));
    return;
  }

  ip6_addr_copy_from_packed(group_address, mld_hdr->multicast_address);
#if LWIP_MPL
  if (ip6_addr_multicast_scope(&group_address) <= IP6_MULTICAST_SCOPE_LINK_LOCAL) {
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_handle_report: not fwd scope\n"));
    return;
  }
#endif /* LWIP_MPL */
  listener = mld6_querier_listener_find(querier->listeners,  &(group_address));
  if (listener == NULL) {
    LWIP_ERROR("mld6_querier_handle_report: LISTENER_MAX_NUM", (querier->listener_num < MLD6_QUERIER_LISTENER_MAX_NUM),
               return);
    listener = mld6_querier_listener_new(&(group_address));
    if (listener == NULL) {
      LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_handle_report: new listener failed\n"));
      return;
    }
    if ((querier->listeners == NULL) && (querier->querier_state == MLD6_QUERIER_STATE_NON_LISTENER)) {
      querier->querier_state = MLD6_QUERIER_STATE_HAVE_LISTENER;
      MLD6_TIMER_SET(querier->timer, ((MLD6_QUERY_INTERVAL) * MLD6_MS_PER_SECOND));
    }
    listener->next = querier->listeners;
    querier->listeners = listener;
    querier->listener_num++;
#if LWIP_MPL
    LWIP_CONF_MLD6_JOIN_GRP((const ip6_addr_t *)(&group_address));
#endif
  } else {
    listener->state = MLD6_LISTENER_STATE_PRESENT;
    MLD6_TIMER_SET(listener->timer, (MLD6_LISTENER_INTERVAL * MLD6_MS_PER_SECOND));
    listener->rexmt_timer = 0;
    listener->query_count = 0;
  }

  return;
}

static void
mld6_querier_handle_done(struct mld_header *mld_hdr, struct netif *netif)
{
  struct mld6_querier *querier = netif_mld6_querier_data(netif);
  struct mld6_listener *listener = NULL;
  u16_t new_timer;
  ip6_addr_t group_address;

  if ((querier == NULL) || (querier->listeners == NULL)) {
    MLD6_STATS_INC(mld6.drop);
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_handle_done: querier not start\n"));
    return;
  }

  if ((ip6_addr_isany_val(mld_hdr->multicast_address)) ||
      !(ip6_addr_isallrouters_linklocal(ip6_current_dest_addr()))) {
    MLD6_STATS_INC(mld6.drop);
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_handle_done: invalid done\n"));
    return;
  }

  ip6_addr_copy_from_packed(group_address, mld_hdr->multicast_address);
  listener = mld6_querier_listener_find(querier->listeners,  &(group_address));
  if (listener == NULL) {
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_handle_done: no such listener\n"));
    return;
  }
  if (listener->state == MLD6_LISTENER_STATE_CHECK) {
    LWIP_DEBUGF(MLD6_DEBUG | LWIP_DBG_TRACE, ("mld6_querier_handle_done: checking listener\n"));
    return;
  }

  listener->state = MLD6_LISTENER_STATE_CHECK;
  /* Multicast-Address-Specific Query */
  mld6_query_send(netif, &(listener->group_address), MLD6_LAST_LISTENER_QUERY_INTERVAL);
  /*
   * [RFC 2710] Section 6
   * sets the timer to the minimum of its current value and either
   * [Last Listener Query Interval] * [Last Listener Query Count] if
   * this router is a Querier
   */
  MLD6_TIMER_SET(new_timer, MLD6_LAST_LISTENER_QUERY_INTERVAL * MLD6_LAST_LISTENER_QUERY_COUNT);
  listener->timer = LWIP_MIN(listener->timer, new_timer);
  /*
   * [RFC 2710] Section 6
   * "start retransmit timer" for the address on the link [Last Listener
   * Query Interval]
   */
  MLD6_TIMER_SET(listener->rexmt_timer, MLD6_LAST_LISTENER_QUERY_INTERVAL);
  listener->query_count = 0;

  return;
}

static void
mld6_querier_input(struct mld_header *mld_hdr, struct netif *netif)
{
  /* here we not support Non-Querier, so not handle ICMP6_TYPE_MLQ */
  if (mld_hdr->type == ICMP6_TYPE_MLR) {
    mld6_querier_handle_report(mld_hdr, netif);
  } else if (mld_hdr->type == ICMP6_TYPE_MLD) {
    mld6_querier_handle_done(mld_hdr, netif);
  }

  return;
}
#endif /* LWIP_IPV6_MLD_QUERIER */

#endif /* LWIP_IPV6 && (LWIP_IPV6_MLD || LWIP_IPV6_MLD_QUERIER) */
