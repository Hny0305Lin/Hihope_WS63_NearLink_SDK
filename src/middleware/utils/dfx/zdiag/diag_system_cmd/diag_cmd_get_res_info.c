/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag get os res info
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_get_res_info.h"
#include "dfx_adapt_layer.h"
#include "dfx_os_st.h"
#include "dfx_res.h"

errcode_t diag_cmd_get_res_info(uint16_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size,
                                diag_option_t *option)
{
    osal_os_resource_use_stat_t os_resource = { 0 };
    unused(cmd_param);
    unused(cmd_param_size);

    dfx_os_get_resource_status(&os_resource);
    return uapi_diag_report_packet(cmd_id, option, (const uint8_t *)&os_resource,
        sizeof(osal_os_resource_use_stat_t), true);
}