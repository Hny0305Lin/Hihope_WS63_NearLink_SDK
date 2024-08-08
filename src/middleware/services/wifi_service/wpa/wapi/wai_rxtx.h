/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: WAPI Implementation
 */

#ifndef WAI_RXTX_H
#define WAI_RXTX_H

typedef struct _wai_hdr_stru {
    unsigned char version[2];
    unsigned char type;
    unsigned char subtype;
    unsigned char reserve[2];
    unsigned char length[2];
    unsigned char frameseq[2];
    unsigned char fragseq;
    unsigned char flag;
    /* Followed by Data */
} STRUCT_PACKED wai_hdr_stru;

void wai_rx_packet(void *ctx, const unsigned char *src_mac,
    const unsigned char *buf, unsigned int datalen);
int wai_tx_packet(struct wpa_supplicant *wpa,
    const unsigned char *buf, unsigned int datalen);

#endif /* end of wai_rxtx.h */
