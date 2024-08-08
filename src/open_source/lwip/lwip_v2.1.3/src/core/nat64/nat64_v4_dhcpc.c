/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: implementation for NAT64 dhcp proxy
 * Author: NA
 * Create: 2019
 */
#include "lwip/opt.h"
#include "lwip/dhcp.h"
#if LWIP_NAT64
#include "arch/sys_arch.h"
#include "lwip/nat64.h"
#include "lwip/nat64_addr.h"
#include "lwip/nat64_v4_dhcpc.h"
#include "lwip/lwip_rpl.h"
#include "rpl_event_api.h"

err_t
nat64_dhcp_request_ip(struct netif *ntf, const linklayer_addr_t *lladdr, const ip4_addr_t *ip)
{
  err_t ret;
#if LWIP_DHCP_SUBSTITUTE
  dhcp_num_t index = 0;
#endif
  if ((ntf == NULL) || (lladdr == NULL)) {
    return ERR_ARG;
  }

#if LWIP_DHCP_SUBSTITUTE
  ret = nat64_entry_mac_to_idx(lladdr->addr, lladdr->addrlen, &index);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }

  ret = dhcp_substitute_start(ntf, index, ip->addr);
#else
  ret = ERR_ARG;
#endif

  return ret;
}

err_t
nat64_dhcp_stop(struct netif *ntf, const linklayer_addr_t *lladdr, u8_t now)
{
  err_t ret;
#if LWIP_DHCP_SUBSTITUTE
  dhcp_num_t index = 0;
#endif
  if ((ntf == NULL) || (lladdr == NULL)) {
    return ERR_ARG;
  }
#if LWIP_DHCP_SUBSTITUTE
  ret = nat64_entry_mac_to_idx(lladdr->addr, lladdr->addrlen, &index);
  if (ret != ERR_OK) {
    return ERR_VAL;
  }

  dhcp_substitute_stop(ntf, index, now);
  ret = ERR_OK;
#else
  ret = ERR_ARG;
#endif

  return ret;
}


void
nat64_dhcp_ip4_event(const u8_t *mac, u8_t maclen, const ip4_addr_t *ipaddr, int event)
{
  nat64_entry_t *entry = NULL;
  int ret;
  linklayer_addr_t llmac;
  if (mac == NULL) {
    return;
  }

  (void)memset_s(&llmac, sizeof(linklayer_addr_t), 0, sizeof(linklayer_addr_t));
  ret = memcpy_s(llmac.addr, sizeof(llmac.addr), mac, maclen);
  if (ret != EOK) {
    LWIP_DEBUGF(NAT64_DEBUG, ("%s:memcpy_s fail(%d)\n", __FUNCTION__, ret));
    return;
  }
  llmac.addrlen = maclen > sizeof(llmac.addr) ? sizeof(llmac.addr) : maclen;
  entry = nat64_entry_lookup_by_mac(&llmac);
  if ((entry == NULL) || (entry->nat64_sync == lwIP_TRUE)) {
    return;
  }

  if (((event == NAT64_DHCP_EVENT_OFFER) || (event == NAT64_DHCP_EVENT_RENEW)) &&
      (ipaddr != NULL)) {
    if ((entry->state == NAT64_STATE_DHCP_REQUEST) ||
        (entry->state == NAT64_STATE_ESTABLISH)) {
      entry->state = NAT64_STATE_ESTABLISH;
#if !LWIP_NAT64_MIN_SUBSTITUTE
      /* dhcp clinet renew its ip, get ipv4 addr may change */
      if (ip4_addr_isany_val(entry->ip)) {
        ip4_addr_copy(entry->ip, *ipaddr);
#if LWIP_MMBR && LWIP_NAT64_CHANGE_MSG
        nat64_send_change_entry_msg(entry, RPL_EVT_NODE_NAT64_ADD);
#endif /* LWIP_MMBR && LWIP_NAT64_CHANGE_MSG */
      } else if (!ip4_addr_cmp(ipaddr, &entry->ip)) {
        ip4_addr_copy(entry->ip, *ipaddr);
#if LWIP_MMBR && LWIP_NAT64_CHANGE_MSG
        nat64_send_change_entry_msg(entry, RPL_EVT_NODE_NAT64_UPDATE);
      } else {
        /*
         * When a node switches from an MBR to another MBR and gets the dhcp event,
         * it also needs to send the add event because the IPv4 address of the node remains unchanged.
         */
        nat64_send_change_entry_msg(entry, RPL_EVT_NODE_NAT64_ADD);
#endif /* LWIP_MMBR && LWIP_NAT64_CHANGE_MSG */
      }
      ip6_addr_t ip6addr;
      if (nat64_entry_to6(entry, &ip6addr) == 0) {
        lwip_notify_rpl_get_ipv4_addr(entry->mnid, &ip6addr);
      }
#else
      (void)ipaddr;
#endif
      return;
    }
  }

  if (event == NAT64_DHCP_EVENT_RELEASE) {
    entry->state = NAT64_STATE_DIRECT_RELEASE;
    entry->lifetime = 0;
  }
}
#endif
