/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: wifi APIs
 */

#ifndef _WIFI_API_H_
#define _WIFI_API_H_

#include "td_type.h"
#include "td_base.h"
#include "soc_wifi_api.h"
#include "service_wifi_api.h"
#include "config_ssid.h"
#include "soc_osal.h"
#ifdef LOS_CONFIG_MESH
#include "soc_wifi_mesh_api.h"
#endif /* LOS_CONFIG_MESH */
#ifdef LOS_CONFIG_P2P
#include "soc_wifi_p2p_api.h"
#endif
#include <stdbool.h>

#define WPA_MIN_KEY_LEN                 8
#define WPA_MAX_KEY_LEN                 64
#define WAPI_MAX_KEY_LEN                65
#define WPA_HT_CAPA_LEN                 20
#define WPA_MAX_SSID_LEN                32
#define WPA_MAX_ESSID_LEN               WPA_MAX_SSID_LEN
#define WPA_MIN_FREQ_LEN                6
#define WPA_MAX_FREQ_LEN                10
#define WPA_AP_MIN_BEACON               25
#define WPA_AP_MAX_BEACON               1000
#define WPA_24G_FREQ_TXT_LEN            4
#define WPA_WEP40_KEY_LEN               5
#define WPA_WEP104_KEY_LEN              13
#define WPA_WEP40_HEX_KEY_LEN           10
#define WPA_WEP104_HEX_KEY_LEN          26
#define WPA_AP_MAX_DTIM                 30
#define WPA_AP_MIN_DTIM                 1
#define WPA_MAX_REKEY_TIME              86400
#define WPA_MIN_REKEY_TIME              30
#define WPA_DOUBLE_IFACE_WIFI_DEV_NUM   2
#define WPA_BASE_WIFI_DEV_NUM           1
#define WPA_MAX_WIFI_DEV_NUM            2
#define WPA_NETWORK_ID_TXT_LEN          5
#define WPA_CTRL_CMD_LEN                256
#define WPA_CMD_BUF_SIZE                64
#define WPA_MIN(x, y)                   (((x) < (y)) ? (x) : (y))
#define WPA_STR_LEN(x)                  (strlen(x) + 1)
#define WPA_EXTERNED_SSID_LEN           (WPA_MAX_SSID_LEN * 5)
#define WPA_CMD_BSSID_LEN               32
#define WPA_CMD_MIN_SIZE                16
#define WPA_MAX_NUM_STA                 8
#define WPA_SSID_SCAN_PREFEX_ENABLE     1
#define WPA_SSID_SCAN_PREFEX_DISABEL    0
#define WPA_P2P_INTENT_LEN              13
#define WPA_P2P_JOIN_LEN                5
#define WPA_P2P_WPS_LEN                 3
#define WPA_INT_TO_CHAR_LEN             11
#define WPA_P2P_LISTEN_LEN              20
#define WPA_P2P_PEER_CMD_LEN            27
#define WPA_P2P_MIN_INTENT              0
#define WPA_P2P_MAX_INTENT              15
#define WIFI_EVENT_DELAY_5S             5000
#define WPA_BIT0                        (1U << 0)
#define WPA_BIT1                        (1U << 1)
#define WPA_BIT2                        (1U << 2)
#define WPA_BIT3                        (1U << 3)
#define WPA_BIT4                        (1U << 4)
#define WPA_BIT5                        (1U << 5)
#define WPA_BIT6                        (1U << 6)
#define WPA_BIT7                        (1U << 7)
#define WPA_BIT8                        (1U << 8)
#define WPA_BIT9                        (1U << 9)
#define WPA_BIT10                       (1U << 10)
#define WPA_BIT11                       (1U << 11)
#define WPA_BIT12                       (1U << 12)
#define WPA_BIT13                       (1U << 13)
#define WPA_BIT14                       (1U << 14)
#define WPA_BIT15                       (1U << 15)
#define WPA_BIT16                       (1U << 16)
#define WPA_BIT17                       (1U << 17)
#define WPA_BIT18                       (1U << 18)
#define WPA_BIT19                       (1U << 19)
#define WPA_BIT20                       (1U << 20)
#define WPA_BIT21                       (1U << 21)
#define WPA_BIT22                       (1U << 22)
#define WPA_BIT23                       (1U << 23)
#define WPA_BIT24                       (1U << 24)
#define WPA_BIT26                       (1U << 26)
#define WPA_BIT27                       (1U << 27)
#define WPA_BIT28                       (1U << 28)
#define WPA_BIT29                       (1U << 29)
#define WPA_BIT30                       (1U << 30)

