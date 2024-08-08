/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: zdiag for sample data.
 */

#include "securec.h"
#include "diag.h"
#include "soc_osal.h"
#include "soc_diag_util.h"
#include "soc_diag_cmd_id.h"
#include "diag_sample_data_st.h"
#include "dfx_adapt_layer.h"
#include "diag_sample_data.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_DIAG_SAMPLE_DATA_C

#undef THIS_MOD_ID
#define THIS_MOD_ID DIAG_MOD_ID_OAM_HOST

#define DIAG_SAMPLE_TRANSMIT_FINISH 11

diag_sample_func_cb g_diag_sample_cb[ZDIAG_SAMPLE_CB_END] = {NULL};
diag_sample_record g_sample_running = {0};

errcode_t diag_cmd_wlan_module_sample_data(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                           diag_option_t *option)
{
    diag_btsoc_sample_cmd *cmd = (diag_btsoc_sample_cmd *)cmd_param;
    uint32_t idx = cmd_id - DIAG_CMD_ID_BGLE_SAMPLE;
    diag_sample_data_ind_t ack = {ERRCODE_FAIL, 0, 0};
    diag_sample_func_cb func_cb = (diag_sample_func_cb)g_diag_sample_cb[idx];

    if (func_cb == NULL) {
        osal_printk("diag_cmd_wlan_module_sample_data:sample func_cb is NULL[%d].", idx);
        goto report_ack;
    }

    osal_printk("flag:%d, transmit_id:%d, sample_size:%d, sample_type:%d,msg_cnt:%d.\n",
                cmd->flag, cmd->transmit_id, cmd->sample_size, cmd->sample_type, g_sample_running.msg_cnt);

    if (g_sample_running.running == false && cmd->flag == ZDIAG_SAMPLE_START) { /* start */
        g_sample_running.running = true;
        ack.ret = func_cb(cmd_param, cmd_param_size);
    } else if (g_sample_running.running == true && cmd->flag == ZDIAG_SAMPLE_STOP && /* stop */
        g_sample_running.msg_cnt == 0) {
        g_sample_running.running = false;
        ack.ret = func_cb(cmd_param, cmd_param_size);
    } else {
        osal_printk("diag_cmd_wlan_module_sample_data:sample_running:%d, sample_start:%d, msg_cnt:%d.\n",
            g_sample_running.running, cmd->flag, g_sample_running.msg_cnt);
        goto report_ack;
    }

    g_sample_running.cmd_id = cmd_id;
    g_sample_running.transmit_id = cmd->transmit_id;
    g_sample_running.offset = 0;
    g_sample_running.msg_cnt = 0;

report_ack:
    ack.flag = cmd->flag;
    ack.transmit_id = cmd->transmit_id;
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ack, sizeof(diag_sample_data_ind_t), true);
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_cmd_report_data(uint8_t *buf, uint32_t len)
{
    diag_sample_data_reply_pkt_t *reply = NULL;
    uint32_t reply_size = (uint32_t)sizeof(diag_sample_data_reply_pkt_t) + len;
    reply = (diag_sample_data_reply_pkt_t *)dfx_malloc(0, reply_size);
    if (reply == NULL || buf == NULL) {
        return ERRCODE_FAIL;
    }
    g_sample_running.offset += len;
    reply->transmit_id = g_sample_running.transmit_id;
    reply->offset = g_sample_running.offset;
    reply->size = len;
    reply->ret = ERRCODE_SUCC;
    if (memcpy_s(reply->data, len, buf, len) != EOK) {
        reply->ret = ERRCODE_FAIL;
        osal_printk("diag_cmd_report_data:memcpy_s error, len:%d.\n", len);
    }
    uapi_diag_report_packet(DIAG_CMD_ID_SAMPLE_DATA, NULL, (uint8_t *)reply, (uint16_t)reply_size, true);
    dfx_free(0, reply);
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_cmd_report_finish(void)
{
    diag_sample_notify finish = {0};
    g_sample_running.running = false;
    finish.transmit_id = g_sample_running.transmit_id;
    finish.state_code = DIAG_SAMPLE_TRANSMIT_FINISH;
    osal_printk("diag_cmd_report_finish:transmit_id:%d, report_msg:%d.\n",
        finish.transmit_id, g_sample_running.msg_cnt);
    uapi_diag_report_packet(DIAG_CMD_ID_SAMPLE_FINISH, NULL, (uint8_t *)&finish, sizeof(diag_sample_notify), true);
    return ERRCODE_SUCC;
}

errcode_t diag_report_wlan_sample_data(const uint8_t *buf, uint32_t len, uint32_t msg_id)
{
    if (len == 0) {
        return diag_cmd_report_finish();
    }
    g_sample_running.msg_cnt++;
    return uapi_diag_report_sys_msg(THIS_MOD_ID, msg_id, buf, (uint16_t)len, DIAG_LOG_LEVEL_WARN);
}

errcode_t diag_cmd_report_sample_data(uint8_t *buf, uint32_t len)
{
    if (len == 0) {
        return diag_cmd_report_finish();
    }
    g_sample_running.msg_cnt++;
    return diag_cmd_report_data(buf, len);
}

void diag_sample_data_register(diag_sample_cb_enum idx, diag_sample_func_cb func)
{
    if (idx >= ZDIAG_SAMPLE_CB_END) {
        return;
    }
    g_diag_sample_cb[idx] = func;
}

