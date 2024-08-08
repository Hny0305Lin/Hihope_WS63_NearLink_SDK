/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag tx
 * This file should be changed only infrequently and with great care.
 */
#include "zdiag_tx.h"
#include "diag_channel_item.h"
#include "zdiag_mem.h"
#include "zdiag_debug.h"
#include "diag_adapt_layer.h"
#include "diag_dfx.h"
#include "errcode.h"

STATIC void zdiag_modify_pkt_before_2_hso(diag_channel_item_t *chan, msp_mux_packet_head_stru_t *mux_head)
{
    if (mux_head->type == MUX_PKT_IND) {
        mux_head->type = MUX_PKT_CMD;
    }

    mux_head->au_id = chan->au_id;
    mux_head->start_flag = MUX_START_FLAG;
    mux_head->ver = MUX_PKT_VER;
}

STATIC errcode_t zdiag_pkt_router_hcc_tx(const diag_channel_item_t *chan, diag_pkt_handle_t *pkt,
    const msp_mux_packet_head_stru_t *mux_head)
{
    unused(mux_head);
    if (chan->tx_hook) {
        return (errcode_t)chan->tx_hook(0, DFX_DATA_DIAG_PKT, pkt->data, pkt->data_len, pkt->data_cnt);
    }

    return ERRCODE_FAIL;
}

STATIC errcode_t zdiag_pkt_router_hso_tx(diag_channel_item_t *chan, diag_pkt_handle_t *pkt,
    msp_mux_packet_head_stru_t *mux_head)
{
    dfx_log_debug("zdiag_pkt_router_hso_tx %d\r\n", pkt->data_cnt);
    zdiag_modify_pkt_before_2_hso(chan, mux_head);
    if (chan->tx_hook) {
        if ((pkt->critical) != 0) {
            return (errcode_t)chan->tx_hook(0, DFX_DATA_DIAG_PKT_CRITICAL, pkt->data, pkt->data_len, pkt->data_cnt);
        } else {
            return (errcode_t)chan->tx_hook(0, DFX_DATA_DIAG_PKT, pkt->data, pkt->data_len, pkt->data_cnt);
        }
    }

    return ERRCODE_FAIL;
}

errcode_t zdiag_pkt_router_tx(diag_pkt_handle_t *pkt)
{
    errcode_t ret;
    msp_mux_packet_head_stru_t *mux_head = diag_pkt_handle_get_mux(pkt);
    diag_addr dst = mux_head->dst;
    diag_channel_item_t *chan = zdiag_dst_2_chan(dst);
    diag_addr_attribute_t attribute = diag_adapt_addr_2_attribute(dst);
    if (((uint32_t)attribute & DIAG_ADDR_ATTRIBUTE_HSO_CONNECT) != 0) {
        ret = zdiag_pkt_router_hso_tx(chan, pkt, mux_head);
    } else {
        ret = zdiag_pkt_router_hcc_tx(chan, pkt, mux_head);
    }

    return ret;
}