#define WPA_P2P_SCAN_MAX_CMD            32
#define WPA_P2P_IFNAME_MAX_LEN          10
#define WPA_P2P_DEFAULT_PERSISTENT      0
#define WPA_P2P_GROUP_MAX_LEN           24

#define MESH_AP                         1
#define MESH_STA                        0

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN                    32
#endif
#define MAX_DRIVER_NAME_LEN             16
#define WPA_MAX_SSID_KEY_INPUT_LEN      128
#define WPA_TXT_ADDR_LEN                17
#define WPA_INVITE_ADDR_LEN             23
#define WPA_INVITE_PERSISTENT_ID        13
#define WPA_STA_PMK_LEN                 32
#define WPA_STA_ITERA                   4096
#define WPA_MAX_TRY_FREQ_SCAN_CNT       3
#define WPA_24G_CHANNEL_NUMS            14

#define WPA_ALIGN_SIZE                  4

#define WPA_COUNTRY_CODE_LEN            3
#define WPA_COUNTRY_CODE_USA            "US"
#define WPA_COUNTRY_CODE_ZZ             "ZZ" // ZZ : support channel 14
#define WPA_COUNTRY_CODE_JAPAN          "JP"

#define WPA_CHANNEL_MAX_USA             11
#define WPA_CHANNEL_MAX_JAPAN           14
#define WPA_CHANNEL_MAX_OTHERS          13
#define SAE_GROUPS_MAX_NUM             4

// LiteOS  : BIT25 and BIT0 should not be used.
// FreeRTOS: BIT31 - BIT24 should not be used.
#define WPA_EVENT_AP_DEAUTH_FAIL              WPA_BIT1
#define WPA_EVENT_AP_DEAUTH_OK                WPA_BIT2
#define WPA_EVENT_AP_DEAUTH_FLAG              (WPA_BIT1 | WPA_BIT2)
#define WPA_EVENT_AP_SHOW_STA_OK              WPA_BIT3
#define WPA_EVENT_AP_SHOW_STA_ERROR           WPA_BIT4
#define WPA_EVENT_AP_SHOW_STA_FLAG            (WPA_BIT3 | WPA_BIT4)
#define WPA_EVENT_AP_STOP_OK                  WPA_BIT5

#define WPA_EVENT_GET_SCAN_RESULT_OK          WPA_BIT1
#define WPA_EVENT_GET_SCAN_RESULT_ERROR       WPA_BIT2
#define WPA_EVENT_GET_SCAN_RESULT_FLAG        (WPA_BIT1 | WPA_BIT2)
#define WPA_EVENT_SCAN_RESULT_FREE_OK         WPA_BIT3
#define WPA_EVENT_STA_STOP_OK                 WPA_BIT4
#define WPA_EVENT_STA_STOP_ERROR              WPA_BIT5
#define WPA_EVENT_STA_STOP_FLAG               (WPA_BIT4 | WPA_BIT5)
#define WPA_EVENT_STA_RM_NETWORK_OK           WPA_BIT6
#define WPA_EVENT_STA_STATUS_OK               WPA_BIT7
#define WPA_EVENT_STA_STATUS_FAIL             WPA_BIT8
#define WPA_EVENT_STA_STATUS_FLAG             (WPA_BIT7 | WPA_BIT8)
#define WPA_EVENT_QUICK_CONNECT_OK            WPA_BIT9
#define WPA_EVENT_QUICK_CONNECT_ERROR         WPA_BIT10
#define WPA_EVENT_QUICK_CONNECT_FLAG          (WPA_BIT9 | WPA_BIT10)
#define WPA_EVENT_SCAN_OK                     WPA_BIT11
#define WPA_EVENT_WPA_START_OK                WPA_BIT12
#define WPA_EVENT_WPA_START_ERROR             WPA_BIT13
#define WPA_EVENT_WPA_START_FLAG              (WPA_BIT12 | WPA_BIT13)
#define WPA_EVENT_ADD_IFACE_OK                WPA_BIT14
#define WPA_EVENT_ADD_IFACE_ERROR             WPA_BIT15
#define WPA_EVENT_ADD_IFACE_FLAG              (WPA_BIT14 | WPA_BIT15)
#define WPA_EVENT_REMOVE_IFACE_FLAG           WPA_BIT16
#define WPA_EVENT_SCAN_RESULT_CLEAR_OK        WPA_BIT17
#define WPA_EVENT_STA_WNM_BSS_QUERY           WPA_BIT18
#define WPA_EVENT_STA_WNM_NOTIFY              WPA_BIT19

