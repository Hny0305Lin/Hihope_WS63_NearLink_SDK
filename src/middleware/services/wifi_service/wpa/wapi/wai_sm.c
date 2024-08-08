/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: WAPI Implementation
 */

#include "securec.h"
#include "wapi.h"
#include "wai_rxtx.h"
#include "wai_crypto.h"
#include "crypto.h"
#include "utils/includes.h"
#include "utils/common.h"
#include "common/defs.h"
#include "l2_packet/l2_packet.h"
#include "driver_i.h"

#define DEAUTH_REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT 15

static int wai_recv_usk_nego_request(struct wpa_supplicant *wpa,
    const unsigned char *payload, unsigned int payload_len);
static int wai_recv_usk_nego_confirm(struct wpa_supplicant *wpa,
    const unsigned char *payload, unsigned int payload_len);
static int wai_recv_msk_announce_request(struct wpa_supplicant *wpa,
    const unsigned char *payload, unsigned int payload_len);
static int wai_send_usk_nego_response(struct wpa_supplicant *wpa, const unsigned char *ae_nonce);
static int wai_send_msk_announce_response(struct wpa_supplicant *wpa,
    const unsigned char *payload, unsigned int payload_len);
static int wai_bk_to_usk(struct wapi_asue_struct *wapi, const unsigned char *ae_nonce);
static int wai_nmk_to_msk(const unsigned char *nmk, unsigned int nmklen,
    unsigned char *msk, unsigned int msklen);
static int wai_install_usk(struct wpa_supplicant *wpa);
static int wai_install_msk(struct wpa_supplicant *wpa, const unsigned char *msk);

static const wai_sm_hdl_stru g_usk_nego_req_hdl = {
    .msgid = WAI_USK_NEGOTIATION_REQUEST,
    .handler = wai_recv_usk_nego_request
};

static const wai_sm_hdl_stru g_usk_nego_cfm_hdl = {
    .msgid = WAI_USK_NEGOTIATION_CONFIRM,
    .handler = wai_recv_usk_nego_confirm
};

static const wai_sm_hdl_stru g_msk_anno_req_hdl = {
    .msgid = WAI_MSK_ANNOUNCEMENT,
    .handler = wai_recv_msk_announce_request
};

static const wai_sm_hdl_stru *g_waism_init[] = {
    NULL,
};

static const wai_sm_hdl_stru *g_waism_already_assoc[] = {
    &g_usk_nego_req_hdl,
    NULL,
};

static const wai_sm_hdl_stru *g_waism_usk_neg_res[] = {
    &g_usk_nego_cfm_hdl,
    NULL,
};

static const wai_sm_hdl_stru *g_waism_usk_neg_confirm[] = {
    &g_msk_anno_req_hdl,
    NULL,
};

static const wai_sm_hdl_stru *g_waism_finished[] = {
    &g_usk_nego_req_hdl,
    &g_msk_anno_req_hdl,
    NULL,
};

static const wai_sm_hdl_stru **g_wai_sm[] = {
    g_waism_init,
    g_waism_already_assoc,
    g_waism_usk_neg_res,
    g_waism_usk_neg_confirm,
    g_waism_finished
};

static void wai_sm_set_state(struct wapi_asue_struct *wapi, wai_state_enum new_state)
{
    wpa_printf(MSG_DEBUG, "wapi sm state change %u to %u", (unsigned int)wapi->state, (unsigned int)new_state);
    wapi->state = new_state;
}

static void wai_timeout_retry_tx(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_supplicant *wpa = (struct wpa_supplicant *)eloop_ctx;
    int ret;

    (void)timeout_ctx;

    if (wpa == NULL || wpa->wapi == NULL || wpa->wapi->tx_framebuf == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return;
    }

    wpa->wapi->tx_count++;
    if (wpa->wapi->tx_count <= WAI_MAX_TX_COUNT) {
        ret = wai_tx_packet(wpa, wpabuf_mhead_u8(wpa->wapi->tx_framebuf),
            wpabuf_len(wpa->wapi->tx_framebuf));
        if (ret != WAPI_SUCCESS)
            wpa_printf(MSG_WARNING, "Fail to send retry-frame");
        else
            wpa_printf(MSG_INFO, "success to send retry-frame");

        (void)eloop_cancel_timeout(wai_timeout_retry_tx, wpa, NULL);
        (void)eloop_register_timeout(1, 0, wai_timeout_retry_tx, wpa, NULL);
    } else {
        wpa_printf(MSG_WARNING, "Fail to resend response frame many times, now disassociate");
        wpa_supplicant_deauthenticate(wpa, DEAUTH_REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT);
    }
}

static void wai_register_timeout_retry_tx(struct wpa_supplicant *wpa,
    unsigned int timeout, struct wpabuf *framebuf)
{
    if (wpa->wapi->tx_framebuf != NULL) {
        wpabuf_free(wpa->wapi->tx_framebuf);
    }
    wpa->wapi->tx_framebuf = framebuf;
    wpa->wapi->tx_count = 1;

    (void)eloop_cancel_timeout(wai_timeout_retry_tx, wpa, NULL);
    (void)eloop_register_timeout(timeout, 0, wai_timeout_retry_tx, wpa, NULL);
}

static void wai_cancel_timeout_retry_tx(struct wpa_supplicant *wpa)
{
    if (wpa->wapi->tx_framebuf != NULL) {
        wpabuf_free(wpa->wapi->tx_framebuf);
        wpa->wapi->tx_framebuf = NULL;
    }

    (void)eloop_cancel_timeout(wai_timeout_retry_tx, wpa, NULL);
}

