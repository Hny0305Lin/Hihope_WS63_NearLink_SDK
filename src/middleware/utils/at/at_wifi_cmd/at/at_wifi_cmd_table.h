/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: struction of AT command and public function
 */

#if ! defined (AT_WIFI_CMD_AT_CMD_TALBE_H)
#define AT_WIFI_CMD_AT_CMD_TALBE_H

#include "at.h"
typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 3 */
} cc_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 1..3 */
} pwe_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..15 */
} trans_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..255 */
} clog_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Optional. Range: 0..255 */
    int32_t                 para2; /* Optional. Range: 0..255 */
    int32_t                 para3; /* Optional. Range: 0..255 */
    int32_t                 para4; /* Optional. Range: 0..255 */
} grp_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..32 */
} sta_auth_type_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *ssid; /* Optional.   Length: 32 */
    const uint8_t           *bssid; /* Optional.   Length: 32 */
    const uint8_t           *passwd; /* Optional.   Length: 64 */
    int32_t                 psk_type;   /* Optional.    Range:0-1 */
} conn_args_t;

typedef struct {
    uint32_t                para_map;
} scan_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 1..14 */
} scanchn_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* Optional.   Length: 32 */
} scanssid_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* Optional.   Length: 17 */
} scanbssid_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    int32_t                 para2; /* Range: 1..14 */
    int32_t                 para3;
    const uint8_t           *para4; /* Optional.   Length: 64 */
} startap_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1;
    int32_t                 para2;
} startsta_args_t;

typedef struct {
    uint32_t                para_map;
} stastat_args_t;

typedef struct {
    uint32_t                para_map;
} stopap_args_t;

typedef struct {
    uint32_t                para_map;
} stopsta_args_t;

typedef struct {
    uint32_t                para_map;
} scanresult_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1;
    int32_t                 para2;
    int32_t                 para3;
    int32_t                 para4;
    int32_t                 para5;
    int32_t                 para6;
} startpromise_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1;
    int32_t                 para2;
    int32_t                 para3;
} startsdp_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                  para1;
    const uint8_t           *para2; /*    Length: 32 */
    int32_t                  para3;
} sdp_subscribe_args_t;

typedef struct {
    uint32_t                para_map;
} disconn_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *ifname; /*    Length: 32 */
    int32_t                 index;
    int32_t                 frame_type_bitmap;
    const uint8_t           *cmd; /*    Length: 32 */
    int32_t                 ie_type; /* Optional. */
    int32_t                 ie_len; /* Optional. */
    int32_t                 para7; /* Optional. Range: 0..255 */
    int32_t                 para8; /* Optional. Range: 0..255 */
    int32_t                 para9; /* Optional. Range: 0..255 */
    int32_t                 para10; /* Optional. Range: 0..255 */
} userieset_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* Length: 32 */
} deauthsta_args_t;

typedef struct {
    uint32_t                para_map;
} showsta_args_t;

typedef struct {
    uint32_t                para_map;
} apscan_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* Optional.      Length: 32 */
    const uint8_t           *para2; /* Optional.      Length: 32 */
    int32_t                 para3; /* Range: 1..14 */
    int32_t                 para4;
    const uint8_t           *para5; /* Optional.   Length: 64 */
} fconn_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *ifname; /* Length: 32 */
    int32_t                 user_id;
    int32_t                 enable;
    int32_t                 cfg_match_ta_ra_sel; /* Optional. */
    const uint8_t           *mac; /* Optional. Length: 32 */
    int32_t                 frame_type_filter_bitmap; /* Optional. Range: 0..7 */
    int32_t                 sub_type_filter_enable; /* Optional. */
    int32_t                 sub_type_filter; /* Optional. Range: 0..15 */
    int32_t                 ppdu_filter_bitmap; /* Optional. Range: 0..63 */
    int32_t                 period; /* Optional. Range: 0..4095 */
} csisetconfig_args_t;

typedef struct {
    uint32_t                para_map;
} csistart_args_t;

typedef struct {
    uint32_t                para_map;
} csistop_args_t;

typedef struct {
    uint32_t                para_map;
} pinshow_args_t;

