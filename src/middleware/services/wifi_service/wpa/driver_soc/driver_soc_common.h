/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: driver_soc_common.header
 */

#ifndef __DRIVER_SOC_COMMON_H__
#define __DRIVER_SOC_COMMON_H__

/*****************************************************************************
  1 Other header files included
*****************************************************************************/
#include "wifi_api.h"
#ifdef CONFIG_MESH
#include "soc_wifi_mesh_api.h"
#endif /* LOS_CONFIG_MESH */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 Basic data type definitions
*****************************************************************************/
typedef char int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

/*******************************************************************************
	Uncertain length, 32bits on 32-bit machine, 64bits on 64-bit machine
*******************************************************************************/
typedef signed long long int64;
typedef unsigned long long uint64;

/*****************************************************************************
  3 Macro definition
*****************************************************************************/
#define OAL_STATIC static
#define OAL_VOLATILE volatile
#ifdef INLINE_TO_FORCEINLINE
#define OAL_INLINE __forceinline
#else
#define OAL_INLINE inline
#endif

#define EXT_SUCC 0
#define EXT_EFAIL  1
#define EXT_EINVAL 22

#ifndef ETH_ADDR_LEN
#define ETH_ADDR_LEN 6
#endif

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN 32
#endif

#ifndef EXT_MAX_NR_CIPHER_SUITES
#define EXT_MAX_NR_CIPHER_SUITES 5
#endif

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

#ifndef EXT_MAX_NR_AKM_SUITES
#define EXT_MAX_NR_AKM_SUITES 2
#endif

#ifndef	EXT_PTR_NULL
#define	EXT_PTR_NULL NULL
#endif

#ifndef	SCAN_AP_LIMIT
#define	SCAN_AP_LIMIT 64
#endif

#ifndef   NETDEV_UP
#define   NETDEV_UP   0x0001
#endif
#ifndef   NETDEV_DOWN
#define   NETDEV_DOWN 0x0002
#endif

#ifndef   NOTIFY_DONE
#define   NOTIFY_DONE 0x0000
#endif

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
#define EXT_REKEY_OFFLOAD_KCK_LEN            16
#define EXT_REKEY_OFFLOAD_KEK_LEN            16
#define EXT_REKEY_OFFLOAD_REPLAY_COUNTER_LEN 8
#endif
/*****************************************************************************
  4 Enum definition
*****************************************************************************/
typedef enum {
	EXT_FALSE = 0,
	EXT_TRUE = 1,

	EXT_BUTT
} ext_bool_enum;
typedef uint8 ext_bool_enum_uint8;

#define EXT_KEYTYPE_DEFAULT_INVALID (-1)
typedef uint8 ext_iftype_enum_uint8;

typedef enum {
	EXT_KEYTYPE_GROUP,
	EXT_KEYTYPE_PAIRWISE,
	EXT_KEYTYPE_PEERKEY,

	NUM_EXT_KEYTYPES
} ext_key_type_enum;
typedef uint8 ext_key_type_enum_uint8;

typedef enum {
	EXT_KEY_DEFAULT_TYPE_INVALID,
	EXT_KEY_DEFAULT_TYPE_UNICAST,
	EXT_KEY_DEFAULT_TYPE_MULTICAST,

	NUM_EXT_KEY_DEFAULT_TYPES
} ext_key_default_types_enum;
typedef uint8 ext_key_default_types_enum_uint8;

typedef enum {
	EXT_NO_SSID_HIDING,
	EXT_HIDDEN_SSID_ZERO_LEN,
	EXT_HIDDEN_SSID_ZERO_CONTENTS
} ext_hidden_ssid_enum;
typedef uint8 ext_hidden_ssid_enum_uint8;

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
} ext_event_enum;
typedef uint8 ext_event_enum_uint8;

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
} ext_eloop_event_enum;
typedef uint8 ext_eloop_event_enum_uint8;

/**
 * Action to perform with external authentication request.
 * @EXT_EXTERNAL_AUTH_START: Start the authentication.
 * @EXT_EXTERNAL_AUTH_ABORT: Abort the ongoing authentication.
 */
typedef enum {
    EXT_EXTERNAL_AUTH_START,
    EXT_EXTERNAL_AUTH_ABORT,

    EXT_EXTERNAL_AUTH_BUTT,
} ext_external_auth_action_enum;
typedef uint8 ext_external_auth_action_enum_uint8;