static int wai_sm_dispose(struct wpa_supplicant *wpa, const struct wpabuf *waibuf)
{
    wai_hdr_stru *hdr = NULL;
    wai_sm_hdl_stru **hdl = NULL;
    unsigned char subtype;
    int ret = WAPI_FAILED;
    int loop;

    hdr = (wai_hdr_stru *)wpabuf_head(waibuf);
    if (hdr == NULL) {
        wpa_printf(MSG_ERROR, "waibuf head is null");
        return ret;
    }
    if (wpa->wapi->state > WAISM_FINSHED) {
        wpa_printf(MSG_ERROR, "invalid state %u", (unsigned int)wpa->wapi->state);
        return ret;
    }
    subtype = hdr->subtype;
    hdl = (wai_sm_hdl_stru **)g_wai_sm[wpa->wapi->state];
    for (loop = 0; hdl[loop] != NULL; ++loop) {
        if (hdl[loop]->msgid != subtype)
            continue;

        if (hdl[loop]->handler != NULL)
            ret = hdl[loop]->handler(wpa, (unsigned char *)&hdr[1], wpabuf_len(waibuf) - sizeof(wai_hdr_stru));

        break;
    }
    return ret;
}

int wai_sm_process(struct wpa_supplicant *wpa, const struct wpabuf *waibuf)
{
    int ret;

    if (wpa == NULL || wpa->wapi == NULL || waibuf == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    ret = wai_sm_dispose(wpa, waibuf);
    if (ret == WAPI_FAILED) {
        wpa_printf(MSG_WARNING, "Frame subtype is wrong or the frame is not expected.");
    }

    return ret;
}

static int wai_validate_usk_nego_req_state(unsigned int update, wai_state_enum state)
{
    if (((update != 0) && (state != WAISM_FINSHED)) ||
        ((update == 0) && (state != WAISM_ALREADY_ASSOC))) {
        return WAPI_FAILED;
    }
    return WAPI_SUCCESS;
}

static int wai_recv_usk_nego_request(struct wpa_supplicant *wpa,
    const unsigned char *payload, unsigned int payload_len)
{
    struct wapi_asue_struct *wapi = NULL;
    unsigned int offset = 0;
    unsigned char *ae_nonce = NULL;
    unsigned int update;
    if (payload == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;

    if (payload_len != (WAI_FLAG_SIZE + WAI_BKID_SIZE + WAI_USKID_SIZE + WAI_ADDID_SIZE + WAI_CHALLENGE_SIZE)) {
        wpa_printf(MSG_ERROR, "WAI usk negotiation len error");
        return WAPI_FAILED;
    }
    if (payload[offset] & ~WAI_FLAG_USK_UPDATE) {
        wpa_printf(MSG_ERROR, "WAI usk negotiation flag error");
        return WAPI_FAILED;
    }
    wapi->wai_flag = payload[offset];
    wpa_printf(MSG_DEBUG, "usk_nego_req flag %#x", (unsigned int)wapi->wai_flag);

    update = (wapi->wai_flag & WAI_FLAG_USK_UPDATE);
    offset += WAI_FLAG_SIZE;

    if (wai_validate_usk_nego_req_state(update, wapi->state) != WAPI_SUCCESS) {
        wpa_printf(MSG_ERROR, "receive usk negotiation on wrong state(%u) update %u", (unsigned int)wapi->state,
            update);
        return WAPI_FAILED;
    }

    if (os_memcmp(&payload[offset], wapi->bksa.bkid, WAI_BKID_SIZE) != 0) {
        wpa_printf(MSG_WARNING, "BKID mistake, disassociate");
        return WAPI_FAILED;
    }
    offset += WAI_BKID_SIZE;

    if (payload[offset] >= WAI_USKSA_CNT) {
        wpa_printf(MSG_ERROR, "uskid %u error", (unsigned int)payload[offset]);
        return WAPI_FAILED;
    }
    wpa_printf(MSG_DEBUG, "update %#x", update);
    if (update) {
        /* if update flag is 1, uskid should not be equal to the saved */
        if (payload[offset] == wapi->usksa.uskid) {
            wpa_printf(MSG_ERROR, "uskid error");
            return WAPI_FAILED;
        }
        /* ae challenge should be equal to the last usk calculated. */
        if (os_memcmp(&payload[(unsigned int)(offset + WAI_USKID_SIZE + WAI_ADDID_SIZE)],
            wapi->ae_next_nonce, WAI_CHALLENGE_SIZE) != 0) {
            wpa_printf(MSG_ERROR, "ae challenge error");
            return WAPI_FAILED;
        }
    }

    wapi->usksa.uskid = payload[offset];
    wpa_printf(MSG_DEBUG, "set uskid %u", (unsigned int)wapi->usksa.uskid);
    offset += WAI_USKID_SIZE;

    if (memcpy_s(wapi->addid, WAI_ADDID_SIZE, &payload[offset], WAI_ADDID_SIZE) != EOK) {
        wpa_printf(MSG_WARNING, "memcpy addid failed");
        return WAPI_FAILED;
    }
    offset += WAI_ADDID_SIZE;

    /* Generate ASUE challenge */
    if (os_get_random(wapi->asue_nonce, WAI_CHALLENGE_SIZE) < 0) {
        wpa_printf(MSG_WARNING, "generate random fail");
        return WAPI_FAILED;
    }
    /* Get AE challenge */
    ae_nonce = (unsigned char *)&payload[offset];

    if (wai_bk_to_usk(wapi, ae_nonce) != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "wai_bk_to_usk fail");
        return WAPI_FAILED;
    }

    if (wai_send_usk_nego_response(wpa, ae_nonce) != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "send usknego response frame fail");
        return WAPI_FAILED;
    }

    wai_sm_set_state(wapi, WAISM_USKNEG_RES);
    wpa_supplicant_set_state(wpa, WPA_4WAY_HANDSHAKE);
    return WAPI_SUCCESS;
}

