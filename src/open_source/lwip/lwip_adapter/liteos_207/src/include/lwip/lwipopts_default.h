/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
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
 * Author: Simon Goldschmidt
 *
 */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifndef LWIP_PLATFORM_PRINT
#define LWIP_PLATFORM_PRINT         PRINTK
#endif

/*
  To support the CMSIS interface, you need to enable this macro.
  By default, this macro is not supported.
*/
#ifndef LWIP_ALIOS_COMPAT
#define LWIP_ALIOS_COMPAT 0
#endif

#ifndef LWIP_CMSIS_SUPPORT
#define LWIP_CMSIS_SUPPORT 0
#endif

#ifndef LWIP_FREERTOS_COMPAT
#define LWIP_FREERTOS_COMPAT 0
#endif

#ifndef LWIP_GETHOSTBYNAME
#define LWIP_GETHOSTBYNAME 1
#endif

#ifndef LWIP_GETPEERNAME
#define LWIP_GETPEERNAME 1
#endif

#ifndef LWIP_GETSOCKNAME
#define LWIP_GETSOCKNAME 1
#endif

#ifndef LWIP_SMALL_SIZE
#define LWIP_SMALL_SIZE 1
#endif

/* only valid when LWIP_SMALL_SIZE is 1, mean small size with MESH function */
#ifndef LWIP_SMALL_SIZE_MESH
#define LWIP_SMALL_SIZE_MESH 1
#endif

#if LWIP_SMALL_SIZE

#if LWIP_SMALL_SIZE_MESH

#ifndef LWIP_L3_EVENT_MSG
#define LWIP_L3_EVENT_MSG 1
#endif

#ifndef LWIP_ENABLE_MESH_SHELL_CMD
#define LWIP_ENABLE_MESH_SHELL_CMD 1
#endif

#ifndef LWIP_RPL
#define LWIP_RPL 0
#endif

#ifndef LWIP_RIPPLE
#define LWIP_RIPPLE 0
#endif

#ifndef LWIP_MMBR
#define LWIP_MMBR 0
#endif

#ifndef LWIP_NA_PROXY
#define LWIP_NA_PROXY    1
#endif

#ifndef LWIP_RA_PREFIX_DYNAMIC
#define LWIP_RA_PREFIX_DYNAMIC    1
#endif

#ifndef LWIP_IPV6
#define LWIP_IPV6          1
#endif

#ifndef LWIP_IPV6_MLD
#define LWIP_IPV6_MLD 0
#endif

#ifndef LWIP_IPV6_MLD_QUERIER
#define LWIP_IPV6_MLD_QUERIER 0
#endif

#ifndef LWIP_MPL
#define LWIP_MPL 0
#endif

#ifndef LWIP_MPL_IPV4
#define LWIP_MPL_IPV4  0
#endif

#ifndef LWIP_MPL_IPV4_BCAST
#define LWIP_MPL_IPV4_BCAST 0
#endif

#ifndef LWIP_6LOWPAN
#define LWIP_6LOWPAN 0
#endif

#ifndef ETH_6LOWPAN
#define ETH_6LOWPAN 1
#endif

#ifndef LWIP_IPV6_AUTOCONFIG_DEFAULT
#define LWIP_IPV6_AUTOCONFIG_DEFAULT    1
#endif

#ifndef LWIP_DHCP_SUBSTITUTE
#define LWIP_DHCP_SUBSTITUTE 0
#endif

#ifndef LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS
#define LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS 1
#endif

#ifndef LWIP_NAT64
#define LWIP_NAT64    1
#endif

#ifndef LWIP_NAT64_MIN_SUBSTITUTE
#define LWIP_NAT64_MIN_SUBSTITUTE 0
#endif

#ifndef LWIP_DNS64
#define LWIP_DNS64    LWIP_IPV6
#endif

#ifndef LWIP_ND6_ROUTER
#define LWIP_ND6_ROUTER    1
#endif

#ifndef LWIP_ND6_DAG_PREFIX
#define LWIP_ND6_DAG_PREFIX    1
#endif

#ifndef LWIP_USE_L2_METRICS
#define LWIP_USE_L2_METRICS 1
#endif

#ifndef LWIP_SO_PRIORITY
#define LWIP_SO_PRIORITY 1
#endif

#ifndef LWIP_ENABLE_ROUTER
#define LWIP_ENABLE_ROUTER   1
#endif

/* maximum that an AP can associate with others */
#ifndef LWIP_AP_ASSOCIATE_NUM
#define LWIP_AP_ASSOCIATE_NUM 5
#endif

/* maximum of MBRs */
#ifndef LWIP_MULTI_MBR_NUM
#define LWIP_MULTI_MBR_NUM 3
#endif

/* number of nodes at Router side */
#ifndef LWIP_ROUTER_NBR_NUM
#define LWIP_ROUTER_NBR_NUM 11
#endif

#ifndef LWIP_ND6_NUM_DESTINATIONS
#define LWIP_ND6_NUM_DESTINATIONS \
  (LWIP_ND6_NUM_ROUTERS + LWIP_AP_ASSOCIATE_NUM + (LWIP_MULTI_MBR_NUM - 1) + LWIP_ROUTER_NBR_NUM)
#endif /* LWIP_ND6_NUM_DESTINATIONS */

#ifndef LWIP_ND6_NUM_NEIGHBORS
#define LWIP_ND6_NUM_NEIGHBORS LWIP_ND6_NUM_DESTINATIONS
#endif

#ifndef LWIP_ND6_DESTINATIONS_OLDTIME
#define LWIP_ND6_DESTINATIONS_OLDTIME 180
#endif

#else /* LWIP_SMALL_SIZE_MESH */
#undef LWIP_MMBR
#define LWIP_MMBR 0

#endif /* LWIP_SMALL_SIZE_MESH */

#ifndef LWIP_L3_EVENT_MSG
#define LWIP_L3_EVENT_MSG   0
#endif

#if defined (LOSCFG_CONFIG_MQTT)
#ifndef CONFIG_NFILE_DESCRIPTORS
#define CONFIG_NFILE_DESCRIPTORS 1
#endif
#endif

#ifndef LWIP_API_MESH
#define LWIP_API_MESH   0
#endif

#ifndef LWIP_API_RICH
#define LWIP_API_RICH   0
#endif

#ifndef LWIP_ICMP6_ERR_RT_LMT
#define LWIP_ICMP6_ERR_RT_LMT 0
#endif

#ifndef LWIP_DHCPS_AGENT_INFO
#define LWIP_DHCPS_AGENT_INFO   0
#endif

#ifndef LWIP_SOCK_OPT_ICMP6_FILTER
#define LWIP_SOCK_OPT_ICMP6_FILTER   0
#endif

#ifndef LWIP_IPV6_PER_PROTO_CHKSUM
#define LWIP_IPV6_PER_PROTO_CHKSUM 0
#endif

#ifndef LWIP_SOCK_OPT_IPV6_UNICAST_HOPS
#define LWIP_SOCK_OPT_IPV6_UNICAST_HOPS   0
#endif

#ifndef LWIP_SOCK_OPT_TCP_QUEUE_SEQ
#define LWIP_SOCK_OPT_TCP_QUEUE_SEQ   0
#endif

#ifndef LWIP_LINK_MCAST_FILTER
#define LWIP_LINK_MCAST_FILTER   0
#endif

#ifndef LWIP_IOCTL_IPV6DPCTD
#define LWIP_IOCTL_IPV6DPCTD   0
#endif

#ifndef LWIP_IOCTL_ROUTE
#define LWIP_IOCTL_ROUTE   0
#endif

#ifndef LWIP_IOCTL_IF
#define LWIP_IOCTL_IF   0
#endif

#ifndef LWIP_IPV6_DUP_DETECT_ATTEMPTS
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS   0
#endif

#ifndef LWIP_NETIFAPI_DHCPS_IP
#define LWIP_NETIFAPI_DHCPS_IP 0
#endif

#ifndef LWIP_NETIFAPI_GET_ADDR
#define LWIP_NETIFAPI_GET_ADDR 1
#endif

