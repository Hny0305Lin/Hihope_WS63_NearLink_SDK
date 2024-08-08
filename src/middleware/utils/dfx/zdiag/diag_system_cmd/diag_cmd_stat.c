/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: diag stat query cmd.
 */

#include "diag_cmd_stat.h"
#include "securec.h"
#include "diag_adapt_layer.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "diag_stat.h"

STATIC errcode_t diag_execute_query_stat(void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    errcode_t ret = ERRCODE_FAIL;

    diag_dbg_stat_query_t *stat_q = (diag_dbg_stat_query_t *)cmd_param;
    uint32_t obj = 0;
    uint16_t obj_len = 0;
    uint16_t obj_cnt = 0;

    if ((cmd_param_size == 0) || (stat_q == NULL)) {
        return ERRCODE_FAIL;
    }

    if ((stat_q->id < DFX_STAT_ID_BASE_SYS) || (stat_q->id > DFX_STAT_ID_MAX_SYS)) {
        return ERRCODE_DIAG_INVALID_PARAMETER;
    } else {
        ret = zdiag_query_stat_obj(stat_q->id, (uint32_t *)&obj, (uint16_t *)&obj_len, (uint16_t *)&obj_cnt);
    }

    if (ret == ERRCODE_SUCC) {
        zdiag_report_stat_obj_stru_t obj_pkt;
        memset_s(&obj_pkt, sizeof(zdiag_report_stat_obj_stru_t), 0, sizeof(zdiag_report_stat_obj_stru_t));
        obj_pkt.obj_id = (uint16_t)stat_q->id;
        obj_pkt.object = (uint8_t *)(uintptr_t)obj;
        obj_pkt.obj_size = obj_len;
        obj_pkt.obj_cnt = obj_cnt;
        obj_pkt.option = option;
        obj_pkt.sync = true;

        ret = zdiag_report_stat_obj(obj_pkt);
    }
    return ret;
}

errcode_t diag_cmd_stat_query(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    switch (cmd_id) {
        case DIAG_CMD_ID_DBG_STAT_QUERY:
            return diag_execute_query_stat(cmd_param, cmd_param_size, option);
        default:
            return ERRCODE_NOT_SUPPORT;
    }
}
