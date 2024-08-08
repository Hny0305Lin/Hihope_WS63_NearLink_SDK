/**
 * @file
 * Dynamic Host Configuration Protocol client
 *
 * @defgroup dhcp4 DHCPv4
 * @ingroup ip4
 * DHCP (IPv4) related functions
 * This is a DHCP client for the lwIP TCP/IP stack. It aims to conform
 * with RFC 2131 and RFC 2132.
 *
 * @todo:
 * - Support for interfaces other than Ethernet (SLIP, PPP, ...)
 *
 * Options:
 * @ref DHCP_COARSE_TIMER_SECS (recommended 60 which is a minute)
 * @ref DHCP_FINE_TIMER_MSECS (recommended 500 which equals TCP coarse timer)
 *
 * dhcp_start() starts a DHCP client instance which
 * configures the interface by obtaining an IP address lease and maintaining it.
 *
 * Use dhcp_release() to end the lease and use dhcp_stop()
 * to remove the DHCP client.
 *
 * @see LWIP_HOOK_DHCP_APPEND_OPTIONS
 * @see LWIP_HOOK_DHCP_PARSE_OPTION
 *
 * @see netifapi_dhcp4
 */

/*
 * Copyright (c) 2001-2004 Leon Woestenberg <leon.woestenberg@gmx.net>
 * Copyright (c) 2001-2004 Axon Digital Design B.V., The Netherlands.
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
 * The Swedish Institute of Computer Science and Adam Dunkels
 * are specifically granted permission to redistribute this
 * source code.
 *
 * Author: Leon Woestenberg <leon.woestenberg@gmx.net>
 *
 */

#include "lwip/opt.h"

#if LWIP_IPV4 && LWIP_DHCP /* don't build if not configured for use in lwipopts.h */

#include "lwip/stats.h"
#include "lwip/mem.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/def.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "lwip/etharp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/prot/iana.h"
#if LWIP_NAT64
#include "lwip/nat64_v4_dhcpc.h"
#include "lwip/nat64_addr.h"
#include "lwip/nat64.h"
#endif
#include <string.h>

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif
#ifndef LWIP_HOOK_DHCP_APPEND_OPTIONS
#define LWIP_HOOK_DHCP_APPEND_OPTIONS(netif, dhcp, state, msg, msg_type, options_len_ptr)
#endif
#ifndef LWIP_HOOK_DHCP_PARSE_OPTION
#define LWIP_HOOK_DHCP_PARSE_OPTION(netif, dhcp, state, msg, msg_type, option, len, pbuf, offset) do { LWIP_UNUSED_ARG(msg); } while(0)
#endif

/** DHCP_CREATE_RAND_XID: if this is set to 1, the xid is created using
 * LWIP_RAND() (this overrides DHCP_GLOBAL_XID)
 */
#ifndef DHCP_CREATE_RAND_XID
#define DHCP_CREATE_RAND_XID        1
#endif

#ifndef DHCP_REQUEST_TIMEOUT_DEFAULT
#define DHCP_REQUEST_TIMEOUT_DEFAULT   10  // 10 second
#endif

/** Default for DHCP_GLOBAL_XID is 0xABCD0000
 * This can be changed by defining DHCP_GLOBAL_XID and DHCP_GLOBAL_XID_HEADER, e.g.
 *  \#define DHCP_GLOBAL_XID_HEADER "stdlib.h"
 *  \#define DHCP_GLOBAL_XID rand()
 */
#ifdef DHCP_GLOBAL_XID_HEADER
#include DHCP_GLOBAL_XID_HEADER /* include optional starting XID generation prototypes */
#endif

/** Holds the decoded option values, only valid while in dhcp_recv.
    @todo: move this into struct dhcp? */
u32_t dhcp_rx_options_val[DHCP_OPTION_IDX_MAX];
/** Holds a flag which option was received and is contained in dhcp_rx_options_val,
    only valid while in dhcp_recv.
    @todo: move this into struct dhcp? */
u8_t  dhcp_rx_options_given[DHCP_OPTION_IDX_MAX];

static u8_t dhcp_discover_request_options[] = {
  DHCP_OPTION_SUBNET_MASK,
  DHCP_OPTION_ROUTER,
  DHCP_OPTION_BROADCAST
#if LWIP_DHCP_PROVIDE_DNS_SERVERS
  , DHCP_OPTION_DNS_SERVER
#endif /* LWIP_DHCP_PROVIDE_DNS_SERVERS */
#if LWIP_DHCP_GET_NTP_SRV
  , DHCP_OPTION_NTP
#endif /* LWIP_DHCP_GET_NTP_SRV */
};

#ifdef DHCP_GLOBAL_XID
static u32_t xid;
static u8_t xid_initialised;
#endif /* DHCP_GLOBAL_XID */

#if LWIP_DHCP_VENDOR_CLASS_IDENTIFIER
static struct vci_info g_vci_info = {{0}, 0};
#endif /* LWIP_DHCP_VENDOR_CLASS_IDENTIFIER */

#define dhcp_option_given(dhcp, idx)          (dhcp_rx_options_given[idx] != 0)
#define dhcp_got_option(dhcp, idx)            (dhcp_rx_options_given[idx] = 1)
#define dhcp_clear_option(dhcp, idx)          (dhcp_rx_options_given[idx] = 0)
#define dhcp_clear_all_options(dhcp)          (memset(dhcp_rx_options_given, 0, sizeof(dhcp_rx_options_given)))
#define dhcp_get_option_value(dhcp, idx)      (dhcp_rx_options_val[idx])
#define dhcp_set_option_value(dhcp, idx, val) (dhcp_rx_options_val[idx] = (val))

static struct udp_pcb *dhcp_pcb;
#if LWIP_DHCP_SUBSTITUTE
static u16_t dhcp_pcb_refcount;
#define DHCP_REF_F "hu"

#if LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS
static u16_t g_dhcp_max_concurrent_num = LWIP_DHCP_MAX_CONCURRENT_REQUESTS_NUM;
#endif
#else
static u8_t dhcp_pcb_refcount;
#define DHCP_REF_F "hhu"
#endif

static err_t dhcp_release_client(struct netif *netif, struct dhcp_client *dhcp);
static err_t dhcp_renew_client(struct netif *netif, struct dhcp_client *dhcp);
static void dhcp_stop_client(struct netif *netif, struct dhcp_client *dhcp);
#if LWIP_DHCP_SUBSTITUTE
static void dhcp_substitute_clients_restart(struct netif *netif, struct dhcp_client *dhcp);
static s32_t dhcp_addr_clients_check(struct dhcp *netif_dhcp, const ip4_addr_t *ipaddr);
#if LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS
static u8_t dhcp_client_request_processing(u8_t state);
static void dhcp_concurrent_limit_wait_timeout(struct netif *netif, struct dhcp_client *dhcp);
static void dhcp_concurrent_num_update(struct dhcp_client *dhcp);
static err_t dhcp_concurrent_start_client(struct dhcp_client *dhcp);
static void dhcp_concurrent_stop_client(struct dhcp_client *dhcp);
static void dhcp_concurrent_start_next_client(struct netif *netif, u8_t all);
#endif
#endif /* LWIP_DHCP_SUBSTITUTE */

#if LWIP_DHCP_SUBSTITUTE_MMBR
static void dhcp_release_check(struct netif *netif, struct dhcp_client *dhcp);
#endif

/* DHCP client state machine functions */
static err_t dhcp_discover(struct netif *netif, struct dhcp_client *dhcp);
static err_t dhcp_select(struct netif *netif, struct dhcp_client *dhcp);
static void dhcp_bind(struct netif *netif, struct dhcp_client *dhcp);
#if LWIP_ARP
static void dhcp_announce(struct netif *netif, struct dhcp_client *dhcp);
#endif
#if DHCP_DOES_ARP_CHECK
static err_t dhcp_decline(struct netif *netif, struct dhcp_client *dhcp);
#endif /* DHCP_DOES_ARP_CHECK */
static err_t dhcp_rebind(struct netif *netif, struct dhcp_client *dhcp);
static err_t dhcp_reboot(struct netif *netif);
static void dhcp_set_state(struct dhcp_client *dhcp, u8_t new_state);

/* receive, unfold, parse and free incoming messages */
static void dhcp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

/* set the DHCP timers */
static void dhcp_timeout(struct netif *netif, struct dhcp_client *dhcp);
static void dhcp_t1_timeout(struct netif *netif, struct dhcp_client *dhcp);
static void dhcp_t2_timeout(struct netif *netif, struct dhcp_client *dhcp);

/* build outgoing messages */
/* create a DHCP message, fill in common headers */
static err_t dhcp_create_msg(struct netif *netif, struct dhcp_client *dhcp, u8_t message_type, u16_t *options_out_len);
/* free a DHCP request */
static void dhcp_delete_msg(struct dhcp_client *dhcp);
/* add a DHCP option (type, then length in bytes) */
static u16_t dhcp_option(u16_t options_out_len, u8_t *options, u8_t option_type, u8_t option_len);
/* add option values */
static u16_t dhcp_option_byte(u16_t options_out_len, u8_t *options, u8_t value);
static u16_t dhcp_option_short(u16_t options_out_len, u8_t *options, u16_t value);
static u16_t dhcp_option_long(u16_t options_out_len, u8_t *options, u32_t value);
#if LWIP_NETIF_HOSTNAME
static u16_t dhcp_option_hostname(u16_t options_out_len, u8_t *options, struct netif *netif);
#endif /* LWIP_NETIF_HOSTNAME */

#if LWIP_DHCP_VENDOR_CLASS_IDENTIFIER
static u16_t dhcp_option_vci(u16_t options_out_len, u8_t *options, const struct netif *netif);
#endif /* LWIP_DHCP_VENDOR_CLASS_IDENTIFIER */

/* always add the DHCP options trailer to end and pad */
static void dhcp_option_trailer(u16_t options_out_len, u8_t *options, struct pbuf *p_out);

/** Ensure DHCP PCB is allocated and bound */
static err_t
dhcp_inc_pcb_refcount(u8_t ifindex)
{
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_inc_pcb_refcount:enter %"DHCP_REF_F"\n", dhcp_pcb_refcount);
  if (dhcp_pcb_refcount == 0) {
    LWIP_ASSERT("dhcp_inc_pcb_refcount(): memory leak", dhcp_pcb == NULL);

    /* allocate UDP PCB */
    dhcp_pcb = udp_new();

    if (dhcp_pcb == NULL) {
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, "dhcp_inc_pcb_refcount:udp new failed\n");
      return ERR_MEM;
    }

#if LWIP_SO_PRIORITY
    dhcp_pcb->priority = LWIP_PKT_PRIORITY_DHCP;
#endif /* LWIP_SO_PRIORITY */

    ip_set_option(dhcp_pcb, SOF_BROADCAST);
    LWIP_UNUSED_ARG(ifindex);

    /* set up local and remote port for the pcb -> listen on all interfaces on all src/dest IPs */
    udp_bind(dhcp_pcb, IP4_ADDR_ANY, LWIP_IANA_PORT_DHCP_CLIENT);
    udp_connect(dhcp_pcb, IP4_ADDR_ANY, LWIP_IANA_PORT_DHCP_SERVER);
    udp_recv(dhcp_pcb, dhcp_recv, NULL);
  }

  dhcp_pcb_refcount++;
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_inc_pcb_refcount:leave %"DHCP_REF_F"\n", dhcp_pcb_refcount);

  return ERR_OK;
}

/** Free DHCP PCB if the last netif stops using it */
static void
dhcp_dec_pcb_refcount(void)
{
  LWIP_ASSERT("dhcp_pcb_refcount(): refcount error", (dhcp_pcb_refcount > 0));
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_dec_pcb_refcount:enter %"DHCP_REF_F"\n", dhcp_pcb_refcount);
  dhcp_pcb_refcount--;

  if (dhcp_pcb_refcount == 0) {
    udp_remove(dhcp_pcb);
    dhcp_pcb = NULL;
  }
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_dec_pcb_refcount:leave %"DHCP_REF_F"\n", dhcp_pcb_refcount);
}

static void
dhcp_get_client_ip(u32_t *cli_ip, struct dhcp_client *dhcp, struct dhcp_state *dhcp_state)
{
  if (ip4_addr_isany_val(dhcp->relay_ip)) {
    DHCP_HOST_TO_IP(*cli_ip, ip_2_ip4(&dhcp->server_ip_addr)->addr, dhcp->offered_sn_mask.addr,
                    dhcp_state->offered_ip_addr);
  } else {
    DHCP_HOST_TO_IP(*cli_ip, dhcp->relay_ip.addr, dhcp->offered_sn_mask.addr,
                    dhcp_state->offered_ip_addr);
  }
}

static void
dhcp_ip_to_mask(ip4_addr_t *server_ip_addr, ip4_addr_t *sn_mask)
{
  u8_t first_octet = ip4_addr1(server_ip_addr);
  if (first_octet <= IPV4_ADDRESS_PREFIX_CLASS_A) {
    ip4_addr_set_u32(sn_mask, PP_HTONL(0xff000000UL));
  } else if (first_octet >= IPV4_ADDRESS_PREFIX_CLASS_C) {
    ip4_addr_set_u32(sn_mask, PP_HTONL(0xffffff00UL));
  } else {
    ip4_addr_set_u32(sn_mask, PP_HTONL(0xffff0000UL));
  }

  return;
}

static err_t
dhcp_mac_to_idx(struct netif *netif, const u8_t *hwaddr, u8_t hwaddr_len, dhcp_num_t *mac_idx)
{
  if ((netif->hwaddr_len == hwaddr_len) && (memcmp(netif->hwaddr, hwaddr, hwaddr_len) == 0)) {
    *mac_idx = LWIP_DHCP_NATIVE_IDX;
    return ERR_OK;
  }

#if LWIP_DHCP_SUBSTITUTE && LWIP_NAT64
  return nat64_entry_mac_to_idx(hwaddr, hwaddr_len, mac_idx);
#else
  return ERR_VAL;
#endif /* LWIP_DHCP_SUBSTITUTE && LWIP_NAT64 */
}

err_t
dhcp_idx_to_mac(struct netif *netif, dhcp_num_t mac_idx,
                u8_t *hwaddr, u8_t *hwaddr_len)
{
  if ((hwaddr == NULL) || (hwaddr_len == NULL)) {
    return ERR_VAL;
  }
  if (mac_idx == LWIP_DHCP_NATIVE_IDX) {
    if (memcpy_s(hwaddr, NETIF_MAX_HWADDR_LEN, netif->hwaddr, NETIF_MAX_HWADDR_LEN) != EOK) {
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("dhcp_idx_to_mac:memcpy failed\n"));
      return ERR_MEM;
    }
    *hwaddr_len = netif->hwaddr_len;
    return ERR_OK;
  }

#if LWIP_DHCP_SUBSTITUTE && LWIP_NAT64
  u8_t hw_len = NETIF_MAX_HWADDR_LEN;
  if (nat64_entry_idx_to_mac(mac_idx, hwaddr, &hw_len) == ERR_OK) {
    *hwaddr_len = hw_len;
    return ERR_OK;
  }
#endif /* LWIP_DHCP_SUBSTITUTE && LWIP_NAT64 */
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("dhcp_idx_to_mac:not find in nat64\n"));
  return ERR_VAL;
}

static err_t
dhcp_client_find_by_mac_idx(struct dhcp_client *dhcp, dhcp_num_t mac_idx, dhcp_num_t *cli_idx)
{
  int i;

  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    if ((dhcp->states)[i].idx == mac_idx) {
      *cli_idx = (dhcp_num_t)i;
      return ERR_OK;
    }
  }

  return ERR_VAL;
}

static err_t
dhcp_client_state_new(struct dhcp_client *dhcp, dhcp_num_t mac_idx, dhcp_num_t *cli_idx)
{
  int i;

  if (dhcp->cli_cnt == DHCP_CLIENT_NUM) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_client_state_new:max cnt\n"));
    return ERR_VAL;
  }

  for (i = 1; i < DHCP_CLIENT_NUM; i++) {
    if ((dhcp->states)[i].idx == 0) {
      *cli_idx = (dhcp_num_t)i;
      (dhcp->states)[i].idx = mac_idx;
      return ERR_OK;
    }
  }

  return ERR_VAL;
}

static void
dhcp_clients_count_update(struct dhcp_client *dhcp)
{
  int i;
  dhcp_num_t cnt = 0;

  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    if ((dhcp->states)[i].hwaddr_len != 0) {
      cnt++;
    }
  }

  dhcp->cli_cnt = cnt;
  return;
}

/**
 * Back-off the DHCP client (because of a received NAK response).
 *
 * Back-off the DHCP client because of a received NAK. Receiving a
 * NAK means the client asked for something non-sensible, for
 * example when it tries to renew a lease obtained on another network.
 *
 * We clear any existing set IP address and restart DHCP negotiation
 * afresh (as per RFC2131 3.2.3).
 *
 * @param netif the netif under DHCP control
 */
static void
dhcp_handle_nak(struct netif *netif, struct dhcp_client *dhcp)
{
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_handle_nak(netif=%p) %s%"U16_F"\n",
              (void *)netif, netif->name, (u16_t)netif->num));
  /* Change to a defined state - set this before assigning the address
     to ensure the callback can use dhcp_supplied_address() */
  dhcp_set_state(dhcp, DHCP_STATE_BACKING_OFF);
  /* remove IP address from interface (must no longer be used, as per RFC2131) */
  if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
    (void)netif_set_addr(netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
  }
  /* We can immediately restart discovery */
  (void)dhcp_discover(netif, dhcp);
}

#if DHCP_DOES_ARP_CHECK
/**
 * Checks if the offered IP address is already in use.
 *
 * It does so by sending an ARP request for the offered address and
 * entering CHECKING state. If no ARP reply is received within a small
 * interval, the address is assumed to be free for use by us.
 *
 * @param netif the netif under DHCP control
 */
static void
dhcp_check(struct netif *netif, struct dhcp_client *dhcp)
{
  err_t result;
  u16_t msecs;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  ip4_addr_t cli_ip;
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_check(netif=%p) %s\n", (void *)netif, netif->name));
  dhcp_set_state(dhcp, DHCP_STATE_CHECKING);
  dhcp_get_client_ip(&cli_ip.addr, dhcp, dhcp_state);
  /* create an ARP query for the offered IP address, expecting that no host
     responds, as the IP address should not be in use. */
  result = etharp_query(netif, &cli_ip, NULL);
  if (result != ERR_OK) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("dhcp_check: could not perform ARP query\n"));
  }
  if (dhcp_state->tries < 255) {
    dhcp_state->tries++;
  }
  msecs = 500;
  dhcp_state->request_timeout = (u16_t)((msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS);
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_check(): set request timeout %"U16_F" msecs\n", msecs));
}
#endif /* DHCP_DOES_ARP_CHECK */