#ifndef LWIP_DROP_PKT_WHEN_NETIF_DOWN
#define LWIP_DROP_PKT_WHEN_NETIF_DOWN 1
#endif

#ifndef LWIP_NETIFAPI_LINK_CALLBACK
#define LWIP_NETIFAPI_LINK_CALLBACK 0
#endif

#ifndef LWIP_NETIFAPI_MTU
#define LWIP_NETIFAPI_MTU 0
#endif

#ifndef LWIP_NETIFAPI_IF_NUM
#define LWIP_NETIFAPI_IF_NUM 0
#endif

#ifndef LWIP_NETIFAPI_IF_INDEX
#define LWIP_NETIFAPI_IF_INDEX 0
#endif

#ifndef LWIP_NETIFAPI_IF_HW
#define LWIP_NETIFAPI_IF_HW 0
#endif

#ifndef LWIP_NETIFAPI_IP6_ADDR
#define LWIP_NETIFAPI_IP6_ADDR 0
#endif

#ifndef LWIP_NETIFAPI_MLD6
#define LWIP_NETIFAPI_MLD6 0
#endif

#ifndef LWIP_NETIFAPI_IF_GET_DEFAULT
#define LWIP_NETIFAPI_IF_GET_DEFAULT 0
#endif

#ifndef LWIP_IP_FILTER
#define LWIP_IP_FILTER        0
#endif

#ifndef LWIP_IPV6_FILTER
#define LWIP_IPV6_FILTER        0
#endif

#ifndef LWIP_NETIF_NBR_CACHE_API
#define LWIP_NETIF_NBR_CACHE_API 0
#endif

#if defined (CONFIG_NETIF_HOSTNAME)

#ifndef LWIP_NETIF_HOSTNAME
#define LWIP_NETIF_HOSTNAME    1
#endif

#ifndef LWIP_NETIF_GET_HOSTNAME
#define LWIP_NETIF_GET_HOSTNAME    0
#endif

#else

#ifndef LWIP_NETIF_HOSTNAME
#define LWIP_NETIF_HOSTNAME    0
#endif
#endif

#ifndef LWIP_IFADDRS
#define LWIP_IFADDRS                 0
#endif

#ifndef LWIP_ALLOW_SOCKET_CONFIG
#define LWIP_ALLOW_SOCKET_CONFIG 0
#endif

#if defined (CONFIG_DHCP_VENDOR_CLASS_IDENTIFIER)
#ifndef LWIP_DHCP_VENDOR_CLASS_IDENTIFIER
#define LWIP_DHCP_VENDOR_CLASS_IDENTIFIER    1
#endif

#ifndef LWIP_DHCP_GET_VENDOR_CLASS_IDENTIFIER
#define LWIP_DHCP_GET_VENDOR_CLASS_IDENTIFIER    0
#endif

#else

#ifndef LWIP_DHCP_VENDOR_CLASS_IDENTIFIER
#define LWIP_DHCP_VENDOR_CLASS_IDENTIFIER    0
#endif
#endif

#ifndef LWIP_IPV6_DHCP6
#define LWIP_IPV6_DHCP6                 0
#endif

#ifndef LWIP_TCP_TLP_SUPPORT
#define LWIP_TCP_TLP_SUPPORT    0
#endif

#ifndef LWIP_WND_SCALE
#define LWIP_WND_SCALE    1
#endif

#ifndef TCP_RCV_SCALE
#define TCP_RCV_SCALE     1
#endif

#ifndef LWIP_SNTP
#define LWIP_SNTP               0
#endif

#ifndef LWIP_SOCK_FILTER
#define LWIP_SOCK_FILTER        0
#endif

#ifndef DRIVER_STATUS_CHECK
#define DRIVER_STATUS_CHECK     1
#endif

#ifndef IP6_DEBUG
#define IP6_DEBUG               0
#endif

#ifndef LWIP_DEBUG_TIMERNAMES
#define LWIP_DEBUG_TIMERNAMES   0
#endif

#ifndef ETHARP_SUPPORT_STATIC_ENTRIES
#define ETHARP_SUPPORT_STATIC_ENTRIES 1
#endif

#ifndef LWIP_DHCPS
#define LWIP_DHCPS              1
#endif

#ifndef LWIP_STATS
#define LWIP_STATS              0
#endif

#ifndef ICMP6_STATS
#define ICMP6_STATS             0
#endif

#ifndef LWIP_TCP_INFO
#define LWIP_TCP_INFO           0
#endif

#ifndef LWIP_NETIF_ETHTOOL
#define LWIP_NETIF_ETHTOOL      0
#endif

#ifndef LWIP_ENABLE_LOS_SHELL_CMD
#define LWIP_ENABLE_LOS_SHELL_CMD 0
#endif

#ifndef LWIP_ENABLE_BASIC_SHELL_CMD
#define LWIP_ENABLE_BASIC_SHELL_CMD 1
#endif

#ifndef LWIP_ENABLE_MESH_SHELL_CMD
#define LWIP_ENABLE_MESH_SHELL_CMD 0
#endif

#ifndef LWIP_RIPPLE
#define LWIP_RIPPLE 0
#endif

#ifndef LWIP_RA_PREFIX_DYNAMIC
#define LWIP_RA_PREFIX_DYNAMIC    0
#endif

#ifndef LWIP_DHCP_COAP_RELAY
#define LWIP_DHCP_COAP_RELAY    0
#endif

#ifndef LWIP_MPL_IPV4_BCAST
#define LWIP_MPL_IPV4_BCAST 0
#endif

#ifndef LWIP_6LOWPAN
#define LWIP_6LOWPAN 0
#endif

#ifndef ETH_6LOWPAN
#define ETH_6LOWPAN 0
#endif

#ifndef LWIP_DHCP_SUBSTITUTE
#define LWIP_DHCP_SUBSTITUTE 0
#endif

#ifndef LWIP_DHCPS_MAX_LEASE
#define LWIP_DHCPS_MAX_LEASE 6
#endif

#ifndef LWIP_NAT64
#define LWIP_NAT64    0
#endif

#ifndef LWIP_DNS64
#define LWIP_DNS64    0
#endif

#ifndef LWIP_ND6_ROUTER
#define LWIP_ND6_ROUTER    0
#endif

#ifndef LWIP_RPL_RS_DAO
#define LWIP_RPL_RS_DAO    0
#endif

#ifndef LWIP_SO_BINDTODEVICE
#define LWIP_SO_BINDTODEVICE         1
#endif

#ifndef LWIP_SO_DONTROUTE
#define LWIP_SO_DONTROUTE         1
#endif

#ifndef LWIP_NUM_SOCKETS_MAX
#define LWIP_NUM_SOCKETS_MAX  9
#endif

#ifndef DEFAULT_LWIP_NUM_SOCKETS
#define DEFAULT_LWIP_NUM_SOCKETS        12
#endif /* DEFAULT_LWIP_NUM_SOCKETS */

#ifndef MEMP_NUM_PBUF
#define MEMP_NUM_PBUF           8
#endif

#ifndef MEMP_NUM_RAW_PCB
#define MEMP_NUM_RAW_PCB                1
#endif

#ifndef MEMP_NUM_TCPIP_MSG_API
#define MEMP_NUM_TCPIP_MSG_API          16
#endif

#ifndef MEMP_NUM_TCPIP_MSG_INPKT
#define MEMP_NUM_TCPIP_MSG_INPKT        32
#endif

#ifndef MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN        LWIP_CONFIG_NUM_SOCKETS
#endif

#ifndef MEMP_NUM_NETBUF
#if LWIP_MMBR
#define MEMP_NUM_NETBUF         24
#else
#ifdef WIFI_TCM_OPTIMIZE
#define MEMP_NUM_NETBUF         132
#else
#define MEMP_NUM_NETBUF         24
#endif
#endif
#endif

#ifndef MEMP_NUM_ARP_QUEUE
#define MEMP_NUM_ARP_QUEUE      12
#endif

#ifndef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB        (7 + LWIP_MPL)
#endif

#ifndef MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB        6
#endif

#ifndef MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN 9
#endif

#ifndef MEMP_NUM_TCP_SEG
#ifdef _PRE_WLAN_FEATURE_WS53
#define MEMP_NUM_TCP_SEG        32
#else
#define MEMP_NUM_TCP_SEG        64
#endif
#endif