static void wai_encap_usk_nego_rsp_frame(struct wpabuf *framebuf,
    const struct wapi_asue_struct *wapi, const unsigned char *ae_nonce)
{
    (void)wpabuf_put(framebuf, sizeof(wai_hdr_stru));
    wpabuf_put_u8(framebuf, wapi->wai_flag);
    wpabuf_put_data(framebuf, wapi->bksa.bkid, WAI_BKID_SIZE);
    wpabuf_put_u8(framebuf, wapi->usksa.uskid);
    wpabuf_put_data(framebuf, wapi->addid, WAI_ADDID_SIZE);
    wpabuf_put_data(framebuf, wapi->asue_nonce, WAI_CHALLENGE_SIZE);
    wpabuf_put_data(framebuf, ae_nonce, WAI_CHALLENGE_SIZE);
    wpabuf_put_data(framebuf, wapi->assoc_wapi_ie, wapi->assoc_wapi_ie_len);
}

static int wai_setup_eth_header(const struct wapi_asue_struct *wapi, struct wpabuf *framebuf)
{
    struct l2_ethhdr *l2_ethhdr_ptr = (struct l2_ethhdr *)wpabuf_mhead_u8(framebuf);

    if (memcpy_s(l2_ethhdr_ptr->h_dest, ETH_ALEN, wapi->bssid, ETH_ALEN) != EOK) {
        wpa_printf(MSG_WARNING, "cpy dst addr failed");
        return WAPI_FAILED;
    }
    if (memcpy_s(l2_ethhdr_ptr->h_source, ETH_ALEN, wapi->own_mac, ETH_ALEN) != EOK) {
        wpa_printf(MSG_WARNING, "cpy src addr failed");
        return WAPI_FAILED;
    }
    l2_ethhdr_ptr->h_proto = host_to_be16(ETH_TYPE_WAI);

    /* Reserve ethnet header */
    (void)wpabuf_put(framebuf, sizeof(struct l2_ethhdr));

    return WAPI_SUCCESS;
}

static void wai_setup_wai_header(unsigned char *frame, unsigned short framelen,
    unsigned short frameseq, unsigned char subtype)
{
    wai_hdr_stru *wai_hdr = (wai_hdr_stru *)frame;
    if (wai_hdr == NULL)
        return;

    WPA_PUT_BE16(wai_hdr->version, WAI_VERSION);
    wai_hdr->type = WAI_TYPE;
    wai_hdr->subtype = subtype;
    WPA_PUT_BE16(wai_hdr->reserve, 0);
    WPA_PUT_BE16(wai_hdr->length, framelen);
    WPA_PUT_BE16(wai_hdr->frameseq, frameseq);
    wai_hdr->fragseq = 0;
    wai_hdr->flag = 0;
}