/**
 * Remember the configuration offered by a DHCP server.
 *
 * @param netif the netif under DHCP control
 */
static void
dhcp_handle_offer(struct netif *netif, struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  ip4_addr_t cli_ip;
#if LWIP_DHCP_SUBSTITUTE_MMBR
  dhcp_num_t offered_ip_addr;
#endif
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_handle_offer(netif=%p) %s%"U16_F"\n",
              (void *)netif, netif->name, (u16_t)netif->num));
  /* obtain the server address */
  if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_SERVER_ID)) {
    dhcp_state->request_timeout = 0; /* stop timer */

    ip_addr_set_ip4_u32(&dhcp->server_ip_addr, lwip_htonl(dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_SERVER_ID)));
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_STATE, ("dhcp_handle_offer(): server 0x%08"X32_F"\n",
                ip4_addr_get_u32(ip_2_ip4(&dhcp->server_ip_addr))));
    if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_SUBNET_MASK)) {
      ip4_addr_set_u32(&dhcp->offered_sn_mask, lwip_htonl(dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_SUBNET_MASK)));
    } else {
      dhcp_ip_to_mask(ip_2_ip4(&dhcp->server_ip_addr), &dhcp->offered_sn_mask);
    }
    /* remember offered address */
    ip4_addr_copy(cli_ip, dhcp->msg_in->yiaddr);
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_STATE, ("dhcp_handle_offer(): offer for 0x%08"X32_F"\n",
                ip4_addr_get_u32(&cli_ip)));
#if LWIP_DHCP_SUBSTITUTE_MMBR
    offered_ip_addr = dhcp_state->offered_ip_addr;
#endif
    DHCP_IP_TO_HOST(dhcp_state->offered_ip_addr, cli_ip.addr, dhcp->offered_sn_mask.addr);
#if LWIP_DHCP_SUBSTITUTE_MMBR
    /* if offered IP is not same with the preferred IP, should do duplicate IP check */
    if ((dhcp_state->addr_not_dup_check == lwIP_TRUE) && offered_ip_addr != dhcp_state->offered_ip_addr) {
      dhcp_state->addr_not_dup_check = lwIP_FALSE;
    }
#endif
    ip4_addr_copy(dhcp->relay_ip, dhcp->msg_in->giaddr);
    (void)dhcp_select(netif, dhcp);
  } else {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("dhcp_handle_offer(netif=%p) did not get server ID!\n", (void *)netif));
  }
}

static void
dhcp_fill_options(struct netif *netif, struct dhcp_client *dhcp, u8_t message_type, u32_t opt_flags, u16_t *opts_len)
{
  u8_t i;
  ip4_addr_t cli_ip;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  if (opt_flags & GEN_FLG(MAX_MSG_SIZE)) {
    *opts_len = dhcp_option(*opts_len, dhcp->msg_out->options, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
    u16_t val = (dhcp_state->state == DHCP_STATE_REBOOTING) ?
                (u16_t)DHCP_MAX_MSG_LEN_MIN_REQUIRED : DHCP_MAX_MSG_LEN(netif);
    *opts_len = dhcp_option_short(*opts_len, dhcp->msg_out->options, val);
  }

  if (opt_flags & GEN_FLG(REQUESTED_IP)) {
    /* MUST request the offered IP address */
    *opts_len = dhcp_option(*opts_len, dhcp->msg_out->options, DHCP_OPTION_REQUESTED_IP, 4);
    dhcp_get_client_ip(&cli_ip.addr, dhcp, dhcp_state);
    *opts_len = dhcp_option_long(*opts_len, dhcp->msg_out->options, lwip_ntohl(ip4_addr_get_u32(&cli_ip)));
  }

  *opts_len = dhcp_option(*opts_len, dhcp->msg_out->options, DHCP_OPTION_CLIENT_ID, 1 + dhcp_state->hwaddr_len);
  *opts_len = dhcp_option_byte(*opts_len, dhcp->msg_out->options, dhcp->msg_out->htype);
  for (i = 0; i < dhcp_state->hwaddr_len; i++) {
    *opts_len = dhcp_option_byte(*opts_len, dhcp->msg_out->options, dhcp->hwaddr[i]);
  }

  if (opt_flags & GEN_FLG(PARAMETER_REQUEST_LIST)) {
    *opts_len = dhcp_option(*opts_len, dhcp->msg_out->options, DHCP_OPTION_PARAMETER_REQUEST_LIST,
                            LWIP_ARRAYSIZE(dhcp_discover_request_options));
    for (i = 0; i < LWIP_ARRAYSIZE(dhcp_discover_request_options); i++) {
      *opts_len = dhcp_option_byte(*opts_len, dhcp->msg_out->options, dhcp_discover_request_options[i]);
    }
  }

#if LWIP_NETIF_HOSTNAME
  if ((message_type == DHCP_DISCOVER) ||
      ((message_type == DHCP_REQUEST) && (dhcp_state->state != DHCP_STATE_REBOOTING))) {
    *opts_len = dhcp_option_hostname(*opts_len, dhcp->msg_out->options, netif);
  }
#endif /* LWIP_NETIF_HOSTNAME */

#if LWIP_DHCP_VENDOR_CLASS_IDENTIFIER
  if ((message_type != DHCP_DECLINE) && (message_type != DHCP_RELEASE)) {
    *opts_len = dhcp_option_vci(*opts_len, dhcp->msg_out->options, netif);
  }
#endif /* LWIP_DHCP_VENDOR_CLASS_IDENTIFIER */
  (void)message_type;
  LWIP_HOOK_DHCP_APPEND_OPTIONS(netif, dhcp, DHCP_STATE_REQUESTING, dhcp->msg_out, DHCP_REQUEST, opts_len);
  dhcp_option_trailer(*opts_len, dhcp->msg_out->options, dhcp->p_out);
}

/**
 * Select a DHCP server offer out of all offers.
 *
 * Simply select the first offer received.
 *
 * @param netif the netif under DHCP control
 * @return lwIP specific error (see error.h)
 */
static err_t
dhcp_select(struct netif *netif, struct dhcp_client *dhcp)
{
  err_t result;
  u16_t msecs;
  u16_t options_out_len;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
#if LWIP_DHCP_REQUEST_UNICAST
  ip_addr_t unicast_ip;
#endif

  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_select(netif=%p) %s%"U16_F"\n", (void *)netif, netif->name,
              (u16_t)netif->num));
  dhcp_set_state(dhcp, DHCP_STATE_REQUESTING);

  /* create and initialize the DHCP message header */
  result = dhcp_create_msg(netif, dhcp, DHCP_REQUEST, &options_out_len);
  if (result == ERR_OK) {
    options_out_len = dhcp_option(options_out_len, dhcp->msg_out->options, DHCP_OPTION_SERVER_ID, 4);
    options_out_len = dhcp_option_long(options_out_len, dhcp->msg_out->options,
                                       lwip_ntohl(ip4_addr_get_u32(ip_2_ip4(&dhcp->server_ip_addr))));
    dhcp_fill_options(netif, dhcp, DHCP_REQUEST,
                      GEN_FLG(MAX_MSG_SIZE) | GEN_FLG(REQUESTED_IP) | GEN_FLG(PARAMETER_REQUEST_LIST), &options_out_len);

#if LWIP_DHCP_REQUEST_UNICAST
    if (!ip4_addr_isany_val(dhcp->relay_ip)) {
      ip_addr_set_ip4_u32_val(unicast_ip, ip4_addr_get_u32(&dhcp->relay_ip));
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_select: UNICAST relay\n"));
    } else {
      ip_addr_set_ip4_u32_val(unicast_ip, ip4_addr_get_u32(ip_2_ip4(&dhcp->server_ip_addr)));
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_select: UNICAST serv\n"));
    }
    result = udp_sendto_if_src(dhcp_pcb, dhcp->p_out, &unicast_ip, LWIP_IANA_PORT_DHCP_SERVER, netif, IP4_ADDR_ANY);
#else
    /* send broadcast to any DHCP server */
    result = udp_sendto_if_src(dhcp_pcb, dhcp->p_out, IP_ADDR_BROADCAST, LWIP_IANA_PORT_DHCP_SERVER, netif, IP4_ADDR_ANY);
#endif
    dhcp_delete_msg(dhcp);
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_select: REQUESTING\n"));
  } else {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("dhcp_select: could not allocate DHCP request\n"));
  }
  if (dhcp_state->tries < 255) {
    dhcp_state->tries++;
  }
  msecs = (u16_t)((dhcp_state->tries < 6 ? 1 << dhcp_state->tries : 60) * 1000);
  dhcp_state->request_timeout = (u16_t)((msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS);
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_STATE, ("dhcp_select(): set request timeout %"U16_F" msecs\n", msecs));
  return result;
}

static void
dhcp_client_coarse_tmr(struct netif *netif, struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  if ((dhcp_state->state == DHCP_STATE_OFF)) {
    return;
  }

  /* compare lease time to expire timeout */
  if ((dhcp->t0_timeout != 0) && (++dhcp_state->lease_used >= dhcp->t0_timeout)) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_coarse_tmr(): t0 timeout\n"));
    /* this clients' lease time has expired */
    if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
      (void)dhcp_release(netif);
    } else {
      (void)dhcp_release_client(netif, dhcp);
    }
    (void)dhcp_discover(netif, dhcp);
    /* timer is active (non zero), and triggers (zeroes) now? */
  } else if ((dhcp->t2_timeout != 0) && (dhcp_state->lease_used >= dhcp->t2_timeout)) {
    if ((dhcp_state->re_time == 0) || (dhcp_state->re_time-- != 1)) {
      return;
    }
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_coarse_tmr(): t2 timeout\n"));
    /* this clients' rebind timeout triggered */
    dhcp_t2_timeout(netif, dhcp);
    /* timer is active (non zero), and triggers (zeroes) now */
  } else if ((dhcp->t1_timeout != 0) && (dhcp_state->re_time != 0) && (dhcp_state->re_time-- == 1)) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_coarse_tmr(): t1 timeout\n"));
    /* this clients' renewal timeout triggered */
    dhcp_t1_timeout(netif, dhcp);
  }

  return;
}

static void
dhcp_netif_coarse_tmr(struct netif *netif, struct dhcp *netif_dhcp)
{
  struct dhcp_state *dhcp_state = NULL;
  int i;
  u8_t hwaddr_len;

  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_netif_coarse_tmr()\n"));
  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    netif_dhcp->client.cli_idx = (dhcp_num_t)i;
    dhcp_state = &((netif_dhcp->client.states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    if (dhcp_idx_to_mac(netif, dhcp_state->idx, netif_dhcp->client.hwaddr, &hwaddr_len) != ERR_OK) {
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                       "dhcp_netif_coarse_tmr:idx %u to mac failed\n", dhcp_state->idx);
      continue;
    }
    dhcp_state->hwaddr_len = hwaddr_len;
    dhcp_client_coarse_tmr(netif, &(netif_dhcp->client));
  }

  return;
}

/**
 * The DHCP timer that checks for lease renewal/rebind timeouts.
 * Must be called once a minute (see @ref DHCP_COARSE_TIMER_SECS).
 */
void
dhcp_coarse_tmr(void)
{
  struct netif *netif;
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_coarse_tmr()\n"));
  /* iterate through all network interfaces */
  NETIF_FOREACH(netif) {
    /* only act on DHCP configured interfaces */
    struct dhcp *netif_dhcp = netif_dhcp_data(netif);
    if (netif_dhcp != NULL) {
      dhcp_netif_coarse_tmr(netif, netif_dhcp);
    }
  }
}

#if LWIP_LOWPOWER
#include "lwip/lowpower.h"

static u32_t
dhcp_netif_coarse_tmr_tick(struct dhcp *netif_dhcp)
{
  struct dhcp_state *dhcp_state = NULL;
  struct dhcp_client *client = NULL;
  s32_t i;
  u32_t tick = 0;
  u32_t val;
  u16_t lease_used;

  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    dhcp_state = &((netif_dhcp->client.states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    if ((dhcp_state->state == DHCP_STATE_OFF)) {
      continue;
    }

    client = &(netif_dhcp->client);
    lease_used = dhcp_state->lease_used;
    if (client->t0_timeout > 0) {
      if (client->t0_timeout > lease_used) {
        val = client->t0_timeout - lease_used;
        SET_TMR_TICK(tick, val);
      } else {
        SET_TMR_TICK(tick, 1);
      }
    }

    if (client->t2_timeout > 0) {
      if (client->t2_timeout > lease_used) {
        val = (client->t2_timeout - lease_used);
        SET_TMR_TICK(tick, val);
      } else if (dhcp_state->re_time > 0) {
        val = dhcp_state->re_time;
        SET_TMR_TICK(tick, val);
      } else {
        SET_TMR_TICK(tick, 1);
      }
    }

    if (dhcp_state->re_time > 0) {
      val = dhcp_state->re_time;
      SET_TMR_TICK(tick, val);
    }
  }

  return tick;
}

u32_t
dhcp_coarse_tmr_tick(void)
{
  struct netif *netif = netif_list;
  u32_t tick = 0;
  u32_t val;

  while (netif != NULL) {
    /* only act on DHCP configured interfaces */
    struct dhcp *netif_dhcp = netif_dhcp_data(netif);
    if (netif_dhcp == NULL) {
      /* proceed to next netif */
      netif = netif->next;
      continue;
    }
    val = dhcp_netif_coarse_tmr_tick(netif_dhcp);
    SET_TMR_TICK(tick, val);
    /* proceed to next netif */
    netif = netif->next;
  }

  LOWPOWER_DEBUG(("%s tmr tick: %u\n", __func__, tick));
  return tick;
}

u32_t
dhcp_fine_tmr_tick(void)
{
  struct netif *netif = netif_list;
  struct dhcp_state *dhcp_state = NULL;
  int i;
  u32_t tick = 0;
  u32_t val;

  /* loop through netif's */
  while (netif != NULL) {
    struct dhcp *netif_dhcp = netif_dhcp_data(netif);
    if (netif_dhcp == NULL) {
      netif = netif->next;
      continue;
    }

    for (i = 0; i < DHCP_CLIENT_NUM; i++) {
      dhcp_state = &((netif_dhcp->client.states)[i]);
      if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
        continue;
      }
      if (dhcp_state->request_timeout >= 1) {
        val = dhcp_state->request_timeout;
        SET_TMR_TICK(tick, val);
      }
    }
    /* proceed to next network interface */
    netif = netif->next;
  }
  LOWPOWER_DEBUG(("%s tmr tick: %d\n", __func__, tick));
  return tick;
}
#endif /* LWIP_LOWPOWER */

static void
dhcp_client_fine_tmr(struct netif *netif, struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  /* timer is active (non zero), and is about to trigger now */
  if (dhcp_state->request_timeout > 1) {
    dhcp_state->request_timeout--;
  } else if (dhcp_state->request_timeout == 1) {
    dhcp_state->request_timeout--;
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_fine_tmr(): request timeout\n"));
    /* this client's request timeout triggered */
    dhcp_timeout(netif, dhcp);
  }
  return;
}

static void
dhcp_netif_fine_tmr(struct netif *netif, struct dhcp *netif_dhcp)
{
  struct dhcp_state *dhcp_state = NULL;
  int i;
  u8_t hwaddr_len;
  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    netif_dhcp->client.cli_idx = (dhcp_num_t)i;
    dhcp_state = &((netif_dhcp->client.states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    if (dhcp_idx_to_mac(netif, dhcp_state->idx, netif_dhcp->client.hwaddr, &hwaddr_len) != ERR_OK) {
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                       "dhcp_netif_fine_tmr:idx %u to mac failed\n", dhcp_state->idx);
      continue;
    }
    dhcp_state->hwaddr_len = hwaddr_len;
    dhcp_client_fine_tmr(netif, &(netif_dhcp->client));
  }
  return;
}

/**
 * DHCP transaction timeout handling (this function must be called every 500ms,
 * see @ref DHCP_FINE_TIMER_MSECS).
 *
 * A DHCP server is expected to respond within a short period of time.
 * This timer checks whether an outstanding DHCP request is timed out.
 */
void
dhcp_fine_tmr(void)
{
  struct netif *netif;
  /* loop through netif's */
  NETIF_FOREACH(netif) {
    struct dhcp *netif_dhcp = netif_dhcp_data(netif);
    /* only act on DHCP configured interfaces */
    if (netif_dhcp != NULL) {
      dhcp_netif_fine_tmr(netif, netif_dhcp);
    }
  }
}

/**
 * A DHCP negotiation transaction, or ARP request, has timed out.
 *
 * The timer that was started with the DHCP or ARP request has
 * timed out, indicating no response was received in time.
 *
 * @param netif the netif under DHCP control
 */
static void
dhcp_timeout(struct netif *netif, struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_timeout()\n"));
  /* back-off period has passed, or server selection timed out */
  if ((dhcp_state->state == DHCP_STATE_BACKING_OFF) || (dhcp_state->state == DHCP_STATE_SELECTING)) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_timeout(): restarting discovery\n"));
    (void)dhcp_discover(netif, dhcp);
    /* receiving the requested lease timed out */
  } else if (dhcp_state->state == DHCP_STATE_REQUESTING) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                     ("dhcp_timeout(): REQUESTING, DHCP request timed out\n"));
    if (dhcp_state->tries <= 5) {
      (void)dhcp_select(netif, dhcp);
    } else {
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_timeout(): REQUESTING timed out\n"));
      if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
        (void)dhcp_release(netif);
      } else {
        (void)dhcp_release_client(netif, dhcp);
      }
      (void)dhcp_discover(netif, dhcp);
    }
#if DHCP_DOES_ARP_CHECK
    /* received no ARP reply for the offered address (which is good) */
  } else if (dhcp_state->state == DHCP_STATE_CHECKING) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_timeout(): CHECKING timed out\n"));
    if (dhcp_state->tries <= 1) {
      dhcp_check(netif, dhcp);
      /* no ARP replies on the offered address,
         looks like the IP address is indeed free */
    } else {
      /* bind the interface to the offered address */
      dhcp_bind(netif, dhcp);
    }