#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE          1
#endif

#if defined (CONFIG_LWIP_FOR_WIFI_SIGMA)
#ifndef MEMP_NUM_REASSDATA
#define MEMP_NUM_REASSDATA  64
#endif
#if !defined IP_REASS_MAXAGE
#define IP_REASS_MAXAGE      1
#endif
#else
#ifndef MEMP_NUM_REASSDATA
#define MEMP_NUM_REASSDATA  5
#endif
#endif

#ifndef TCP_SND_BUF
#if defined(WIFI_TCM_OPTIMIZE) || defined(_PRE_WLAN_FEATURE_WS53)
#define TCP_SND_BUF        65535
#else
#define TCP_SND_BUF        (65535 / 3)
#endif
#endif

#ifndef TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN       ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
#endif

#ifndef TCP_WND
#ifdef _PRE_WLAN_FEATURE_WS53
#define TCP_WND            (TCP_SND_BUF / 3)
#else
#define TCP_WND            (98304)
#endif
#endif

#ifndef LWIP_IGMP
#define LWIP_IGMP                       1
#endif

#ifndef LWIP_DNS_REVERSE
#define LWIP_DNS_REVERSE 0
#endif

#ifndef PF_PKT_SUPPORT
#define PF_PKT_SUPPORT 0
#endif

#ifndef LWIP_SO_RCVBUF
#define LWIP_SO_RCVBUF                  0
#endif

#ifndef LWIP_IPV6_MLD
#define LWIP_IPV6_MLD 0
#endif

#ifndef LWIP_IPV6_MLD_QUERIER
#define LWIP_IPV6_MLD_QUERIER 0
#endif

#ifndef LWIP_ND6_NUM_NEIGHBORS
#define LWIP_ND6_NUM_NEIGHBORS 5
#endif

#ifndef LWIP_SO_PRIORITY
#define LWIP_SO_PRIORITY 0
#endif

#ifndef LWIP_NETIF_PROMISC
#define LWIP_NETIF_PROMISC 0
#endif

#ifndef LWIP_NETBUF_RECVINFO
#define LWIP_NETBUF_RECVINFO 0
#endif

#ifndef LWIP_ND6_STATIC_PREFIX
#define LWIP_ND6_STATIC_PREFIX 0
#endif

#ifndef LWIP_USE_L2_METRICS
#define LWIP_USE_L2_METRICS 0
#endif

#ifndef MEMP_NUM_NETDB
#define MEMP_NUM_NETDB                  1
#endif

#ifndef DNS_TABLE_SIZE
#define DNS_TABLE_SIZE                  2
#endif

#ifndef DNS_MAX_IPADDR
#define DNS_MAX_IPADDR                1
#endif

#ifndef LWIP_DNS_SECURE
#define LWIP_DNS_SECURE (LWIP_DNS_SECURE_RAND_XID | LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)
#endif

#ifndef LWIP_ENABLE_ROUTER
#define LWIP_ENABLE_ROUTER   0
#endif

#ifndef LWIP_ND6_NUM_ROUTERS
#define LWIP_ND6_NUM_ROUTERS            1
#endif

#endif /* LWIP_SMALL_SIZE */

#ifndef RPL_CONF_SWITCH_MBR_BY_AUTOLINK
#define RPL_CONF_SWITCH_MBR_BY_AUTOLINK LWIP_MMBR
#endif

#ifndef RPL_CONF_MMBR_MNID
#define RPL_CONF_MMBR_MNID LWIP_MMBR
#endif

#ifndef LWIP_IP6IN4
#define LWIP_IP6IN4 LWIP_MMBR
#endif

#ifndef LWIP_DHCP_SUBSTITUTE_MMBR
#if defined(LWIP_DHCP_SUBSTITUTE) && LWIP_DHCP_SUBSTITUTE && LWIP_MMBR
#define LWIP_DHCP_SUBSTITUTE_MMBR 1
#else
#define LWIP_DHCP_SUBSTITUTE_MMBR 0
#endif
#endif /* LWIP_DHCP_SUBSTITUTE_MMBR */

#ifndef LWIP_IGMP_REPORT_TIMES
#define LWIP_IGMP_REPORT_TIMES 5
#endif

#ifndef LWIP_LIBCOAP
#define LWIP_LIBCOAP 0
#endif

#ifndef LWIP_FCNTL
#define LWIP_FCNTL 0
#endif

#ifndef LWIP_API_MESH
#define LWIP_API_MESH   1
#endif

#ifndef LWIP_L2_NETDEV_STATUS_CALLBACK
#define LWIP_L2_NETDEV_STATUS_CALLBACK 1
#endif

#ifndef LWIP_NETIF_STATUS_CALLBACK
#define LWIP_NETIF_STATUS_CALLBACK 1
#endif

#ifndef LWIP_TCP_TLP_SUPPORT
#define LWIP_TCP_TLP_SUPPORT    0
#endif

#ifndef LWIP_WND_SCALE
#define LWIP_WND_SCALE 0
#endif

#ifndef LWIP_TCP_INFO
#define LWIP_TCP_INFO           0
#endif

#ifndef LWIP_API_RICH
#define LWIP_API_RICH   1
#endif

#ifndef LWIP_DHCPS_AGENT_INFO
#define LWIP_DHCPS_AGENT_INFO   1
#endif

#ifndef LWIP_SOCK_OPT_ICMP6_FILTER
#define LWIP_SOCK_OPT_ICMP6_FILTER   1
#endif

#ifndef LWIP_IPV6_PER_PROTO_CHKSUM
#define LWIP_IPV6_PER_PROTO_CHKSUM 1
#endif

#ifndef LWIP_SOCK_OPT_IPV6_UNICAST_HOPS
#define LWIP_SOCK_OPT_IPV6_UNICAST_HOPS   1
#endif

#ifndef LWIP_SOCK_OPT_TCP_QUEUE_SEQ
#define LWIP_SOCK_OPT_TCP_QUEUE_SEQ   1
#endif

#ifndef LWIP_LINK_MCAST_FILTER
#define LWIP_LINK_MCAST_FILTER   1
#endif

#ifndef LWIP_IOCTL_IPV6DPCTD
#define LWIP_IOCTL_IPV6DPCTD   1
#endif

#ifndef LWIP_IOCTL_ROUTE
#define LWIP_IOCTL_ROUTE   1
#endif

#ifndef LWIP_IOCTL_IF
#define LWIP_IOCTL_IF   1
#endif

#ifndef LWIP_IPV6_DUP_DETECT_ATTEMPTS
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS   1
#endif

#ifndef LWIP_NETIFAPI_DHCPS_IP
#define LWIP_NETIFAPI_DHCPS_IP 1
#endif

#ifndef LWIP_NETIFAPI_GET_ADDR
#define LWIP_NETIFAPI_GET_ADDR 1
#endif

#ifndef LWIP_NETIFAPI_LINK_CALLBACK
#define LWIP_NETIFAPI_LINK_CALLBACK 1
#endif

#ifndef LWIP_NETIFAPI_MTU
#define LWIP_NETIFAPI_MTU 1
#endif

#ifndef LWIP_NETIFAPI_IF_NUM
#define LWIP_NETIFAPI_IF_NUM 1
#endif

#ifndef LWIP_NETIFAPI_IF_INDEX
#define LWIP_NETIFAPI_IF_INDEX 1
#endif

#ifndef LWIP_NETIFAPI_IF_HW
#define LWIP_NETIFAPI_IF_HW 1
#endif

#ifndef LWIP_NETIFAPI_IP6_ADDR
#define LWIP_NETIFAPI_IP6_ADDR 1
#endif

#ifndef LWIP_NETIFAPI_MLD6
#define LWIP_NETIFAPI_MLD6 1
#endif

#ifndef LWIP_NETIFAPI_IF_GET_DEFAULT
#define LWIP_NETIFAPI_IF_GET_DEFAULT 1
#endif

#ifndef LWIP_IFADDRS
#define LWIP_IFADDRS                 1
#endif

