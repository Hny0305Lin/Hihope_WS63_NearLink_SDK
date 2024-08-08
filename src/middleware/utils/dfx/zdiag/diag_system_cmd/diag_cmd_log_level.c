/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: diag set log lvl
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_log_level.h"
#include "dfx_adapt_layer.h"
#include "log_types.h"
#include "platform_types.h"
#include "debug_print.h"

errcode_t diag_cmd_set_log_level(uint16_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size,
    diag_option_t *option)
{
    unused(cmd_param_size);

    diag_log_lvl_data_t *diag_data = (diag_log_lvl_data_t *)cmd_param;

    uint8_t log_core = (cores_t)(diag_data->core);
    uint32_t log_level = diag_data->log_lvl;
    if (log_core > CORES_MAX_NUMBER_PHYSICAL || log_level > LOG_LEVEL_MAX) {
        dfx_log_err("para invalid!\r\n");
        return ERRCODE_FAIL;
    }

    uint8_t origin_lvl = 0;
    dfx_log_get_level(log_core, &origin_lvl);
    dfx_log_set_level(log_core, (uint8_t)log_level);
    dfx_log_info("set core:%d log level from %d to %d OK!\r\n", log_core, origin_lvl, log_level);

    return uapi_diag_report_packet(cmd_id, option, (const uint8_t *)&log_level, sizeof(uint32_t), true);
}