#endif /* DHCP_DOES_ARP_CHECK */
#if LWIP_DHCP_SUBSTITUTE_MMBR
    /* received no ARP reply for the releasing address (no other using this address) */
  } else if (dhcp_state->state == DHCP_STATE_RELEASING) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_timeout(): RELEASING timed out\n"));
    if (dhcp_state->tries <= 1) {
      dhcp_release_check(netif, dhcp);
    } else {
      /* no other MBR using this address, release it */
      dhcp_stop_client(netif, dhcp);
    }
#endif /* LWIP_DHCP_SUBSTITUTE_MMBR */
  } else if (dhcp_state->state == DHCP_STATE_REBOOTING) {
    if (dhcp_state->tries < REBOOT_TRIES) {
      (void)dhcp_reboot(netif);
    } else {
      (void)dhcp_discover(netif, dhcp);
    }
  } else if (dhcp_state->state == DHCP_STATE_RENEWING) {
    /* 5: send dhcp request package six times to renew its lease */
    if (dhcp_state->tries <= 5) {
      (void)dhcp_renew_client(netif, dhcp);
    }
  } else if (dhcp_state->state == DHCP_STATE_REBINDING) {
    /* 5: send dhcp request package six times to Rebind with a DHCP server for an existing DHCP lease. */
    if (dhcp_state->tries <= 5) {
      (void)dhcp_rebind(netif, dhcp);
    }
#if LWIP_ARP
  } else if (dhcp_state->state == DHCP_STATE_BOUND) {
    if (dhcp_state->tries < LWIP_DHCP_ANNOUNCE_NUM) {
      dhcp_announce(netif, dhcp);
    } else {
      dhcp_state->tries = 0;
    }
#endif
#if (LWIP_DHCP_SUBSTITUTE && LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS)
  } else if (dhcp_state->state == DHCP_STATE_LIMIT_WAIT) {
    dhcp_concurrent_limit_wait_timeout(netif, dhcp);
#endif
  }
}

/**
 * The renewal period has timed out.
 *
 * @param netif the netif under DHCP control
 */
static void
dhcp_t1_timeout(struct netif *netif, struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_STATE, ("dhcp_t1_timeout()\n"));
  if ((dhcp_state->state == DHCP_STATE_REQUESTING) || (dhcp_state->state == DHCP_STATE_BOUND) ||
      (dhcp_state->state == DHCP_STATE_RENEWING)) {
    /* just retry to renew - note that the rebind timer (t2) will
     * eventually time-out if renew tries fail. */
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_t1_timeout(): must renew\n"));
    /* This slightly different to RFC2131: DHCPREQUEST will be sent from state
       DHCP_STATE_RENEWING, not DHCP_STATE_BOUND */
    (void)dhcp_renew_client(netif, dhcp);
    /* Calculate next timeout */
    if (((dhcp->t2_timeout - dhcp_state->lease_used) / 2) >=
        ((60 + DHCP_COARSE_TIMER_SECS / 2) / DHCP_COARSE_TIMER_SECS)) {
      dhcp_state->re_time = (u16_t)((dhcp->t2_timeout - dhcp_state->lease_used) / 2);
    } else {
      dhcp_state->re_time = (u16_t)(dhcp->t2_timeout - dhcp_state->lease_used);
    }
  }
}

/**
 * The rebind period has timed out.
 *
 * @param netif the netif under DHCP control
 */
static void
dhcp_t2_timeout(struct netif *netif, struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_t2_timeout()\n"));
  if ((dhcp_state->state == DHCP_STATE_REQUESTING) || (dhcp_state->state == DHCP_STATE_BOUND) ||
      (dhcp_state->state == DHCP_STATE_RENEWING) || (dhcp_state->state == DHCP_STATE_REBINDING)) {
    /* just retry to rebind */
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_t2_timeout(): must rebind\n"));
    /* This slightly different to RFC2131: DHCPREQUEST will be sent from state
       DHCP_STATE_REBINDING, not DHCP_STATE_BOUND */
    (void)dhcp_rebind(netif, dhcp);
    /* Calculate next timeout */
    if (((dhcp->t0_timeout - dhcp_state->lease_used) / 2) >=
        ((60 + DHCP_COARSE_TIMER_SECS / 2) / DHCP_COARSE_TIMER_SECS)) {
      dhcp_state->re_time = (u16_t)((dhcp->t0_timeout - dhcp_state->lease_used) / 2);
    }
  }
}

/**
 * Handle a DHCP ACK packet
 *
 * @param netif the netif under DHCP control
 */
static void
dhcp_handle_ack(struct netif *netif, struct dhcp_client *dhcp)
{
#if LWIP_DHCP_BOOTP_FILE
  struct dhcp *netif_dhcp = netif_dhcp_data(netif);
#else
  (void)netif;
#endif /* LWIP_DHCP_BOOTP_FILE */
  ip4_addr_t cli_ip;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

#if LWIP_DHCP_PROVIDE_DNS_SERVERS || LWIP_DHCP_GET_NTP_SRV
  u8_t n;
#endif /* LWIP_DHCP_PROVIDE_DNS_SERVERS || LWIP_DHCP_GET_NTP_SRV */
#if LWIP_DHCP_GET_NTP_SRV
  ip4_addr_t ntp_server_addrs[LWIP_DHCP_MAX_NTP_SERVERS];
#endif

  /* clear options we might not get from the ACK */
  ip4_addr_set_zero(&dhcp->offered_sn_mask);
  ip4_addr_set_zero(&dhcp->offered_gw_addr);
#if LWIP_DHCP_BOOTP_FILE
  if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
    ip4_addr_set_zero(&netif_dhcp->offered_si_addr);
  }
#endif /* LWIP_DHCP_BOOTP_FILE */

  /* lease time given? */
  if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_LEASE_TIME)) {
    /* remember offered lease time */
    dhcp->offered_t0_lease = dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_LEASE_TIME);
  }
  /* renewal period given? */
  if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_T1)) {
    /* remember given renewal period */
    dhcp->offered_t1_renew = dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_T1);
  } else {
    /* calculate safe periods for renewal */
    dhcp->offered_t1_renew = dhcp->offered_t0_lease / 2;
  }

  /* renewal period given? */
  if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_T2)) {
    /* remember given rebind period */
    dhcp->offered_t2_rebind = dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_T2);
  } else {
    /* calculate safe periods for rebinding (offered_t0_lease * 0.875 -> 87.5%)*/
    dhcp->offered_t2_rebind = (dhcp->offered_t0_lease * 7U) / 8U;
  }

#if LWIP_DHCP_BOOTP_FILE
  /* copy boot server address,
     boot file name copied in dhcp_parse_reply if not overloaded */
  if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
    ip4_addr_copy(netif_dhcp->offered_si_addr, dhcp->msg_in->siaddr);
  }
#endif /* LWIP_DHCP_BOOTP_FILE */

  /* subnet mask given? */
  if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_SUBNET_MASK)) {
    /* remember given subnet mask */
    ip4_addr_set_u32(&dhcp->offered_sn_mask, lwip_htonl(dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_SUBNET_MASK)));
    dhcp->subnet_mask_given = lwIP_TRUE;
  } else {
    dhcp_ip_to_mask(ip_2_ip4(&dhcp->server_ip_addr), &dhcp->offered_sn_mask);
    dhcp->subnet_mask_given = lwIP_FALSE;
  }

  /* (y)our internet address */
  ip4_addr_copy(cli_ip, dhcp->msg_in->yiaddr);
  DHCP_IP_TO_HOST(dhcp_state->offered_ip_addr, cli_ip.addr, dhcp->offered_sn_mask.addr);

  /* gateway router */
  if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_ROUTER)) {
    ip4_addr_set_u32(&dhcp->offered_gw_addr, lwip_htonl(dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_ROUTER)));
  }

#if LWIP_DHCP_GET_NTP_SRV
  /* NTP servers */
  if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
    for (n = 0; (n < LWIP_DHCP_MAX_NTP_SERVERS) && dhcp_option_given(dhcp, DHCP_OPTION_IDX_NTP_SERVER + n); n++) {
      ip4_addr_set_u32(&ntp_server_addrs[n], lwip_htonl(dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_NTP_SERVER + n)));
    }
    dhcp_set_ntp_servers(n, ntp_server_addrs);
  }
#endif /* LWIP_DHCP_GET_NTP_SRV */

#if LWIP_DNS
#if LWIP_DHCP_PROVIDE_DNS_SERVERS
  /* DNS servers */
  if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
    for (n = 0; (n < LWIP_DHCP_PROVIDE_DNS_SERVERS) && dhcp_option_given(dhcp, DHCP_OPTION_IDX_DNS_SERVER + n); n++) {
      ip_addr_t dns_addr;
      ip_addr_set_ip4_u32_val(dns_addr, lwip_htonl(dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_DNS_SERVER + n)));
      dns_setserver(n, &dns_addr);
    }
  }
#endif /* LWIP_DHCP_PROVIDE_DNS_SERVERS */
#endif
}

/**
 * @ingroup dhcp4
 * Set a statically allocated struct dhcp to work with.
 * Using this prevents dhcp_start to allocate it using mem_malloc.
 *
 * @param netif the netif for which to set the struct dhcp
 * @param dhcp (uninitialised) dhcp struct allocated by the application
 */
void
dhcp_set_struct(struct netif *netif, struct dhcp *dhcp)
{
  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ERROR("netif != NULL", (netif != NULL), return;);
  LWIP_ERROR("dhcp != NULL", (dhcp != NULL), return;);

  /* clear data structure */
  memset(dhcp, 0, sizeof(struct dhcp));
  /* dhcp_set_state(&dhcp, DHCP_STATE_OFF); */
  netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP, dhcp);
}

void
dhcp_remove_struct(struct netif *netif)
{
  LWIP_ERROR("netif != NULL", (netif != NULL), return);

  netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP, NULL);
  return;
}

static void
dhcp_cleanup_indeed(void)
{
  if (dhcp_pcb != NULL) {
    udp_remove(dhcp_pcb);
    dhcp_pcb = NULL;
  }
  dhcp_pcb_refcount = 0;
}

/**
 * @ingroup dhcp4
 * Removes a struct dhcp from a netif.
 *
 * ATTENTION: Only use this when not using dhcp_set_struct() to allocate the
 *            struct dhcp since the memory is passed back to the heap.
 *
 * @param netif the netif from which to remove the struct dhcp
 */
void dhcp_cleanup(struct netif *netif)
{
  LWIP_ASSERT_CORE_LOCKED();
  struct dhcp *netif_dhcp = NULL;
#if LWIP_DHCP_SUBSTITUTE
  int i;
  struct dhcp_client *dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
#endif /* LWIP_DHCP_SUBSTITUTE */
  LWIP_ERROR("netif != NULL", (netif != NULL), return);

  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp == NULL) {
    return;
  }
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_cleanup\n"));
  if (netif_dhcp->pcb_allocated != 0) {
    dhcp_stop(netif);
  }
#if LWIP_DHCP_SUBSTITUTE
  dhcp = &(netif_dhcp->client);
  for (i = 1; i < DHCP_CLIENT_NUM; i++) {
    dhcp_state = &((dhcp->states)[i]);
    if (dhcp_state->idx == 0) {
      continue;
    }
    dhcp_substitute_stop(netif, dhcp_state->idx, lwIP_FALSE);
  }
  if (netif_dhcp->clis_info != NULL) {
    mem_free(netif_dhcp->clis_info);
    netif_dhcp->clis_info = NULL;
  }
#endif /* LWIP_DHCP_SUBSTITUTE */
  dhcp_cleanup_indeed();
  mem_free(netif_dhcp_data(netif));
  netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP, NULL);
}

/**
 * Check DHCP negotiation is done for a network interface.
 *
 * @param netif The lwIP network interface
 * @return
 * - ERR_OK - if DHCP is bound
 * - ERR_MEM - if DHCP bound is still progressing
 */
err_t
dhcp_is_bound(struct netif *netif)
{
  struct dhcp *netif_dhcp = NULL;

  LWIP_ERROR("netif != NULL", (netif != NULL), return ERR_ARG);

  netif_dhcp =  netif_dhcp_data(netif);
  LWIP_ERROR("netif->dhcp != NULL", (netif_dhcp != NULL), return ERR_ARG);

  if ((netif_dhcp->client.states)[LWIP_DHCP_NATIVE_IDX].state == DHCP_STATE_BOUND) {
    return ERR_OK;
  } else {
    return ERR_INPROGRESS;
  }
}

static void
dhcp_native_reset(struct dhcp *netif_dhcp)
{
  netif_dhcp->pcb_allocated = 0;
#if LWIP_DHCP_AUTOIP_COOP
  netif_dhcp->autoip_coop_state = DHCP_AUTOIP_COOP_STATE_OFF;
#endif /* LWIP_DHCP_AUTOIP_COOP */
#if LWIP_DHCP_BOOTP_FILE
  (void)memset_s(&(netif_dhcp->offered_si_addr), sizeof(netif_dhcp->offered_si_addr),
                 0x0, sizeof(netif_dhcp->offered_si_addr));
  (void)memset_s(netif_dhcp->boot_file_name, sizeof(netif_dhcp->boot_file_name),
                 0x0, sizeof(netif_dhcp->boot_file_name));
#endif /* LWIP_DHCP_BOOTP_FILE */

  return;
}

static struct dhcp *
dhcp_netif_dhcp_new(void)
{
  struct dhcp *netif_dhcp = NULL;

  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_netif_dhcp_new(): starting new DHCP client\n"));
  netif_dhcp = (struct dhcp *)mem_malloc(sizeof(struct dhcp));
  if (netif_dhcp == NULL) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_netif_dhcp_new(): could not allocate dhcp\n"));
    return NULL;
  }

  (void)memset_s(netif_dhcp, sizeof(struct dhcp), 0x0, sizeof(struct dhcp));

  return netif_dhcp;
}

static err_t
dhcp_start_client_native(struct netif *netif)
{
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_client *dhcp = NULL;

  netif_dhcp = netif_dhcp_data(netif);
  dhcp = &(netif_dhcp->client);
  (void)dhcp;
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                   ("dhcp_start_client_native(): restarting DHCP configuration\n"));
  LWIP_ASSERT("pbuf p_out wasn't freed", dhcp->p_out == NULL);
  LWIP_ASSERT("reply wasn't freed", dhcp->msg_in == NULL);

  if (netif_dhcp->pcb_allocated != 0) {
    dhcp_dec_pcb_refcount(); /* free DHCP PCB if not needed any more */
  }
  /* dhcp is cleared below, no need to reset flag */
  /* clear data structure */
  dhcp_native_reset(netif_dhcp);

  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_start_client_native(): starting DHCP configuration\n"));

  if (dhcp_inc_pcb_refcount(netif->ifindex) != ERR_OK) { /* ensure DHCP PCB is allocated */
    dhcp_stop(netif);
    return ERR_MEM;
  }
  netif_dhcp->pcb_allocated = 1;

  return ERR_OK;
}

#if LWIP_DHCP_SUBSTITUTE_MMBR
/*
 * substitute client start with a preferred IP, then
 * Discover message will carry Request IP option filling with the preferred IP
 * it will not do duplicate IP check
 */
static void
dhcp_substitute_prefer_ip(struct dhcp_client *dhcp, u32_t pref_ipv4)
{
  ip4_addr_t cli_ip, netaddr;
  dhcp_num_t offered_ip_addr;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  if (ip4_addr_isany_val(dhcp->offered_sn_mask)) {
    return;
  }

  if (ip4_addr_isany_val(dhcp->relay_ip)) {
    netaddr.addr = ip_2_ip4(&dhcp->server_ip_addr)->addr;
  } else {
    netaddr.addr = dhcp->relay_ip.addr;
  }
  cli_ip.addr = pref_ipv4;

  /* it also can be checkout that pref_ipv4 is 0 */
  if (!ip4_addr_netcmp(&cli_ip, &netaddr, &(dhcp->offered_sn_mask))) {
    return;
  }

  DHCP_IP_TO_HOST(offered_ip_addr, pref_ipv4, dhcp->offered_sn_mask.addr);
  dhcp_state->offered_ip_addr = offered_ip_addr;
  dhcp_state->addr_not_dup_check = lwIP_TRUE;

  return;
}
#endif

static err_t
dhcp_start_client(struct netif *netif, dhcp_num_t mac_idx, u32_t pref_ipv4)
{
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_client *dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
  u8_t hwaddr_len;
  err_t result;
#if LWIP_DHCP_SUBSTITUTE
  u8_t is_new = lwIP_FALSE;
#endif /* LWIP_DHCP_SUBSTITUTE */

#if !(LWIP_DHCP_SUBSTITUTE && LWIP_DHCP_SUBSTITUTE_MMBR)
  (void)pref_ipv4;
#endif

  /* check MTU of the netif */
  if (netif->mtu < DHCP_MAX_MSG_LEN_MIN_REQUIRED) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE,
                     ("dhcp_start_client(): Cannot use this netif with DHCP: MTU is too small\n"));
    return ERR_MEM;
  }

  netif_dhcp = netif_dhcp_data(netif);
  /* no DHCP client attached yet? */
  if (netif_dhcp == NULL) {
    netif_dhcp = dhcp_netif_dhcp_new();
    if (netif_dhcp == NULL) {
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_start_client:new dhcp failed\n");
      return ERR_MEM;
    }
    /* store this dhcp client in the netif */
    netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP, netif_dhcp);
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_start_client(): allocated dhcp"));
    /* already has DHCP client attached */
  }
  dhcp = &(netif_dhcp->client);
  if (dhcp_client_find_by_mac_idx(dhcp, mac_idx, &(dhcp->cli_idx)) != ERR_OK) {
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_start_client(): client state not find for %u\n", mac_idx);
    if (dhcp_client_state_new(dhcp, mac_idx, &(dhcp->cli_idx)) != ERR_OK) {
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_start_client(): no client state for %u\n", mac_idx);
      return ERR_MEM;
    } else {
#if LWIP_DHCP_SUBSTITUTE
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_start_client(): new client state for %u\n", mac_idx);
      is_new = lwIP_TRUE;
#endif /* LWIP_DHCP_SUBSTITUTE */
    }
  }
  dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  if (dhcp_idx_to_mac(netif, dhcp_state->idx, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_start_client(): no client state for %u\n", mac_idx);
    return ERR_MEM;
  }
  dhcp_state->hwaddr_len = hwaddr_len;

  if (mac_idx == LWIP_DHCP_NATIVE_IDX) {
    result = dhcp_start_client_native(netif);
    if (result != ERR_OK) {
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_start_client:native failed\n");
      (void)memset_s(dhcp_state, sizeof(struct dhcp_state), 0, sizeof(struct dhcp_state));
      return result;
    }
  }
#if LWIP_DHCP_SUBSTITUTE
  else {
    if ((is_new == lwIP_TRUE) && (dhcp_inc_pcb_refcount(netif->ifindex) != ERR_OK)) {
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_start_client:inc ref failed\n");
      (void)memset_s(dhcp_state, sizeof(struct dhcp_state), 0, sizeof(struct dhcp_state));
      return ERR_MEM;
    }
#if LWIP_DHCP_SUBSTITUTE_MMBR
    dhcp_substitute_prefer_ip(dhcp, pref_ipv4);
#endif
  }
#if LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS
  if (dhcp_concurrent_start_client(dhcp) != ERR_OK) {
    return ERR_OK;
  }
#endif
#endif /* LWIP_DHCP_SUBSTITUTE */

#if LWIP_DHCP_CHECK_LINK_UP
  if (!netif_is_link_up(netif)) {
    /* set state INIT and wait for dhcp_network_changed() to call dhcp_discover() */
    dhcp_set_state(dhcp, DHCP_STATE_INIT);
    return ERR_OK;
  }
#endif

  /* (re)start the DHCP negotiation */
  result = dhcp_discover(netif, dhcp);
  if (result != ERR_OK) {
    /* free resources allocated above */
    if (mac_idx == LWIP_DHCP_NATIVE_IDX) {
      dhcp_stop(netif);
    } else {
      dhcp_stop_client(netif, dhcp);
    }
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_start_client:discover failed\n");
    return ERR_MEM;
  }

  return ERR_OK;
}

