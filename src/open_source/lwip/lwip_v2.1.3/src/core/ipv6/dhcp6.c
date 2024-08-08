/**
 * @file
 *
 * @defgroup dhcp6 DHCPv6
 * @ingroup ip6
 * DHCPv6 client: IPv6 address autoconfiguration as per
 * RFC 3315 (stateful DHCPv6) and
 * RFC 3736 (stateless DHCPv6).
 *
 * For now, only stateless DHCPv6 and stateful DHCPv6 client is implemented!
 *
 * dhcp6_enable_stateful() enables stateful DHCPv6 for a netif (stateless disabled)\n
 * dhcp6_enable_stateless() enables stateless DHCPv6 for a netif (stateful disabled)\n
 * dhcp6_disable() disable DHCPv6 for a netif
 *
 * When enabled, requests are only issued after receipt of RA with the
 * corresponding bits set.
 * When stateful DHCPv6 enabled, requests are sent after state of linklocal address is
 * IP6_ADDR_PREFERRED, not depend on receipt of an RA message with the
 * ND6_RA_FLAG_MANAGED_ADDR_CONFIG flag set.
 */

/*
 * Copyright (c) 2018 Simon Goldschmidt
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
 * Author: Simon Goldschmidt <goldsimon@gmx.de>
 */

#include "lwip/opt.h"

#if LWIP_IPV6 && LWIP_IPV6_DHCP6 /* don't build if not configured for use in lwipopts.h */

#include "lwip/dhcp6.h"
#include "lwip/prot/dhcp6.h"
#include "lwip/def.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/sys.h"

#include <string.h>
static inline void dhcp6_put_ushort(unsigned char *obuf, u16_t val);
#if LWIP_IPV6_DHCP6_STATEFUL
static void dhcp6_stateful_struct_free(struct dhcp6 *dhcp6_p);
static void dhcp6_linklocal_address_check(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static err_t dhcp6_form_duid(const struct netif *netif_p, struct dhcp6 *dhcp6_p, u8_t type);
static err_t dhcp6_create_iaid(const struct netif *netif_p, struct dhcp6 *dhcp6_p, u8_t idx);
static err_t dhcp6_stateful_init(const struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_rebind_failure_cleanup(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_post_release_cleanup(struct dhcp6 *dhcp6_p);
static void dhcp6_set_timeout_params(struct dhcp6 *dhcp6_p);
static void dhcp6_stateful_timeout(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_solicit_to_request(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_solicit(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_request(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_request_next_server(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_renew(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_rebind(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_release(struct netif *netif_p, struct dhcp6 *dhcp6_p);
static void dhcp6_decline(struct netif *netif_p, struct dhcp6 *dhcp6_p);
#endif /* LWIP_IPV6_DHCP6_STATEFUL */

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif
#ifndef LWIP_HOOK_DHCP6_APPEND_OPTIONS
#define LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, state, msg, msg_type, options_len_ptr, max_len)
#endif
#ifndef LWIP_HOOK_DHCP6_PARSE_OPTION
#define LWIP_HOOK_DHCP6_PARSE_OPTION(netif, dhcp6, state, msg, msg_type, option, len, pbuf, offset) do { LWIP_UNUSED_ARG(msg); } while(0)
#endif

#if LWIP_DNS && LWIP_DHCP6_MAX_DNS_SERVERS
#if DNS_MAX_SERVERS > LWIP_DHCP6_MAX_DNS_SERVERS
#define LWIP_DHCP6_PROVIDE_DNS_SERVERS LWIP_DHCP6_MAX_DNS_SERVERS
#else
#define LWIP_DHCP6_PROVIDE_DNS_SERVERS DNS_MAX_SERVERS
#endif
#else
#define LWIP_DHCP6_PROVIDE_DNS_SERVERS 0
#endif


/** Option handling: options are parsed in dhcp6_parse_reply
 * and saved in an array where other functions can load them from.
 * This might be moved into the struct dhcp6 (not necessarily since
 * lwIP is single-threaded and the array is only used while in recv
 * callback). */
enum dhcp6_option_idx {
  DHCP6_OPTION_IDX_CLI_ID = 0,
  DHCP6_OPTION_IDX_SERVER_ID,
#if LWIP_IPV6_DHCP6_STATEFUL
  DHCP6_OPTION_IDX_IA_NA,
  DHCP6_OPTION_IDX_PREFERENCE,
#if DHCP6_ENABLE_UNICAST_SUPPORT
  DHCP6_OPTION_IDX_UNICAST,
#endif /* DHCP6_ENABLE_UNICAST_SUPPORT */
  DHCP6_OPTION_IDX_STATUS_CODE,
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
  DHCP6_OPTION_IDX_DNS_SERVER,
  DHCP6_OPTION_IDX_DOMAIN_LIST,
#endif /* LWIP_DHCP_PROVIDE_DNS_SERVERS */
#if LWIP_DHCP6_GET_NTP_SRV
  DHCP6_OPTION_IDX_NTP_SERVER,
#endif /* LWIP_DHCP_GET_NTP_SRV */
  DHCP6_OPTION_IDX_MAX
};

struct dhcp6_option_info {
  u8_t option_given;
  u16_t val_start;
  u16_t val_length;
};

#if LWIP_IPV6_DHCP6_STATEFUL
#ifdef LWIP_DEBUG
static const char *dhcp6_stateful_message_string[] = {
  "SOLICIT",              /* 1 */
  "ADVERTISE",            /* 2 */
  "REQUEST",              /* 3 */
  "CONFIRM",              /* 4 */
  "RENEW",                /* 5 */
  "REBIND",               /* 6 */
  "REPLY",                /* 7 */
  "RELEASE",              /* 8 */
  "DECLINE",              /* 9 */
  "RECONFIGURE",          /* 10 */
  "INFOREQUEST",          /* 11 */
  "RELAYFORW",            /* 12 */
  "RELAYREPL"             /* 13 */
};
#define DHCP6_STATEFUL_MESSAGE_TYPE_TO_STRING(message_type) \
  (((message_type) > 0 && (message_type) < DHCP6_MESSAGE_END) ? \
  dhcp6_stateful_message_string[(message_type)-1] : "UNKNOWN MESSAGE")
#endif
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
/** Holds the decoded option info, only valid while in dhcp6_recv. */
struct dhcp6_option_info dhcp6_rx_options[DHCP6_OPTION_IDX_MAX];

#define dhcp6_option_given(dhcp6, idx)           (dhcp6_rx_options[idx].option_given != 0)
#define dhcp6_got_option(dhcp6, idx)             (dhcp6_rx_options[idx].option_given = 1)
#define dhcp6_clear_option(dhcp6, idx)           (dhcp6_rx_options[idx].option_given = 0)
#define dhcp6_clear_all_options(dhcp6)           (memset(dhcp6_rx_options, 0, sizeof(dhcp6_rx_options)))
#define dhcp6_get_option_start(dhcp6, idx)       (dhcp6_rx_options[idx].val_start)
#define dhcp6_get_option_length(dhcp6, idx)      (dhcp6_rx_options[idx].val_length)
#define dhcp6_set_option(dhcp6, idx, start, len) do { dhcp6_rx_options[idx].val_start = (start); dhcp6_rx_options[idx].val_length = (len); }while(0)


const ip_addr_t dhcp6_All_DHCP6_Relay_Agents_and_Servers = IPADDR6_INIT_HOST(0xFF020000, 0, 0, 0x00010002);
const ip_addr_t dhcp6_All_DHCP6_Servers = IPADDR6_INIT_HOST(0xFF050000, 0, 0, 0x00010003);

/* receive, unfold, parse and free incoming messages */
static void dhcp6_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

static void
dhcp6_pcb_del(struct dhcp6 *dhcp6_p)
{
  udp_remove(dhcp6_p->pcb);
  dhcp6_p->pcb = NULL;

  return;
}

static err_t
dhcp6_pcb_new(struct dhcp6 *dhcp6_p)
{
  err_t result;

   /* allocate UDP PCB */
  dhcp6_p->pcb = udp_new_ip6();
  if (dhcp6_p->pcb == NULL) {
    DHCP6_STATS_INC(dhcp6.memerr);
    return ERR_MEM;
  }

#if LWIP_SO_PRIORITY
  dhcp6_p->pcb->priority = LWIP_PKT_PRIORITY_DHCP6;
#endif /* LWIP_SO_PRIORITY */

  ip_set_option(dhcp6_p->pcb, SOF_BROADCAST);

  result = udp_bind(dhcp6_p->pcb, IP6_ADDR_ANY, DHCP6_CLIENT_PORT);
  if (result != ERR_OK) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6 bind failed"));
    dhcp6_pcb_del(dhcp6_p);
    return result;
  }

  result = udp_connect(dhcp6_p->pcb, IP6_ADDR_ANY, DHCP6_SERVER_PORT);
  if (result != ERR_OK) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6 connect failed"));
    dhcp6_pcb_del(dhcp6_p);
    return result;
  }

  udp_recv(dhcp6_p->pcb, dhcp6_recv, NULL);

  return ERR_OK;
}

/**
 * @ingroup dhcp6
 * Set a statically allocated struct dhcp6 to work with.
 * Using this prevents dhcp6_start to allocate it using mem_malloc.
 *
 * @param netif the netif for which to set the struct dhcp
 * @param dhcp6 (uninitialised) dhcp6 struct allocated by the application
 */
void
dhcp6_set_struct(struct netif *netif, struct dhcp6 *dhcp6)
{
  LWIP_ASSERT("netif != NULL", netif != NULL);
  LWIP_ASSERT("dhcp6 != NULL", dhcp6 != NULL);
  LWIP_ASSERT("netif already has a struct dhcp6 set", netif_dhcp6_data(netif) == NULL);

  /* clear data structure */
  memset(dhcp6, 0, sizeof(struct dhcp6));
  /* dhcp6_set_state(&dhcp, DHCP6_STATE_OFF); */
  netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, dhcp6);
}

/**
 * @ingroup dhcp6
 * Removes a struct dhcp6 from a netif.
 *
 * ATTENTION: Only use this when not using dhcp6_set_struct() to allocate the
 *            struct dhcp6 since the memory is passed back to the heap.
 *
 * @param netif the netif from which to remove the struct dhcp
 */
void dhcp6_cleanup(struct netif *netif)
{
  LWIP_ERROR("invalid netif", netif != NULL, return);
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  if (dhcp6 != NULL) {
#if LWIP_IPV6_DHCP6_STATEFUL
    dhcp6_stateful_struct_free(dhcp6);
#else
    LWIP_UNUSED_ARG(dhcp6);
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
    mem_free(dhcp6);
    netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, NULL);
  }
}

static struct dhcp6*
dhcp6_get_struct(struct netif *netif, const char *dbg_requester)
{
#ifndef LWIP_DEBUG
  LWIP_UNUSED_ARG(dbg_requester);
#endif
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
  if (dhcp6 == NULL) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("%s: mallocing new DHCPv6 client\n", dbg_requester));
    dhcp6 = (struct dhcp6 *)mem_malloc(sizeof(struct dhcp6));
    if (dhcp6 == NULL) {
      DHCP6_STATS_INC(dhcp6.memerr);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("%s: could not allocate dhcp6\n", dbg_requester));
      return NULL;
    }

    /* clear data structure, this implies DHCP6_STATE_OFF */
    memset(dhcp6, 0, sizeof(struct dhcp6));
    /* store this dhcp6 client in the netif */
    netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, dhcp6);
  } else {
    /* already has DHCP6 client attached */
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("%s: using existing DHCPv6 client\n", dbg_requester));
  }

  if (dhcp6->pcb == NULL) {
    if (dhcp6_pcb_new(dhcp6) != ERR_OK) {
      mem_free(dhcp6);
      netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, NULL);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("%s: allocated dhcp6 pcb failed\n", dbg_requester));
      return NULL;
    }
    /* bind dhcp udp_pcb to specific netif, this could make dhcp client start on multiple netif */
    dhcp6->pcb->netif_idx = netif->ifindex;
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("%s: allocated dhcp6 pcb", dbg_requester));
  }
  return dhcp6;
}

/*
 * Set the DHCPv6 state
 * If the state changed, reset the number of tries.
 */
static void
dhcp6_set_state(struct dhcp6 *dhcp6, u8_t new_state, const char *dbg_caller)
{
#ifndef LWIP_DEBUG
    LWIP_UNUSED_ARG(dbg_caller);
#endif
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("DHCPv6 state: %d -> %d (%s)\n",
    dhcp6->state, new_state, dbg_caller));
  if (new_state != dhcp6->state) {
    dhcp6->state = new_state;
    dhcp6->tries = 0;
    dhcp6->request_timeout = 0;
  }
}

static int
dhcp6_stateless_enabled(struct dhcp6 *dhcp6)
{
  if ((dhcp6->state == DHCP6_STATE_STATELESS_IDLE) ||
      (dhcp6->state == DHCP6_STATE_REQUESTING_CONFIG)) {
    return 1;
  }
  return 0;
}

int
dhcp6_stateful_enabled(struct dhcp6 *dhcp6)
{
  if (dhcp6->state == DHCP6_STATE_OFF) {
    return 0;
  }
  if (dhcp6_stateless_enabled(dhcp6)) {
    return 0;
  }
  return 1;
}

/**
 * @ingroup dhcp6
 * Enable stateful DHCPv6 on this netif
 * Requests are sent on receipt of an RA message with the
 * ND6_RA_FLAG_MANAGED_ADDR_CONFIG flag set.
 *
 * A struct dhcp6 will be allocated for this netif if not
 * set via @ref dhcp6_set_struct before.
 *
 */
