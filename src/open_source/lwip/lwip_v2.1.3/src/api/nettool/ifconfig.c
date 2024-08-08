/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: ifconfig shell cmd API implementation
 * Author: none
 * Create: 2020
 */

#include "lwip/priv/nd6_priv.h"
#include "lwip/sockets.h"
#include "lwip/inet_chksum.h"
#include "lwip/raw.h"
#include "lwip/priv/api_msg.h"
#include "lwip/icmp.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/ip.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/nettool/ifconfig.h"
#include "lwip/netifapi.h"
#include "lwip/etharp.h"
#include "lwip/nettool/utility.h"
#if LWIP_LITEOS_COMPAT
#include "los_config.h"
#endif

#ifndef LWIP_IFCONFIG_SHOW_SINGLE
#define LWIP_IFCONFIG_SHOW_SINGLE 1
#endif

#if LWIP_ENABLE_BASIC_SHELL_CMD

#define BYTE_CONVERT_UNIT 1024

#define IFCONFIG_OPTION_SET_IP          1
#define IFCONFIG_OPTION_SET_NETMASK     (1 << 1)
#define IFCONFIG_OPTION_SET_GW          (1 << 2)
#define IFCONFIG_OPTION_SET_HW          (1 << 3)
#define IFCONFIG_OPTION_SET_UP          (1 << 4)
#define IFCONFIG_OPTION_SET_DOWN        (1 << 5)
#define IFCONFIG_OPTION_SET_MTU         (1 << 6)
#define IFCONFIG_OPTION_DEL_IP          (1 << 7)

struct ifconfig_option {
  char            iface[NETIF_NAMESIZE];
  unsigned int    option;
  ip_addr_t       ip_addr;
  ip_addr_t       netmask;
  ip_addr_t       gw;
  unsigned char   ethaddr[NETIF_MAX_HWADDR_LEN];
  u16_t           mtu;
  u8_t            result;
  /*
   * when using telnet, print to the telnet socket will result in system deadlock.
   * So we cahe the prinf data to a buf, and when the tcpip callback returns,
   * then print the data out to the telnet socket
   */
  sys_sem_t       cb_completed;
  char            cb_print_buf[PRINT_BUF_LEN];
  unsigned int    print_len;
};

#if LWIP_ARP && LWIP_ENABLE_IP_CONFLICT_SIGNAL
extern sys_sem_t ip_conflict_detect;
extern u32_t is_ip_conflict_signal;
LWIP_STATIC u32_t g_old_ip4addr;
LWIP_STATIC u32_t g_old_ip4mask;
LWIP_STATIC u32_t g_old_ip4gw;
#endif
#if LWIP_IPV6
extern sys_sem_t dup_addr_detect;
extern u32_t is_dup_detect_initialized;
#endif

#ifndef CUSTOM_AT_COMMAND
LWIP_STATIC void
err_ifconfig_string_put(struct ifconfig_option *ifconfig_cmd, char* errcode)
{
  int ret;
  ret = snprintf_s(ifconfig_cmd->cb_print_buf + ifconfig_cmd->print_len,
                   PRINT_BUF_LEN - ifconfig_cmd->print_len,
                   ((PRINT_BUF_LEN - ifconfig_cmd->print_len) - 1), "ERR:%s\n", errcode);
  if ((ret > 0) && ((unsigned int)ret < (PRINT_BUF_LEN - ifconfig_cmd->print_len))) {
    ifconfig_cmd->print_len += (unsigned int)ret;
  }
}
#endif

#ifndef LWIP_TESTBED
LWIP_STATIC
#endif
int print_netif_ip(struct netif *netif, char *print_buf, unsigned int buf_len)
{
  int ret;
  char *tmp = print_buf;

  if (buf_len <= 1) {
    return -1;
  }

#if LWIP_IPV4
  char ip_str[IPADDR_STRLEN_MAX];
  char netmask_str[IPADDR_STRLEN_MAX];
  char gateway_str[IPADDR_STRLEN_MAX];

  (void)ipaddr_ntoa_r(&netif->ip_addr, ip_str, IPADDR_STRLEN_MAX);
  (void)ipaddr_ntoa_r(&netif->netmask, netmask_str, IPADDR_STRLEN_MAX);
  (void)ipaddr_ntoa_r(&netif->gw, gateway_str, IPADDR_STRLEN_MAX);
#ifdef CUSTOM_AT_COMMAND
  ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "ip=%s,netmask=%s,gateway=%s,",
                   ip_str, netmask_str, gateway_str);
#else
  ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "ip:%s netmask:%s gateway:%s"CRLF,
                   ip_str, netmask_str, gateway_str);
#endif
  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    return -1;
  }
  tmp += ret;
  buf_len -= (unsigned int)ret;
#endif

#if LWIP_IPV6
  char ip6_str[IPADDR_STRLEN_MAX];
  char *addr = NULL;
  int i;
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    /* only PREFERRED addresses are displyaed */
    if (!ip6_addr_isvalid(netif->ip6_addr_state[i])) {
      continue;
    }
    addr = ip6addr_ntoa_r((const ip6_addr_t *)&netif->ip6_addr[i], ip6_str, IPADDR_STRLEN_MAX);
#ifdef CUSTOM_AT_COMMAND
    if (addr != NULL) {
      ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "ip6=%s,", ip6_str);
    }
#else
    (void)addr;
    ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "\tip6: %s/64"CRLF, ip6_str);
#endif
    if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
      return -1;
    }
    tmp += ret;
    buf_len -= (unsigned int)ret;
  }
#endif

  return (tmp - print_buf);
}

LWIP_STATIC int
print_netif_hw(struct netif *netif, char *print_buf, unsigned int buf_len)
{
  int ret, i;
  char *tmp = print_buf;
  if (buf_len <= 1) {
    return -1;
  }
#ifdef CUSTOM_AT_COMMAND
  ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%s", "HWaddr=");
#else
  ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%s", "\tHWaddr ");
#endif
  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    return -1;
  }
  tmp += ret;
  buf_len -= (unsigned int)ret;

  for (i = 0; i < netif->hwaddr_len - 1; i++) {
    ret = snprintf_s(tmp, buf_len, (buf_len - 1), "%02x:", netif->hwaddr[i]);
    if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
      return -1;
    }
    tmp += ret;
    buf_len -= (unsigned int)ret;
  }

#ifdef CUSTOM_AT_COMMAND
  ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%02x,", netif->hwaddr[i]);
#else
  ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%02x", netif->hwaddr[i]);
