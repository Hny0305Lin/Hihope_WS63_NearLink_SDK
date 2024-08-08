/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: header file for wifi wpa api.
 */

#ifndef __SOC_WIFI_DRIVER_WPA_IF_H__
#define __SOC_WIFI_DRIVER_WPA_IF_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_net_cfg80211.h"
#include "osal_types.h"
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#ifndef ETH_ADDR_LEN
#define ETH_ADDR_LEN        6
#endif

#define EXT_SUCC            0
#define EXT_EFAIL           1
#define EXT_EINVAL         22

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN        32
#endif

#ifndef EXT_MAX_NR_CIPHER_SUITES
#define EXT_MAX_NR_CIPHER_SUITES    5
#endif

#ifndef EXT_WPAS_MAX_SCAN_SSIDS
#define EXT_WPAS_MAX_SCAN_SSIDS     2
#endif

#ifndef EXT_PTR_NULL
#define EXT_PTR_NULL  NULL
#endif

#ifndef EXT_MAX_NR_AKM_SUITES
#define EXT_MAX_NR_AKM_SUITES       2
#endif


#ifndef   NOTIFY_DONE
#define   NOTIFY_DONE            0x0000
#endif
#ifndef   NETDEV_UP
#define   NETDEV_UP              0x0001
#endif
#ifndef    SCAN_AP_LIMIT
#define    SCAN_AP_LIMIT            64
#endif
#ifndef   NETDEV_DOWN
#define   NETDEV_DOWN            0x0002
#endif

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
#define EXT_REKEY_OFFLOAD_KCK_LEN              16
#define EXT_REKEY_OFFLOAD_KEK_LEN              16
#define EXT_REKEY_OFFLOAD_REPLAY_COUNTER_LEN   8
#endif
#ifndef IFNAMSIZ
#define IFNAMSIZ  16
#endif

#define HI_WIFI_CCPRIV_ARGC_MAX   15

/*****************************************************************************
  4 枚举定义
*****************************************************************************/
typedef enum {
    EXT_FALSE   = 0,
    EXT_TRUE    = 1,

    EXT_BUTT
}ext_bool_enum;

typedef td_u8 ext_bool_enum_uint8;

typedef enum {
    EXT_IFTYPE_UNSPECIFIED,
    EXT_IFTYPE_ADHOC,
    EXT_IFTYPE_STATION,
    EXT_IFTYPE_AP,
    EXT_IFTYPE_AP_VLAN,
    EXT_IFTYPE_WDS,
    EXT_IFTYPE_MONITOR,
    EXT_IFTYPE_MESH_POINT,
    EXT_IFTYPE_P2P_CLIENT,
    EXT_IFTYPE_P2P_GO,
    EXT_IFTYPE_P2P_DEVICE,

    /* keep last */
    EXT_IFTYPES_NUM,
    EXT_IFTYPE_MAX = EXT_IFTYPES_NUM - 1
}ext_iftype_enum;
typedef td_u8 ext_iftype_enum_uint8;

typedef enum {
    EXT_KEYTYPE_GROUP,
    EXT_KEYTYPE_PAIRWISE,
    EXT_KEYTYPE_PEERKEY,

    NUM_EXT_KEYTYPES
}ext_key_type_enum;
typedef td_u8 ext_key_type_enum_uint8;

typedef enum {
    __EXT_KEY_DEFAULT_TYPE_INVALID,
    EXT_KEY_DEFAULT_TYPE_UNICAST,
    EXT_KEY_DEFAULT_TYPE_MULTICAST,

    NUM_EXT_KEY_DEFAULT_TYPES
}ext_key_default_types_enum;
typedef td_u8 ext_key_default_types_enum_uint8;

typedef enum {
    EXT_NO_SSID_HIDING,
    EXT_HIDDEN_SSID_ZERO_LEN,
    EXT_HIDDEN_SSID_ZERO_CONTENTS
}ext_hidden_ssid_enum;
typedef td_u8 ext_hidden_ssid_enum_uint8;

