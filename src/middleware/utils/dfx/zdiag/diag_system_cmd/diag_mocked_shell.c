/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: mocked shell
 */

#include "diag_mocked_shell.h"
#include "soc_diag_cmd_id.h"
#include "diag_filter.h"
#include "diag_common.h"
#include "diag_channel_item.h"
#include "diag_ind_src.h"
#include "diag_adapt_layer.h"
#include "securec.h"
#include "dfx_adapt_layer.h"
#include "uapi_crc.h"

#define SIZEBUF                 256
#define BITS_PER_BYTE           8
#define MUTEX_STS_BIT           0x1
#define ASCII_MAX_VALID_VALUE   0x7F
#define DIAG_CMD_SHELL_STR_MAX_LEN 256

static bool g_zdiag_mocked_shell_enable = false;
static uint32_t g_zdiag_msg_flag = MUX_START_FLAG;
static zdiag_data_put_uart  g_zdiag_data_to_uart  = NULL;
static diag_cmd_shell_data_proc  g_cmd_shell_data_proc  = NULL;

static diag_cmd_reg_obj_t g_zdiag_mocked_shell_cmd_tbl[] = {
    {DIAG_CMD_MOCKED_SHELL, DIAG_CMD_MOCKED_SHELL, zdiag_cmd_mocked_shell},
    {DIAG_CMD_MOCKED_SHELL_ENABLE, DIAG_CMD_MOCKED_SHELL_ENABLE, zdiag_cmd_mocked_shell_enable},
};

errcode_t zdiag_cmd_mocked_shell(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                 diag_option_t *option)
{
    unused(cmd_id);
    unused(option);
    unused(cmd_param_size);

    uint32_t str_len = *(uint32_t *)cmd_param;
    if ((str_len == 0) || (str_len >= DIAG_CMD_SHELL_STR_MAX_LEN)) {
        return ERRCODE_FAIL;
    }

    uint8_t *data = (uint8_t *)dfx_malloc(0, str_len + 1);
    if (data == NULL) {
        return ERRCODE_MALLOC;
    }

    if (memcpy_s(data, str_len, (void *)((uint32_t)(uintptr_t)cmd_param + sizeof(uint32_t)), str_len) != EOK) {
        dfx_free(0, data);
        return ERRCODE_MEMCPY;
    }
    data[str_len] = 0;
    if (g_cmd_shell_data_proc != NULL) {
        g_cmd_shell_data_proc(data, str_len);
    }
    dfx_free(0, data);
    return ERRCODE_SUCC;
}

errcode_t zdiag_cmd_mocked_shell_enable(uint16_t cmd_id, void *cmd_param,
                                        uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_id);
    unused(option);
    unused(cmd_param_size);

    bool shell_enabled = false;
    uint32_t enable = *(uint32_t *)cmd_param;

    if (enable == (uint32_t)-1) {
        shell_enabled = zdiag_mocked_shell_is_enabled();
    } else if (enable == 0 || enable == 1) {
        shell_enabled = (enable == 0) ? false : true;
        zdiag_mocked_shell_set_enable(shell_enabled);
    } else {
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)("Invaild Parameter\r\n"),
                                (uint16_t)strlen("Invaild Parameter\r\n") + 1, true);
        return ERRCODE_SUCC;
    }

    if (shell_enabled) {
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)("Mocked Shell Enabled\r\n"),
                                (uint16_t)strlen("Mocked Shell Enabled\r\n") + 1, true);
    } else {
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)("Mocked Shell Disabled\r\n"),
                                (uint16_t)strlen("Mocked Shell Disabled\r\n") + 1, true);
    }
    return ERRCODE_SUCC;
}

void zdiag_mocked_shell_register_cmd_data_proc(diag_cmd_shell_data_proc data_proc_hook)
{
    g_cmd_shell_data_proc = data_proc_hook;
}

#ifdef CONFIG_SUPPORT_UART_SHELL
static void zdiag_mocked_shell_process_uart_data(uint8_t *data, uint16_t size)
{
    if (data == NULL || size == 0) {
        return;
    }

    if (zdiag_is_enable() != true) {
        for (uint16_t i = 0; i < size; i++) {
            g_zdiag_msg_flag =
                ((uint8_t)g_zdiag_msg_flag == data[i]) ? (g_zdiag_msg_flag >> BITS_PER_BYTE) : MUX_START_FLAG;
            if (g_zdiag_msg_flag == 0) {
                g_zdiag_mocked_shell_enable = false;
                return;
            }
        }

        if (g_cmd_shell_data_proc != NULL) {
            g_cmd_shell_data_proc(data, size);
        }
    }
}
#endif