#ifndef LWIP_ENABLE_BASIC_SHELL_CMD
#define LWIP_ENABLE_BASIC_SHELL_CMD 1
#endif

#ifndef LWIP_ENABLE_MESH_SHELL_CMD
#define LWIP_ENABLE_MESH_SHELL_CMD 1
#endif

#ifndef LWIP_NETIF_ETHTOOL
#define LWIP_NETIF_ETHTOOL      0
#endif

#ifndef LWIP_RPL_CTX
#define LWIP_RPL_CTX  0
#endif

#ifndef LWIP_TFTP
#define LWIP_TFTP               0
#endif

#ifndef IP_REASSEMBLY
#define IP_REASSEMBLY          1
#endif

#ifndef LWIP_RPL
#define LWIP_RPL 0
#endif

#ifndef LWIP_RIPPLE
#define LWIP_RIPPLE 1
#endif

#ifndef LWIP_LITEOS_TASK
#define LWIP_LITEOS_TASK 1
#endif

#ifndef LWIP_DHCP_COAP_RELAY
#define LWIP_DHCP_COAP_RELAY    0
#endif

#ifndef LWIP_NA_PROXY
#define LWIP_NA_PROXY    1
#endif

#ifndef LWIP_RA_PREFIX_DYNAMIC
#define LWIP_RA_PREFIX_DYNAMIC    1
#endif

#if LWIP_RIPPLE
#if !defined(LWIP_MPL)
#define LWIP_MPL  1
#endif
#else
#if !defined(LWIP_MPL)
#define LWIP_MPL  0
#endif
#endif

#if LWIP_MPL
#if !defined(LWIP_MPL_IPV4)
#define LWIP_MPL_IPV4  1
#endif
#else
#if !defined(LWIP_MPL_IPV4)
#define LWIP_MPL_IPV4  0
#endif
#endif

#ifndef LWIP_MPL_IPV4_BCAST
#define LWIP_MPL_IPV4_BCAST 1
#endif

#ifndef LWIP_6LOWPAN
#define LWIP_6LOWPAN 0
#endif

#ifndef ETH_6LOWPAN
#define ETH_6LOWPAN 1
#endif

#ifndef LWIP_IPV6_AUTOCONFIG_DEFAULT
#define LWIP_IPV6_AUTOCONFIG_DEFAULT    1
#endif
/**
 * LWIP_DHCP_SUBSTITUTE==1: Enables MBR Querying IPv4 address for non-mesh IPv6 STA.
 */
#ifndef LWIP_DHCP_SUBSTITUTE
#define LWIP_DHCP_SUBSTITUTE 1
#endif

#ifndef LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS
#define LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS 1
#endif

#ifndef MEM_SIZE
#define MEM_SIZE               (512*1024)
#endif

#ifndef LWIP_PLC
#define LWIP_PLC    0
#endif

#ifndef LWIP_IEEE802154
#define LWIP_IEEE802154 0
#endif

#ifndef LWIP_NAT64
#define LWIP_NAT64    1
#endif

#ifndef LWIP_DNS64
#define LWIP_DNS64    1
#endif

#ifndef LWIP_ND6_ROUTER
#define LWIP_ND6_ROUTER    1
#endif

#ifndef LWIP_ND6_DAG_PREFIX
#define LWIP_ND6_DAG_PREFIX    1
#endif

#ifndef LWIP_ND6_RDNSS_MAX_DNS_SERVERS
#define LWIP_ND6_RDNSS_MAX_DNS_SERVERS  2
#endif

#ifndef LWIP_COMPAT_SOCKETS
#define LWIP_COMPAT_SOCKETS 2
#endif

#define LWIP_PLATFORM_BYTESWAP 0


#ifndef LWIP_GETHOSTBYNAME
#define LWIP_GETHOSTBYNAME 1
#endif

#ifndef LWIP_GETPEERNAME
#define LWIP_GETPEERNAME 1
#endif

#ifndef LWIP_GETSOCKNAME
#define LWIP_GETSOCKNAME 1
#endif

#ifndef LWIP_SMALL_SIZE
#define LWIP_SMALL_SIZE 1
#endif
#ifndef LWIP_L2_NETDEV_STATUS_CALLBACK
#define LWIP_L2_NETDEV_STATUS_CALLBACK 1
#endif

#ifndef LWIP_RIPPLE
#define LWIP_RIPPLE 0
#endif

#ifndef LWIP_NAT64
#define LWIP_NAT64    0
#endif

#ifndef LWIP_ND6_ROUTER
#define LWIP_ND6_ROUTER    0
#endif

#ifndef LWIP_ND6_DAG_PREFIX
#define LWIP_ND6_DAG_PREFIX    1
#endif

#ifndef LWIP_USE_L2_METRICS
#define LWIP_USE_L2_METRICS 1
#endif

#ifndef LWIP_ENABLE_ROUTER
#define LWIP_ENABLE_ROUTER   0
#endif

#ifndef LWIP_RPL_RS_DAO
#define LWIP_RPL_RS_DAO    0
#endif

#ifndef LWIP_API_MESH
#define LWIP_API_MESH   1
#endif

#ifndef LWIP_DHCP_SUBSTITUTE
#define LWIP_DHCP_SUBSTITUTE 1
#endif

#ifndef LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS
#define LWIP_DHCP_LIMIT_CONCURRENT_REQUESTS 0
#endif

#ifndef LWIP_ENABLE_BASIC_SHELL_CMD
#define LWIP_ENABLE_BASIC_SHELL_CMD 1
#endif

#ifndef LWIP_NA_PROXY
#define LWIP_NA_PROXY    1
#endif

#ifndef LWIP_LITEOS_COMPAT
#define LWIP_LITEOS_COMPAT  1
#endif

/* Set to 1 if target platform is linux */
#ifndef LWIP_LINUX_COMPAT
#define LWIP_LINUX_COMPAT   0
#endif

/* Set to 1 if target platform provides non-volatile ram APIs via <xx_nv.h> */
#ifndef LWIP_PLATFORM_PROVIDE_NV_API
#define LWIP_PLATFORM_PROVIDE_NV_API 0
#endif

#ifndef LWIP_COMPAT_SOCKETS
#define LWIP_COMPAT_SOCKETS 2
#define LWIP_POSIX_SOCKETS_IO_NAMES 0
#endif


#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS 1


#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

#ifndef LWIP_CHKSUM_ALGORITHM
#define LWIP_CHKSUM_ALGORITHM       3
#endif

#ifndef LWIP_NETIF_IFINDEX_MAX
#define LWIP_NETIF_IFINDEX_MAX 0xFE
#endif

#ifndef LWIP_NEIGHBOR_CLEANUP_WHILE_NETIF_LINK_DOWN
#define LWIP_NEIGHBOR_CLEANUP_WHILE_NETIF_LINK_DOWN   LWIP_RIPPLE
#endif

#ifndef LWIP_PROVIDE_ERRNO
#define LWIP_PROVIDE_ERRNO 1
#endif

#ifndef LWIP_COMPAT_MUTEX_ALLOWED
#define LWIP_COMPAT_MUTEX_ALLOWED        1
#endif

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#ifndef LWIP_SO_SNDTIMEO
#define LWIP_SO_SNDTIMEO                1
#endif

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#ifndef LWIP_SO_RCVTIMEO
#define LWIP_SO_RCVTIMEO                1
#endif

/**
 * LWIP_SO_BINDTODEVICE==1: Enable bind sockets/netconns to specific netif
 */
#ifndef LWIP_SO_BINDTODEVICE
#define LWIP_SO_BINDTODEVICE         1
#endif

/**
 * LWIP_SO_DONTROUTE==1: Enable don't use universal route entry
 */
#ifndef LWIP_SO_DONTROUTE
#define LWIP_SO_DONTROUTE         1
#endif

#ifndef LWIP_SACK
#define LWIP_SACK           1
#endif

#if LWIP_SACK
#ifndef LWIP_SACK_PERF_OPT
#define LWIP_SACK_PERF_OPT 1
#endif
#else
#define LWIP_SACK_PERF_OPT 0
#endif

#ifndef LWIP_SACK_CWND_OPT
#define LWIP_SACK_CWND_OPT  LWIP_SACK_PERF_OPT
#endif