#endif
  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    return -1;
  }
  tmp += ret;

  return (tmp - print_buf);
}

LWIP_STATIC int
print_netif_link(struct netif *netif, char *print_buf, unsigned int buf_len)
{
  int ret;
  char *tmp = print_buf;
  if (buf_len <= 1) {
    return -1;
  }
#ifdef CUSTOM_AT_COMMAND
  ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "MTU=%hu,LinkStatus=%d,RunStatus=%d"CRLF,
                   netif->mtu,
                   ((netif->flags & NETIF_FLAG_LINK_UP) != 0) ? 1 : 0,
                   ((netif->flags & NETIF_FLAG_UP) != 0) ? 1 : 0);

#else
  ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), " MTU:%hu %s %s %s"CRLF,
                   netif->mtu,
                   ((netif->flags & NETIF_FLAG_UP) != 0) ? "Running" : "Stop",
                   "Default",
                   ((netif->flags & NETIF_FLAG_LINK_UP) != 0) ? "Link UP" : "Link Down");
#endif
  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    return -1;
  }
  tmp += ret;

  return (tmp - print_buf);
}

#if MIB2_STATS
LWIP_STATIC int
print_netif_mib(struct netif *netif, char *print_buf, unsigned int buf_len)
{
  int ret;
  char *tmp = print_buf;

  if (buf_len <= 1) {
    return -1;
  }

  ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1),
                   "\tRX packets:%u errors:%u dropped:%u overruns:%u"CRLF
                   "\tTX packets:%u errors:%u dropped:%u"CRLF"\tRX bytes:%u TX bytes:%u"CRLF,
                   netif->mib2_counters.ifinucastpkts + netif->mib2_counters.ifinnucastpkts,
                   netif->mib2_counters.ifinerrors,
                   netif->mib2_counters.ifindiscards,
                   netif->mib2_counters.ifinoverruns,
                   netif->mib2_counters.ifoutucastpkts + netif->mib2_counters.ifoutnucastpkts,
                   netif->mib2_counters.ifouterrors,
                   netif->mib2_counters.ifoutdiscards,
                   netif->mib2_counters.ifinoctets,
                   netif->mib2_counters.ifoutoctets);

  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    return -1;
  }
  tmp += ret;
  return (tmp - print_buf);
}
#endif

#ifndef LWIP_TESTBED
LWIP_STATIC
#endif
int
print_netif(struct netif *netif, char *print_buf, unsigned int buf_len)
{
  int ret;
  char *tmp = print_buf;

  if (buf_len <= 1) {
    return -1;
  }
#ifdef CUSTOM_AT_COMMAND
  /* printf +IFCFG */
  (void)uapi_at_printf("+IFCFG:");
#endif

  if (netif->link_layer_type == LOOPBACK_IF) {
#ifdef CUSTOM_AT_COMMAND
    ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%s,", netif->name);
#else
    ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%s\t", netif->name);
#endif
  } else {
#ifdef CUSTOM_AT_COMMAND
    ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%s%u,", netif->name, netif->num);
#else
    ret = snprintf_s(tmp, buf_len, (unsigned int)(buf_len - 1), "%s%u\t", netif->name, netif->num);
#endif
  }
  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    goto out;
  }
  tmp += ret;
  buf_len -= (unsigned int)ret;

  ret = print_netif_ip(netif, tmp, buf_len);
  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    goto out;
  }
  tmp += ret;
  buf_len -= (unsigned int)ret;

  ret = print_netif_hw(netif, tmp, buf_len);
  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    goto out;
  }
  tmp += ret;
  buf_len -= (unsigned int)ret;

  ret = print_netif_link(netif, tmp, buf_len);
  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    goto out;
  }
  tmp += ret;
  buf_len -= (unsigned int)ret;

#if MIB2_STATS
  ret = print_netif_mib(netif, tmp, buf_len);
  if ((ret <= 0) || ((unsigned int)ret >= buf_len)) {
    goto out;
  }
  tmp += ret;
  buf_len -= (unsigned int)ret;
#endif

#if LWIP_IFCONFIG_SHOW_SINGLE
#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("%s", print_buf);
#else
  LWIP_PLATFORM_PRINT("%s", print_buf);
#endif
#endif
out:
  return (int)(tmp - print_buf);
}

#ifndef LWIP_TESTBED
LWIP_STATIC
#endif
void
lwip_ifconfig_show_internal(void *arg)
{
  struct netif *netif = NULL;
  struct ifconfig_option *ifconfig_cmd = (struct ifconfig_option *)arg;
  int ret;

  if (netif_list == NULL) {
#ifndef CUSTOM_AT_COMMAND
    err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_DEV_NOT_READY);
#endif
    sys_sem_signal(&ifconfig_cmd->cb_completed);
#ifdef CUSTOM_AT_COMMAND
    (void)uapi_at_printf("ERROR"CRLF);
#endif
    return;
  }

  if (ifconfig_cmd->iface[0] == '\0') {
    /* display all netif */
    for (netif = netif_list; netif != NULL; netif = netif->next) {
      ret = print_netif(netif,
                        ifconfig_cmd->cb_print_buf + ifconfig_cmd->print_len,
                        PRINT_BUF_LEN - ifconfig_cmd->print_len);
      if (ret == -1) {
        sys_sem_signal(&ifconfig_cmd->cb_completed);
        return;
      }
      ifconfig_cmd->print_len += (unsigned int)ret;
    }
  } else {
    netif = netif_find(ifconfig_cmd->iface);
    if (netif == NULL) {
#ifndef CUSTOM_AT_COMMAND
      err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_DEV_NOT_FOUND);
#endif
      sys_sem_signal(&ifconfig_cmd->cb_completed);
#ifdef CUSTOM_AT_COMMAND
      (void)uapi_at_printf("ERROR"CRLF);
#endif
      PRINT_ERRCODE(API_SHELL_ERRCODE_DEV_NOT_FOUND);
      return;
    }

    ret = print_netif(netif,
                      ifconfig_cmd->cb_print_buf + ifconfig_cmd->print_len,
                      PRINT_BUF_LEN - ifconfig_cmd->print_len);
    if (ret == -1) {
      sys_sem_signal(&ifconfig_cmd->cb_completed);
      return;
    }
    ifconfig_cmd->print_len += (unsigned int)ret;
  }
  sys_sem_signal(&ifconfig_cmd->cb_completed);

#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("OK"CRLF);
#endif
}

