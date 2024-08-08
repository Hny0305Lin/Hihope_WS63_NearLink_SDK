/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Header file for wal_enent.c.
 */

#ifndef __WAL_CFG80211_APT_H__
#define __WAL_CFG80211_APT_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oam_ext_if.h"
#include "oal_ext_if.h"
#include "wal_event.h"
#include "soc_wifi_driver_wpa_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
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
/*****************************************************************************
  8 UNION定义
*****************************************************************************/
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  10 函数声明
*****************************************************************************/
td_s32 drv_soc_register_send_event_cb(ext_send_event_cb func);
td_u32 cfg80211_new_sta(const oal_net_device_stru *netdev, const td_u8 *mac_addr, td_u8 addr_len,
    const oal_station_info_stru *sinfo);
td_u32 cfg80211_del_sta(oal_net_device_stru *netdev, const td_u8 *mac_addr, td_u8 addr_len);
td_u8 cfg80211_mgmt_tx_status(struct wireless_dev *wdev, const td_u8 *buf, size_t len, td_u8 ack);
td_void cfg80211_inform_bss_frame(const oal_net_device_stru *netdev, const oal_ieee80211_channel *ieee80211_channel,
    const wal_scanned_bss_info_stru *scanned_bss_info);
td_void cfg80211_connect_result(const oal_net_device_stru *netdev, const oal_connet_result_stru *connet_result);
td_u32 cfg80211_disconnected(const oal_net_device_stru *netdev, td_u16 reason, const td_u8 *ie, td_u32 ie_len);
td_u32 cfg80211_timeout_disconnected(const oal_net_device_stru *netdev);
td_void cfg80211_scan_done(const oal_net_device_stru *netdev, ext_scan_status_enum status);
td_u8 cfg80211_rx_mgmt(const oal_net_device_stru *netdev, td_s32 freq, td_s32 l_sig_mbm,
    const td_u8 *buf, size_t len);
td_u32 cfg80211_csa_channel_switch(const oal_net_device_stru *netdev, td_s32 freq);
td_u32 cfg80211_remain_on_channel(const oal_net_device_stru *netdev, td_u32 freq, td_u32 duration);
td_u32 cfg80211_cancel_remain_on_channel(const oal_net_device_stru *netdev, td_u32 freq);

#ifdef _PRE_WLAN_FEATURE_MESH
td_u32 cfg80211_mesh_close(const oal_net_device_stru *netdev, const td_u8 *mac_addr, td_u8 addr_len, td_u16 reason);
#endif

osal_void wal_register_upload_frame_cb(wifi_promis_cb func);
osal_s32 wal_do_upload_promis_frame_cb(const hmac_rx_mgmt_event_stru *frame);
#ifdef _PRE_WLAN_FEATURE_WPA3
td_void cfg80211_external_auth_req(const oal_net_device_stru *netdev, const oal_external_auth_stru *ext_auth);
td_void cfg80211_owe_info(const oal_net_device_stru *netdev, const ext_update_dh_ie_stru *owe_info);
#endif

#if defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_WLAN_FEATURE_11R)
td_u32 cfg80211_ft_event_proc_sta_etc(const oal_net_device_stru *netdev,
    const oal_cfg80211_ft_event_stru *cfg_ft_event);
#endif

osal_void uapi_wlan_register_report_mgmt_frame(wifi_rx_mgmt_cb mgmt_process_cb);
osal_s32 wal_do_report_rx_mgmt_frame_cb(hmac_rx_mgmt_event_stru *mgmt_frame);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
#endif /* __WAL_CFG80211_APT_H__ */
