/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag connect cmd.
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_connect.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "diag_filter.h"
#include "diag_ind_src.h"
#include "errcode.h"
#ifdef CONFIG_DIAG_WITH_SECURE
#include "diag_secure.h"
#endif
#include "dfx_adapt_layer.h"

#define DIAG_TEMP_MAGIC_1 0xFF

STATIC errcode_t diag_cmd_hso_connect(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                      diag_option_t *option)
{
    uint16_t product_type = 1;
    uint16_t hso_dev_type = 0x0001;
    diag_addr tool_addr;
    errcode_t ret;
    diag_cmd_host_connect_cnf_stru_t cnf = { 0 };

    cnf.imei[0] = 1;                 /* 0:item subscript */
    cnf.imei[1] = DIAG_TEMP_MAGIC_1; /* 1:item subscript */
    cnf.imei[2] = DIAG_TEMP_MAGIC_1; /* 2:item subscript */
    cnf.imei[3] = DIAG_TEMP_MAGIC_1; /* 3:item subscript */
    cnf.imei[4] = DIAG_TEMP_MAGIC_1; /* 4:item subscript */
    cnf.imei[5] = DIAG_TEMP_MAGIC_1; /* 5:item subscript */
    cnf.imei[6] = 0;                 /* 6:item subscript */
    cnf.imei[7] = 0;                 /* 7:item subscript */
    cnf.imei[8] = 1;                 /* 8:item subscript */
    cnf.imei[9] = 0;                 /* 9:item subscript */
    cnf.imei[10] = 1;                /* 10:item subscript */
    cnf.imei[11] = 0;                /* 11:item subscript */

    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(hso_dev_type);
    unused(product_type);
    tool_addr = option->peer_addr;

    zdiag_set_enable(true, tool_addr);
#ifdef CONFIG_DIAG_WITH_SECURE
    diag_secure_ctx_t *secure_ctx = diag_get_secure_ctx();
    dfx_timer_stop(&secure_ctx->diag_secure_timer);
#endif
#ifndef SUPPORT_DIAG_V2_PROTOCOL
    msp_diag_ack_param_t ack;
    ack.sn = 0;
    ack.ctrl = 0;
    ack.cmd_id = DIAG_CMD_HOST_CONNECT;
    ack.param_size = (uint16_t)sizeof(diag_cmd_host_connect_cnf_stru_t);
    ack.param = (uint8_t *)&cnf;
    ret = uapi_diag_report_ack(&ack, option);
#else
    ret = uapi_diag_report_packet_direct(DIAG_CMD_HOST_CONNECT, option, (uint8_t *)&cnf,
                                         (uint16_t)sizeof(diag_cmd_host_connect_cnf_stru_t));
#endif
    return ret;
}


STATIC errcode_t diag_cmd_host_disconn(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                       diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);
    zdiag_set_enable(false, option->peer_addr);
    return ERRCODE_NOT_SUPPORT;
}


errcode_t diag_cmd_hso_connect_disconnect(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                          diag_option_t *option)
{
    switch (cmd_id) {
        case DIAG_CMD_HOST_CONNECT:
            return diag_cmd_hso_connect(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_CMD_HOST_DISCONNECT:
            return diag_cmd_host_disconn(cmd_id, cmd_param, cmd_param_size, option);
        default:
            return ERRCODE_NOT_SUPPORT;
    }
}