#ifndef LWIP_SACK_DATA_SEG_PIGGYBACK
#define LWIP_SACK_DATA_SEG_PIGGYBACK       (LWIP_SACK & 1)
#endif


/*
@page RFC-5827
@par  RFC-5827 Appendix A.  Compliance Information
@par Compliant Sections
<Appendix A. Research Issues in Adjusting the Duplicate ACK Threshold>
@par Behavior Description
<Behavior:  Reducing the number of duplicate acks for fast retransmit has drawback during minor \n
  network reordering.\n
  Our node do not realize the mitigations proposed. So MITIGATION A.1, MITIGATION A.2 and \n
  MITIGATION A.3 are not implemented.>
*/
#ifndef LWIP_TCP_ER_SUPPORT
#define LWIP_TCP_ER_SUPPORT           1
#endif

#if LWIP_SACK
#ifndef LWIP_TCP_TLP_SUPPORT
#define LWIP_TCP_TLP_SUPPORT 0
#endif
#else
#define LWIP_TCP_TLP_SUPPORT 0
#endif

#if LWIP_SACK
#ifndef LWIP_FACK_THRESHOLD_BASED_FR
#define LWIP_FACK_THRESHOLD_BASED_FR 1
#endif
#else
#define LWIP_FACK_THRESHOLD_BASED_FR 0
#endif

#ifndef LWIP_WND_SCALE
#define LWIP_WND_SCALE 0
#endif
#ifndef TCP_OOSEQ_MAX_PBUFS
#define TCP_OOSEQ_MAX_PBUFS     8
#endif


/**
 * NO_SYS==1: Provides very minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                  0

#ifndef LWIP_NOASSERT
#define LWIP_NOASSERT
#endif

/* ---------- DEBUG options --------- */
/* LWIP_DEBUG_INFO is opened by default. */
#ifndef LWIP_DEBUG_INFO
#define LWIP_DEBUG_INFO
#endif

/**
 * LWIP_DBG_TYPES_ON: A mask that can be used to globally enable/disable
 * debug messages of certain types.
 */
#ifndef LWIP_DBG_TYPES_ON
#define LWIP_DBG_TYPES_ON               LWIP_DBG_OFF
#endif

#ifndef IP_DEBUG
#define IP_DEBUG                        LWIP_DBG_OFF
#endif

#ifndef ETHARP_DEBUG
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#endif

#ifndef NETIF_DEBUG
#define NETIF_DEBUG                     LWIP_DBG_OFF
#endif

#ifndef L3_EVENT_MSG_DEBUG
#define L3_EVENT_MSG_DEBUG              LWIP_DBG_OFF
#endif

#ifndef MLD6_DEBUG
#define MLD6_DEBUG                      LWIP_DBG_OFF
#endif

#ifndef LWIP_NETIF_PROMISC
#define LWIP_NETIF_PROMISC              0
#endif

/* ---------- Memory options ---------- */

#if LWIP_LITEOS_COMPAT
/**
 * MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library
 * instead of the lwIP internal allocator. Can save code size if you
 * already use it.
 */
#define MEM_LIBC_MALLOC         1
#ifndef MEM_SIZE
#define MEM_SIZE                        (4 * 1024 * 1024)
#endif

#ifndef MEM_LIBC_MALLOC_TOTAL_SIZE_LIMIT
#define MEM_LIBC_MALLOC_TOTAL_SIZE_LIMIT 0
#endif

#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS 1
#else
/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#ifndef MEM_SIZE
#define MEM_SIZE               (512 * 1024)
#endif

#endif

#if MEM_LIBC_MALLOC
#ifndef MEM_PBUF_RAM_SIZE_LIMIT
#define MEM_PBUF_RAM_SIZE_LIMIT   1
#endif
#endif

#ifndef MAX_NUM_PID
#define MAX_NUM_PID            128
#endif

/**
 * Defines the align size of the DMA memory. If set to 1, then not using DMA.
 */
#ifndef MEM_MALLOC_DMA_ALIGN
#if LWIP_LITEOS_COMPAT
#define MEM_MALLOC_DMA_ALIGN            1
#else
#define MEM_MALLOC_DMA_ALIGN            1
#endif
#endif


/**
  *  Attention: Be careful while configure large sockets number, i.e, greater than 128, for that
  *  LWIP_NUM_SOCKETS is bind with file system descriptons CONFIG_NFILE_DESCRIPTORS.
  *  If socket num is large, file descriptions for file system would be small. See vfs_config.h
  *  for detail.
  */
#ifndef LWIP_NUM_SOCKETS_MAX
#define LWIP_NUM_SOCKETS_MAX  8 // 128
#endif


/**
 * LWIP_NUM_SOCKETS can be used to override DEFAULT_LWIP_NUM_SOCKETS in lwipopts_*.h
 */
#ifndef LWIP_NUM_SOCKETS

/**
 * DEFAULT_LWIP_NUM_SOCKETS is default number of sockets.
 */
#ifndef DEFAULT_LWIP_NUM_SOCKETS
#define DEFAULT_LWIP_NUM_SOCKETS        8
#endif /* DEFAULT_LWIP_NUM_SOCKETS */

#else /* else LWIP_NUM_SOCKETS*/

#ifndef DEFAULT_LWIP_NUM_SOCKETS

#if LWIP_NUM_SOCKETS > 0
#define DEFAULT_LWIP_NUM_SOCKETS        LWIP_NUM_SOCKETS
#else /* else LWIP_NUM_SOCKETS > 0*/
#define DEFAULT_LWIP_NUM_SOCKETS        8
#endif /* LWIP_NUM_SOCKETS*/

#endif /* DEFAULT_LWIP_NUM_SOCKETS*/
#endif /* LWIP_NUM_SOCKETS*/

/*
 * Provides flexibility to use configured values to spread across code
 */
#ifndef LWIP_ALLOW_SOCKET_CONFIG
#define LWIP_ALLOW_SOCKET_CONFIG 1
#endif

#if LWIP_ALLOW_SOCKET_CONFIG == 0
#ifndef LWIP_CONFIG_NUM_SOCKETS
#define LWIP_CONFIG_NUM_SOCKETS         DEFAULT_LWIP_NUM_SOCKETS
#endif

#else /* LWIP_ALLOW_SOCKET_CONFIG */
#ifndef LWIP_CONFIG_NUM_SOCKETS
extern unsigned int g_lwip_num_sockets;
#define LWIP_CONFIG_NUM_SOCKETS         ((int)g_lwip_num_sockets)
#endif
#endif /* LWIP_ALLOW_SOCKET_CONFIG */

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#ifndef MEMP_NUM_PBUF
#define MEMP_NUM_PBUF           LWIP_CONFIG_NUM_SOCKETS*2
#endif

/*
 * Defines the number of raw connection PCBs
 * (requires the LWIP_RAW option).
 */
#ifndef MEMP_NUM_RAW_PCB
#define MEMP_NUM_RAW_PCB                1
#endif

/*
 * Defines the number of struct tcpip_msg, which are used
 * for callback/timeout API communication.
 * (only needed if you use tcpip.c)
 */
#ifndef MEMP_NUM_TCPIP_MSG_API
#define MEMP_NUM_TCPIP_MSG_API          64
#endif

/*
 * Defines the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 */
#ifndef MEMP_NUM_TCPIP_MSG_INPKT
#define MEMP_NUM_TCPIP_MSG_INPKT        512
#endif

#ifndef MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN        (LWIP_CONFIG_NUM_SOCKETS * 4)
#endif

/*
 * Defines the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifndef MEMP_NUM_NETBUF
#define MEMP_NUM_NETBUF         (65535 * 3 * LWIP_CONFIG_NUM_SOCKETS / (IP_FRAG_MAX_MTU - 20 - 8))
#endif

/*
  * Defines the number of simulateously queued outgoing
  * packets (pbufs) that are waiting for an ARP request (to resolve
  * their destination address) to finish.
  * (requires the ARP_QUEUEING option)
  */
#ifndef MEMP_NUM_ARP_QUEUE
#define MEMP_NUM_ARP_QUEUE      30
#endif

/** Indicates the number of UDP protocol control blocks. One
   per active UDP "connection". */
