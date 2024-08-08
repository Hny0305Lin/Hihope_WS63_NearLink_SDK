/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: shell cmds APIs implementation about dhcps dhcp dns and so on.
 * Author: none
 * Create: 2020
 */

#include "lwip/nettool/misc.h"
#include "lwip/sntp.h"
#include "lwip/tftpc.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/sockets.h"
#include "lwip/netifapi.h"
#include "lwip/mld6.h"
#include "lwip/igmp.h"
#include "lwip/tcpip.h"
#include "lwip/init.h"
#include "lwip/nettool/utility.h"
#if LWIP_LITEOS_COMPAT
#include "los_config.h"
#endif

#if LWIP_ENABLE_BASIC_SHELL_CMD

#if LWIP_SNTP
u32_t
os_shell_ntpdate(int argc, const char **argv)
{
  int server_num;
  char *ret = NULL;
  struct timeval get_time;

  (void)memset_s(&get_time, sizeof(struct timeval), 0, sizeof(struct timeval));

  if (tcpip_init_finish == 0) {
    LWIP_PLATFORM_PRINT("%s: tcpip_init have not been called"CRLF, __FUNCTION__);
    return OS_NOK;
  }
  if (argv == NULL) {
    return OS_NOK;
  }
  if (argc < 1) {
    goto usage;
  }

  server_num = lwip_sntp_start(argc, (char **)argv, &get_time);
  if (server_num >= 0 && server_num < argc) {
    ret = ctime((time_t *)&get_time.tv_sec);
    if (ret) {
      LWIP_PLATFORM_PRINT("time server %s: %s"CRLF, argv[server_num], ret);
    } else {
      LWIP_PLATFORM_PRINT("ctime return null error"CRLF);
    }
  } else {
    LWIP_PLATFORM_PRINT("no server suitable for synchronization found"CRLF);
  }

  return OS_OK;

usage:
  LWIP_PLATFORM_PRINT("\nUsage:\n");
  LWIP_PLATFORM_PRINT("ntpdate [SERVER_IP1] [SERVER_IP2] ..."CRLF);
  return OS_NOK;
}

#endif /* LWIP_SNTP */

#if LWIP_TFTP
static char *tftp_error[] = {
  "TFTP transfer finish"CRLF,
  "Error while creating UDP socket"CRLF,
  "Error while binding to the UDP socket"CRLF,
  "Error returned by lwip_select() system call"CRLF,
  "Error while receiving data from the peer"CRLF,
  "Error while sending data to the peer"CRLF,
  "Requested file is not found"CRLF,
  "This is the error sent by the server when hostname cannot be resolved"CRLF,
  "Input paramters passed to TFTP interfaces are invalid"CRLF,
  "Error detected in TFTP packet or the error received from the TFTP server"CRLF,
  "Error during packet synhronization while sending or unexpected packet is received"CRLF,
  "File size limit crossed, Max block can be 0xFFFF, each block containing 512 bytes"CRLF,
  "File name lenght greater than 256"CRLF,
  "Hostname IP is not valid"CRLF,
  "TFTP server returned file access error"CRLF,
  "TFTP server returned error signifying that the DISK is full to write"CRLF,
  "TFTP server returned error signifying that the file exist"CRLF,
  "The source file name do not exisits"CRLF,
  "Memory allocaion failed in TFTP client"CRLF,
  "File open failed"CRLF,
  "File read error"CRLF,
  "File create error"CRLF,
  "File write error"CRLF,
  "Max time expired while waiting for file to be recived"CRLF,
  "Error when the received packet is less than 4bytes(error lenght) or greater than 512bytes"CRLF,
  "Returned by TFTP server for protocol user error"CRLF,
  "The destination file path length greater than 256"CRLF,
  "Returned by TFTP server for undefined transfer ID"CRLF,
  "IOCTL fucntion failed at TFTP client while setting the socket to non-block"CRLF,
};

static u32_t
tftp_invoke_operation(u8_t uc_tftp_get, u32_t ul_remote_addr, s8_t *sz_local_file_name, s8_t *sz_remote_file_name)
{
  u16_t us_tftp_serv_port = 69;
  u32_t ret;
  if (sz_local_file_name == NULL || sz_remote_file_name == NULL) {
    return OS_NOK;
  }

  if (uc_tftp_get) {
    ret = lwip_tftp_get_file_by_filename(ntohl(ul_remote_addr), us_tftp_serv_port,
                                         TRANSFER_MODE_BINARY, sz_remote_file_name, sz_local_file_name);
  } else {
    ret = lwip_tftp_put_file_by_filename(ntohl(ul_remote_addr), us_tftp_serv_port,
                                         TRANSFER_MODE_BINARY, sz_local_file_name, sz_remote_file_name);
  }

  LWIP_ASSERT("TFTP UNKNOW ERROR!", ret < LWIP_ARRAYSIZE(tftp_error));
  LWIP_PLATFORM_PRINT("%s", tftp_error[ret]);
  if (ret) {
    return OS_NOK;
  } else {
    return OS_OK;
  }
}

u32_t os_shell_tftp(int argc, const char **argv)
{
  /* log off temporary for upgrade lwip to 2.0 */
  u32_t  ul_remote_addr = IPADDR_NONE;
  u8_t   uc_tftp_get = 0;
  s8_t   *sz_local_file_name = NULL;
  s8_t   *sz_remote_file_name = NULL;
  int i = 0;
  if (argv == NULL) {
    return OS_NOK;
  }
  if (!tcpip_init_finish) {
    LWIP_PLATFORM_PRINT("%s: tcpip_init have not been called"CRLF, __FUNCTION__);
    return OS_NOK;
  }

  while (i < argc) {
    if (strcmp(argv[i], "-p") == 0) {
      uc_tftp_get = 0;
      i++;
      continue;
    }

    if (strcmp(argv[i], "-g") == 0) {
      uc_tftp_get = 1;
      i++;
      continue;
    }

    if (strcmp(argv[i], "-l") == 0 && ((i + 1) < argc)) {
      sz_local_file_name = (s8_t *)argv[i + 1];
      i += 2;
      continue;
    }

    if (strcmp(argv[i], "-r") == 0 && ((i + 1) < argc)) {
      sz_remote_file_name = (s8_t *)argv[i + 1];
      i += 2;
      continue;
    }

    if ((i + 1) == argc) {
      ul_remote_addr = inet_addr(argv[i]);
      break;
    }

    goto usage;
  }

  if (ul_remote_addr == IPADDR_NONE || sz_local_file_name == NULL || sz_remote_file_name == NULL) {
    goto usage;
  }

  return tftp_invoke_operation(uc_tftp_get, ul_remote_addr, sz_local_file_name, sz_remote_file_name);

usage:
  LWIP_PLATFORM_PRINT("usage:\nTransfer a file from/to tftp server"CRLF);
  LWIP_PLATFORM_PRINT("tftp <-g/-p> -l FullPathLocalFile -r RemoteFile Host"CRLF);
  return OS_NOK;
}
#endif /* LWIP_TFTP */