LWIP_STATIC int
lwip_ifconfig_ip(struct netif *netif, struct ifconfig_option *ifconfig_cmd)
{
  ip_addr_t ip_addr;
  struct netif *loc_netif = NULL;

  ip_addr_set_val(ip_addr, (ifconfig_cmd->ip_addr));
  if (IP_IS_V4_VAL(ifconfig_cmd->ip_addr)) {
    /* check the address is not multicast/broadcast/0/loopback */
    if (ip_addr_ismulticast_val(ip_addr) || ip_addr_isbroadcast_val(ip_addr, netif) ||
        (ip_addr_isany(&ip_addr) && (g_old_ip4addr != IPADDR_ANY)) || ip_addr_isloopback(&ip_addr)) {
#ifndef CUSTOM_AT_COMMAND
      err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_INVALID);
#endif
      return -1;
    }
    /* lwip disallow two netif sit in same net at the same time */
    loc_netif = netif_list;
    while (loc_netif != NULL) {
      if ((loc_netif == netif) || (ip4_addr_isany_val(*ip_2_ip4(&loc_netif->netmask)))) {
        loc_netif = loc_netif->next;
        continue;
      }
      if (ip_addr_cmp(&netif->netmask, &loc_netif->netmask) &&
          ip_addr_netcmp_val(loc_netif->ip_addr, ip_addr, ip_2_ip4(&netif->netmask))) {
#ifndef CUSTOM_AT_COMMAND
        err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_DUPLICATE_NETWORK);
#endif
        return -1;
      }
      loc_netif = loc_netif->next;
    }
    /* reset gateway if new and previous ipaddr not in same net */
    if (!ip_addr_netcmp_val(ip_addr, netif->ip_addr, ip_2_ip4(&netif->netmask))) {
      ip_addr_set_zero_val(netif->gw);
      if (netif == netif_default) {
        (void)netif_set_default(NULL);
      }
    }

#if LWIP_DHCP
    if ((netif_dhcp_data(netif) != NULL) &&
        (netif_dhcp_data(netif)->client.states[LWIP_DHCP_NATIVE_IDX].state != DHCP_STATE_OFF)) {
      (void)netif_dhcp_off(netif);
    }
#endif
    netif_set_ipaddr(netif, ip_2_ip4(&ip_addr));
  }
#if LWIP_IPV6
  else if (IP_IS_V6_VAL(ifconfig_cmd->ip_addr)) {
    s8_t idx;
    err_t err;
    idx = -1;
    err = netif_add_ip6_address(netif, ip_2_ip6(&ip_addr), &idx);
    if ((err != ERR_OK) || (idx == -1)) {
#ifndef CUSTOM_AT_COMMAND
      err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_MEM_ERR);
#endif
      return -1;
    }
  }
#endif /* LWIP_IPV6 */
  else {
#ifndef CUSTOM_AT_COMMAND
    err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_INVALID);
#endif
    return -1;
  }
  return 0;
}

LWIP_STATIC int
lwip_ifconfig_netmask(struct netif *netif, struct ifconfig_option *ifconfig_cmd)
{
  ip_addr_t netmask;
  struct netif *loc_netif = NULL;
  ip_addr_set_val(netmask, (ifconfig_cmd->netmask));
  /* check data valid */
  if (ip_addr_netmask_valid(ip_2_ip4(&netmask)) == 0) {
#ifndef CUSTOM_AT_COMMAND
    err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_INVALID);
#endif
    return -1;
  }

#if LWIP_DHCP
  if ((netif_dhcp_data(netif) != NULL) &&
      (netif_dhcp_data(netif)->client.states[LWIP_DHCP_NATIVE_IDX].state != DHCP_STATE_OFF)) {
    (void)netif_dhcp_off(netif);
  }
#endif
  if (netif_ip4_netmask(netif)->addr != ip_2_ip4(&netmask)->addr) {
    /* lwip disallow two netif sit in same net at the same time */
    loc_netif = netif_list;
    while (loc_netif != NULL) {
      if (loc_netif == netif) {
        loc_netif = loc_netif->next;
        continue;
      }
      if (ip_addr_cmp(&loc_netif->netmask, &netmask) &&
          ip_addr_netcmp(&loc_netif->ip_addr,
                         &netif->ip_addr, ip_2_ip4(&netmask))) {
#ifndef CUSTOM_AT_COMMAND
        err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_DUPLICATE_NETWORK);
#endif
        return -1;
      }
      loc_netif = loc_netif->next;
    }
    netif_set_netmask(netif, ip_2_ip4(&netmask));
    /* check if gateway still reachable */
    if (!ip_addr_netcmp(&netif->gw,
                        &netif->ip_addr, ip_2_ip4(&netmask))) {
      ip_addr_set_zero_val(netif->gw);
      if (netif == netif_default) {
        (void)netif_set_default(NULL);
      }
    }
  }
  return 0;
}

LWIP_STATIC int
lwip_ifconfig_gw(struct netif *netif, struct ifconfig_option *ifconfig_cmd)
{
  ip_addr_t gw;

  ip_addr_set_val(gw, ifconfig_cmd->gw);

  /* check the address multicast/0/loopback */
  if (ip_addr_ismulticast_val(gw) || ip_addr_isbroadcast_val(gw, netif) ||
      ip_addr_isany(&gw) || ip_addr_isloopback(&gw)) {
#ifndef CUSTOM_AT_COMMAND
    err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_DUPLICATE_NETWORK);
#endif
    return -1;
  }

  /* check if reachable */
  if (!ip_addr_netcmp_val(gw, netif->ip_addr, ip_2_ip4(&netif->netmask))) {
#ifndef CUSTOM_AT_COMMAND
    err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_NO_ROUTE);
#endif
    return -1;
  }

  if (netif_default != netif) {
    ip_addr_set_zero_val(netif->gw);
    (void)netif_set_default(netif);
  }

#if LWIP_DHCP
  if ((netif_dhcp_data(netif) != NULL) &&
      (netif_dhcp_data(netif)->client.states[LWIP_DHCP_NATIVE_IDX].state != DHCP_STATE_OFF)) {
    (void)netif_dhcp_off(netif);
  }
#endif
  netif_set_gw(netif, ip_2_ip4(&gw));
  return 0;
}