/**
 * @ingroup dhcp4
 * Start DHCP negotiation for a network interface.
 *
 * If no DHCP client instance was attached to this interface,
 * a new client is created first. If a DHCP client instance
 * was already present, it restarts negotiation.
 *
 * @param netif The lwIP network interface
 * @return lwIP error code
 * - ERR_OK - No error
 * - ERR_MEM - Out of memory
 */
err_t
dhcp_start(struct netif *netif)
{
  err_t result;

  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ERROR("netif != NULL", (netif != NULL), return ERR_ARG;);
  LWIP_ERROR("netif is not up, old style port?", netif_is_up(netif), return ERR_ARG;);

  netif_set_ipaddr(netif, IP4_ADDR_ANY4);
  netif_set_gw(netif, IP4_ADDR_ANY4);
  netif_set_netmask(netif, IP4_ADDR_ANY4);

  LWIP_DEBUGF_LOG4(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                   "dhcp_start(netif=%p) %c%c%"U16_F"\n", (void *)netif, netif->name[0], netif->name[1],
                   (u16_t)netif->num);

  /* Remove the flag that says this netif is handled by DHCP,
     it is set when we succeeded starting. */
  netif->flags = (netif->flags & (~NETIF_FLAG_DHCP));

  result = dhcp_start_client(netif, LWIP_DHCP_NATIVE_IDX, 0);

  if (result != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp start failed\n");
    return result;
  }

  dhcp_clients_count_update(&(netif_dhcp_data(netif)->client));
  /* Set the flag that says this netif is handled by DHCP. */
  netif->flags |= NETIF_FLAG_DHCP;
  return ERR_OK;
}

/**
 * @ingroup dhcp4
 * Inform a DHCP server of our manual configuration.
 *
 * This informs DHCP servers of our fixed IP address configuration
 * by sending an INFORM message. It does not involve DHCP address
 * configuration, it is just here to be nice to the network.
 *
 * @param netif The lwIP network interface
 */
void
dhcp_inform(struct netif *netif)
{
  err_t result, ret;
  u16_t options_out_len;

  u8_t is_malloc = lwIP_FALSE;
  u16_t malloc_size;
  u8_t hwaddr_len;
  struct dhcp_state *dhcp_state = NULL;
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_client *dhcp = NULL;

  LWIP_ASSERT_CORE_LOCKED();
  LWIP_ERROR("netif != NULL", (netif != NULL), return;);
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_inform\n");
  if (dhcp_inc_pcb_refcount(netif->ifindex) != ERR_OK) { /* ensure DHCP PCB is allocated */
    return;
  }

  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp != NULL) {
    dhcp = &(netif_dhcp->client);
  } else {
    malloc_size = (u16_t)(sizeof(struct dhcp_client) - (DHCP_CLIENT_NUM - 1) * sizeof(struct dhcp_state));
    dhcp = (struct dhcp_client *)mem_malloc(malloc_size);
    if (dhcp == NULL) {
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp_inform: malloc failed\n"));
      return;
    }
    (void)memset_s(dhcp, malloc_size, 0, malloc_size);
    is_malloc = lwIP_TRUE;
  }

  dhcp->cli_idx = LWIP_DHCP_NATIVE_IDX;
  dhcp_state = &(dhcp->states[LWIP_DHCP_NATIVE_IDX]);
  dhcp_set_state(dhcp, DHCP_STATE_INFORMING);
  if (dhcp_idx_to_mac(netif, LWIP_DHCP_NATIVE_IDX, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp_inform: get mac failed\n"));
    dhcp_dec_pcb_refcount();
    if (is_malloc == lwIP_TRUE) {
      mem_free(dhcp);
    }
    return;
  }
  dhcp_state->hwaddr_len = hwaddr_len;

  /* create and initialize the DHCP message header */
  result = dhcp_create_msg(netif, dhcp, DHCP_INFORM, &options_out_len);
  if (result == ERR_OK) {
    dhcp_fill_options(netif, dhcp, DHCP_INFORM, GEN_FLG(MAX_MSG_SIZE), &options_out_len);

    ret = udp_sendto_if(dhcp_pcb, dhcp->p_out, IP_ADDR_BROADCAST, LWIP_IANA_PORT_DHCP_SERVER, netif);
    (void)ret;
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, "dhcp_inform: INFORMING %d\n", ret);
    dhcp_delete_msg(dhcp);
  } else {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp_inform: could not allocate DHCP request\n"));
  }

  dhcp_dec_pcb_refcount(); /* delete DHCP PCB if not needed any more */
  if (is_malloc == lwIP_TRUE) {
    mem_free(dhcp);
  }
}

/** Handle a possible change in the network configuration.
 *
 * This enters the REBOOTING state to verify that the currently bound
 * address is still valid.
 */
static void
dhcp_network_changed_client(struct netif *netif, struct dhcp_client *dhcp)
{
#if LWIP_DHCP_AUTOIP_COOP
  struct dhcp *netif_dhcp = netif_dhcp_data(netif);
#endif /* LWIP_DHCP_AUTOIP_COOP */
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_network_changed_client:state %hhu\n", dhcp_state->state);

  switch (dhcp_state->state) {
    case DHCP_STATE_REBINDING:
    case DHCP_STATE_RENEWING:
    case DHCP_STATE_BOUND:
    case DHCP_STATE_REBOOTING:
      dhcp_state->tries = 0;
#if (LWIP_DHCP_SUBSTITUTE && LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS)
      if (dhcp_concurrent_start_client(dhcp) != ERR_OK) {
        LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_network_changed_client:up limit %hu\n",
                         g_dhcp_max_concurrent_num);
        return;
      }
#endif
      (void)dhcp_reboot(netif);
      break;
    case DHCP_STATE_OFF:
      /* stay off */
      break;
    default:
      /* INIT/REQUESTING/CHECKING/BACKING_OFF restart with new 'rid' because the
         state changes, SELECTING: continue with current 'rid' as we stay in the
         same state */
#if LWIP_DHCP_AUTOIP_COOP
      if ((dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) &&
          (netif_dhcp->autoip_coop_state == DHCP_AUTOIP_COOP_STATE_ON)) {
        (void)autoip_stop(netif);
        netif_dhcp->autoip_coop_state = DHCP_AUTOIP_COOP_STATE_OFF;
      }
#endif /* LWIP_DHCP_AUTOIP_COOP */
      /* ensure we start with short timeouts, even if already discovering */
      dhcp_state->tries = 0;
#if (LWIP_DHCP_SUBSTITUTE && LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS)
      if (dhcp_concurrent_start_client(dhcp) != ERR_OK) {
        LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_network_changed_client:up limit %hu\n",
                         g_dhcp_max_concurrent_num);
        return;
      }
#endif
      (void)dhcp_discover(netif, dhcp);
      break;
  }
}

void
dhcp_network_changed(struct netif *netif)
{
  struct dhcp *netif_dhcp = netif_dhcp_data(netif);
  struct dhcp_client *dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;

  int i;
  u8_t hwaddr_len;
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_network_changed()\n"));
  if (netif_dhcp == NULL) {
    return;
  }
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_network_changed() dhcp\n"));

  dhcp = &(netif_dhcp->client);
#if (LWIP_DHCP_SUBSTITUTE && LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS)
  dhcp->rqst_cli_cnt = 0;
#endif
  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    dhcp->cli_idx = (dhcp_num_t)i;
    dhcp_state = &((dhcp->states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    if (dhcp_idx_to_mac(netif, dhcp_state->idx, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                       "dhcp_network_changed:idx %"DHCP_NUM_F" to mac failed\n", dhcp_state->idx);
      continue;
    }
    dhcp_state->hwaddr_len = hwaddr_len;
    dhcp_network_changed_client(netif, dhcp);
  }
}

#if DHCP_DOES_ARP_CHECK
/**
 * Match an ARP reply with the offered IP address:
 * check whether the offered IP address is not in use using ARP
 *
 * @param netif the network interface on which the reply was received
 * @param addr The IP address we received a reply from
 */
void
dhcp_arp_reply(struct netif *netif, const ip4_addr_t *addr)
{
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_client *dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
  ip4_addr_t cli_ip;
  u8_t hwaddr_len;
  int i;

  LWIP_ERROR("netif != NULL", (netif != NULL), return;);
  netif_dhcp = netif_dhcp_data(netif);
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_arp_reply()\n"));
  /* is a DHCP client doing an ARP check? */
  if (netif_dhcp == NULL) {
    return;
  }
  dhcp = &(netif_dhcp->client);
  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    dhcp->cli_idx = (dhcp_num_t)i;
    dhcp_state = &((dhcp->states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    if (
#if LWIP_DHCP_SUBSTITUTE_MMBR
        (dhcp_state->state == DHCP_STATE_RELEASING) ||
#endif
        (dhcp_state->state == DHCP_STATE_CHECKING)) {
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                       "dhcp_arp_reply(): CHECKING, arp reply for 0x%08"X32_F"\n",
                       ip4_addr_get_u32(addr));
      /* did a host respond with the address we
         were offered by the DHCP server? */
      dhcp_get_client_ip(&cli_ip.addr, dhcp, dhcp_state);
      if (!ip4_addr_cmp(addr, &cli_ip)) {
        continue;
      }
      /* we will not accept the offered address */
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_LEVEL_WARNING,
                       ("dhcp_arp_reply(): arp reply matched with offered address, declining\n"));
      if (dhcp_idx_to_mac(netif, dhcp_state->idx, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
        LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                         "dhcp_arp_reply:idx %u to mac failed\n", dhcp_state->idx);
        return;
      }
      dhcp_state->hwaddr_len = hwaddr_len;
#if LWIP_DHCP_SUBSTITUTE_MMBR
      if (dhcp_state->state == DHCP_STATE_RELEASING) {
        /* just think that there is a MBR using this address, do not send Release message by me */
        dhcp_set_state(dhcp, DHCP_STATE_OFF);
        dhcp_stop_client(netif, dhcp);
      } else
#endif
      {
        (void)dhcp_decline(netif, dhcp);
      }
      return;
    }
  }
}

/**
 * Decline an offered lease.
 *
 * Tell the DHCP server we do not accept the offered address.
 * One reason to decline the lease is when we find out the address
 * is already in use by another host (through ARP).
 *
 * @param netif the netif under DHCP control
 */
static err_t
dhcp_decline(struct netif *netif, struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  err_t result, ret;
  u16_t msecs;
  u16_t options_out_len;

  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_decline()\n"));
  dhcp_set_state(dhcp, DHCP_STATE_BACKING_OFF);
  /* create and initialize the DHCP message header */
  result = dhcp_create_msg(netif, dhcp, DHCP_DECLINE, &options_out_len);
  if (result == ERR_OK) {
    options_out_len = dhcp_option(options_out_len, dhcp->msg_out->options, DHCP_OPTION_SERVER_ID, 4);
    options_out_len = dhcp_option_long(options_out_len, dhcp->msg_out->options,
                                       lwip_ntohl(ip4_addr_get_u32(ip_2_ip4(&dhcp->server_ip_addr))));
    dhcp_fill_options(netif, dhcp, DHCP_DECLINE, GEN_FLG(REQUESTED_IP), &options_out_len);

    /* per section 4.4.4, broadcast DECLINE messages */
    ret = udp_sendto_if_src(dhcp_pcb, dhcp->p_out, IP_ADDR_BROADCAST, LWIP_IANA_PORT_DHCP_SERVER, netif, IP4_ADDR_ANY);
    dhcp_delete_msg(dhcp);
    (void)ret;
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_decline: BACKING OFF\n"));
  } else {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                     ("dhcp_decline: could not allocate DHCP request\n"));
  }
  if (dhcp_state->tries < 255) {
    dhcp_state->tries++;
  }
  msecs = DHCP_REQUEST_TIMEOUT_DEFAULT * MS_PER_SECOND;
  dhcp_state->request_timeout = (u16_t)((msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS);
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_decline(): set request timeout %"U16_F" msecs\n", msecs));
  return result;
}
#endif /* DHCP_DOES_ARP_CHECK */


/**
 * Start the DHCP process, discover a DHCP server.
 *
 * @param netif the netif under DHCP control
 */
static err_t
dhcp_discover(struct netif *netif, struct dhcp_client *dhcp)
{
#if LWIP_DHCP_AUTOIP_COOP
  struct dhcp *netif_dhcp = netif_dhcp_data(netif);
#endif
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  err_t result, ret;
  u16_t msecs;
  u16_t options_out_len;
  u32_t opt_flags = GEN_FLG(MAX_MSG_SIZE) | GEN_FLG(PARAMETER_REQUEST_LIST);
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_discover()\n"));
#if LWIP_DHCP_SUBSTITUTE_MMBR
  if (dhcp_state->addr_not_dup_check != lwIP_TRUE)
#endif
  {
    dhcp_state->offered_ip_addr = 0;
  }

  dhcp_set_state(dhcp, DHCP_STATE_SELECTING);
  /* create and initialize the DHCP message header */
  result = dhcp_create_msg(netif, dhcp, DHCP_DISCOVER, &options_out_len);
  if (result == ERR_OK) {
#if LWIP_DHCP_SUBSTITUTE_MMBR
    if ((dhcp_state->addr_not_dup_check == lwIP_TRUE) && dhcp_state->offered_ip_addr != 0) {
      /* Request IP option with preferred IP */
      opt_flags |= GEN_FLG(REQUESTED_IP);
    }
#endif
    dhcp_fill_options(netif, dhcp, DHCP_DISCOVER, opt_flags, &options_out_len);
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_discover: making request\n"));

    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_discover: sendto(DISCOVER, IP_ADDR_BROADCAST, LWIP_IANA_PORT_DHCP_SERVER)\n"));
    ret = udp_sendto_if_src(dhcp_pcb, dhcp->p_out, IP_ADDR_BROADCAST, LWIP_IANA_PORT_DHCP_SERVER, netif, IP4_ADDR_ANY);
    (void)ret;
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_discover: deleting()ing\n"));
    dhcp_delete_msg(dhcp);
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_discover: SELECTING\n"));
  } else {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                     ("dhcp_discover: could not allocate DHCP request\n"));
  }
  if (dhcp_state->tries < 255) {
    dhcp_state->tries++;
  }
#if LWIP_DHCP_AUTOIP_COOP
  if ((dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) && (dhcp_state->tries >= LWIP_DHCP_AUTOIP_COOP_TRIES) &&
      (netif_dhcp->autoip_coop_state == DHCP_AUTOIP_COOP_STATE_OFF)) {
    netif_dhcp->autoip_coop_state = DHCP_AUTOIP_COOP_STATE_ON;
    autoip_start(netif);
  }
#endif /* LWIP_DHCP_AUTOIP_COOP */
  msecs = (u16_t)((dhcp_state->tries < 6 ? (1UL << dhcp_state->tries) : 60) * DHCP_DISCOVER_RETRANSMIT_INTERVAL);
  dhcp_state->request_timeout = (u16_t)((msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS);
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                   "dhcp_discover(): set request timeout %"U16_F" msecs\n", msecs);
  return result;
}

#if LWIP_ARP
static void
dhcp_announce(struct netif *netif, struct dhcp_client *dhcp)
{
  err_t result;
  u16_t msecs;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  ip4_addr_t cli_ip;
  LWIP_DEBUGF_LOG3(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_announce(netif=%p) %c%c\n", (void *)netif, (s16_t)netif->name[0],
                   (s16_t)netif->name[1]);
  dhcp_get_client_ip(&cli_ip.addr, dhcp, dhcp_state);
  result = etharp_announce(netif, (const ip4_addr_t *)(&cli_ip));
  if (result != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("dhcp_announce: arp announce failed\n"));
  }
  if (dhcp_state->tries < 255) {
    dhcp_state->tries++;
  }
  msecs = LWIP_DHCP_ANNOUNCE_INTERVAL * MS_PER_SECOND;
  dhcp_state->request_timeout = (u16_t)((msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS);
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, "dhcp_announce: set request timeout %"U16_F" msecs\n",
                   msecs);
}
#endif

/**
 * Bind the interface to the offered IP address.
 *
 * @param netif network interface to bind to the offered address
 */