typedef enum {
    EXT_IOCTL_SET_AP = 0,
    EXT_IOCTL_NEW_KEY,
    EXT_IOCTL_DEL_KEY,
    EXT_IOCTL_SET_KEY,
    EXT_IOCTL_SEND_MLME,
    EXT_IOCTL_SEND_EAPOL,
    EXT_IOCTL_RECEIVE_EAPOL,
    EXT_IOCTL_ENALBE_EAPOL,
    EXT_IOCTL_DISABLE_EAPOL,
    HIIS_IOCTL_GET_ADDR,
    EXT_IOCTL_SET_MODE,
    EXT_IOCTL_GET_MODE,
    EXT_IOCTL_DEL_BEACON,
    HIIS_IOCTL_GET_HW_FEATURE,
    EXT_IOCTL_SCAN,
    EXT_IOCTL_DISCONNET,
    EXT_IOCTL_ASSOC,
    EXT_IOCTL_SET_NETDEV,
    EXT_IOCTL_CHANGE_BEACON,
    EXT_IOCTL_SET_REKEY_INFO,
    EXT_IOCTL_STA_REMOVE,
    EXT_IOCTL_SEND_ACTION,
    EXT_IOCTL_SET_MESH_USER,
    EXT_IOCTL_SET_MESH_GTK,
    EXT_IOCTL_EN_ACCEPT_PEER,
    EXT_IOCTL_EN_ACCEPT_STA,
    EXT_IOCTL_ADD_IF,
    EXT_IOCTL_PROBE_REQUEST_REPORT,
    EXT_IOCTL_REMAIN_ON_CHANNEL,
    EXT_IOCTL_CANCEL_REMAIN_ON_CHANNEL,
    EXT_IOCTL_SET_P2P_NOA,
    EXT_IOCTL_SET_P2P_POWERSAVE,
    EXT_IOCTL_SET_AP_WPS_P2P_IE,
    EXT_IOCTL_REMOVE_IF,
    EXT_IOCTL_GET_P2P_MAC_ADDR,
    EXT_IOCTL_GET_DRIVER_FLAGS,
    EXT_IOCTL_SET_USR_APP_IE,
    EXT_IOCTL_DELAY_REPORT,
    EXT_IOCTL_SEND_EXT_AUTH_STATUS,
    EXT_IOCTL_ADD_PMKID,
    EXT_IOCTL_DEL_PMKID,
    EXT_IOCTL_FLUSH_PMKID,
    EXT_IOCTL_GET_SURVEY,
    EXT_IOCTL_UPDATE_FT_IES,
    EXT_IOCTL_SET_CSI_CONFIG,
    EXT_IOCTL_CSI_SWITCH,
    EXT_IOCTL_UPDATE_DH_IE,
    EXT_IOCTL_WIFI_APP_SERVICE,
    HWAL_EVENT_BUTT
}ext_event_enum;
typedef td_u8 ext_event_enum_uint8;

typedef enum {
    EXT_ELOOP_EVENT_NEW_STA = 0,
    EXT_ELOOP_EVENT_DEL_STA,
    EXT_ELOOP_EVENT_RX_MGMT,
    EXT_ELOOP_EVENT_TX_STATUS,
    EXT_ELOOP_EVENT_SCAN_DONE,
    EXT_ELOOP_EVENT_SCAN_RESULT,
    EXT_ELOOP_EVENT_CONNECT_RESULT,
    EXT_ELOOP_EVENT_DISCONNECT,
    EXT_ELOOP_EVENT_MESH_CLOSE,
    EXT_ELOOP_EVENT_REMAIN_ON_CHANNEL,
    EXT_ELOOP_EVENT_CANCEL_REMAIN_ON_CHANNEL,
    EXT_ELOOP_EVENT_CHANNEL_SWITCH,
    EXT_ELOOP_EVENT_TIMEOUT_DISCONN,
    EXT_ELOOP_EVENT_EXTERNAL_AUTH,
    EXT_ELOOP_EVENT_FT_RESPONSE,
    EXT_ELOOP_EVENT_RX_CSI,
    EXT_ELOOP_EVENT_OWE_INFO,
    EXT_ELOOP_EVENT_BUTT
}ext_eloop_event_enum;
typedef td_u8 ext_eloop_event_enum_uint8;

