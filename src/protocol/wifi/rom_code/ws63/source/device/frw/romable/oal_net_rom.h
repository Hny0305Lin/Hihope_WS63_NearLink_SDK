/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: oal net interface
 */

#ifndef OAL_NET_ROM_H
#define OAL_NET_ROM_H

#include "oal_types_device_rom.h"
#include "frw_util_rom.h"
#include "eth_ip_common_rom.h"
#include "dmac_misc_type.h"
#include "oal_net_pkt_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OAL_NETBUF_DEFAULT_DATA_OFFSET  48 /* 5115上实际测得data比head大48，用于netbuf data指针复位 */

#define OAL_ASSOC_REQ_IE_OFFSET         28 /* 上报内核关联请求帧偏移量 */
#define OAL_ASSOC_RSP_IE_OFFSET         30 /* 上报内核关联响应帧偏移量 */
#define OAL_AUTH_IE_OFFSET              30
#define OAL_FT_ACTION_IE_OFFSET         40
#define OAL_ASSOC_RSP_FIXED_OFFSET      6 /* 关联响应帧帧长FIXED PARAMETERS偏移量 */
#define OAL_MAC_ADDR_LEN                6
#define OAL_PMKID_LEN                   16
#define OAL_WPA_KEY_LEN                 32
#define OAL_WPA_SEQ_LEN                 16
#define OAL_WLAN_SA_QUERY_TR_ID_LEN     2
#define OAL_BITFIELD_LITTLE_ENDIAN      0
#define OAL_BITFIELD_BIG_ENDIAN         1
/*
 * Byte order/swapping support.
 */
#define OAL_LITTLE_ENDIAN               1234
#define OAL_BIG_ENDIAN                  4321
#define OAL_BYTE_ORDER                  OAL_BIG_ENDIAN

/*****************************************************************************
  2.10 IP宏定义
*****************************************************************************/
#define WLAN_IP_PRI_SHIFT               5
#define WLAN_IPV6_PRIORITY_MASK         0x0FF00000
#define WLAN_IPV6_PRIORITY_SHIFT        20

/*****************************************************************************
  2.12 LLC SNAP宏定义
*****************************************************************************/
#define LLC_UI                          0x3
#define SNAP_LLC_FRAME_LEN              8
#define SNAP_LLC_LSAP                   0xaa
#define SNAP_RFC1042_ORGCODE_0          0x00
#define SNAP_RFC1042_ORGCODE_1          0x00
#define SNAP_RFC1042_ORGCODE_2          0x00
#define SNAP_BTEP_ORGCODE_0             0x00
#define SNAP_BTEP_ORGCODE_1             0x00
#define SNAP_BTEP_ORGCODE_2             0xf8

/*****************************************************************************
  2.13 ETHER宏定义
*****************************************************************************/

#define ETHER_TYPE_LEN                  2 /* length of the Ethernet type field */
#define ETHER_CRC_LEN                   4  /* length of the Ethernet CRC */
#ifndef ETHER_HDR_LEN
#define ETHER_HDR_LEN                   14
#endif
#ifndef ETHER_MAX_LEN
#define ETHER_MAX_LEN                   1518
#endif
#define ETHER_MTU                       (ETHER_MAX_LEN - ETHER_HDR_LEN - ETHER_CRC_LEN)

/* Probe Rsp APP IE长度超过该值，发送帧netbuf采用大包 */
#define OAL_MGMT_NETBUF_APP_PROBE_RSP_IE_LEN_LIMIT 450


/* ether type */
#define ETHER_TYPE_PAE          0x888e      /* EAPOL PAE/802.1x */
#define ETHER_TYPE_IP           0x0800       /* IP protocol */
#define ETHER_TYPE_AARP         0x80f3     /* Appletalk AARP protocol */
#define ETHER_TYPE_IPX          0x8137      /* IPX over DIX protocol */
#define ETHER_TYPE_ARP          0x0806      /* ARP protocol */
#define ETHER_TYPE_IPV6         0x86dd     /* IPv6 */
#define ETHER_TYPE_TDLS         0x890d     /* TDLS */
#define ETHER_TYPE_VLAN         0x8100     /* VLAN TAG protocol */
#define ETHER_TYPE_WAI          0x88b4      /* WAI/WAPI */
#define ETHER_LLTD_TYPE         0x88D9     /* LLTD */
#define ETHER_ONE_X_TYPE        0x888E    /* 802.1x Authentication */
#define ETHER_TUNNEL_TYPE       0x88bd   /* 自定义tunnel协议 */
#define ETHER_TYPE_PPP_DISC     0x8863 /* PPPoE discovery messages */
#define ETHER_TYPE_PPP_SES      0x8864  /* PPPoE session messages */


/* ip头到协议类型字段的偏移 */
#define IP_HDR_LEN              20