static void
dhcp_bind(struct netif *netif, struct dhcp_client *dhcp)
{
  u32_t timeout;
  struct dhcp *netif_dhcp = NULL;
  ip4_addr_t sn_mask, gw_addr;
  u8_t is_native = lwIP_FALSE;
  ip4_addr_t cli_ip;
  struct dhcp_state *dhcp_state = NULL;
#if (LWIP_DHCP_SUBSTITUTE && LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS)
  dhcp_state_enum_t pre_state;
#endif
  LWIP_ERROR("dhcp_bind: netif != NULL", (netif != NULL), return;);
  netif_dhcp = netif_dhcp_data(netif);
  LWIP_ERROR("dhcp_bind: netif_dhcp != NULL", (netif_dhcp != NULL), return;);
  LWIP_DEBUGF_LOG3(DHCP_DEBUG | LWIP_DBG_TRACE,
                   "dhcp_bind(netif=%p) %s%"U16_F"\n", (void *)netif, netif->name, (u16_t)netif->num);

  if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
    is_native = lwIP_TRUE;
  }
  dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  /* reset time used of lease */
  dhcp_state->lease_used = 0;

  if (dhcp->offered_t0_lease != 0xffffffffUL) {
    /* set renewal period timer */
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_bind(): t0 renewal timer %"U32_F" secs\n",
                     dhcp->offered_t0_lease);
    timeout = (dhcp->offered_t0_lease + DHCP_COARSE_TIMER_SECS / 2) / DHCP_COARSE_TIMER_SECS;
    if (timeout > 0xffff) {
      timeout = 0xffff;
    }
    dhcp->t0_timeout = (u16_t)timeout;
    if (dhcp->t0_timeout == 0) {
      dhcp->t0_timeout = 1;
    }
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                     "dhcp_bind(): set request timeout %"U32_F" msecs\n", dhcp->offered_t0_lease * 1000);
  }

  /* temporary DHCP lease? */
  if (dhcp->offered_t1_renew != 0xffffffffUL) {
    /* set renewal period timer */
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_bind(): t1 renewal timer %"U32_F" secs\n",
                     dhcp->offered_t1_renew);
    timeout = (dhcp->offered_t1_renew + DHCP_COARSE_TIMER_SECS / 2) / DHCP_COARSE_TIMER_SECS;
    if (timeout > 0xffff) {
      timeout = 0xffff;
    }
    dhcp->t1_timeout = (u16_t)timeout;
    if (dhcp->t1_timeout == 0) {
      dhcp->t1_timeout = 1;
    }
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                     "dhcp_bind(): set request timeout %"U32_F" msecs\n", dhcp->offered_t1_renew * 1000);
    dhcp_state->re_time = dhcp->t1_timeout;
  }
  /* set renewal period timer */
  if (dhcp->offered_t2_rebind != 0xffffffffUL) {
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_bind(): t2 rebind timer %"U32_F" secs\n",
                     dhcp->offered_t2_rebind);
    timeout = (dhcp->offered_t2_rebind + DHCP_COARSE_TIMER_SECS / 2) / DHCP_COARSE_TIMER_SECS;
    if (timeout > 0xffff) {
      timeout = 0xffff;
    }
    dhcp->t2_timeout = (u16_t)timeout;
    if (dhcp->t2_timeout == 0) {
      dhcp->t2_timeout = 1;
    }
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                     "dhcp_bind(): set request timeout %"U32_F" msecs\n", dhcp->offered_t2_rebind * 1000);
  }

  /* If we have sub 1 minute lease, t2 and t1 will kick in at the same time. */
  if ((dhcp->t1_timeout >= dhcp->t2_timeout) && (dhcp->t2_timeout > 0)) {
    dhcp->t1_timeout = 0;
    dhcp_state->re_time = dhcp->t2_timeout;
  }

  if (dhcp->subnet_mask_given == lwIP_TRUE) {
    /* copy offered network mask */
    ip4_addr_copy(sn_mask, dhcp->offered_sn_mask);
  } else {
    /* subnet mask not given, choose a safe subnet mask given the network class */
    dhcp_ip_to_mask(ip_2_ip4(&dhcp->server_ip_addr), &sn_mask);
    ip4_addr_copy(dhcp->offered_sn_mask, sn_mask);
  }

  dhcp_get_client_ip(&cli_ip.addr, dhcp, dhcp_state);

  ip4_addr_copy(gw_addr, dhcp->offered_gw_addr);

#if LWIP_DHCP_AUTOIP_COOP
  if ((is_native == lwIP_TRUE) && (netif_dhcp->autoip_coop_state == DHCP_AUTOIP_COOP_STATE_ON)) {
    autoip_stop(netif);
    netif_dhcp->autoip_coop_state = DHCP_AUTOIP_COOP_STATE_OFF;
  }
#endif /* LWIP_DHCP_AUTOIP_COOP */

  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_STATE, ("dhcp_bind(): IP: 0x%08"X32_F" SN: 0x%08"X32_F" GW: 0x%08"X32_F"\n",
              ip4_addr_get_u32(&cli_ip), ip4_addr_get_u32(&sn_mask), ip4_addr_get_u32(&gw_addr)));
#if (LWIP_DHCP_SUBSTITUTE && LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS)
  pre_state = dhcp_state->state;
#endif
  /* netif is now bound to DHCP leased address - set this before assigning the address
     to ensure the callback can use dhcp_supplied_address() */
  dhcp_set_state(dhcp, DHCP_STATE_BOUND);

  if (is_native == lwIP_TRUE) {
    (void)netif_set_addr(netif, &cli_ip, &sn_mask, &gw_addr);
  }
#if LWIP_NAT64
  else {
    nat64_dhcp_ip4_event(dhcp->hwaddr, dhcp_state->hwaddr_len, &cli_ip, NAT64_DHCP_EVENT_OFFER);
  }
#endif
#if LWIP_ARP
  if ((netif->flags & NETIF_FLAG_ETHARP) != 0) {
    dhcp_announce(netif, dhcp);
  }
#endif

#if (LWIP_DHCP_SUBSTITUTE && LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS)
  if ((g_dhcp_max_concurrent_num != 0) && (dhcp_client_request_processing(pre_state) == lwIP_TRUE)) {
    dhcp->rqst_cli_cnt--;
    dhcp_concurrent_start_next_client(netif, lwIP_FALSE);
  }
#endif
}

/* handle dhcp renew, rebind, reboot -- hence the name */
static err_t
dhcp_re3(struct netif *netif, struct dhcp_client *dhcp, u8_t state)
{
  err_t result, ret;
  u16_t options_out_len;
  u16_t msecs;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  dhcp_set_state(dhcp, state);

  /* create and initialize the DHCP message header */
  result = dhcp_create_msg(netif, dhcp, DHCP_REQUEST, &options_out_len);
  if (result == ERR_OK) {
    u32_t opt_flags = GEN_FLG(MAX_MSG_SIZE) | GEN_FLG(PARAMETER_REQUEST_LIST);
    if (state == DHCP_STATE_REBOOTING) {
      opt_flags |= GEN_FLG(REQUESTED_IP);
    }
    dhcp_fill_options(netif, dhcp, DHCP_REQUEST, opt_flags, &options_out_len);

    const ip_addr_t *server_ip_addr = (state == DHCP_STATE_RENEWING) ?
                                      (&dhcp->server_ip_addr) : IP_ADDR_BROADCAST;
    ret = udp_sendto_if(dhcp_pcb, dhcp->p_out, server_ip_addr, LWIP_IANA_PORT_DHCP_SERVER, netif);
    (void)ret;

    dhcp_delete_msg(dhcp);

    LWIP_DEBUGF_LOG2(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, "dhcp_re3: state=%u ret %d\n", state, ret);
  } else {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                     ("dhcp_re3: could not allocate DHCP request\n"));
  }
  if (dhcp_state->tries < 255) {
    dhcp_state->tries++;
  }
  /* back-off on retries, but to a maximum of 20 seconds for RENEW, 10 seccond for others */
  msecs = (u16_t)(dhcp_state->tries < 10 ? dhcp_state->tries * 1000 : 10 * 1000);
  if (state == DHCP_STATE_RENEWING) {
    msecs *= 2U;
  }
  dhcp_state->request_timeout = (u16_t)((msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS);
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                   "dhcp_re3: set request timeout %"U16_F" msecs\n", msecs);

  return result;
}

static err_t
dhcp_renew_client(struct netif *netif, struct dhcp_client *dhcp)
{
  return dhcp_re3(netif, dhcp, DHCP_STATE_RENEWING);
}

/**
 * @ingroup dhcp4
 * Renew an existing DHCP lease at the involved DHCP server.
 *
 * @param netif network interface which must renew its lease
 */
err_t
dhcp_renew(struct netif *netif)
{
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
  u8_t hwaddr_len;

  LWIP_ASSERT_CORE_LOCKED();
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_renew()\n"));
  LWIP_ERROR("netif != NULL", (netif != NULL), return ERR_ARG);

  netif_dhcp = netif_dhcp_data(netif);
  LWIP_ERROR("netif != NULL", (netif_dhcp != NULL), return ERR_VAL);
  netif_dhcp->client.cli_idx = LWIP_DHCP_NATIVE_IDX;
  dhcp_state = &((netif_dhcp->client.states)[LWIP_DHCP_NATIVE_IDX]);

  if (dhcp_idx_to_mac(netif, LWIP_DHCP_NATIVE_IDX, netif_dhcp->client.hwaddr, &hwaddr_len) != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_renew():get mac failed\n"));
    return ERR_VAL;
  }
  dhcp_state->hwaddr_len = hwaddr_len;

  return dhcp_renew_client(netif, &(netif_dhcp->client));
}

/**
 * Rebind with a DHCP server for an existing DHCP lease.
 *
 * @param netif network interface which must rebind with a DHCP server
 */
static err_t
dhcp_rebind(struct netif *netif, struct dhcp_client *dhcp)
{
  return dhcp_re3(netif, dhcp, DHCP_STATE_REBINDING);
}

/**
 * Enter REBOOTING state to verify an existing lease
 *
 * @param netif network interface which must reboot
 */
static err_t
dhcp_reboot(struct netif *netif)
{
  struct dhcp_client *dhcp = &(netif_dhcp_data(netif)->client);
  return dhcp_re3(netif, dhcp, DHCP_STATE_REBOOTING);
}

void
dhcp_release_and_stop(struct netif *netif)
{
  dhcp_stop(netif);
}

/**
 * @ingroup dhcp4
 * Release a DHCP lease (usually called before @ref dhcp_stop).
 *
 * @param netif network interface which must release its lease
 */
static err_t
dhcp_release_client(struct netif *netif, struct dhcp_client *dhcp)
{
  err_t result, ret;
  ip_addr_t server_ip_addr;
  u16_t options_out_len = 0;

  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  ip_addr_copy(server_ip_addr, dhcp->server_ip_addr);

  dhcp_state->lease_used = 0;

  if (dhcp->cli_cnt == 1) {
    dhcp->offered_t0_lease = dhcp->offered_t1_renew = dhcp->offered_t2_rebind = 0;
    dhcp->t0_timeout = 0;
    dhcp->t1_timeout = 0;
    dhcp->t2_timeout = 0;
  }

  if (!((dhcp_state->state == DHCP_STATE_BOUND) ||
        (dhcp_state->state == DHCP_STATE_RENEWING) ||
        (dhcp_state->state == DHCP_STATE_REBINDING) ||
        (dhcp_state->state == DHCP_STATE_RELEASING))) {
    /* clean old DHCP offer */
    if (dhcp->cli_cnt == 1) {
      ip_addr_set_zero_ip4(&dhcp->server_ip_addr);
      ip4_addr_set_zero(&dhcp->offered_sn_mask);
      ip4_addr_set_zero(&dhcp->offered_gw_addr);
      ip4_addr_set_zero(&dhcp->relay_ip);
    }
    dhcp_state->offered_ip_addr = 0;
    /* don't issue release message when address is not dhcp-assigned */
    return ERR_OK;
  }

  /* idle DHCP client */
  dhcp_set_state(dhcp, DHCP_STATE_OFF);
  /* create and initialize the DHCP message header */
  result = dhcp_create_msg(netif, dhcp, DHCP_RELEASE, &options_out_len);
  if (result == ERR_OK) {
    options_out_len = dhcp_option(options_out_len, dhcp->msg_out->options, DHCP_OPTION_SERVER_ID, 4);
    options_out_len = dhcp_option_long(options_out_len, dhcp->msg_out->options,
                                       lwip_ntohl(ip4_addr_get_u32(ip_2_ip4(&dhcp->server_ip_addr))));
    dhcp_fill_options(netif, dhcp, DHCP_RELEASE, 0, &options_out_len);
    ret = udp_sendto_if(dhcp_pcb, dhcp->p_out, &server_ip_addr, LWIP_IANA_PORT_DHCP_SERVER, netif);
    (void)ret;
    dhcp_delete_msg(dhcp);
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp_release: RELEASED, DHCP_STATE_OFF\n"));
  } else {
    /* sending release failed, but that's not a problem since the correct behaviour of dhcp does not rely on release */
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                     ("dhcp_release: could not allocate DHCP request\n"));
  }
  /* clean old DHCP offer */
  if (dhcp->cli_cnt == 1) {
    ip_addr_set_zero_ip4(&dhcp->server_ip_addr);
    ip4_addr_set_zero(&dhcp->offered_sn_mask);
    ip4_addr_set_zero(&dhcp->offered_gw_addr);
    ip4_addr_set_zero(&dhcp->relay_ip);
  }
  dhcp_state->offered_ip_addr = 0;
#if LWIP_NAT64
  nat64_dhcp_ip4_event(dhcp->hwaddr, dhcp_state->hwaddr_len, NULL, NAT64_DHCP_EVENT_RELEASE);
#endif

  return result;
}


/**
 * @ingroup dhcp4
 * Release a DHCP lease (usually called before @ref dhcp_stop).
 *
 * @param netif network interface which must release its lease
 */
err_t
dhcp_release(struct netif *netif)
{
  err_t result;
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
  u8_t hwaddr_len;
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_release()\n"));
  LWIP_ERROR("netif != NULL", (netif != NULL), return ERR_ARG);
  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp == NULL) {
    return ERR_ARG;
  }

  netif_dhcp->client.cli_idx = LWIP_DHCP_NATIVE_IDX;
  dhcp_state = &((netif_dhcp->client.states)[LWIP_DHCP_NATIVE_IDX]);
  if (dhcp_state->state == DHCP_STATE_OFF) {
    return ERR_ARG;
  }

  if (dhcp_idx_to_mac(netif, LWIP_DHCP_NATIVE_IDX, netif_dhcp->client.hwaddr, &hwaddr_len) != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_release():get mac failed\n"));
    return ERR_VAL;
  }
  dhcp_state->hwaddr_len = hwaddr_len;

#if LWIP_DHCP_BOOTP_FILE
  ip4_addr_set_zero(&netif_dhcp->offered_si_addr);
#endif /* LWIP_DHCP_BOOTP_FILE */

  result = dhcp_release_client(netif, &(netif_dhcp->client));

  /* remove IP address from interface (prevents routing from selecting this interface) */
  (void)netif_set_addr(netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);

  return result;
}

static void
dhcp_stop_client(struct netif *netif, struct dhcp_client *dhcp)
{
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  netif_dhcp = netif_dhcp_data(netif);

  LWIP_ASSERT("reply wasn't freed", dhcp->msg_in == NULL);
#if (LWIP_DHCP_SUBSTITUTE && LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS)
  dhcp_concurrent_stop_client(dhcp);
#endif
  if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
    (void)dhcp_release(netif);
    if (netif_dhcp->pcb_allocated != 0) {
      dhcp_dec_pcb_refcount(); /* free DHCP PCB if not needed any more */
      netif_dhcp->pcb_allocated = 0;
    }
  } else {
#if LWIP_DHCP_SUBSTITUTE
    (void)dhcp_release_client(netif, dhcp);
    dhcp_dec_pcb_refcount();
#endif /* LWIP_DHCP_SUBSTITUTE */
  }
  dhcp_set_state(dhcp, DHCP_STATE_OFF);
  (void)memset_s(dhcp_state, sizeof(struct dhcp_state), 0, sizeof(struct dhcp_state));
  dhcp_clients_count_update(dhcp);
}

/**
 * @ingroup dhcp4
 * Remove the DHCP client from the interface.
 *
 * @param netif The network interface to stop DHCP on
 */
void
dhcp_stop(struct netif *netif)
{
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
  u8_t hwaddr_len;
  LWIP_ERROR("dhcp_stop: netif != NULL", (netif != NULL), return);
  netif_dhcp = netif_dhcp_data(netif);
  netif->flags = netif->flags & (~NETIF_FLAG_DHCP);
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_stop()\n"));
  if (netif_dhcp != NULL) {
#if LWIP_DHCP_AUTOIP_COOP
    if (netif_dhcp->autoip_coop_state == DHCP_AUTOIP_COOP_STATE_ON) {
      autoip_stop(netif);
      netif_dhcp->autoip_coop_state = DHCP_AUTOIP_COOP_STATE_OFF;
    }
#endif /* LWIP_DHCP_AUTOIP_COOP */
    netif_dhcp->client.cli_idx = LWIP_DHCP_NATIVE_IDX;
    dhcp_state = &((netif_dhcp->client.states)[LWIP_DHCP_NATIVE_IDX]);
    if (dhcp_idx_to_mac(netif, LWIP_DHCP_NATIVE_IDX, netif_dhcp->client.hwaddr, &hwaddr_len) != ERR_OK) {
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_stop():get mac failed\n"));
      return;
    }
    dhcp_state->hwaddr_len = hwaddr_len;
    dhcp_stop_client(netif, &(netif_dhcp->client));
  }
}

/*
 * Set the DHCP state of a DHCP client.
 *
 * If the state changed, reset the number of tries.
 */
static void
dhcp_set_state(struct dhcp_client *dhcp, u8_t new_state)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  if (new_state != dhcp_state->state) {
    LWIP_DEBUGF_LOG2(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_set_state:%hhu to %hhu\n", dhcp_state->state, new_state);
    dhcp_state->state = new_state;
    dhcp_state->tries = 0;
    dhcp_state->request_timeout = 0;
  }
}

/*
 * Concatenate an option type and length field to the outgoing
 * DHCP message.
 *
 */
static u16_t
dhcp_option(u16_t options_out_len, u8_t *options, u8_t option_type, u8_t option_len)
{
  LWIP_ASSERT("dhcp_option: options_out_len + 2 + option_len <= DHCP_OPTIONS_LEN", options_out_len + 2U + option_len <= DHCP_OPTIONS_LEN);
  options[options_out_len++] = option_type;
  options[options_out_len++] = option_len;
  return options_out_len;
}
/*
 * Concatenate a single byte to the outgoing DHCP message.
 *
 */
