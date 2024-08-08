/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag rx
 * This file should be changed only infrequently and with great care.
 */
#include "zdiag_rx_api.h"
#include "securec.h"
#include "zdiag_mem.h"
#include "diag_pkt.h"
#include "errcode.h"
#include "diag_channel_item.h"
#include "zdiag_pkt_router.h"
#include "diag_adapt_layer.h"
#include "diag_dfx.h"

#define MUX_START_OFFSET 0
#define MAX_MUX_PARAM_SIZE 512

STATIC int32_t zdiag_find_start_flag(uint8_t *data, uint16_t size)
{
    int i;
    int pos = -1;
    uint32_t start_flag = MUX_START_FLAG;
    for (i = 0; i < size; i++) {
        uint8_t *flag = &data[i];
        if (memcmp(flag, &start_flag, sizeof(start_flag)) == 0) {
            pos = i;
            break;
        }
    }
    return pos;
}

STATIC int32_t soc_diag_chan_copy_data(diag_channel_item_t *item, uint8_t *data, uint16_t size)
{
    uint16_t free_size = item->rx_buf_len - item->rx_buf_pos;
    uint16_t copy_size = uapi_min(size, free_size);
    if (memcpy_s(&item->rx_buf[item->rx_buf_pos], free_size, data, copy_size) != EOK) {
        return ERRCODE_FAIL;
    }
    item->rx_buf_pos += copy_size;
    return copy_size;
}

STATIC void diag_rest_rx_buf(diag_channel_item_t *item)
{
    item->rx_buf_pos = MUX_START_OFFSET;
    item->rx_buf_is_using = false;
}

STATIC uint16_t diag_get_rx_data_size(diag_channel_item_t *item)
{
    return item->rx_buf_pos - MUX_START_OFFSET;
}

STATIC msp_mux_packet_head_stru_t *diag_chan_full_pkt_receive(diag_channel_item_t *item)
{
    errcode_t ret;
    msp_mux_packet_head_stru_t *mux = (msp_mux_packet_head_stru_t *)&item->rx_buf[MUX_START_OFFSET];
    if (mux->packet_data_size > MAX_MUX_PARAM_SIZE) {
        diag_rest_rx_buf(item);
        return NULL;
    }

    if (diag_get_rx_data_size(item) < sizeof(msp_mux_packet_head_stru_t) + mux->packet_data_size) {
        return NULL;
    }

    ret = zdiag_check_mux_pkt(mux, sizeof(msp_mux_packet_head_stru_t) + mux->packet_data_size);
    if (ret != ERRCODE_SUCC) {
        diag_rest_rx_buf(item);
        return NULL;
    }
    return mux;
}

STATIC errcode_t diag_chan_move_buf(diag_channel_item_t *item, uint32_t dst, uint32_t src, uint32_t len)
{
    if (memmove_s(&item->rx_buf[dst], item->rx_buf_len, &item->rx_buf[src], len) != EOK) {
        return ERRCODE_FAIL;
    }
    item->rx_buf_pos -= (uint16_t)(src - dst);
    return ERRCODE_SUCC;
}

STATIC int32_t diag_chan_rx_mux_char_data(diag_channel_item_t *item, uint8_t *data, uint16_t size)
{
    int32_t copied_size = 0;
    int32_t start_flag_pos;
    int32_t frame_count = 0;
    diag_pkt_process_param_t process_param;
    diag_pkt_handle_t pkt;

    while (copied_size != size && item->rx_buf_pos != item->rx_buf_len) { /* 数据未处理完成 并且 接收缓存有空间 */
        copied_size += soc_diag_chan_copy_data(item, data + copied_size, (uint16_t)(size - copied_size));
        if (item->rx_buf_is_using) {
            return frame_count;
        }

        if (diag_get_rx_data_size(item) < sizeof(msp_mux_packet_head_stru_t)) {
            continue;
        }

        start_flag_pos = zdiag_find_start_flag(&item->rx_buf[MUX_START_OFFSET], item->rx_buf_pos - MUX_START_OFFSET);
        if (start_flag_pos < 0) {
            diag_rest_rx_buf(item);
            continue;
        } else if (start_flag_pos > 0) {
            diag_chan_move_buf(item, MUX_START_OFFSET, (uint32_t)start_flag_pos,
                               (uint32_t)(item->rx_buf_pos - start_flag_pos));
        }

        if (diag_get_rx_data_size(item) < sizeof(msp_mux_packet_head_stru_t)) {
            continue;
        }

        do {
            msp_mux_packet_head_stru_t *mux = diag_chan_full_pkt_receive(item);
            if (mux == NULL) {
                break;
            }

            item->au_id = mux->au_id;
            mux->src = item->hso_addr;
            mux->ctrl = 0;
            if (mux->dst == 0) {
                mux->dst = diag_adapt_get_local_addr();
            }

            diag_pkt_handle_init(&pkt, 1);
            diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_0, (uint8_t*)mux,
                                     sizeof(msp_mux_packet_head_stru_t) + mux->packet_data_size, DIAG_PKT_DATA_STACK);

            process_param.cur_proc = DIAG_PKT_PROC_PORT_PKT;
            diag_pkt_router(&pkt, &process_param);
            frame_count++;
            diag_chan_move_buf(item, MUX_START_OFFSET, sizeof(msp_mux_packet_head_stru_t) + mux->packet_data_size,
                               item->rx_buf_pos - (sizeof(msp_mux_packet_head_stru_t) + mux->packet_data_size));
        } while (true);
    }
    return frame_count;
}

/* 字符串数据接收函数 */
int32_t soc_diag_channel_rx_mux_char_data(diag_channel_id_t id, uint8_t *data, uint16_t size)
{
    int32_t frame_cnt;
    diag_channel_item_t *item = diag_chan_idx_2_item(id);

    if (item == NULL) {
        return ERRCODE_FAIL;
    }

    diag_dfx_channel_receive_data(id, size);
    frame_cnt = diag_chan_rx_mux_char_data(item, data, size);
    diag_dfx_channel_receive_frame(id, (uint32_t)frame_cnt);
    return frame_cnt;
}

errcode_t soc_diag_channel_rx_mux_data(diag_channel_id_t id, uint8_t *data, uint16_t size)
{
    errcode_t ret;
    diag_channel_item_t *item = diag_chan_idx_2_item(id);
    msp_mux_packet_head_stru_t *mux = (msp_mux_packet_head_stru_t *)data;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param;

    if (item == NULL) {
        return ERRCODE_FAIL;
    }

    diag_dfx_channel_receive_data(id, size);

    ret = zdiag_check_mux_pkt(mux, size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    diag_pkt_handle_init(&pkt, 1);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_0, (uint8_t*)mux,
                             sizeof(msp_mux_packet_head_stru_t) + mux->packet_data_size, DIAG_PKT_DATA_STACK);
    process_param.cur_proc = DIAG_PKT_PROC_PORT_PKT;

    return diag_pkt_router(&pkt, &process_param);
}