#ifndef LWIP_TESTBED
LWIP_STATIC
#endif
void
lwip_ifconfig_internal(void *arg)
{
  struct ifconfig_option *ifconfig_cmd = NULL;
  struct netif *netif = NULL;

  ifconfig_cmd = (struct ifconfig_option *)arg;
  netif = netif_find(ifconfig_cmd->iface);
  if (netif == NULL) {
#ifndef CUSTOM_AT_COMMAND
    err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_DEV_NOT_FOUND);
#endif
    ifconfig_cmd->result = OS_NOK;
    PRINT_ERRCODE(API_SHELL_ERRCODE_DEV_NOT_FOUND);
    goto out;
  }

  /* Saves the current address information of the NetIF interface. */
  struct ifconfig_option ori_ifconfig_cmd;
  ori_ifconfig_cmd.ip_addr = netif->ip_addr;
  ori_ifconfig_cmd.netmask = netif->netmask;
  ori_ifconfig_cmd.gw = netif->gw;
  
  if ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_UP) != 0) {
    (void)netif_set_up(netif);
    goto out;
  } else if ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_DOWN) != 0) {
    (void)netif_set_down(netif);
    goto out;
  }

  if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) ||
      ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_NETMASK) != 0) ||
      ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_HW) != 0)) {
    (void)netif_set_down(netif);
  }

  if ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) {
    if (lwip_ifconfig_ip(netif, ifconfig_cmd) != 0) {
      (void)netif_set_up(netif);
      ifconfig_cmd->result = OS_NOK;
      goto out;
    }
  }

#if LWIP_IPV6
  if ((ifconfig_cmd->option & IFCONFIG_OPTION_DEL_IP) != 0) {
    (void)netif_do_rmv_ipv6_addr(netif, &ifconfig_cmd->ip_addr);
  }
#endif

  if ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_NETMASK) != 0) {
    if (lwip_ifconfig_netmask(netif, ifconfig_cmd) != 0) {
      (void)netif_set_up(netif);
      ifconfig_cmd->result = OS_NOK;
      goto out;
    }
  }

  if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_HW) != 0) &&
      (netif_set_hwaddr(netif, ifconfig_cmd->ethaddr, NETIF_MAX_HWADDR_LEN) != ERR_OK)) {
#ifndef CUSTOM_AT_COMMAND
    err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_SERVICE_FAILURE);
#endif
    (void)netif_set_up(netif);
    ifconfig_cmd->result = OS_NOK;
    goto out;
  }

  if ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_GW) != 0) {
    if (lwip_ifconfig_gw(netif, ifconfig_cmd) != 0) {
      ifconfig_cmd->result = OS_NOK;
      goto out;
    }
  }

  if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) ||
      ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_NETMASK) != 0) ||
      ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_HW) != 0)) {
    (void)netif_set_up(netif);
  }

  if ((ifconfig_cmd->option & IFCONFIG_OPTION_SET_MTU) != 0) {
    if (netif_set_mtu(netif, ifconfig_cmd->mtu) != ERR_OK) {
#ifndef CUSTOM_AT_COMMAND
      err_ifconfig_string_put(ifconfig_cmd, API_SHELL_ERRCODE_INVALID);
#endif
      ifconfig_cmd->result = OS_NOK;
    }
  }
out:

  if(netif && ifconfig_cmd->result == OS_NOK) {
    (void)lwip_ifconfig_ip(netif, &ori_ifconfig_cmd);
    (void)lwip_ifconfig_netmask(netif, &ori_ifconfig_cmd);
    (void)lwip_ifconfig_gw(netif, &ori_ifconfig_cmd);
    (void)netif_set_down(netif);
  }
  
  sys_sem_signal(&ifconfig_cmd->cb_completed);
}

void
lwip_printsize(size_t size)
{
  static const char *sizes[] = { "", "K", "M", "G" };
  size_t divis = 0;
  size_t rem = 0;
  while ((size >= BYTE_CONVERT_UNIT) && (divis < ((sizeof(sizes) / sizeof(char *)) - 1))) {
    rem = (size % BYTE_CONVERT_UNIT);
    divis++;
    size /= BYTE_CONVERT_UNIT;
  }
  /* 1024.0 : byte in float unit */
  LWIP_PLATFORM_PRINT("(%.1f %sB) \r\n", (float)size + (float)rem / 1024.0, sizes[divis]);
}

#ifndef CUSTOM_AT_COMMAND
LWIP_STATIC void
lwip_ifconfig_usage(const char *cmd)
{
  LWIP_PLATFORM_PRINT("Usage:"\
                      CRLF"%s [-a] "\
                      CRLF"[interface]"\
                      CRLF"[interface ipaddr] <netmask mask> <gateway gw>"\
                      CRLF"[interface inet6 add|del ipaddr]"\
                      CRLF"[interface hw ether MAC]"\
                      CRLF"[interface mtu NN]"\
                      CRLF"[interface up|down]"CRLF,
                      cmd);
}
#endif