typedef struct {
    uint32_t                para_map;
} pbc_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *pin_code; /*    Length: 8 */
} pin_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1;
    const uint8_t           *para2; /*    Length: 2800 */
} sendpkt_args_t;

#include "at.h"
typedef struct {
    uint32_t                para_map;
    int32_t                 ps_switch;
} ps_args_t;

typedef struct {
    uint32_t                para_map;
} wifiinit_args_t;

typedef struct {
    uint32_t                para_map;
} wifiisinit_args_t;

typedef struct {
    uint32_t                para_map;
} wifiuninit_args_t;

typedef union {
    cc_args_t       cc;
    pwe_args_t      pwe;
    trans_args_t    trans;
    clog_args_t     clog;
    grp_args_t      sae_grp;
    sta_auth_type_args_t auth_type;
    conn_args_t     conn;
    disconn_args_t  disconn;
    startsdp_args_t  startsdp;
    sdp_subscribe_args_t sdpsubscribe;
    startpromise_args_t promisestart;
    scan_args_t     scan;
    scanchn_args_t  scanchn;
    scanssid_args_t scanssid;
    scanbssid_args_t scanbssid;
    startap_args_t  startap;
    startsta_args_t startsta;
    stastat_args_t  stastat;
    stopap_args_t   stopap;
    stopsta_args_t  stopsta;
    scanresult_args_t scanresult;
    userieset_args_t userieset;
    deauthsta_args_t deauthsta;
    showsta_args_t  showsta;
    apscan_args_t   apscan;
    fconn_args_t    fconn;
    csisetconfig_args_t csisetconfig;
    csistart_args_t csistart;
    csistop_args_t  csistop;
    pinshow_args_t   pinshow;
    pbc_args_t      pbc;
    pin_args_t      pin;
    sendpkt_args_t  sendpkt;
    ps_args_t       ps;
    wifiinit_args_t wifiinit;
    wifiisinit_args_t wifiisinit;
    wifiuninit_args_t wifiuninit;
} at_wifi_cmd_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 protocol_mode; /* Optional.Range: 1..4 */
    int32_t                 beacon_interval; /* Optional.Range: 25..1000 */
    int32_t                 dtim_period; /* Optional.Range: 1..30 */
    int32_t                 group_rekey; /* Optional.Range: 30..86400 */
    int32_t                 hidden_ssid_flag; /* Optional.Range: 1..2 */
    int32_t                 gi; /* Optional. */
} setapadv_args_t;

typedef struct {
    uint32_t                para_map;
} apconfig_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 enable;
    int32_t                 period; /* Optional.Range: 1..65535 */
    int32_t                 count; /* Optional.Range: 1..65535 */
    int32_t                 timeout; /* Optional.Range: 2..65535 */
} reconn_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..0 */
} setrate_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t          *para1; /* Length: 32 */
} getrate_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..2 */
} tpc_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..1 */
} trc_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..2 */
    int32_t                 para2; /* Range: 0..32767 */
} rts_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..1 */
    int32_t                 para2; /* Range: -128..127 */
} cca_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t          *para1; /* Length: 32 */
    int32_t                 para2; /* Range: 0..1 */
    int32_t                 para3; /* Range: 0..63 */
} intrf_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 enable;
    int32_t                 cycle; /* Range: 100..1000 */
    int32_t                 duration; /* Range: 1..65535 */
} psd_args_t;

/* AT Command */
at_ret_t cmd_wpa_set_country(const cc_args_t *args);
at_ret_t cmd_wpa_get_country(void);
at_ret_t cmd_set_sae_pwe(const pwe_args_t *args);
at_ret_t cmd_set_sae_transition(const trans_args_t *args);
at_ret_t cmd_set_sae_anti_clog_threshold(const clog_args_t *args);
at_ret_t cmd_set_sae_groups(const grp_args_t *args);

/* AT+STAAUTH */
at_ret_t cmd_set_sta_assoc_auth(const sta_auth_type_args_t *args);

/* AT Command */
at_ret_t cmd_sta_connect(const conn_args_t *args);

/* AT Command */
at_ret_t cmd_sta_disconnect(void);

/* PROMISE ENABLE */
at_ret_t cmd_promise_enable(const startpromise_args_t *args);