#ifndef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB        LWIP_CONFIG_NUM_SOCKETS + LWIP_MPL
#endif

/** MEMP_NUM_TCP_PCB: Indicates the number of simultaneously active TCP
   connections. */
#ifndef MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB        LWIP_CONFIG_NUM_SOCKETS
#endif

/** MEMP_NUM_TCP_PCB_LISTEN: Indicates the number of listening TCP
   connections. */
#ifndef MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN LWIP_CONFIG_NUM_SOCKETS
#endif

#ifndef IP_FRAG_MAX_MTU
#define IP_FRAG_MAX_MTU         1500
#endif

/* As per RFC 791, "Every internet module must be able to forward a datagram of 68
 * octets without further fragmentation.  This is because an internet header
 * may be up to 60 octets, and the minimum fragment is 8 octets." */
#ifndef  IP_FRAG_MIN_MTU
#define IP_FRAG_MIN_MTU          68
#endif

#ifndef IP_REASS_MAX_MEM_SIZE
#define IP_REASS_MAX_MEM_SIZE    (MEM_SIZE / 4)
#endif

#ifndef MEMP_NUM_REASSDATA
#define MEMP_NUM_REASSDATA    (IP_REASS_MAX_MEM_SIZE / 65535)
#endif

#ifndef IP_REASS_MAX_PBUFS
#define IP_REASS_MAX_PBUFS   ((65535 * 2 * (LWIP_CONFIG_NUM_SOCKETS)) / ((IP_FRAG_MAX_MTU) - 20 - 8))
#endif
/* Max size(65535 + extra buffer(8192) per MAX MTU) */
#ifndef IP_REASS_MAX_PBUFS_PER_PKT
#define IP_REASS_MAX_PBUFS_PER_PKT   ((65535 + 8192) / (IP_FRAG_MAX_MTU - 20 - 8))
#endif

/* TCP Maximum segment size. */
#ifdef LWIP_TCP_MSS
#undef TCP_MSS      /* ensure TCP_MSS value is overridden by LWIP_TCP_MSS */
#define TCP_MSS                 LWIP_TCP_MSS
#else  /*LWIP_TCP_MSS */
#ifndef TCP_MSS
#define TCP_MSS  (IP_FRAG_MAX_MTU - 20 - 20)
#endif  /*TCP_MSS*/
#endif /* LWIP_TCP_MSS*/

/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#ifndef MEMP_NUM_TCP_SEG
#ifdef WIFI_TCM_OPTIMIZE
#define MEMP_NUM_TCP_SEG        64
#else
#define MEMP_NUM_TCP_SEG        32
#endif
#endif

/* ---------- Pbuf options ---------- */
/** PBUF_POOL_SIZE: Indicates the number of buffers in the pbuf pool. */
#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE          64
#endif

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#ifndef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE       1550
#endif

#ifndef PBUF_LINK_CHKSUM_LEN
#define PBUF_LINK_CHKSUM_LEN  0
#endif

#ifndef PBUF_ZERO_COPY_RESERVE
#define PBUF_ZERO_COPY_RESERVE 80
#endif

#ifndef PBUF_ZERO_COPY_TAILROOM
#define PBUF_ZERO_COPY_TAILROOM 4
#endif

#ifndef LWIP_TCPIP_CORE_LOCKING_INPUT
#define LWIP_TCPIP_CORE_LOCKING_INPUT 1
#endif
/* ---------- ARP options ---------- */
/**
 * ARP_QUEUEING==1: Multiple outgoing packets are queued during hardware address
 * resolution. By default, only the most recent packet is queued per IP_add
 * This is sufficient for most protocols and mainly reduces TCP connection
 * startup time. Set this to 1 if you know that your application sends more than one
 * packet in a row to an IP_add that is not in the ARP cache.
 */
#ifndef ARP_QUEUEING
#define ARP_QUEUEING                    1
#endif

/**
 * ETHARP_TRUST_IP_MAC==1: Incoming IP packets cause the ARP table to be
 * updated with the source MAC and IP address supplied in the packet.
 * You may want to disable this if you do not trust LAN peers to have the
 * correct addresses, or as a limited approach to attempt to handle
 * spoofing. If disabled, lwIP will need to make a new ARP request if
 * the peer is not already in the ARP table, adding a little latency.
 * The peer is in the ARP table if it requested our address before.
 * Also notice that this slows down input processing of every IP packet.
 */
#ifndef ETHARP_TRUST_IP_MAC
#define ETHARP_TRUST_IP_MAC             1
#endif

/** Defines the number of bytes added before the ethernet header to ensure
 * alignment of payload after that header. Since the header is 14 bytes long,
 * without this padding, addresses in the IP header will not be aligned
 * on a 32-bit boundary. So setting this to 2 can speed up 32-bit-platforms.
 */
#ifndef ETH_PAD_SIZE
#define ETH_PAD_SIZE                    2
#endif

/** ETHARP_SUPPORT_STATIC_ENTRIES==1: Enables code to support static ARP table
 * entries (using etharp_add_static_entry/etharp_remove_static_entry).
 */
#ifndef ETHARP_SUPPORT_STATIC_ENTRIES
#define ETHARP_SUPPORT_STATIC_ENTRIES   1
#endif

/* ---------- TCP options ---------- */

#ifndef LWIP_TCP
#define LWIP_TCP                1
#endif
/* @ingroup Config_TCP
* Defines the TCP TTL value.
*/
#ifndef TCP_TTL
#define TCP_TTL                 255
#endif

/**
 * Defines the mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#ifndef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE         64
#endif

/**
 * Defines the mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#ifndef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE       128
#endif

/**
 * Defines the mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#ifndef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE         32
#endif

#ifndef MEMP_NUM_REASSDATA
#define MEMP_NUM_REASSDATA  (2*LWIP_CONFIG_NUM_SOCKETS)
#endif

#ifdef LWIP_DBG_PREPROCESSOR
#pragma message(VAR_NAME_VALUE(TCP_MSS))
#endif

/* @ingroup Config_TCP
* TCP sender buffer space (bytes). */
#ifndef TCP_SND_BUF
#define TCP_SND_BUF         65535
#endif

/* @ingroup Config_TCP
TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#ifndef  TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN        (8 * TCP_SND_BUF) / TCP_MSS
#endif
/**

* Defines the size of a TCP receive window. */

#ifndef TCP_WND
#define TCP_WND            32768
#endif

/**
 * Defines the Maximum number of retransmissions of data segments.
 */
#ifndef TCP_MAXRTX
#define TCP_MAXRTX                      15
#endif

/**
 * User define tcp wnd. default is 0
 */
#ifndef LWIP_TCP_USRWND
#define LWIP_TCP_USRWND                 0
#endif /* LWIP_TCP_USRWND */

/* ---------- IP options ---------- */
/**
 * IP_FRAG_USES_STATIC_BUF==1: Use a static MTU-sized buffer for IP
 * fragmentation. Otherwise pbufs are allocated and reference the original
 * packet data to be fragmented (or with LWIP_NETIF_TX_SINGLE_PBUF==1,
 * new PBUF_RAM pbufs are used for fragments).
 * Note: IP_FRAG_USES_STATIC_BUF==1 may not be used for DMA-enabled MACs.
 */
#ifndef IP_FRAG_USES_STATIC_BUF
#define IP_FRAG_USES_STATIC_BUF         1
#endif

/*
 * Defines the assumed maximum MTU on any interface for IP fragment buffer
 * (requires IP_FRAG_USES_STATIC_BUF==1)
 */
#if IP_FRAG_USES_STATIC_BUF && !defined(IP_FRAG_MAX_MTU)
#define IP_FRAG_MAX_MTU                 1500
#endif

/* As per RFC 791, "Every internet module must be able to forward a datagram of 68
 * octets without further fragmentation.  This is because an internet header
 * may be up to 60 octets, and the minimum fragment is 8 octets." */
#if IP_FRAG_USES_STATIC_BUF && !defined(IP_FRAG_MIN_MTU)
#define IP_FRAG_MIN_MTU			68
#endif

/* ---------- ICMP options ---------- */
/**
 * LWIP_ICMP==1: Enables the ICMP module inside the IP stack.
 */