static int wai_send_usk_nego_response(struct wpa_supplicant *wpa, const unsigned char *ae_nonce)
{
    struct wapi_asue_struct *wapi = NULL;
    struct wpabuf *framebuf = NULL;
    unsigned char *framedata = NULL;
    unsigned char mic[WAI_MIC_SIZE] = { 0 };
    size_t framelen;
    int ret;

    if (ae_nonce == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;

    framelen = sizeof(struct l2_ethhdr) + sizeof(wai_hdr_stru) + WAI_FLAG_SIZE + WAI_BKID_SIZE +
        WAI_USKID_SIZE + WAI_ADDID_SIZE + WAI_CHALLENGE_SIZE + WAI_CHALLENGE_SIZE +
        WAPI_IE_MAX_SIZE + WAI_MIC_SIZE;

    framebuf = wpabuf_alloc(framelen);
    if (framebuf == NULL) {
        wpa_printf(MSG_ERROR, "Fail to zalloc memory for usknego response frame");
        return WAPI_FAILED;
    }

    if (wai_setup_eth_header(wapi, framebuf) != WAPI_SUCCESS) {
        wpabuf_free(framebuf);
        wpa_printf(MSG_ERROR, "fill ethnet header failed");
        return WAPI_FAILED;
    }

    wai_encap_usk_nego_rsp_frame(framebuf, wapi, ae_nonce);

    framedata = wpabuf_mhead_u8(framebuf) + sizeof(struct l2_ethhdr) + sizeof(wai_hdr_stru);
    framelen = (wpabuf_len(framebuf) - sizeof(struct l2_ethhdr)) - sizeof(wai_hdr_stru);
    wpa_printf(MSG_DEBUG, "calu MAC framelen %u", framelen);

    ret = wai_hmac_sha256(framedata, (unsigned int)framelen,
        wapi->usksa.usk.mak, WAI_MAK_SIZE,
        mic, WAI_MIC_SIZE);
    if (ret != WAPI_SUCCESS) {
        wpabuf_free(framebuf);
        wpa_printf(MSG_ERROR, "failed to get mic for usknego response frame");
        return WAPI_FAILED;
    }

    wpabuf_put_data(framebuf, mic, WAI_MIC_SIZE);

    /* Allowing for overflow */
    wapi->tx_frame_seq++;

    framedata = wpabuf_mhead_u8(framebuf) + sizeof(struct l2_ethhdr);
    framelen = wpabuf_len(framebuf) - sizeof(struct l2_ethhdr);
    wai_setup_wai_header(framedata, (unsigned short)framelen, wapi->tx_frame_seq, WAI_USK_NEGOTIATION_RESPONSE);

    ret = wai_tx_packet(wpa, wpabuf_mhead_u8(framebuf), wpabuf_len(framebuf));
    if (ret != WAPI_SUCCESS) {
        wpabuf_free(framebuf);
        wpa_printf(MSG_ERROR, "Fail to send usknego response frame");
        return WAPI_FAILED;
    }

    wai_register_timeout_retry_tx(wpa, 1, framebuf);
    return WAPI_SUCCESS;
}

static unsigned char wai_validate_usk_nego_cfm_state(const struct wapi_asue_struct *wapi)
{
    return (wapi->state != WAISM_USKNEG_RES);
}

static void wai_update_sm_state(unsigned char updated, struct wpa_supplicant *wpa,
    struct wapi_asue_struct *wapi)
{
    if (updated) {
        wpa_supplicant_set_state(wpa, WPA_COMPLETED);
        wpa_supplicant_cancel_auth_timeout(wpa);
        wai_sm_set_state(wapi, WAISM_FINSHED);
    } else {
        wpa_supplicant_set_state(wpa, WPA_GROUP_HANDSHAKE);
        wai_sm_set_state(wapi, WAISM_USKNEG_CONFIRM);
    }
}

static int wai_recv_usk_nego_confirm(struct wpa_supplicant *wpa,
    const unsigned char *payload, unsigned int payload_len)
{
    struct wapi_asue_struct *wapi = NULL;
    unsigned int offset;
    unsigned int ie_len;
    unsigned short wapi_ie_len;
    unsigned char mic[WAI_MIC_SIZE] = { 0 };
    unsigned char updated;

    if (payload == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;

    /* notify the higher software with the current state */
    wpa_supplicant_set_state(wpa, WPA_4WAY_HANDSHAKE);

    offset = WAI_FLAG_SIZE + WAI_BKID_SIZE + WAI_USKID_SIZE + WAI_ADDID_SIZE + WAI_CHALLENGE_SIZE;
    if ((offset + WAPI_IE_ID_SIZE) >= payload_len) {
        wpa_printf(MSG_ERROR, "error length");
        return WAPI_FAILED;
    }
    wapi_ie_len = (unsigned short)(payload[offset + WAPI_IE_ID_SIZE] + WAPI_IE_ID_SIZE + WAPI_IE_LENGTH_SIZE);
    ie_len = offset + wapi_ie_len + WAI_MIC_SIZE;
    if (ie_len != payload_len) {
        wpa_printf(MSG_ERROR, "error length");
        return WAPI_FAILED;
    }

    offset = 0;
    if (payload[offset] != wapi->wai_flag) {
        wpa_printf(MSG_ERROR, "error flag");
        return WAPI_FAILED;
    }
    updated = payload[offset] & WAI_FLAG_USK_UPDATE;
    wpa_printf(MSG_DEBUG, "updated %#x", (unsigned int)updated);
    offset += WAI_FLAG_SIZE;

    if (wai_validate_usk_nego_cfm_state(wapi)) {
        wpa_printf(MSG_ERROR, "received usknego confirm on error state. state:%u update:%u",
            (unsigned int)wapi->state, (unsigned int)updated);
        return WAPI_FAILED;
    }

    if (os_memcmp(&payload[offset], wapi->bksa.bkid, WAI_BKID_SIZE) != 0) {
        wpa_printf(MSG_ERROR, "error bkid");
        return WAPI_FAILED;
    }
    offset += WAI_BKID_SIZE;

    wpa_printf(MSG_DEBUG, "rcv uskid %u", (unsigned int)payload[offset]);
    if (payload[offset] != wapi->usksa.uskid) {
        wpa_printf(MSG_ERROR, "error uskid");
        return WAPI_FAILED;
    }
    offset += WAI_USKID_SIZE;

    if (os_memcmp(&payload[offset], wapi->addid, WAI_ADDID_SIZE) != 0) {
        wpa_printf(MSG_ERROR, "error addid");
        return WAPI_FAILED;
    }
    offset += WAI_ADDID_SIZE;

    if (os_memcmp(&payload[offset], wapi->asue_nonce, WAI_CHALLENGE_SIZE) != 0) {
        wpa_printf(MSG_ERROR, "error ause_nonce");
        return WAPI_FAILED;
    }
    offset += WAI_CHALLENGE_SIZE;
    offset += wapi_ie_len;

    /* generate the MIC2 and then compare it with the MIC */
    if (wai_hmac_sha256((const unsigned char *)payload, offset,
        wapi->usksa.usk.mak, WAI_BK_SIZE,
        mic, WAI_MIC_SIZE) != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "calc mic failed");
        return WAPI_FAILED;
    }

    if (os_memcmp(mic, &payload[offset], WAI_MIC_SIZE) != 0) {
        wpa_printf(MSG_ERROR, "error mic");
        return WAPI_FAILED;
    }

    if (wai_install_usk(wpa)) {
        wpa_printf(MSG_ERROR, "wai install usk fail, disassociate");
        wpa_supplicant_deauthenticate(wpa, DEAUTH_REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT);
        return WAPI_FAILED;
    }

    wai_cancel_timeout_retry_tx(wpa);

    wai_update_sm_state(updated, wpa, wapi);
    return WAPI_SUCCESS;
}

static int wai_validate_addid(struct wapi_asue_struct *wapi,
    const unsigned char *addid)
{
    if (addid == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    if (os_memcmp(wapi->bssid, addid, ETH_ALEN) != 0 ||
        os_memcmp(wapi->own_mac, addid + ETH_ALEN, ETH_ALEN) != 0) {
        wpa_printf(MSG_WARNING, "addid error");
        return WAPI_FAILED;
    }

    return WAPI_SUCCESS;
}

static int wai_bignum_cmp(const unsigned char *x, const unsigned char *y, size_t size, int *status)
{
    struct crypto_bignum *big_x = NULL;
    struct crypto_bignum *big_y = NULL;
    int ret;

    big_x = crypto_bignum_init_set(x, size);
    if (big_x == NULL) {
        *status = WAPI_FAILED;
        return 0;
    }
    big_y = crypto_bignum_init_set(y, size);
    if (big_y == NULL) {
        crypto_bignum_deinit(big_x, 0);
        *status = WAPI_FAILED;
        return 0;
    }

    ret = crypto_bignum_cmp(big_x, big_y);
    crypto_bignum_deinit(big_x, 0);
    crypto_bignum_deinit(big_y, 0);
    *status = WAPI_SUCCESS;
    return ret;
}

static int wai_seq_pn_validate(unsigned char *curr, const unsigned char *next, size_t size)
{
    int status;
    int ret;

    ret = wai_bignum_cmp(curr, next, size, &status);
    if (status != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "bignum cmp failed");
        return WAPI_FAILED;
    }
    if (ret >= 0) {
        wpa_printf(MSG_WARNING, "not increase");
        return WAPI_FAILED;
    }
    if (memcpy_s(curr, size, next, size) != EOK) {
        wpa_printf(MSG_WARNING, "memcpy failed");
        return WAPI_FAILED;
    }
    return WAPI_SUCCESS;
}

static int wai_anno_iv_validate(unsigned char *curr, const unsigned char *next, size_t size)
{
    int status;
    int ret;

    ret = wai_bignum_cmp(curr, next, size, &status);
    if (status != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "bignum cmp failed");
        return WAPI_FAILED;
    }
    if (ret > 0) {
        wpa_printf(MSG_WARNING, "should not decrease");
        return WAPI_FAILED;
    }
    if (memcpy_s(curr, size, next, size) != EOK) {
        wpa_printf(MSG_WARNING, "memcpy failed");
        return WAPI_FAILED;
    }
    return WAPI_SUCCESS;
}