typedef enum {
    EXT_MFP_NO,
    EXT_MFP_OPTIONAL,
    EXT_MFP_REQUIRED,
}ext_mfp_enum;
typedef td_u8 ext_mfp_enum_uint8;

typedef enum {
    EXT_AUTHTYPE_OPEN_SYSTEM = 0,
    EXT_AUTHTYPE_SHARED_KEY,
    EXT_AUTHTYPE_FT,
    EXT_AUTHTYPE_SAE,
    EXT_AUTHTYPE_NETWORK_EAP,

    /* keep last */
    EXT_AUTHTYPE_AUTOMATIC,

    EXT_AUTHTYPE_BUTT
}ext_auth_type_enum;
typedef td_u8 ext_auth_type_enum_uint8;

typedef enum {
    EXT_SCAN_SUCCESS = 0,
    EXT_SCAN_FAILED = 1,
    EXT_SCAN_REFUSED = 2,
    EXT_SCAN_TIMEOUT = 3,
} ext_scan_status_enum;
typedef td_u8 ext_scan_status_enum_uint8;

typedef enum  {
    EXT_EXTEND_IE1 = 7,
    EXT_EXTEND_IE2,
    EXT_EXTEND_IE3,
    EXT_EXTEND_IE4,
    EXT_EXTEND_IE_BUTT
} ext_extend_ie_index;

/**
 * Action to perform with external authentication request.
 * @EXT_EXTERNAL_AUTH_START: Start the authentication.
 * @EXT_EXTERNAL_AUTH_ABORT: Abort the ongoing authentication.
 */
typedef enum {
    EXT_EXTERNAL_AUTH_START,
    EXT_EXTERNAL_AUTH_ABORT,

    EXT_EXTERNAL_AUTH_BUTT,
}ext_external_auth_action_enum;
typedef td_u8 ext_external_auth_action_enum_uint8;

/*****************************************************************************
  5 全局变量声明
*****************************************************************************/
/*****************************************************************************
  6 消息头定义
*****************************************************************************/
/*****************************************************************************
  7 消息定义
*****************************************************************************/
/*****************************************************************************
  8 STRUCT定义
*****************************************************************************/
typedef struct _ext_driver_scan_status_stru {
    ext_scan_status_enum scan_status;
} ext_driver_scan_status_stru;

typedef struct _ext_ioctl_command_stru {
    td_u32      cmd;
    td_void    *buf;
}ext_ioctl_command_stru;

typedef td_s32 (*ext_send_event_cb)(const td_char*, td_s32, td_u8*, td_u32);

#ifdef _PRE_WLAN_FEATURE_P2P
typedef struct _ext_remain_on_channel_stru {
    td_s32 freq;
    td_s32 duration;
} ext_on_channel_stru;

typedef struct _ext_if_add_stru {
    td_s8 type;
} ext_if_add_stru;

typedef struct _ext_if_remove_stru {
    td_u8 ifname[IFNAMSIZ];
} ext_if_remove_stru;

typedef struct _ext_get_p2p_addr_stru {
    td_s8 type;
    td_u8 mac_addr[ETH_ADDR_LEN];
    td_u8 resv;
} ext_get_p2p_addr_stru;

typedef struct _ext_p2p_noa_stru {
    td_s32 start;
    td_s32 duration;
    td_u8  count;
    td_u8  resv[3]; /* 3 byte保留字段 */
} ext_p2p_noa_stru;

typedef struct _ext_p2p_power_save_stru {
    td_s32 legacy_ps;
    td_s8  opp_ps;
    td_u8  ctwindow;
    td_u8  resv[2]; /* 2 BYTE 保留字段 */
} ext_p2p_power_save_stru;
#endif

typedef struct _ext_get_drv_flags_stru {
    osal_u64 drv_flags;
} ext_get_drv_flags_stru;