err_t
dhcp6_enable_stateful(struct netif *netif)
{
  struct dhcp6 *dhcp6 = NULL;
  s8_t addr_idx = LWIP_IPV6_NUM_ADDRESSES;
  s8_t j;

  LWIP_UNUSED_ARG(netif);
#if LWIP_IPV6_DHCP6_STATEFUL
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_enable_stateful(netif=%p) %s%"U16_F"\n",
    (void *)netif, netif->name, (u16_t)netif->num));

  if (!ip6_addr_ispreferred(netif_ip6_addr_state(netif, 0))) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateful(): linklocal address is invalid"));
    return ERR_VAL;
  }
  for (j = 1; j < LWIP_IPV6_NUM_ADDRESSES; j++) {
    if ((ip6_addr_isinvalid(netif_ip6_addr_state(netif, j))) ||
        (ip6_addr_isduplicated(netif_ip6_addr_state(netif, j)))) {
      addr_idx = j;
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("ip6 addr addr_idx= %u is free\n", addr_idx));
      break;
    }
  }
  if (addr_idx == LWIP_IPV6_NUM_ADDRESSES) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("IA_NA addr assign failed, the netif IP address space is not enough\n"));
    return ERR_NOADDR;
  }

  dhcp6 = dhcp6_get_struct(netif, "dhcp6_enable_stateful()");
  if (dhcp6 == NULL) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateful(): get dhcp6 struct failed"));
    return ERR_MEM;
  }
  if (dhcp6_stateful_enabled(dhcp6)) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateful(): stateful DHCPv6 already enabled"));
    return ERR_OK;
  } else if (dhcp6->state != DHCP6_STATE_OFF) {
    /* stateless running */
#if LWIP_IPV6_AUTOCONFIG
    netif_set_ip6_autoconfig_disabled(netif);
#endif /* LWIP_IPV6_AUTOCONFIG */
    /* not handle message from stateless server */
    dhcp6->xid = 0;
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateful(): switching from stateless to stateful DHCPv6"));
  }

  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateful(): stateful DHCPv6 enabled\n"));
  if (dhcp6->struct_init == 0) {
    (void)dhcp6_stateful_init(netif, dhcp6);
  }
  dhcp6->addr_idx = addr_idx;
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_INIT, "dhcp6_enable_stateful");
  dhcp6_linklocal_address_check(netif, dhcp6);
  return ERR_OK;
#else
  LWIP_UNUSED_ARG(j);
  LWIP_UNUSED_ARG(addr_idx);
  LWIP_UNUSED_ARG(dhcp6);
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateful(): not support stateful DHCPv6"));
  return ERR_VAL;
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
}

/**
 * @ingroup dhcp6
 * Enable stateless DHCPv6 on this netif
 * Requests are sent on receipt of an RA message with the
 * ND6_RA_FLAG_OTHER_CONFIG flag set.
 *
 * A struct dhcp6 will be allocated for this netif if not
 * set via @ref dhcp6_set_struct before.
 */
err_t
dhcp6_enable_stateless(struct netif *netif)
{
  struct dhcp6 *dhcp6;

  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
              ("dhcp6_enable_stateless netif= %s%"U16_F"\n", netif->name, (u16_t)netif->num));

  dhcp6 = dhcp6_get_struct(netif, "dhcp6_enable_stateless()");
  if (dhcp6 == NULL) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateless(): get dhcp6 struct failed"));
    return ERR_MEM;
  }
  if (dhcp6_stateless_enabled(dhcp6)) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateless(): stateless DHCPv6 already enabled"));
    return ERR_OK;
  } else if (dhcp6->state != DHCP6_STATE_OFF) {
    /* stateful running */
#if LWIP_IPV6_DHCP6_STATEFUL
    (void)dhcp6_release_stateful(netif);
    dhcp6_post_release_cleanup(dhcp6);
    /* won't handle the REPLY of this RELEASE when timeout */
    dhcp6->request_timeout = 0;
    /* not handle message from stateful server */
    dhcp6->xid = 0;
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateless(): switching from stateful to stateless DHCPv6"));
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
  }
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateless(): stateless DHCPv6 enabled\n"));
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATELESS_IDLE, "dhcp6_enable_stateless");
  return ERR_OK;
}

/**
 * @ingroup dhcp6
 * Disable stateful or stateless DHCPv6 on this netif
 * Requests are sent on receipt of an RA message with the
 * ND6_RA_FLAG_OTHER_CONFIG flag set.
 */
void
dhcp6_disable(struct netif *netif)
{
  struct dhcp6 *dhcp6;

  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
              ("dhcp6_disable(netif=%p) %s%"U16_F"\n", (void *)netif, netif->name, (u16_t)netif->num));

  dhcp6 = netif_dhcp6_data(netif);
  if (dhcp6 != NULL) {
    if (dhcp6->state != DHCP6_STATE_OFF) {
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_disable(): DHCPv6 disabled (old state: %s)\n",
        (dhcp6_stateless_enabled(dhcp6) ? "stateless" : "stateful")));
#if LWIP_IPV6_DHCP6_STATEFUL
      /* try to send RELEASE if needed */
      (void)dhcp6_release_stateful(netif);
      /* the REPLY of this RELEASE won't be handled */
      dhcp6_post_release_cleanup(dhcp6);
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
      /* not handle message from server if enabled immediately */
      dhcp6->xid = 0;
      dhcp6_set_state(dhcp6, DHCP6_STATE_OFF, "dhcp6_disable");
    }
    if (dhcp6->pcb != NULL) {
      dhcp6_pcb_del(dhcp6);
    }
  }
}

/*
 * @ingroup dhcp6
 * Stateful DHCPv6 release on this netif
 */
err_t
dhcp6_release_stateful(struct netif *netif)
{
  struct dhcp6 *dhcp6 = NULL;

  LWIP_UNUSED_ARG(netif);
#if LWIP_IPV6_DHCP6_STATEFUL
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_release_stateful(netif=%p) %s%"U16_F"\n",
              (void *)netif, netif->name, (u16_t)netif->num));

  dhcp6 = netif_dhcp6_data(netif);
  if (dhcp6 == NULL) {
    return ERR_VAL;
  }
  if (dhcp6->state == DHCP6_STATE_STATEFUL_RENEW ||
      dhcp6->state == DHCP6_STATE_STATEFUL_REBIND ||
      dhcp6->state == DHCP6_STATE_STATEFUL_IDLE) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_release_stateful(): have address to be released"));
    /* [RFC3315 Section 18.1.6]: The client MUST stop using all of the addresses being released as
       soon as the client begins the Release message exchange process */
    if (dhcp6->addr_idx != LWIP_IPV6_NUM_ADDRESSES) {
      netif_ip6_addr_set_state(netif, dhcp6->addr_idx, IP6_ADDR_INVALID);
      netif_ip6_addr_set_parts(netif, dhcp6->addr_idx, 0, 0, 0, 0);
    }

    dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_RELEASE, "dhcp6_release_stateful");
    dhcp6_set_timeout_params(dhcp6);
    dhcp6_release(netif, dhcp6);
    return ERR_OK;
  } else if (dhcp6->state == DHCP6_STATE_STATEFUL_RELEASE) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_release_stateful(): address has been released"));
    return ERR_OK;
  } else {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_release_stateful(): no address to be released"));
    return ERR_VAL;
  }
#else
  LWIP_UNUSED_ARG(dhcp6);
  return ERR_VAL;
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
}

/*
 * Create a DHCPv6 request, fill in common headers
 *
 * @param netif the netif under DHCPv6 control
 * @param dhcp6 dhcp6 control struct
 * @param message_type message type of the request
 * @param opt_len_alloc option length to allocate
 * @param options_out_len option length on exit
 * @return a pbuf for the message
 */
static struct pbuf *
dhcp6_create_msg(struct netif *netif, struct dhcp6 *dhcp6, u8_t message_type,
                 u16_t opt_len_alloc, u16_t *options_out_len)
{
  struct pbuf *p_out;
  struct dhcp6_msg *msg_out;

  LWIP_ERROR("dhcp6_create_msg: netif != NULL", (netif != NULL), return NULL;);
  LWIP_ERROR("dhcp6_create_msg: dhcp6 != NULL", (dhcp6 != NULL), return NULL;);
  LWIP_ERROR("dhcp6_create_msg: opt_len_alloc to large", (opt_len_alloc <= (0xffff - sizeof(struct dhcp6_msg))),
             return NULL);
  p_out = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcp6_msg) + opt_len_alloc, PBUF_RAM);
  if (p_out == NULL) {
    DHCP6_STATS_INC(dhcp6.memerr);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("dhcp6_create_msg(): could not allocate pbuf\n"));
    return NULL;
  }
  LWIP_ASSERT("dhcp6_create_msg: check that first pbuf can hold struct dhcp6_msg",
              (p_out->len >= sizeof(struct dhcp6_msg) + opt_len_alloc));

  /* @todo: limit new xid for certain message types? */
  /* reuse transaction identifier in retransmissions */
  if (dhcp6->tries == 0) {
#ifdef LWIP_RAND
    dhcp6->xid = LWIP_RAND() & 0xFFFFFF;
#else
    dhcp6->xid = (dhcp6->xid + 1) & 0xFFFFFF;
#endif
  }

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE,
              ("transaction id xid(%"X32_F")\n", dhcp6->xid));

  msg_out = (struct dhcp6_msg *)p_out->payload;
  memset(msg_out, 0, sizeof(struct dhcp6_msg) + opt_len_alloc);

  msg_out->msgtype = message_type;
  msg_out->transaction_id[0] = (u8_t)(dhcp6->xid >> 16);
  msg_out->transaction_id[1] = (u8_t)(dhcp6->xid >> 8);
  msg_out->transaction_id[2] = (u8_t)dhcp6->xid;
  *options_out_len = 0;
  return p_out;
}

static u16_t
dhcp6_option_short(u16_t options_out_len, u8_t *options, u16_t value)
{
  dhcp6_put_ushort(options + options_out_len, value);
  options_out_len = (u16_t)(options_out_len + sizeof(u16_t));
  return options_out_len;
}

static u16_t
dhcp6_option_optionrequest(u16_t options_out_len, u8_t *options, const u16_t *req_options,
                           u16_t num_req_options, u16_t max_len)
{
  size_t i;
  u16_t ret;

  LWIP_ASSERT("dhcp6_option_optionrequest: options_out_len + sizeof(struct dhcp6_msg) + addlen <= max_len",
    sizeof(struct dhcp6_msg) + options_out_len + 4U + (2U * num_req_options) <= max_len);
  LWIP_UNUSED_ARG(max_len);

  ret = dhcp6_option_short(options_out_len, options, DHCP6_OPTION_ORO);
  ret = dhcp6_option_short(ret, options, 2 * num_req_options);
  for (i = 0; i < num_req_options; i++) {
    ret = dhcp6_option_short(ret, options, req_options[i]);
  }
  return ret;
}

/* All options are added, shrink the pbuf to the required size */
static void
dhcp6_msg_finalize(u16_t options_out_len, struct pbuf *p_out)
{
  /* shrink the pbuf to the actual content length */
  pbuf_realloc(p_out, (u16_t)(sizeof(struct dhcp6_msg) + options_out_len));
}

#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
static err_t
dhcp6_set_dns_servers(const struct netif *netif, const struct dhcp6 *dhcp6_p, const struct pbuf *p_msg_in)
{
  ip_addr_t dns_addr;
  ip6_addr_t *dns_addr6 = NULL;
  u16_t op_start, op_len;
  u16_t idx;
  u8_t n;

  LWIP_UNUSED_ARG(dhcp6_p);

  op_start = dhcp6_get_option_start(dhcp6_p, DHCP6_OPTION_IDX_DNS_SERVER);
  op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_DNS_SERVER);
  ip_addr_set_zero_ip6(&dns_addr);
  dns_addr6 = ip_2_ip6(&dns_addr);
  for (n = 0, idx = op_start; (idx < op_start + op_len) && (n < LWIP_DHCP6_PROVIDE_DNS_SERVERS);
       n++, idx = (u16_t)(idx + sizeof(struct ip6_addr_packed))) {
    u16_t copied = pbuf_copy_partial(p_msg_in, dns_addr6, sizeof(struct ip6_addr_packed), idx);
    if (copied != sizeof(struct ip6_addr_packed)) {
      /* pbuf length mismatch */
      return ERR_VAL;
    }
    ip6_addr_assign_zone(dns_addr6, IP6_UNKNOWN, netif);
    /* @todo: do we need a different offset than DHCP(v4)? */
    dns_setserver(n, &dns_addr);
  }

  return ERR_OK;
}
#endif /* LWIP_DHCP6_PROVIDE_DNS_SERVERS */

#if LWIP_DHCP6_GET_NTP_SRV
static err_t
dhcp6_set_ntp_srv(const struct netif *netif, const struct dhcp6 *dhcp6_p, const struct pbuf *p_msg_in)
{
  ip_addr_t ntp_server_addrs[LWIP_DHCP6_MAX_NTP_SERVERS];
  u16_t idx;
  u16_t copied;
  u8_t n;

  LWIP_UNUSED_ARG(dhcp6_p);

  u16_t op_start = dhcp6_get_option_start(dhcp6_p, DHCP6_OPTION_IDX_NTP_SERVER);
  u16_t op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_NTP_SERVER);

  for (n = 0, idx = op_start; (idx < op_start + op_len) && (n < LWIP_DHCP6_MAX_NTP_SERVERS);
       n++, idx += sizeof(struct ip6_addr_packed)) {
    ip6_addr_t *ntp_addr6 = ip_2_ip6(&ntp_server_addrs[n]);
    ip_addr_set_zero_ip6(&ntp_server_addrs[n]);
    copied = pbuf_copy_partial(p_msg_in, ntp_addr6, sizeof(struct ip6_addr_packed), idx);
    if (copied != sizeof(struct ip6_addr_packed)) {
      /* pbuf length mismatch */
      return ERR_VAL;
    }
    ip6_addr_assign_zone(ntp_addr6, IP6_UNKNOWN, netif);
  }
  dhcp6_set_ntp_servers(n, ntp_server_addrs);

  return ERR_OK;
}
#endif /* LWIP_DHCP6_GET_NTP_SRV */