static int wai_recv_msk_anno_install_msk(struct wpa_supplicant *wpa,
    const unsigned char *keydata, unsigned short keydata_len)
{
    struct wapi_asue_struct *wapi = NULL;
    unsigned char nmk[WAI_NMK_SIZE] = { 0 };
    unsigned char msk[WAI_MSK_SIZE] = { 0 };
    unsigned int nmksize = 0;
    int ret;

    wapi = wpa->wapi;

    if (wapi->ucast_cipher_suite->wai_decrypt_cb == NULL) {
        wpa_printf(MSG_WARNING, "wai decrypt function error");
        return WAPI_FAILED;
    }

    if (wapi->ucast_cipher_suite->wai_decrypt_cb(
        (const unsigned char *)wapi->msksa.msk_anno_iv, WAI_MSK_ANNO_IV_SIZE,
        (const unsigned char *)wapi->usksa.usk.kek, WAI_KEK_SIZE,
        keydata, keydata_len, nmk, &nmksize) != WAPI_SUCCESS) {
        forced_memzero(nmk, WAI_NMK_SIZE);
        wpa_printf(MSG_WARNING, "wai decrypt fail");
        return WAPI_FAILED;
    }

    if (nmksize != WAI_NMK_SIZE) {
        forced_memzero(nmk, WAI_NMK_SIZE);
        wpa_printf(MSG_WARNING, "nmk decrypt size error");
        return WAPI_FAILED;
    }

    if (wai_nmk_to_msk(nmk, WAI_NMK_SIZE, msk, WAI_MSK_SIZE) != WAPI_SUCCESS) {
        forced_memzero(nmk, WAI_NMK_SIZE);
        wpa_supplicant_deauthenticate(wpa, DEAUTH_REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT);
        wpa_printf(MSG_WARNING, "derivate msk fail");
        return WAPI_FAILED;
    }
    forced_memzero(nmk, WAI_NMK_SIZE);

    ret = wai_install_msk(wpa, msk);
    forced_memzero(msk, WAI_MSK_SIZE);
    if (ret != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "wai install msk failure, disassociate");
        wpa_supplicant_deauthenticate(wpa, DEAUTH_REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT);
        return WAPI_FAILED;
    }

    return WAPI_SUCCESS;
}