#ifndef CUSTOM_AT_COMMAND
LWIP_STATIC u32_t lwip_ifconfig_all(int argc, const char **argv)
{
  LWIP_UNUSED_ARG(argc);
  LWIP_UNUSED_ARG(argv);
  /* To support "ifconfig -a" command
   *   RX packets:XXXX errors:X dropped:X overruns:X bytes:XXXX (Human readable format)
   *   TX packets:XXXX errors:X dropped:X overruns:X bytes:XXXX (Human readable format)
   *
   *   Below is assumed for 'overrun' stat.
   *   Linux Kernel:
   *          RX: FIFO overrun
   *              Data structure: net_device->stats->rx_fifo_errors
   *              Flag which is marked when FIFO overrun: ENRSR_FO
   *
   *          Function: ei_receive->ENRSR_FO
   *
   *          TX: A "FIFO underrun" occurred during transmit.
   *              Data structure: net_device->stats->tx_fifo_errors
   *              Flag which is marked when FIFO underrun: ENTSR_FU
   *
   *          Function: ei_tx_intr->ENTSR_FU
   *
   *  LWIP:
   *      So in our case,
   *      while receiving a packet RX case, if the buffer is full (trypost - it is sys_mbox_trypost)
   *      the error will be returned, we can consider that an overflow has happend.
   *      So this can be RX overrun.
   *
   *      But while transmitting a packet TX case, underrun cannot happen because it block on the
   *      message Q if it is full (NOT trypost - it is sys_mbox_post). So TX overrun is always 0.
   */
#if LWIP_STATS
  u32_t stat_err_cnt;
  u32_t stat_drop_cnt;
  u32_t stat_rx_or_tx_cnt;
  u32_t stat_rx_or_tx_bytes;

  stat_rx_or_tx_cnt     = lwip_stats.ip.recv;
  stat_err_cnt          = (u32_t)(lwip_stats.ip.ip_rx_err
                                  + lwip_stats.ip.lenerr
                                  + lwip_stats.ip.chkerr
                                  + lwip_stats.ip.opterr
                                  + lwip_stats.ip.proterr);
  stat_drop_cnt         = (u32_t)(lwip_stats.ip.drop + lwip_stats.link.link_rx_drop);
  stat_rx_or_tx_bytes = lwip_stats.ip.ip_rx_bytes;

  LWIP_PLATFORM_PRINT("%18s:%u\t errors:%u\t ip dropped:%u\t link dropped:%u\t overrun:%hu\t bytes:%u ",
                      "RX packets",
                      stat_rx_or_tx_cnt,
                      stat_err_cnt,
                      stat_drop_cnt,
                      lwip_stats.link.link_rx_drop,
                      lwip_stats.ip.link_rx_overrun,
                      stat_rx_or_tx_bytes);

  /* Print in Human readable format of the incoming bytes */
  lwip_printsize(lwip_stats.ip.ip_rx_bytes);
#if IP6_STATS
  stat_rx_or_tx_cnt     = lwip_stats.ip6.recv;
  stat_err_cnt          = (u32_t)(lwip_stats.ip6.ip_rx_err
                                  + lwip_stats.ip6.lenerr
                                  + lwip_stats.ip6.chkerr
                                  + lwip_stats.ip6.opterr
                                  + lwip_stats.ip6.proterr);
  stat_drop_cnt         = lwip_stats.ip6.drop;
  stat_rx_or_tx_bytes = lwip_stats.ip6.ip_rx_bytes;

  LWIP_PLATFORM_PRINT("%18s:%u\t errors:%u\t dropped:%u\t overrun:%hu\t bytes:%u ",
                      "RX packets(ip6)",
                      stat_rx_or_tx_cnt,
                      stat_err_cnt,
                      stat_drop_cnt,
                      lwip_stats.ip.link_rx_overrun,
                      stat_rx_or_tx_bytes);

  /* Print in Human readable format of the incoming bytes */
  lwip_printsize(lwip_stats.ip6.ip_rx_bytes);
#endif
  stat_rx_or_tx_cnt     = (u32_t)(lwip_stats.ip.fw + lwip_stats.ip.xmit);
  stat_err_cnt          = (u32_t)(lwip_stats.ip.rterr + lwip_stats.ip.ip_tx_err);
  /* IP layer drop stat param is not maintained, failure at IP is considered in 'errors' stat */
  stat_drop_cnt         = lwip_stats.link.link_tx_drop;
  stat_rx_or_tx_bytes   = lwip_stats.ip.ip_tx_bytes;

  LWIP_PLATFORM_PRINT("%18s:%u\t errors:%u\t link dropped:%u\t overrun:0\t bytes:%u",
                      "TX packets",
                      stat_rx_or_tx_cnt,
                      stat_err_cnt,
                      stat_drop_cnt,
                      stat_rx_or_tx_bytes);

  /* Print in Human readable format of the outgoing bytes */
  lwip_printsize(lwip_stats.ip.ip_tx_bytes);

#if IP6_STATS
  stat_rx_or_tx_cnt     = (u32_t)(lwip_stats.ip6.fw + lwip_stats.ip6.xmit);
  stat_err_cnt          = (u32_t)(lwip_stats.ip6.rterr + lwip_stats.ip6.ip_tx_err);
  stat_rx_or_tx_bytes   = lwip_stats.ip6.ip_tx_bytes;

  LWIP_PLATFORM_PRINT("%18s:%u\t errors:%u\t overrun:0\t bytes:%u",
                      "TX packets(ip6)",
                      stat_rx_or_tx_cnt,
                      stat_err_cnt,
                      stat_rx_or_tx_bytes);

  /* Print in Human readable format of the outgoing bytes */
  lwip_printsize(lwip_stats.ip6.ip_tx_bytes);
#endif /* IP6_STATS */
#endif /* LWIP_STATS */
  return 0;
}
#endif

LWIP_STATIC u32_t lwip_ifconfig_callback(tcpip_callback_fn function, struct ifconfig_option *ifconfig_cmd)
{
  if (tcpip_callback(function, ifconfig_cmd) != ERR_OK) {
    sys_sem_free(&ifconfig_cmd->cb_completed);
    PRINT_ERRCODE(API_SHELL_ERRCODE_SERVICE_FAILURE);
    return OS_NOK;
  }
  (void)sys_arch_sem_wait(&ifconfig_cmd->cb_completed, 0);
  sys_sem_free(&ifconfig_cmd->cb_completed);

  ifconfig_cmd->cb_print_buf[PRINT_BUF_LEN - 1] = '\0';
  if (ifconfig_cmd->result != OS_OK) {
    return OS_NOK;
  }
  return OS_OK;
}

LWIP_STATIC u32_t lwip_ifconfig_print_all(struct ifconfig_option *ifconfig_cmd)
{
  u32_t ret = lwip_ifconfig_callback(lwip_ifconfig_show_internal, ifconfig_cmd);
  if (ret != 0) {
    return ret;
  }

#if (LWIP_IFCONFIG_SHOW_SINGLE != lwIP_TRUE)
#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("+IFCONFIG:");
  (void)uapi_at_printf("%s", ifconfig_cmd->cb_print_buf);
  (void)uapi_at_printf("OK"CRLF);
#else
  LWIP_PLATFORM_PRINT("%s", ifconfig_cmd->cb_print_buf);
#endif
#endif
  return OS_OK;
}

LWIP_STATIC u32_t lwip_ifconfig_up_down(struct ifconfig_option *ifconfig_cmd)
{
  u32_t ret = lwip_ifconfig_callback(lwip_ifconfig_internal, ifconfig_cmd);
  if (ret != 0) {
    return ret;
  }

#ifndef CUSTOM_AT_COMMAND
  LWIP_PLATFORM_PRINT("%s", ifconfig_cmd->cb_print_buf);
  return 0;
#else
  return OS_NOK;
#endif
}

LWIP_STATIC u32_t
lwip_ifconfig_prase_inet(int *argc, const char **argv, int *idx,
                         struct ifconfig_option *ifconfig_cmd)
{
#if LWIP_ARP
  struct netif *netiftmp = NULL;
#endif /* LWIP_ARP */

