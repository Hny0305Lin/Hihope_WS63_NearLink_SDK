/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Header file for wal_event.c.
 */

#ifndef __WAL_EVENT_H__
#define __WAL_EVENT_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "wal_main.h"
#include "hmac_ext_if.h"
#include "soc_wifi_sdp_api.h"
#ifdef _PRE_WLAN_FEATURE_WPA3
#include "soc_wifi_driver_wpa_if.h"
#endif
#include "soc_wifi_api.h"

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
/* 驱动sta上报内核的扫描结果 */
typedef struct {
    td_s32               l_signal;      /* 信号强度 */

    td_s16               s_freq;        /* bss所在信道的中心频率 */
    td_u8               arry[2];   /* 2 数组元素个数 */

    td_u32                mgmt_len;   /* 管理帧长度 */
    oal_ieee80211_mgmt_stru  *mgmt;      /* 管理帧起始地址 */
}wal_scanned_bss_info_stru;

/* 驱动sta上报内核的关联结果 */
typedef struct {
    td_u8       bssid[WLAN_MAC_ADDR_LEN];  /* sta关联的ap mac地址 */
    td_u16      status_code;                /* ieee协议规定的16位状态码 */

    td_u8       *rsp_ie;                 /* asoc_req_ie  */
    td_u8       *req_ie;

    td_u32      req_ie_len;               /* asoc_req_ie len */
    td_u32      rsp_ie_len;

    td_u16      connect_status;
    td_u16      freq;
}oal_connet_result_stru;

/* 驱动sta上报内核的去关联结果 */
typedef struct {
    td_u16  reason_code;         /* 去关联 reason code */
    td_u8   auc_resv[2];            /* 2 保留字节 */

    td_u8  *pus_disconn_ie;        /* 去关联关联帧 ie */
    td_u32  disconn_ie_len;      /* 去关联关联帧 ie 长度 */
}oal_disconnect_result_stru;

#ifdef _PRE_WLAN_FEATURE_WPA3
/* 驱动上报事件触发WPA开始SAE认证 */
typedef struct {
    ext_external_auth_action_enum auth_action;
    td_u8 bssid[WLAN_MAC_ADDR_LEN];
    td_u8 *ssid;
    td_u32 ssid_len;
    td_u32 key_mgmt_suite;
    td_u16 status;
    td_u8 pmkid[WLAN_PMKID_LEN];
}oal_external_auth_stru;
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
/* 驱动MESH上报WPA去关联 */
typedef struct {
    td_u16  reason_code;         /* 去关联 reason code */
    td_u8   auc_resv[2];            /* 2 保留字节 */

    td_u8  *pus_disconn_ie;        /* 去关联关联帧 ie */
    td_u32  disconn_ie_len;      /* 去关联关联帧 ie 长度 */
}oal_mesh_close_peer_stru;
#endif

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
osal_s32 wal_scan_comp_proc_sta(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_asoc_comp_proc_sta(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_disasoc_comp_proc_sta(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_connect_new_sta_proc_ap(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_disconnect_sta_proc_ap(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_send_mgmt_to_host(hmac_vap_stru *hmac_vap, frw_msg *msg);
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
osal_s32 wal_p2p_listen_timeout(hmac_vap_stru *hmac_vap, frw_msg *msg);
#endif

td_void wal_set_tpc_mode(td_u32 mode);
td_void wal_set_tpc_mode_add_3db(oal_bool_enum_uint8 flag, oal_bool_enum_uint8 under_ragulation_flag);
td_s16 *wal_get_mac_status_code(td_void);
#ifdef _PRE_WLAN_FEATURE_WPA3
osal_s32 wal_trigger_external_auth_sta(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_cfg80211_report_owe_info(hmac_vap_stru *hmac_vap, frw_msg *msg);
#endif
osal_u32 wal_report_tx_params(mac_cfg_ar_tx_params_stru *ar_tx_params);
osal_void wal_register_tx_params_callback(uapi_wifi_report_tx_params_callback func);
osal_s32 wal_upload_rx_mgmt_frame(hmac_vap_stru *hmac_vap, frw_msg *msg);
osal_s32 wal_upload_rx_promis_frame(hmac_vap_stru *hmac_vap, frw_msg *msg);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of wal_event.h */