/* SDP ENABLE */
at_ret_t cmd_sdp_enable(const startsdp_args_t *args);

/* SDP SUBSCRIBE */
at_ret_t cmd_sdp_subscribe(const sdp_subscribe_args_t *args);

/* Reconnect */
at_ret_t cmd_set_reconn(const reconn_args_t *args);

/* AT Command */
at_ret_t cmd_wpa_scan(void);

/* AT Command */
at_ret_t cmd_wpa_scan_result_clear(void);

/* AT Command */
at_ret_t cmd_wpa_channel_scan(const scanchn_args_t *args);

/* AT Command */
at_ret_t cmd_wpa_ssid_scan(const scanssid_args_t *args);

/* AT Command */
at_ret_t cmd_ssid_prefix_scan(const scanssid_args_t *args);

/* AT Command */
at_ret_t cmd_wpa_bssid_scan(const scanbssid_args_t *args);

/* Get Scan Results */
at_ret_t cmd_wpa_scan_results(void);

/* AT Command */
at_ret_t cmd_start_softap(const startap_args_t *args);

/* AT Command */
at_ret_t cmd_sta_start(void);
at_ret_t cmd_sta_start_adv(const startsta_args_t *args);

/* AT Command */
at_ret_t cmd_sta_status(void);
at_ret_t cmd_sta_show_config(void);

/* AT Command */
at_ret_t cmd_stop_softap(void);

/* AT Command */
at_ret_t cmd_sta_stop(void);
/* SETAPADV */
at_ret_t cmd_set_softap_advance(const setapadv_args_t *args);
/* Get APCONFIG */
at_ret_t cmd_softap_show_config(void);

/* AT Command */
at_ret_t cmd_sta_user_ie_set(const userieset_args_t *args);

/* AT Command */
at_ret_t cmd_softap_deauth_sta(const deauthsta_args_t *args);

/* AT Command */
at_ret_t cmd_softap_show_sta(void);

/* AT+WFARECONN */
at_ret_t cmd_sta_reconnect_for_wfa_test(void);

/* AT+APSCAN */
at_ret_t cmd_softap_scan(void);

/* AT+FCONN */
at_ret_t cmd_sta_quick_connect(const fconn_args_t *args);

/* AT Command */
at_ret_t cmd_csi_set_config(const csisetconfig_args_t *args);

/* AT Command */
at_ret_t cmd_csi_start(void);

/* AT Command */
at_ret_t cmd_csi_stop(void);

/* AT+GETPIN */
at_ret_t cmd_wpa_wps_pin_get(void);

/* AT+PBC */
at_ret_t cmd_wpa_wps_pbc(void);

/* AT+PIN */
at_ret_t cmd_wpa_wps_pin(const pin_args_t *args);

/* AT Command */
at_ret_t cmd_send_custom_pkt(const sendpkt_args_t *args);

/* AT+SETRATE */
at_ret_t cmd_autorate_set_rate(const setrate_args_t *args);

/* AT+GETRATE */
at_ret_t cmd_get_rate(const getrate_args_t *args);

/* AT+TPC */
at_ret_t cmd_tpc_switch(const tpc_args_t *args);

/* AT+TRC */
at_ret_t cmd_autorate_fix_switch(const trc_args_t *args);

/* AT+RTS */
at_ret_t cmd_rts_switch(const rts_args_t *args);

/* AT+CCA */
at_ret_t cmd_cca_switch(const cca_args_t *args);

/* AT+INTRF */
at_ret_t cmd_intrf_switch(const intrf_args_t *args);

/* AT+PS */
at_ret_t cmd_set_sta_pm(const ps_args_t *args);

/* AT+WIFIINIT */
at_ret_t cmd_wifi_init(void);

/* AT+WIFIISINIT */
at_ret_t cmd_wifi_is_init(void);

/* AT+WIFIUNINIT */
at_ret_t cmd_wifi_uninit(void);

/* AT+PSD=enable,cycle,duration PSD Control */
at_ret_t cmd_control_psd(const psd_args_t *args);

const at_para_parse_syntax_t cc_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(cc_args_t, para1)
    },
};

const at_para_parse_syntax_t sae_pwe_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 3,
        .offset = offsetof(pwe_args_t, para1)
    },
};

