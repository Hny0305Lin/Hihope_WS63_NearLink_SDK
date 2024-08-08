/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: software at by zdiag
 */

#ifdef CONFIG_AT_SUPPORT_ZDIAG

#include "nv.h"
#include "uapi_crc.h"
#include "soc_diag_cmd_id.h"
#include "dfx_adapt_layer.h"
#include "at_product.h"
#include "debug_print.h"
#include "at_zdiag.h"

#define KVALUE_MAX_LENGTH 4060

typedef struct {
    uint32_t data_size;
    uint8_t data[0];
} zdiag_at_input_t;

typedef struct {
    uint8_t data[0];
} zdiag_at_output_t;

static void at_zdiag_write_func(const char *data)
{
    int data_size = strlen(data);
    if (data == NULL) {
        return;
    }
    zdiag_at_output_t *at_output = (zdiag_at_output_t *)dfx_malloc(0, data_size);
    if (at_output == NULL) {
        return;
    }
    memcpy_s(at_output->data, data_size, data, data_size);
    uapi_diag_report_packet(DIAG_CMD_SIMULATE_AT_IND, NULL, (uint8_t *)at_output, data_size, true);
    dfx_free(0, at_output);
}

void zdiag_at_init(void)
{
    uapi_at_channel_write_register(AT_ZDIAG_PORT, at_zdiag_write_func);
}

errcode_t zdiag_at_proc(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param_size);
    unused(cmd_id);
    unused(option);
    static uint8_t g_at_pre_char = 0;
    errcode_t ret;
    zdiag_at_input_t *at_input = (zdiag_at_input_t *)cmd_param;

    print_str("xxx zdiag_at_proc at_input.str: '%s', size = %u\r\n", at_input->data, at_input->data_size);
    ret = uapi_at_channel_data_recv(AT_ZDIAG_PORT, (uint8_t *)at_input->data, (uint32_t)at_input->data_size);
    if (ret != ERRCODE_SUCC) {
        /* 前一个字符为'\r'时单独一个'\n'导致的CHANNEL_BUSY不打印 */
        if (g_at_pre_char != '\r' || at_input->data_size != 1 || at_input->data[0] != '\n' ||\
                ret != ERRCODE_AT_CHANNEL_BUSY) {
            print_str("\r\nat_uart_rx_callback fail:0x%x\r\n", ret);
            return ret;
        }
    }
    g_at_pre_char = at_input->data[at_input->data_size - 1];
    return ERRCODE_SUCC;
}

#endif