#if LWIP_DNS
u32_t os_shell_show_dns(void)
{
  ip_addr_t dns;
  err_t err;
  int i;
#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("+DNS:"CRLF);
#else
  LWIP_PLATFORM_PRINT("+DNS:"CRLF);
#endif
  
  for (i = 0; i < DNS_MAX_SERVERS; i++) {
    err = lwip_dns_getserver((u8_t)i, &dns);
    if (err == ERR_OK) {
      char buf[IPADDR_STRLEN_MAX];
      (void)ipaddr_ntoa_r(&dns, buf, IPADDR_STRLEN_MAX);
#ifdef CUSTOM_AT_COMMAND
      (void)uapi_at_printf("%s"CRLF, buf);
#else
      LWIP_PLATFORM_PRINT("%s"CRLF, buf);
#endif
    } else {
      return OS_NOK;
    }
  }
#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("OK"CRLF);
#else
  LWIP_PLATFORM_PRINT("OK"CRLF);
#endif
  return OS_OK;
}

u32_t
os_shell_dns(int argc, const char **argv)
{
  ip_addr_t dns;
  err_t err;
  int i;

  if (tcpip_init_finish == 0) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_TCPIP_UNINTED);
    return OS_NOK;
  }
  if (argv == NULL) {
    return OS_NOK;
  }

  if(lwip_isdigitstr((s8_t *)argv[0]) != 0) {
#ifdef CUSTOM_AT_COMMAND
    (void)uapi_at_printf("dns error parameter:%s"CRLF, argv[0]);
#else
    LWIP_PLATFORM_PRINT("dns error parameter:%s"CRLF, argv[0]);
#endif
    return OS_NOK;
  }

  if ((argc == 1) && (strcmp(argv[0], "-a") == 0)) {
    for (i = 0; i < DNS_MAX_SERVERS; i++) {
      err = lwip_dns_getserver((u8_t)i, &dns);
      if (err == ERR_OK) {
        char buf[IPADDR_STRLEN_MAX];
        (void)ipaddr_ntoa_r(&dns, buf, IPADDR_STRLEN_MAX);
#ifdef CUSTOM_AT_COMMAND
        (void)uapi_at_printf("dns %d: %s"CRLF, i + 1, buf);
#else
        LWIP_PLATFORM_PRINT("dns %d: %s"CRLF, i + 1, buf);
#endif
      } else {
#ifdef CUSTOM_AT_COMMAND
        (void)uapi_at_printf("dns: failed"CRLF);
#else
        LWIP_PLATFORM_PRINT("dns: failed"CRLF);
#endif
        return OS_NOK;
      }
    }
#ifdef CUSTOM_AT_COMMAND
    (void)uapi_at_printf("OK"CRLF);
#endif
    return OS_OK;
  } else if (argc == 2) { /* 2 : current argc index */
    i = atoi(argv[0]);
    if ((i <= 0) || (i > DNS_MAX_SERVERS)) {
      goto usage;
    }
#if LWIP_IPV6
    if (ip6addr_aton(argv[1], ((ip6_addr_t *)&dns))) {
#if LWIP_IPV4 && LWIP_IPV6
      dns.type = IPADDR_TYPE_V6;
#endif
#ifdef LWIP_DNS_GLOBAL_ADDR
      if (!ip6_addr_isglobal((ip6_addr_t *)&dns)) {
#ifdef CUSTOM_AT_COMMAND
        (void)uapi_at_printf("ip address<%s> is wrong"CRLF, argv[1]);
#else
        PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
#endif
        return OS_NOK;
      }
#endif
    } else
#endif
    {
#if LWIP_IPV4
      ((ip4_addr_t *)&dns)->addr = ipaddr_addr(argv[1]);
      if (((ip4_addr_t *)&dns)->addr == IPADDR_NONE) {
#ifdef CUSTOM_AT_COMMAND
        (void)uapi_at_printf("ip address<%s> is wrong"CRLF, argv[1]);
#else
        PRINT_ERRCODE(API_SHELL_ERRCODE_INVALID);
#endif
        return OS_NOK;
      }
#if LWIP_IPV4 && LWIP_IPV6
      dns.type = IPADDR_TYPE_V4;
#endif
#endif
    }

    err = lwip_dns_setserver((u8_t)(i - 1), &dns);
    if (err != ERR_OK) {
#ifdef CUSTOM_AT_COMMAND
      (void)uapi_at_printf("dns : failed"CRLF);
#else
      LWIP_PLATFORM_PRINT("dns : failed"CRLF);
#endif
      return OS_NOK;
    }
#ifdef CUSTOM_AT_COMMAND
    (void)uapi_at_printf("OK"CRLF);
#endif
    return OS_OK;
  }
usage:
#ifndef CUSTOM_AT_COMMAND
  LWIP_PLATFORM_PRINT("usage:"CRLF);
  LWIP_PLATFORM_PRINT("\tdns <1-%d> <IP>"CRLF, DNS_MAX_SERVERS);
  LWIP_PLATFORM_PRINT("\tdns -a"CRLF);
#endif
  return OS_NOK;
}
#endif /* LWIP_DNS */

#ifdef LWIP_TESTBED
extern void cmd_reset(void);

void os_shell_reboot(int argc, const char **argv)
{
  cmd_reset();
}

#endif /* LWIP_TESTBED */