const at_para_parse_syntax_t sae_trans_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 15,
        .offset = offsetof(trans_args_t, para1)
    },
};

const at_para_parse_syntax_t sae_clog_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(clog_args_t, para1)
    },
};

const at_para_parse_syntax_t sae_grp_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(grp_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(grp_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(grp_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(grp_args_t, para4)
    },
};

const at_para_parse_syntax_t sta_auth_type_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 32,
        .offset = offsetof(sta_auth_type_args_t, para1)
    },
};

const at_para_parse_syntax_t start_promise_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 4,
        .offset = offsetof(startpromise_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(startpromise_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(startpromise_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(startpromise_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(startpromise_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(startpromise_args_t, para6)
    },
};

const at_para_parse_syntax_t start_sdp_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 4,
        .offset = offsetof(startsdp_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 2,
        .offset = offsetof(startsdp_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 9,
        .offset = offsetof(startsdp_args_t, para3)
    },
};

const at_para_parse_syntax_t sdp_subscribe_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 4,
        .offset = offsetof(sdp_subscribe_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 34,
        .offset = offsetof(sdp_subscribe_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(sdp_subscribe_args_t, para3)
    },
};


const int32_t conn_para3_values[] = {
    0, 1, 2, 3, 4, 7, 8, 10
};
const at_para_parse_syntax_t conn_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 131,
        .offset = offsetof(conn_args_t, ssid)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(conn_args_t, bssid)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 66,
        .offset = offsetof(conn_args_t, passwd)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(conn_args_t, psk_type)
    },
};

const at_para_parse_syntax_t scanchn_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 14,
        .offset = offsetof(scanchn_args_t, para1)
    },
};

const at_para_parse_syntax_t scanssid_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 131,
        .offset = offsetof(scanssid_args_t, para1)
    },
};

const at_para_parse_syntax_t scanbssid_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 17,
        .offset = offsetof(scanbssid_args_t, para1)
    },
};

const int32_t reconn_enable_values[] = {
    0, 1
};
const at_para_parse_syntax_t reconn_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, reconn_enable_values},
        .offset = offsetof(reconn_args_t, enable)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(reconn_args_t, period)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(reconn_args_t, count)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 2,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(reconn_args_t, timeout)
    },
};

const at_para_parse_syntax_t setapadv_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.max_val = 4,
        .offset = offsetof(setapadv_args_t, protocol_mode)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.max_val = 1000,
        .offset = offsetof(setapadv_args_t, beacon_interval)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.max_val = 30,
        .offset = offsetof(setapadv_args_t, dtim_period)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.max_val = 86400,
        .offset = offsetof(setapadv_args_t, group_rekey)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.max_val = 2,
        .offset = offsetof(setapadv_args_t, hidden_ssid_flag)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .last = true,
        .offset = offsetof(setapadv_args_t, gi)
    },
};

const int32_t startap_para3_values[] = {
    0, 1, 2, 3, 7, 8, 10, 14
};
const at_para_parse_syntax_t startap_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 34,
        .offset = offsetof(startap_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 14,
        .offset = offsetof(startap_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {8, startap_para3_values},
        .offset = offsetof(startap_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 66,
        .offset = offsetof(startap_args_t, para4)
    },
};

const int32_t startsta_para1_values[] = {
    0, 1, 2, 3, 4
};
const int32_t startsta_para2_values[] = {
    0, 1, 2
};
const at_para_parse_syntax_t startsta_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {5, startsta_para1_values},
        .offset = offsetof(startsta_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, startsta_para2_values},
        .offset = offsetof(startsta_args_t, para2)
    },
};

const int32_t userieset_index_values[] = {
    1, 2, 3, 4
};
const int32_t userieset_frame_type_bitmap_values[] = {
    1, 2, 4
};
const int32_t userieset_ie_type_values[] = {
    221
};
const int32_t userieset_ie_len_values[] = {
    4
};