#if LWIP_IPV6_DHCP6_STATELESS
static void
dhcp6_information_request(struct netif *netif, struct dhcp6 *dhcp6)
{
  const u16_t requested_options[] = {
#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
    DHCP6_OPTION_DNS_SERVERS, 
    DHCP6_OPTION_DOMAIN_LIST
#endif
#if LWIP_DHCP6_GET_NTP_SRV
    , DHCP6_OPTION_SNTP_SERVERS
#endif
  };
  
  u16_t msecs;
  struct pbuf *p_out;
  u16_t options_out_len;
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_information_request()\n"));
  /* create and initialize the DHCP message header */
  p_out = dhcp6_create_msg(netif, dhcp6, DHCP6_INFOREQUEST,
                           DHCP6_OPT_HEADER_LEN + sizeof(requested_options), &options_out_len);
  if (p_out != NULL) {
    err_t err;
    struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
    u8_t *options = (u8_t *)(msg_out + 1);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_information_request: making request\n"));

    options_out_len = dhcp6_option_optionrequest(options_out_len, options, requested_options,
      LWIP_ARRAYSIZE(requested_options), p_out->len);
    LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, DHCP6_STATE_REQUESTING_CONFIG, msg_out,
      DHCP6_INFOREQUEST, options_out_len, p_out->len);
    dhcp6_msg_finalize(options_out_len, p_out);

    DHCP6_STATS_INC(dhcp6.xmit);
    err = udp_sendto_if(dhcp6->pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif);
    pbuf_free(p_out);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_information_request: INFOREQUESTING -> %d\n", (int)err));
    LWIP_UNUSED_ARG(err);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp6_information_request: could not allocate DHCP6 request\n"));
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_REQUESTING_CONFIG, "dhcp6_information_request");
  if (dhcp6->tries < 255) {
    dhcp6->tries++;
  }
  msecs = (u16_t)((dhcp6->tries < 6 ? 1 << dhcp6->tries : 60) * 1000);
  dhcp6->request_timeout = (u16_t)((msecs + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_information_request(): set request timeout %"U16_F" msecs\n", msecs));
}

static err_t
dhcp6_request_config(struct netif *netif, struct dhcp6 *dhcp6)
{
  /* stateless mode enabled and no request running? */
  if (dhcp6->state == DHCP6_STATE_STATELESS_IDLE) {
    /* send Information-request and wait for answer; setup receive timeout */
    dhcp6_information_request(netif, dhcp6);
  }

  return ERR_OK;
}

static void
dhcp6_abort_config_request(struct dhcp6 *dhcp6)
{
  if (dhcp6->state == DHCP6_STATE_REQUESTING_CONFIG) {
    /* abort running request */
    dhcp6_set_state(dhcp6, DHCP6_STATE_STATELESS_IDLE, "dhcp6_abort_config_request");
  }
}

/* Handle a REPLY to INFOREQUEST
 * This parses DNS and NTP server addresses from the reply.
 */
static void
dhcp6_handle_config_reply(struct netif *netif, struct pbuf *p_msg_in)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  LWIP_UNUSED_ARG(dhcp6);
  LWIP_UNUSED_ARG(p_msg_in);

#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
  if (dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER)) {
    if (ERR_OK != dhcp6_set_dns_servers(netif, dhcp6, p_msg_in)) {
      DHCP6_STATS_INC(dhcp6.err);
      DHCP6_STATS_INC(dhcp6.drop);
      return;
    }
  }
  /* @ todo: parse and set Domain Search List */
#endif /* LWIP_DHCP6_PROVIDE_DNS_SERVERS */

#if LWIP_DHCP6_GET_NTP_SRV
  if (dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER)) {
    if (ERR_OK != dhcp6_set_ntp_srv(netif, dhcp6, p_msg_in)) {
      DHCP6_STATS_INC(dhcp6.err);
      DHCP6_STATS_INC(dhcp6.drop);
      return;
    }
  }
#endif /* LWIP_DHCP6_GET_NTP_SRV */
}
#endif /* LWIP_IPV6_DHCP6_STATELESS */

static inline void
dhcp6_put_ushort(u8_t *obuf, u16_t val)
{
  u16_t tmp = htons(val);
  (void)memcpy_s(obuf, sizeof(tmp), &tmp, sizeof(tmp));
}

#if LWIP_IPV6_DHCP6_STATEFUL
static void
dhcp6_stateful_struct_free(struct dhcp6 *dhcp6_p)
{
  if (dhcp6_p == NULL) {
    return;
  }
  if (dhcp6_p->duid.duid_id != NULL) {
    mem_free(dhcp6_p->duid.duid_id);
    dhcp6_p->duid.duid_id = NULL;
    dhcp6_p->duid.duid_len = 0;
  }

  dhcp6_p->struct_init = 0;

  return;
}

/*
 * caller : dhcp6_enable_stateful, check netif_p != NULL by netifapi_netif_common
 * caller : dhcp6_stateful_timeout, check netif_p != NULL by NETIF_FOREACH(netif)
 */
static void
dhcp6_linklocal_address_check(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  if (ip6_addr_ispreferred(netif_ip6_addr_state(netif_p, 0))) {
    dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_SOLICIT, "dhcp6_linklocal_address_check");
    dhcp6_set_timeout_params(dhcp6_p);
    dhcp6_solicit(netif_p, dhcp6_p);
  } else {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_linklocal_address_check(): linklocal address not ready"));
    dhcp6_p->request_timeout = 1;
  }

  return;
}

static err_t
dhcp6_form_duid(const struct netif *netif_p, struct dhcp6 *dhcp6_p, u8_t type)
{
  duid_t *duid_p = NULL;
  u16_t len;
  err_t ret = ERR_OK;

  duid_p = &(dhcp6_p->duid);
  switch (type) {
    case DHCP6_DUID_LLT:
      /* to do */
      ret = ERR_VAL;
      break;
    case DHCP6_DUID_EN:
      /* to do */
      ret = ERR_VAL;
      break;
    case DHCP6_DUID_LL:
      len = (u16_t)(4U + netif_p->hwaddr_len);
      duid_p->duid_id = (u8_t *)mem_malloc(len);
      if (duid_p->duid_id == NULL) {
        DHCP6_STATS_INC(dhcp6.memerr);
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("could not allocate duid_id\n"));
        return ERR_MEM;
      }
      dhcp6_put_ushort(duid_p->duid_id, DHCP6_DUID_LL);
      dhcp6_put_ushort(duid_p->duid_id + 2U, DHCP6HTYPE_ETHER);
      if (memcpy_s(duid_p->duid_id + 4U, netif_p->hwaddr_len, netif_p->hwaddr, netif_p->hwaddr_len) != EOK) {
        DHCP6_STATS_INC(dhcp6.memerr);
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("memory copy error\n"));
        return ERR_MEM;
      }
      duid_p->duid_len = len;
      break;
    case DHCP6_DUID_UUID:
      /* to do */
      ret = ERR_VAL;
      break;
    default:
      DHCP6_STATS_INC(dhcp6.opterr);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("unkonw duid type(%u)\n", type));
      ret = ERR_VAL;
      break;
  }

  return ret;
}

static err_t
dhcp6_create_iaid(const struct netif *netif_p, struct dhcp6 *dhcp6_p, u8_t idx)
{
  iaid_t *iaid_p = NULL;
  u16_t start_idx, copy_len;

  iaid_p = &((dhcp6_p->ia[idx]).iaid);
  iaid_p->iaid_len = sizeof(struct ia_info);
  (void)memset_s(&(iaid_p->ia), iaid_p->iaid_len, 0x0, iaid_p->iaid_len);

  if (netif_p->hwaddr_len > DHCP6_IAID_SIZE) {
    start_idx = (u16_t)(netif_p->hwaddr_len - DHCP6_IAID_SIZE);
    copy_len = DHCP6_IAID_SIZE;
  } else {
    start_idx = 0;
    copy_len = netif_p->hwaddr_len;
  }

  (void)memcpy_s(&(iaid_p->ia.dh6_ia_iaid), copy_len, netif_p->hwaddr + start_idx, copy_len);

  if (idx) {
    ((u8_t *)(&(iaid_p->ia.dh6_ia_iaid)))[3] = (u8_t)(((u8_t *)(&(iaid_p->ia.dh6_ia_iaid)))[3] + idx);
  }

  return ERR_OK;
}

static err_t
dhcp6_stateful_init(const struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  err_t ret;
  u8_t i;

  LWIP_ASSERT("dhcp6_p != NULL", dhcp6_p != NULL);
  LWIP_ASSERT("netif_p != NULL", netif_p != NULL);

  ret = dhcp6_form_duid(netif_p, dhcp6_p, DHCP6_DUID_LL);
  if (ret != ERR_OK) {
    goto dhcp6_stateful_init_failed;
  }

  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    ret = dhcp6_create_iaid(netif_p, dhcp6_p, i);
    if (ret != ERR_OK) {
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_create_iaid failed because of memory error.\n"));
      goto dhcp6_stateful_init_failed;
    }
  }

  dhcp6_p->struct_init = 1;

  return ERR_OK;
dhcp6_stateful_init_failed:
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("init stateful DHCPv6 failed\n"));
  dhcp6_stateful_struct_free(dhcp6_p);
  return ret;
}

static u16_t
dhcp6_option_append(u16_t options_out_len, u8_t *options, u8_t option_type, u16_t op_len, const u8_t *val)
{
  u16_t len = options_out_len;
  dhcp6_put_ushort(options + options_out_len, option_type);
  options_out_len = (u16_t)(options_out_len + sizeof(u16_t));
  dhcp6_put_ushort(options + options_out_len, op_len);
  options_out_len = (u16_t)(options_out_len + sizeof(u16_t));
  if (memcpy_s(options + options_out_len, op_len, val, op_len) != EOK) {
    return len;
  }
  options_out_len = (u16_t)(options_out_len + op_len);

  return options_out_len;
}

static void
dhcp6_set_timeout_params(struct dhcp6 *dhcp6_p)
{
  s8_t addr_idx;
  if (dhcp6_p == NULL) {
    return;
  }
  addr_idx = dhcp6_p->addr_idx;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("set timeout params of state %d\n", dhcp6_p->state));

  dhcp6_p->tries = 0;
  dhcp6_p->duration_start = sys_now();
  dhcp6_p->IRT = 0;
  dhcp6_p->MRC = 0;
  dhcp6_p->MRT = 0;
  dhcp6_p->MRD = 0;

  switch (dhcp6_p->state) {
    case DHCP6_STATE_STATEFUL_SOLICIT:
      dhcp6_p->IRT = SOL_TIMEOUT * MS_PER_SECOND;
      dhcp6_p->MRT = SOL_MAX_RT * MS_PER_SECOND;
      break;
    case DHCP6_STATE_STATEFUL_INFOREQ:
      dhcp6_p->IRT = INF_TIMEOUT * MS_PER_SECOND;
      dhcp6_p->MRT = INF_MAX_RT * MS_PER_SECOND;
      break;
    case DHCP6_STATE_STATEFUL_REQUEST:
      dhcp6_p->IRT = REQ_TIMEOUT * MS_PER_SECOND;
      dhcp6_p->MRC = REQ_MAX_RC;
      dhcp6_p->MRT = REQ_MAX_RT * MS_PER_SECOND;
      break;
    case DHCP6_STATE_STATEFUL_RENEW:
      dhcp6_p->IRT = REN_TIMEOUT * MS_PER_SECOND;
      dhcp6_p->MRT = REN_MAX_RT * MS_PER_SECOND;
      if (addr_idx != LWIP_IPV6_NUM_ADDRESSES) {
        if (dhcp6_p->ia[addr_idx].iaid.ia.dh6_ia_t2 == 0) {
          dhcp6_p->MRD = 900000; /* 900000: 15 min */
        } else if (dhcp6_p->ia[addr_idx].iaid.ia.dh6_ia_t2 == DHCP6_DURATION_INFINITE) {
          /* Renew forever, never hit Rebind */
          dhcp6_p->MRD = 0;
        } else {
          u32_t max;
          max = dhcp6_p->ia[addr_idx].iaid.ia.dh6_ia_t2 - dhcp6_p->ia[addr_idx].iaid.ia.dh6_ia_t1;
          if (max > (u32_t)(~0) / MS_PER_SECOND) {
            max = (u32_t)(~0) / MS_PER_SECOND;
          }
          /* Remaining util T2 */
          dhcp6_p->MRD = max * MS_PER_SECOND;
        }
      }
      break;
    case DHCP6_STATE_STATEFUL_REBIND:
      dhcp6_p->IRT = REB_TIMEOUT * MS_PER_SECOND;
      dhcp6_p->MRT = REB_MAX_RT * MS_PER_SECOND;
      if (addr_idx != LWIP_IPV6_NUM_ADDRESSES) {
        u32_t max;
        max = dhcp6_p->ia[addr_idx].iaaddr.dh6_iaaddr_valid - dhcp6_p->ia[addr_idx].iaid.ia.dh6_ia_t2;
        if (max > (u32_t)(~0) / MS_PER_SECOND) {
          max = (u32_t)(~0) / MS_PER_SECOND;
        }
        dhcp6_p->MRD = max * MS_PER_SECOND;
      }
      break;
    case DHCP6_STATE_STATEFUL_RELEASE:
      dhcp6_p->IRT = REL_TIMEOUT * MS_PER_SECOND;
      dhcp6_p->MRC = REL_MAX_RC;
      break;
    case DHCP6_STATE_STATEFUL_DECLINE:
      dhcp6_p->IRT = DEC_TIMEOUT * MS_PER_SECOND;
      dhcp6_p->MRC = DEC_MAX_RC;
      break;
    default:
      DHCP6_STATS_INC(dhcp6.err);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("unkonw state(%d)\n", dhcp6_p->state));
      break;
  }

  return;
}

static u32_t
dhcp6_retransmission_duration(u32_t duration_start)
{
  u32_t now_ms = sys_now();
  u32_t duration;

  if (now_ms >= duration_start) {
    duration = now_ms - duration_start;
  } else {
    /*
     * as (now_ms < duration_start), ((0xffffffff - duration_start) + now_ms)
     * must be smaller than 0xffffffff. duration will not be overflow.
     */
    duration = (0xffffffff - duration_start) + now_ms;
  }

  return duration;
}