#if LWIP_DHCP
err_t dhcp_idx_to_mac(const struct netif *netif, dhcp_num_t mac_idx, u8_t *hwaddr, u8_t *hwaddr_len);
static void
dhcp_client_info_show(struct dhcp_client *dhcp)
{
  ip4_addr_t cli_ip;
  struct dhcp_state *dhcp_state = &((dhcp->states)[dhcp->cli_idx]);
  char macaddr[MACADDR_BUF_LEN];
  char *char_p = macaddr;
  u8_t i;
  u8_t state;
  char buf[IP4ADDR_STRLEN_MAX];

  (void)memset_s(macaddr, sizeof(macaddr), 0, sizeof(macaddr));
  for (i = 0; i < dhcp_state->hwaddr_len; i++) {
    /* 2 : macaddr show in two hex */
    (void)snprintf_s(char_p, (size_t)(MACADDR_BUF_LEN - 2 * i), (size_t)(MACADDR_BUF_LEN - 2 * i - 1),
                     "%02x", dhcp->hwaddr[i]);
    /* 2 : skip two index */
    char_p = char_p + 2;
  }
  if (ip4_addr_isany_val(dhcp->relay_ip)) {
    DHCP_HOST_TO_IP(cli_ip.addr, ip_2_ip4(&dhcp->server_ip_addr)->addr, dhcp->offered_sn_mask.addr,
                    dhcp_state->offered_ip_addr);
  } else {
    DHCP_HOST_TO_IP(cli_ip.addr, dhcp->relay_ip.addr, dhcp->offered_sn_mask.addr,
                    dhcp_state->offered_ip_addr);
  }
  state = dhcp_state->state;

  (void)ip4addr_ntoa_r(&cli_ip, buf, IP4ADDR_STRLEN_MAX);
#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("\t%-8"DHCP_NUM_F"%-16s%-16s%-8hhu%-8hu%-8hhu%-8hu"CRLF, dhcp_state->idx, macaddr,
                     (dhcp_state->offered_ip_addr == 0) ? "0.0.0.0" : buf,
                     state, dhcp_state->lease_used, dhcp_state->tries, dhcp_state->request_timeout);
#else
  LWIP_PLATFORM_PRINT("\t%-8"DHCP_NUM_F"%-16s%-16s%-8hhu%-8hu%-8hhu%-8hu"CRLF, dhcp_state->idx, macaddr,
                      (dhcp_state->offered_ip_addr == 0) ? "0.0.0.0" : buf,
                      state, dhcp_state->lease_used, dhcp_state->tries, dhcp_state->request_timeout);
#endif

  return;
}

void
dhcp_clients_info_show(struct netif *netif_p)
{
  struct dhcp *netif_dhcp = NULL;
  struct dhcp_client *dhcp = NULL;
  struct dhcp_state *dhcp_state = NULL;
  int i;
  u8_t hwaddr_len;
  char buf[IP4ADDR_STRLEN_MAX];

  if (netif_p == NULL) {
    return;
  }
  netif_dhcp = netif_dhcp_data(netif_p);
  if (netif_dhcp == NULL) {
#ifdef CUSTOM_AT_COMMAND
    (void)uapi_at_printf("dhcpc not start on %s%hhu"CRLF, netif_p->name, netif_p->num);
#else
    PRINT_ERRCODE(API_SHELL_ERRCODE_DEV_NOT_READY);
#endif
    return;
  }
  dhcp = &(netif_dhcp->client);
#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("server :"CRLF);
  (void)ip4addr_ntoa_r(ip_2_ip4(&(dhcp->server_ip_addr)), buf, IP4ADDR_STRLEN_MAX);
  (void)uapi_at_printf("\tserver_id : %s"CRLF, buf);
  (void)ip4addr_ntoa_r(&(dhcp->offered_sn_mask), buf, IP4ADDR_STRLEN_MAX);
  (void)uapi_at_printf("\tmask : %s, %hhu"CRLF, buf, dhcp->subnet_mask_given);
  (void)ip4addr_ntoa_r(&(dhcp->offered_gw_addr), buf, IP4ADDR_STRLEN_MAX);
  (void)uapi_at_printf("\tgw : %s"CRLF, buf);
  (void)uapi_at_printf("\tT0 : %u"CRLF, dhcp->offered_t0_lease);
  (void)uapi_at_printf("\tT1 : %u"CRLF, dhcp->offered_t1_renew);
  (void)uapi_at_printf("\tT2 : %u"CRLF, dhcp->offered_t2_rebind);
#else
  LWIP_PLATFORM_PRINT("server :"CRLF);
  (void)ip4addr_ntoa_r(ip_2_ip4(&(dhcp->server_ip_addr)), buf, IP4ADDR_STRLEN_MAX);
  LWIP_PLATFORM_PRINT("\tserver_id : %s"CRLF, buf);
  (void)ip4addr_ntoa_r(&(dhcp->offered_sn_mask), buf, IP4ADDR_STRLEN_MAX);
  LWIP_PLATFORM_PRINT("\tmask : %s, %hhu"CRLF, buf, dhcp->subnet_mask_given);
  (void)ip4addr_ntoa_r(&(dhcp->offered_gw_addr), buf, IP4ADDR_STRLEN_MAX);
  LWIP_PLATFORM_PRINT("\tgw : %s"CRLF, buf);
  LWIP_PLATFORM_PRINT("\tT0 : %u"CRLF, dhcp->offered_t0_lease);
  LWIP_PLATFORM_PRINT("\tT1 : %u"CRLF, dhcp->offered_t1_renew);
  LWIP_PLATFORM_PRINT("\tT2 : %u"CRLF, dhcp->offered_t2_rebind);
#endif
  if (ip4_addr_isany_val(*ip_2_ip4(&dhcp->server_ip_addr))) {
    return;
  }

#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("clients <%"DHCP_NUM_F"> :"CRLF, dhcp->cli_cnt);
  (void)uapi_at_printf("\t%-8s%-16s%-16s%-8s%-8s%-8s%-8s"CRLF,
                     "mac_idx", "mac", "addr", "state", "lease", "tries", "rto");
#else
  LWIP_PLATFORM_PRINT("clients <%"DHCP_NUM_F"> :"CRLF, dhcp->cli_cnt);
  LWIP_PLATFORM_PRINT("\t%-8s%-16s%-16s%-8s%-8s%-8s%-8s"CRLF,
                      "mac_idx", "mac", "addr", "state", "lease", "tries", "rto");
#endif
  for (i = 0; i < DHCP_CLIENT_NUM; i++) {
    dhcp->cli_idx = (dhcp_num_t)i;
    dhcp_state = &((dhcp->states)[i]);
    if ((i != LWIP_DHCP_NATIVE_IDX) && (dhcp_state->idx == 0)) {
      continue;
    }
    if (dhcp_idx_to_mac(netif_p, dhcp_state->idx, dhcp->hwaddr, &hwaddr_len) != ERR_OK) {
#ifdef CUSTOM_AT_COMMAND
      (void)uapi_at_printf("<%d> idx %"DHCP_NUM_F" to mac failed"CRLF, i, dhcp_state->idx);
#else
      LWIP_PLATFORM_PRINT("<%d> idx %"DHCP_NUM_F" to mac failed"CRLF, i, dhcp_state->idx);
#endif
      continue;
    }
    dhcp_state->hwaddr_len = hwaddr_len;
    dhcp_client_info_show(dhcp);
  }

  return;
}

