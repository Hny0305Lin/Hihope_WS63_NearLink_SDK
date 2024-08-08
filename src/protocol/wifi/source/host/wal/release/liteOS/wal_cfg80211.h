/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: Header file for wal_cfg80211.c.
 */

#ifndef __WAL_CFG80211_H__
#define __WAL_CFG80211_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "wal_main.h"
#include "hmac_ext_if.h"
#include "wal_ioctl.h"
#include "wal_ccpriv.h"
#include "wal_scan.h"
#include "oal_net.h"
#include "soc_wifi_driver_wpa_if.h"
#ifdef _PRE_WLAN_FEATURE_P2P
#include "hmac_p2p.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define     WAL_MAX_SCAN_TIME_PER_CHANNEL  400

#define     WAL_MAX_SCAN_TIME_PER_SCAN_REQ (5 * 1000)      /* wpa_s下发扫描请求，超时时间为5s，单位为ms */

/* channel index and frequence */
#define WAL_MIN_CHANNEL_2G      1
#define WAL_MAX_CHANNEL_2G      14

#define WAL_MIN_FREQ_2G    (2412 + 5*(WAL_MIN_CHANNEL_2G - 1))
#ifdef _PRE_WLAN_FEATURE_015CHANNEL_EXP
#define WAL_MAX_FREQ_2G    2512
#else
#define WAL_MAX_FREQ_2G    2484
#endif

/* wiphy 结构体初始化变量 */
#define WAL_MAX_SCAN_IE_LEN        1000
/* 802.11n HT 能力掩码 */
#define IEEE80211_HT_CAP_LDPC_CODING        0x0001
#define IEEE80211_HT_CAP_SUP_WIDTH_20_40    0x0002
#define IEEE80211_HT_CAP_SM_PS          0x000C
#define IEEE80211_HT_CAP_SM_PS_SHIFT    2
#define IEEE80211_HT_CAP_GRN_FLD        0x0010
#define IEEE80211_HT_CAP_SGI_20         0x0020
#define IEEE80211_HT_CAP_SGI_40         0x0040
#define IEEE80211_HT_CAP_TX_STBC        0x0080
#define IEEE80211_HT_CAP_RX_STBC        0x0300
#define IEEE80211_HT_CAP_DELAY_BA       0x0400
#define IEEE80211_HT_CAP_MAX_AMSDU      0x0800
#define IEEE80211_HT_CAP_DSSSCCK40      0x1000
#define IEEE80211_HT_CAP_RESERVED       0x2000
#define IEEE80211_HT_CAP_40MHZ_INTOLERANT   0x4000
#define IEEE80211_HT_CAP_LSIG_TXOP_PROT     0x8000

/* 802.11ac VHT Capabilities */
#define IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_3895            0x00000000
#define IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_7991            0x00000001
#define IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_11454            0x00000002
#define IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ        0x00000004
#define IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ    0x00000008
#define IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_MASK            0x0000000C
#define IEEE80211_VHT_CAP_RXLDPC                0x00000010
#define IEEE80211_VHT_CAP_SHORT_GI_80                0x00000020
#define IEEE80211_VHT_CAP_SHORT_GI_160                0x00000040
#define IEEE80211_VHT_CAP_TXSTBC                0x00000080
#define IEEE80211_VHT_CAP_RXSTBC_1                0x00000100
#define IEEE80211_VHT_CAP_RXSTBC_2                0x00000200
#define IEEE80211_VHT_CAP_RXSTBC_3                0x00000300
#define IEEE80211_VHT_CAP_RXSTBC_4                0x00000400
#define IEEE80211_VHT_CAP_RXSTBC_MASK                0x00000700
#define IEEE80211_VHT_CAP_SU_BEAMFORMER_CAPABLE            0x00000800
#define IEEE80211_VHT_CAP_SU_BEAMFORMEE_CAPABLE            0x00001000
#define IEEE80211_VHT_CAP_BEAMFORMER_ANTENNAS_MAX        0x00006000
#define IEEE80211_VHT_CAP_SOUNDING_DIMENSIONS_MAX        0x00030000
#define IEEE80211_VHT_CAP_MU_BEAMFORMER_CAPABLE            0x00080000
#define IEEE80211_VHT_CAP_MU_BEAMFORMEE_CAPABLE            0x00100000
#define IEEE80211_VHT_CAP_VHT_TXOP_PS                0x00200000
#define IEEE80211_VHT_CAP_HTC_VHT                0x00400000
#define IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_SHIFT    23
#define IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MASK    \
        (7 << IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_SHIFT)