static u16_t
dhcp_option_byte(u16_t options_out_len, u8_t *options, u8_t value)
{
  LWIP_ASSERT("dhcp_option_byte: options_out_len < DHCP_OPTIONS_LEN", options_out_len < DHCP_OPTIONS_LEN);
  options[options_out_len++] = value;
  return options_out_len;
}

static u16_t
dhcp_option_short(u16_t options_out_len, u8_t *options, u16_t value)
{
  LWIP_ASSERT("dhcp_option_short: options_out_len + 2 <= DHCP_OPTIONS_LEN", options_out_len + 2U <= DHCP_OPTIONS_LEN);
  options[options_out_len++] = (u8_t)((value & 0xff00U) >> 8);
  options[options_out_len++] = (u8_t) (value & 0x00ffU);
  return options_out_len;
}

static u16_t
dhcp_option_long(u16_t options_out_len, u8_t *options, u32_t value)
{
  LWIP_ASSERT("dhcp_option_long: options_out_len + 4 <= DHCP_OPTIONS_LEN", options_out_len + 4U <= DHCP_OPTIONS_LEN);
  options[options_out_len++] = (u8_t)((value & 0xff000000UL) >> 24);
  options[options_out_len++] = (u8_t)((value & 0x00ff0000UL) >> 16);
  options[options_out_len++] = (u8_t)((value & 0x0000ff00UL) >> 8);
  options[options_out_len++] = (u8_t)((value & 0x000000ffUL));
  return options_out_len;
}

#if LWIP_NETIF_HOSTNAME
static u16_t
dhcp_option_hostname(u16_t options_out_len, u8_t *options, struct netif *netif)
{
  const char *p = NULL;
  char dhcp_hostname[NETIF_HOSTNAME_MAX_LEN];
  size_t namelen = strlen(netif->hostname);
  if (namelen > 0) {
    p = netif->hostname;
  }

  if (p == NULL) {
    if (snprintf_s(dhcp_hostname, NETIF_HOSTNAME_MAX_LEN,
                   NETIF_HOSTNAME_MAX_LEN - 1, "%02x%02x%02x%02x%02x%02x",
                   netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
                   netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]) <= 0) {
      return options_out_len;
    }
    dhcp_hostname[NETIF_HOSTNAME_MAX_LEN - 1] = '\0';
    p = dhcp_hostname;
  }

  namelen = strlen(p);
  /* Validate length against available bytes (need 2 bytes for OPTION_HOSTNAME
     and 1 byte for trailer) */
  LWIP_ASSERT("DHCP: hostname is too long!", namelen + 3 + options_out_len <= DHCP_OPTIONS_LEN);
  options_out_len = dhcp_option(options_out_len, options, DHCP_OPTION_HOSTNAME, (u8_t)namelen);
  while (namelen--) {
    options_out_len = dhcp_option_byte(options_out_len, options, (u8_t)(*p++));
  }

  return options_out_len;
}
#endif /* LWIP_NETIF_HOSTNAME */

#if LWIP_DHCP_VENDOR_CLASS_IDENTIFIER
static u16_t
dhcp_option_vci(u16_t options_out_len, u8_t *options, const struct netif *netif)
{
  const char *p = NULL;
  u8_t len;
  size_t vci_len;
  size_t available;

  LWIP_UNUSED_ARG(netif);

  vci_len = g_vci_info.vci_len;
  if (vci_len > 0) {
    p = g_vci_info.vci;
  } else {
    return options_out_len;
  }

  /* Shrink len to available bytes (need 2 bytes for DHCP_OPTION_VCI
     and 1 byte for trailer) */
  available = DHCP_OPTIONS_LEN - options_out_len - 3;
  LWIP_ASSERT("DHCP: vci is too long!", vci_len <= available);
  len = (u8_t)LWIP_MIN(vci_len, available);
  options_out_len = dhcp_option(options_out_len, options, DHCP_OPTION_VCI, (u8_t)len);
  while (len--) {
    options_out_len = dhcp_option_byte(options_out_len, options, *p++);
  }
  return options_out_len;
}

err_t
dhcp_set_vci(const char *vci, u8_t vci_len)
{
  if (memcpy_s(g_vci_info.vci, DHCP_VCI_MAX_LEN, vci, vci_len) == EOK) {
    g_vci_info.vci_len = vci_len;
    return ERR_OK;
  }
  return ERR_MEM;
}

err_t
dhcp_get_vci(char *vci, u8_t *vci_len)
{
  if (g_vci_info.vci_len == 0) {
    *vci_len = 0;
    return ERR_VAL;
  } else {
    if (memcpy_s(vci, *vci_len, g_vci_info.vci, g_vci_info.vci_len) == EOK) {
      *vci_len = g_vci_info.vci_len;
      return ERR_OK;
    } else {
      *vci_len = 0;
      return ERR_VAL;
    }
  }
}
#endif /* LWIP_DHCP_VENDOR_CLASS_IDENTIFIER */

/**
 * Extract the DHCP message and the DHCP options.
 *
 * Extract the DHCP message and the DHCP options, each into a contiguous
 * piece of memory. As a DHCP message is variable sized by its options,
 * and also allows overriding some fields for options, the easy approach
 * is to first unfold the options into a contiguous piece of memory, and
 * use that further on.
 *
 */
static err_t
dhcp_parse_reply(struct dhcp *netif_dhcp, struct dhcp_client *dhcp, struct pbuf *p)
{
  u8_t *options = NULL;
  u16_t offset;
  u16_t offset_max;
  u16_t options_offset;
  u16_t options_idx;
  u16_t options_idx_max;
  struct pbuf *q = NULL;
  int parse_file_as_options = 0;
  int parse_sname_as_options = 0;
#if LWIP_DHCP_BOOTP_FILE
  int file_overloaded = 0;
#endif
  (void)netif_dhcp;

  /* clear received options */
  dhcp_clear_all_options(dhcp);
  /* check that beginning of dhcp_msg (up to and including chaddr) is in first pbuf */
  if (p->len < DHCP_SNAME_OFS) {
    return ERR_BUF;
  }
  dhcp->msg_in = (struct dhcp_msg *)p->payload;
#if LWIP_DHCP_BOOTP_FILE
  /* clear boot file name */
  if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
    netif_dhcp->boot_file_name[0] = 0;
  }
#endif /* LWIP_DHCP_BOOTP_FILE */

  /* parse options */

  /* start with options field */
  options_idx = DHCP_OPTIONS_OFS;
  /* parse options to the end of the received packet */
  options_idx_max = p->tot_len;
again:
  q = p;
  options_offset = options_idx;
  while ((q != NULL) && (options_idx >= q->len)) {
    options_idx = (u16_t)(options_idx - q->len);
    options_idx_max = (u16_t)(options_idx_max - q->len);
    q = q->next;
  }
  if (q == NULL) {
    return ERR_BUF;
  }
  offset = options_idx;
  offset_max = options_idx_max;
  options = (u8_t *)q->payload;
  /* at least 1 byte to read and no end marker, then at least 3 bytes to read? */
  while ((q != NULL) && (offset < offset_max) && (options[offset] != DHCP_OPTION_END)) {
    u8_t op = options[offset];
    u8_t len;
    u8_t decode_len = 0;
    int decode_idx = 0;
    u16_t val_offset = (u16_t)(offset + 2);
    if (val_offset < offset) {
      /* overflow */
      return ERR_BUF;
    }
    /* len byte might be in the next pbuf */
    if ((offset + 1) < q->len) {
      len = options[offset + 1];
    } else {
      len = (q->next != NULL ? ((u8_t *)q->next->payload)[0] : 0);
    }
    /* LWIP_DEBUGF(DHCP_DEBUG, ("msg_offset=%"U16_F", q->len=%"U16_F, msg_offset, q->len)); */
    decode_len = len;
    switch (op) {
      /* case(DHCP_OPTION_END): handled above */
      case (DHCP_OPTION_PAD):
        /* special option: no len encoded */
        decode_len = len = 0;
        /* will be increased below */
        break;
      case (DHCP_OPTION_SUBNET_MASK):
        LWIP_DHCP_INPUT_ERROR("len == 4", len == 4, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_SUBNET_MASK;
        break;
      case (DHCP_OPTION_ROUTER):
        decode_len = 4; /* only copy the first given router */
        LWIP_DHCP_INPUT_ERROR("len >= decode_len", len >= decode_len, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_ROUTER;
        break;
#if LWIP_DHCP_PROVIDE_DNS_SERVERS
      case (DHCP_OPTION_DNS_SERVER):
        /* special case: there might be more than one server */
        LWIP_DHCP_INPUT_ERROR("len %% 4 == 0", len % 4 == 0, return ERR_VAL;);
        /* limit number of DNS servers */
#if DNS_MAX_SERVERS > 64
#error "Max number of servers can not be greater than 64"
#endif
        decode_len = LWIP_MIN(len, 4 * DNS_MAX_SERVERS);
        LWIP_DHCP_INPUT_ERROR("len >= decode_len", len >= decode_len, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_DNS_SERVER;
        break;
#endif /* LWIP_DHCP_PROVIDE_DNS_SERVERS */
      case (DHCP_OPTION_LEASE_TIME):
        LWIP_DHCP_INPUT_ERROR("len == 4", len == 4, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_LEASE_TIME;
        break;
#if LWIP_DHCP_GET_NTP_SRV
      case (DHCP_OPTION_NTP):
        /* special case: there might be more than one server */
        LWIP_DHCP_INPUT_ERROR("len %% 4 == 0", len % 4 == 0, return ERR_VAL;);
        /* limit number of NTP servers */
        decode_len = LWIP_MIN(len, 4 * LWIP_DHCP_MAX_NTP_SERVERS);
        LWIP_DHCP_INPUT_ERROR("len >= decode_len", len >= decode_len, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_NTP_SERVER;
        break;
#endif /* LWIP_DHCP_GET_NTP_SRV*/
      case (DHCP_OPTION_OVERLOAD):
        LWIP_DHCP_INPUT_ERROR("len == 1", len == 1, return ERR_VAL;);
        /* decode overload only in options, not in file/sname: invalid packet */
        LWIP_DHCP_INPUT_ERROR("overload in file/sname", options_offset == DHCP_OPTIONS_OFS, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_OVERLOAD;
        break;
      case (DHCP_OPTION_MESSAGE_TYPE):
        LWIP_DHCP_INPUT_ERROR("len == 1", len == 1, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_MSG_TYPE;
        break;
      case (DHCP_OPTION_SERVER_ID):
        LWIP_DHCP_INPUT_ERROR("len == 4", len == 4, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_SERVER_ID;
        break;
      case (DHCP_OPTION_T1):
        LWIP_DHCP_INPUT_ERROR("len == 4", len == 4, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_T1;
        break;
      case (DHCP_OPTION_T2):
        LWIP_DHCP_INPUT_ERROR("len == 4", len == 4, return ERR_VAL;);
        decode_idx = DHCP_OPTION_IDX_T2;
        break;
      default:
        decode_len = 0;
        LWIP_DEBUGF_LOG1(DHCP_DEBUG, "skipping option %"U16_F" in options\n", (u16_t)op);
        LWIP_HOOK_DHCP_PARSE_OPTION(ip_current_netif(), dhcp, dhcp->state, dhcp->msg_in,
                                    dhcp_option_given(dhcp, DHCP_OPTION_IDX_MSG_TYPE) ? (u8_t)dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_MSG_TYPE) : 0,
                                    op, len, q, val_offset);
        break;
    }
    if (op == DHCP_OPTION_PAD) {
      offset++;
    } else {
      if (offset + len + 2 > 0xFFFF) {
        /* overflow */
        return ERR_BUF;
      }
      offset = (u16_t)(offset + len + 2);
      if (decode_len > 0) {
        u32_t value = 0;
        u16_t copy_len;
decode_next:
        LWIP_ASSERT("check decode_idx", decode_idx >= 0 && decode_idx < DHCP_OPTION_IDX_MAX);
        if (!dhcp_option_given(dhcp, decode_idx)) {
          copy_len = LWIP_MIN(decode_len, 4);
          if (pbuf_copy_partial(q, &value, copy_len, val_offset) != copy_len) {
            return ERR_BUF;
          }
          if (decode_len > 4) {
            /* decode more than one u32_t */
            u16_t next_val_offset;
            LWIP_DHCP_INPUT_ERROR("decode_len %% 4 == 0", decode_len % 4 == 0, return ERR_VAL;);
            dhcp_got_option(dhcp, decode_idx);
            dhcp_set_option_value(dhcp, decode_idx, lwip_htonl(value));
            decode_len = (u8_t)(decode_len - 4);
            next_val_offset = (u16_t)(val_offset + 4);
            if (next_val_offset < val_offset) {
              /* overflow */
              return ERR_BUF;
            }
            val_offset = next_val_offset;
            decode_idx++;
            goto decode_next;
          } else if (decode_len == 4) {
            value = lwip_ntohl(value);
          } else {
            LWIP_DHCP_INPUT_ERROR("invalid decode_len", decode_len == 1, return ERR_VAL;);
            value = ((u8_t *)&value)[0];
          }
          dhcp_got_option(dhcp, decode_idx);
          dhcp_set_option_value(dhcp, decode_idx, value);
        }
      }
    }
    if (offset >= q->len) {
      offset = (u16_t)(offset - q->len);
      offset_max = (u16_t)(offset_max - q->len);
      if (offset < offset_max) {
        q = q->next;
        LWIP_DHCP_INPUT_ERROR("next pbuf was null", q != NULL, return ERR_VAL;);
        options = (u8_t *)q->payload;
      } else {
        /* We've run out of bytes, probably no end marker. Don't proceed. */
        return ERR_BUF;
      }
    }
  }
  /* is this an overloaded message? */
  if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_OVERLOAD)) {
    u32_t overload = dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_OVERLOAD);
    dhcp_clear_option(dhcp, DHCP_OPTION_IDX_OVERLOAD);
    if (overload == DHCP_OVERLOAD_FILE) {
      parse_file_as_options = 1;
      LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("overloaded file field\n"));
    } else if (overload == DHCP_OVERLOAD_SNAME) {
      parse_sname_as_options = 1;
      LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("overloaded sname field\n"));
    } else if (overload == DHCP_OVERLOAD_SNAME_FILE) {
      parse_sname_as_options = 1;
      parse_file_as_options = 1;
      LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("overloaded sname and file field\n"));
    } else {
      LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("invalid overload option: %d\n", (int)overload));
    }
  }
  if (parse_file_as_options) {
    /* if both are overloaded, parse file first and then sname (RFC 2131 ch. 4.1) */
    parse_file_as_options = 0;
    options_idx = DHCP_FILE_OFS;
    options_idx_max = DHCP_FILE_OFS + DHCP_FILE_LEN;
#if LWIP_DHCP_BOOTP_FILE
    file_overloaded = 1;
#endif
    goto again;
  } else if (parse_sname_as_options) {
    parse_sname_as_options = 0;
    options_idx = DHCP_SNAME_OFS;
    options_idx_max = DHCP_SNAME_OFS + DHCP_SNAME_LEN;
    goto again;
  }
#if LWIP_DHCP_BOOTP_FILE
  if (!file_overloaded) {
    /* only do this for ACK messages */
    if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_MSG_TYPE) &&
        (dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_MSG_TYPE) == DHCP_ACK)) {
      /* copy bootp file name, don't care for sname (server hostname) */
      if (pbuf_copy_partial(p, netif_dhcp->boot_file_name, DHCP_FILE_LEN - 1, DHCP_FILE_OFS) != (DHCP_FILE_LEN - 1)) {
        return ERR_BUF;
      }
    }
    /* make sure the string is really NULL-terminated */
    netif_dhcp->boot_file_name[DHCP_FILE_LEN - 1] = 0;
  }
#endif /* LWIP_DHCP_BOOTP_FILE */
  return ERR_OK;
}

/**
 * If an incoming DHCP message is in response to us, then trigger the state machine
 */
static void
dhcp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  struct netif *netif = ip_current_input_netif();
  struct dhcp *netif_dhcp = netif_dhcp_data(netif);
  struct dhcp_client *dhcp = NULL;
  struct dhcp_msg *reply_msg = (struct dhcp_msg *)p->payload;
  u8_t msg_type;
  dhcp_num_t mac_idx;
  struct dhcp_state *dhcp_state = NULL;
  u8_t hwaddr_len;
  u32_t xid;
#if LWIP_DHCP_SUBSTITUTE
  ip_addr_t server_id;
#endif /* LWIP_DHCP_SUBSTITUTE */

  LWIP_UNUSED_ARG(arg);

  /* Caught DHCP message from netif that does not have DHCP enabled? -> not interested */
  if ((netif_dhcp == NULL) || (dhcp_pcb_refcount == 0)) {
    goto free_pbuf_and_return;
  }

  LWIP_ASSERT("invalid server address type", IP_IS_V4(addr));

  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_recv(pbuf = %p) from DHCP server %"U16_F".%"U16_F".%"U16_F".%"U16_F" port %"U16_F"\n", (void *)p,
              ip4_addr1_16(ip_2_ip4(addr)), ip4_addr2_16(ip_2_ip4(addr)), ip4_addr3_16(ip_2_ip4(addr)), ip4_addr4_16(ip_2_ip4(addr)), port));
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("pbuf->len = %"U16_F"\n", p->len));
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("pbuf->tot_len = %"U16_F"\n", p->tot_len));
  /* prevent warnings about unused arguments */
  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(addr);
  LWIP_UNUSED_ARG(port);

  if (p->len < DHCP_MIN_REPLY_LEN) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("DHCP reply message or pbuf too short\n"));
    goto free_pbuf_and_return;
  }

  if (reply_msg->op != DHCP_BOOTREPLY) {
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                     "not a DHCP reply message, but type %"U16_F"\n", (u16_t)reply_msg->op);
    goto free_pbuf_and_return;
  }

  dhcp = &(netif_dhcp->client);

  if (dhcp_mac_to_idx(netif, reply_msg->chaddr, reply_msg->hlen, &mac_idx) != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_recv(): mac idx failed\n"));
    goto free_pbuf_and_return;
  }
  if (dhcp_client_find_by_mac_idx(dhcp, mac_idx, &(dhcp->cli_idx)) != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_recv(): no client\n"));
    goto free_pbuf_and_return;
  }
  dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  if (dhcp_idx_to_mac(netif, dhcp_state->idx, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_recv(): get mac failed\n"));
    goto free_pbuf_and_return;
  }
  dhcp_state->hwaddr_len = hwaddr_len;

  LWIP_ASSERT("reply wasn't freed", dhcp->msg_in == NULL);

  /* match transaction ID against what we expected */
  DHCP_XID(xid, dhcp->hwaddr, dhcp_state->hwaddr_len, dhcp_state->xid);
  if (lwip_ntohl(reply_msg->xid) != xid) {
    LWIP_DEBUGF_LOG2(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                     "transaction id mismatch reply_msg->xid(%"X32_F")!=dhcp->xid(%"X32_F")\n",
                     lwip_ntohl(reply_msg->xid), xid);
    goto free_pbuf_and_return;
  }
  /* option fields could be unfold? */
  if (dhcp_parse_reply(netif_dhcp, dhcp, p) != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                     ("problem unfolding DHCP message - too short on memory?\n"));
    goto free_pbuf_and_return;
  }

