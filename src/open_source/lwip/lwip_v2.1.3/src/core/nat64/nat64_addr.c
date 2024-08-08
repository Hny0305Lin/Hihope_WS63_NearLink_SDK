/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: according the rfc 6052, the prefix length could be 32/40/48/56/64/96.
 * but only 96 prefix length can be supported in our nat64 now.
 * when the rpl start, the prefix will be setted, now this prefix is used in nat64.
 * noted:this prefix should be stored independently in nat64 when needed.
 * Author: NA
 * Create: 2019
 */
#include "lwip/opt.h"
#include "lwip/dhcp.h"
#if LWIP_NAT64
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/nat64.h"
#include "lwip/lwip_rpl.h"
#include "lwip/nat64_addr.h"
#include "lwip/nat64_dns64.h"

/* maybe the prefix can be stored */
int
nat64_stateless_addr_4to6(const ip4_addr_t *ip4addr, ip6_addr_t *ip6addr)
{
  ip6_addr_t prefix;
  uint8_t len;
  err_t err;

  if ((ip4addr == NULL) || (ip6addr == NULL)) {
    return -1;
  }
  (void)memset_s(&prefix, sizeof(ip6_addr_t), 0, sizeof(ip6_addr_t));
#ifdef NAT64_USING_DAG_PREFIX
  err = lwip_rpl_get_default_prefix(&prefix, &len);
#elif LWIP_DNS64
  err = nat64_dns64_get_prefix(&prefix, &len);
#else
  err = ERR_VAL;
#endif
  if (err != ERR_OK) {
    return -1;
  }

  (void)len;
  /* the address convert should be related with the prefix len, here is for simple */
  ip6_addr_copy_ptr(ip6addr, &prefix);
  /* the last 32bit address of ipv6 is ipv4 address. */
  ip6addr->addr[3] = ip4addr->addr;

  return 0;
}

/* handle special prefix or different prefix length */
int
nat64_stateless_addr_6to4(const ip6_addr_t *ip6addr, ip4_addr_t *ip4addr)
{
  ip6_addr_t prefix;
  err_t err;

  if ((ip4addr == NULL) || (ip6addr == NULL)) {
    return -1;
  }

  if (nat64_addr_is_ip4(ip6addr) == 0) {
    return -1;
  }
   /* the last 32bit address of ipv6 */
  ip4addr->addr = ip6addr->addr[3];
  (void)memset_s(&prefix, sizeof(ip6_addr_t), 0, sizeof(ip6_addr_t));
#ifdef LWIP_NAT64_SAME_PREFIX
  uint8_t len;
  err = lwip_rpl_get_default_prefix(&prefix, &len);
  if (err != ERR_OK) {
    return -1;
  }

  /* 96 prefix length is used now, should check that (96 - prefix.length) bit is zero */
  if (memcmp(ip6addr, &prefix, len >> 3) == 0) {
    ip4addr->addr = ip6addr->addr[3]; /* the last address of ipv6 */
    return 0;
  }

  return -1;
#else
  err = 0;
  (void)err;
  (void)prefix;
  return 0;
#endif
}

int
nat64_entry_to6(const nat64_entry_t *entry, ip6_addr_t *ip6addr)
{
  ip6_addr_t prefix;
  uint8_t len;
  err_t err;

  if ((entry == NULL) || (ip6addr == NULL)) {
    return -1;
  }
  (void)memset_s(&prefix, sizeof(ip6_addr_t), 0, sizeof(ip6_addr_t));
  err = lwip_rpl_get_default_prefix(&prefix, &len);
  if (err != ERR_OK) {
    return -1;
  }

  (void)len;
  ip6_addr_copy_ptr(ip6addr, &prefix);
  lwip_rpl_set_ip6_iid(&entry->mac, entry->orig_mnid, ip6addr);

  return 0;
}

int
nat64_entry_to4(const nat64_entry_t *entry, ip4_addr_t *ip4addr)
{
#if LWIP_NAT64_MIN_SUBSTITUTE
  dhcp_num_t index = 0;
  ip4_addr_t ip;
  err_t ret;
#endif

#if LWIP_NAT64_MIN_SUBSTITUTE
  ret = nat64_entry_mac_to_idx(entry->mac.addr, entry->mac.addrlen, &index);
  if (ret != ERR_OK) {
    return -1;
  }
  (void)memset_s(&ip, sizeof(ip4_addr_t), 0, sizeof(ip4_addr_t));
#endif

  if ((entry->state == NAT64_STATE_ESTABLISH) &&
#if !LWIP_NAT64_MIN_SUBSTITUTE
      (!ip4_addr_isany_val(entry->ip))
#elif LWIP_DHCP_SUBSTITUTE
      (dhcp_substitute_idx_to_ip(nat64_netif_get(), index, &ip) == ERR_OK) &&
      (!ip4_addr_isany_val(ip))
#else
      (lwIP_FALSE)
#endif
     ) {
#if !LWIP_NAT64_MIN_SUBSTITUTE
    ip4_addr_copy(*ip4addr, entry->ip);
#elif LWIP_DHCP_SUBSTITUTE
    ip4_addr_copy(*ip4addr, ip);
#else
    ip4addr->addr = 0;
#endif
    return 0;
  }

  return -1;
}

int
nat64_addr_6to4(const ip6_addr_t *ip6addr, ip4_addr_t *ip4addr)
{
  linklayer_addr_t lladdr;
  nat64_entry_t *entry = NULL;
  int ret;

  if ((ip4addr == NULL) || (ip6addr == NULL)) {
    return -1;
  }
  /* 64ff:9b/96 prefix is unique */
  ret = nat64_stateless_addr_6to4(ip6addr, ip4addr);
  if (ret == 0) {
    return 0;
  }

  ret = lwip_rpl_get_lladdr(ip6addr, &lladdr);
  if (ret != ERR_OK) {
    return -1;
  }

  entry = nat64_entry_lookup_by_mac(&lladdr);
  if (entry == NULL) {
    return -1;
  }

  return nat64_entry_to4(entry, ip4addr);
}

int
nat64_addr_is_ip4(const ip6_addr_t *ip6addr)
{
  ip6_addr_t prefix;
  uint8_t len;
  err_t err;

  if (ip6addr == NULL) {
    return 0;
  }
  (void)memset_s(&prefix, sizeof(ip6_addr_t), 0, sizeof(ip6_addr_t));
#ifdef NAT64_USING_DAG_PREFIX
  err = lwip_rpl_get_default_prefix(&prefix, &len);
#elif LWIP_DNS64
  err = nat64_dns64_get_prefix(&prefix, &len);
#else
  err = ERR_VAL;
#endif
  if (err != ERR_OK) {
    return 0;
  }
  /* bit64~71 is not zero, the address is not ipv4 */
  if (memcmp(&prefix, ip6addr, len >> 3) != 0) {
    return 0;
  }

  return 1;
}

#endif