  /* check if set the ip address. */
#if LWIP_ARP
  char buf[IPADDR_STRLEN_MAX] = {0};
  netiftmp = netif_find(ifconfig_cmd->iface);
  if (netiftmp == NULL) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_DEV_NOT_FOUND);
    return EINVAL;
  }
  (void)ipaddr_ntoa_r(&netiftmp->netmask, buf, IPADDR_STRLEN_MAX);
  g_old_ip4mask = ipaddr_addr(buf);
  (void)memset_s(buf, IPADDR_STRLEN_MAX, 0x0, IPADDR_STRLEN_MAX);
  (void)ipaddr_ntoa_r(&netiftmp->gw, buf, IPADDR_STRLEN_MAX);
  g_old_ip4gw = ipaddr_addr(buf);
  (void)memset_s(buf, IPADDR_STRLEN_MAX, 0x0, IPADDR_STRLEN_MAX);
  (void)ipaddr_ntoa_r(&netiftmp->ip_addr, buf, IPADDR_STRLEN_MAX);
  g_old_ip4addr = ipaddr_addr(buf);
#endif /* LWIP_ARP */
  if ((strcmp(argv[*idx], "inet") == 0) || (ip4addr_aton(argv[*idx], ip_2_ip4(&ifconfig_cmd->ip_addr)) != 0)) {
    if (strcmp(argv[*idx], "inet") == 0) {
      if (*argc <= 1) {
        return EINVAL;
      }

      if (ip4addr_aton(argv[*idx + 1], ip_2_ip4(&ifconfig_cmd->ip_addr)) == 0) {
        PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
        return EINVAL;
      }
      (*argc)--;
      (*idx)++;
    }
    IP_SET_TYPE_VAL((ifconfig_cmd->ip_addr), IPADDR_TYPE_V4);
#if LWIP_ARP
    if (!ip_addr_cmp(&ifconfig_cmd->ip_addr, &netiftmp->ip_addr)) {
      ifconfig_cmd->option |= IFCONFIG_OPTION_SET_IP;
    }
#else
    ifconfig_cmd->option |= IFCONFIG_OPTION_SET_IP;
#endif /* LWIP_ARP */
    (*argc)--;
    (*idx)++;
  }
#if LWIP_IPV6
  else if (strcmp(argv[*idx], "inet6") == 0) {
    /* 3 : min argc parameter num from command line */
    if (*argc < 3) {
      return OS_NOK;
    }
    if ((strcmp(argv[*idx + 1], "add") != 0) && (strcmp(argv[*idx + 1], "del") != 0)) {
      return OS_NOK;
    }

    /* 2 : skip two argv index */
    if (ip6addr_aton(argv[*idx + 2], ip_2_ip6(&ifconfig_cmd->ip_addr)) == 0) {
      PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
      return EINVAL;
    }

    IP_SET_TYPE_VAL((ifconfig_cmd->ip_addr), IPADDR_TYPE_V6);
    ifconfig_cmd->option |= (!strcmp(argv[*idx + 1], "add") ? IFCONFIG_OPTION_SET_IP : IFCONFIG_OPTION_DEL_IP);
    *argc -= 3; /* 3: skip already check three parameter */
    (*idx) += 3; /* 3: skip three argv index */
  }
#endif /* LWIP_IPV6 */
  else {
    PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
    return EINVAL;
  }

  if (((ifconfig_cmd->option & IFCONFIG_OPTION_DEL_IP) != 0) && (*argc != 0)) {
    return OS_NOK;
  }

  return OS_OK;
}

LWIP_STATIC u32_t
lwip_ifconfig_prase_additional(int *argc, const char **argv, int *idx,
                               struct ifconfig_option *ifconfig_cmd)
{
  while (*argc > 0) {
    /* if set netmask */
    if ((strcmp("netmask", argv[*idx]) == 0) && (*argc > 1) &&
        (ipaddr_addr(argv[*idx + 1]) != IPADDR_NONE)) {
      ip_addr_set_ip4_u32_val((ifconfig_cmd->netmask), ipaddr_addr(argv[*idx + 1]));
      ifconfig_cmd->option |= IFCONFIG_OPTION_SET_NETMASK;
      *idx += 2; /* 2: skip two argv index */
      *argc -= 2; /* 2: skip already check two parameter */
    } else if ((strcmp("gateway", argv[*idx]) == 0) && (*argc > 1) && /* if set gateway */
               (ipaddr_addr(argv[*idx + 1]) != IPADDR_NONE)) {
      ip_addr_set_ip4_u32_val((ifconfig_cmd->gw), ipaddr_addr(argv[*idx + 1]));
      ifconfig_cmd->option |= IFCONFIG_OPTION_SET_GW;
      *idx += 2; /* 2: skip two argv index */
      *argc -= 2; /* 2: skip already check two parameter */
    } else if ((strcmp("hw", argv[*idx]) == 0) && (*argc > 2) && /* 2 : if set HWaddr and more than two parameter */
               (strcmp("ether", argv[*idx + 1]) == 0)) {
      char *digit = NULL;
      u32_t macaddrlen = strlen(argv[*idx + 2]) + 1; /* 2: skip two argv index */
      char tmp_str[MAX_MACADDR_STRING_LENGTH];
      char *tmp_str1 = NULL;
      char *saveptr = NULL;
      int j;

      if (macaddrlen != MAX_MACADDR_STRING_LENGTH) {
        PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
        return EINVAL;
      }
      /* 2: skip two argv index */
      if (strncpy_s(tmp_str, MAX_MACADDR_STRING_LENGTH, argv[*idx + 2], macaddrlen - 1) != EOK) {
        PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
        return EINVAL;
      }
      /* 6 : max ':' num */
      for (j = 0, tmp_str1 = tmp_str; j < 6; j++, tmp_str1 = NULL) {
        digit = strtok_r(tmp_str1, ":", &saveptr);
        if ((digit == NULL) || (strlen(digit) > 2)) { /* 2 : char len */
          PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
          return EINVAL;
        }
        if(convert_string_to_hex(digit, &ifconfig_cmd->ethaddr[j]) != 0) {
          LWIP_PLATFORM_PRINT("MAC address is not correct"CRLF);
          return EINVAL;        
        }
      }
      ifconfig_cmd->option |= IFCONFIG_OPTION_SET_HW;
      *idx += 3; /* 3: skip already check three parameter */
      *argc -= 3; /* 3: skip three argv index */
    } else if ((strcmp("mtu", argv[*idx]) == 0) && (*argc > 1)) { /* if set mtu */
      if ((atoi(argv[*idx + 1]) < 0) || (atoi(argv[*idx + 1]) > 0xFFFF)) { /* 0xFFFF : max argv num */
        PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
        return EINVAL;
      }

      ifconfig_cmd->mtu = (u16_t)(atoi(argv[*idx + 1]));
      ifconfig_cmd->option |= IFCONFIG_OPTION_SET_MTU;
      *idx += 2; /* 2: skip two argv index */
      *argc -= 2; /* 2: skip already check two parameter */
    } else {
      return OS_NOK;
    }
  }