#if LWIP_DHCP_SUBSTITUTE
  /* to check if the server changed */
  if (dhcp_option_given(dhcp, DHCP_OPTION_IDX_SERVER_ID)) {
    ip_addr_set_ip4_u32_val(server_id, lwip_htonl(dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_SERVER_ID)));
    if (!(ip4_addr_isany_val(dhcp->server_ip_addr.u_addr.ip4)) &&
        !(ip4_addr_cmp(ip_2_ip4(&server_id), ip_2_ip4(&(dhcp->server_ip_addr))))) {
      LWIP_DEBUGF_LOG2(DHCP_DEBUG | LWIP_DBG_TRACE,
                       "%u diff serv_id %u\n", dhcp->cli_idx, ip_2_ip4(&server_id)->addr);
      if (dhcp->cli_idx != LWIP_DHCP_NATIVE_IDX) {
        goto free_pbuf_and_return;
      }
      dhcp_substitute_clients_restart(netif, dhcp);
      dhcp->cli_idx = LWIP_DHCP_NATIVE_IDX;
      if (dhcp_idx_to_mac(netif, dhcp_state->idx, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
        LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_recv(): get mac failed\n"));
        goto free_pbuf_and_return;
      }
      dhcp_state->hwaddr_len = hwaddr_len;
    }
  } else {
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                     "dhcp_recv(netif=%p) did not get server ID!\n", (void *)netif);
    goto free_pbuf_and_return;
  }
#endif /* LWIP_DHCP_SUBSTITUTE */
  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("searching DHCP_OPTION_MESSAGE_TYPE\n"));
  /* obtain pointer to DHCP message type */
  if (!dhcp_option_given(dhcp, DHCP_OPTION_IDX_MSG_TYPE)) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                     ("DHCP_OPTION_MESSAGE_TYPE option not found\n"));
    goto free_pbuf_and_return;
  }

  /* read DHCP message type */
  msg_type = (u8_t)dhcp_get_option_value(dhcp, DHCP_OPTION_IDX_MSG_TYPE);
  /* message type is DHCP ACK? */
  if (msg_type == DHCP_ACK) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("DHCP_ACK received\n"));
    /* in requesting state or just reconnected to the network? */
    if ((dhcp_state->state == DHCP_STATE_REQUESTING) ||
        (dhcp_state->state == DHCP_STATE_REBOOTING)) {
      dhcp_handle_ack(netif, dhcp);
#if LWIP_DHCP_SUBSTITUTE
      ip4_addr_t cli_ip;
      dhcp_get_client_ip(&cli_ip.addr, dhcp, dhcp_state);
      if (dhcp_addr_clients_check(netif_dhcp, &cli_ip) == lwIP_TRUE) {
        LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                         ("addr been used by substitute client\n"));
        (void)dhcp_decline(netif, dhcp);
        goto free_pbuf_and_return;
      }
#endif /* LWIP_DHCP_SUBSTITUTE */
#if DHCP_DOES_ARP_CHECK
      if (
#if LWIP_DHCP_SUBSTITUTE_MMBR
          (dhcp_state->addr_not_dup_check == lwIP_FALSE) &&
#endif
          ((netif->flags & NETIF_FLAG_ETHARP) != 0)) {
        /* check if the acknowledged lease address is already in use */
        dhcp_check(netif, dhcp);
      } else {
        /* bind interface to the acknowledged lease address */
        dhcp_bind(netif, dhcp);
      }
#else
      /* bind interface to the acknowledged lease address */
      dhcp_bind(netif, dhcp);
#endif
#if LWIP_DHCP_SUBSTITUTE_MMBR
      dhcp_state->addr_not_dup_check = lwIP_FALSE;
#endif /* LWIP_DHCP_SUBSTITUTE_MMBR */
    }
    /* already bound to the given lease address? */
    else if ((dhcp_state->state == DHCP_STATE_REBINDING) ||
             (dhcp_state->state == DHCP_STATE_RENEWING)) {
      dhcp_handle_ack(netif, dhcp);
      dhcp_bind(netif, dhcp);
    }
  }
  /* received a DHCP_NAK in appropriate state? */
  else if ((msg_type == DHCP_NAK) &&
           ((dhcp_state->state == DHCP_STATE_REBOOTING) || (dhcp_state->state == DHCP_STATE_REQUESTING) ||
            (dhcp_state->state == DHCP_STATE_REBINDING) || (dhcp_state->state == DHCP_STATE_RENEWING))) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("DHCP_NAK received\n"));
#if LWIP_DHCP_SUBSTITUTE_MMBR
    /* ignore the preferred IP if receive NAK */
    dhcp_state->addr_not_dup_check = lwIP_FALSE;
#endif /* LWIP_DHCP_SUBSTITUTE_MMBR */
    dhcp_handle_nak(netif, dhcp);
  }
  /* received a DHCP_OFFER in DHCP_STATE_SELECTING state? */
  else if ((msg_type == DHCP_OFFER) && (dhcp_state->state == DHCP_STATE_SELECTING)) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("DHCP_OFFER received in DHCP_STATE_SELECTING state\n"));
    /* remember offered lease */
    dhcp_handle_offer(netif, dhcp);
  }

free_pbuf_and_return:
  if (dhcp != NULL) {
    dhcp->msg_in = NULL;
  }
  pbuf_free(p);
}

/**
 * Create a DHCP request, fill in common headers
 *
 * @param netif the netif under DHCP control
 * @param dhcp dhcp control struct
 * @param message_type message type of the request
 */
static err_t
dhcp_create_msg(struct netif *netif, struct dhcp_client *dhcp, u8_t message_type, u16_t *options_out_len)
{
  u16_t i;
  ip4_addr_t cli_ip;
  u16_t options_out_len_loc;
  struct dhcp_state *dhcp_state = NULL;

#ifndef DHCP_GLOBAL_XID
  /** default global transaction identifier starting value (easy to match
   *  with a packet analyser). We simply increment for each new request.
   *  Predefine DHCP_GLOBAL_XID to a better value or a function call to generate one
   *  at runtime, any supporting function prototypes can be defined in DHCP_GLOBAL_XID_HEADER */
#if DHCP_CREATE_RAND_XID && defined(LWIP_RAND)
  static u32_t xid;
#else /* DHCP_CREATE_RAND_XID && defined(LWIP_RAND) */
  static u32_t xid = 0xABCD0000;
#endif /* DHCP_CREATE_RAND_XID && defined(LWIP_RAND) */
#else
  if (!xid_initialised) {
    xid = DHCP_GLOBAL_XID;
    xid_initialised = !xid_initialised;
  }
#endif
  LWIP_ERROR("dhcp_create_msg: netif != NULL", (netif != NULL), return ERR_ARG);
  LWIP_ERROR("dhcp_create_msg: dhcp != NULL", (dhcp != NULL), return ERR_VAL);
  LWIP_ASSERT("dhcp_create_msg: dhcp->p_out == NULL", dhcp->p_out == NULL);
  LWIP_ASSERT("dhcp_create_msg: dhcp->msg_out == NULL", dhcp->msg_out == NULL);
  dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  dhcp->p_out = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcp_msg), PBUF_RAM);
  if (dhcp->p_out == NULL) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                     ("dhcp_create_msg(): could not allocate pbuf\n"));
    return ERR_MEM;
  }
  LWIP_ASSERT("dhcp_create_msg: check that first pbuf can hold struct dhcp_msg",
              (dhcp->p_out->len >= sizeof(struct dhcp_msg)));

#if DRIVER_STATUS_CHECK
  dhcp->p_out->flags |= PBUF_FLAG_DHCP_BUF;
#endif

  /* DHCP_REQUEST should reuse 'xid' from DHCPOFFER */
  if ((message_type != DHCP_REQUEST) || (dhcp_state->state == DHCP_STATE_REBOOTING)) {
    /* reuse transaction identifier in retransmissions */
    if (dhcp_state->tries == 0) {
#if DHCP_CREATE_RAND_XID && defined(LWIP_RAND)
      xid = (u32_t)LWIP_RAND();
#else /* DHCP_CREATE_RAND_XID && defined(LWIP_RAND) */
      xid++;
#endif /* DHCP_CREATE_RAND_XID && defined(LWIP_RAND) */
      dhcp_state->xid = (u8_t)xid;
    }
  }
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE,
                   "transaction id xid(%"X32_F")\n", xid);

  dhcp->msg_out = (struct dhcp_msg *)dhcp->p_out->payload;
  memset(dhcp->msg_out, 0, sizeof(struct dhcp_msg));

  dhcp->msg_out->op = DHCP_BOOTREQUEST;
#if LWIP_ALWAYS_SEND_HWTYPE_AS_ETHER_IN_DHCP
  dhcp->msg_out->htype = LWIP_IANA_HWTYPE_ETHERNET;
#else
  dhcp->msg_out->htype = (u8_t)(netif->link_layer_type & 0xFF);
#endif
  dhcp->msg_out->hlen = dhcp_state->hwaddr_len;
  dhcp->msg_out->hops = 0;
  DHCP_XID(dhcp->msg_out->xid, dhcp->hwaddr, dhcp_state->hwaddr_len, dhcp_state->xid);
  dhcp->msg_out->xid = lwip_htonl(dhcp->msg_out->xid);
  dhcp->msg_out->secs = 0;
  /* we don't need the broadcast flag since we can receive unicast traffic
     before being fully configured! */
  dhcp->msg_out->flags = 0;
#if LWIP_DHCP_SUBSTITUTE
  if (dhcp->cli_idx != LWIP_DHCP_NATIVE_IDX) {
    dhcp->msg_out->flags |= lwip_htons(DHCP_BROADCAST_FLAG);
  }
#endif /* LWIP_DHCP_SUBSTITUTE */
  ip4_addr_set_zero(&dhcp->msg_out->ciaddr);
  /* set ciaddr to dhcp->offered_ip_addr based on message_type and state */
  if (message_type == DHCP_INFORM) {
    ip4_addr_copy(dhcp->msg_out->ciaddr, *netif_ip4_addr(netif));
  }
  if (((message_type == DHCP_DECLINE) &&
       ((dhcp_state->state == DHCP_STATE_BOUND) ||
        (dhcp_state->state == DHCP_STATE_RENEWING) ||
        (dhcp_state->state == DHCP_STATE_REBINDING))) ||
      (message_type == DHCP_RELEASE) ||
      ((message_type == DHCP_REQUEST) && /* DHCP_STATE_BOUND not used for sending! */
       ((dhcp_state->state == DHCP_STATE_RENEWING) ||
        dhcp_state->state == DHCP_STATE_REBINDING))) {
    dhcp_get_client_ip(&cli_ip.addr, dhcp, dhcp_state);
    ip4_addr_copy(dhcp->msg_out->ciaddr, cli_ip);
  }
  ip4_addr_set_zero(&dhcp->msg_out->yiaddr);
  ip4_addr_set_zero(&dhcp->msg_out->siaddr);
  ip4_addr_set_zero(&dhcp->msg_out->giaddr);
  for (i = 0; i < LWIP_MIN(DHCP_CHADDR_LEN, NETIF_MAX_HWADDR_LEN); i++) {
    /* copy netif hardware address (padded with zeroes through memset already) */
    dhcp->msg_out->chaddr[i] = (u8_t)((i < dhcp_state->hwaddr_len &&
                                       i < NETIF_MAX_HWADDR_LEN) ? dhcp->hwaddr[i] : 0); /* pad byte */
  }
  for (i = 0; i < DHCP_SNAME_LEN; i++) {
    dhcp->msg_out->sname[i] = 0;
  }
  for (i = 0; i < DHCP_FILE_LEN; i++) {
    dhcp->msg_out->file[i] = 0;
  }
  dhcp->msg_out->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
  dhcp->options_out_len = 0;
  /* fill options field with an incrementing array (for debugging purposes) */
  for (i = 0; i < DHCP_OPTIONS_LEN; i++) {
    dhcp->msg_out->options[i] = (u8_t)i; /* for debugging only, no matter if truncated */
  }
  /* Add option MESSAGE_TYPE */
  options_out_len_loc = dhcp_option(0, dhcp->msg_out->options, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
  options_out_len_loc = dhcp_option_byte(options_out_len_loc, dhcp->msg_out->options, message_type);
  if (options_out_len) {
    *options_out_len = options_out_len_loc;
  }
  return ERR_OK;
}

/**
 * Free previously allocated memory used to send a DHCP request.
 *
 * @param dhcp the dhcp struct to free the request from
 */
static void
dhcp_delete_msg(struct dhcp_client *dhcp)
{
  LWIP_ERROR("dhcp_delete_msg: dhcp != NULL", (dhcp != NULL), return);
  LWIP_ASSERT("dhcp_delete_msg: dhcp->p_out != NULL", dhcp->p_out != NULL);
  LWIP_ASSERT("dhcp_delete_msg: dhcp->msg_out != NULL", dhcp->msg_out != NULL);
  if (dhcp->p_out != NULL) {
    (void)pbuf_free(dhcp->p_out);
  }
  dhcp->p_out = NULL;
  dhcp->msg_out = NULL;
}

/**
 * Add a DHCP message trailer
 *
 * Adds the END option to the DHCP message, and if
 * necessary, up to three padding bytes.
 */
static void
dhcp_option_trailer(u16_t options_out_len, u8_t *options, struct pbuf *p_out)
{
  options[options_out_len++] = DHCP_OPTION_END;
  /* packet is too small, or not 4 byte aligned? */
  while (((options_out_len < DHCP_MIN_OPTIONS_LEN) || (options_out_len & 3)) &&
         (options_out_len < DHCP_OPTIONS_LEN)) {
    /* add a fill/padding byte */
    options[options_out_len++] = 0;
  }
  /* shrink the pbuf to the actual content length */
  pbuf_realloc(p_out, (u16_t)(sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN + options_out_len));
}

/** check if DHCP supplied netif->ip_addr
 *
 * @param netif the netif to check
 * @return 1 if DHCP supplied netif->ip_addr (states BOUND or RENEWING),
 *         0 otherwise
 */
u8_t
dhcp_supplied_address(const struct netif *netif)
{
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
  if (netif != NULL) {
    netif_dhcp = netif_dhcp_data(netif);
    if (netif_dhcp == NULL) {
      return 0;
    }
    dhcp_state = &((netif_dhcp->client.states)[LWIP_DHCP_NATIVE_IDX]);
    return (u8_t)((dhcp_state->state == DHCP_STATE_BOUND) ||
                  (dhcp_state->state == DHCP_STATE_RENEWING) ||
                  (dhcp_state->state == DHCP_STATE_REBINDING));
  }
  return 0;
}

#if LWIP_DHCP_SUBSTITUTE
#if LWIP_ARP
void
dhcp_events_record(struct netif *netif, u8_t flags)
{
  struct dhcp *netif_dhcp = netif_dhcp_data(netif);

  if (netif_dhcp == NULL) {
    return;
  }
  netif_dhcp->flags |= flags;
}

static void
dhcp_clients_announce(struct netif *netif)
{
  struct dhcp *netif_dhcp = netif_dhcp_data(netif);
  struct dhcp_client *dhcp = &(netif_dhcp->client);
  struct dhcp_state *dhcp_state = NULL;
  dhcp_num_t i;
  u8_t hwaddr_len;

  /*
   * when hwaddr of netif is changed, for netif's dhcp client, it may keep use its IP
   * or restart dhcp client by APP to obtain a new IP
   */
  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    dhcp_state = &((dhcp->states)[i]);
    if ((dhcp_state->state != DHCP_STATE_BOUND) && (dhcp_state->state != DHCP_STATE_RENEWING) &&
        (dhcp_state->state != DHCP_STATE_REBINDING)) {
      continue;
    }
    if (dhcp_idx_to_mac(netif, dhcp_state->idx, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
      continue;
    }
    dhcp->cli_idx = (dhcp_num_t)i;
    dhcp_state->hwaddr_len = hwaddr_len;
    /*
     * set state to be DHCP_STATE_BOUND to send ARP Announce
     * states DHCP_STATE_RENEWING or DHCP_STATE_REBINDING can recover later
     */
    dhcp_set_state(dhcp, DHCP_STATE_BOUND);
    /* maybe old state was DHCP_STATE_BOUND, so try times need to reset here */
    dhcp_state->tries = 0;
    dhcp_announce(netif, dhcp);
  }
}

void
dhcp_events_trigger(struct netif *netif)
{
  struct dhcp *netif_dhcp = NULL;

  if (!(netif_is_up(netif) && netif_is_link_up(netif))) {
    return;
  }
  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp == NULL) {
    return;
  }
  /* currently just support event DHCP_EVENT_HW */
  if ((netif_dhcp->flags & DHCP_EVENT_HW) == 0) {
    return;
  }
  if ((netif->flags & NETIF_FLAG_ETHARP) != 0) {
    /* hwaddr has been modified, must send ARP Announce */
    dhcp_clients_announce(netif);
  }

  /* clear flag DHCP_EVENT_HW */
  netif_dhcp->flags &= ~DHCP_EVENT_HW;
}
#endif /* LWIP_ARP */

static void
dhcp_substitute_clients_restart(struct netif *netif, struct dhcp_client *dhcp)
{
  int i;
  u8_t hwaddr_len;
  struct dhcp_state *dhcp_state = NULL;

  for (i = 1; i < DHCP_CLIENT_NUM; i++) {
    dhcp_state = &((dhcp->states)[i]);
    if ((dhcp_state->idx == 0)) {
      continue;
    }
    dhcp->cli_idx = (dhcp_num_t)i;
    if (dhcp_idx_to_mac(netif, dhcp_state->idx, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE,
                       "dhcp_substitute_clients_restart(): %u get mac failed\n", dhcp_state->idx);
      continue;
    }
    dhcp_state->hwaddr_len = hwaddr_len;
    (void)dhcp_release_client(netif, dhcp);
    (void)dhcp_discover(netif, dhcp);
  }

  return;
}

