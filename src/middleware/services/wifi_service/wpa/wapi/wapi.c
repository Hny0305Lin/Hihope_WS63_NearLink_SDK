/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: WAPI Implementation
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
#include "wai_rxtx.h"
#include "wai_crypto.h"
#include "wai_sm.h"

#define WAPI_IE_INFO_AKM_SUITE_TYPE_OFFSET 9

static const wapi_cipher_suite_stru g_wai_cipher_suite[] = {
    {
        .wpa_alg_type    = WPA_ALG_NONE,
        .wpa_cipher_flag = WPA_CIPHER_NONE,
        .wai_cipher_type = CIPHER_TYPE_NONE,
        .wai_decrypt_cb  = NULL,
    },
    {
        .wpa_alg_type    = WPA_ALG_SMS4,
        .wpa_cipher_flag = WPA_CIPHER_SMS4,
        .wai_cipher_type = CIPHER_TYPE_SM4_OFB,
        .wai_decrypt_cb  = wai_sm4_ofb_decrypt,
    },
};

static int g_ucast_cipher_suite = (int)CIPHER_TYPE_NONE;
static int g_mcast_cipher_suite = (int)CIPHER_TYPE_NONE;

void wapi_iface_init(struct wapi_asue_struct *wapi)
{
    if (wapi == NULL)
        return;

    wapi->state = WAISM_INIT;
    wapi->next_frame_seq = 1;
    wapi->tx_frame_seq  = 0;
    (void)memset_s(&wapi->usksa, sizeof(wapi_usksa_stru), 0, sizeof(wapi_usksa_stru));
    (void)memset_s(&wapi->msksa, sizeof(wapi_msksa_stru), 0, sizeof(wapi_msksa_stru));
}

void wapi_iface_deinit(struct wapi_asue_struct *wapi)
{
    if (wapi == NULL)
        return;

    if (wapi->tx_framebuf != NULL) {
        wpabuf_free(wapi->tx_framebuf);
        wapi->tx_framebuf = NULL;
    }

    (void)memset_s(&wapi->bksa,  sizeof(wapi_bksa_stru),  0, sizeof(wapi_bksa_stru));
    (void)memset_s(&wapi->usksa, sizeof(wapi_usksa_stru), 0, sizeof(wapi_usksa_stru));
    (void)memset_s(&wapi->msksa, sizeof(wapi_msksa_stru), 0, sizeof(wapi_msksa_stru));
}