u32_t
os_shell_dhcp(int argc, const char **argv)
{
  int ret = 0;
  struct netif *netif_p = NULL;

  if (tcpip_init_finish == 0) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_TCPIP_UNINTED);
    return OS_NOK;
  }

  if ((argc != 2) || (argv == NULL)) { /* 2 : total argc num */
    goto usage;
  }

  netif_p = netif_find(argv[0]);
  if (netif_p == NULL) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_DEV_NOT_FOUND);
    goto usage;
  }
#ifdef CUSTOM_AT_COMMAND
  if (strcmp(argv[1], "1") == 0) {
    ret = netifapi_dhcp_start(netif_p);
  } else if (strcmp(argv[1], "0") == 0) {
    ret = netifapi_dhcp_stop(netif_p);
  } else {
    goto usage;
  }
#else
  if (strcmp(argv[1], "start") == 0) {
    ret = netifapi_dhcp_start(netif_p);
  } else if (strcmp(argv[1], "stop") == 0) {
    ret = netifapi_dhcp_stop(netif_p);
  } else if (strcmp(argv[1], "inform") == 0) {
    ret = netifapi_dhcp_inform(netif_p);
  } else if (strcmp(argv[1], "renew") == 0) {
    ret = netifapi_dhcp_renew(netif_p);
  } else if (strcmp(argv[1], "release") == 0) {
    ret = netifapi_dhcp_release(netif_p);
  } else if (strcmp(argv[1], "show") == 0) {
    ret = netifapi_netif_common(netif_p, dhcp_clients_info_show, NULL);
  } else {
    goto usage;
  }
#endif
  if (ret == OS_OK) {
    LWIP_PLATFORM_PRINT("OK"CRLF);
  }

  return (ret == 0) ? OS_OK : OS_NOK;
usage:
#ifndef CUSTOM_AT_COMMAND
  LWIP_PLATFORM_PRINT("dhcp\n\tifname {start | stop | inform | renew | release | show}"CRLF);
#endif
  return OS_NOK;
}
#endif /* LWIP_DHCP */

#if (LWIP_IPV6 && (LWIP_IPV6_MLD || LWIP_IPV6_MLD_QUERIER))
static void
os_shell_mld6_usage(void)
{
#ifdef LWIP_DEBUG_OPEN
  (void)uapi_at_printf("mld6\r\n\tshow\r\n\tifname {report | stop}\r\n\tifname {join | leave} groupaddr\r\n\t\
                     ifname querier {start | stop | show}"CRLF);
#else
  LWIP_PLATFORM_PRINT("mld6\n\tshow\n\tifname {report | stop}\n\tifname {join | leave} groupaddr\n\tifname querier \
                      {start | stop | show}"CRLF);
#endif
}

static void
mld6_netif_show(void)
{
  struct netif *netif_p = NULL;
  struct mld_group *group = NULL;
  char buf[IP6ADDR_STRLEN_MAX];

#ifdef LWIP_DEBUG_OPEN
  (void)uapi_at_printf("\t%-50s%-16s%-16s%-16s%-16s"CRLF, "groupaddr", "reporter", "state", "timer(100ms)", "use");
#else
  LWIP_PLATFORM_PRINT("\t%-50s%-16s%-16s%-16s%-16s"CRLF, "groupaddr", "reporter", "state", "timer(100ms)", "use");
#endif
  for (netif_p = netif_list; netif_p != NULL; netif_p = netif_p->next) {
    if ((netif_p->flags & NETIF_FLAG_MLD6) == 0) {
      continue;
    }
#ifdef LWIP_DEBUG_OPEN
    (void)uapi_at_printf("%s%hhu"CRLF, netif_p->name, netif_p->num);
#else
    LWIP_PLATFORM_PRINT("%s%hhu"CRLF, netif_p->name, netif_p->num);
#endif
    group = netif_mld6_data(netif_p);
    if (group == NULL) {
#ifdef LWIP_DEBUG_OPEN
      (void)uapi_at_printf(CRLF);
#else
      LWIP_PLATFORM_PRINT("\n");
#endif
      continue;
    }
    while (group != NULL) {
      (void)ip6addr_ntoa_r(&(group->group_address), buf, IP6ADDR_STRLEN_MAX);
#ifdef LWIP_DEBUG_OPEN
      (void)uapi_at_printf("\t%-50s%-16hhu%-16hhu%-16hu%-16hhu"CRLF, buf,
                         group->last_reporter_flag, group->group_state,
                         group->timer, group->use);
#else
      LWIP_PLATFORM_PRINT("\t%-50s%-16hhu%-16hhu%-16hu%-16hhu"CRLF, buf,
                          group->last_reporter_flag, group->group_state,
                          group->timer, group->use);
#endif
      group = group->next;
    }
  }
}

static s32_t
mld6_netif_ctrl(const char **argv)
{
  s32_t ret = 0;
  struct netif *netif_p = NULL;

  netif_p = netif_find(argv[0]);
  if (netif_p == NULL) {
    LWIP_PLATFORM_PRINT("no such netif named %s"CRLF, argv[0]);
    goto usage;
  }
  if (strcmp(argv[1], "stop") == 0) {
    ret = mld6_stop(netif_p);
  } else if (strcmp(argv[1], "report") == 0) {
    mld6_report_groups(netif_p);
  } else {
    goto usage;
  }

  return ret;
usage:
  os_shell_mld6_usage();
  return -1;
}

static s32_t
mld6_membership_ctrl(const char **argv)
{
  s32_t ret = 0;
  struct netif *netif_p = NULL;
  ip6_addr_t groupaddr = {0};

  if (!ip6addr_aton(argv[2], &(groupaddr))) {
    LWIP_PLATFORM_PRINT("Invalid IPv6 Address : %s"CRLF, argv[2]);
    goto usage;
  }

  if (ip6_addr_isany_val(groupaddr) || ip6_addr_isnone_val(groupaddr) || !(ip6_addr_ismulticast(&groupaddr))) {
    LWIP_PLATFORM_PRINT("invalid groupaddr %s"CRLF, argv[2]);
    goto usage;
  }
  netif_p = netif_find(argv[0]);
  if (netif_p == NULL) {
    LWIP_PLATFORM_PRINT("no such netif named %s"CRLF, argv[0]);
    goto usage;
  }
  if (strcmp(argv[1], "join") == 0) {
    ret = mld6_joingroup_netif(netif_p, &groupaddr);
  } else if (strcmp(argv[1], "leave") == 0) {
    ret = mld6_leavegroup_netif(netif_p, &groupaddr);
  } else {
    goto usage;
  }

  return ret;
usage:
  os_shell_mld6_usage();
  return -1;
}