typedef enum {
	EXT_MFP_NO,
	EXT_MFP_OPTIONAL,
	EXT_MFP_REQUIRED,
} ext_mfp_enum;
typedef uint8 ext_mfp_enum_uint8;
typedef uint8 ext_mesh_plink_state_enum_uint8;
typedef enum {
	EXT_AUTHTYPE_OPEN_SYSTEM = 0,
	EXT_AUTHTYPE_SHARED_KEY,
	EXT_AUTHTYPE_FT,
	EXT_AUTHTYPE_SAE,
	EXT_AUTHTYPE_NETWORK_EAP,
	/* keep last */
	EXT_AUTHTYPE_NUM,
	EXT_AUTHTYPE_MAX = (uint32)EXT_AUTHTYPE_NUM - 1,
	EXT_AUTHTYPE_AUTOMATIC,
	EXT_AUTHTYPE_BUTT
} ext_auth_type_enum;
typedef uint8 ext_auth_type_enum_uint8;

typedef enum {
	EXT_SCAN_SUCCESS,
	EXT_SCAN_FAILED,
	EXT_SCAN_REFUSED,
	EXT_SCAN_TIMEOUT
} ext_scan_status_enum;

/*****************************************************************************
  5 STRUCT definition
*****************************************************************************/
typedef struct {
	ext_scan_status_enum scan_status;
} ext_driver_scan_status_stru;

typedef struct {
	uint8 set;  // 0: del, 1: add
	wifi_extend_ie_index ie_index;
	uint8 bitmap;
	uint16 ie_len;
	uint8 *ie;
} ext_usr_app_ie_stru;

typedef struct {
	unsigned int cmd;
	const void *buf;
} ext_ioctl_command_stru;
typedef int32 (*ext_send_event_cb)(const char*, signed int, const unsigned char *, unsigned int);

typedef struct {
	int32  reassoc;
	size_t ielen;
	uint8  *ie;
	uint8  macaddr[ETH_ADDR_LEN];
	uint8  resv[2];
} ext_new_sta_info_stru;

typedef struct {
	uint8  *buf;
	uint32 len;
	int32  sig_mbm;
	int32  freq;
} ext_rx_mgmt_stru;

typedef struct {
	uint8                *buf;
	uint32               len;
	ext_bool_enum_uint8 ack;
	uint8                resv[3];
} ext_tx_status_stru;

typedef struct {
	uint32 freq;
	size_t data_len;
	uint8  *data;
	uint64 *send_action_cookie;
} ext_mlme_data_stru;

typedef struct {
	size_t head_len;
	size_t tail_len;
	uint8 *head;
	uint8 *tail;
} ext_beacon_data_stru;

typedef struct {
	uint8 *dst;
	uint8 *src;
	uint8 *bssid;
	uint8 *data;
	size_t data_len;
} ext_action_data_stru;

typedef struct {
	uint8                            *addr;
	ext_mesh_plink_state_enum_uint8 plink_state;
	uint8                            set;
	uint8                            mesh_bcn_priority;
	uint8                            mesh_is_mbr;
	uint8                            mesh_initiative_peering;
} ext_mesh_usr_params_stru;

typedef struct {
	uint8 enable_flag;
} ext_mesh_enable_flag_stru;

typedef struct {
	uint8  enable;
	uint16 timeout;
	uint8  resv;
} ext_delay_report_stru;

typedef struct {
	int32 mode;
	int32 freq;
	int32 channel;

	/* for HT */
	int32 ht_enabled;

	/* 0 = HT40 disabled, -1 = HT40 enabled,
	 * secondary channel below primary, 1 = HT40
	 * enabled, secondary channel above primary */
	int32 sec_channel_offset;

	/* for VHT */
	int32 vht_enabled;

	/* valid for both HT and VHT, center_freq2 is non-zero
	 * only for bandwidth 80 and an 80+80 channel */
	int32 center_freq1;
	int32 center_freq2;
	int32 bandwidth;
} ext_freq_params_stru;

typedef struct {
	int32                             type;
	uint32                            key_idx;
	uint32                            key_len;
	uint32                            seq_len;
	uint32                            cipher;
	uint8                             *addr;
	uint8                             *key;
	uint8                             *seq;
	ext_bool_enum_uint8              def;
	ext_bool_enum_uint8              defmgmt;
 	ext_key_default_types_enum_uint8 default_types;
	uint8                             resv;
} ext_key_ext_stru;

