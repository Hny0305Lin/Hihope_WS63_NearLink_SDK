/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: dfx beat heart
 * This file should be changed only infrequently and with great care.
 */
#include "diag_cmd_beat_heart.h"
#include "diag_cmd_beat_heart_st.h"
#include "diag.h"
#include "stdlib.h"
#include "soc_diag_cmd_id.h"
#include "diag_filter.h"
#include "dfx_adapt_layer.h"
#include "diag_ind_src.h"
#include "errcode.h"


dfx_timer g_diag_beat_heart_timer;
#define CONFIG_DFX_DIAG_BEAT_HEART_PER_TIME (60 * 1000) /* ms */
#define CONFIG_DFX_DIAG_BEAT_HEART_TOTAL_TIME (60 * 6)  /* second */

errcode_t diag_cmd_beat_heart(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    diag_beat_heart_cmd_ind_t *req = cmd_param;
    unused(cmd_id);
    unused(cmd_param_size);

    if (req->dir == DIAG_HEART_BEAT_DIR_DOWN) {
        zdiag_state_beat_heart_pkt_recv(option->peer_addr);
    }
    return ERRCODE_SUCC;
}

void diag_beat_heart_process(void)
{
    uint32_t last_receive_time;
    uint32_t cur_time;
    diag_beat_heart_cmd_ind_t ind;
    diag_option_t option = DIAG_OPTION_INIT_VAL;

    if (zdiag_is_enable() == false) {
        return;
    }

    cur_time = dfx_get_cur_second();
    ind.dir = DIAG_HEART_BEAT_DIR_UP;
    ind.random_data = (uint32_t)rand();
    option.peer_addr = zdiag_get_connect_tool_addr();
    last_receive_time = zdiag_state_get_last_recv_time(option.peer_addr);
    if (cur_time > last_receive_time + CONFIG_DFX_DIAG_BEAT_HEART_TOTAL_TIME) {
        zdiag_set_enable(false, option.peer_addr);
    } else {
#ifndef SUPPORT_DIAG_V2_PROTOCOL
        msp_diag_ack_param_t ack;
        ack.sn = 0;
        ack.ctrl = 0;
        ack.cmd_id = DIAG_CMD_HEART_BEAT;
        ack.param_size = (uint16_t)sizeof(diag_beat_heart_cmd_ind_t);
        ack.param = (uint8_t *)&ind;

        uapi_diag_report_ack(&ack, &option);
#else
        uapi_diag_report_packet(DIAG_CMD_HEART_BEAT, &option, (uint8_t *)&ind,
                                (uint16_t)sizeof(diag_beat_heart_cmd_ind_t), true);
#endif
        dfx_timer_start(&g_diag_beat_heart_timer, CONFIG_DFX_DIAG_BEAT_HEART_PER_TIME);
    }
}

STATIC void diag_beat_heart_timer_call_back(uintptr_t data)
{
    uint32_t ret;

    ret = dfx_msg_write(DFX_MSG_ID_BEAT_HEART, (uint8_t *)data, sizeof(uintptr_t), false);
    if (ret != ERRCODE_SUCC) {
        dfx_timer_start(&g_diag_beat_heart_timer, CONFIG_DFX_DIAG_BEAT_HEART_PER_TIME);
    }
}

STATIC void diag_beat_heart_connect_state_receive(bool enable)
{
    if (enable == true) {
        dfx_timer_start(&g_diag_beat_heart_timer, CONFIG_DFX_DIAG_BEAT_HEART_PER_TIME);
    } else {
        dfx_timer_stop(&g_diag_beat_heart_timer);
    }
}

errcode_t diag_beat_heart_init(void)
{
    zdiag_filter_register_notify_hook(diag_beat_heart_connect_state_receive);
    errcode_t ret = dfx_timer_init(&g_diag_beat_heart_timer, diag_beat_heart_timer_call_back, 0,
        CONFIG_DFX_DIAG_BEAT_HEART_PER_TIME);
    return ret;
}