#ifndef LWIP_ICMP
#define LWIP_ICMP                       1
#endif

/* ---------- IGMP options ---------- */
#ifndef LWIP_IGMP
#define LWIP_IGMP                       1
#endif

/* ---------- DHCP options ---------- */
/**
 * LWIP_DHCP==1: Enables the DHCP module.
 */
#ifndef LWIP_DHCP
#define LWIP_DHCP               1
#endif

/* ---------- DNS options ---------- */
/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#ifndef LWIP_DNS
#define LWIP_DNS                 1
#endif

#if LWIP_DNS
#ifndef LWIP_DNS_REVERSE
#define LWIP_DNS_REVERSE 1
#endif
#else
#undef LWIP_DNS_REVERSE
#define LWIP_DNS_REVERSE 0
#endif

#if !defined DNS_MAX_RETRIES && LWIP_SMALL_SIZE
#define DNS_MAX_RETRIES          2
#endif

/* ---------- UDP options ---------- */
/**
 * LWIP_UDP==1: Turns on UDP.
 */
#ifndef LWIP_UDP
#define LWIP_UDP                1
#endif

#ifndef UDP_TTL
#define UDP_TTL                 255
#endif

#ifndef MC_TTL
#define MC_TTL   1
#endif

/**
 * Defines the mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */


#ifndef DEFAULT_UDP_RECVMBOX_SIZE
#define DEFAULT_UDP_RECVMBOX_SIZE       128
#endif

/* -----------RAW options -----------*/
#ifndef DEFAULT_RAW_RECVMBOX_SIZE

/**
 * LWIP_RAW==1: Enables the application layer to hook into the IP layer itself.
 */
#if !defined LWIP_RAW || defined __DOXYGEN__
#define LWIP_RAW                        1
#endif

/**
 * Enable to support PF_PACKET RAW sockets
*/
#if  defined(LWIP_RAW) && LWIP_RAW
#ifndef PF_PKT_SUPPORT
#define PF_PKT_SUPPORT 1
#endif
#endif

/**
 * Defines the mailbox size for the incoming packets on a
 * NETCONN_RAW. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */

#define DEFAULT_RAW_RECVMBOX_SIZE       128
#endif

#ifndef LWIP_MAX_PF_RAW_SEND_SIZE
#define LWIP_MAX_PF_RAW_SEND_SIZE (0xFFFF - 40) //  - IPv6 header
#endif

/* -----------LOOPIF options -----------*/
#ifndef LWIP_HAVE_LOOPIF
#define LWIP_HAVE_LOOPIF                1
#endif

/* -----------LOOPBACK options -----------*/
#ifndef LWIP_NETIF_LOOPBACK
#define LWIP_NETIF_LOOPBACK             1
#endif

/* ---------- Statistics options ---------- */
/**
 * LWIP_STATS==1: Enables statistics gathering in lwip_stats.
 */
#ifndef LWIP_STATS
#define LWIP_STATS 1
#endif

/* ---------- TFTP options ---------- */
#ifndef LWIP_TFTP
#define LWIP_TFTP               1
#endif

/* ---------- SNTP options ---------- */
#ifndef LWIP_SNTP
#define LWIP_SNTP                      1
#endif

/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 */
#ifndef SO_REUSE
#define SO_REUSE                        1
#endif

#define LWIP_ETHERNET               1
#define LWIP_NETCONN                    1
#define LWIP_SOCKET                    1

#ifndef IP_FORWARD
#define IP_FORWARD                      0
#endif

#ifndef LWIP_DHCPS_START_ADDR_OFFSET
#define LWIP_DHCPS_START_ADDR_OFFSET   2
#endif

#define LWIP_DHCPS_DISCOVER_BROADCAST   0

#if defined (CONFIG_DHCPS_GW)
#ifndef LWIP_DHCPS_GW
#define LWIP_DHCPS_GW 1
#endif
#else
#ifndef LWIP_DHCPS_GW
#define LWIP_DHCPS_GW 0
#endif
#endif

#ifdef NOT_SUPPORT_IP_SOF_BROADCAST
#ifndef IP_SOF_BROADCAST
#define IP_SOF_BROADCAST 0
#endif
#else
#ifndef IP_SOF_BROADCAST
#define IP_SOF_BROADCAST 1
#endif
#endif

#ifndef LWIP_DHCP_REQUEST_UNICAST
#define LWIP_DHCP_REQUEST_UNICAST 0
#endif

#if LWIP_LITEOS_COMPAT
#ifndef TCPIP_THREAD_STACKSIZE
#define TCPIP_THREAD_STACKSIZE          0x6000
#endif
#ifndef TCPIP_THREAD_PRIO
#define TCPIP_THREAD_PRIO               5
#endif

#ifndef LWIP_SOCKET_START_NUM
#define LWIP_SOCKET_START_NUM CONFIG_NFILE_DESCRIPTORS
#endif


#ifndef LWIP_SOCKET_OFFSET
#define LWIP_SOCKET_OFFSET CONFIG_NFILE_DESCRIPTORS
#endif

#ifndef LWIP_ENABLE_LOS_SHELL_CMD
#define LWIP_ENABLE_LOS_SHELL_CMD     1
#endif
#ifndef ERRNO
#define ERRNO
#endif
#ifndef LWIP_PROVIDE_ERRNO
#define LWIP_PROVIDE_ERRNO 0
#endif
/* Currently pthread_cond_timedwait() in liteos doesn't support absolute time */
#ifndef LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT
#define LWIP_USE_POSIX_COND_WAIT_WITH_ABS_TIMEOUT 0
#endif

#ifndef LWIP_INET_ADDR_FUNC
#define LWIP_INET_ADDR_FUNC         1
#endif

#ifndef LWIP_INET_NTOA_FUNC
#define LWIP_INET_NTOA_FUNC         1
#endif

#ifndef LWIP_INET_ATON_FUNC
#define LWIP_INET_ATON_FUNC         1
#endif


#else
#define ERRNO
#define LWIP_PROVIDE_ERRNO 1

#ifndef LWIP_INET_ADDR_FUNC
#define LWIP_INET_ADDR_FUNC         1
#endif

#ifndef LWIP_INET_NTOA_FUNC
#define LWIP_INET_NTOA_FUNC         1
#endif

#ifndef LWIP_INET_ATON_FUNC
#define LWIP_INET_ATON_FUNC         1
#endif

#endif

/* ---------- Socket Filter options ---------- */
/**
  * implement sock filter based on BPF.
  * BPF-extension was not supported.
  * only AF_PACKET RAW socket support socket filter now,
  * we would add supports for other socket type in the feature.
*/
#if defined(LWIP_RAW) && LWIP_RAW
#ifndef LWIP_SOCK_FILTER
#define LWIP_SOCK_FILTER 1
#endif
#else
#define LWIP_SOCK_FILTER 0
#endif

#ifndef LWIP_STATIC
#define LWIP_STATIC static
#endif


#ifndef LWIP_ALWAYS_SEND_HWTYPE_AS_ETHER_IN_DHCP
#define LWIP_ALWAYS_SEND_HWTYPE_AS_ETHER_IN_DHCP    1
#endif

#ifndef USE_ONLY_PRIMARY_IFACE
#define USE_ONLY_PRIMARY_IFACE 1
#endif

#ifndef DRIVER_STATUS_CHECK
#define DRIVER_STATUS_CHECK    0
#endif

#if DRIVER_STATUS_CHECK
#ifndef DRIVER_WAKEUP_INTERVAL
#define DRIVER_WAKEUP_INTERVAL 120000
#endif
#endif

/* ---------- NETIF options ---------- */
/**
 * LWIP_NETIF_API==1: Supports netif api (in netifapi.c)
 */
#ifndef LWIP_NETIF_API
#define LWIP_NETIF_API                  1
#endif

/**
 * LWIP_NETIF_EXT_STATUS_CALLBACK==1: Support an extended callback function
 * for several netif related event that supports multiple subscribers.
 * @see netif_ext_status_callback
 */
#ifndef LWIP_NETIF_EXT_STATUS_CALLBACK
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1
#endif