static void
dhcp6_stateful_timeout(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_timeout() : %02x\n", dhcp6_p->state));

  switch (dhcp6_p->state) {
    case DHCP6_STATE_STATEFUL_INIT:
      dhcp6_linklocal_address_check(netif_p, dhcp6_p);
      break;
    case DHCP6_STATE_STATEFUL_SOLICIT:
      if (dhcp6_p->serv_list == NULL) {
        dhcp6_solicit(netif_p, dhcp6_p);
      } else {
        dhcp6_solicit_to_request(netif_p, dhcp6_p);
      }
      break;
    case DHCP6_STATE_STATEFUL_INFOREQ:
      break;
    case DHCP6_STATE_STATEFUL_REQUEST:
      if ((dhcp6_p->MRC != 0) && (dhcp6_p->tries >= dhcp6_p->MRC)) {
        dhcp6_request_next_server(netif_p, dhcp6_p);
      } else {
        dhcp6_request(netif_p, dhcp6_p);
      }
      break;
    case DHCP6_STATE_STATEFUL_IDLE:
      dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_RENEW, "dhcp6_stateful_timeout");
      dhcp6_set_timeout_params(dhcp6_p);
      dhcp6_renew(netif_p, dhcp6_p);
      break;
    case DHCP6_STATE_STATEFUL_RENEW:
      if ((dhcp6_p->MRD && (dhcp6_retransmission_duration(dhcp6_p->duration_start) >= dhcp6_p->MRD)) ||
          (dhcp6_p->MRC && dhcp6_p->tries >=  dhcp6_p->MRC)) {
        dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_REBIND, "dhcp6_stateful_timeout");
        dhcp6_set_timeout_params(dhcp6_p);
        dhcp6_rebind(netif_p, dhcp6_p);
      } else {
        dhcp6_renew(netif_p, dhcp6_p);
      }
      break;
    case DHCP6_STATE_STATEFUL_REBIND:
      if ((dhcp6_p->MRD && (dhcp6_retransmission_duration(dhcp6_p->duration_start) >= dhcp6_p->MRD)) ||
          (dhcp6_p->MRC && dhcp6_p->tries >=  dhcp6_p->MRC)) {
        dhcp6_rebind_failure_cleanup(netif_p, dhcp6_p);
        dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_SOLICIT, "dhcp6_stateful_timeout");
        dhcp6_set_timeout_params(dhcp6_p);
        dhcp6_solicit(netif_p, dhcp6_p);
      } else {
        dhcp6_rebind(netif_p, dhcp6_p);
      }
      break;
    case DHCP6_STATE_STATEFUL_RELEASE:
      if ((dhcp6_p->MRD && (dhcp6_retransmission_duration(dhcp6_p->duration_start) >= dhcp6_p->MRD)) ||
          (dhcp6_p->MRC && dhcp6_p->tries >=  dhcp6_p->MRC)) {
        dhcp6_post_release_cleanup(dhcp6_p);
        /* Don't need timer any more. */
        dhcp6_p->request_timeout = 0;
        dhcp6_set_state(dhcp6_p, DHCP6_STATE_OFF, "dhcp6_stateful_timeout");
      } else {
        dhcp6_release(netif_p, dhcp6_p);
      }
      break;
    case DHCP6_STATE_STATEFUL_DECLINE:
      if ((dhcp6_p->MRC != 0) && (dhcp6_p->tries >= dhcp6_p->MRC)) {
        dhcp6_request_next_server(netif_p, dhcp6_p);
      } else {
        dhcp6_decline(netif_p, dhcp6_p);
      }
      break;
    default:
      DHCP6_STATS_INC(dhcp6.err);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("unkonw state(%d)\n", dhcp6_p->state));
      break;
  }

  return;
}

static opt_comm_t *
dhcp6_server_find_opt(dhcp6_serverinfo_t *dhcp6_serverinfo_p, u16_t opt_type)
{
  opt_comm_t *opt_comm_p = dhcp6_serverinfo_p->opts;

  while (opt_comm_p) {
    if (opt_comm_p->opt_type == opt_type) {
      break;
    }
    opt_comm_p = opt_comm_p->next;
  }

  return opt_comm_p;
}

static dhcp6_serverinfo_t *
dhcp6_find_server(const struct dhcp6 *dhcp6_p, const u8_t *serv_duid, u16_t serv_duid_len)
{
  dhcp6_serverinfo_t *dhcp6_serverinfo_p = dhcp6_p->serv_list;
  opt_comm_t *opt_comm_p = NULL;

  while (dhcp6_serverinfo_p) {
    opt_comm_p = dhcp6_server_find_opt(dhcp6_serverinfo_p, DHCP6_OPTION_SERVERID);
    if ((opt_comm_p != NULL) && (opt_comm_p->opt_start != NULL) && (opt_comm_p->opt_len == serv_duid_len) &&
        memcmp(serv_duid, opt_comm_p->opt_start, serv_duid_len) == 0) {
        break;
    }
    dhcp6_serverinfo_p = dhcp6_serverinfo_p->next;
  }

  return dhcp6_serverinfo_p;
}

static void
dhcp6_serverinfo_free(dhcp6_serverinfo_t *dhcp6_serverinfo_p)
{
  opt_comm_t *opt_p = NULL;
  opt_comm_t *opt_tmp = NULL;

  if (dhcp6_serverinfo_p == NULL) {
    return;
  }

  if (dhcp6_serverinfo_p->buff) {
    mem_free(dhcp6_serverinfo_p->buff);
    dhcp6_serverinfo_p->buff = NULL;
  }

  opt_p = dhcp6_serverinfo_p->opts;
  while (opt_p) {
    opt_tmp = opt_p->next;
    mem_free(opt_p);
    opt_p = opt_tmp;
  }
  dhcp6_serverinfo_p->opts = NULL;

  mem_free(dhcp6_serverinfo_p);

  return;
}

static err_t
dhcp6_serverinfo_parse(dhcp6_serverinfo_t *dhcp6_serverinfo_p, u8_t *buff, u16_t buff_len)
{
  u16_t idx, op, len, opt_num;
  opt_comm_t *opt_p = NULL;

  if ((dhcp6_serverinfo_p == NULL) || (buff == NULL)) {
    return ERR_VAL;
  }

  idx = 0;
  opt_num = 0;
  while (idx <= (buff_len - DHCP6_OPT_HEADER_LEN)) {
    op = (u16_t)((buff[idx] << 8) | buff[idx + 1]);  // the first two bytes denotes option type
    len = (u16_t)((buff[idx + 2] << 8) | buff[idx + 3]);  // the second two bytes denotes option length
    if (len > (buff_len - DHCP6_OPT_HEADER_LEN - idx)) {
      /* invalid option length */
      return ERR_VAL;
    }
    opt_p = (opt_comm_t *)mem_malloc(sizeof(opt_comm_t));
    if (opt_p == NULL) {
      DHCP6_STATS_INC(dhcp6.memerr);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("malloc mem failed\n"));
      return ERR_MEM;
    }
    (void)memset_s(opt_p, sizeof(opt_comm_t), 0x0, sizeof(opt_comm_t));
    opt_p->next = NULL;
    opt_p->opt_type = op;
    opt_p->opt_len = len;
    if (len == 0) {
      opt_p->opt_start = NULL;
    } else {
      opt_p->opt_start = buff + idx + DHCP6_OPT_HEADER_LEN;
    }

    if (dhcp6_serverinfo_p->opts == NULL) {
      dhcp6_serverinfo_p->opts = opt_p;
    } else {
      opt_p->next = dhcp6_serverinfo_p->opts;
      dhcp6_serverinfo_p->opts = opt_p;
    }

    /* because (len > (buff_len-4-idx)) is false, so idx will not reverse */
    idx = (u16_t)(idx + len + DHCP6_OPT_HEADER_LEN);
    if (++opt_num > DHCP6_SERVERINFO_MAX_OPT_NUM) {
      DHCP6_STATS_INC(dhcp6.opterr);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("too many dhcp6 options\n"));
      return ERR_VAL;
    }
  }

  return ERR_OK;
}

static dhcp6_serverinfo_t *
dhcp6_serverinfo_new(const struct pbuf *p_msg_in)
{
  dhcp6_serverinfo_t *dhcp6_serverinfo_p = NULL;
  u16_t copied, buff_len;

  /*
   * tot_len must be at least length of dhcp6 message header plus 8.
   * 8 : op and len code length of option SERVER_ID and option CLIENT_ID is 4*2
   */
  if (p_msg_in->tot_len < (sizeof(struct dhcp6_msg) + 8)) {
    DHCP6_STATS_INC(dhcp6.lenerr);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("invalid dhcp6 message\n"));
    return NULL;
  }

  buff_len = (u16_t)(p_msg_in->tot_len - sizeof(struct dhcp6_msg));
  if (buff_len > DHCP6_SERVERINFO_MAX_LEN) {
    DHCP6_STATS_INC(dhcp6.lenerr);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6 server advertise message is too large\n"));
    return NULL;
  }

  dhcp6_serverinfo_p = (dhcp6_serverinfo_t *)mem_malloc(sizeof(dhcp6_serverinfo_t));
  if (dhcp6_serverinfo_p == NULL) {
    DHCP6_STATS_INC(dhcp6.memerr);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("malloc mem failed\n"));
    return NULL;
  }
  (void)memset_s(dhcp6_serverinfo_p, sizeof(dhcp6_serverinfo_t), 0x0, sizeof(dhcp6_serverinfo_t));

  dhcp6_serverinfo_p->buff = (u8_t *)mem_malloc(buff_len);
  if (dhcp6_serverinfo_p->buff == NULL) {
    DHCP6_STATS_INC(dhcp6.memerr);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("malloc mem failed\n"));
    goto serverinfo_new_failed;
  }

  (void)memset_s(dhcp6_serverinfo_p->buff, buff_len, 0x0, buff_len);
  copied = pbuf_copy_partial(p_msg_in, (void *)(dhcp6_serverinfo_p->buff), buff_len, sizeof(struct dhcp6_msg));
  if (copied != buff_len) {
    DHCP6_STATS_INC(dhcp6.lenerr);
    goto serverinfo_new_failed;
  }
  dhcp6_serverinfo_p->buff_len = buff_len;
  if (ERR_OK != dhcp6_serverinfo_parse(dhcp6_serverinfo_p, dhcp6_serverinfo_p->buff, dhcp6_serverinfo_p->buff_len)) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("parse dhcp6 server info failed\n"));
    goto serverinfo_new_failed;
  }

  dhcp6_serverinfo_p->active = 1;
  dhcp6_serverinfo_p->unicast = 0;
  dhcp6_serverinfo_p->pref = 0;
  dhcp6_serverinfo_p->next = NULL;
  IP_SET_TYPE_VAL(dhcp6_serverinfo_p->unicast_addr, IPADDR_TYPE_V6);

  return dhcp6_serverinfo_p;
serverinfo_new_failed:
  dhcp6_serverinfo_free(dhcp6_serverinfo_p);
  return NULL;
}

static void
dhcp6_serverinfo_insert(dhcp6_serverinfo_t **head, dhcp6_serverinfo_t *new_p)
{
  dhcp6_serverinfo_t *p = NULL;
  dhcp6_serverinfo_t *q = NULL;

  if (new_p == NULL) {
    return;
  }

  if (*head == NULL) {
    *head = new_p;
    return;
  }

  p = *head;
  while (new_p->pref <= p->pref && p->next != NULL) {
    q = p;
    p = p->next;
  }

  if (new_p->pref > p->pref) {
    if (p == *head) {
      *head = new_p;
      new_p->next = p;
    } else {
      if (q != NULL) {
        q->next = new_p;
        new_p->next = p;
      }
    }
  } else {
    p->next = new_p;
  }

  return;
}

static void
dhcp6_serverinfo_free_list(dhcp6_serverinfo_t **head)
{
  dhcp6_serverinfo_t *p = NULL;
  dhcp6_serverinfo_t *q = NULL;

  if ((head == NULL) || (*head == NULL)) {
    return;
  }

  p = *head;
  while (p != NULL) {
    q = p->next;
    dhcp6_serverinfo_free(p);
    p = q;
  }

  *head = NULL;
}

static dhcp6_serverinfo_t *
dhcp6_serverinfo_find_active(dhcp6_serverinfo_t *head)
{
  dhcp6_serverinfo_t *p = NULL;

  if (head == NULL) {
    return NULL;
  }

  for (p = head; p; p = p->next) {
    if (p->active == 1) {
      return p;
    }
  }

  return NULL;
}

static ia_t*
dhcp6_get_ia_by_iaid(struct dhcp6 *dhcp6_p, const u8_t *iaid, s8_t *pdhcp6_addr_idx)
{
  s8_t i;
  ia_t *ia_p = NULL;

  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    ia_p = &(dhcp6_p->ia[i]);
    if (memcmp((u8_t *)&(ia_p->iaid.ia.dh6_ia_iaid), iaid, DHCP6_IAID_SIZE) == 0) {
      *pdhcp6_addr_idx = i;
      return ia_p;
    }
  }

  return NULL;
}

static void
dhcp6_clear_ia(struct dhcp6 *dhcp6_p)
{
  s8_t addr_idx = dhcp6_p->addr_idx;

  if (addr_idx == LWIP_IPV6_NUM_ADDRESSES) {
    return;
  }

  (void)memset_s(&dhcp6_p->ia[addr_idx].iaaddr, sizeof(dhcp6_p->ia[addr_idx].iaaddr),
                 0, sizeof(dhcp6_p->ia[addr_idx].iaaddr));
  dhcp6_p->ia[addr_idx].iaid.ia.dh6_ia_t1 = 0;
  dhcp6_p->ia[addr_idx].iaid.ia.dh6_ia_t2 = 0;
}

static void
dhcp6_rebind_failure_cleanup(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  s8_t addr_idx = dhcp6_p->addr_idx;

  if (addr_idx != LWIP_IPV6_NUM_ADDRESSES) {
    netif_ip6_addr_set_state(netif_p, addr_idx, IP6_ADDR_INVALID);
    netif_ip6_addr_set_parts(netif_p, addr_idx, 0, 0, 0, 0);
  }
  dhcp6_clear_ia(dhcp6_p);
  dhcp6_clear_all_options(dhcp6_p);
  dhcp6_p->current_serv = NULL;
  dhcp6_serverinfo_free_list(&dhcp6_p->serv_list);
}

static void
dhcp6_post_release_cleanup(struct dhcp6 *dhcp6_p)
{
  dhcp6_clear_ia(dhcp6_p);
  dhcp6_clear_all_options(dhcp6_p);
  dhcp6_p->current_serv = NULL;
  dhcp6_serverinfo_free_list(&dhcp6_p->serv_list);
  dhcp6_p->addr_idx = LWIP_IPV6_NUM_ADDRESSES;
}