#define WPA_EVENT_P2P_GET_STATUS_OK           WPA_BIT1
#define WPA_EVENT_P2P_GET_STATUS_ERROR        WPA_BIT2
#define WPA_EVENT_P2P_GET_STATUS_FLAG         (WPA_BIT1 | WPA_BIT2)
#define WPA_EVENT_P2P_GET_PEER_OK             WPA_BIT3
#define WPA_EVENT_P2P_GET_PEER_ERROR          WPA_BIT4
#define WPA_EVENT_P2P_GET_PEER_FLAG           (WPA_BIT3 | WPA_BIT4)
#define WPA_EVENT_P2P_GET_PERSISTENT_GROUP_OK     WPA_BIT5
#define WPA_EVENT_P2P_GET_PERSISTENT_GROUP_ERROR  WPA_BIT6
#define WPA_EVENT_P2P_GET_PERSISTENT_GROUP_FLAG   (WPA_BIT5 | WPA_BIT6)

enum ext_scan_record_flag {
    EXT_SCAN_UNSPECIFIED,
    EXT_SCAN,
    EXT_CHANNEL_SCAN,
    EXT_SSID_SCAN,
    EXT_PREFIX_SSID_SCAN,
    EXT_BSSID_SCAN,
};

typedef enum {
    SOC_WPA_RM_NETWORK_END = 0,
    SOC_WPA_RM_NETWORK_START,
    SOC_WPA_RM_NETWORK_WORKING
} wpa_rm_network;

typedef enum {
    SOC_WPA_BIT_SCAN_UNKNOW        = 0,
    SOC_WPA_BIT_SCAN_SSID          = WPA_BIT0,
    SOC_WPA_BIT_SCAN_BSSID         = WPA_BIT1,
} wpa_scan_type;

struct wpa_assoc_request {
    /* ssid input before ssid txt parsing */
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];
    char key[WPA_MAX_SSID_KEY_INPUT_LEN + 1];
    unsigned char bssid[EXT_WIFI_MAC_LEN];
    ext_wifi_auth_mode auth;
    u8 channel;
    ext_wifi_pairwise wpa_pairwise;
    unsigned char scan_type;
    unsigned char hex_flag;
    ext_wifi_wpa_psk_usage_type psk_flag;
    unsigned char ft_flag;
    unsigned char resv[3];
};

struct ext_scan_record {
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];
    unsigned char bssid[EXT_WIFI_MAC_LEN];
    unsigned char chan_list[WIFI_24G_CHANNEL_NUMS];
    unsigned char chan_num;
    enum ext_scan_record_flag flag;
};

struct wpa_scan_params {
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];
    int  ssid_len;
    unsigned char chan_list[WIFI_24G_CHANNEL_NUMS];
    unsigned char chan_num;
    enum ext_scan_record_flag flag;
    unsigned char bssid[EXT_WIFI_MAC_LEN];
};
typedef enum ext_iftype {
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
    NUM_EXT_IFTYPES,
} wifi_iftype;

enum ext_mesh_enable_flag_type {
    EXT_MESH_ENABLE_ACCEPT_PEER,
    EXT_MESH_ENABLE_ACCEPT_STA,
    EXT_MESH_ENABLE_FLAG_BUTT
};

struct wifi_ap_opt_set {
    protocol_mode_enum hw_mode;
    int short_gi_off;
    int beacon_period;
    int dtim_period;
    int wpa_group_rekey;
    enum mfp_options ieee80211w;
#ifdef CONFIG_HOSTAPD_WPA3
    int sae_pwe;
    int transition;
    int clog_threshold;
    int sae_groups[SAE_GROUPS_MAX_NUM];
#endif /* CONFIG_HOSTAPD_WPA3  */
};

struct wifi_mesh_opt_set {
    int beacon_period;
    int dtim_period;
    int wpa_group_rekey;
};