#if LWIP_IPV6_MLD_QUERIER
static void
mld6_querier_status_show(struct mld6_querier *querier)
{
  struct mld6_listener *listener = NULL;
  char buf[IP6ADDR_STRLEN_MAX];

#ifdef LWIP_DEBUG_OPEN
  (void)uapi_at_printf("\t%-16s%-16s%-16s"CRLF, "state", "count", "timer");
  (void)uapi_at_printf("\t%-16hhu%-16hhu%-16hu"CRLF, querier->querier_state, querier->query_count, querier->timer);
  (void)uapi_at_printf("listeners:"CRLF);
#else
  LWIP_PLATFORM_PRINT("\t%-16s%-16s%-16s"CRLF, "state", "count", "timer");
  LWIP_PLATFORM_PRINT("\t%-16hhu%-16hhu%-16hu"CRLF, querier->querier_state, querier->query_count, querier->timer);
  LWIP_PLATFORM_PRINT("listeners:"CRLF);
#endif

  if (querier->listeners != NULL) {
#ifdef LWIP_DEBUG_OPEN
    (void)uapi_at_printf("\t%-50s%-16s%-16s%-16s%-16s"CRLF, "addr", "state", "count", "timer", "rexmt_timer");
#else
    LWIP_PLATFORM_PRINT("\t%-50s%-16s%-16s%-16s%-16s"CRLF, "addr", "state", "count", "timer", "rexmt_timer");
#endif
    listener = querier->listeners;
    do {
      (void)ip6addr_ntoa_r(&(listener->group_address), buf, IP6ADDR_STRLEN_MAX);
#ifdef LWIP_DEBUG_OPEN
      (void)uapi_at_printf("\t%-50s%-16hhu%-16hhu%-16hu%-16hu"CRLF, buf,
                         listener->state, listener->query_count, listener->timer, listener->rexmt_timer);
#else
      LWIP_PLATFORM_PRINT("\t%-50s%-16hhu%-16hhu%-16hu%-16hu"CRLF, buf,
                          listener->state, listener->query_count, listener->timer, listener->rexmt_timer);
#endif
      listener = listener->next;
    } while (listener != NULL);
  }

  return;
}
#endif /* LWIP_IPV6_MLD_QUERIER */

static s32_t
mld6_querier_ctrl(const char **argv)
{
#if LWIP_IPV6_MLD_QUERIER
  s32_t ret = 0;
  struct netif *netif_p = NULL;
  struct mld6_querier *querier = NULL;

  netif_p = netif_find(argv[0]);
  if (netif_p == NULL) {
    LWIP_PLATFORM_PRINT("no such netif named %s"CRLF, argv[0]);
    goto usage;
  }
  if (strcmp(argv[2], "start") == 0) {
    ret = mld6_querier_start(netif_p);
  } else if (strcmp(argv[2], "stop") == 0) {
    mld6_querier_stop(netif_p);
  } else if (strcmp(argv[2], "show") == 0) {
    querier = netif_mld6_querier_data(netif_p);
    if (querier == NULL) {
      LWIP_PLATFORM_PRINT("mld6 querier not start"CRLF);
    } else {
      mld6_querier_status_show(querier);
    }
  } else {
    goto usage;
  }

  return ret;
usage:
  os_shell_mld6_usage();
  return -1;
#else
  (void)argv;
  LWIP_PLATFORM_PRINT("mld6 querier not support"CRLF);
  return -1;
#endif /* LWIP_IPV6_MLD_QUERIER */
}

u32_t
os_shell_mld6(int argc, const char **argv)
{
  s32_t ret = 0;

  if (tcpip_init_finish == 0) {
#ifdef LWIP_DEBUG_OPEN
    (void)uapi_at_printf("%s: tcpip_init have not been called"CRLF, __FUNCTION__);
#else
    LWIP_PLATFORM_PRINT("%s: tcpip_init have not been called"CRLF, __FUNCTION__);
#endif
    return OS_NOK;
  }
  if (argv == NULL) {
    return OS_NOK;
  }
  if ((argc == 1) && (strcmp(argv[0], "show") == 0)) {
    mld6_netif_show();
  } else if (argc == 2) { /* 2 : argc index */
    ret = mld6_netif_ctrl(argv);
  } else if (argc == 3) { /* 3 : argc index */
    if (strcmp(argv[1], "querier") == 0) {
      ret = mld6_querier_ctrl(argv);
    } else {
      ret = mld6_membership_ctrl(argv);
    }
  } else {
    os_shell_mld6_usage();
  }
#ifdef LWIP_DEBUG_OPEN
  (void)uapi_at_printf("mld6 ret %d"CRLF, ret);
#else
  LWIP_PLATFORM_PRINT("mld6 ret %d"CRLF, ret);
#endif
  return OS_OK;
}

#endif /* (LWIP_IPV6 && (LWIP_IPV6_MLD || LWIP_IPV6_MLD_QUERIER)) */

#if LWIP_IPV4 && LWIP_IGMP
static void
os_shell_igmp_usage(void)
{
  LWIP_PLATFORM_PRINT("igmp\tshow\tifname {start | stop | report}\t{ifname | ifaddr} {join | leave} groupaddr"CRLF);
}