typedef struct _ext_new_sta_info_stru {
    td_s32 l_reassoc;
    td_u32 ielen;
    td_u8 *ie;
    td_u8  macaddr[ETH_ADDR_LEN];
    td_u8  resv[2]; /* 2 BYTE保留字段 */
}ext_new_sta_info_stru;

typedef struct _ext_rx_mgmt_stru {
    td_u8 *buf;
    td_u32 len;
    td_s32 l_sig_mbm;
    td_u32 freq;
}ext_rx_mgmt_stru;

typedef struct _ext_tx_status_stru {
    td_u8               *buf;
    td_u32               len;
    ext_bool_enum_uint8 ack;
    td_u8                resv[3]; /* 3 BYTE保留字段 */
}ext_tx_status_stru;

typedef struct _ext_mlme_data_stru {
    td_u32      freq;
    td_u32      data_len;
    td_u8      *data;
    osal_u64     *pull_send_action_cookie;
}ext_mlme_data_stru;

typedef struct _ext_beacon_data_stru {
    td_u32      head_len;
    td_u32      tail_len;
    td_u8      *head;
    td_u8      *tail;
}ext_beacon_data_stru;

typedef struct _ext_action_data_stru {
    td_u8 *dst;
    td_u8 *src;
    td_u8 *bssid;
    td_u8 *data;
    td_u32 data_len;
}ext_action_data_stru;

#ifdef _PRE_WLAN_FEATURE_MESH
typedef struct _ext_set_mesh_user_data_stru {
    td_u8 *addr;
    td_u8 plink_state;
    td_u8 set;
    td_u8 bcn_prio; /* 上报lwip使用 */
    td_u8 is_mbr;
    td_u8 mesh_initiative_peering;
}ext_set_mesh_user_data_stru;

typedef struct _ext_enable_auto_peer_stru {
    td_u8 enable_auto_peer;
}ext_enable_auto_peer_stru;

typedef struct _ext_set_mesh_user_gtk_stru {
    td_u8 *addr;
    td_u8 *gtk;
    td_u8 gtk_len;
    td_u8 resv[3]; /* 3 BYTE保留字段 */
}ext_set_mesh_user_gtk_str;

typedef struct _ext_enable_accept_peer_stru {
    td_u8 enable_accept_peer;
}ext_enable_accept_peer_stru;

typedef struct _ext_enable_accept_sta_stru {
    td_u8 enable_accept_sta;
}ext_enable_accept_sta_stru;
#endif

/* 延时上报关联状态配置参数结构体 */
typedef struct _ext_delay_report_stru {
    td_u8   enable;     /* 配置开关 */
    td_u16  timeout;    /* 延时时间 */
    td_u8   resv;
}ext_delay_report_stru;

typedef struct _ext_freq_params_stru {
    td_s32 l_mode;
    td_s32 l_freq;
    td_s32 l_channel;

    /* for HT */
    td_s32 l_ht_enabled;

    /* 0 = HT40 disabled, -1 = HT40 enabled,
     * secondary channel below primary, 1 = HT40
     * enabled, secondary channel above primary */
    td_s32 l_sec_channel_offset;

    /* for VHT */
    td_s32 l_vht_enabled;

    /* valid for both HT and VHT, center_freq2 is non-zero
     * only for bandwidth 80 and an 80+80 channel */
    td_s32 l_center_freq1;
    td_s32 l_center_freq2;
    td_s32 l_bandwidth;
}ext_freq_params_stru;

/* 密钥信息设置数据传递结构体 */
typedef struct _ext_key_ext_stru {
    td_s32                          l_type;
    td_u32                          key_idx;
    td_u32                          key_len;
    td_u32                          seq_len;
    td_u32                          cipher;
    td_u8                          *addr;
    td_u8                          *key;
    td_u8                          *seq;
    ext_bool_enum_uint8                def;
    ext_bool_enum_uint8                defmgmt;
    ext_key_default_types_enum_uint8   default_types;
    td_u8                               resv;
}ext_key_ext_stru;