static u8_t *
dhcp6_option_extract_content(const struct pbuf *p_msg_in, const struct dhcp6 *dhcp6_p,
                             enum dhcp6_option_idx idx)
{
  LWIP_UNUSED_ARG(dhcp6_p);
  u16_t op_start = dhcp6_get_option_start(dhcp6_p, idx);
  u16_t op_len = dhcp6_get_option_length(dhcp6_p, idx);
  u8_t *option_p = mem_malloc((mem_size_t)op_len);
  if (option_p == NULL) {
    return NULL;
  }
  if (pbuf_copy_partial(p_msg_in, option_p, op_len, op_start) != op_len) {
    mem_free(option_p);
    return NULL;
  }
  return option_p;
}

static void
dhcp6_option_free_content(u8_t *option_p)
{
  if (option_p != NULL) {
    mem_free(option_p);
  }
}

static void
dhcp6_handle_advertise(struct netif *netif_p, const struct pbuf *p_msg_in)
{
  struct dhcp6 *dhcp6_p = netif_dhcp6_data(netif_p);
  dhcp6_serverinfo_t *dhcp6_serverinfo_p = NULL;
  u16_t op_start, op_len, copied;
  u8_t *option_p = NULL;
  struct dhcp6opt_iaaddr *iaaddr_p = NULL;
  u32_t dh6_iaaddr_preferred;
  u32_t dh6_iaaddr_valid;

  LWIP_UNUSED_ARG(dhcp6_p);
  LWIP_UNUSED_ARG(p_msg_in);

  if (dhcp6_p->state != DHCP6_STATE_STATEFUL_SOLICIT) {
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("discard this Advertise\n"));
    goto exit;
  }

  if (!dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_IA_NA)) {
    /* we should not accept this ADVERTISE if not contain option IA_NA */
    DHCP6_STATS_INC(dhcp6.proterr);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("no IA_NA of Advertise\n"));
    goto exit;
  }
  u16_t idx, idx_last;
  u16_t op, len;
  u8_t ia_addr_found = 0;
  op_start = dhcp6_get_option_start(dhcp6_p, DHCP6_OPTION_IDX_IA_NA);
  op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_IA_NA);
  option_p = dhcp6_option_extract_content(p_msg_in, dhcp6_p, DHCP6_OPTION_IDX_IA_NA);
  if (option_p == NULL) {
    DHCP6_STATS_INC(dhcp6.lenerr);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("len : %u\n", op_len));
    goto exit;
  }

  /* IA_NA option-len : 12 + length of IA_NA-options field */
  if (op_len <= 12) {
    /* there is no sub option of IA_NA */
    DHCP6_STATS_INC(dhcp6.proterr);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("no sub option of IA_NA\n"));
    goto exit;
  }
  idx = 12;
  while (idx <= (op_len - DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN)) {
    op = (u16_t)((option_p[idx] << 8) | option_p[idx + 1]);
    if (DHCP6_OPTION_IAADDR == op) {
      ia_addr_found = 1;
      break;
    }
    len = (u16_t)((option_p[idx + 2] << 8) | option_p[idx + 3]);
    /*
     * plus 4 to skip the field option code and length code, because (idx <= (op_len- 4)), so there
     * is no need to check idx_last overflow
     */
    idx_last = (u16_t)(idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN);
    idx = (u16_t)(idx_last + len);
    if (idx < idx_last) {
      /* length mismatch */
      DHCP6_STATS_INC(dhcp6.lenerr);
      DHCP6_STATS_INC(dhcp6.drop);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("idx_last : %u, len : %u\n", idx_last, len));
      goto exit;
    }
  }
  if (ia_addr_found == 0) {
    /* there is no option IA Address as a sub option of IA_NA */
    DHCP6_STATS_INC(dhcp6.proterr);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("no option IA Address of IA_NA\n"));
    goto exit;
  }

  /*
   * the client MUST ignore any Advertise message that includes
   * a Status Code option containing the value NoAddrsAvail.
   * [RFC3315 Section 17.1.3].
   * We only apply this when we are going to request an address or
   * a prefix.
   */
  if (dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_STATUS_CODE)) {
    u16_t status_code;
    op_start = dhcp6_get_option_start(dhcp6_p, DHCP6_OPTION_IDX_STATUS_CODE);
    op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_STATUS_CODE);
    if (op_len < sizeof(u16_t)) {
      /* length of option Status Code must be not less than 2 */
      DHCP6_STATS_INC(dhcp6.lenerr);
      DHCP6_STATS_INC(dhcp6.drop);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("invalid option STATUS_CODE len %u\n", op_len));
      goto exit;
    }
    copied = pbuf_copy_partial(p_msg_in, (void *)(&status_code), sizeof(status_code), op_start);
    if (copied != sizeof(status_code)) {
      /* pbuf length mismatch */
      DHCP6_STATS_INC(dhcp6.lenerr);
      DHCP6_STATS_INC(dhcp6.drop);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("length mismatch, copied %u\n", copied));
      goto exit;
    }
    status_code = ntohs(status_code);
    if (DHCP6_STATUS_NOADDRSAVAIL == status_code) {
      /* NoAddrsAvail */
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("have status code : NoAddrsAvail\n"));
      goto exit;
    }
  }

  /* Status Code also can be the sub option of IA_NA/IA_TA/IAADDR */
  if (dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_IA_NA)) {
    u16_t status_code;
    op_start = dhcp6_get_option_start(dhcp6_p, DHCP6_OPTION_IDX_IA_NA);
    op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_IA_NA);
    dhcp6_option_free_content(option_p);
    option_p = dhcp6_option_extract_content(p_msg_in, dhcp6_p, DHCP6_OPTION_IDX_IA_NA);
    if (option_p == NULL) {
      DHCP6_STATS_INC(dhcp6.lenerr);
      DHCP6_STATS_INC(dhcp6.drop);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("len : %u\n", op_len));
      goto exit;
    }
    /* IA_NA option-len : 12 + IA_NA option code length */
    if (op_len > 12) {
      idx = 12;
      while (idx <= (op_len - DHCP6_OPT_HEADER_LEN)) {
        op = (u16_t)((option_p[idx] << 8) | option_p[idx + 1]);
        len = (u16_t)((option_p[idx + 2] << 8) | option_p[idx + 3]);
        if (DHCP6_OPTION_IAADDR == op) {
          if (len >= 24) {
            if ((idx + len) > (op_len - DHCP6_OPT_HEADER_LEN)) {
              /* length mismatch */
              DHCP6_STATS_INC(dhcp6.lenerr);
              DHCP6_STATS_INC(dhcp6.drop);
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("length mismatch idx : %u, len : %u, op_len : %u\n",
                idx, len, op_len));
              goto exit;
            }
            iaaddr_p = (struct dhcp6opt_iaaddr *)(&(option_p[idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN]));
            dh6_iaaddr_preferred = ntohl(iaaddr_p->dh6_iaaddr_preferred);
            dh6_iaaddr_valid = ntohl(iaaddr_p->dh6_iaaddr_valid);
            if (dh6_iaaddr_preferred > dh6_iaaddr_valid) {
              /* A client discards any addresses for which the preferred lifetime is greater than the valid lifetime. */
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE,
                          ("error: the preferred lifetime is greater than the valid lifetime\n"));
              goto exit;
            }
          }
          /* IAADDR option-len : 24 + length of IAaddr-options field */
          if (len > 24) {
            u16_t sub_idx, sub_op, sub_len;
            sub_idx = (u16_t)(idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + 24);
            if ((sub_idx < idx) || (sub_idx > (op_len - DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN))) {
              /* length mismatch */
              DHCP6_STATS_INC(dhcp6.lenerr);
              DHCP6_STATS_INC(dhcp6.drop);
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("length mismatch sub_idx : %u, idx : %u, op_len : %u\n",
                sub_idx, idx, op_len));
              goto exit;
            }
            /* the sub option of IAADDR only can be Status Code */
            sub_op = (u16_t)((option_p[sub_idx] << 8) | option_p[sub_idx + 1]);
            sub_len = (u16_t)((option_p[sub_idx + 2] << 8) | option_p[sub_idx + 3]);
            if ((u16_t)(sub_idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + sub_len) < sub_idx ||
                (u16_t)(sub_idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + sub_len) > op_len) {
              /* length mismatch */
              DHCP6_STATS_INC(dhcp6.lenerr);
              DHCP6_STATS_INC(dhcp6.drop);
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("length mismatch sub_idx : %u, sub_len : %u, op_len : %u\n",
                sub_idx, sub_len, op_len));
              goto exit;
            }

            if ((DHCP6_OPTION_STATUS_CODE == sub_op) && (sub_len >= sizeof(u16_t))) {
              status_code = (u16_t)((option_p[sub_idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN] << 8) |
                option_p[sub_idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + 1]);
              if (DHCP6_STATUS_NOADDRSAVAIL == status_code) {
                /* NoAddrsAvail */
                LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("have status code : NoAddrsAvail of IAADDR\n"));
                goto exit;
              }
            } else {
              /* as the sub option of IAADDR only can be Status Code and length of option Status Code
               * must be not less than 2, if not, this message is invalid.
               */
              if (DHCP6_OPTION_STATUS_CODE == sub_op) {
                DHCP6_STATS_INC(dhcp6.proterr);
              } else {
                DHCP6_STATS_INC(dhcp6.lenerr);
              }
              DHCP6_STATS_INC(dhcp6.drop);
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE,
                          ("inlvaid sub option %u or invalid length of option STATUS_CODE\n", sub_op));
              goto exit;
            }
          }
        } else if (DHCP6_OPTION_STATUS_CODE == op) {
          if (len < 2) {
            /* length of option Status Code must be not less than 2 */
            DHCP6_STATS_INC(dhcp6.lenerr);
            DHCP6_STATS_INC(dhcp6.drop);
            LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("invalid option STATUS_CODE len %u\n", len));
            goto exit;
          }
          status_code = (u16_t)((option_p[idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN] << 8) |
            option_p[idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + 1]);
          if (DHCP6_STATUS_NOADDRSAVAIL == status_code) {
            /* NoAddrsAvail */
            LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("have status code : NoAddrsAvail of IA_NA\n"));
            goto exit;
          }
        }
        /*
         * plus 4 to skip the field option code and length code, because (idx <= (op_len- 4)), so there
         * is no need to check idx_last overflow
         */
        idx_last = (u16_t)(idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN);
        idx = (u16_t)(idx_last + len);
        if (idx < idx_last) {
          /* length mismatch */
          DHCP6_STATS_INC(dhcp6.lenerr);
          DHCP6_STATS_INC(dhcp6.drop);
          LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("length mismatch idx_last : %u, len : %u\n", idx_last, len));
          goto exit;
        }
      }
    }
  }

  /* A Advertise message must contain a Server ID option and a Client ID option */
  if (!dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_SERVER_ID) ||
      !dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_CLI_ID)) {
    DHCP6_STATS_INC(dhcp6.proterr);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("no SERVER_ID or CLI_ID\n"));
    goto exit;
  }

  /*
   * DUID in the Client ID option (which must be contained for our
   * client implementation) must match ours.
   */
  op_start = dhcp6_get_option_start(dhcp6_p, DHCP6_OPTION_IDX_CLI_ID);
  op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_CLI_ID);
  dhcp6_option_free_content(option_p);
  option_p = dhcp6_option_extract_content(p_msg_in, dhcp6_p, DHCP6_OPTION_IDX_CLI_ID);
  if ((op_len != dhcp6_p->duid.duid_len) || (option_p == NULL) ||
      (memcmp(option_p, dhcp6_p->duid.duid_id, op_len) != 0)) {
    /*
     * condition 1 : op_len must be equal to the netif's duid_len
     * condition 2 : the content of option CLI_ID must be the same with the netif's duid
     */
    DHCP6_STATS_INC(dhcp6.err);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("DUID not match\n"));
    goto exit;
  }

  op_start = dhcp6_get_option_start(dhcp6_p, DHCP6_OPTION_IDX_SERVER_ID);
  op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_SERVER_ID);
  dhcp6_option_free_content(option_p);
  option_p = dhcp6_option_extract_content(p_msg_in, dhcp6_p, DHCP6_OPTION_IDX_SERVER_ID);
  if (option_p == NULL) {
    DHCP6_STATS_INC(dhcp6.lenerr);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("invalid len(%u) of SERVER_ID or copy failed\n", op_len));
    goto exit;
  }
  dhcp6_serverinfo_p = dhcp6_find_server(dhcp6_p, option_p, op_len);
  if (dhcp6_serverinfo_p == NULL) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("have new server info\n"));
    if (dhcp6_p->serv_cnt >= DHCP6_MAX_SERVER_NUM) {
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("too many server info\n"));
      DHCP6_STATS_INC(dhcp6.drop);
      goto exit;
    }
    dhcp6_p->serv_cnt++;
    dhcp6_serverinfo_p = dhcp6_serverinfo_new(p_msg_in);
    if (dhcp6_serverinfo_p == NULL) {
      DHCP6_STATS_INC(dhcp6.drop);
      goto exit;
    }
    if (dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_PREFERENCE)) {
      u8_t pref;
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("have PREFERENCE\n"));
      op_start = dhcp6_get_option_start(dhcp6_p, DHCP6_OPTION_IDX_PREFERENCE);
      copied = pbuf_copy_partial(p_msg_in, (void *)(&pref), sizeof(pref), op_start);
      if (copied != sizeof(pref)) {
        /* pbuf length mismatch */
        DHCP6_STATS_INC(dhcp6.lenerr);
        DHCP6_STATS_INC(dhcp6.drop);
        dhcp6_serverinfo_free(dhcp6_serverinfo_p);
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("copy PREFERENCE failed\n"));
        goto exit;
      }
      dhcp6_serverinfo_p->pref = pref;
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("PREFERENCE %02x\n", pref));
    }
    dhcp6_serverinfo_insert(&(dhcp6_p->serv_list), dhcp6_serverinfo_p);

    /*
     * If the client receives an Advertise message that includes a
     * Preference option with a preference value of 255, the client
     * immediately begins a client-initiated message exchange.
     * [RFC3315 Section 17.1.2]
     */
    /*
     * If the client does not receive any Advertise messages before the
     * first RT has elapsed, it begins the retransmission mechanism
     * described in section 14. The client terminates the retransmission
     * process as soon as it receives any Advertise message, and the client
     * acts on the received Advertise message without waiting for any
     * additional Advertise messages.
     * [RFC3315 Section 17.1.2]
     */
    if ((dhcp6_serverinfo_p->pref == DHCP6_OPTION_PREFERENCE_MAX) ||
        (dhcp6_p->RT > (2 * dhcp6_p->IRT))) {
      dhcp6_p->current_serv = dhcp6_serverinfo_p;
      dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_REQUEST, "dhcp6_handle_advertise");
      dhcp6_set_timeout_params(dhcp6_p);
      dhcp6_request(netif_p, dhcp6_p);
    }
  }