const at_para_parse_syntax_t userieset_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(userieset_args_t, ifname)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, userieset_index_values},
        .offset = offsetof(userieset_args_t, index)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, userieset_frame_type_bitmap_values},
        .offset = offsetof(userieset_args_t, frame_type_bitmap)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(userieset_args_t, cmd)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_list = {1, userieset_ie_type_values},
        .offset = offsetof(userieset_args_t, ie_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_list = {1, userieset_ie_len_values},
        .offset = offsetof(userieset_args_t, ie_len)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(userieset_args_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(userieset_args_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(userieset_args_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(userieset_args_t, para10)
    },
};

const at_para_parse_syntax_t deauthsta_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 17,
        .offset = offsetof(deauthsta_args_t, para1)
    },
};

const at_para_parse_syntax_t fconn_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 131,
        .offset = offsetof(fconn_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(fconn_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 14,
        .offset = offsetof(fconn_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {8, conn_para3_values},
        .offset = offsetof(fconn_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 66,
        .offset = offsetof(fconn_args_t, para5)
    },
};
const int32_t csisetconfig_user_id_values[] = {
    0, 1, 2, 3
};
const int32_t csisetconfig_enable_values[] = {
    0, 1
};
const int32_t csisetconfig_cfg_match_ta_ra_sel_values[] = {
    0, 1
};
const int32_t csisetconfig_sub_type_filter_enable_values[] = {
    0, 1
};
const at_para_parse_syntax_t csisetconfig_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(csisetconfig_args_t, ifname)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, csisetconfig_user_id_values},
        .offset = offsetof(csisetconfig_args_t, user_id)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, csisetconfig_enable_values},
        .offset = offsetof(csisetconfig_args_t, enable)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_list = {2, csisetconfig_cfg_match_ta_ra_sel_values},
        .offset = offsetof(csisetconfig_args_t, cfg_match_ta_ra_sel)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.string.max_length = 32,
        .offset = offsetof(csisetconfig_args_t, mac)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7,
        .offset = offsetof(csisetconfig_args_t, frame_type_filter_bitmap)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_list = {2, csisetconfig_sub_type_filter_enable_values},
        .offset = offsetof(csisetconfig_args_t, sub_type_filter_enable)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 15,
        .offset = offsetof(csisetconfig_args_t, sub_type_filter)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 63,
        .offset = offsetof(csisetconfig_args_t, ppdu_filter_bitmap)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 4095,
        .offset = offsetof(csisetconfig_args_t, period)
    },
};

const at_para_parse_syntax_t pin_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(pin_args_t, pin_code)
    },
};

const int32_t sendpkt_para1_values[] = {
    0, 1
};
const at_para_parse_syntax_t sendpkt_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, sendpkt_para1_values},
        .offset = offsetof(sendpkt_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 2800,
        .offset = offsetof(sendpkt_args_t, para2)
    },
};

const at_para_parse_syntax_t setrate_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(setrate_args_t, para1)
    },
};

const at_para_parse_syntax_t getrate_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(getrate_args_t, para1)
    },
};

const at_para_parse_syntax_t tpc_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(tpc_args_t, para1)
    },
};

const at_para_parse_syntax_t trc_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(trc_args_t, para1)
    },
};

const at_para_parse_syntax_t rts_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(rts_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 32767,
        .offset = offsetof(rts_args_t, para2)
    },
};

const at_para_parse_syntax_t cca_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(cca_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = -128,
        .entry.int_range.max_val = 127,
        .offset = offsetof(cca_args_t, para2)
    },
};

const at_para_parse_syntax_t intrf_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(intrf_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(intrf_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(intrf_args_t, para3)
    },
};

const int32_t ps_ps_switch_values[] = {
    0, 1, 2, 3, 255
};
const at_para_parse_syntax_t ps_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {5, ps_ps_switch_values},
        .offset = offsetof(ps_args_t, ps_switch)
    },
};

const int32_t psd_enable_values[] = {
    0, 1
};

const at_para_parse_syntax_t psd_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, psd_enable_values},
        .offset = offsetof(psd_args_t, enable)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 100,
        .entry.int_range.max_val = 1000,
        .offset = offsetof(psd_args_t, cycle)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(psd_args_t, duration)
    },
};