/* AP信息设置相关数据传递结构体 */
typedef struct _ext_ap_settings_stru {
    ext_freq_params_stru        freq_params;
    ext_beacon_data_stru        beacon_data;
    td_u32                       ssid_len;
    td_s32                       l_beacon_interval;
    td_s32                       l_dtim_period;
    td_u8                       *ssid;
    ext_hidden_ssid_enum_uint8  hidden_ssid;
    ext_auth_type_enum_uint8    auth_type;
    td_u8                        resv[2]; /* 2 BYTE 保留字段 */
    osal_u32                     mesh_ssid_len;
    osal_u8                      *mesh_ssid;
    osal_s32                     sae_pwe;
}ext_ap_settings_stru;

/* 设置模式结构体 */
typedef struct _ext_set_mode_stru {
    td_u8                   bssid[ETH_ADDR_LEN];
    ext_iftype_enum_uint8  iftype;
    td_u8                   resv;
}ext_set_mode_stru;

typedef struct _ext_tx_eapol_stru {
    td_u8          *buf;
    td_u32          len;
}ext_tx_eapol_stru;

typedef struct _ext_rx_eapol_stru {
    td_u8          *buf;
    td_u32          len;
}ext_rx_eapol_stru;

typedef struct _ext_enable_eapol_stru {
    td_void           *callback;
    td_void           *contex;
}ext_enable_eapol_stru;

typedef struct _ext_ieee80211_channel_stru {
    td_u16  channel;
    td_u8   resv[2]; /* 2 BYTE保留字段 */
    td_u32  freq;
    td_u32  flags;
}ext_ieee80211_channel_stru;

typedef struct _ext_hw_feature_data_stru {
    td_s32                      l_channel_num;
    td_u16                      aus_bitrate[12];          /* 占12 short */
    td_u16                      ht_capab;
    td_u8                       resv[2];                  /* 2 BYTE保留字段 */
    ext_ieee80211_channel_stru iee80211_channel[14]; /* 14维 */
}ext_hw_feature_data_stru;

#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
typedef struct _ext_survey_results {
    td_u32 ifidx;
    void (*add_survey)(void *sinfo, osal_u32 ifidx, void *survey_results);
    void *survey_results;
} ext_survey_results_stru;
#endif

typedef struct _ext_driver_scan_ssid_stru {
    td_u8                     ssid[MAX_SSID_LEN];
    td_u32                    ssid_len;
} ext_driver_scan_ssid_stru;

typedef struct _ext_scan_stru {
    ext_driver_scan_ssid_stru *ssids;
    td_s32                     *pl_freqs;
    td_u8                      *extra_ies;
    td_u8                      *bssid;
    td_u8                       num_ssids;
    td_u8                       num_freqs;
    td_u8                       prefix_ssid_scan_flag;
    td_u8                       fast_connect_flag;
    td_u32                      extra_ies_len;
#if defined(_PRE_WLAN_FEATURE_WS92_MERGE) && defined(_PRE_WLAN_FEATURE_ACS)
    td_u32                      acs_scan_flag;
#endif
}ext_scan_stru;

typedef struct  _ext_crypto_settings_stru {
    td_u32              wpa_versions;
    td_u32              cipher_group;
    td_s32               l_n_ciphers_pairwise;
    td_u32              ciphers_pairwise[EXT_MAX_NR_CIPHER_SUITES];
    td_s32               l_n_akm_suites;
    td_u32              akm_suites[EXT_MAX_NR_AKM_SUITES];
    enum nl80211_sae_pwe_mechanism sae_pwe;
}ext_crypto_settings_stru;

typedef struct _ext_associate_params_stru {
    td_u8                          *bssid;
    td_u8                          *ssid;
    td_u8                          *ie;
    td_u8                          *key;
    td_u8                           auth_type;
    td_u8                           privacy;
    td_u8                           key_len;
    td_u8                           key_idx;
    td_u8                           mfp;
    td_u8                           auto_conn;  /* 自动重关联标志 */
    td_u8                           resv[2];    /* 2 BYTE保留字段 */
    td_u32                          freq;
    td_u32                          ssid_len;
    td_u32                          ie_len;
    ext_crypto_settings_stru       *crypto;
} ext_associate_params_stru;