exit:
  dhcp6_option_free_content(option_p);
}

static void
dhcp6_handle_reply(struct netif *netif_p, const struct pbuf *p_msg_in)
{
  struct dhcp6 *dhcp6_p = netif_dhcp6_data(netif_p);
  u16_t op_len;
  u8_t state;
  u8_t *option_p = NULL;
#if DHCP6_ENABLE_UNICAST_SUPPORT
  dhcp6_serverinfo_t *dhcp6_serverinfo_p = NULL;
#endif
  s8_t addr_idx = dhcp6_p->addr_idx;

  LWIP_UNUSED_ARG(dhcp6_p);
  LWIP_UNUSED_ARG(p_msg_in);

  state = dhcp6_p->state;
  if ((state != DHCP6_STATE_STATEFUL_INFOREQ) &&
      (state != DHCP6_STATE_STATEFUL_REQUEST) &&
      (state != DHCP6_STATE_STATEFUL_RENEW) &&
      (state != DHCP6_STATE_STATEFUL_REBIND) &&
      (state != DHCP6_STATE_STATEFUL_RELEASE) &&
      (state != DHCP6_STATE_STATEFUL_DECLINE)) {
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("discard this Reply\n"));
    goto exit;
  }

  /* A Reply message must contain a Server ID option and a Client ID option */
  if (!dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_SERVER_ID) ||
      !dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_CLI_ID)) {
    DHCP6_STATS_INC(dhcp6.proterr);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("no SERVER_ID or CLI_ID\n"));
    goto exit;
  }

  /*
   * DUID in the Client ID option (which must be contained for our
   * client implementation) must match ours.
   */
  op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_CLI_ID);
  option_p = dhcp6_option_extract_content(p_msg_in, dhcp6_p, DHCP6_OPTION_IDX_CLI_ID);
  if ((op_len != dhcp6_p->duid.duid_len) || (option_p == NULL) ||
      (memcmp(option_p, dhcp6_p->duid.duid_id, op_len) != 0)) {
    /*
     * condition 1 : op_len must be equal to the netif's duid_len
     * condition 2 : the content of option CLI_ID must be the same with the netif's duid
     */
    DHCP6_STATS_INC(dhcp6.err);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("DUID not match\n"));
    goto exit;
  }

#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
  if (dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_DNS_SERVER)) {
    if (ERR_OK != dhcp6_set_dns_servers(netif_p, dhcp6_p, p_msg_in)) {
      DHCP6_STATS_INC(dhcp6.err);
      DHCP6_STATS_INC(dhcp6.drop);
      goto exit;
    }
  }
  /* @ Note: parse and set Domain Search List */
#endif /* LWIP_DHCP6_PROVIDE_DNS_SERVERS */

#if LWIP_DHCP6_GET_NTP_SRV
  if (dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_NTP_SERVER)) {
    if (ERR_OK != dhcp6_set_ntp_srv(netif_p, dhcp6_p, p_msg_in)) {
      DHCP6_STATS_INC(dhcp6.err);
      DHCP6_STATS_INC(dhcp6.drop);
      goto exit;
    }
  }
#endif /* LWIP_DHCP6_GET_NTP_SRV */

  if (state != DHCP6_STATE_STATEFUL_RELEASE) {
    if (dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_IA_NA)) {
      ia_t *ia_p = NULL;
      u16_t idx, idx_last, status_code;
      u16_t op, len;
      s8_t addr_idx_match;
      struct netif *netif = NULL;
      struct ia_info *ia_info_p = NULL;
      struct dhcp6opt_iaaddr *iaaddr_p = NULL;
      op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_IA_NA);
      dhcp6_option_free_content(option_p);
      option_p = dhcp6_option_extract_content(p_msg_in, dhcp6_p, DHCP6_OPTION_IDX_IA_NA);
      if (option_p == NULL) {
        DHCP6_STATS_INC(dhcp6.lenerr);
        DHCP6_STATS_INC(dhcp6.drop);
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("len : %u\n", op_len));
        goto exit;
      }
      ia_p = dhcp6_get_ia_by_iaid(dhcp6_p, &(option_p[0]), &addr_idx);
      if ((ia_p == NULL) || (addr_idx != dhcp6_p->addr_idx)) {
        DHCP6_STATS_INC(dhcp6.proterr);
        DHCP6_STATS_INC(dhcp6.drop);
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("no such iaid of netif, dhcp6_addr_idx:%d\n", dhcp6_p->addr_idx));
        goto exit;
      }

      ia_info_p = (struct ia_info *)(&(option_p[0]));
      ia_p->iaid.ia.dh6_ia_t1 = ntohl(ia_info_p->dh6_ia_t1);
      ia_p->iaid.ia.dh6_ia_t2 = ntohl(ia_info_p->dh6_ia_t2);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("T1:%u T2:%u\n", ia_p->iaid.ia.dh6_ia_t1, ia_p->iaid.ia.dh6_ia_t2));

      /* IA_NA option-len : 12 + length of IA_NA-options field */
      if (op_len > 12) {
        idx = 12;
        while (idx <= (op_len - DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN)) {
          op = (u16_t)((option_p[idx] << 8) | option_p[idx + 1]);
          len = (u16_t)((option_p[idx + 2] << 8) | option_p[idx + 3]);

          if (DHCP6_OPTION_IAADDR == op) {
            if (len < 24) {
              DHCP6_STATS_INC(dhcp6.proterr);
              DHCP6_STATS_INC(dhcp6.drop);
              /* option-len 24 + length of IAaddr-options field. */
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("invalid len(%u) of IAADDR\n", len));
              goto exit;
            }
            /* IAADDR option-len : 24 + length of IAaddr-options field */
            if (len > 24) {
              u16_t sub_idx, sub_op, sub_len;
              sub_idx = (u16_t)(idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + 24);
              if (sub_idx < idx || sub_idx > (op_len - DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN)) {
                /* length mismatch */
                DHCP6_STATS_INC(dhcp6.lenerr);
                DHCP6_STATS_INC(dhcp6.drop);
                LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("length mismatch sub_idx : %u, idx : %u, op_len : %u\n",
                  sub_idx, idx, op_len));
                goto exit;
              }
              /* the sub option of IAADDR only can be Status Code */
              sub_op = (u16_t)((option_p[sub_idx] << 8) | option_p[sub_idx + 1]);
              sub_len = (u16_t)((option_p[sub_idx + 2] << 8) | option_p[sub_idx + 3]);
              /* sub_op filed and sub_len filed length is 4 */
              if ((u16_t)(sub_idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + sub_len) < sub_idx ||
                  (u16_t)(sub_idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + sub_len) > op_len) {
                /* length mismatch */
                DHCP6_STATS_INC(dhcp6.lenerr);
                DHCP6_STATS_INC(dhcp6.drop);
                LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("length mismatch sub_idx : %u, sub_len : %u, op_len : %u\n",
                  sub_idx, sub_len, op_len));
                goto exit;
              }
              /* Status Code len must >= 2 */
              if ((DHCP6_OPTION_STATUS_CODE == sub_op) && (sub_len >= 2)) {
                status_code = (u16_t)((option_p[sub_idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN] << 8) |
                  option_p[sub_idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + 1]);
                if (DHCP6_STATUS_NOADDRSAVAIL == status_code) {
                  /* NoAddrsAvail */
                  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("have status code : NoAddrsAvail of IAADDR\n"));
                  goto exit;
                } else if (DHCP6_STATUS_UNSPECFAIL == status_code) {
                  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("have status code : DHCP6_STATUS_UNSPECFAIL of IA_NA\n"));
                  goto exit;
                }
              }
            }
            if ((idx + len) > (op_len - DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN)) {
              /* length mismatch */
              DHCP6_STATS_INC(dhcp6.lenerr);
              DHCP6_STATS_INC(dhcp6.drop);
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("length mismatch idx : %u, len : %u, op_len : %u\n",
                idx, len, op_len));
              goto exit;
            }

            iaaddr_p = (struct dhcp6opt_iaaddr *)(&(option_p[idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN]));
            NETIF_FOREACH(netif) {
              /* check if the address is being used by us */
              addr_idx_match = netif_get_ip6_addr_match(netif, &iaaddr_p->dh6_iaaddr_addr);
              if ((addr_idx_match >= 0) && ((netif != netif_p) || addr_idx_match != addr_idx)) {
                LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("error: the addres is already being used by us\n"));
                dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_DECLINE, "dhcp6_handle_reply");
                dhcp6_set_timeout_params(dhcp6_p);
                dhcp6_decline(netif_p, dhcp6_p);
                goto exit;
              }
            }

            ia_p->iaaddr.dh6_iaaddr_addr = iaaddr_p->dh6_iaaddr_addr;
            ia_p->iaaddr.dh6_iaaddr_preferred = ntohl(iaaddr_p->dh6_iaaddr_preferred);
            ia_p->iaaddr.dh6_iaaddr_valid = ntohl(iaaddr_p->dh6_iaaddr_valid);
            if (ia_p->iaaddr.dh6_iaaddr_preferred > ia_p->iaaddr.dh6_iaaddr_valid) {
              /* A client discards any addresses for which the preferred lifetime is greater than the valid lifetime. */
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE,
                          ("error: the preferred lifetime is greater than the valid lifetime\n"));
              goto exit;
            }

#if LWIP_IPV6_ADDRESS_LIFETIMES
            netif_ip6_addr_set_valid_life(netif_p, addr_idx, ia_p->iaaddr.dh6_iaaddr_valid);
            netif_ip6_addr_set_pref_life(netif_p, addr_idx, ia_p->iaaddr.dh6_iaaddr_preferred);
#endif
            if (LWIP_IS_DAD_ENABLED(netif_p)) {
              netif_ip6_addr_set_state(netif_p, addr_idx, IP6_ADDR_TENTATIVE);
              /* as addr state is IP6_ADDR_TENTATIVE, NS will be sent via nd6_tmr->nd6_send_ns */
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("IA_NA addr assigned, will do DAD\n"));
            } else {
              netif_ip6_addr_set_state(netif_p, addr_idx, IP6_ADDR_PREFERRED);
              /* as addr state is IP6_ADDR_PREFERRED, NA will be sent */
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("IA_NA addr assigned, will send NA\n"));
            }
            netif_ip6_addr_set_parts(netif_p, addr_idx,
                                     iaaddr_p->dh6_iaaddr_addr.addr[0], iaaddr_p->dh6_iaaddr_addr.addr[1],
                                     iaaddr_p->dh6_iaaddr_addr.addr[2], iaaddr_p->dh6_iaaddr_addr.addr[3]);
          } else if (op == DHCP6_OPTION_STATUS_CODE) {
            if (len < 2) {
              /* length of option Status Code must be not less than 2 */
              DHCP6_STATS_INC(dhcp6.lenerr);
              DHCP6_STATS_INC(dhcp6.drop);
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("invalid option STATUS_CODE len %u\n", len));
              goto exit;
            }
            /* skip the field option code and length code to get status_code */
            status_code = (u16_t)((option_p[idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN] << 8) |
                                  option_p[idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN + 1]);
            if (status_code == DHCP6_STATUS_NOADDRSAVAIL) {
              /* NoAddrsAvail */
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("have status code : NoAddrsAvail of IA_NA\n"));
              goto exit;
            } else if (status_code == DHCP6_STATUS_UNSPECFAIL) {
              LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("have status code : DHCP6_STATUS_UNSPECFAIL of IA_NA\n"));
              goto exit;
            }
          }
          /*
           * plus 4 to skip the field option code and length code, because (idx <= (op_len- 4)), so there
           * is no need to check idx_last overflow
           */
          idx_last = (u16_t)(idx + DHCP6_OPTION_IAADDR_SUBOP_LEN_MIN);
          idx = (u16_t)(idx_last + len);
          if (idx < idx_last) {
            /* length mismatch */
            DHCP6_STATS_INC(dhcp6.lenerr);
            DHCP6_STATS_INC(dhcp6.drop);
            LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("length mismatch idx_last : %u, len : %u\n", idx_last, len));
            goto exit;
          }
        }
      }
    }
  }

#if DHCP6_ENABLE_UNICAST_SUPPORT
  if (dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_SERVER_ID) &&
      dhcp6_option_given(dhcp6_p, DHCP6_OPTION_IDX_UNICAST)) {
    op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_SERVER_ID);
    dhcp6_option_free_content(option_p);
    option_p = dhcp6_option_extract_content(p_msg_in, dhcp6_p, DHCP6_OPTION_IDX_SERVER_ID);
    if (option_p == NULL) {
      DHCP6_STATS_INC(dhcp6.lenerr);
      DHCP6_STATS_INC(dhcp6.drop);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("len : %u\n", op_len));
      goto exit;
    }
    dhcp6_serverinfo_p = dhcp6_find_server(dhcp6_p, option_p, op_len);
    if (dhcp6_serverinfo_p != NULL) {
      op_len = dhcp6_get_option_length(dhcp6_p, DHCP6_OPTION_IDX_UNICAST);
      if (op_len == sizeof(ip6_addr_t)) {
        dhcp6_option_free_content(option_p);
        option_p = dhcp6_option_extract_content(p_msg_in, dhcp6_p, DHCP6_OPTION_IDX_UNICAST);
        if (option_p == NULL) {
          DHCP6_STATS_INC(dhcp6.lenerr);
          DHCP6_STATS_INC(dhcp6.drop);
          LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("len : %u\n", op_len));
          goto exit;
        }
        (void)memcpy_s(&dhcp6_serverinfo_p->unicast_addr, op_len, option_p, op_len);
        dhcp6_serverinfo_p->unicast = 1;
      }
    }
  }