#define IEEE80211_VHT_CAP_VHT_LINK_ADAPTATION_VHT_UNSOL_MFB    0x08000000
#define IEEE80211_VHT_CAP_VHT_LINK_ADAPTATION_VHT_MRQ_MFB    0x0c000000
#define IEEE80211_VHT_CAP_RX_ANTENNA_PATTERN            0x10000000
#define IEEE80211_VHT_CAP_TX_ANTENNA_PATTERN            0x20000000

/* management */
#define IEEE80211_STYPE_ASSOC_REQ       0x0000
#define IEEE80211_STYPE_ASSOC_RESP      0x0010
#define IEEE80211_STYPE_REASSOC_REQ     0x0020
#define IEEE80211_STYPE_REASSOC_RESP    0x0030
#define IEEE80211_STYPE_PROBE_REQ       0x0040
#define IEEE80211_STYPE_PROBE_RESP      0x0050
#define IEEE80211_STYPE_BEACON          0x0080
#define IEEE80211_STYPE_ATIM            0x0090
#define IEEE80211_STYPE_DISASSOC        0x00A0
#define IEEE80211_STYPE_AUTH            0x00B0
#define IEEE80211_STYPE_DEAUTH          0x00C0
#define IEEE80211_STYPE_ACTION          0x00D0

#define WAL_COOKIE_ARRAY_SIZE           8       /* 采用8bit 的map 作为保存cookie 的索引状态 */
#define WAL_MGMT_TX_TIMEOUT_MSEC        500     /* WAL 发送管理帧超时时间 */
#define WAL_MGMT_TX_RETRY_CNT           8       /* WAL 发送管理帧最大重传次数 */

#define IEEE80211_FCTL_FTYPE            0x000c
#define IEEE80211_FCTL_STYPE            0x00f0
#define IEEE80211_FTYPE_MGMT            0x0000

#define WAL_GET_STATION_THRESHOLD 1000 /* 固定时间内允许一次抛事件读DMAC RSSI */

typedef struct cookie_arry {
    osal_u64  ull_cookie;
    td_u32  record_time;
    td_u32  reserved;
}cookie_arry_stru;

#define ratetab_ent(_rate, _rateid, _flags)     \
{                                                               \
    .bitrate        = (_rate),                                  \
    .hw_value       = (_rateid),                                \
    .flags          = (_flags),                                 \
}

#define chan2g(_channel, _freq, _flags)  \
{                       \
    .band                   = OAL_IEEE80211_BAND_2GHZ,          \
    .center_freq            = (_freq),                      \
    .hw_value               = (_channel),                   \
    .flags                  = (_flags),                     \
    .max_antenna_gain       = 0,                            \
    .max_power              = 30,                           \
}

#define WAL_MIN_RTS_THRESHOLD 256
#define WAL_MAX_RTS_THRESHOLD 0xFFFF

#define WAL_MAX_FRAG_THRESHOLD 7536
#define WAL_MIN_FRAG_THRESHOLD 256

#define WAL_MAX_WAIT_TIME 3000

#if defined(_PRE_OS_VERSION_LITEOS) && defined(_PRE_WLAN_FEATURE_11R)
typedef struct {
    osal_u16 md;
    const osal_u8 *ie;
    osal_u16 ie_len;
} oal_ft_ies_stru;
#endif

#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
typedef struct survey_info {
    oal_ieee80211_channel *channel;
    osal_u64 channel_time;
    osal_u64 channel_time_busy;
    osal_u64 channel_time_ext_busy;
    osal_u64 channel_time_rx;
    osal_u64 channel_time_tx;
    osal_u32 filled;
    osal_s8   noise;
} oal_survey_info_stru;

enum survey_info_flags {
    SURVEY_INFO_NOISE_DBM,
    SURVEY_INFO_IN_USE,
    SURVEY_INFO_CHANNEL_TIME,
    SURVEY_INFO_CHANNEL_TIME_BUSY,
    SURVEY_INFO_CHANNEL_TIME_EXT_BUSY,
    SURVEY_INFO_CHANNEL_TIME_RX,
    SURVEY_INFO_CHANNEL_TIME_TX
};

#define SURVEY_HAS_NF             (1U << 0)
#define SURVEY_HAS_CHAN_TIME      (1U << 1)
#define SURVEY_HAS_CHAN_TIME_BUSY (1U << 2)
#define SURVEY_HAS_CHAN_TIME_RX   (1U << 3)
#define SURVEY_HAS_CHAN_TIME_TX   (1U << 4)
#endif
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

