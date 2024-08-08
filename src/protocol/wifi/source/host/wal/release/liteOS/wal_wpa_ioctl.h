/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Header file for wal_wpa_ioctl.c.
 */

#ifndef __WAL_WPA_IOCTL_H__
#define __WAL_WPA_IOCTL_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "soc_wifi_driver_wpa_if.h"
#include "oam_ext_if.h"
#include "wal_main.h"
#include "oal_net.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* Driver supports AP mode */
#define EXT_DRIVER_FLAGS_AP                         0x00000040
/* Driver supports concurrent P2P operations */
#define EXT_DRIVER_FLAGS_P2P_CONCURRENT             0x00000200
/*
 * Driver uses the initial interface as a
 * dedicated management interface, i.e., it cannot
 * be used for P2P group operations or non-P2P purposes.
 */
#define EXT_DRIVER_FLAGS_P2P_DEDICATED_INTERFACE    0x00000400
/* P2P capable (P2P GO or P2P Client) */
#define EXT_DRIVER_FLAGS_P2P_CAPABLE                0x00000800
/* Driver supports a dedicated interface for P2P Device */
#define EXT_DRIVER_FLAGS_DEDICATED_P2P_DEVICE       0x20000000

/* Driver supports SAE with user space SME */
#define EXT_DRIVER_FLAGS_SAE                         0x02000000

#define EXT_DRIVER_FLAGS_SME 0x00000020
#define EXT_DRIVER_FLAGS_UPDATE_FT_IES 0x1000000000000000ULL
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
typedef enum {
    EXT_CHAN_DISABLED      = 1<<0,
    EXT_CHAN_PASSIVE_SCAN  = 1<<1,
    EXT_CHAN_NO_IBSS       = 1<<2,
    EXT_CHAN_RADAR         = 1<<3,
    EXT_CHAN_NO_HT40PLUS   = 1<<4,
    EXT_CHAN_NO_HT40MINUS  = 1<<5,
    EXT_CHAN_NO_OFDM       = 1<<6,
    EXT_CHAN_NO_80MHZ      = 1<<7,
    EXT_CHAN_NO_160MHZ     = 1<<8,
}ext_channel_flags_enum;
typedef td_u8 ext_channel_flags_enum_uint8;
/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
/*****************************************************************************
  5 消息头定义
*****************************************************************************/
/*****************************************************************************
  6 消息定义
*****************************************************************************/
/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef td_s32 (*hwal_ioctl_handler)(td_char *puc_ifname, td_void *p_buf);

/*****************************************************************************
  8 UNION定义
*****************************************************************************/
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  10 函数声明
*****************************************************************************/
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
td_s32 uapi_ioctl_set_key(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_get_hw_feature(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_change_beacon(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_scan(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_set_mode(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_get_mode(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_del_beacon(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_disconnect(const td_char *puc_ifname, const td_void *buf);
td_s32 uapi_ioctl_sta_remove(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_assoc(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_set_delay_report(const td_char *puc_ifname, td_void *buf);
#endif
td_s32 uapi_ioctl_new_key(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_del_key(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_set_ap(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_send_mlme(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_send_eapol(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_receive_eapol(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_enable_eapol(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_disable_eapol(const td_char *puc_ifname, const td_void *buf);
td_s32 uapi_ioctl_get_addr(const td_char *puc_ifname, td_void *buf);
#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
td_s32 uapi_ioctl_get_survey(const td_char *puc_ifname, td_void *buf);
#endif
#if defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_WLAN_FEATURE_11R)
td_s32 uapi_ioctl_update_ft_ies(const td_char *puc_ifname, td_void *buf);
#endif
td_s32 drv_soc_hwal_wpa_ioctl(td_char *pc_ifname, const ext_ioctl_command_stru *cmd);
td_s32 uapi_ioctl_set_netdev(const td_char *puc_ifname, const td_void *buf);
td_u8 hwal_is_valid_ie_attr(const td_u8 *ie, td_u32 ie_len);
td_s32 uapi_ioctl_send_action(const td_char *puc_ifname, td_void *buf);
#ifdef _PRE_WLAN_FEATURE_MESH
td_s32 uapi_ioctl_set_mesh_user(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_set_mesh_gtk(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_set_accept_peer(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_set_accept_sta(const td_char *puc_ifname, td_void *buf);
#endif
td_s32 uapi_ioctl_set_user_app_ie(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_set_csi_config(const td_char *puc_ifname, const td_void *buf);
td_s32 uapi_ioctl_csi_switch(const td_char *puc_ifname, const td_void *buf);

#ifdef _PRE_WLAN_FEATURE_P2P
td_s32 uapi_ioctl_add_if(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_remove_if(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_probe_req_report(const td_char *puc_ifname, const td_void *buf);
td_s32 uapi_ioctl_remain_on_channel(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_cancel_remain_on_channel(const td_char *puc_ifname, const td_void *buf);
td_s32 uapi_ioctl_set_p2p_noa(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_set_p2p_powersave(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_get_p2p_addr(const td_char *puc_ifname, td_void *buf);
#endif
td_s32 uapi_ioctl_set_ap_wps_p2p_ie(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_get_drv_flags(const td_char *puc_ifname, td_void *buf);

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
td_s32 uapi_ioctl_set_rekey_info(const td_char *puc_ifname, td_void *buf);
#endif
#ifdef _PRE_WLAN_FEATURE_WPA3
td_s32 uapi_ioctl_send_ext_auth_status(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_update_dh_ie(const td_char *ifname, td_void *buf);
#endif
td_s32 uapi_ioctl_add_pmkid(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_remove_pmkid(const td_char *puc_ifname, td_void *buf);
td_s32 uapi_ioctl_flush_pmkid(const td_char *puc_ifname, td_void *buf);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of wal_wpa_ioctl.h */