/*
 * LWIP_NETIF_LINK_CALLBACK==1: Supports a callback function from an interface
 * whenever the link changes. Example: when link goes down.
 */
#ifndef LWIP_NETIF_LINK_CALLBACK
#define LWIP_NETIF_LINK_CALLBACK        1
#endif

/*
 * LWIP_NETIF_NUM_MAX==255: Defines the maximum support number of same type netif.
 */
/* Increase the count of netifs supported by stack.
Currently stack restricts the number of netif to 5, but older versions of linux kernel supports
upto 255 interfaces . New versions donot have these restrictions but we stick to 255   */
#ifndef LWIP_NETIF_NUM_MAX
#define LWIP_NETIF_NUM_MAX            10
#endif

#ifndef LWIP_NETIF_IFINDEX_START
#define LWIP_NETIF_IFINDEX_START        1
#endif /* LWIP_NETIF_IFINDEX_START */

#ifndef LWIP_NETIF_IFINDEX_MAX
#define LWIP_NETIF_IFINDEX_MAX 0xFE
#endif

/* ---------- SOCKET options ---------- */
/**
 * LWIP_SO_RCVBUF==1: Enables SO_RCVBUF processing.
 */
#ifndef LWIP_SO_RCVBUF
#define LWIP_SO_RCVBUF                  1
#endif

/* ---------- Statistics options ---------- */
/**
 * LWIP_STATS_DISPLAY==1: Compile in the statistics output functions.
 */
#ifndef LWIP_STATS_DISPLAY
#define LWIP_STATS_DISPLAY              0
#endif

/* ---------- Debugging options ---------- */
/**
 * LWIP_DBG_MIN_LEVEL: After masking, the value of the debug is
 * compared against this value. If it is smaller, then debugging
 * messages are written.
 */
#ifndef LWIP_DBG_MIN_LEVEL
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_OFF
#endif

#ifndef LWIP_SOCKET_POLL
#define LWIP_SOCKET_POLL 1
#endif

#ifndef LWIP_EXT_POLL_SUPPORT
#define LWIP_EXT_POLL_SUPPORT 1
#endif

// flags for cross compilation-- manual.. to be automated
#ifndef LWIP_TCP_INFO
#define LWIP_TCP_INFO 1
#endif

#ifndef LWIP_RPL
#define LWIP_RPL 0
#endif

#if !defined(LWIP_RPL) || (0 == LWIP_RPL)
#ifndef LWIP_RPL_TIMER_COUNT
#define LWIP_RPL_TIMER_COUNT 0
#endif
#else
#ifndef LWIP_RPL_TIMER_COUNT
#define LWIP_RPL_TIMER_COUNT 12
#endif
#endif

#ifndef LWIP_IPV6_MLD
#define LWIP_IPV6_MLD 1
#endif

#ifndef LWIP_MLD6_ENABLE_MLD_ON_DAD
#define LWIP_MLD6_ENABLE_MLD_ON_DAD   LWIP_IPV6 && LWIP_IPV6_MLD
#endif

#ifndef LWIP_MLD6_DONE_ONLYFOR_LAST_REPORTER
#define LWIP_MLD6_DONE_ONLYFOR_LAST_REPORTER   LWIP_IPV6 && LWIP_IPV6_MLD
#endif
#ifndef NETIF_MAX_HWADDR_LEN
#define NETIF_MAX_HWADDR_LEN 6
#endif

#ifndef LWIP_ND6_NUM_NEIGHBORS
#define LWIP_ND6_NUM_NEIGHBORS 10
#endif
#ifndef LWIP_IPV6_SCOPES
#define LWIP_IPV6_SCOPES 0
#endif

#ifndef LWIP_ND6_STATIC_NBR
#define LWIP_ND6_STATIC_NBR 0
#endif
#ifndef LWIP_L3_EVENT_MSG
#define LWIP_L3_EVENT_MSG 1
#endif

#if LWIP_L3_EVENT_MSG

#ifndef MEMP_NUM_L3_EVENT_MSG
#define MEMP_NUM_L3_EVENT_MSG 15
#endif

#ifndef LWIP_ROUTE_CHANGE_MSG
#define LWIP_ROUTE_CHANGE_MSG 1
#endif

#ifndef LWIP_NAT64_CHANGE_MSG
#define LWIP_NAT64_CHANGE_MSG 1
#endif

#ifndef LWIP_RPL_JOIN_SUCC_MSG
#define LWIP_RPL_JOIN_SUCC_MSG 1
#endif

#ifndef LWIP_RPL_PARENT_CLEAR_MSG
#define LWIP_RPL_PARENT_CLEAR_MSG 1
#endif

#ifndef LWIP_RPL_BEACON_PRI_RESET
#define LWIP_RPL_BEACON_PRI_RESET 1
#endif

#ifndef LWIP_RPL_GET_IPV4_ADDR_FAIL
#define LWIP_RPL_GET_IPV4_ADDR_FAIL 1
#endif

#ifndef LWIP_RPL_MSTA_GET_IPV4_ADDR_FAIL
#define LWIP_RPL_MSTA_GET_IPV4_ADDR_FAIL 1
#endif

#ifndef LWIP_RPL_MG_FULL
#define LWIP_RPL_MG_FULL 1
#endif

#ifndef LWIP_RPL_RT_FULL
#define LWIP_RPL_RT_FULL 1
#endif

#ifndef LWIP_RPL_GET_MNID_FAIL
#define LWIP_RPL_GET_MNID_FAIL 1
#endif

#ifndef LWIP_L3_EVENT_MSG_EXIST
#if (LWIP_ROUTE_CHANGE_MSG || LWIP_RPL_JOIN_SUCC_MSG || \
       LWIP_RPL_PARENT_CLEAR_MSG || LWIP_RPL_BEACON_PRI_RESET || \
       LWIP_RPL_GET_IPV4_ADDR_FAIL || LWIP_RPL_MSTA_GET_IPV4_ADDR_FAIL || \
       LWIP_RPL_MG_FULL || LWIP_RPL_RT_FULL || LWIP_RPL_GET_MNID_FAIL)
#define LWIP_L3_EVENT_MSG_EXIST 1
#else
#define LWIP_L3_EVENT_MSG_EXIST 0
#endif
#endif

#endif /* LWIP_L3_EVENT_MSG */

#ifndef LWIP_LITEOS_TASK
#define LWIP_LITEOS_TASK 1
#endif

#ifndef LWIP_SOCK_OPT_ICMP6_FILTER
#define LWIP_SOCK_OPT_ICMP6_FILTER 0
#endif

#if LWIP_RIPPLE
#ifndef LWIP_IPV6_FORWARD
#define LWIP_IPV6_FORWARD 1
#endif
#endif

#if LWIP_RIPPLE
#ifndef LWIP_MPL
#define LWIP_MPL  0
#endif
#else
#ifndef LWIP_MPL
#define LWIP_MPL  0
#endif
#endif

#if LWIP_MPL
#ifndef LWIP_MPL_IPV4
#define LWIP_MPL_IPV4  1
#endif
#else
#ifndef LWIP_MPL_IPV4
#define LWIP_MPL_IPV4  0
#endif
#endif

#ifndef LWIP_ENABLE_MESH_SHELL_CMD
#define LWIP_ENABLE_MESH_SHELL_CMD 1
#else
#ifndef LWIP_MPL_IPV4
#define LWIP_MPL_IPV4  0
#endif
#endif

#ifndef LWIP_MPL_IPV4_BCAST
#define LWIP_MPL_IPV4_BCAST 0
#endif
#ifndef LWIP_IPV6_MLD_QUERIER
#define LWIP_IPV6_MLD_QUERIER 1
#endif

#ifndef LWIP_LINK_STATUS_CALLBACK
#define LWIP_LINK_STATUS_CALLBACK 1
#endif

#ifdef CONFIG_DENY_DNS_SERVER

#ifndef LWIP_DENY_DNS_SERVER
#define LWIP_DENY_DNS_SERVER 1
#endif

#else

#ifndef LWIP_DENY_DNS_SERVER
#define LWIP_DENY_DNS_SERVER 0
#endif

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LWIPOPTS_H__ */