static void
igmp_netif_show(void)
{
  struct netif *netif_p = NULL;
  struct igmp_group *igmp_group_p = NULL;
  char buf[IP4ADDR_STRLEN_MAX];
#ifdef LWIP_DEBUG_OPEN
  (void)uapi_at_printf("\t%-16s%-16s%-16s%-16s%-16s"CRLF, "groupaddr", "reporter", "state", "timer(100ms)", "use");
#else
  LWIP_PLATFORM_PRINT("\t%-16s%-16s%-16s%-16s%-16s"CRLF, "groupaddr", "reporter", "state", "timer(100ms)", "use");
#endif
  for (netif_p = netif_list; netif_p != NULL; netif_p = netif_p->next) {
    if ((netif_p->flags & NETIF_FLAG_IGMP) == 0) {
      continue;
    }
#ifdef LWIP_DEBUG_OPEN
    (void)uapi_at_printf("%s%d"CRLF, netif_p->name, netif_p->num);
#else
    LWIP_PLATFORM_PRINT("%s%d"CRLF, netif_p->name, netif_p->num);
#endif
    igmp_group_p = netif_igmp_data(netif_p);
    if (igmp_group_p == NULL) {
#ifdef LWIP_DEBUG_OPEN
      (void)uapi_at_printf(CRLF);
#else
      LWIP_PLATFORM_PRINT(CRLF);
#endif
      continue;
    }
    while (igmp_group_p != NULL) {
      (void)ip4addr_ntoa_r(&(igmp_group_p->group_address), buf, IP4ADDR_STRLEN_MAX);
#ifdef LWIP_DEBUG_OPEN
      (void)uapi_at_printf("\t%-16s%-16hhu%-16hhu%-16hu%-16hhu"CRLF, buf,
                         igmp_group_p->last_reporter_flag, igmp_group_p->group_state,
                         igmp_group_p->timer, igmp_group_p->use);
#else
      LWIP_PLATFORM_PRINT("\t%-16s%-16hhu%-16hhu%-16hu%-16hhu"CRLF, buf,
                          igmp_group_p->last_reporter_flag, igmp_group_p->group_state,
                          igmp_group_p->timer, igmp_group_p->use);
#endif
      igmp_group_p = igmp_group_p->next;
    }
  }
}

static s32_t
igmp_netif_control(const char **argv)
{
  s32_t ret = 0;
  struct netif *netif_p = NULL;

  netif_p = netif_find(argv[0]);
  if (netif_p == NULL) {
#ifdef LWIP_DEBUG_OPEN
    (void)uapi_at_printf("no such netif named %s"CRLF, argv[0]);
#else
    LWIP_PLATFORM_PRINT("no such netif named %s"CRLF, argv[0]);
#endif
    goto usage;
  }
  if (strcmp(argv[1], "start") == 0) {
    ret = igmp_start(netif_p);
  } else if (strcmp(argv[1], "stop") == 0) {
    ret = igmp_stop(netif_p);
  } else if (strcmp(argv[1], "report") == 0) {
    igmp_report_groups(netif_p);
  } else {
    goto usage;
  }

  return ret;
usage:
  os_shell_igmp_usage();
  return -1;
}

static s32_t
igmp_membership_netif_ctrl(struct netif *netif_p,
                           ip4_addr_t groupaddr, const char **argv)
{
  s32_t ret;

  if (strcmp(argv[1], "join") == 0) {
    ret = igmp_joingroup_netif(netif_p, &groupaddr);
  } else if (strcmp(argv[1], "leave") == 0) {
    ret = igmp_leavegroup_netif(netif_p, &groupaddr);
  } else {
    ret = -1;
    os_shell_igmp_usage();
  }

  return ret;
}

static s32_t
igmp_membership_addr_ctrl(ip4_addr_t groupaddr,
                          const char **argv)
{
  s32_t ret = 0;
  ip4_addr_t ifaddr = {0};

  ifaddr.addr = inet_addr(argv[0]);
  if (ifaddr.addr == IPADDR_NONE) {
    goto usage;
  }
  if (strcmp(argv[1], "join") == 0) {
    ret = igmp_joingroup(&ifaddr, &groupaddr);
  } else if (strcmp(argv[1], "leave") == 0) {
    ret = igmp_leavegroup(&ifaddr, &groupaddr);
  } else {
    goto usage;
  }

  return ret;
usage:
  os_shell_igmp_usage();
  return -1;
}

static s32_t
igmp_membership_ctrl(const char **argv)
{
  s32_t ret;
  struct netif *netif_p = NULL;
  ip4_addr_t groupaddr = {0};

  groupaddr.addr = inet_addr(argv[2]);
  if ((groupaddr.addr == IPADDR_NONE) || !(ip4_addr_ismulticast(&groupaddr))) {
#ifdef LWIP_DEBUG_OPEN
    (void)uapi_at_printf("invalid groupaddr %s"CRLF, argv[2]); // 2 is cast addr
#else
    LWIP_PLATFORM_PRINT("invalid groupaddr %s"CRLF, argv[2]); // 2 is cast addr
#endif
    os_shell_igmp_usage();
    return -1;
  }
  netif_p = netif_find(argv[0]);
  if (netif_p == NULL) {
    ret = igmp_membership_addr_ctrl(groupaddr, argv);
  } else {
    ret = igmp_membership_netif_ctrl(netif_p, groupaddr, argv);
  }

  return ret;
}

u32_t
os_shell_igmp(int argc, const char **argv)
{
  s32_t ret = 0;

  if (tcpip_init_finish == 0) {
#ifdef LWIP_DEBUG_OPEN
    (void)uapi_at_printf("%s: tcpip_init have not been called"CRLF, __FUNCTION__);
#else
    LWIP_PLATFORM_PRINT("%s: tcpip_init have not been called"CRLF, __FUNCTION__);
#endif
    return OS_NOK;
  }
  if (argv == NULL) {
    return OS_NOK;
  }
  LOCK_TCPIP_CORE();
  if ((argc == 1) && (strcmp(argv[0], "show") == 0)) {
    igmp_netif_show();
  } else if (argc == 2) {
    ret = igmp_netif_control(argv);
  } else if (argc == 3) {
    ret = igmp_membership_ctrl(argv);
  } else {
    os_shell_igmp_usage();
  }
  UNLOCK_TCPIP_CORE();
#ifdef LWIP_DEBUG_OPEN
  (void)uapi_at_printf("igmp ret %d"CRLF, ret);
#else
  LWIP_PLATFORM_PRINT("igmp ret %d"CRLF, ret);
#endif
  return OS_OK;
}

u32_t
at_os_shell_igmp(int argc, const char **argv)
{
  u32_t ret = os_shell_igmp(argc, argv);

  return ret;
}

#endif /* LWIP_IPV4 && LWIP_IGMP */

#ifdef LWIP_DEBUG_TCPSERVER