#endif

  if (state == DHCP6_STATE_STATEFUL_RELEASE) {
    dhcp6_post_release_cleanup(dhcp6_p);
    /* Don't need timer any more. */
    dhcp6_p->request_timeout = 0;
    dhcp6_set_state(dhcp6_p, DHCP6_STATE_OFF, "dhcp6_handle_reply");
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("receive Reply for Release\n"));
  } else if (state ==  DHCP6_STATE_STATEFUL_INFOREQ) {
    /* not supported yet. */
  } else if (state ==  DHCP6_STATE_STATEFUL_DECLINE) {
    dhcp6_request_next_server(netif_p, dhcp6_p);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("receive Reply for Decline\n"));
  } else if (addr_idx != LWIP_IPV6_NUM_ADDRESSES) {
    ia_t *ia_p = NULL;
    dhcp6_set_state(dhcp6_p,  DHCP6_STATE_STATEFUL_IDLE, "dhcp6_handle_reply");
    ia_p = &(dhcp6_p->ia[addr_idx]);

    dhcp6_p->request_timeout = (u16_t)((ia_p->iaid.ia.dh6_ia_t1 * MS_PER_SECOND + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                ("dhcp6_handle_reply(): set request timeout %"U32_F" msecs\n", ia_p->iaid.ia.dh6_ia_t1*1000));
  }

exit:
  dhcp6_option_free_content(option_p);
}

void
dhcp6_dad_handle(struct netif *netif_p, const ip6_addr_t *target_address)
{
  struct dhcp6 *dhcp6_p = netif_dhcp6_data(netif_p);
  ia_t *ia_p = NULL;
  s8_t addr_idx;

  if ((dhcp6_p == NULL) || (target_address == NULL)) {
    return;
  }

  if (dhcp6_p->current_serv == NULL) {
    return;
  }

  addr_idx = dhcp6_p->addr_idx;
  if (addr_idx == LWIP_IPV6_NUM_ADDRESSES) {
    return;
  }
  ia_p = &(dhcp6_p->ia[addr_idx]);
  if (ip6_addr_cmp(&(ia_p->iaaddr.dh6_iaaddr_addr), target_address)) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("receive NA for request address from others\n"));
    dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_DECLINE, "dhcp6_dad_handle");
    dhcp6_set_timeout_params(dhcp6_p);
    dhcp6_decline(netif_p, dhcp6_p);
  }

  return;
}

static void
dhcp6_solicit_to_request(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  dhcp6_serverinfo_t *dhcp6_serverinfo_p = NULL;

  dhcp6_serverinfo_p = dhcp6_serverinfo_find_active(dhcp6_p->serv_list);
  if (dhcp6_serverinfo_p == NULL) {
    dhcp6_solicit(netif_p, dhcp6_p);
  } else {
    dhcp6_p->current_serv = dhcp6_serverinfo_p;
    dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_REQUEST, "dhcp6_solicit_to_request");
    dhcp6_set_timeout_params(dhcp6_p);
    dhcp6_request(netif_p, dhcp6_p);
  }

  return;
}

static const u16_t*
dhcp6_get_option_list(u8_t message_type)
{
  static u16_t dhcp6_solicit_option_list[] = {
    DHCP6_OPTION_ELAPSED_TIME, DHCP6_OPTION_CLIENTID, DHCP6_OPTION_IA_NA, DHCP6_OPTION_ORO, 0
  };
  static u16_t dhcp6_request_option_list[] = {
    DHCP6_OPTION_ELAPSED_TIME, DHCP6_OPTION_CLIENTID, DHCP6_OPTION_IA_NA, DHCP6_OPTION_SERVERID, DHCP6_OPTION_ORO, 0
  };
  static u16_t dhcp6_renew_option_list[] = {
    DHCP6_OPTION_ELAPSED_TIME, DHCP6_OPTION_CLIENTID, DHCP6_OPTION_IA_NA, DHCP6_OPTION_SERVERID, DHCP6_OPTION_ORO, 0
  };
  static u16_t dhcp6_rebind_option_list[] = {
    DHCP6_OPTION_ELAPSED_TIME, DHCP6_OPTION_CLIENTID, DHCP6_OPTION_IA_NA, DHCP6_OPTION_ORO, 0
  };
  static u16_t dhcp6_release_option_list[] = {
    DHCP6_OPTION_ELAPSED_TIME, DHCP6_OPTION_CLIENTID, DHCP6_OPTION_IA_NA, DHCP6_OPTION_SERVERID, 0
  };
  static u16_t dhcp6_decline_option_list[] = {
    DHCP6_OPTION_ELAPSED_TIME, DHCP6_OPTION_CLIENTID, DHCP6_OPTION_IA_NA, DHCP6_OPTION_SERVERID, 0
  };

  switch (message_type) {
    case DHCP6_SOLICIT:
      return dhcp6_solicit_option_list;
    case DHCP6_REQUEST:
      return dhcp6_request_option_list;
    case DHCP6_RENEW:
      return dhcp6_renew_option_list;
    case DHCP6_REBIND:
      return dhcp6_rebind_option_list;
    case DHCP6_RELEASE:
      return dhcp6_release_option_list;
    case DHCP6_DECLINE:
      return dhcp6_decline_option_list;
    default:
      return NULL;
  }
}

static u8_t
dhcp6_options_list_check(const u16_t *option_list, u16_t option)
{
  u32_t i = 0;
  for (i = 0; option_list[i] && option_list[i] != option; i++) {
    ;
  }
  return (option_list[i] != 0);
}

static void
dhcp6_stateful_send(struct netif *netif_p, struct dhcp6 *dhcp6_p, u8_t message_type)
{
  const u16_t requested_options[] = {DHCP6_OPTION_DNS_SERVERS, DHCP6_OPTION_DOMAIN_LIST, DHCP6_OPTION_SNTP_SERVERS};
  struct pbuf *p_out = NULL;
  u16_t options_out_len;
  u16_t opt_len_alloc = 0;
  u16_t val_16;
  const u16_t *option_list = NULL;
  u32_t i = 0;
  opt_comm_t *serv_id = NULL;
  opt_comm_t *ia_info = NULL;
  u8_t *ia_option = NULL;
  u16_t ia_option_length = 0;
  u32_t elapsed_time;
  u32_t remaining_timeout = 0;
  s8_t addr_idx = dhcp6_p->addr_idx;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE,
              ("dhcp6_stateful_send: %s\n", DHCP6_STATEFUL_MESSAGE_TYPE_TO_STRING(message_type)));

  option_list = dhcp6_get_option_list(message_type);
  if (option_list == NULL) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("Unsupported message\n"));
    return;
  }

  if ((dhcp6_p->current_serv == NULL) && dhcp6_options_list_check(option_list, DHCP6_OPTION_SERVERID)) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("DHCPv6 Server is not available\n"));
    return;
  }

  while (option_list[i]) {
    switch (option_list[i]) {
      case DHCP6_OPTION_ELAPSED_TIME:
        opt_len_alloc = (u16_t)(opt_len_alloc + DHCP6_OPT_HEADER_LEN + DHCPv6_ELAPSED_TIME_VALUE_BYTES);
        break;
      case DHCP6_OPTION_CLIENTID:
        opt_len_alloc = (u16_t)(opt_len_alloc + DHCP6_OPT_HEADER_LEN + dhcp6_p->duid.duid_len);
        break;
      case DHCP6_OPTION_IA_NA:
        /* prepare ia_option and length here */
        if (dhcp6_p->current_serv) {
          ia_info = dhcp6_server_find_opt(dhcp6_p->current_serv, DHCP6_OPTION_IA_NA);
          if (ia_info == NULL) {
            LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("ia_info not found\n"));
            return;
          }
          ia_option = ia_info->opt_start;
          ia_option_length = ia_info->opt_len;
        } else {
          LWIP_ASSERT("dhcp6_addr_idx is error", addr_idx != LWIP_IPV6_NUM_ADDRESSES);
          ia_option = (u8_t *)&((dhcp6_p->ia[addr_idx]).iaid.ia);
          ia_option_length = (dhcp6_p->ia[addr_idx]).iaid.iaid_len;
        }
        opt_len_alloc = (u16_t)(opt_len_alloc + DHCP6_OPT_HEADER_LEN + ia_option_length);
        break;
      case DHCP6_OPTION_SERVERID:
        if (dhcp6_p->current_serv) {
          serv_id = dhcp6_server_find_opt(dhcp6_p->current_serv, DHCP6_OPTION_SERVERID);
        }

        if (serv_id == NULL) {
          LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("serv_id not found\n"));
          return;
        }
        opt_len_alloc = (u16_t)(opt_len_alloc + DHCP6_OPT_HEADER_LEN + serv_id->opt_len);
        break;
      case DHCP6_OPTION_ORO:
        opt_len_alloc = (u16_t)(opt_len_alloc + DHCP6_OPT_HEADER_LEN + sizeof(requested_options));
        break;
      default:
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("Unsupported DHCPv6 Option (%u)\n", option_list[i]));
        return;
    }
    i++;
  }

  /* create and initialize the DHCP message header */
  p_out = dhcp6_create_msg(netif_p, dhcp6_p, message_type, opt_len_alloc, &options_out_len);
  if (p_out != NULL) {
    err_t err;
    struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
    u8_t *options = (u8_t *)(msg_out + 1);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE,
                ("DHCP6_%s: making request\n", DHCP6_STATEFUL_MESSAGE_TYPE_TO_STRING(message_type)));

    // DHCP6_OPTION_ELAPSED_TIME
    if (dhcp6_p->tries == 0) {
      elapsed_time = 0;
      val_16 = htons(0);
    } else {
      elapsed_time = dhcp6_retransmission_duration(dhcp6_p->duration_start);
      /* elapsed-time is expressed in hundredths of a second (10^-2 seconds), max is 0xffff */
      /*
       * when (dhcp6_p->tries == 255), elapsed_time MUST be greater than 0xffff, but be smaller than (u32_t)(-1).
       * because sys_now() may overflow more than 1 time, we use (dhcp6_p->tries == 255) to indicate that value of
       * OPTION_ELAPSED_TIME have reach the maximum, instead of that value of OPTION_ELAPSED_TIME reverse while
       * sys_now() overflow.
       */
      if (dhcp6_p->tries == UCHAR_MAX) {
        val_16 = htons((u16_t)(0xffff));
      } else {
        if ((u32_t)(elapsed_time / 100) > (u32_t)(0x0000ffff)) {
          val_16 = htons((u16_t)(0xffff));
        } else {
          val_16 = htons((u16_t)(elapsed_time / 100));
        }
      }
    }
    if (dhcp6_p->MRD && (elapsed_time < dhcp6_p->MRD)) {
      remaining_timeout = dhcp6_p->MRD - elapsed_time;
    } else {
      remaining_timeout = 0;
    }

    i = 0;
    while (option_list[i]) {
      switch (option_list[i]) {
        case DHCP6_OPTION_ELAPSED_TIME:
          /* DHCP6_OPTION_ELAPSED_TIME option len is 2 */
          options_out_len = dhcp6_option_append(options_out_len, options, DHCP6_OPTION_ELAPSED_TIME,
                                                2, (u8_t *)(&(val_16)));
          break;
        case DHCP6_OPTION_CLIENTID:
          options_out_len = dhcp6_option_append(options_out_len, options, DHCP6_OPTION_CLIENTID,
                                                dhcp6_p->duid.duid_len, dhcp6_p->duid.duid_id);
          break;
        case DHCP6_OPTION_IA_NA:
          /* ia_option_length and ia_option will be set properly in the aboved 'switch' block */
          options_out_len = dhcp6_option_append(options_out_len, options, DHCP6_OPTION_IA_NA,
                                                ia_option_length, ia_option);
          break;
        case DHCP6_OPTION_SERVERID:
          /* serv_id will be set in the above 'switch' block */
          if (serv_id) {
            options_out_len = dhcp6_option_append(options_out_len, options, DHCP6_OPTION_SERVERID,
                                                  serv_id->opt_len, serv_id->opt_start);
          }
          break;
        case DHCP6_OPTION_ORO:
          options_out_len = dhcp6_option_optionrequest(options_out_len, options, requested_options,
                                                       LWIP_ARRAYSIZE(requested_options), p_out->len);
          break;
        default:
          break;
      }
      i++;
    }

    LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif_p, dhcp6_p, dhcp6_p->state, msg_out,
      message_type, options_out_len, p_out->len);

    dhcp6_msg_finalize(options_out_len, p_out);

#if DHCP6_ENABLE_UNICAST_SUPPORT
    if (dhcp6_p->current_serv && dhcp6_p->current_serv->unicast) {
      ip_addr_t src_ip;
      /* Offset to IAID */
      IP_ADDR6(&src_ip,
               ia_option[4 + 0],
               ia_option[4 + 1],
               ia_option[4 + 2],
               ia_option[4 + 3]);

      DHCP6_STATS_INC(dhcp6.xmit);
      err = udp_sendto_if_src(dhcp6_p->pcb, p_out, &dhcp6_p->current_serv->unicast_addr, DHCP6_SERVER_PORT,
                              netif_p, &src_ip);
    } else {
#endif
      DHCP6_STATS_INC(dhcp6.xmit);
      err = udp_sendto_if(dhcp6_p->pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif_p);
#if DHCP6_ENABLE_UNICAST_SUPPORT
    }
