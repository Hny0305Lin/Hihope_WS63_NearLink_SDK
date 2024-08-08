/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: dfx channel
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_adapt_layer.h"
#include "diag.h"
#include "diag_ind_src.h"
#include "diag_channel.h"
#include "diag_adapt_layer.h"
#include "log_uart.h"
#include "dfx_channel.h"

diag_channel_id_t g_diag_channel_id = DIAG_CHANNEL_ID_INVALID;

void diag_uart_rx_proc(uint8_t *buffer, uint16_t length)
{
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    soc_diag_channel_rx_mux_char_data(g_diag_channel_id, buffer, length);
#else
    uapi_diag_channel_rx_mux_char_data(DIAG_CHANNEL_ID_0, buffer, length);
#endif
}

static int32_t diag_channel_uart_output(void *fd, dfx_data_type_t data_type, uint8_t *data[],
    uint16_t len[], uint8_t cnt)
{
    unused(fd);
    unused(data_type);
    for (uint8_t i = 0; i < cnt; i++) {
        log_uart_send_buffer(data[i], len[i]);
    }
    return ERRCODE_SUCC;
}

errcode_t diag_register_channel(void)
{
    g_diag_channel_id = DIAG_CHANNEL_ID_0;
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    if (soc_diag_channel_init(g_diag_channel_id, SOC_DIAG_CHANNEL_ATTR_NEED_RX_BUF) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (soc_diag_channel_set_connect_hso_addr(g_diag_channel_id, diag_adapt_get_default_dst()) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (soc_diag_channel_set_tx_hook(g_diag_channel_id, diag_channel_uart_output) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#else /* SUPPORT_DIAG_V2_PROTOCOL */

    if (uapi_diag_channel_init(g_diag_channel_id, DIAG_CHANNEL_ATTR_NEED_RX_BUF) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (uapi_diag_channel_set_connect_hso_addr(g_diag_channel_id, (uint8_t)diag_adapt_get_default_dst()) !=
        ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (uapi_diag_channel_set_tx_hook(g_diag_channel_id, diag_channel_uart_output) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

#endif /* SUPPORT_DIAG_V2_PROTOCOL */
    return ERRCODE_SUCC;
}