#define MAX_SIZE 1024
void
tcp_access(int sockfd)
{
  size_t n, i;
  ssize_t ret;
  char msg[MAX_SIZE] = {0};
  while (1) {
    LWIP_PLATFORM_PRINT("waiting for recv"CRLF);
    (void)memset_s(msg, MAX_SIZE, 0, MAX_SIZE);
    ret = recv(sockfd, msg, MAX_SIZE - 1, 0);
    if (ret < 0) {
      LWIP_PLATFORM_PRINT("recv failed errno = %d "CRLF, errno);
      (void)closesocket(sockfd);
      return;
    } else if (ret == 0) {
      (void)closesocket(sockfd);
      LWIP_PLATFORM_PRINT("client disconnect."CRLF);
      return;
    }

    n = strlen(msg);
    for (i = 0; i < n; ++i) {
      if ((msg[i] >= 'a') && (msg[i] <= 'z')) {
        msg[i] = (char)(msg[i] + ('A' - 'a'));
      } else if ((msg[i] >= 'A') && (msg[i] <= 'Z')) {
        msg[i] = (char)(msg[i] + ('a' - 'A'));
      }
    }

    if (send(sockfd, msg, n, 0) < 0) {
      LWIP_PLATFORM_PRINT("send failed!"CRLF);
      continue;
    }
  }
}

u32_t
os_tcpserver(int argc, const char **argv)
{
  uint16_t port;
  int sockfd = -1;
  int ret;
  struct sockaddr_in seraddr;
  struct sockaddr_in cliaddr;
  u32_t cliaddr_size = (u32_t)sizeof(cliaddr);
  int reuse, i_port_val;

  if (tcpip_init_finish == 0) {
    LWIP_PLATFORM_PRINT("tcpip_init have not been called"CRLF);
    return OS_NOK;
  }

  if (argc < 1) {
    LWIP_PLATFORM_PRINT(CRLF"Usage: tcpserver <port>"CRLF);
    return OS_NOK;
  }

  i_port_val = atoi(argv[0]);
  /* Port 0 not supported , negative values not supported , max port limit is 65535 */
  if (i_port_val <= 0 || i_port_val > 65535) {
    LWIP_PLATFORM_PRINT(CRLF"Usage: Invalid port"CRLF);
    return OS_NOK;
  }

  port = (uint16_t)i_port_val;

  /*
   * removed the print of argv[1] as its accessing argv[1] without verifying argc and argv[1] not used anywhere else
   */
  LWIP_PLATFORM_PRINT("argv[0]:%s, argc:%d, port:%d"CRLF, argv[0], argc, port);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  reuse = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                 (const char *)&reuse, sizeof(reuse)) != 0) {
    LWIP_PLATFORM_PRINT("set SO_REUSEADDR failed errno = %d"CRLF, errno);
    (void)closesocket(sockfd);
    return OS_NOK;
  }

  (void)memset_s(&seraddr, sizeof(seraddr), 0, sizeof(seraddr));
  seraddr.sin_family = AF_INET;
  seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  seraddr.sin_port = htons(port);

  ret = bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
  if (ret < 0) {
    LWIP_PLATFORM_PRINT("bind ip and port failed errno = %d", errno);
    (void)closesocket(sockfd);
    return OS_NOK;
  }

  ret = listen(sockfd, 5); /* 5 : max tcp listen backlog queue */
  if (ret < 0) {
    LWIP_PLATFORM_PRINT("listen failed errno = %d"CRLF, errno);
    (void)closesocket(sockfd);
    return OS_NOK;
  }
  while (1) {
    LWIP_PLATFORM_PRINT("waiting for accept"CRLF);
    (void)memset_s(&cliaddr, sizeof(struct sockaddr_in), 0, sizeof(struct sockaddr_in));
    ret = (int)accept(sockfd, (struct sockaddr *)&cliaddr, &cliaddr_size);
    if (ret < 0) {
      LWIP_PLATFORM_PRINT("Accept failed errno = %d"CRLF, errno);
      (void)closesocket(sockfd);
      break;
    }
    tcp_access(ret);
  }
  return OS_NOK;            // Hits Only If Accept Fails
}
#endif /* LWIP_DEBUG_TCPSERVER */

#ifdef LWIP_DEBUG_UDPSERVER
u32_t udpserver(int argc, const char **argv)
{
  int sockfd = -1;
  int ret, i_port_val, fromlen;
  struct sockaddr_in seraddr;
  struct sockaddr_in cliaddr;
  size_t n, i;

  char msg[MAX_SIZE];
  uint16_t port;
  if (argv == NULL) {
    return OS_NOK;
  }
  if (argc < 1) {
    LWIP_PLATFORM_PRINT(CRLF"Usage: udpserver <port>"CRLF);
    return OS_NOK;
  }

  i_port_val = atoi(argv[0]);
  /* Port 0 not supported , negative values not supported , max port limit is 65535 */
  if ((i_port_val <= 0) || (i_port_val > 65535)) {
    LWIP_PLATFORM_PRINT(CRLF"Usage: Invalid Port"CRLF);
    return OS_NOK;
  }

  port = (uint16_t)i_port_val;

  LWIP_PLATFORM_PRINT("port:%d"CRLF, port);

  sockfd = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  (void)memset_s(&seraddr, sizeof(seraddr), 0, sizeof(seraddr));
  (void)memset_s(&cliaddr, sizeof(cliaddr), 0, sizeof(cliaddr));
  seraddr.sin_family = AF_INET;
  seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  seraddr.sin_port = htons(port);
  ret = lwip_bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
  if (ret < 0) {
    LWIP_PLATFORM_PRINT("bind ip and port failed:%d"CRLF, errno);
    (void)closesocket(sockfd);
    return OS_NOK;
  }

  fromlen = (int)sizeof(cliaddr);
  while (1) {
    ret = recvfrom(sockfd, msg, MAX_SIZE - 1, 0, (struct sockaddr *)&cliaddr, (socklen_t *)&fromlen);
    if (ret < 0) {
      if ((errno == EAGAIN) || (errno == EINTR)) {
        continue;
      } else {
        break;
      }
    }
    n = strlen(msg);
    for (i = 0; i < n; ++i) {
      if (msg[i] >= 'a' && msg[i] <= 'z') {
        msg[i] = (char)(msg[i] + 'A' - 'a');
      } else if (msg[i] >= 'A' && msg[i] <= 'Z') {
        msg[i] = (char)(msg[i] + 'a' - 'A');
      }
    }
    ret = sendto(sockfd, msg, n + 1, 0, (struct sockaddr *)&cliaddr, (socklen_t)fromlen);
    if ((ret <= 0) && (errno == EPIPE)) {
      break;
    }
  }

  (void)closesocket(sockfd);
  return OS_NOK;
}
#endif /* LWIP_DEBUG_UDPSERVER */

