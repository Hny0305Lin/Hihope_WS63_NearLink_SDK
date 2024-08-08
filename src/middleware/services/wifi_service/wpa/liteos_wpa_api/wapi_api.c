/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: WAPI APIs
 */

#include "securec.h"
#include "utils/includes.h"
#include "utils/common.h"
#include "utils/os.h"
#include "utils/eloop.h"
#include "utils/base64.h"
#include "utils/wpabuf.h"
#include "common/wpa_common.h"
#include "common/defs.h"
#include "common/wpa_ctrl.h"
#include "l2_packet/l2_packet.h"
#include "wpa_supplicant_i.h"
#include "driver_i.h"
#include "bss.h"
#include "wapi.h"
#include "wapi_api.h"
#include "wai_sm.h"
#include "wai_rxtx.h"

#define WAPI_HEAD_LEN (WAPI_IE_ID_SIZE + WAPI_IE_LENGTH_SIZE + WAPI_IE_VERSION_SIZE + WAPI_IE_AKM_CNT_LEN)

int wapi_init_iface(struct wpa_supplicant *wpa)
{
    struct wapi_asue_struct *wapi = NULL;

    if (wpa == NULL) {
        wpa_error_log0(MSG_WARNING, "wpa struct is null");
        return WAPI_FAILED;
    }
    if (wpa->wapi != NULL) {
        wpa_error_log0(MSG_WARNING, "wapi has initialized");
        return WAPI_FAILED;
    }

    wapi = (struct wapi_asue_struct *)os_zalloc(sizeof(struct wapi_asue_struct));
    if (wapi == NULL) {
        wpa_error_log0(MSG_WARNING, "Malloc wapi_s memory failed");
        return WAPI_FAILED;
    }

    wpa_error_log0(MSG_DEBUG, "Initial WAPI L2 packet");
    wapi->wapi_l2 = l2_packet_init(wpa->ifname,
        wpa_drv_get_mac_addr(wpa), ETH_TYPE_WAI, wai_rx_packet, wpa, 1);
    if (wapi->wapi_l2 == NULL) {
        os_free(wapi);
        wpa_error_log0(MSG_DEBUG, "l2_packet_init failed");
        return WAPI_FAILED;
    }

    if (l2_packet_get_own_addr(wapi->wapi_l2, wapi->own_mac, ETH_ALEN) != 0) {
        wpa_error_log0(MSG_DEBUG, "Failed to get own L2 address");
        l2_packet_deinit(wapi->wapi_l2);
        os_free(wapi);
        return WAPI_FAILED;
    }

    wapi_iface_init(wapi);

    wpa->wapi = wapi;
    wapi->wpa = wpa;

    wpa_error_log0(MSG_DEBUG, "wapi iface init success");
    return WAPI_SUCCESS;
}

