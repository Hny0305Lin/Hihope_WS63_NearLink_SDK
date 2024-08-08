/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag get memory info
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_get_mem_info.h"
#include "dfx_mem.h"
#include "dfx_adapt_layer.h"
#include "errcode.h"

errcode_t diag_cmd_get_mem_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    errcode_t ret;
    mdm_mem_info_t info;

    unused(cmd_param);
    unused(cmd_param_size);

    ret = dfx_mem_get_sys_pool_info(&info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&info, (uint16_t)sizeof(mdm_mem_info_t), true);
    return ERRCODE_SUCC;
}