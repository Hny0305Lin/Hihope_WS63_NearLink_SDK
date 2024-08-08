/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: shell cmds APIs implementation about dhcps dhcp dns and so on.
 * Author: none
 * Create: 2020
 */

#ifndef LWIP_OTHERS_H
#define LWIP_OTHERS_H
#include "lwip/opt.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_SNTP
u32_t os_shell_ntpdate(int argc, const char **argv);
#endif

u32_t os_shell_tftp(int argc, const char **argv);

#if LWIP_DNS
u32_t os_shell_dns(int argc, const char **argv);
#ifdef CUSTOM_AT_COMMAND
u32_t os_shell_show_dns(void);
#endif
#endif /* LWIP_DNS */

#if LWIP_DHCP
u32_t os_shell_dhcp(int argc, const char **argv);
void dhcp_clients_info_show(struct netif *netif_p);
#endif /* LWIP_DHCP */

#if (LWIP_IPV6 && (LWIP_IPV6_MLD || LWIP_IPV6_MLD_QUERIER))
u32_t os_shell_mld6(int argc, const char **argv);
#endif /* (LWIP_IPV6 && (LWIP_IPV6_MLD || LWIP_IPV6_MLD_QUERIER)) */

#if LWIP_IPV4 && LWIP_IGMP
u32_t os_shell_igmp(int argc, const char **argv);
u32_t at_os_shell_igmp(int argc, const char **argv);
#endif /* LWIP_IPV4 && LWIP_IGMP */

u32_t os_tcpserver(int argc, const char **argv);
u32_t udpserver(int argc, const char **argv);
#if LWIP_DHCPS
u32_t os_shell_dhcps(int argc, const char **argv);
#endif

void os_shell_reboot(int argc, const char **argv);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_OTHERS_H */
