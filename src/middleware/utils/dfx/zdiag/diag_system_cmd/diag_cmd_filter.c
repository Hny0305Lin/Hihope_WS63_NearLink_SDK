/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag filter
 * This file should be changed only infrequently and with great care.
 */
#include "diag_cmd_filter.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "diag_adapt_layer.h"
#include "diag_filter.h"
#include "diag_cmd_filter_st.h"
#include "errcode.h"

STATIC errcode_t diag_cmd_filter_set_id(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                        diag_option_t *option)
{
    uint32_t i;
    diag_cmd_msg_cfg_req_stru_t *cfg = cmd_param;
    uint32_t cnt = cmd_param_size / (uint32_t)sizeof(diag_cmd_msg_cfg_req_stru_t);
    for (i = 0; i < cnt; i++) {
        zdiag_set_id_enable(cfg->module_id, cfg->switch_code);
        cfg++;
    }

    unused(cmd_id);
    unused(option);
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_cmd_filter_set_level(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
    diag_option_t *option)
{
    uint32_t i;
    uint8_t highest_level = 0;
    diag_cmd_msg_cfg_req_stru_t *cfg = cmd_param;
    uint32_t cnt = cmd_param_size / (uint32_t)sizeof(diag_cmd_msg_cfg_req_stru_t);
    for (i = 0; i < cnt; i++) {
        zdiag_set_level_enable((uint8_t)(cfg->module_id), cfg->switch_code);
        if (cfg->switch_code == 1 && cfg->module_id > highest_level) {
            highest_level = (uint8_t)(cfg->module_id);
        }
        cfg++;
    }
    diag_highest_level_proc(highest_level);

    unused(cmd_id);
    unused(option);
    return ERRCODE_SUCC;
}

errcode_t diag_cmd_filter_set(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    dfx_assert(cmd_param);
    switch (cmd_id) {
        case DIAG_CMD_MSG_CFG_SET_SYS:
        case DIAG_CMD_MSG_CFG_SET_DEV:
        case DIAG_CMD_MSG_CFG_SET_USR:
        case DIAG_CMD_MSG_CFG_SET_AIR:
            return diag_cmd_filter_set_id(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_CMD_MSG_CFG_SET_LEVEL:
            return diag_cmd_filter_set_level(cmd_id, cmd_param, cmd_param_size, option);
        default:
            return ERRCODE_NOT_SUPPORT;
    }
}