struct wifi_sta_opt_set {
    protocol_mode_enum hw_mode;
    wifi_pmf_option_enum pmf;
    int  usr_pmf_set_flag;
#ifdef CONFIG_WPA3
    int sae_pwe;
    int sae_groups[SAE_GROUPS_MAX_NUM];
#endif
};

struct wifi_reconnect_set {
    int enable;
    unsigned int timeout;
    unsigned int period;
    unsigned int max_try_count;
    unsigned int try_count;
    unsigned int try_freq_scan_count;
    int pending_flag;
    struct wpa_ssid *current_ssid;
};

struct ext_wifi_dev {
    ext_wifi_iftype iftype;
    void *priv;
    int network_id;
    int ifname_len;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];
    char reserve[1];
};

struct hostapd_conf {
    unsigned char bssid[ETH_ALEN];
    char resv1[WPA_ALIGN_SIZE - ETH_ALEN % WPA_ALIGN_SIZE];
    char ssid[EXT_WIFI_MAX_SSID_LEN + 1];
    char resv2[WPA_ALIGN_SIZE - 1];
    unsigned int channel_num;
    int wpa_key_mgmt;
    int wpa_pairwise;
    ext_wifi_auth_mode authmode;
    unsigned char key[WPA_MAX_KEY_LEN + 1];
    char resv3[WPA_ALIGN_SIZE - 1];
    int auth_algs;
    unsigned char wep_idx;
    char resv4[WPA_ALIGN_SIZE - 1];
    int wpa;
    char driver[MAX_DRIVER_NAME_LEN];
    int ignore_broadcast_ssid;
};

typedef enum {
    WIFI_EXTEND_IE1 = 7, // same wifi driver
    WIFI_EXTEND_IE2,
    WIFI_EXTEND_IE3,
    WIFI_EXTEND_IE4,
    WIFI_EXTEND_IE_BUTT
} wifi_extend_ie_index;

typedef struct hostapd_conf wifi_softap_config;

typedef enum {
    WIFI_CSI_SWITCH_OFF,
    WIFI_CSI_SWITCH_ON,
    WIFI_CSI_SWITCH_BUTT
} wifi_csi_switch;

typedef struct {
    u8 mac[ETH_ALEN];
} ext_sta_info;

#ifdef LOS_CONFIG_P2P
typedef struct {
    char intent_param[WPA_P2P_INTENT_LEN + 1];
    char addr_txt[EXT_WIFI_TXT_ADDR_LEN + 1];
    char join_txt[WPA_P2P_JOIN_LEN + 1];
    char wps_txt[WPS_PINCODE_MAX_LEN];
} p2p_connect_param;

typedef struct {
    char mac_set[WPA_INVITE_ADDR_LEN];
    char per_id_set[WPA_INVITE_PERSISTENT_ID];
    char group[WPA_P2P_GROUP_MAX_LEN];
} p2p_invite_param;
#endif

typedef enum {
    WIFI_STASTUS_OK,
    WIFI_STASTUS_PTR_NULL,
    WIFI_STASTUS_CHAN_FAIL,
    WIFI_STASTUS_SET_FAIL
} ext_wifi_status_val;

typedef struct {
    ext_wifi_status *conn_status;
    osal_semaphore sta_status_sem;
    ext_wifi_status_val status;
} ext_wifi_status_sem;

extern int g_fast_connect_flag;
extern int g_fast_connect_scan_flag;
extern int g_connecting_flag;
extern wpa_rm_network g_wpa_rm_network;
extern int g_usr_scanning_flag;
extern size_t g_result_len;
extern char *g_scan_result_buf;
extern struct wifi_reconnect_set g_reconnect_set;
extern struct wifi_sta_opt_set g_sta_opt_set;
extern struct ext_scan_record g_scan_record;
extern struct wpa_ssid *g_connecting_ssid;
extern uapi_wifi_event_cb g_wpa_event_cb;
extern ext_wifi_scan_no_save_cb g_raw_scan_cb;
extern unsigned int g_wpa_event;
extern unsigned int g_softap_event;
#ifdef LOS_CONFIG_P2P
extern unsigned int g_p2p_event;
#endif /* LOS_CONFIG_P2P */
extern unsigned int g_sta_num;
extern ext_wifi_status *g_sta_status;
extern unsigned int g_mesh_get_sta_flag;
extern unsigned char  g_quick_conn_psk[EXT_WIFI_STA_PSK_LEN];
extern unsigned int  g_quick_conn_psk_flag;
extern struct wifi_ap_opt_set g_ap_opt_set;
extern struct hostapd_data *g_hapd;
extern ext_wifi_ap_sta_info *g_ap_sta_info;
extern int g_mesh_flag;
extern int g_mesh_sta_flag;
extern int g_scan_flag;
extern struct hostapd_conf g_global_conf;
extern unsigned char g_ssid_prefix_flag;
extern struct ext_wifi_dev *g_wifi_dev[WPA_MAX_WIFI_DEV_NUM];