#endif
    (void)pbuf_free(p_out);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                ("DHCP6_%s -> %d\n", DHCP6_STATEFUL_MESSAGE_TYPE_TO_STRING(message_type), (int)err));
    LWIP_UNUSED_ARG(err);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("dhcp6_stateful_send: could not allocate DHCP6 msg\n"));
  }
  if (dhcp6_p->tries < UCHAR_MAX) {
    dhcp6_p->tries++;
  }

  if (dhcp6_p->tries == 1) {
    dhcp6_p->RT = dhcp6_p->IRT + (u32_t)(dhcp6_p->IRT * DHCPV6_RT_RAND);
  } else {
    dhcp6_p->RT = (dhcp6_p->RT << 1) + (u32_t)(dhcp6_p->RT * DHCPV6_RT_RAND);
  }

  if (dhcp6_p->MRT != 0 && dhcp6_p->RT > dhcp6_p->MRT) {
    /* [RFC3315 Section 14 ] */
    dhcp6_p->RT = dhcp6_p->MRT + (u32_t)(dhcp6_p->MRT * DHCPV6_RT_RAND);
  }
  if (dhcp6_p->MRD != 0 && remaining_timeout && (dhcp6_p->RT > remaining_timeout)) {
    dhcp6_p->RT = remaining_timeout;
  }
  dhcp6_p->request_timeout = (u16_t)((dhcp6_p->RT + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
              ("dhcp6_stateful_request_timeout(): set request timeout %"U32_F" msecs\n", dhcp6_p->RT));

  return;
}

static void
dhcp6_solicit(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  dhcp6_stateful_send(netif_p, dhcp6_p, DHCP6_SOLICIT);
}

static void
dhcp6_request(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  dhcp6_stateful_send(netif_p, dhcp6_p, DHCP6_REQUEST);
}

static void
dhcp6_renew(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  dhcp6_stateful_send(netif_p, dhcp6_p, DHCP6_RENEW);
}

static void
dhcp6_rebind(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  dhcp6_stateful_send(netif_p, dhcp6_p, DHCP6_REBIND);
}

static void
dhcp6_release(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  dhcp6_stateful_send(netif_p, dhcp6_p, DHCP6_RELEASE);
}

static void
dhcp6_request_next_server(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  dhcp6_serverinfo_t *dhcp6_serverinfo_p = NULL;

  dhcp6_serverinfo_p = dhcp6_p->current_serv;
  dhcp6_p->current_serv = NULL;
  dhcp6_serverinfo_p->active = 0;
  dhcp6_clear_ia(dhcp6_p);

  dhcp6_serverinfo_p = dhcp6_serverinfo_find_active(dhcp6_p->serv_list);
  if (dhcp6_serverinfo_p == NULL) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("no more active server, solicit again\n"));
    dhcp6_serverinfo_free_list(&dhcp6_p->serv_list);
    dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_SOLICIT, "dhcp6_request_next_server");
    dhcp6_set_timeout_params(dhcp6_p);
    dhcp6_solicit(netif_p, dhcp6_p);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("request another active server\n"));
    dhcp6_p->current_serv = dhcp6_serverinfo_p;
    dhcp6_set_state(dhcp6_p, DHCP6_STATE_STATEFUL_REQUEST, "dhcp6_request_next_server");
    dhcp6_set_timeout_params(dhcp6_p);
    dhcp6_request(netif_p, dhcp6_p);
  }

  return;
}

static void
dhcp6_decline(struct netif *netif_p, struct dhcp6 *dhcp6_p)
{
  dhcp6_stateful_send(netif_p, dhcp6_p, DHCP6_DECLINE);
}
#endif /* LWIP_IPV6_DHCP6_STATEFUL */

/** This function is called from nd6 module when an RA messsage is received
 * It triggers DHCPv6 requests (if enabled).
 */
void
dhcp6_nd6_ra_trigger(struct netif *netif, u8_t managed_addr_config, u8_t other_config)
{
  struct dhcp6 *dhcp6;

  LWIP_ASSERT("netif != NULL", netif != NULL);
  dhcp6 = netif_dhcp6_data(netif);

  LWIP_UNUSED_ARG(managed_addr_config);
  LWIP_UNUSED_ARG(other_config);
  LWIP_UNUSED_ARG(dhcp6);

#if LWIP_IPV6_DHCP6_STATELESS
  if (dhcp6 != NULL) {
    if (dhcp6_stateless_enabled(dhcp6)) {
      if (other_config) {
        dhcp6_request_config(netif, dhcp6);
      } else {
        dhcp6_abort_config_request(dhcp6);
      }
    }
  }
#endif /* LWIP_IPV6_DHCP6_STATELESS */
}

/**
 * Parse the DHCPv6 message and extract the DHCPv6 options.
 *
 * Extract the DHCPv6 options (offset + length) so that we can later easily
 * check for them or extract the contents.
 */
static err_t
dhcp6_parse_reply(struct pbuf *p, struct dhcp6 *dhcp6)
{
  u16_t offset;
  u16_t offset_max;
  u16_t options_idx;
  u8_t op_len_buf[DHCP6_OPT_HEADER_LEN];
  u8_t *op_len = NULL;
  u16_t op;
  u16_t len;
  u16_t val_offset;
  struct dhcp6_msg *msg_in = NULL;

  LWIP_UNUSED_ARG(dhcp6);

  /* clear received options */
  dhcp6_clear_all_options(dhcp6);
  msg_in = (struct dhcp6_msg *)p->payload;

  /* parse options */

  options_idx = sizeof(struct dhcp6_msg);
  /* parse options to the end of the received packet */
  offset_max = p->tot_len;

  offset = options_idx;
  /* at least 4 byte to read? */
  while ((offset + DHCP6_OPT_HEADER_LEN <= offset_max)) {
    val_offset = (u16_t)(offset + DHCP6_OPT_HEADER_LEN);
    if (val_offset < offset) {
      /* overflow */
      DHCP6_STATS_INC(dhcp6.lenerr);
      DHCP6_STATS_INC(dhcp6.drop);
      return ERR_BUF;
    }
    /* copy option + length, might be split accross pbufs */
    op_len = (u8_t *)pbuf_get_contiguous(p, op_len_buf, DHCP6_OPT_HEADER_LEN, DHCP6_OPT_HEADER_LEN, offset);
    if (op_len == NULL) {
      /* failed to get option and length */
      DHCP6_STATS_INC(dhcp6.lenerr);
      DHCP6_STATS_INC(dhcp6.drop);
      return ERR_VAL;
    }
    op = (op_len[0] << 8) | op_len[1];
    len = (op_len[2] << 8) | op_len[3];
    offset = val_offset + len;
    if (offset < val_offset) {
      /* overflow */
      DHCP6_STATS_INC(dhcp6.lenerr);
      DHCP6_STATS_INC(dhcp6.drop);
      return ERR_BUF;
    }
    if (offset > offset_max) {
      /* length mismatch */
      DHCP6_STATS_INC(dhcp6.lenerr);
      DHCP6_STATS_INC(dhcp6.drop);
      return ERR_BUF;
    }

    switch (op) {
      case (DHCP6_OPTION_CLIENTID):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_CLI_ID);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_CLI_ID, val_offset, len);
        break;
      case (DHCP6_OPTION_SERVERID):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_SERVER_ID);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_SERVER_ID, val_offset, len);
        break;
#if LWIP_IPV6_DHCP6_STATEFUL
      case (DHCP6_OPTION_IA_NA):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_IA_NA);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_IA_NA, val_offset, len);
        break;
      case (DHCP6_OPTION_PREFERENCE):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_PREFERENCE);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_PREFERENCE, val_offset, len);
        break;
#if DHCP6_ENABLE_UNICAST_SUPPORT
      case (DHCP6_OPTION_UNICAST):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_UNICAST);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_UNICAST, val_offset, len);
        break;
#endif
      case (DHCP6_OPTION_STATUS_CODE):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_STATUS_CODE);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_STATUS_CODE, val_offset, len);
        break;
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
      case (DHCP6_OPTION_DNS_SERVERS):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER, val_offset, len);
        break;
      case (DHCP6_OPTION_DOMAIN_LIST):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_DOMAIN_LIST);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_DOMAIN_LIST, val_offset, len);
        break;
#endif /* LWIP_DHCP6_PROVIDE_DNS_SERVERS */
#if LWIP_DHCP6_GET_NTP_SRV
      case (DHCP6_OPTION_SNTP_SERVERS):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER, val_offset, len);
        break;
#endif /* LWIP_DHCP6_GET_NTP_SRV*/
      default:
        LWIP_DEBUGF(DHCP6_DEBUG, ("skipping option %"U16_F" in options\n", op));
        LWIP_HOOK_DHCP6_PARSE_OPTION(ip_current_netif(), dhcp6, dhcp6->state, msg_in,
          msg_in->msgtype, op, len, q, val_offset);
        break;
    }
  }
  return ERR_OK;
}

static void
dhcp6_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  struct netif *netif = ip_current_input_netif();
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
  struct dhcp6_msg *reply_msg = (struct dhcp6_msg *)p->payload;
  u8_t msg_type;
  u32_t xid;

  LWIP_UNUSED_ARG(arg);

  DHCP6_STATS_INC(dhcp6.recv);

  /* Caught DHCPv6 message from netif that does not have DHCPv6 enabled? -> not interested */
  if ((dhcp6 == NULL) || (dhcp6->pcb == NULL)) {
    DHCP6_STATS_INC(dhcp6.drop);
    goto free_pbuf_and_return;
  }

  LWIP_ERROR("invalid server address type", IP_IS_V6(addr), goto free_pbuf_and_return;);

#ifdef LWIP_DEBUG
  char buf[IPADDR_STRLEN_MAX];
  (void)ipaddr_ntoa_r(addr, buf, IPADDR_STRLEN_MAX);
#endif
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_recv(pbuf = %p) from DHCPv6 server %s port %"U16_F"\n", (void *)p,
              buf, port));
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("pbuf->len = %"U16_F"\n", p->len));
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("pbuf->tot_len = %"U16_F"\n", p->tot_len));
  /* prevent warnings about unused arguments */
  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(addr);
  LWIP_UNUSED_ARG(port);

  if (p->len < sizeof(struct dhcp6_msg)) {
    DHCP6_STATS_INC(dhcp6.lenerr);
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("DHCPv6 reply message or pbuf too short\n"));
    goto free_pbuf_and_return;
  }

  /* match transaction ID against what we expected */
  xid = reply_msg->transaction_id[0] << 16;
  xid |= reply_msg->transaction_id[1] << 8;
  xid |= reply_msg->transaction_id[2];
  if (xid != dhcp6->xid) {
    DHCP6_STATS_INC(dhcp6.drop);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                ("transaction id mismatch reply_msg->xid(%"X32_F")!= dhcp6->xid(%"X32_F")\n", xid, dhcp6->xid));
    goto free_pbuf_and_return;
  }
  /* option fields could be unfold? */
  if (dhcp6_parse_reply(p, dhcp6) != ERR_OK) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("problem unfolding DHCPv6 message - too short on memory?\n"));
    goto free_pbuf_and_return;
  }

  /* read DHCP message type */
  msg_type = reply_msg->msgtype;
  /* message type is DHCP6 REPLY? */
  if (msg_type == DHCP6_REPLY) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("DHCP6_REPLY received\n"));
#if LWIP_IPV6_DHCP6_STATELESS
    /* in info-requesting state? */
    if (dhcp6->state == DHCP6_STATE_REQUESTING_CONFIG) {
      dhcp6_set_state(dhcp6, DHCP6_STATE_STATELESS_IDLE, "dhcp6_recv");
      dhcp6_handle_config_reply(netif, p);
    } else
#endif /* LWIP_IPV6_DHCP6_STATELESS */
    {
#if LWIP_IPV6_DHCP6_STATEFUL
      dhcp6_handle_reply(netif, p);
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
    }
  }
#if LWIP_IPV6_DHCP6_STATEFUL
  else if (msg_type == DHCP6_ADVERTISE) {
    dhcp6_handle_advertise(netif, p);
  }
#endif /* LWIP_IPV6_DHCP6_STATEFUL */

free_pbuf_and_return:
  pbuf_free(p);
}

/**
 * A DHCPv6 request has timed out.
 *
 * The timer that was started with the DHCPv6 request has
 * timed out, indicating no response was received in time.
 */
static void
dhcp6_timeout(struct netif *netif, struct dhcp6 *dhcp6)
{
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_timeout()\n"));

  LWIP_UNUSED_ARG(netif);
  LWIP_UNUSED_ARG(dhcp6);

#if LWIP_IPV6_DHCP6_STATELESS
  /* back-off period has passed, or server selection timed out */
  if (dhcp6->state == DHCP6_STATE_REQUESTING_CONFIG) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_timeout(): retrying information request\n"));
    dhcp6_information_request(netif, dhcp6);
  } else
#endif /* LWIP_IPV6_DHCP6_STATELESS */
  {
#if LWIP_IPV6_DHCP6_STATEFUL
    dhcp6_stateful_timeout(netif, dhcp6);
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
  }
}

/**
 * DHCPv6 timeout handling (this function must be called every 500ms,
 * see @ref DHCP6_TIMER_MSECS).
 *
 * A DHCPv6 server is expected to respond within a short period of time.
 * This timer checks whether an outstanding DHCPv6 request is timed out.
 */
void
dhcp6_tmr(void)
{
  struct netif *netif;
  /* loop through netif's */
  NETIF_FOREACH(netif) {
    struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
    /* only act on DHCPv6 configured interfaces */
    if (dhcp6 != NULL) {
      /* timer is active (non zero), and is about to trigger now */
      if (dhcp6->request_timeout > 1) {
        dhcp6->request_timeout--;
      } else if (dhcp6->request_timeout == 1) {
        dhcp6->request_timeout--;
        /* { dhcp6->request_timeout == 0 } */
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_tmr(): request timeout\n"));
        /* this client's request timeout triggered */
        dhcp6_timeout(netif, dhcp6);
      }
    }
  }
}

#if LWIP_LOWPOWER
#include "lwip/lowpower.h"
u32_t
dhcp6_tmr_tick()
{
  struct netif *netif = NULL;
  u32_t tick = 0;
  /* loop through netif's */
  NETIF_FOREACH(netif) {
    struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
    /* only act on DHCPv6 configured interfaces */
    if ((dhcp6 != NULL) && (dhcp6->request_timeout > 0)) {
      SET_TMR_TICK(tick, dhcp6->request_timeout);
    }
  }
  LOWPOWER_DEBUG(("%s tmr tick: %u\n", __func__, tick));
  return tick;
}
#endif /* LWIP_LOWPOWER */

#endif /* LWIP_IPV6 && LWIP_IPV6_DHCP6 */