typedef struct {
	ext_freq_params_stru       freq_params;
	ext_beacon_data_stru       beacon_data;
	size_t                      ssid_len;
	int32                       beacon_interval;
	int32                       dtim_period;
	uint8                       *ssid;
	ext_hidden_ssid_enum_uint8 hidden_ssid;
	ext_auth_type_enum_uint8   auth_type;
	size_t                      mesh_ssid_len;
	uint8                       *mesh_ssid;
	int32 sae_pwe;
} ext_ap_settings_stru;

typedef struct {
	uint8                  bssid[ETH_ADDR_LEN];
	ext_iftype_enum_uint8 iftype;
	uint8                  resv;
} ext_set_mode_stru;

typedef struct {
	const uint8 *puc_buf;
	uint32 ul_len;
} ext_tx_eapol_stru;

typedef struct {
	uint8  *buf;
	uint32 len;
} ext_rx_eapol_stru;

typedef struct {
	void *callback;
	void *contex;
} ext_enable_eapol_stru;

typedef struct {
	uint16 channel;
	uint8  resv[2];
	uint32 freq;
	uint32 flags;
} ext_ieee80211_channel_stru;

typedef struct {
	int32                       channel_num;
	uint16                      bitrate[12];
	uint16                      ht_capab;
	uint8                       resv[2];
	ext_ieee80211_channel_stru iee80211_channel[14];
} ext_hw_feature_data_stru;

#ifdef CONFIG_ACS
enum nl80211_band {
    NL80211_BAND_2GHZ,
    NL80211_BAND_5GHZ,
};

enum ieee80211_band {
    IEEE80211_BAND_2GHZ = (uint32)NL80211_BAND_2GHZ,
    IEEE80211_BAND_5GHZ = (uint32)NL80211_BAND_5GHZ,

    /* keep last */
    IEEE80211_NUM_BANDS
};

typedef struct ieee80211_channel {
    enum ieee80211_band band;
    uint16         center_freq;
    uint16         hw_value;
    uint32         flags;
    int32          max_antenna_gain;
    int32          max_power;
    bool           beacon_found;
    uint8          resv[3];
    uint32         orig_flags;
    int32          orig_mag;
    int32          orig_mpwr;
} oal_ieee80211_channel_stru;
typedef struct ieee80211_channel            oal_ieee80211_channel;

typedef struct survey_info {
    oal_ieee80211_channel *channel;
    uint64 channel_time;
    uint64 channel_time_busy;
    uint64 channel_time_ext_busy;
    uint64 channel_time_rx;
    uint64 channel_time_tx;
    uint32 filled;
    int8   noise;
} oal_survey_info_stru;

typedef struct {
	uint32 ifidx;
	void (*add_survey)(void *sinfo, uint32 ifidx, void *survey_results);
	void *survey_results;
} ext_survey_results_stru;
#endif

#ifdef CONFIG_IEEE80211R
typedef struct {
    uint8 *ies;
    size_t ies_len;
    uint8 target_ap[ETH_ADDR_LEN];
    uint8 resv[2];
    uint8 *ric_ies;
    size_t ric_ies_len;
} ext_ft_event_stru;

typedef struct {
    uint16 md;
    const uint8 *ie;
    uint16 ie_len;
} ext_ft_ies_stru;
#endif /* CONFIG_IEEE80211R */

typedef struct {
	uint8  ssid[MAX_SSID_LEN];
	size_t ssid_len;
} ext_driver_scan_ssid_stru;

typedef struct {
	ext_driver_scan_ssid_stru *ssids;
	int32                      *freqs;
	uint8                      *extra_ies;
	uint8                      *bssid;
	uint8                      num_ssids;
	uint8                      num_freqs;
	uint8                      prefix_ssid_scan_flag;
	uint8                      fast_connect_flag;
	int32                      extra_ies_len;
	uint32                     acs_scan_flag;
} ext_scan_stru;

typedef struct {
	uint32 freq;
	uint32 duration;
} ext_on_channel_stru;

typedef struct {
	uint8 type;
} ext_if_add_stru;

typedef struct {
	int32 start;
	int32 duration;
	uint8 count;
	uint8 resv[3];
} ext_p2p_noa_stru;

typedef struct {
	int32 legacy_ps;
	int8  opp_ps;
	uint8 ctwindow;
	int8  resv[2];
} ext_p2p_power_save_stru;