#if LWIP_DHCPS
static void
dhcps_info_show(struct netif *netif)
{
  struct dhcps *dhcps = NULL;
  struct dyn_lease_addr *lease = NULL;
  u8_t i, j;
  int k = 0;
  ip4_addr_t addr;
  char buf[IP4ADDR_STRLEN_MAX];

  /* netif is ensured not to be NULL by the caller */
  LWIP_ASSERT("netif is NULL!", netif != NULL);

  dhcps = netif->dhcps;

  if ((dhcps == NULL) || (dhcps->pcb == NULL)) {
#ifdef CUSTOM_AT_COMMAND
    (void)uapi_at_printf("dhcps of %s%hhu not start\r\n", netif->name, netif->num);
#else
    PRINT_ERRCODE(API_SHELL_ERRCODE_DEV_NOT_READY);
#endif
    return;
  }
  addr.addr = lwip_htonl(dhcps->start_addr.addr);
  (void)ip4addr_ntoa_r(&addr, buf, IP4ADDR_STRLEN_MAX);
#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("start ip : %s"CRLF, buf);
#else
  LWIP_PLATFORM_PRINT("start ip : %s"CRLF, buf);
#endif
  addr.addr = lwip_htonl(dhcps->end_addr.addr);
  (void)ip4addr_ntoa_r(&addr, buf, IP4ADDR_STRLEN_MAX);
#ifdef CUSTOM_AT_COMMAND
  (void)uapi_at_printf("end ip : %s"CRLF, buf);
  (void)uapi_at_printf("lease num %hhu"CRLF, dhcps->lease_num);
  (void)uapi_at_printf("sys_now %u"CRLF, sys_now());
#else
  LWIP_PLATFORM_PRINT("end ip : %s"CRLF, buf);
  LWIP_PLATFORM_PRINT("lease num %hhu"CRLF, dhcps->lease_num);
  LWIP_PLATFORM_PRINT("sys_now %u"CRLF, sys_now());
#endif

  for (i = 0; i < dhcps->lease_num; i++) {
    if (dhcps->leasearr[i].flags != 0) {
      lease = &(dhcps->leasearr[i]);
#ifdef CUSTOM_AT_COMMAND
      (void)uapi_at_printf("%d : ", k++);
#else
      LWIP_PLATFORM_PRINT("%d : ", k++);
#endif
      for (j = 0; j < NETIF_MAX_HWADDR_LEN; j++) {
#ifdef CUSTOM_AT_COMMAND
        (void)uapi_at_printf("%02x", lease->cli_hwaddr[j]);
#else
        LWIP_PLATFORM_PRINT("%02x", lease->cli_hwaddr[j]);
#endif
      }
      addr.addr = lwip_htonl(lease->cli_addr.addr);
      (void)ip4addr_ntoa_r(&addr, buf, IP4ADDR_STRLEN_MAX);
#ifdef CUSTOM_AT_COMMAND
      (void)uapi_at_printf(" %s leasetime = %u(%u), proposed_leasetime = %u"CRLF, buf, lease->leasetime,
                         (lease->leasetime - sys_now()) / US_PER_MSECOND, lease->proposed_leasetime);
#else
      LWIP_PLATFORM_PRINT(" %s leasetime = %u(%u), proposed_leasetime = %u"CRLF, buf, lease->leasetime,
                          (lease->leasetime - sys_now()) / US_PER_MSECOND, lease->proposed_leasetime);
#endif
    }
  }
}

u32_t
os_shell_dhcps(int argc, const char **argv)
{
  int ret = 0;
  struct netif *netif_p = NULL;

  if (tcpip_init_finish == 0) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_TCPIP_UNINTED);
    return OS_NOK;
  }

  if ((argc != 2) || (argv == NULL)) { /* 2 : totol argc num */
    goto usage;
  }

  netif_p = netif_find(argv[0]);
  if (netif_p == NULL) {
    PRINT_ERRCODE(API_SHELL_ERRCODE_DEV_NOT_FOUND);
    goto usage;
  }
#ifdef CUSTOM_AT_COMMAND
  if (strcmp(argv[1], "1") == 0) {
    ret = netifapi_dhcps_start(netif_p, NULL, 0);
  } else if (strcmp(argv[1], "0") == 0) {
    ret = netifapi_dhcps_stop(netif_p);
  } else {
    goto usage;
  }
#else
  if (strcmp(argv[1], "start") == 0) {
    ret = netifapi_dhcps_start(netif_p, NULL, 0);
  } else if (strcmp(argv[1], "stop") == 0) {
    ret = netifapi_dhcps_stop(netif_p);
  } else if (strcmp(argv[1], "show") == 0) {
    ret = netifapi_netif_common(netif_p, dhcps_info_show, NULL);
  } else {
    goto usage;
  }
#endif
  if (ret == OS_OK) {
    LWIP_PLATFORM_PRINT("OK"CRLF);
  }

  return (ret == 0) ? OS_OK : OS_NOK;
usage:
#ifndef CUSTOM_AT_COMMAND
  LWIP_PLATFORM_PRINT("dhcps\n\tifname {start | stop | show}"CRLF);
#endif
  return OS_NOK;
}
#endif /* LWIP_DHCPS */


void ifup_internal(void *arg);
void ifdown_internal(void *arg);

struct if_cmd_data {
  char *if_name;
  err_t err;
  sys_sem_t cb_completed;
};

void ifup_internal(void *arg)
{
  struct netif *netif = NULL;
  struct if_cmd_data *ifcmd_data = NULL;

  ifcmd_data = (struct if_cmd_data *)arg;
  netif = netif_find(ifcmd_data->if_name);
  if (netif == NULL) {
    ifcmd_data->err = ERR_VAL;
  } else {
    (void)netif_set_up(netif);
    ifcmd_data->err = ERR_OK;
  }

  sys_sem_signal(&ifcmd_data->cb_completed);
  return;
}

void ifdown_internal(void *arg)
{
  struct netif *netif = NULL;
  struct if_cmd_data *ifcmd_data = NULL;

  ifcmd_data = (struct if_cmd_data *)arg;
  netif = netif_find(ifcmd_data->if_name);
  if (netif == NULL) {
    ifcmd_data->err = ERR_VAL;
  } else {
    (void)netif_set_down(netif);
    ifcmd_data->err = ERR_OK;
  }

  sys_sem_signal(&ifcmd_data->cb_completed);
  return;
}

#endif /* LWIP_ENABLE_BASIC_SHELL_CMD */