/*****************************************************************************
 函 数 名  : oal_ieee80211_is_probe_resp
*****************************************************************************/
static inline td_u32 oal_ieee80211_is_probe_resp(td_u16 fc)
{
    return (fc &  (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
           (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_RESP);
}
#ifndef _PRE_WLAN_FEATURE_MFG_ONLY
osal_s32 wal_cfg80211_mgmt_tx_status(hmac_vap_stru *hmac_vap, frw_msg *msg);
#endif
td_void wal_cfg80211_reset_bands(td_void);
td_u32 wal_cfg80211_add_vap(const hmac_cfg_add_vap_param_stru *add_vap_param);
td_u32 wal_cfg80211_set_default_key(oal_net_device_stru *netdev, td_u8 key_index, td_bool unicast,
    td_bool multicast);
td_u32 wal_cfg80211_add_key(oal_net_device_stru *netdev, const cfg80211_add_key_info_stru *cfg80211_add_key_info,
    const td_u8 *mac_addr, oal_key_params_stru *params);
td_u32 wal_cfg80211_remove_key(oal_net_device_stru *netdev, td_u8 key_index, td_bool pairwise, const td_u8 *mac_addr);
td_u32 wal_cfg80211_p2p_mgmt_tx(oal_wireless_dev *wdev, oal_ieee80211_channel *chan, ext_mlme_data_stru *data,
    osal_u64 *pull_cookie);
td_u32 wal_cfg80211_mgmt_tx(oal_net_device_stru *netdev, oal_ieee80211_channel *chan, ext_mlme_data_stru *mlme_data);
td_u32 wal_cfg80211_intf_mode_check(oal_net_device_stru *netdev, nl80211_iftype_uint8 type);
td_u32 wal_cfg80211_scan(oal_cfg80211_scan_request_stru *request, hmac_scan_stru *scan_mgmt);
td_s32 uapi_wifi_app_service(const td_char *ifname, td_void *buf);
td_u32 wal_cfg80211_del_station(oal_net_device_stru *netdev, const td_u8 *mac_addr);
td_u32 wal_cfg80211_start_ap(oal_net_device_stru *netdev, oal_ap_settings_stru *ap_settings);
td_u32 wal_cfg80211_change_beacon(oal_net_device_stru *netdev, oal_beacon_data_stru *beacon_info);
td_u32 wal_cfg80211_disconnect(oal_net_device_stru *netdev, td_u16 reason_code);
td_u32 wal_cfg80211_connect(oal_net_device_stru *netdev, oal_cfg80211_connect_params_stru *sme);
#ifdef _PRE_WLAN_FEATURE_PNO_SCAN
td_u32 wal_cfg80211_start_sched_scan(oal_net_device_stru *netdev, mac_pno_scan_stru *pno_scan_info);
#endif
td_u32 wal_cfg80211_set_pmksa(oal_net_device_stru *netdev, const ext_pmkid_params *pmkid_params);
#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
osal_s32 wal_cfg80211_get_survey_etc(oal_net_device_stru *netdev, osal_u32 ifidx, oal_survey_info_stru *sinfo);
#endif
osal_s32 wal_report_csa_done(hmac_vap_stru *hmac_vap, frw_msg *msg);
#ifdef _PRE_WLAN_FEATURE_ROAM
osal_s32  wal_roam_comp_proc_sta_etc(hmac_vap_stru *hmac_vap, frw_msg *msg);
#ifdef _PRE_WLAN_FEATURE_11R
osal_s32 wal_cfg80211_update_ft_ies(oal_net_device_stru *netdev, oal_ft_ies_stru *fties);
osal_s32 wal_ft_event_proc_sta_etc(hmac_vap_stru *hmac_vap, frw_msg *msg);
#endif
#endif

td_u32 wal_cfg80211_remove_pmksa(oal_net_device_stru *netdev, const ext_pmkid_params *pmkid_params);
td_u32 wal_cfg80211_flush_pmksa(oal_net_device_stru *netdev);
#ifdef _PRE_WLAN_FEATURE_P2P
td_u32 wal_del_p2p_group(const hmac_device_stru *hmac_device);
td_u32 wal_cfg80211_remain_on_channel(const oal_wireless_dev *wdev, const struct ieee80211_channel *chan,
    td_u32 duration, osal_u64 *pull_cookie);
td_u32 wal_cfg80211_cancel_remain_on_channel(const oal_wireless_dev *wdev, osal_u64 ull_cookie);
#endif

oal_ieee80211_channel* wal_get_g_wifi_2ghz_channels(td_void);
oal_ieee80211_supported_band* wal_get_wifi_2ghz_band(td_void);
oal_ieee80211_channel* wal_cfg80211_get_channel(td_s32 freq);

td_u32 wal_cfg80211_del_beacon(oal_net_device_stru *netdev);
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of wal_cfg80211.h */