typedef struct {
	uint8 ifname[IFNAMSIZ];
} ext_if_remove_stru;

typedef struct {
	uint8 type;
	uint8 mac_addr[ETH_ADDR_LEN];
	uint8 resv;
} ext_get_p2p_addr_stru;
typedef struct {
	ext_wifi_iftype iftype;
	uint8          *mac_addr;
} ext_iftype_mac_addr_stru;
typedef struct {
	uint64 drv_flags;
} ext_get_drv_flags_stru;

typedef struct {
	int32 freq;
} ext_ch_switch_stru;

/* The driver reports an event to trigger WPA to start SAE authentication. */
typedef struct {
	ext_external_auth_action_enum auth_action;
	uint8 bssid[ETH_ADDR_LEN];
	uint8 *ssid;
	uint32 ssid_len;
	uint32 key_mgmt_suite;
	uint16 status;
	uint8 *pmkid;
} ext_external_auth_stru;

typedef struct {
	uint8 peer[ETH_ADDR_LEN];
	uint16 status;
	uint8 *ie;
	uint32 ie_len;
} ext_update_dh_ie_stru;

typedef struct {
	uint32 wpa_versions;
	uint32 cipher_group;
	int32  n_ciphers_pairwise;
	uint32 ciphers_pairwise[EXT_MAX_NR_CIPHER_SUITES];
	int32  n_akm_suites;
	uint32 akm_suites[EXT_MAX_NR_AKM_SUITES];
	int32 sae_pwe;
} ext_crypto_settings_stru;

typedef struct {
	uint8                     *bssid;
	uint8                     *ssid;
	uint8                     *ie;
	uint8                     *key;
	uint8                     auth_type;
	uint8                     privacy;
	uint8                     key_len;
	uint8                     key_idx;
	uint8                     mfp;
	uint8                     auto_conn;
	uint8                     rsv[2];    /* 2: reserve 2 bytes */
	uint32                    freq;
	uint32                    ssid_len;
	uint32                    ie_len;
	ext_crypto_settings_stru *crypto;
} ext_associate_params_stru;

typedef struct {
	uint8  *req_ie;
	size_t req_ie_len;
	uint8  *resp_ie;
	size_t resp_ie_len;
	uint8  bssid[ETH_ADDR_LEN];
	uint8  rsv[2];
	uint16 status;
	uint16 freq;
} ext_connect_result_stru;

typedef struct {
	int32  flags;
	uint8  bssid[ETH_ADDR_LEN];
	int16  caps;
	int32  freq;
#ifndef EXT_SCAN_SIZE_CROP
	int16  beacon_int;
	int32  qual;
	uint32 beacon_ie_len;
#endif /* EXT_SCAN_SIZE_CROP */
	int32  level;
	uint32 age;
	uint32 ie_len;
	uint8  *variable;
} ext_scan_result_stru;

typedef struct {
	uint8  *ie;
	uint16 reason;
	uint8  rsv[2];
	uint32 ie_len;
} ext_disconnect_stru;
#ifdef CONFIG_WPS_AP
typedef struct _ext_app_ie_stru {
	uint32 ie_len;
	uint8  app_ie_type;
	uint8  rsv[3];
	uint8  *ie;
} ext_app_ie_stru;
#endif

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
typedef struct {
	uint8 kck[EXT_REKEY_OFFLOAD_KCK_LEN];
	uint8 kek[EXT_REKEY_OFFLOAD_KEK_LEN];
	uint8 replay_ctr[EXT_REKEY_OFFLOAD_REPLAY_COUNTER_LEN];
} ext_rekey_offload_stru;
#endif

typedef struct {
	uint8 peer_addr[ETH_ADDR_LEN];
	uint8 mesh_bcn_priority;
	uint8 mesh_is_mbr;
	int8  rssi;
	int8  reserved[3];
} ext_mesh_new_peer_candidate_stru;

typedef struct {
	uint8 bssid[ETH_ADDR_LEN];
	uint8 rsv[2];
	uint8 *pmkid;
} ext_pmkid_params;


/*****************************************************************************
  6 Function declaration
*****************************************************************************/
extern int32 drv_soc_register_send_event_cb(ext_send_event_cb func);
extern int32 drv_soc_hwal_wpa_ioctl(int8 *ifname, const ext_ioctl_command_stru *cmd);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* end of driver_soc_common.h */