typedef struct _ext_connect_result_stru {
    td_u8              *req_ie;
    td_u32              req_ie_len;
    td_u8              *resp_ie;
    td_u32              resp_ie_len;
    td_u8               bssid[ETH_ADDR_LEN];
    td_u8               resv[2]; /* 2 BYTE保留字段 */
    td_u16              status;
    td_u16              freq;
} ext_connect_result_stru;

typedef struct _ext_scan_result_stru {
    td_s32             l_flags;
    td_u8              bssid[ETH_ADDR_LEN];
    td_s16             s_caps;
    td_s32             l_freq;
    td_s16             beacon_int;
    td_s32             qual;
    td_u32             beacon_ie_len;
    td_s32             l_level;
    td_u32             age;
    td_u32             ie_len;
    td_u8              *variable;
} ext_scan_result_stru;

typedef struct _ext_disconnect_stru {
    td_u8                  *ie;
    td_u16                  reason;
    td_u8                   rsv[2]; /* 2 BYTE保留字段 */
    td_u32                  ie_len;
} ext_disconnect_stru;

typedef struct _ext_app_ie_stru {
    td_u32              ie_len;
    td_u8               app_ie_type;
    td_u8               rsv[3]; /* 3 BYTE保留字段 */
    /* ie 中保存信息元素 */
    td_u8              *ie;
}ext_app_ie_stru;

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
typedef struct _ext_rekey_offload_stru {
    td_u8   kck[EXT_REKEY_OFFLOAD_KCK_LEN];
    td_u8   kek[EXT_REKEY_OFFLOAD_KEK_LEN];
    td_u8   replay_ctr[EXT_REKEY_OFFLOAD_REPLAY_COUNTER_LEN];
}ext_rekey_offload_stru;
#endif
#ifdef _PRE_WLAN_FEATURE_MESH
typedef struct _ext_mesh_close_peer_stru {
    td_u8  macaddr[ETH_ADDR_LEN];
    td_u16 reason;
} ext_mesh_close_peer_stru;

typedef struct _ext_mesh_new_peer_candidate_stru {
    td_u8 peer_addr[ETH_ADDR_LEN];
    td_u8 bcn_prio;
    td_u8 is_mbr;
    td_s8 rssi;
    td_u8 reserved[3]; /* 3 BYTE保留字段 */
}ext_mesh_new_peer_candidate_stru;
#endif

typedef struct {
    td_u8 set;  // 0: del, 1: add
    ext_extend_ie_index ie_index;
    td_u8 bitmap;
    td_u16 ie_len;
    td_u8 *ie;
} ext_usr_app_ie_stru;

typedef struct _ext_ch_switch_stru {
    td_s32 freq;
} ext_ch_switch_stru;

#ifdef _PRE_WLAN_FEATURE_WPA3
/* The driver reports an event to trigger WPA to start SAE authentication. */
/* 驱动上报事件触发WPA开始SAE认证 */
typedef struct {
    ext_external_auth_action_enum auth_action;
    td_u8 bssid[ETH_ADDR_LEN];
    td_u8 *ssid;
    td_u32 ssid_len;
    td_u32 key_mgmt_suite;
    td_u16 status;
    td_u8 *pmkid;
}ext_external_auth_stru;

typedef struct {
    td_u8 peer[ETH_ADDR_LEN];
    td_u16 status;
    td_u8 *ie;
    td_u32 ie_len;
} ext_update_dh_ie_stru;

#endif
typedef struct {
    td_u8 bssid[ETH_ADDR_LEN];
    td_u8 rsv[2];
    td_u8 *pmkid;
} ext_pmkid_params;

/*****************************************************************************
  9 UNION定义
*****************************************************************************/
/*****************************************************************************
  10 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  11 函数声明
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of soc_wifi_driver_wpa_if.h */