static int wai_recv_msk_announce_request(struct wpa_supplicant *wpa,
    const unsigned char *payload, unsigned int payload_len)
{
    struct wapi_asue_struct *wapi = NULL;
    unsigned int offset;
    unsigned char mic[WAI_MIC_SIZE] = { 0 };
    unsigned char *keydata = NULL;
    unsigned short keydata_len;

    if (wpa->wapi->ucast_cipher_suite == NULL || payload == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    offset = WAI_FLAG_SIZE + WAI_MSKID_SIZE + WAI_USKID_SIZE + WAI_ADDID_SIZE +
        WAI_DATA_SERIAL_NUMBER + WAI_MSK_ANNO_IV_SIZE;
    keydata_len = payload[offset];

    if (payload_len != (WAI_FLAG_SIZE + WAI_MSKID_SIZE + WAI_USKID_SIZE + WAI_ADDID_SIZE +
        WAI_DATA_SERIAL_NUMBER + WAI_MSK_ANNO_IV_SIZE + WAI_KEY_LENGTH_SIZE + keydata_len + WAI_MIC_SIZE)) {
        wpa_printf(MSG_WARNING, "msk announce data len is short.(%u)", payload_len);
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;
    offset = 0;

    if ((payload[offset] & WAI_FLAG_STAKEY_NEG) ||
        (payload[offset] & WAI_FLAG_STAKEY_DEL)) {
        wpa_printf(MSG_WARNING, "stakey is not supptable");
        return WAPI_FAILED;
    }

    wapi->wai_flag = payload[offset];
    offset += WAI_FLAG_SIZE;

    wapi->msksa.mskid = payload[offset];
    offset += WAI_MSKID_SIZE;

    if (payload[offset] != wapi->usksa.uskid) {
        wpa_printf(MSG_WARNING, "annoucement uskid is wrong");
        return WAPI_FAILED;
    }
    offset += WAI_USKID_SIZE;

    if (wai_validate_addid(wapi, &payload[offset]) != 0) {
        wpa_printf(MSG_WARNING, "annoucement identfier is wrong");
        return WAPI_FAILED;
    }
    offset += WAI_ADDID_SIZE;

    if (wai_seq_pn_validate(wapi->msksa.msk_seq_pn, (unsigned char *)&payload[offset],
        WAI_DATA_SERIAL_NUMBER) != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "parse msk_seq_pn failed");
        return WAPI_FAILED;
    }
    offset += WAI_DATA_SERIAL_NUMBER;

    if (wai_anno_iv_validate(wapi->msksa.msk_anno_iv, (unsigned char *)&payload[offset],
        WAI_MSK_ANNO_IV_SIZE) != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "parse msk_anno_iv failed");
        return WAPI_FAILED;
    }
    offset += WAI_MSK_ANNO_IV_SIZE;

    if (keydata_len > WAI_NMK_SIZE) {
        wpa_printf(MSG_WARNING, "key data len %u error", (unsigned int)keydata_len);
        return WAPI_FAILED;
    }
    offset += WAI_KEY_LENGTH_SIZE;

    keydata = (unsigned char *)&payload[offset];
    offset += keydata_len;

    if (wai_hmac_sha256(payload, offset, wapi->usksa.usk.mak, WAI_MAK_SIZE, mic, WAI_MIC_SIZE) != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "calc mic failed");
        return WAPI_FAILED;
    }
    if (os_memcmp(mic, &payload[offset], WAI_MIC_SIZE) != 0) {
        wpa_printf(MSG_WARNING, "MIC is error");
        return WAPI_FAILED;
    }

    if (wai_recv_msk_anno_install_msk(wpa, keydata, keydata_len) != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "install msk failure during msk anno");
        return WAPI_FAILED;
    }

    if (wai_send_msk_announce_response(wpa, payload, payload_len)) {
        wpa_printf(MSG_WARNING, "send msk announcement response fail");
        return WAPI_FAILED;
    }
    wai_sm_set_state(wapi, WAISM_FINSHED);
    wpa_supplicant_set_state(wpa, WPA_COMPLETED);
    wpa_supplicant_cancel_auth_timeout(wpa);
    return WAPI_SUCCESS;
}

static int wai_send_msk_announce_response(struct wpa_supplicant *wpa,
    const unsigned char *payload, unsigned int payload_len)
{
    struct wapi_asue_struct *wapi = NULL;
    struct wpabuf *framebuf = NULL;
    unsigned char *framedata = NULL;
    unsigned char mic[WAI_MIC_SIZE] = { 0 };
    size_t framelen;
    int ret;

    framelen = sizeof(struct l2_ethhdr) + sizeof(wai_hdr_stru) + WAI_FLAG_SIZE + WAI_MSKID_SIZE +
        WAI_USKID_SIZE + WAI_ADDID_SIZE + WAI_MSK_ANNO_IV_SIZE + WAI_MIC_SIZE;
    framebuf = wpabuf_alloc(framelen);
    if (framebuf == NULL) {
        wpa_printf(MSG_WARNING, "Fail to zalloc memory for msk announcement response frame");
        return WAPI_FAILED;
    }

    wapi = wpa->wapi;
    if (wai_setup_eth_header(wapi, framebuf) != WAPI_SUCCESS) {
        wpabuf_free(framebuf);
        wpa_printf(MSG_WARNING, "fill ethnet header failed");
        return WAPI_FAILED;
    }

    (void)wpabuf_put(framebuf, sizeof(wai_hdr_stru));
    wpabuf_put_u8(framebuf, wapi->wai_flag);
    wpabuf_put_u8(framebuf, wapi->msksa.mskid);
    wpabuf_put_u8(framebuf, wapi->usksa.uskid);
    wpabuf_put_data(framebuf, wapi->addid, WAI_ADDID_SIZE);
    wpabuf_put_data(framebuf, wapi->msksa.msk_anno_iv, WAI_MSK_ANNO_IV_SIZE);
    framedata = wpabuf_mhead_u8(framebuf) + sizeof(struct l2_ethhdr) + sizeof(wai_hdr_stru);
    ret = wai_hmac_sha256((const unsigned char *)framedata,
        WAI_FLAG_SIZE + WAI_MSKID_SIZE + WAI_USKID_SIZE + WAI_ADDID_SIZE + WAI_MSK_ANNO_IV_SIZE,
        wapi->usksa.usk.mak, WAI_MAK_SIZE,
        mic, WAI_MIC_SIZE);
    if (ret != WAPI_SUCCESS) {
        wpabuf_free(framebuf);
        wpa_printf(MSG_WARNING, "failed to get mic for usknego response frame");
        return WAPI_FAILED;
    }

    wpabuf_put_data(framebuf, mic, WAI_MIC_SIZE);

    wapi->tx_frame_seq++;

    framedata = wpabuf_mhead_u8(framebuf) + sizeof(struct l2_ethhdr);
    framelen = wpabuf_len(framebuf) - sizeof(struct l2_ethhdr);
    wai_setup_wai_header(framedata, (unsigned short)framelen, wapi->tx_frame_seq, WAI_MSK_ANNOUNCEMENT_RESPONSE);

    ret = wai_tx_packet(wpa, wpabuf_mhead_u8(framebuf), wpabuf_len(framebuf));
    if (ret != WAPI_SUCCESS) {
        wpabuf_free(framebuf);
        wpa_printf(MSG_WARNING, "Fail to send msk announcement response frame");
        return WAPI_FAILED;
    }

    wpa_printf(MSG_DEBUG, "send msk announcement response successful");

    wpabuf_free(framebuf);
    return WAPI_SUCCESS;
}