/* CCMP加密字节数 */
#define WLAN_CCMP_ENCRYP_LEN    16
/* CCMP256加密字节数 */
#define WLAN_CCMP256_GCMP_ENCRYP_LEN 24

#define WLAN_DATA_VIP_TID       WLAN_TIDNO_BCAST

/* wiphy  */
#define IEEE80211_HT_MCS_MASK_LEN 10

/* ICMP codes for neighbour discovery messages */
#define OAL_NDISC_ROUTER_SOLICITATION       133
#define OAL_NDISC_ROUTER_ADVERTISEMENT      134
#define OAL_NDISC_NEIGHBOUR_SOLICITATION    135
#define OAL_NDISC_NEIGHBOUR_ADVERTISEMENT   136

#define OAL_ND_OPT_TARGET_LL_ADDR           2
#define OAL_ND_OPT_SOURCE_LL_ADDR           1
#define OAL_IPV6_ADDR_ANY                   0x0000U
#define OAL_IPV6_ADDR_MULTICAST             0x0002U
#define OAL_IPV6_MAC_ADDR_LEN               16

#define IPV6_ADDR_MULTICAST                 0x0002U
#define IPV6_ADDR_UNICAST                   0x0001U
#define IPV6_ADDR_SCOPE_NODELOCAL           0x01
#define IPV6_ADDR_SCOPE_LINKLOCAL           0x02
#define IPV6_ADDR_SCOPE_SITELOCAL           0x05
#define IPV6_ADDR_SCOPE_ORGLOCAL            0x08
#define IPV6_ADDR_SCOPE_GLOBAL              0x0e
#define IPV6_ADDR_LOOPBACK                  0x0010U
#define IPV6_ADDR_LINKLOCAL                 0x0020U
#define IPV6_ADDR_SITELOCAL                 0x0040U

/*
 * support for ARP/ND offload. add 2015.6.12
 */
#define OAL_IPV4_ADDR_SIZE      4
#define OAL_IPV6_ADDR_SIZE      16
#define OAL_IP_ADDR_MAX_SIZE    OAL_IPV6_ADDR_SIZE

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
/* DHCP options */
enum dhcp_opt {
    DHCP_OPT_PAD                    = 0,
    DHCP_OPT_SUBNETMASK             = 1,
    DHCP_OPT_ROUTER                 = 3,
    DHCP_OPT_DNSSERVER              = 6,
    DHCP_OPT_HOSTNAME               = 12,
    DHCP_OPT_DNSDOMAIN              = 15,
    DHCP_OPT_MTU                    = 26,
    DHCP_OPT_BROADCAST              = 28,
    DHCP_OPT_STATICROUTE            = 33,
    DHCP_OPT_NISDOMAIN              = 40,
    DHCP_OPT_NISSERVER              = 41,
    DHCP_OPT_NTPSERVER              = 42,
    DHCP_OPT_VENDOR                 = 43,
    DHCP_OPT_IPADDRESS              = 50,
    DHCP_OPT_LEASETIME              = 51,
    DHCP_OPT_OPTIONSOVERLOADED      = 52,
    DHCP_OPT_MESSAGETYPE            = 53,
    DHCP_OPT_SERVERID               = 54,
    DHCP_OPT_PARAMETERREQUESTLIST   = 55,
    DHCP_OPT_MESSAGE                = 56,
    DHCP_OPT_MAXMESSAGESIZE         = 57,
    DHCP_OPT_RENEWALTIME            = 58,
    DHCP_OPT_REBINDTIME             = 59,
    DHCP_OPT_VENDORCLASSID          = 60,
    DHCP_OPT_CLIENTID               = 61,
    DHCP_OPT_USERCLASS              = 77,  /* RFC 3004 */
    DHCP_OPT_FQDN                   = 81,
    DHCP_OPT_DNSSEARCH              = 119, /* RFC 3397 */
    DHCP_OPT_CSR                    = 121, /* RFC 3442 */
    DHCP_OPT_SIXRD                  = 212, /* RFC 5969 */
    DHCP_OPT_MSCSR                  = 249, /* MS code for RFC 3442 */
    DHCP_OPT_END                    = 255
};
#endif

#define OAL_NL80211_MAX_NR_CIPHER_SUITES    5
#define OAL_NL80211_MAX_NR_AKM_SUITES       2
#define OAL_IFF_RUNNING                     0x40    /* linux net_device flag宏定义，表示设备在运行 */
#define OAL_IEEE80211_MAX_SSID_LEN          32      /* 最大SSID长度 */
#define OAL_DEVICE_MAX_SCAN_CHANNELS               40      /* 扫描信道数 */

enum {
    OAL_INET_ECN_NOT_ECT = 0,
    OAL_INET_ECN_ECT_1 = 1,
    OAL_INET_ECN_ECT_0 = 2,
    OAL_INET_ECN_CE = 3,
    OAL_INET_ECN_MASK = 3,
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_net_rom.h */