int wapi_init_ie(struct wpa_supplicant *wpa)
{
    /*  Framing in little-endian mode.
     *  OUI is "0x00 0x14 0x72".
     *  Information about the WAPI information element supported by the current device:
     *  Name                        Length          Value(Bytes)
     *  IE                          1               68
     *  Pkg length                  1               22
     *  Version                     2               1
     *  AKM count                   2               1
     *  AKM suite type              4               OUI + 2(Default support PSK)
     *  ucast cipher suite cnt      2               1
     *  ucast cipher suite type     4               OUI + 1(Only support SM4)
     *  mcast cipher suite type     4               OUI + 1(Only support SM4)
     *  WAPI Capability info        2               0x0
     *  Number of BKID              2               0x0
     */
    unsigned char wapi_ie[] = {
        0x44, 0x16, 0x01, 0x00, 0x01, 0x00, 0x00, 0x14,
        0x72, 0x02, 0x01, 0x00, 0x00, 0x14, 0x72, 0x01,
        0x00, 0x14, 0x72, 0x01, 0x00, 0x00, 0x00, 0x00
    };
    struct wapi_asue_struct *wapi = NULL;

    if ((wpa == NULL) || (wpa->wapi == NULL)) {
        wpa_printf(MSG_ERROR, "wapi struct error");
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;
    if (wapi->auth_type == AUTH_TYPE_NONE_WAPI) {
        wpa_printf(MSG_ERROR, "open, needn't set wapi-ie");
        return WAPI_SUCCESS;
    }

    if (memset_s(wapi->assoc_wapi_ie, WAPI_IE_MAX_SIZE, 0, sizeof(wapi->assoc_wapi_ie)) != EOK) {
        wpa_printf(MSG_ERROR, "memset assoc_wapi_ie failed");
        return WAPI_FAILED;
    }
    wapi->assoc_wapi_ie_len = 0;

    if ((unsigned int)wpa->key_mgmt == WPA_KEY_MGMT_WAPI_PSK) {
        wapi_ie[WAPI_IE_INFO_AKM_SUITE_TYPE_OFFSET] = AUTH_TYPE_WAPI_PSK;
    } else if ((unsigned int)wpa->key_mgmt == WPA_KEY_MGMT_WAPI_CERT) {
        wapi_ie[WAPI_IE_INFO_AKM_SUITE_TYPE_OFFSET] = AUTH_TYPE_WAPI_CERT;
    } else {
        wpa_printf(MSG_ERROR, "invalid key_mgmt %#x", (unsigned int)wpa->key_mgmt);
        return WAPI_FAILED;
    }

    if (memcpy_s(wapi->assoc_wapi_ie, WAPI_IE_MAX_SIZE, wapi_ie, sizeof(wapi_ie)) != EOK) {
        wpa_printf(MSG_ERROR, "memcpy wapi ie failed");
        return WAPI_FAILED;
    }
    wapi->assoc_wapi_ie_len = sizeof(wapi_ie);

    return WAPI_SUCCESS;
}

int wapi_parse_ie(const unsigned char *wapi_ie, unsigned int ie_len,
    struct wpa_ie_data *ie_data)
{
    const unsigned char wapi_oui[] = {0x00, 0x14, 0x72};
    wapi_cipher_suite_stru *cipher_suite = NULL;
    unsigned short version;
    unsigned short suite_cnt;
    unsigned char suite_type;
    unsigned char *ptr = NULL;
    unsigned short cnt;
    int left;

    if ((wapi_ie == NULL) || (ie_data == NULL))
        return WAPI_FAILED;

    if (memset_s(ie_data, sizeof(struct wpa_ie_data), 0, sizeof(struct wpa_ie_data)) != EOK)
        return WAPI_FAILED;

    /* default setting */
    ie_data->proto = (int)WPA_PROTO_WAPI;
    ie_data->pairwise_cipher = (int)WPA_CIPHER_SMS4;
    ie_data->group_cipher = (int)WPA_CIPHER_SMS4;
    ie_data->key_mgmt = (int)WPA_KEY_MGMT_WAPI_PSK;
    ie_data->capabilities = 0;
    ie_data->pmkid = NULL;
    ie_data->num_pmkid = 0;
    ie_data->mgmt_group_cipher = 0;

    /* wapi_ie[1]: IE data length */
    if ((ie_len != (wapi_ie[1] + (unsigned int)WAPI_IE_ID_SIZE + (unsigned int)WAPI_IE_LENGTH_SIZE)) ||
        (ie_len < (WAPI_IE_ID_SIZE + WAPI_IE_LENGTH_SIZE + WAPI_IE_VERSION_SIZE + WAPI_IE_AKM_CNT_LEN))) {
        wpa_printf(MSG_ERROR, "ie_len error");
        return WAPI_FAILED;
    }

    ptr = (unsigned char *)wapi_ie + WAPI_IE_ID_SIZE + WAPI_IE_LENGTH_SIZE;
    version = *((unsigned short *)ptr);
    if (version != WAI_VERSION)
        return WAPI_FAILED;

    ptr += WAPI_IE_VERSION_SIZE;

    /* check akm suite type */
    suite_cnt = WPA_GET_LE16(ptr);
    ptr += WAPI_IE_AKM_CNT_LEN;
    left = (int)((((ie_len - WAPI_IE_ID_SIZE) - WAPI_IE_LENGTH_SIZE) - WAPI_IE_VERSION_SIZE) - WAPI_IE_AKM_CNT_LEN);
    for (cnt = 0; cnt < suite_cnt; ++cnt) {
        if (left < (WAPI_IE_OUI_SIZE + WAPI_IE_AKM_SUIT_LEN))
            return WAPI_FAILED;

        /* check akm suite oui */
        if (os_memcmp(ptr, wapi_oui, sizeof(wapi_oui)) != 0)
            return WAPI_FAILED;

        ptr += WAPI_IE_OUI_SIZE;
        suite_type = *ptr;

        wpa_printf(MSG_DEBUG, "akm suite %#x", (unsigned int)suite_type);

        if (suite_type == (int)AUTH_TYPE_WAPI_PSK)
            ie_data->key_mgmt |= (int)WPA_KEY_MGMT_WAPI_PSK;
        else if (suite_type == (int)AUTH_TYPE_WAPI_CERT)
            ie_data->key_mgmt |= (int)WPA_KEY_MGMT_WAPI_CERT;
        else
            return WAPI_FAILED;

        ptr += WAPI_IE_AKM_SUIT_LEN;
        left -= WAPI_IE_OUI_SIZE + WAPI_IE_AKM_SUIT_LEN;
    }

    if (left < WAPI_IE_CIPHER_CNT_LEN)
        return WAPI_FAILED;

    /* check ucast cipher suite type */
    suite_cnt = WPA_GET_LE16(ptr);
    ptr += WAPI_IE_CIPHER_CNT_LEN;
    left -= WAPI_IE_CIPHER_CNT_LEN;
    for (cnt = 0; cnt < suite_cnt; ++cnt) {
        if (left < (WAPI_IE_OUI_SIZE + WAPI_IE_CIPHER_SUIT_LEN))
            return WAPI_FAILED;

        /* check akm suite oui */
        if (os_memcmp(ptr, wapi_oui, sizeof(wapi_oui)) != 0)
            return WAPI_FAILED;

        ptr += WAPI_IE_OUI_SIZE;
        suite_type = *ptr;
        wpa_printf(MSG_DEBUG, "ucast cipher suite %#x", (unsigned int)suite_type);

        if (suite_type >= (unsigned int)CIPHER_TYPE_MAX)
            return WAPI_FAILED;

        cipher_suite = (wapi_cipher_suite_stru *)&g_wai_cipher_suite[suite_type];
        ie_data->pairwise_cipher |= cipher_suite->wpa_cipher_flag;
        g_ucast_cipher_suite = (int)suite_type;
        ptr += WAPI_IE_CIPHER_SUIT_LEN;
        left -= WAPI_IE_OUI_SIZE + WAPI_IE_CIPHER_SUIT_LEN;
    }
    ie_data->has_pairwise = (int)suite_cnt;

    if (left < (WAPI_IE_OUI_SIZE + WAPI_IE_CIPHER_SUIT_LEN))
        return WAPI_FAILED;

    /* check akm suite oui */
    if (os_memcmp(ptr, wapi_oui, sizeof(wapi_oui)) != 0)
        return WAPI_FAILED;

    ptr += WAPI_IE_OUI_SIZE;

    /* check mcast cipher suite type */
    suite_type = *ptr;
    wpa_printf(MSG_DEBUG, "mcast cipher suite %#x", (unsigned int)suite_type);

    if (suite_type >= (unsigned int)CIPHER_TYPE_MAX)
        return WAPI_FAILED;

    cipher_suite = (wapi_cipher_suite_stru *)&g_wai_cipher_suite[suite_type];
    ie_data->group_cipher |= cipher_suite->wpa_cipher_flag;
    g_mcast_cipher_suite = (int)suite_type;
    ie_data->has_group = 1;

    wpa_printf(MSG_DEBUG, "parse wapi info element success");
    return WAPI_SUCCESS;
}

int wapi_generate_addid(struct wapi_asue_struct *wapi, struct wpa_bss *bss)
{
    if ((wapi == NULL) || (bss == NULL))
        return WAPI_FAILED;

    if (memcpy_s(wapi->addid, WAI_ADDID_SIZE, bss->bssid, ETH_ALEN) != EOK) {
        wpa_error_log0(MSG_WARNING, "memcpy ae mac failed");
        return WAPI_FAILED;
    }
    if (memcpy_s(&wapi->addid[ETH_ALEN], (WAI_ADDID_SIZE - ETH_ALEN), wapi->own_mac, ETH_ALEN) != EOK) {
        wpa_error_log0(MSG_WARNING, "memcpy asue mac failed");
        return WAPI_FAILED;
    }
    return WAPI_SUCCESS;
}

int wapi_event_process(struct wapi_asue_struct *wapi, conn_status_enum action,
    const unsigned char *assoc_ie, size_t assoc_ie_len)
{
    unsigned char bkid[WAI_BKID_SIZE] = { 0 };

    if (wapi == NULL ||
        (assoc_ie != NULL && assoc_ie_len == 0) ||
        (assoc_ie == NULL && assoc_ie_len != 0)) {
        wpa_printf(MSG_WARNING, "param is null");
        return WAPI_FAILED;
    }

    wpa_printf(MSG_DEBUG, "action %u auth_type %u assoc_ie_len %u",
        (unsigned int)action, (unsigned int)wapi->auth_type, assoc_ie_len);

    if (action == CONN_ASSOC) {
        if (wapi->auth_type == AUTH_TYPE_WAPI_PSK) {
            if (wai_kd_hmac_sha256(wapi->addid, WAI_ADDID_SIZE, wapi->bk, WAI_BK_SIZE,
                bkid, WAI_BKID_SIZE) != WAPI_SUCCESS) {
                forced_memzero(bkid, WAI_BKID_SIZE);
                wpa_printf(MSG_WARNING, "hmac_sha256 calc bkid failed");
                return WAPI_FAILED;
            }
            if (memcpy_s(wapi->bksa.bk, WAI_BK_SIZE, wapi->bk, WAI_BK_SIZE) != EOK) {
                forced_memzero(bkid, WAI_BKID_SIZE);
                forced_memzero(&wapi->bksa, sizeof(wapi_bksa_stru));
                wpa_printf(MSG_WARNING, "memcpy bk failed");
                return WAPI_FAILED;
            }
            if (memcpy_s(wapi->bksa.bkid, WAI_BKID_SIZE, bkid, WAI_BKID_SIZE) != EOK) {
                forced_memzero(bkid, WAI_BKID_SIZE);
                forced_memzero(&wapi->bksa, sizeof(wapi_bksa_stru));
                wpa_printf(MSG_WARNING, "memcpy bkid failed");
                return WAPI_FAILED;
            }
            forced_memzero(bkid, WAI_BKID_SIZE);
            if (memcpy_s(wapi->bksa.ae_mac, ETH_ALEN, wapi->bssid, ETH_ALEN) != EOK) {
                forced_memzero(&wapi->bksa, sizeof(wapi_bksa_stru));
                wpa_printf(MSG_WARNING, "memcpy ae mac failed");
                return WAPI_FAILED;
            }
            if (memcpy_s(wapi->bksa.asue_mac, ETH_ALEN, wapi->own_mac, ETH_ALEN) != EOK) {
                forced_memzero(&wapi->bksa, sizeof(wapi_bksa_stru));
                wpa_printf(MSG_WARNING, "memcpy asue mac failed");
                return WAPI_FAILED;
            }
        }

        if (wapi->auth_type != AUTH_TYPE_NONE_WAPI) {
            if (assoc_ie_len > (sizeof(wapi->wapi_ie) - 1))
                assoc_ie_len = sizeof(wapi->wapi_ie) - 1;

            if (memcpy_s(wapi->wapi_ie, WAPI_IE_MAX_SIZE, assoc_ie, assoc_ie_len) != EOK) {
                forced_memzero(&wapi->bksa, sizeof(wapi_bksa_stru));
                wpa_printf(MSG_WARNING, "memcpy wapi ie failed");
                return WAPI_FAILED;
            }
            wapi->wapi_ie_len = assoc_ie_len;
        }
        /* The BK cache is cleared only after the ASSOC RSP is successful,
         * because the BK cache will still be used in retransmission. */
        (void)memset_s(wapi->bk, WAI_BK_SIZE, 0, WAI_BK_SIZE);

        /* Reset state when new association start */
        wapi->state = WAISM_INIT;
        wapi->next_frame_seq = 1;
        wapi->tx_frame_seq  = 0;
        (void)memset_s(&wapi->usksa, sizeof(wapi_usksa_stru), 0, sizeof(wapi_usksa_stru));
        (void)memset_s(&wapi->msksa, sizeof(wapi_msksa_stru), 0, sizeof(wapi_msksa_stru));

        wapi->ucast_cipher_suite = &g_wai_cipher_suite[g_ucast_cipher_suite];
        wapi->mcast_cipher_suite = &g_wai_cipher_suite[g_mcast_cipher_suite];
        wapi->state = WAISM_ALREADY_ASSOC;
    } else if (action == CONN_DISASSOC) {
        wapi->state = WAISM_INIT;
        (void)memset_s(wapi->bk, WAI_BK_SIZE, 0, WAI_BK_SIZE);
        (void)memset_s(&wapi->bksa,  sizeof(wapi_bksa_stru),  0, sizeof(wapi_bksa_stru));
        (void)memset_s(&wapi->usksa, sizeof(wapi_usksa_stru), 0, sizeof(wapi_usksa_stru));
        (void)memset_s(&wapi->msksa, sizeof(wapi_msksa_stru), 0, sizeof(wapi_msksa_stru));
    }

    return WAPI_SUCCESS;
}