static int wai_bk_to_usk(struct wapi_asue_struct *wapi, const unsigned char *ae_nonce)
{
    const char *usk_text = "pairwise key expansion for unicast and additional keys and nonce";
    unsigned char indata[MAX_KEYDATA_SIZE];
    unsigned char outdata[WAI_USK_DERIVATION_SIZE];
    unsigned int offset = 0;
    unsigned char *data;
    size_t len;

    if (ae_nonce == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    if (memcpy_s(indata, MAX_KEYDATA_SIZE, wapi->addid, WAI_ADDID_SIZE) != EOK) {
        wpa_printf(MSG_WARNING, "memcpy addid failed");
        return WAPI_FAILED;
    }
    offset += WAI_ADDID_SIZE;
    if (memcpy_s(&indata[offset], MAX_KEYDATA_SIZE - offset, ae_nonce, WAI_CHALLENGE_SIZE) != EOK) {
        wpa_printf(MSG_WARNING, "memcpy ae challenge failed");
        return WAPI_FAILED;
    }
    offset += WAI_CHALLENGE_SIZE;
    if (memcpy_s(&indata[offset], MAX_KEYDATA_SIZE - offset, wapi->asue_nonce, WAI_CHALLENGE_SIZE) != EOK) {
        wpa_printf(MSG_WARNING, "memcpy asue challenge failed");
        return WAPI_FAILED;
    }
    offset += WAI_CHALLENGE_SIZE;
    if (memcpy_s(&indata[offset], MAX_KEYDATA_SIZE - offset, usk_text, os_strlen(usk_text)) != EOK) {
        wpa_printf(MSG_WARNING, "memcpy usk_text failed");
        return WAPI_FAILED;
    }
    offset += os_strlen(usk_text);
    if (wai_kd_hmac_sha256((const unsigned char *)indata, offset, wapi->bksa.bk, WAI_BK_SIZE,
        outdata, WAI_USK_DERIVATION_SIZE) != WAPI_SUCCESS) {
        forced_memzero(outdata, WAI_USK_DERIVATION_SIZE);
        wpa_printf(MSG_WARNING, "hmac sha256 failed");
        return WAPI_FAILED;
    }
    wpa_printf(MSG_DEBUG, "wai_kd_hmac_sha256 using uskid %u", (unsigned int)wapi->usksa.uskid);

    offset = 0;
    if (memcpy_s(wapi->usksa.usk.uek_uck, WAI_UEK_UCK_SIZE,
        outdata, WAI_UEK_UCK_SIZE) != EOK) {
        forced_memzero(outdata, WAI_USK_DERIVATION_SIZE);
        wpa_printf(MSG_WARNING, "memcpy uek uck failed");
        return WAPI_FAILED;
    }
    offset += WAI_UEK_UCK_SIZE;

    if (memcpy_s(wapi->usksa.usk.mak, WAI_MAK_SIZE,
        &outdata[offset], WAI_MAK_SIZE) != EOK) {
        forced_memzero(outdata, WAI_USK_DERIVATION_SIZE);
        wpa_printf(MSG_WARNING, "memcpy mak failed");
        return WAPI_FAILED;
    }
    offset += WAI_MAK_SIZE;

    if (memcpy_s(wapi->usksa.usk.kek, WAI_KEK_SIZE,
        &outdata[offset], WAI_KEK_SIZE) != EOK) {
        forced_memzero(outdata, WAI_USK_DERIVATION_SIZE);
        wpa_printf(MSG_WARNING, "memcpy kek failed");
        return WAPI_FAILED;
    }
    offset += WAI_KEK_SIZE;

    data = &outdata[offset];
    len = WAI_CHALLENGE_SIZE;
    if (sha256_vector(1, (const u8 **)&data, &len, wapi->ae_next_nonce) < 0) {
        forced_memzero(outdata, WAI_USK_DERIVATION_SIZE);
        wpa_printf(MSG_WARNING, "sha256 failed");
        return WAPI_FAILED;
    }
    forced_memzero(outdata, WAI_USK_DERIVATION_SIZE);
    return WAPI_SUCCESS;
}

static int wai_nmk_to_msk(const unsigned char *nmk, unsigned int nmk_len,
    unsigned char *msk, unsigned int msk_len)
{
    const char *msk_text = "multicast or station key expansion for station unicast and multicast and broadcast";

    if (nmk == NULL || msk == NULL) {
        wpa_printf(MSG_WARNING, "nmk or msk is null");
        return WAPI_FAILED;
    }
    return wai_kd_hmac_sha256((const unsigned char *)msk_text, os_strlen(msk_text),
        nmk, nmk_len, msk, msk_len);
}

static int wai_install_usk(struct wpa_supplicant *wpa)
{
    /* Everytime ASUE usk update, WPI PN will be initialized to this bignum. */
    const unsigned char wpi_pn[WAI_USK_PN_IV_SIZE] = {
        0x5C, 0x36, 0x5C, 0x36, 0x5C, 0x36, 0x5C, 0x36, 0x5C, 0x36, 0x5C, 0x36, 0x5C, 0x36, 0x5C, 0x36
    };
    struct wapi_asue_struct *wapi = NULL;
    unsigned char uskid;

    if (wpa->wapi->ucast_cipher_suite == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    wapi  = wpa->wapi;
    uskid = wapi->usksa.uskid;
    wpa_printf(MSG_DEBUG, "uskid %u", (unsigned int)wapi->usksa.uskid);

    if (uskid >= WAI_USKSA_CNT) {
        wpa_printf(MSG_ERROR, "invalid uskid %u", (unsigned int)uskid);
        return WAPI_FAILED;
    }

    /* set_tx set 1 when uskidx update, to prevente usk from being set to msk of driver */
    if (wpa_drv_set_key(wpa, wapi->ucast_cipher_suite->wpa_alg_type, wpa->bssid, uskid, 1,
        wpi_pn, WAI_USK_PN_IV_SIZE, wapi->usksa.usk.uek_uck, WAI_UEK_UCK_SIZE, KEY_FLAG_PAIRWISE) < 0) {
        wpa_printf(MSG_WARNING, "Failed to install usk to the driver");
        return WAPI_FAILED;
    }

    /* uek and uck are not needed anymore in wai, mak and kek will be used in msk update. */
    (void)memset_s(wapi->usksa.usk.uek_uck, WAI_UEK_UCK_SIZE, 0, WAI_UEK_UCK_SIZE);

    wpa_printf(MSG_DEBUG, "install uskid %u success", (unsigned int)uskid);
    return WAPI_SUCCESS;
}

static int wai_install_msk(struct wpa_supplicant *wpa, const unsigned char *msk)
{
    struct wapi_asue_struct *wapi = NULL;
    wapi_msksa_stru *msksa = NULL;
    unsigned char mskid;

    if (wpa->wapi->mcast_cipher_suite == NULL || msk == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    wapi  = wpa->wapi;
    msksa = &(wapi->msksa);
    mskid = msksa->mskid;

    if (mskid > 1) {
        wpa_printf(MSG_ERROR, "invalid mskid %u", (unsigned int)mskid);
        return WAPI_FAILED;
    }

    if (wpa_drv_set_key(wpa, wapi->mcast_cipher_suite->wpa_alg_type, broadcast_ether_addr,
        mskid, 0, msksa->msk_seq_pn, WAI_DATA_SERIAL_NUMBER, msk, WAI_MSK_SIZE, KEY_FLAG_GROUP) < 0) {
        wpa_printf(MSG_WARNING, "Failed to install MSK to the driver.");
        return WAPI_FAILED;
    }

    wpa_printf(MSG_DEBUG, "install mskid %u success", (unsigned int)mskid);
    return WAPI_SUCCESS;
}

int wapi_psk_to_bk(struct wpa_supplicant *wpa, const struct wpa_ssid *ssid)
{
    const char *psk_text = "preshared key expansion for authentication and key negotiation";
    const unsigned char *key_src = NULL;
    struct wapi_asue_struct *wapi = NULL;
    unsigned char key_val[WAI_PRE_PSK_SIZE];
    unsigned int key_length = 0;
    int ret;

    wapi = wpa->wapi;

    if (ssid->passphrase == NULL && ssid->psk_set == 0) {
        wpa_printf(MSG_ERROR, "passphrase error");
        return WAPI_FAILED;
    }

    if (memset_s(key_val, WAI_PRE_PSK_SIZE, 0, WAI_PRE_PSK_SIZE) != EOK) {
        wpa_printf(MSG_WARNING, "memset key failed");
        return WAPI_FAILED;
    }

    if (ssid->passphrase != NULL) {
        key_length = os_strlen(ssid->passphrase);
        key_src = ssid->passphrase;
    } else if (ssid->psk_set != 0) {
        key_length = (unsigned int)ssid->hex_key_len;
        key_src = ssid->psk;
    } else {
        wpa_printf(MSG_WARNING, "get key failed");
        return WAPI_FAILED;
    }

    if (memcpy_s(key_val, WAI_PRE_PSK_SIZE, key_src, key_length) != EOK) {
        forced_memzero(key_val, WAI_PRE_PSK_SIZE);
        wpa_printf(MSG_WARNING, "memcpy key failed");
        return WAPI_FAILED;
    }

    if (memset_s(wapi->bk, WAI_BK_SIZE, 0, WAI_BK_SIZE) != EOK) {
        forced_memzero(key_val, WAI_PRE_PSK_SIZE);
        wpa_printf(MSG_WARNING, "memset bk failed");
        return WAPI_FAILED;
    }

    ret = wai_kd_hmac_sha256((const unsigned char *)psk_text, os_strlen((char *)psk_text),
        key_val, key_length, wapi->bk, WAI_BK_SIZE);

    forced_memzero(key_val, WAI_PRE_PSK_SIZE);
    return ret;
}
