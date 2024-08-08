/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag ind process
 * This file should be changed only infrequently and with great care.
 */

#include "zdiag_ind_dst.h"
#include "diag.h"
#include "diag_adapt_layer.h"
#include "errcode.h"

typedef struct {
    const diag_cmd_reg_obj_t *user_cmd_list[CONFIG_DIAG_IND_TBL_NUM];
    uint16_t aus_usert_cmd_num[CONFIG_DIAG_IND_TBL_NUM]; /* cmd obj num */
} diag_ind_ctrl_t;

STATIC diag_ind_ctrl_t g_diag_ind_ctrl;

STATIC inline diag_ind_ctrl_t *diag_get_ind_ctrl(void)
{
    return &g_diag_ind_ctrl;
}

STATIC diag_cmd_f diag_find_usr_ind_proc_func(uint32_t cmd_id)
{
    diag_ind_ctrl_t *cmd_ctrl = diag_get_ind_ctrl();
    uint32_t i;
    uint16_t k;
    for (i = 0; i < CONFIG_DIAG_IND_TBL_NUM; i++) {
        for (k = 0; k < cmd_ctrl->aus_usert_cmd_num[i]; k++) {
            const diag_cmd_reg_obj_t *cmd_tbl = cmd_ctrl->user_cmd_list[i];
            const diag_cmd_reg_obj_t *cmd_list = &cmd_tbl[k];

            if ((cmd_id >= cmd_list->min_id) && (cmd_id <= cmd_list->max_id)) {
                diag_cmd_f cmd;
                cmd = (diag_cmd_f)cmd_list->fn_input_cmd;
                return cmd;
            }
        }
    }
    return NULL;
}

errcode_t diag_pkt_router_run_ind(diag_pkt_handle_t *pkt, const diag_option_t *option)
{
    msp_diag_head_ind_stru_t *ind_head = diag_receive_pkt_handle_get_ind(pkt);
    diag_option_t new_option;
    uint8_t *usr_data = diag_receive_pkt_handle_get_ind_data(pkt);
    diag_cmd_f cmd_f = diag_find_usr_ind_proc_func(ind_head->cmd_id);
    if (cmd_f) {
        new_option = *option;
        cmd_f(ind_head->cmd_id, usr_data, ind_head->param_size, &new_option);
        return ERRCODE_SUCC;
    }

    return ERRCODE_FAIL;
}

STATIC errcode_t diag_ind_tbl_check(const diag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num)
{
    unused(cmd_tbl);
    unused(cmd_num);

    return ERRCODE_SUCC;
}

errcode_t uapi_diag_register_ind(const diag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num)
{
    errcode_t ret;
    uint32_t lock_stat;
    int i;
    diag_ind_ctrl_t *cmd_ctrl = diag_get_ind_ctrl();

    lock_stat = dfx_int_lock();
    ret = diag_ind_tbl_check(cmd_tbl, cmd_num);
    if (ret != ERRCODE_SUCC) {
        goto end;
    }

    for (i = 0; i < CONFIG_DIAG_IND_TBL_NUM; i++) {
        if ((cmd_ctrl->user_cmd_list[i] == NULL) || (cmd_ctrl->aus_usert_cmd_num[i] == 0)) {
            cmd_ctrl->user_cmd_list[i] = cmd_tbl;
            cmd_ctrl->aus_usert_cmd_num[i] = cmd_num;
            ret = ERRCODE_SUCC;
            goto end;
        }
    }
    ret = ERRCODE_FAIL;
end:
    dfx_int_restore(lock_stat);
    return ret;
}