#ifdef LOS_CONFIG_MESH
extern struct wifi_mesh_opt_set g_mesh_opt_set;
extern ext_wifi_mesh_peer_info *g_mesh_sta_info;
#endif /* LOS_CONFIG_MESH */

#ifdef LOS_CONFIG_P2P
extern ext_wifi_p2p_status_info *g_p2p_status_buf;
extern ext_wifi_p2p_peer_info *g_p2p_peers;
extern unsigned int g_p2p_peers_discoverd;
extern ext_wifi_p2p_client_info *g_p2p_client_info;
extern ext_p2p_persistent_group *g_p2p_persistent_group;
extern unsigned int g_persistent_group_num;
int los_set_p2p_wifi_dev_role(wifi_iftype role);
#endif
void wifi_new_task_event_cb(const ext_wifi_event *event_cb);
void los_free_wifi_dev(struct ext_wifi_dev *wifi_dev);
#if defined(LOS_CONFIG_P2P) && defined(LOS_CONFIG_P2P_TWO_IFACE)
void los_free_p2p_wifi_dev(void);
int los_set_p2p_go_gc_wifi_dev(struct wpa_supplicant *wpa_s, int role);
#endif
int los_wpa_ssid_config_set(struct wpa_ssid *ssid, const char *name, const char *value);
int los_freq_to_channel(int freq, unsigned int *channel);
struct ext_wifi_dev * los_get_wifi_dev_by_name(const char *ifname);
struct ext_wifi_dev * los_get_wifi_dev_by_iftype(ext_wifi_iftype iftype);
struct ext_wifi_dev * los_get_wifi_dev_by_priv(const void *ctx);
int los_count_wifi_dev_in_use(void);
struct ext_wifi_dev * wpa_get_other_existed_wpa_wifi_dev(const void *priv);
int los_wpa_scan(struct wpa_scan_params *params, ext_wifi_iftype iftype);
int wifi_scan(ext_wifi_iftype iftype, bool is_mesh, const ext_wifi_scan_params *sp);
int wifi_scan_result_bssid_parse(char **starttmp, void *buf, size_t *reply_len);
int wifi_scan_result_freq_parse(char **starttmp, void *buf, size_t *reply_len);
int wifi_scan_result_rssi_parse(char **starttmp, void *buf, size_t *reply_len);
void wifi_scan_result_base_flag_parse(const char *starttmp, void *buf);
int wifi_scan_result_filter_parse(const void *buf);
int wifi_scan_result_ssid_parse(char **starttmp, void *buf, size_t *reply_len);
int wifi_scan_result(ext_wifi_iftype iftype);
int chan_to_freq(unsigned char chan);
int addr_precheck(const unsigned char *addr);
int try_set_lock_flag(void);
void clr_lock_flag(void);
int is_lock_flag_off(void);
int is_ap_mesh_or_p2p_on(void);
int is_hex_string(const char *data, size_t len);
int wifi_add_iface(const struct ext_wifi_dev *wifi_dev);
int wifi_wpa_start(const struct ext_wifi_dev *wifi_dev);
int wifi_remove_iface(struct ext_wifi_dev *wifi_dev);
int wifi_wpa_stop(ext_wifi_iftype iftype);
int los_set_wifi_dev(struct ext_wifi_dev *wifi_dev);
struct ext_wifi_dev * wifi_dev_get(ext_wifi_iftype iftype);
struct ext_wifi_dev * wifi_dev_creat(ext_wifi_iftype iftype, protocol_mode_enum mode);
int wifi_channel_check(unsigned char chan);
void wpa_event_task_free(void);
#endif
