/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: WAPI Implementation
 */

#include "wapi.h"
#include "wai_rxtx.h"
#include "wai_sm.h"
#include "utils/wpabuf.h"
#include "l2_packet/l2_packet.h"
#include "config_ssid.h"
#include "common/defs.h"

static void wpabuf_free_keep_extdata(struct wpabuf *buf)
{
    if (buf == NULL)
        return;

    os_free(buf);
}

static int wai_validate_ae_info(const struct wpa_supplicant *wpa, const unsigned char *src_mac)
{
    if (os_memcmp(src_mac, wpa->bssid, ETH_ALEN) != 0)
        return WAPI_FAILED;

    return WAPI_SUCCESS;
}

static int wai_validate_authentication_type(const struct wpa_supplicant *wpa)
{
    const struct wpa_ssid *ssid = wpa->current_ssid;

    if (((unsigned int)wpa->key_mgmt != WPA_KEY_MGMT_WAPI_PSK) ||
        (ssid == NULL) || (((unsigned int)ssid->key_mgmt & WPA_KEY_MGMT_WAPI_PSK) == 0))
        return WAPI_FAILED;

    return WAPI_SUCCESS;
}

static int wai_validate_subtype(const wai_hdr_stru *wai_hdr)
{
    if ((wai_hdr->subtype < (unsigned int)WAI_USK_NEGOTIATION_REQUEST) ||
        (wai_hdr->subtype >= (unsigned int)WAI_SUBTYPE_MAX)) {
        return WAPI_FAILED;
    }
    return WAPI_SUCCESS;
}

static int wai_check_valid_frame(const struct wapi_asue_struct *wapi,
    const unsigned char *src_mac, const unsigned char *buf, unsigned int datalen)
{
    wai_hdr_stru *wai_hdr = NULL;
    unsigned short version;
    unsigned short length;
    unsigned short frameseq;

    if (src_mac == NULL || buf == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    wai_hdr = (wai_hdr_stru *)buf;

    wpa_printf(MSG_DEBUG, "Frame(Ver:%u,Type:%u,Subtype:%u,Len:%u,FrameSeq:%u,FragSeq:%u,Flag:%u)",
        (unsigned int)WPA_GET_BE16(wai_hdr->version), (unsigned int)wai_hdr->type, (unsigned int)wai_hdr->subtype,
        (unsigned int)WPA_GET_BE16(wai_hdr->length), (unsigned int)WPA_GET_BE16(wai_hdr->frameseq),
        (unsigned int)wai_hdr->fragseq, (unsigned int)wai_hdr->flag);

    version = WPA_GET_BE16(wai_hdr->version);
    if (version != WAI_VERSION) {
        wpa_printf(MSG_WARNING, "WAI frame version(%u) error", (unsigned int)version);
        return WAPI_FAILED;
    }

    if (wai_hdr->type != WAI_TYPE) {
        wpa_printf(MSG_WARNING, "WAI frame type(%u) error", (unsigned int)wai_hdr->type);
        return WAPI_FAILED;
    }

    if (wai_validate_subtype(wai_hdr)) {
        wpa_printf(MSG_WARNING, "WAI frame subtype error");
        return WAPI_FAILED;
    }

    length = WPA_GET_BE16(wai_hdr->length);
    if (length != datalen) {
        wpa_printf(MSG_WARNING, "WAI frame length(%u),should be %u", (unsigned int)length, datalen);
        return WAPI_FAILED;
    }

    /* Check the frame sequence number. Filters received duplicate frames.
     * If the frame is a duplicate frame before the required sequence number, discard the frame. */
    frameseq = WPA_GET_BE16(wai_hdr->frameseq);
    /* For AE retransmission, change the comparison value from 0 to -1. */
    if ((short)(frameseq - wapi->next_frame_seq) < -1) {
        wpa_printf(MSG_DEBUG, "WAI frame sequence(%u) is mistake, should be %u",
            (unsigned int)frameseq, (unsigned int)wapi->next_frame_seq);
        return WAPI_FAILED;
    }

    /* Do not support fragment */
    if (wai_hdr->flag != 0x00) {
        wpa_printf(MSG_WARNING, "WAI frame moreflag is mistake");
        return WAPI_FAILED;
    }

    return WAPI_SUCCESS;
}

/**
 *  @brief Sending WAI frames
 */
int wai_tx_packet(struct wpa_supplicant *wpa, const unsigned char *framebuf, unsigned int datalen)
{
    int ret = 0;

    if (wpa == NULL || wpa->wapi == NULL || framebuf == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return WAPI_FAILED;
    }

    ret = l2_packet_send(wpa->wapi->wapi_l2,
        wpa->bssid, ETH_TYPE_WAI, framebuf, datalen);

    return (ret >= 0) ? WAPI_SUCCESS : WAPI_FAILED;
}

/**
 *  @brief Receives WAI frames, which are called back when L2 receives WAI data.
 */
void wai_rx_packet(void *ctx, const unsigned char *src_mac,
    const unsigned char *buf, size_t datalen)
{
    struct wpa_supplicant *wpa = (struct wpa_supplicant *)ctx;
    struct wapi_asue_struct *wapi = NULL;
    struct wpabuf *waibuf = NULL;
    wai_hdr_stru *wai_hdr = NULL;

    if (datalen < (sizeof(struct l2_ethhdr) + sizeof(wai_hdr_stru))) {
        wpa_printf(MSG_WARNING, "datalen %u invalid", datalen);
        return;
    }

    if (wpa == NULL || wpa->wapi == NULL || src_mac == NULL || buf == NULL) {
        wpa_printf(MSG_ERROR, "input param is null");
        return;
    }

    buf += sizeof(struct l2_ethhdr);
    datalen -= sizeof(struct l2_ethhdr);

    /* filter the frames which incorrect or not from BSS. */
    if (wai_validate_ae_info(wpa, src_mac) != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "mac address of WAI frame is wrong");
        return;
    }

    /* Only PSK authentication is supported. */
    if (wai_validate_authentication_type(wpa) != WAPI_SUCCESS) {
        wpa_printf(MSG_WARNING, "WAI frame authentication error");
        return;
    }

    wapi = wpa->wapi;

    if (wapi->state < WAISM_ALREADY_ASSOC) {
        wpa_printf(MSG_WARNING, "WAI frame is received in wrong state %u", (unsigned int)wapi->state);
        return;
    }

    if (wai_check_valid_frame(wapi, src_mac, buf, (unsigned int)datalen) == WAPI_FAILED) {
        wpa_printf(MSG_WARNING, "wai_check_valid_frame failed");
        return;
    }

    waibuf = wpabuf_alloc_ext_data((unsigned char *)buf, datalen);
    if (waibuf == NULL) {
        wpa_printf(MSG_WARNING, "Failed to get wpabuf");
        return;
    }

    /* The required frame has been received. Update next_frame_seq. */
    wai_hdr = (wai_hdr_stru *)wpabuf_head(waibuf);
    wapi->next_frame_seq = WPA_GET_BE16(wai_hdr->frameseq) + 1;

    /* Process of entering the state machine */
    wai_sm_process(wpa, waibuf);

    wpabuf_free_keep_extdata(waibuf);
}