  return OS_OK;
}

#if LWIP_DHCP && LWIP_DHCP_SUBSTITUTE
LWIP_STATIC err_t
lwip_ifconfig_dhcp_addr_clients_check_fn(struct netif *nif, void *arg)
{
  struct ifconfig_option *ifconfig_cmd = (struct ifconfig_option *)arg;
  struct netif *netif = netif_find(ifconfig_cmd->iface);
  s32_t ret;
  (void)nif;

  if (netif == NULL) {
    ifconfig_cmd->result = lwIP_FALSE;
    PRINT_ERRCODE(API_SHELL_ERRCODE_DEV_NOT_FOUND);
    return ERR_OK;
  }
  ret = dhcp_netif_addr_clients_check(netif, ip_2_ip4(&ifconfig_cmd->ip_addr));
  if ((ret == lwIP_TRUE) && (!ip_addr_cmp(&ifconfig_cmd->ip_addr, &netif->ip_addr))) {
    ifconfig_cmd->result = lwIP_TRUE;
  } else {
    ifconfig_cmd->result = lwIP_FALSE;
  }

  return ERR_OK;
}

LWIP_STATIC u8_t
lwip_ifconfig_dhcp_addr_clients_check(struct ifconfig_option *ifconfig_cmd)
{
  (void)netifapi_call_argcb(lwip_ifconfig_dhcp_addr_clients_check_fn, (void *)ifconfig_cmd);
  return ifconfig_cmd->result;
}
#endif

LWIP_STATIC u32_t
lwip_ifconfig_prase_args(int *argc, const char **argv, int *idx,
                         struct ifconfig_option *ifconfig_cmd)
{
  u32_t ret = lwip_ifconfig_prase_inet(argc, argv, idx, ifconfig_cmd);
  if (ret != 0) {
    return ret;
  }

  ret = lwip_ifconfig_prase_additional(argc, argv, idx, ifconfig_cmd);
  if (ret != 0) {
    return ret;
  }

#if LWIP_DHCP && LWIP_DHCP_SUBSTITUTE
  if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) && IP_IS_V4_VAL((ifconfig_cmd->ip_addr))) {
    if (lwip_ifconfig_dhcp_addr_clients_check(ifconfig_cmd) == lwIP_TRUE) {
      PRINT_ERRCODE(API_SHELL_ERRCODE_IP_CONFLICT);
      return EINVAL;
    }
  }
#endif

#if LWIP_ARP && LWIP_ENABLE_IP_CONFLICT_SIGNAL
  if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) && IP_IS_V4_VAL((ifconfig_cmd->ip_addr))) {
    /* Create the semaphore for ip conflict detection. */
    if (sys_sem_new(&ip_conflict_detect, 0) != ERR_OK) {
      PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
      return EINVAL;
    }
    is_ip_conflict_signal = 1;
  }
#endif /* LWIP_ARP && LWIP_ENABLE_IP_CONFLICT_SIGNAL */

#if LWIP_IPV6
  if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) && IP_IS_V6_VAL((ifconfig_cmd->ip_addr))) {
    /* Create the semaphore for duplicate address detection. */
    if (sys_sem_new(&dup_addr_detect, 0) != ERR_OK) {
      PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
      return EINVAL;
    }
    is_dup_detect_initialized = 1;
  }
#endif /* LWIP_IPV6 */
  return OS_OK;
}

LWIP_STATIC void
lwip_ifconfig_conflict_res_free(struct ifconfig_option *ifconfig_cmd)
{
#if LWIP_ARP && LWIP_ENABLE_IP_CONFLICT_SIGNAL
    if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) && IP_IS_V4_VAL((ifconfig_cmd->ip_addr))) {
      is_ip_conflict_signal = 0;
      sys_sem_free(&ip_conflict_detect);
    }
#endif /* LWIP_ARP && LWIP_ENABLE_IP_CONFLICT_SIGNAL */

#if LWIP_IPV6
    if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) && IP_IS_V6_VAL((ifconfig_cmd->ip_addr))) {
      is_dup_detect_initialized = 0;
      sys_sem_free(&dup_addr_detect);
    }
#endif /* LWIP_IPV6 */
}

LWIP_STATIC u32_t
lwip_ifconfig_conflict_process(struct ifconfig_option *ifconfig_cmd)
{
#if LWIP_ARP && LWIP_ENABLE_IP_CONFLICT_SIGNAL
  u32_t err;
  /* Pend 2 seconds for waiting the arp reply if the ip is already in use. */
  if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) && IP_IS_V4_VAL((ifconfig_cmd->ip_addr))) {
    err = sys_arch_sem_wait(&ip_conflict_detect, DUP_ARP_DETECT_TIME);
    is_ip_conflict_signal = 0;
    sys_sem_free(&ip_conflict_detect);
    if (err == SYS_ARCH_ERROR) {
      /* The result neither conflict nor timeout. */
      PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
      return OS_NOK;
    }
    if (err != SYS_ARCH_TIMEOUT) {
      /* Duplicate use of new ip, restore it to the old one. */
      PRINT_ERRCODE(API_SHELL_ERRCODE_IP_CONFLICT);
      ip_addr_set_ip4_u32_val(ifconfig_cmd->ip_addr, g_old_ip4addr);
      ip_addr_set_ip4_u32_val(ifconfig_cmd->netmask, g_old_ip4mask);
      ifconfig_cmd->option |= IFCONFIG_OPTION_SET_NETMASK;
      ip_addr_set_ip4_u32_val(ifconfig_cmd->gw, g_old_ip4gw);
      ifconfig_cmd->option |= IFCONFIG_OPTION_SET_GW;
      if (sys_sem_new(&ifconfig_cmd->cb_completed, 0) != ERR_OK) {
        PRINT_ERRCODE(API_SHELL_ERRCODE_SEM_ERR);
        return OS_NOK;
      }
      if (tcpip_callback(lwip_ifconfig_internal, ifconfig_cmd) != ERR_OK) {
        PRINT_ERRCODE(API_SHELL_ERRCODE_SERVICE_FAILURE);
        return OS_NOK;
      }
      (void)sys_arch_sem_wait(&ifconfig_cmd->cb_completed, 0);
      ifconfig_cmd->cb_print_buf[PRINT_BUF_LEN - 1] = '\0';
#ifndef CUSTOM_AT_COMMAND
      LWIP_PLATFORM_PRINT("%s", ifconfig_cmd->cb_print_buf);
#endif
      return OS_NOK;
    }
  }