static s32_t
dhcp_addr_clients_check(struct dhcp *netif_dhcp, const ip4_addr_t *ipaddr)
{
  struct dhcp_client *dhcp = NULL;
  dhcp_num_t offered_ip_addr;
  struct dhcp_state *dhcp_state = NULL;
  int i;

  dhcp = &(netif_dhcp->client);

  DHCP_IP_TO_HOST(offered_ip_addr, ipaddr->addr, dhcp->offered_sn_mask.addr);
  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    dhcp_state = &((dhcp->states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    if ((dhcp_state->state != DHCP_STATE_BOUND) && (dhcp_state->state != DHCP_STATE_RENEWING) &&
        (dhcp_state->state != DHCP_STATE_REBINDING)) {
      continue;
    }
    if (dhcp_state->offered_ip_addr == offered_ip_addr) {
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_addr_clients_check(): %u used by substitute\n", ipaddr->addr);
      return lwIP_TRUE;
    }
  }

  return lwIP_FALSE;
}

err_t
dhcp_substitute_start(struct netif *netif, dhcp_num_t mac_idx, u32_t pref_ipv4)
{
  err_t err;
  LWIP_ERROR("netif != NULL", (netif != NULL), return ERR_ARG);
  LWIP_ERROR("netif is not up, old style port?", netif_is_up(netif), return ERR_ARG);
  LWIP_ERROR("mac_idx != LWIP_DHCP_NATIVE_IDX", (mac_idx != LWIP_DHCP_NATIVE_IDX), return ERR_ARG);
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_substitute_start:%u\n", mac_idx);

  err = dhcp_start_client(netif, mac_idx, pref_ipv4);
  if (err == ERR_OK) {
    dhcp_clients_count_update(&(netif_dhcp_data(netif)->client));
  }
  return err;
}

#if LWIP_DHCP_SUBSTITUTE_MMBR
/*
 * send ARP Request to check if there is an another MBR using this IP.
 * If receive the ARP Reply, then just think there is a MBR using this IP and should not send the Release message.
 * If we send the Release message, it may lead to that MBR renew IP failed and that MBR may get a different IP
 * from DHCP server.
 */
static void
dhcp_release_check(struct netif *netif, struct dhcp_client *dhcp)
{
  err_t result;
  u16_t msecs;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  ip4_addr_t cli_ip;
  dhcp_set_state(dhcp, DHCP_STATE_RELEASING);
  dhcp_get_client_ip(&cli_ip.addr, dhcp, dhcp_state);
  result = etharp_request(netif, &cli_ip);
  if (result != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                     ("dhcp_release_check: could not perform ARP query\n"));
  }
  if (dhcp_state->tries < 255) {
    dhcp_state->tries++;
  }
  msecs = DHCP_FINE_TIMER_MSECS;
  dhcp_state->request_timeout = (u16_t)((msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS);
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE,
                   "dhcp_release_check(): set request timeout %"U16_F" msecs\n", msecs);
}
#endif /* LWIP_DHCP_SUBSTITUTE_MMBR */

void
dhcp_substitute_stop(struct netif *netif, dhcp_num_t mac_idx, u8_t now)
{
  struct dhcp_client *dhcp = NULL;
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
  u8_t hwaddr_len;
  (void)now;
  LWIP_ERROR("netif != NULL", (netif != NULL), return);
  LWIP_ERROR("mac_idx != LWIP_DHCP_NATIVE_IDX", (mac_idx != LWIP_DHCP_NATIVE_IDX), return);
  LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_substitute_stop:%u\n", mac_idx);

  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp == NULL) {
    return;
  }

  dhcp = &(netif_dhcp->client);
  if (dhcp_client_find_by_mac_idx(dhcp, mac_idx, &(dhcp->cli_idx)) != ERR_OK) {
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_substitute_stop(): client state not find for %u\n", mac_idx);
    return;
  }

  dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  if (dhcp_idx_to_mac(netif, dhcp_state->idx, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
    LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE, "dhcp_substitute_stop(): no client state for %u\n", mac_idx);
    return;
  }
  dhcp_state->hwaddr_len = hwaddr_len;

#if LWIP_DHCP_SUBSTITUTE_MMBR
  if (now == lwIP_FALSE) {
    if ((dhcp_state->state == DHCP_STATE_BOUND) ||
        (dhcp_state->state == DHCP_STATE_RENEWING) ||
        (dhcp_state->state == DHCP_STATE_REBINDING)) {
      dhcp_release_check(netif, dhcp);
    } else if (dhcp_state->state == DHCP_STATE_RELEASING) {
      /* do nothing */
    } else {
      dhcp_stop_client(netif, dhcp);
    }
  } else
#endif /* LWIP_DHCP_SUBSTITUTE_MMBR */
  {
    dhcp_stop_client(netif, dhcp);
  }

  return;
}

err_t
dhcp_substitute_idx_to_ip(struct netif *netif, dhcp_num_t idx, ip4_addr_t *ip)
{
  struct dhcp_client *dhcp = NULL;
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
  int i;

  LWIP_ERROR("dhcp_substitute_idx_to_ip:netif != NULL", (netif != NULL), return ERR_ARG);
  LWIP_ERROR("dhcp_substitute_idx_to_ip:ip != NULL", (ip != NULL), return ERR_ARG);

  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp == NULL) {
    return ERR_VAL;
  }
  dhcp = &(netif_dhcp->client);

  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    dhcp_state = &((dhcp->states)[i]);
    if ((dhcp_state->idx != idx)) {
      continue;
    }
    if ((dhcp_state->state != DHCP_STATE_BOUND) && (dhcp_state->state != DHCP_STATE_RENEWING) &&
        (dhcp_state->state != DHCP_STATE_REBINDING)) {
      return ERR_INPROGRESS;
    }
    dhcp_get_client_ip(&(ip->addr), dhcp, dhcp_state);
    return ERR_OK;
  }

  return ERR_VAL;
}

static struct dhcp_clients_info *
dhcp_clients_info_fill(struct netif *netif, dhcp_num_t cnt)
{
  mem_size_t clis_info_size = sizeof(struct dhcp_clients_info) + cnt * sizeof(struct dhcp_client_mac_info);
  struct dhcp_clients_info *clis_info = (struct dhcp_clients_info *)mem_malloc(clis_info_size);
  struct dhcp_client_mac_info *cli = NULL;
  struct dhcp *netif_dhcp = netif_dhcp_data(netif);
  struct dhcp_client *dhcp = &(netif_dhcp->client);
  struct dhcp_state *dhcp_state = NULL;
  dhcp_num_t i;
  dhcp_num_t fill_cnt = 0;
  u8_t hwaddr_len;
  LWIP_ERROR("dhcp_clients_info_fill:malloc fail\n", (clis_info != NULL), return NULL);

  (void)memset_s(clis_info, clis_info_size, 0, clis_info_size);

  for (i = 0; (i < DHCP_CLIENT_NUM) && (fill_cnt < cnt); i++) {
    dhcp_state = &((dhcp->states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    cli = &(clis_info->clis[fill_cnt]);
    if (dhcp_idx_to_mac(netif, dhcp_state->idx, cli->mac, &hwaddr_len) != ERR_OK) {
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                       "dhcp_clients_info_fill:idx %"DHCP_NUM_F" to mac failed\n", dhcp_state->idx);
      goto failure;
    }
    cli->mac_len = hwaddr_len;
    fill_cnt++;
  }
  clis_info->num = fill_cnt;
  return clis_info;
failure:
  mem_free(clis_info);
  return NULL;
}

err_t
dhcp_clients_info_get(struct netif *netif, void *arg)
{
  struct dhcp_clients_info **clis_info = (struct dhcp_clients_info **)arg;
  struct dhcp *netif_dhcp = NULL;

  LWIP_ERROR("dhcp_clients_info_get:netif != NULL\n", (netif != NULL), return ERR_ARG);
  LWIP_ERROR("dhcp_clients_info_get:clis_info != NULL\n", (clis_info != NULL), return ERR_ARG);
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_clients_info_get:%s%hhu\n", netif->name, netif->num));
  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp == NULL) {
    return ERR_VAL;
  }
  if (netif_dhcp->clis_info != NULL) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_clients_info_get:last not free\n"));
    return ERR_INPROGRESS;
  }

  if (netif_dhcp->client.cli_cnt == 0) {
    return ERR_VAL;
  }
  *clis_info = dhcp_clients_info_fill(netif, netif_dhcp->client.cli_cnt);
  if (*clis_info == NULL) {
    return ERR_MEM;
  }
  netif_dhcp->clis_info = *clis_info;
  return ERR_OK;
}

err_t
dhcp_clients_info_free(struct netif *netif, void *arg)
{
  struct dhcp_clients_info **clis_info = (struct dhcp_clients_info **)arg;
  struct dhcp *netif_dhcp = NULL;
  LWIP_ERROR("dhcp_clients_info_free:netif != NULL\n", (netif != NULL), return ERR_ARG);
  LWIP_ERROR("dhcp_clients_info_free:clis_info != NULL\n", (clis_info != NULL), return ERR_ARG);
  LWIP_ERROR("dhcp_clients_info_free:*clis_info != NULL\n", (*clis_info != NULL), return ERR_ARG);
  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp == NULL) {
    return ERR_VAL;
  }
  if (netif_dhcp->clis_info != (*clis_info)) {
    return ERR_VAL;
  }
  mem_free(*clis_info);
  *clis_info = NULL;
  netif_dhcp->clis_info = NULL;

  return ERR_OK;
}

static void
dhcp_client_info_fill_ip4(struct dhcp_client_info *cli_info, struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  if ((dhcp_state->state == DHCP_STATE_BOUND) || (dhcp_state->state == DHCP_STATE_RENEWING) ||
      (dhcp_state->state == DHCP_STATE_REBINDING)) {
    dhcp_get_client_ip(&(cli_info->ip4addr.addr), dhcp, dhcp_state);
  } else {
    ip4_addr_set_any(&(cli_info->ip4addr));
  }
}

static err_t
dhcp_native_ip6addr_get(struct dhcp_client_info *cli_info)
{
#if LWIP_RIPPLE
  struct netif *netif = (struct netif *)rpl_platform_get_dev();
  u8_t i;
  if (netif == NULL) {
    return ERR_VAL;
  }
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (ip6_addr_isinvalid(netif_ip6_addr_state(netif, i))) {
      continue;
    }
    if (ip6_addr_islinklocal(netif_ip6_addr(netif, i))) {
      continue;
    }
    (void)memcpy_s(&(cli_info->ip6addr), sizeof(ip6_addr_t), netif_ip6_addr(netif, i), sizeof(ip6_addr_t));
    return ERR_OK;
  }
  return ERR_VAL;
#else
  (void)cli_info;
  return ERR_VAL;
#endif
}

static err_t
dhcp_client_info_fill_ip6(struct dhcp_client_info *cli_info, struct dhcp_client *dhcp)
{
  if (dhcp->cli_idx == LWIP_DHCP_NATIVE_IDX) {
    return dhcp_native_ip6addr_get(cli_info);
  } else {
#if LWIP_NAT64
    struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
    if (nat64_entry_idx_to_ip6addr(dhcp_state->idx, &(cli_info->ip6addr))) {
      LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                       "dhcp_client_info_fill_ip6:get ip6addr failed\n");
      return ERR_VAL;
    }
#else
    return ERR_VAL;
#endif
  }
  return ERR_OK;
}

static err_t
dhcp_client_info_fill(struct dhcp_client_info *cli_info, struct dhcp_client *dhcp)
{
  dhcp_client_info_fill_ip4(cli_info, dhcp);

  return dhcp_client_info_fill_ip6(cli_info, dhcp);
}

err_t
dhcp_client_info_find(struct netif *netif, void *cli_info)
{
  struct dhcp_client_info *cli_info_p = (struct dhcp_client_info *)cli_info;
  err_t err;
  struct dhcp *netif_dhcp = NULL;
  dhcp_num_t mac_idx;

  LWIP_ERROR("dhcp_client_info_find:netif != NULL\n", (netif != NULL), return ERR_ARG);
  LWIP_ERROR("dhcp_client_info_find:cli_info_p != NULL\n", (cli_info_p != NULL), return ERR_ARG);
  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp == NULL) {
    return ERR_VAL;
  }

  err = dhcp_mac_to_idx(netif, cli_info_p->mac, cli_info_p->mac_len, &mac_idx);
  if (err != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_client_info_find(): mac idx failed\n"));
    return ERR_VAL;
  }

  if (dhcp_client_find_by_mac_idx(&(netif_dhcp->client), mac_idx, &(netif_dhcp->client.cli_idx)) != ERR_OK) {
    LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_recv(): no client\n"));
    return ERR_VAL;
  }

  return dhcp_client_info_fill(cli_info_p, &(netif_dhcp->client));
}

#if LWIP_ENABLE_BASIC_SHELL_CMD
s32_t
dhcp_netif_addr_clients_check(const struct netif *netif, const ip4_addr_t *ipaddr)
{
  struct dhcp *netif_dhcp = NULL;

  if ((netif == NULL) || (ipaddr == NULL)) {
    return lwIP_FALSE;
  }

  netif_dhcp = netif_dhcp_data(netif);
  if (netif_dhcp == NULL) {
    return lwIP_FALSE;
  }

  return dhcp_addr_clients_check(netif_dhcp, ipaddr);
}
#endif /* LWIP_ENABLE_BASIC_SHELL_CMD */

#if LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS
static void
dhcp_concurrent_limit_wait_timeout(struct netif *netif, struct dhcp_client *dhcp)
{
  if (dhcp_concurrent_start_client(dhcp) != ERR_OK) {
    return;
  }
  if (!netif_is_link_up(netif)) {
    /* set state INIT and wait for dhcp_network_changed() to call dhcp_discover() */
    dhcp_set_state(dhcp, DHCP_STATE_INIT);
    return;
  }

  (void)dhcp_discover(netif, dhcp);

  return;
}

static u8_t
dhcp_client_request_processing(u8_t state)
{
  u8_t is_processing;

  switch (state) {
    case DHCP_STATE_REQUESTING:
    case DHCP_STATE_INIT:
    case DHCP_STATE_REBOOTING:
    case DHCP_STATE_SELECTING:
    case DHCP_STATE_CHECKING:
    case DHCP_STATE_BACKING_OFF:
      is_processing = lwIP_TRUE;
      break;
    default:
      is_processing = lwIP_FALSE;
      break;
  }

  return is_processing;
}

static err_t
dhcp_concurrent_start_client(struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  if (g_dhcp_max_concurrent_num != 0) {
    if (dhcp->rqst_cli_cnt >= g_dhcp_max_concurrent_num) {
      dhcp_set_state(dhcp, DHCP_STATE_LIMIT_WAIT);
      dhcp_state->request_timeout = 1;
      return ERR_INPROGRESS;
    }
    dhcp->rqst_cli_cnt++;
  }
  return ERR_OK;
}

static void
dhcp_concurrent_stop_client(struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);

  if (g_dhcp_max_concurrent_num == 0) {
    return;
  }

  if (dhcp_client_request_processing(dhcp_state->state) == lwIP_TRUE) {
    dhcp->rqst_cli_cnt--;
  }

  return;
}

static void
dhcp_concurrent_start_next_client(struct netif *netif, u8_t all)
{
  struct dhcp *netif_dhcp = netif_dhcp_data(netif);
  struct dhcp_state *dhcp_state = NULL;
  int i;
  u8_t hwaddr_len;

  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    netif_dhcp->client.cli_idx = (dhcp_num_t)i;
    dhcp_state = &((netif_dhcp->client.states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    if (dhcp_state->state != DHCP_STATE_LIMIT_WAIT) {
      continue;
    }
    if (dhcp_idx_to_mac(netif, dhcp_state->idx, netif_dhcp->client.hwaddr, &hwaddr_len) != ERR_OK) {
      LWIP_DEBUGF_LOG1(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                       "dhcp_concurrent_start_next_client:idx %u to mac failed\n", dhcp_state->idx);
      continue;
    }
    dhcp_state->hwaddr_len = hwaddr_len;
    (void)dhcp_discover(netif, &(netif_dhcp->client));
    if (all == lwIP_FALSE) {
      (netif_dhcp->client.rqst_cli_cnt)++;
      break;
    }
  }

  return;
}

static void
dhcp_concurrent_num_update(struct dhcp_client *dhcp)
{
  struct dhcp_state *dhcp_state = NULL;
  int i;

  dhcp->rqst_cli_cnt = 0;
  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    dhcp->cli_idx = (dhcp_num_t)i;
    dhcp_state = &((dhcp->states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    if (dhcp_client_request_processing(dhcp_state->state) == lwIP_FALSE) {
      continue;
    }
    if (dhcp->rqst_cli_cnt < g_dhcp_max_concurrent_num) {
      dhcp->rqst_cli_cnt++;
    } else {
      dhcp_set_state(dhcp, DHCP_STATE_LIMIT_WAIT);
      dhcp_state->request_timeout = 1;
    }
  }

  return;
}

err_t
dhcp_set_max_concurrent_num(struct netif *netif, u16_t dhcp_max_concurrent_num)
{
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_client *dhcp = NULL;

  LWIP_DEBUGF_LOG0(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_set_max_concurrent_num()\n"));
  LWIP_ERROR("netif != NULL", (netif != NULL), return ERR_ARG);
  netif_dhcp = netif_dhcp_data(netif);
  LWIP_ERROR("netif_dhcp != NULL", (netif_dhcp != NULL), return ERR_VAL);

  dhcp = &(netif_dhcp->client);
  if (dhcp_max_concurrent_num == 0) {
    g_dhcp_max_concurrent_num = 0;
    dhcp->rqst_cli_cnt = 0;
    dhcp_concurrent_start_next_client(netif, lwIP_TRUE);
  } else {
    g_dhcp_max_concurrent_num = dhcp_max_concurrent_num;
    dhcp_concurrent_num_update(dhcp);
  }

  return ERR_OK;
}
#endif /* LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS */
#endif /* LWIP_DHCP_SUBSTITUTE */

#endif /* LWIP_IPV4 && LWIP_DHCP */