const at_cmd_entry_t at_wifi_cmd_parse_table[] = {
    {
        "CC",
        3,
        0,
        cc_syntax,
        NULL,
        (at_set_func_t)cmd_wpa_set_country,
        cmd_wpa_get_country,
        NULL,
    },
    {
        "SAEPWE",
        6,
        0,
        sae_pwe_syntax,
        NULL,
        (at_set_func_t)cmd_set_sae_pwe,
        NULL,
        NULL,
    },
    {
        "SAETRANS",
        9,
        0,
        sae_trans_syntax,
        NULL,
        (at_set_func_t)cmd_set_sae_transition,
        NULL,
        NULL,
    },
    {
        "SAECLOG",
        8,
        0,
        sae_clog_syntax,
        NULL,
        (at_set_func_t)cmd_set_sae_anti_clog_threshold,
        NULL,
        NULL,
    },
    {
        "SAEGRP",
        7,
        0,
        sae_grp_syntax,
        NULL,
        (at_set_func_t)cmd_set_sae_groups,
        NULL,
        NULL,
    },
    {
        "STAAUTH",
        8,
        0,
        sta_auth_type_syntax,
        NULL,
        (at_set_func_t)cmd_set_sta_assoc_auth,
        NULL,
        NULL,
    },
    {
        "CONN",
        5,
        0,
        conn_syntax,
        NULL,
        (at_set_func_t)cmd_sta_connect,
        NULL,
        NULL,
    },
	{
        "DISCONN",
        8,
        0,
        NULL,
        cmd_sta_disconnect,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "RECONN",
        7,
        0,
        reconn_syntax,
        NULL,
        (at_set_func_t)cmd_set_reconn,
        NULL,
        NULL,
    },
    {
        "WFARECONN",
        8,
        0,
        NULL,
        cmd_sta_reconnect_for_wfa_test,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "FCONN",
        16,
        0,
        fconn_syntax,
        NULL,
        (at_set_func_t)cmd_sta_quick_connect,
        NULL,
        NULL,
    },
    {
        "SCAN",
        5,
        0,
        NULL,
        cmd_wpa_scan,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SCANCHN",
        8,
        0,
        scanchn_syntax,
        NULL,
        (at_set_func_t)cmd_wpa_channel_scan,
        NULL,
        NULL,
    },
    {
        "SCANSSID",
        8,
        0,
        scanssid_syntax,
        NULL,
        (at_set_func_t)cmd_wpa_ssid_scan,
        NULL,
        NULL,
    },
    {
        "SCANPRSSID",
        8,
        0,
        scanssid_syntax,
        NULL,
        (at_set_func_t)cmd_ssid_prefix_scan,
        NULL,
        NULL,
    },
    {
        "SCANBSSID",
        8,
        0,
        scanbssid_syntax,
        NULL,
        (at_set_func_t)cmd_wpa_bssid_scan,
        NULL,
        NULL,
    },
    {
        "STARTAP",
        8,
        0,
        startap_syntax,
        NULL,
        (at_set_func_t)cmd_start_softap,
        NULL,
        NULL,
    },
    {
        "STARTSTA",
        9,
        0,
        startsta_syntax,
        cmd_sta_start,
        (at_set_func_t)cmd_sta_start_adv,
        NULL,
        NULL,
    },
    {
        "STASTAT",
        8,
        0,
        NULL,
        cmd_sta_status,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "STACONFIG",
        10,
        0,
        NULL,
        cmd_sta_show_config,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "STOPAP",
        7,
        0,
        NULL,
        cmd_stop_softap,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "STOPSTA",
        8,
        0,
        NULL,
        cmd_sta_stop,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SETPROMISE",
        9,
        0,
        start_promise_syntax,
        NULL,
        (at_set_func_t)cmd_promise_enable,
        NULL,
        NULL,
    },
#ifdef _PRE_WLAN_FEATURE_SDP
    {
        "SDPENABLE",
        9,
        0,
        start_sdp_syntax,
        NULL,
        (at_set_func_t)cmd_sdp_enable,
        NULL,
        NULL,
    },
    {
        "SDPSUBSCRIBE",
        10,
        0,
        sdp_subscribe_syntax,
        NULL,
        (at_set_func_t)cmd_sdp_subscribe,
        NULL,
        NULL,
    },
#endif
    {
        "SCANRESULT",
        11,
        0,
        NULL,
        cmd_wpa_scan_results,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SETAPADV",
        9,
        0,
        setapadv_syntax,
        NULL,
        (at_set_func_t)cmd_set_softap_advance,
        NULL,
        NULL,
    },
	{
        "APCONFIG",
        9,
        0,
        NULL,
        cmd_softap_show_config,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "USERIESET",
        10,
        0,
        userieset_syntax,
        NULL,
        (at_set_func_t)cmd_sta_user_ie_set,
        NULL,
        NULL,
    },
    {
        "DEAUTHSTA",
        10,
        0,
        deauthsta_syntax,
        NULL,
        (at_set_func_t)cmd_softap_deauth_sta,
        NULL,
        NULL,
    },
    {
        "SHOWSTA",
        8,
        0,
        NULL,
        cmd_softap_show_sta,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "APSCAN",
        15,
        0,
        NULL,
        cmd_softap_scan,
        NULL,
        NULL,
        NULL,
    },
    {
        "CSISETCONFIG",
        13,
        0,
        csisetconfig_syntax,
        NULL,
        (at_set_func_t)cmd_csi_set_config,
        NULL,
        NULL,
    },
    {
        "CSISTART",
        9,
        0,
        NULL,
        cmd_csi_start,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "CSISTOP",
        8,
        0,
        NULL,
        cmd_csi_stop,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#ifdef CONFIG_WPS_SUPPORT
    {
        "PINSHOW",
        8,
        0,
        NULL,
        cmd_wpa_wps_pin_get,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "PBC",
        4,
        0,
        NULL,
        cmd_wpa_wps_pbc,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "PIN",
        4,
        0,
        pin_syntax,
        NULL,
        (at_set_func_t)cmd_wpa_wps_pin,
        NULL,
        NULL,
    },
#endif /* end of CONFIG_WPS_SUPPORT */
    {
        "SENDPKT",
        5,
        0,
        sendpkt_syntax,
        NULL,
        (at_set_func_t)cmd_send_custom_pkt,
        NULL,
        NULL,
    },
    {
        "SETRATE",
        10,
        0,
        setrate_syntax,
        NULL,
        (at_set_func_t)cmd_autorate_set_rate,
        NULL,
        NULL,
    },
    {
        "GETRATE",
        11,
        0,
        getrate_syntax,
        NULL,
        (at_set_func_t)cmd_get_rate,
        NULL,
        NULL,
    },
    {
        "TPC",
        12,
        0,
        tpc_syntax,
        NULL,
        (at_set_func_t)cmd_tpc_switch,
        NULL,
        NULL,
    },
    {
        "TRC",
        13,
        0,
        trc_syntax,
        NULL,
        (at_set_func_t)cmd_autorate_fix_switch,
        NULL,
        NULL,
    },
    {
        "RTS",
        16,
        0,
        rts_syntax,
        NULL,
        (at_set_func_t)cmd_rts_switch,
        NULL,
        NULL,
    },
    {
        "CCA",
        17,
        0,
        cca_syntax,
        NULL,
        (at_set_func_t)cmd_cca_switch,
        NULL,
        NULL,
    },
    {
        "INTRF",
        18,
        0,
        intrf_syntax,
        NULL,
        (at_set_func_t)cmd_intrf_switch,
        NULL,
        NULL,
    },
    {
        "PS",
        5,
        0,
        ps_syntax,
        NULL,
        (at_set_func_t)cmd_set_sta_pm,
        NULL,
        NULL,
    },
    {
        "WIFIINIT",
        5,
        0,
        NULL,
        cmd_wifi_init,
        NULL,
        NULL,
        NULL,
    },
    {
        "WIFIISINIT",
        5,
        0,
        NULL,
        NULL,
        NULL,
        cmd_wifi_is_init,
        NULL,
    },
    {
        "WIFIUNINIT",
        5,
        0,
        NULL,
        cmd_wifi_uninit,
        NULL,
        NULL,
        NULL,
    },
    {
        "PSD",
        10,
        0,
        psd_syntax,
        NULL,
        (at_set_func_t)cmd_control_psd,
        NULL,
        NULL,
    },
    {
        "CLEARSCANRESULT",
        20,
        0,
        NULL,
        cmd_wpa_scan_result_clear,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
};

#endif  /* AT_WIFI_CMD_AT_CMD_TALBE_H */
