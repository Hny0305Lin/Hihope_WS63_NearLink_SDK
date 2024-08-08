/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: Header file for wal_ioctl.c.
 */

#ifndef __WAL_IOCTL_H__
#define __WAL_IOCTL_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "hmac_ext_if.h"
#include "wal_main.h"
#include "wal_event_msg.h"
#include "soc_wifi_driver_wpa_if.h"
#include "msg_rekey_offload_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define WAL_IOCTL_PRIV_SUBCMD_MAX_LEN          20

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
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
/* 协议模式与字符串映射 */
typedef struct wal_android_wifi_priv_cmd {
    td_s32    l_total_len;
    td_s32    l_used_len;
    td_u8   *buf;
}wal_android_wifi_priv_cmd_stru;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  10 函数声明
*****************************************************************************/
td_u32 wal_start_vap(oal_net_device_stru *netdev);
td_u32 wal_stop_vap(oal_net_device_stru *netdev);
td_u32 wal_init_wlan_vap(oal_net_device_stru *netdev);
td_u32 wal_deinit_wlan_vap(oal_net_device_stru *netdev);
td_u32 wal_setup_vap(oal_net_device_stru *netdev);
td_u32 wal_host_dev_init(oal_net_device_stru *netdev);
td_u32 wal_recover_ini_main_gloabal(td_void);
td_u32 wal_host_dev_exit(oal_net_device_stru *netdev);
td_u32 uapi_ccpriv_set_monitor_switch(oal_net_device_stru *netdev, td_s8 *pc_param);
td_u32 uapi_ccpriv_set_mode(oal_net_device_stru *netdev, td_s8 *pc_param);
td_u32 wal_ioctl_set_essid(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 wal_ioctl_set_txpower(oal_net_device_stru *netdev, td_char *pc_param);
td_u32 wal_set_mac_to_mib(oal_net_device_stru *netdev);
td_u32 uapi_ccpriv_show_frag_threshold(oal_net_device_stru *net_dev, td_s8 *param);
#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
td_u32 wal_cfg80211_set_rekey_info(oal_net_device_stru *netdev, mac_rekey_offload_stru *rekey_offload);
#endif
td_u32 wal_macaddr_check(const td_u8 *mac_addr);
#ifdef _PRE_WLAN_FEATURE_P2P
td_u32 wal_ioctl_set_p2p_noa(oal_net_device_stru *netdev, const mac_cfg_p2p_noa_param_stru *p2p_noa_param);
td_u32 wal_ioctl_set_p2p_ops(oal_net_device_stru *netdev, const mac_cfg_p2p_ops_param_stru *p2p_ops_param);
#endif
td_u32 wal_ioctl_set_wps_p2p_ie(oal_net_device_stru *netdev, const td_u8 *buf, td_u32 len,
    app_ie_type_uint8 type);
#ifdef _PRE_WLAN_FEATURE_WPA3
td_u32 wal_cfg80211_external_auth_status(oal_net_device_stru *netdev, const ext_external_auth_stru *param);
osal_void wal_cfg80211_update_dh_ie(oal_net_device_stru *netdev, const ext_update_dh_ie_stru *param);
#endif
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of wal_ioctl.h */

