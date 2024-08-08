/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 */

#include "diag_stat.h"
#include "diag_common.h"
#include "diag_adapt_layer.h"
#include "diag.h"
#include "errcode.h"

zdiag_stat_ctrl_t g_diag_stat_ctrl;

STATIC zdiag_stat_ctrl_t *diag_get_stat_ctrl(void)
{
    return &g_diag_stat_ctrl;
}

errcode_t uapi_diag_register_stat_obj(const diag_sys_stat_obj_t *stat_obj_tbl, uint16_t obj_num)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t lock_stat;
    zdiag_stat_ctrl_t *stat_ctrl;
    uint16_t i;

    stat_ctrl = diag_get_stat_ctrl();
    lock_stat = dfx_int_lock();

    for (i = 0; i < CONFIG_STAT_CMD_LIST_NUM; i++) {
        if ((stat_ctrl->stat_cmd_list[i] == NULL) || (stat_ctrl->aus_stat_cmd_num[i] == 0)) {
            stat_ctrl->stat_cmd_list[i] = stat_obj_tbl;
            stat_ctrl->aus_stat_cmd_num[i] = obj_num;
            ret = ERRCODE_SUCC;
            goto end;
        }
    }
    ret = ERRCODE_FAIL;
end:
    dfx_int_restore(lock_stat);
    return ret;
}

errcode_t zdiag_report_stat_obj(zdiag_report_stat_obj_stru_t pkt)
{
    errcode_t ret = ERRCODE_FAIL;
    uint16_t m;
    void *obj = NULL;

    for (m = 0; m < pkt.obj_cnt; m++) {
        obj = (void *)((uint8_t *)pkt.object + m * pkt.obj_size);
        ret = uapi_diag_report_packet(pkt.obj_id, pkt.option, (const uint8_t *)obj, pkt.obj_size, pkt.sync);
        if (ret != ERRCODE_SUCC) {
            break;
        }
    }

    return ret;
}

errcode_t zdiag_query_stat_obj(uint32_t id, uint32_t *obj, uint16_t *obj_len, uint16_t *obj_cnt)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t n;
    uint32_t k;
    zdiag_stat_ctrl_t *ctx = diag_get_stat_ctrl();

    for (n = 0; n < CONFIG_STAT_CMD_LIST_NUM; n++) {
        if (ctx->stat_cmd_list[n] == NULL || ctx->aus_stat_cmd_num[n] == 0) {
            dfx_log_err("stat_cmd_list is null or cmd_num is 0\r\n");
            return ERRCODE_FAIL;
        }

        for (k = 0; k < ctx->aus_stat_cmd_num[n]; k++) {
            const diag_sys_stat_obj_t *tbl = ctx->stat_cmd_list[n];
            const diag_sys_stat_obj_t *node = &tbl[k];
            if ((uint16_t)node->id != id) {
                continue;
            }
            dfx_log_err("node->id=0x%x\r\n", node->id);
            if (obj != NULL && obj_len != NULL && obj_cnt != NULL) {
                *obj = (uint32_t)(uintptr_t)node->stat_packet; /* asume the address is 4bytes. */
                *obj_len = (uint16_t)node->stat_packet_size;
                *obj_cnt = node->array_cnt;
            }

            return ERRCODE_SUCC;
        }
    }

    return ret;
}
