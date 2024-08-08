/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag get task info
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_get_task_info.h"
#include "dfx_task.h"
#include "dfx_adapt_layer.h"
#include "errcode.h"

errcode_t diag_cmd_get_task_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    uint32_t task_cnt;
    errcode_t ret;
    task_info_t *infs = NULL;

    unused(cmd_param);
    unused(cmd_param_size);

    task_cnt = dfx_os_get_task_cnt();
    if (task_cnt == 0) {
        return ERRCODE_FAIL;
    }

    infs = dfx_malloc(0, task_cnt * sizeof(task_info_t));
    if (infs == NULL) {
        return ERRCODE_FAIL;
    }

    ret = dfx_os_get_all_task_info(infs, task_cnt);
    if (ret != ERRCODE_SUCC) {
        dfx_free(0, infs);
        return ret;
    }

    for (unsigned i = 0; i < task_cnt; i++) {
        task_info_t *inf = &infs[i];
        if (inf->valid) {
            uapi_diag_report_packet(cmd_id, option, (uint8_t *)inf, (uint16_t)sizeof(task_info_t), true);
        }
    }
    dfx_free(0, infs);
    return ERRCODE_SUCC;
}