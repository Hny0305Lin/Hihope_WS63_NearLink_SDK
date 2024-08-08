/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag channel
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_CHANNEL_ITEM_H
#define DIAG_CHANNEL_ITEM_H

#include "errcode.h"
#include "diag_channel.h"
#include "dfx_write_interface.h"

typedef struct {
    uint32_t init : 1;
    uint32_t pad : 31;
    uint32_t name_flag;
    uint8_t *rx_buf;
    uint32_t au_id;
    uint16_t rx_buf_len;
    uint16_t rx_buf_pos;
    uint16_t using_size;
    uint16_t min_pkt_size;
    bool rx_buf_is_using;
    uint8_t hso_addr;
    uint8_t pad1[2];
    diag_channel_tx_hook tx_hook;
    diag_channel_notify_hook notify_hook;
} diag_channel_item_t;

diag_channel_item_t *diag_chan_idx_2_item(diag_channel_id_t id);
diag_channel_item_t *zdiag_dst_2_chan(uint8_t addr);

#endif