int wapi_deinit_iface(struct wpa_supplicant *wpa)
{
    struct wapi_asue_struct *wapi = NULL;

    if (wpa == NULL) {
        wpa_error_log0(MSG_WARNING, "wpa is null");
        return WAPI_FAILED;
    }

    if (wpa->wapi == NULL) {
        wpa_error_log0(MSG_WARNING, "wapi is null");
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;

    if (wapi->wapi_l2 != NULL) {
        l2_packet_deinit(wapi->wapi_l2);
        wapi->wapi_l2 = NULL;
    }

    wapi_iface_deinit(wapi);

    os_free(wapi);
    wpa->wapi = NULL;

    return WAPI_SUCCESS;
}

int wapi_parse_wapi_ie(const unsigned char *wapi_ie, unsigned int ie_len,
    struct wpa_ie_data *ie_data)
{
    return wapi_parse_ie(wapi_ie, ie_len, ie_data);
}

static int wapi_config_associate_param(struct wpa_supplicant *wpa,
    struct wpa_bss *bss, struct wpa_ssid *ssid,
    struct wpa_driver_associate_params *params)
{
    unsigned char *wapi_ie = NULL;
    unsigned char *ie_ssid = NULL;
    unsigned int   cipher_pairwise;
    unsigned int   cipher_group;

    cipher_pairwise = WPA_CIPHER_NONE;
    cipher_group    = WPA_CIPHER_NONE;

    if (bss != NULL) {
        ie_ssid = (unsigned char *)wpa_bss_get_ie(bss, 0);
        if (ie_ssid == NULL) {
            wpa_error_log0(MSG_WARNING, "get bss ie failed");
            return WAPI_FAILED;
        }
        params->ssid     = ie_ssid + 2; /* 2 bytes for EID and length */
        params->ssid_len = ie_ssid[1];
        params->bssid    = bss->bssid;

        wapi_ie = (unsigned char *)wpa_bss_get_ie(bss, WAPI_IE_ID);
        if (wapi_ie != NULL) {
            wpa->wapi->wapi_ie_len = (size_t)(wapi_ie[1] + 2); /* 2 bytes for EID and length */
            /* 2 bytes for EID and length */
            if (wapi_ie[1] &&
                memcpy_s(wpa->wapi->wapi_ie, WAPI_IE_MAX_SIZE, wapi_ie, (size_t)(wapi_ie[1] + 2)) != EOK) {
                wpa_error_log0(MSG_WARNING, "memcpy wapi ie failed");
                return WAPI_FAILED;
            }
            wpa->wapi->wapi_ie[1]  += 2; /* 2 bytes for EID and length */
            wpa->wapi->wapi_ie_len += 2; /* 2 bytes for EID and length */
            wpa->wapi->wapi_ie[wpa->wapi->wapi_ie_len - 2] = 0; /* 2 bytes for EID and length */
            wpa->wapi->wapi_ie[wpa->wapi->wapi_ie_len - 1] = 0;
        }

        params->freq.freq = bss->freq;
    } else {
        wapi_ie          = NULL;
        params->ssid     = ssid->ssid;
        params->ssid_len = ssid->ssid_len;
    }

    params->mode       = 0; /* Modes of operation: Let the driver decides */
    params->wpa_ie     = wpa->wapi->wapi_ie;
    params->wpa_ie_len = wpa->wapi->wapi_ie_len;

    params->pairwise_suite = cipher_pairwise;
    params->group_suite = cipher_group;

    if (!os_memcmp(wpa->bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN)) {
        if (wpa_drv_associate(wpa, params)) {
            wpa_error_log0(MSG_WARNING, "wpa_drv_associate failed");
            wpas_connection_failed(wpa, wpa->pending_bssid);
            wpa_supplicant_set_state(wpa, WPA_DISCONNECTED);
            if (memset_s(wpa->pending_bssid, ETH_ALEN, 0, ETH_ALEN) != EOK) {
                wpa_error_log0(MSG_WARNING, "memset bssid failed");
                return WAPI_FAILED;
            }
            wpa->current_bss = NULL;
            wpa->current_ssid = NULL;
            return WAPI_FAILED;
        }

        /* Timeout for IEEE 802.11 authentication and association */
        wpa_supplicant_req_auth_timeout(wpa, WAI_AUTH_TIMEOUT, 0);
    }

    return WAPI_SUCCESS;
}

int wapi_asue_event_assoc(struct wpa_supplicant *wpa,
    struct wpa_bss *bss, struct wpa_ssid *ssid,
    struct wpa_driver_associate_params *params)
{
    struct wapi_asue_struct *wapi = NULL;

    if ((wpa == NULL) || (wpa->wapi == NULL) ||
        (bss == NULL) || (ssid == NULL) || (params == NULL)) {
        wpa_error_log0(MSG_WARNING, "wapi_asue_event_assoc input null pointer!");
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;

    if (wapi_generate_addid(wapi, bss) != WAPI_SUCCESS) {
        wpa_error_log0(MSG_WARNING, "generate addid failed");
        return WAPI_FAILED;
    }

    if ((unsigned int)(wpa->key_mgmt) & WPA_KEY_MGMT_WAPI_PSK) {
        wapi->auth_type = AUTH_TYPE_WAPI_PSK;
        params->key_mgmt_suite = WPA_KEY_MGMT_WAPI_PSK;
        if (WAPI_SUCCESS != wapi_psk_to_bk(wpa, ssid)) {
            wpa_error_log0(MSG_WARNING, "fail to derivate psk");
            return WAPI_FAILED;
        }
    } else if ((unsigned int)(wpa->key_mgmt) & WPA_KEY_MGMT_WAPI_CERT) {
        wpa_error_log0(MSG_WARNING, "not support wapi-cert");
        return WAPI_FAILED;
    } else {
        wpa->wapi->auth_type = AUTH_TYPE_NONE_WAPI;
    }

    wpa_error_log1(MSG_DEBUG, "auth_type %u", (unsigned int)wpa->wapi->auth_type);

    if (wapi_init_ie(wpa) != WAPI_SUCCESS) {
        wpa_error_log0(MSG_WARNING, "fail to init ie");
        return WAPI_FAILED;
    }

    if (wapi_config_associate_param(wpa, bss, ssid, params) != WAPI_SUCCESS) {
        wpa_error_log0(MSG_WARNING, "fail to configurate associating param");
        return WAPI_FAILED;
    }

    return WAPI_SUCCESS;
}

int wapi_asue_event_disassoc(struct wpa_supplicant *wpa,
    unsigned short us_reasonus_reason)
{
    (void)us_reasonus_reason;
    struct wapi_asue_struct *wapi = NULL;
    unsigned char bssid[ETH_ALEN];

    if ((wpa == NULL) || (wpa->wapi == NULL)) {
        wpa_error_log0(MSG_WARNING, "wpa or wpa->wapi is null");
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;

    (void)wpa_drv_get_bssid(wpa, bssid);

    if (memcpy_s(wapi->own_mac, ETH_ALEN, wpa->own_addr, ETH_ALEN) != EOK) {
        wpa_error_log0(MSG_WARNING, "memcpy own_mac failed");
        return WAPI_FAILED;
    }
    if (memcpy_s(wapi->bssid, ETH_ALEN, bssid, ETH_ALEN) != EOK) {
        wpa_error_log0(MSG_WARNING, "memcpy bssid failed");
        return WAPI_FAILED;
    }

    (void)wapi_event_process(wapi, CONN_DISASSOC, NULL, 0);

    return WAPI_SUCCESS;
}

int wapi_asue_event(struct wpa_supplicant *wpa,
    enum wpa_event_type event, void *data)
{
    (void)data;
    struct wapi_asue_struct *wapi = NULL;
    unsigned char bssid[ETH_ALEN];

    if ((wpa == NULL) || (wpa->wapi == NULL)) {
        wpa_error_log0(MSG_WARNING, "wapi_asue_event input null pointer");
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;

    wpa_error_log1(MSG_DEBUG, "event %u", (unsigned int)event);
    switch (event) {
        case EVENT_ASSOC:
            (void)wpa_drv_get_bssid(wpa, bssid);
            if (memcpy_s(wapi->own_mac, ETH_ALEN, wpa->own_addr, ETH_ALEN) != EOK) {
                wpa_error_log0(MSG_WARNING, "memcpy own_mac failed");
                return WAPI_FAILED;
            }
            if (memcpy_s(wapi->bssid, ETH_ALEN, bssid, ETH_ALEN) != EOK) {
                wpa_error_log0(MSG_WARNING, "memcpy bssid failed");
                return WAPI_FAILED;
            }
            if (wapi->wapi_ie_len) {
                (void)wapi_event_process(wapi, CONN_ASSOC, wapi->wapi_ie, wapi->wapi_ie_len);
            } else {
                (void)wapi_event_process(wapi, CONN_ASSOC, NULL, 0);
            }
            break;
        case EVENT_DISASSOC:
            (void)wapi_event_process(wapi, CONN_DISASSOC, NULL, 0);
            break;
        default:
            wpa_error_log0(MSG_DEBUG, "event is unknown");
            break;
    }

    return WAPI_SUCCESS;
}

char *wpa_supplicant_wapi_ie_txt(char *pos, char *end, const unsigned char *ie, size_t ie_len)
{
    int akm_suit_cnt = 0, i, ret;
    unsigned char *ie_hdr = (unsigned char *)ie;
    unsigned char *p_akm_auit_cnt = NULL;
    unsigned char *p_akm = NULL;
    char *old_pos = pos;

    if (end <= pos) {
        wpa_error_log0(MSG_ERROR, "end is smaller than pos");
        return old_pos;
    }

    if (ie_len < WAPI_HEAD_LEN) {
        /* ensure ie_len much more length */
        wpa_error_log1(MSG_ERROR, "ie_len is too short,ie_len=%zu(<6)", ie_len);
        return old_pos;
    }

    if (ie_hdr == NULL) {
        wpa_error_log0(MSG_ERROR, "ie_hdr is NULL");
        return old_pos;
    }

    p_akm_auit_cnt = ie_hdr + (WAPI_IE_ID_SIZE + WAPI_IE_LENGTH_SIZE + WAPI_IE_VERSION_SIZE);
    akm_suit_cnt = (p_akm_auit_cnt[0] | ((p_akm_auit_cnt[1] << 8) & 0xff00)) & 0xffff;

    wpa_error_log2(MSG_DEBUG, "wpa_supplicant_wapi_ie_txt,ie_len=%zu,akm_suit_cnt=%d", ie_len, akm_suit_cnt);

    if (akm_suit_cnt > 2) {
        wpa_error_log1(MSG_ERROR, "akm_suit_cnt(%d > 2) is error.", akm_suit_cnt);
        return old_pos;
    }

    if (ie_len < (size_t)(WAPI_HEAD_LEN + akm_suit_cnt * WAPI_IE_AKM_SUIT_LEN)) {
        wpa_error_log2(MSG_ERROR, "ie_len(%zu) is short,the right len should more than:%d",
            ie_len, (WAPI_HEAD_LEN + akm_suit_cnt * WAPI_IE_AKM_SUIT_LEN));
        return old_pos;
    }

    p_akm = (p_akm_auit_cnt + WAPI_IE_AKM_CNT_LEN);
    for (i = 0; i < akm_suit_cnt; i++) {
        if (WAPI_IE_AKM_SUIT_PSK == WPA_GET_BE32(p_akm)) {
            ret = snprintf_s(pos, (size_t)(end - pos), (size_t)(end - pos), "[WAPI-PSK]");
            if (os_snprintf_error((size_t)(end - pos), ret)) {
                return pos;
            }
            pos += ret;
        } else {
            wpa_error_log0(MSG_ERROR, "unknow wpai akm_suit");
            return old_pos;
        }

        p_akm += WAPI_IE_AKM_SUIT_LEN;
    }
    return pos;
}