#endif /* LWIP_ARP && LWIP_ENABLE_IP_CONFLICT_SIGNAL */
#if LWIP_IPV6
  if (((ifconfig_cmd->option & IFCONFIG_OPTION_SET_IP) != 0) && IP_IS_V6_VAL(ifconfig_cmd->ip_addr)) {
    /* Pend 2 seconds for waiting the arp reply if the ip is already in use. */
    u32_t retval = sys_arch_sem_wait(&dup_addr_detect, DUP_ARP_DETECT_TIME);
    is_dup_detect_initialized = 0;
    sys_sem_free(&dup_addr_detect);

    if (retval == SYS_ARCH_ERROR) {
      /* The result neither conflict nor timeout. */
      PRINT_ERRCODE(API_SHELL_ERRCODE_SERVICE_FAILURE);
      return OS_NOK;
    }
    if (retval != SYS_ARCH_TIMEOUT) {
      /* Duplicate use of new ip, restore it to the old one. */
      struct netif *netif = NULL;
      PRINT_ERRCODE(API_SHELL_ERRCODE_IP_CONFLICT);
      netif = netif_find(ifconfig_cmd->iface);
      s8_t i = netif_get_ip6_addr_match(netif, &ifconfig_cmd->ip_addr.u_addr.ip6);
      if (i >= 0) {
        netif->ip6_addr_state[i] = IP6_ADDR_INVALID;
      }

      ifconfig_cmd->cb_print_buf[PRINT_BUF_LEN - 1] = '\0';
#ifndef CUSTOM_AT_COMMAND
      LWIP_PLATFORM_PRINT("%s", ifconfig_cmd->cb_print_buf);
#endif
      return OS_NOK;
    }
  }
#endif /* LWIP_IPV6 */

  return OS_OK;
}

LWIP_STATIC u32_t
lwip_ifconfig_basic(int *argc, const char **argv, int *idx,
                    struct ifconfig_option *ifconfig_cmd)
{
  /* Get the interface */
  if (*argc > 0) {
    if (strlen(argv[*idx]) < NETIF_NAMESIZE) {
      (void)strncpy_s(ifconfig_cmd->iface, NETIF_NAMESIZE, argv[*idx], (strlen(argv[*idx])));
      ifconfig_cmd->iface[NETIF_NAMESIZE - 1] = '\0';
    } else {
      sys_sem_free(&ifconfig_cmd->cb_completed);
      PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
      return 1;
    }
    (*idx)++;
    (*argc)--;
  }

  if (*argc == 0) {
    return lwip_ifconfig_print_all(ifconfig_cmd);
  }

  /* ifup/ifdown */
  if (strcmp("up", argv[*idx]) == 0) {
    ifconfig_cmd->option |= IFCONFIG_OPTION_SET_UP;
  } else if (strcmp("down", argv[*idx]) == 0) {
    ifconfig_cmd->option |= IFCONFIG_OPTION_SET_DOWN;
  }

  if ((ifconfig_cmd->option & (IFCONFIG_OPTION_SET_UP | IFCONFIG_OPTION_SET_DOWN)) != 0) {
    return lwip_ifconfig_up_down(ifconfig_cmd);
  }

  /* not a basic process, continue next process */
  return (u32_t)(-1);
}

u32_t
lwip_ifconfig(int argc, const char **argv)
{
  int i;
  u32_t ret;
  if (argv == NULL) {
    return OS_NOK;
  }

  if (!tcpip_init_finish) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_TCPIP_UNINTED);
    return OS_NOK;
  }

#ifndef CUSTOM_AT_COMMAND
  if (argc != 0 && strcmp("-a", argv[0]) == 0) {
    return lwip_ifconfig_all(argc, argv);
  }
#endif

  struct ifconfig_option *ifconfig_cmd = mem_malloc(sizeof(struct ifconfig_option));
  if (ifconfig_cmd == NULL) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_MEM_ERR);
    return OS_NOK;
  }
  (void)memset_s(ifconfig_cmd, sizeof(struct ifconfig_option), 0, sizeof(struct ifconfig_option));
  if (sys_sem_new(&ifconfig_cmd->cb_completed, 0) != ERR_OK) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_SEM_ERR);
    ret = OS_NOK;
    goto ifconfig_free_and_return;
  }

  i = 0;
  ret = lwip_ifconfig_basic(&argc, argv, &i, ifconfig_cmd);
  if (ret != (u32_t)(-1)) {
    goto ifconfig_free_and_return;
  }

  ret = lwip_ifconfig_prase_args(&argc, argv, &i, ifconfig_cmd);
  if (ret != 0) {
#ifndef CUSTOM_AT_COMMAND
    if (ret != EINVAL) {
      lwip_ifconfig_usage("ifconfig");
    }
#endif
    sys_sem_free(&ifconfig_cmd->cb_completed);
    ret = OS_NOK;
    goto ifconfig_free_and_return;
  }

  ret = lwip_ifconfig_callback(lwip_ifconfig_internal, ifconfig_cmd);
  if (ret != 0) {
    lwip_ifconfig_conflict_res_free(ifconfig_cmd);
    PRINT_ERRCODE(API_SHELL_ERRCODE_SERVICE_FAILURE);
    goto ifconfig_free_and_return;
  }
#ifndef CUSTOM_AT_COMMAND
  LWIP_PLATFORM_PRINT("%s", ifconfig_cmd->cb_print_buf);
#endif

  ret = lwip_ifconfig_conflict_process(ifconfig_cmd);
#ifdef CUSTOM_AT_COMMAND
  if (ret == OS_OK) {
    (void)uapi_at_printf("OK"CRLF);
  }
#endif

ifconfig_free_and_return:
  mem_free(ifconfig_cmd);
  return ret;
}

u32_t
os_shell_netif(int argc, const char **argv)
{
  struct netif *netif = NULL;
  if (argv == NULL) {
    return OS_NOK;
  }
  if (argc < 1) {
    LWIP_PLATFORM_PRINT("netif_default wlan0"CRLF);
    return OS_NOK;
  }

  netif = netif_find(argv[0]);
  if (netif == NULL) {
    LWIP_PLATFORM_PRINT("not find %s"CRLF, argv[0]);
    return OS_NOK;
  }

  (void)netifapi_netif_set_default(netif);
  return OS_OK;
}

#endif /* LWIP_ENABLE_BASIC_SHELL_CMD */