static void zdiag_mocked_shell_update_status(bool enable)
{
    if (enable == false) {
        g_zdiag_msg_flag = MUX_START_FLAG;
    }
}

int zdiag_mocked_shell_uart_puts(uint8_t *data, uint32_t data_len)
{
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    uint8_t header_len = (uint8_t)(sizeof(msp_mux_packet_head_stru_t) + sizeof(msp_diag_head_ind_stru_t));
    uint8_t *print_buf = (uint8_t *)dfx_malloc(0, header_len + data_len);
    if (print_buf == NULL) {
        return 0;
    }

    (void)memset_s(print_buf, header_len + data_len, 0, header_len + data_len);
    if (memcpy_s(print_buf + header_len, data_len, data, data_len) != EOK) {
        dfx_free(0, print_buf);
        return 0;
    }

    msp_mux_packet_head_stru_t *header = (msp_mux_packet_head_stru_t *)print_buf;
    header->start_flag = MUX_START_FLAG;
    header->au_id = 0;
    header->ver = MUX_PKT_VER;
    header->cmd_id = DIAG_CMD_MOCKED_SHELL_IND;
    header->dst = zdiag_get_connect_tool_addr();
    header->src = diag_adapt_get_local_addr();
    header->packet_data_size = (uint16_t)sizeof(msp_diag_head_ind_stru_t) + (uint16_t)data_len;

    msp_diag_head_ind_stru_t *ind_head = (msp_diag_head_ind_stru_t *)(print_buf + sizeof(msp_mux_packet_head_stru_t));
    ind_head->cmd_id = DIAG_CMD_MOCKED_SHELL_IND;
    ind_head->param_size = (uint16_t)data_len;
    header->crc16 = uapi_crc16(0, header->puc_packet_data, header->packet_data_size);

    uint8_t *out_data[2];
    uint16_t out_len[2];
    out_data[0] = print_buf;
    out_len[0] = header_len + (uint16_t)data_len;
    diag_channel_item_t *chan = zdiag_dst_2_chan(header->dst);

    uint32_t lock = dfx_int_lock();
    chan->tx_hook(0, DFX_DATA_DIAG_PKT, out_data, out_len, 1);
    dfx_int_restore(lock);

    dfx_free(0, print_buf);
    return (int)data_len;
#else
    diag_option_t option;
    option.peer_addr = DIAG_FRAME_FID_UART;
    if (uapi_diag_report_packet_direct(DIAG_CMD_MOCKED_SHELL_IND, &option, data, (uint16_t)data_len) == ERRCODE_SUCC) {
        return (int)data_len;
    } else {
        return 0;
    }
#endif
}

int zdiag_mocked_shell_print(const char *fmt, va_list ap)
{
    uint8_t *out_buf = (uint8_t *)dfx_malloc(0, SIZEBUF);
    if (out_buf == NULL) {
        return -1;
    }
    (void)memset_s(out_buf, SIZEBUF, 0, SIZEBUF);
    int32_t len = vsnprintf_s((char *)out_buf, SIZEBUF, SIZEBUF - 1, fmt, ap);
    if (len == -1) {
        dfx_free(0, out_buf);
        return -1;
    }

    if (strcmp((char *)(out_buf), "LiteOS$ ") == 0 ||
        strcmp((char *)(out_buf), "FreeRTOS$ ") == 0) {
        strcpy_s((char *)(out_buf), SIZEBUF, "End shell command.\r\n");
        len = (int32_t)strlen("End shell command.\r\n");
    }

    zdiag_mocked_shell_uart_puts(out_buf, (uint32_t)len);
    dfx_free(0, out_buf);
    return len;
}

void zdiag_mocked_shell_register_directly_uartput(zdiag_data_put_uart uart_hook)
{
    g_zdiag_data_to_uart = uart_hook;
}

void zdiag_mocked_shell_init(void)
{
    zdiag_filter_register_notify_hook(zdiag_mocked_shell_update_status);
#ifdef CONFIG_SUPPORT_UART_SHELL
    zdiag_uart_register_bypass_hook(zdiag_mocked_shell_process_uart_data);
#endif
    uapi_diag_register_cmd(g_zdiag_mocked_shell_cmd_tbl,
        sizeof(g_zdiag_mocked_shell_cmd_tbl) / sizeof(g_zdiag_mocked_shell_cmd_tbl[0]));
}

void zdiag_mocked_shell_set_enable(bool enable)
{
    g_zdiag_mocked_shell_enable = enable;
}

bool zdiag_mocked_shell_is_enabled(void)
{
    return g_zdiag_mocked_shell_enable;
}