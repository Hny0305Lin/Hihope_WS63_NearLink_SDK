/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: pkt router
 * This file should be changed only infrequently and with great care.
 */
#include "zdiag_pkt_router.h"
#include "diag_msg.h"
#include "zdiag_mem.h"
#include "zdiag_cmd_dst.h"
#include "zdiag_ind_dst.h"
#include "zdiag_tx.h"
#include "zdiag_debug.h"
#include "diag_adapt_layer.h"
#include "diag_dfx.h"
#include "dfx_adapt_layer.h"
#include "zdiag_mem.h"
#include "errcode.h"

STATIC errcode_t diag_pkt_router_forwarding_exec(diag_pkt_handle_t *pkt, const diag_pkt_process_param_t *process_param)
{
    unused(process_param);
    return zdiag_pkt_router_tx(pkt);
}

STATIC errcode_t diag_pkt_router_forwarding_queue(diag_pkt_handle_t *pkt, const diag_pkt_process_param_t *process_param)
{
    errcode_t ret;
    diag_pkt_msg_t msg;
    unused(process_param);

    zdiag_pkt_printf("forward_a", pkt);
    ret = diag_pkt_need_cross_task(pkt);
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("[ERROR][%s][%d][errcode=%u]\r\n", __func__, __LINE__, ret);
        return ret;
    }
    zdiag_pkt_printf("forward_b", pkt);

    msg.pkt = *pkt;
    msg.cur_proc = DIAG_PKT_PPOC_OUT_PUT_Q;
    ret = dfx_msg_write(DFX_MSG_ID_DIAG_PKT, (uint8_t *)&msg, sizeof(diag_pkt_msg_t), false);
    if (ret == ERRCODE_SUCC) {
        diag_pkt_handle_clean(pkt);
    }
    return ret;
}

STATIC errcode_t diag_pkt_router_local_exec(diag_pkt_handle_t *pkt, const diag_pkt_process_param_t *process_param)
{
    msp_mux_packet_head_stru_t *mux = diag_pkt_handle_get_mux(pkt);
    mux_pkt_type_t mux_type = mux->type;
    diag_option_t option = DIAG_OPTION_INIT_VAL;
    option.peer_addr = mux->src;

    unused(process_param);

    dfx_log_debug("diag_pkt_router_local_exec mux_type=%d\r\n", mux_type);
    switch (mux_type) {
        case MUX_PKT_CMD:
            return diag_pkt_router_run_cmd(pkt, &option);
        case MUX_PKT_IND:
            return diag_pkt_router_run_ind(pkt, &option);
        default:
            return ERRCODE_FAIL;
    }
}

STATIC errcode_t diag_pkt_router_local_queue(diag_pkt_handle_t *pkt, const diag_pkt_process_param_t *process_param)
{
    errcode_t ret;
    diag_pkt_msg_t msg;

    unused(process_param);

    ret = diag_pkt_need_cross_task(pkt);
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("[ERROR][%s][%d][errcode=%u]\r\n", __func__, __LINE__, ret);
        return ret;
    }

    msg.cur_proc = DIAG_PKT_PPOC_LOCAL_EXEC_Q;
    msg.pkt = *pkt;
    ret = dfx_msg_write(DFX_MSG_ID_DIAG_PKT, (uint8_t *)&msg, sizeof(diag_pkt_msg_t), false);
    if (ret == ERRCODE_SUCC) {
        zdiag_dfx_send_local_q_success();
        diag_pkt_handle_clean(pkt);
    } else {
        zdiag_dfx_send_local_q_fail();
    }
    return ret;
}

STATIC errcode_t diag_pkt_router_out_put(diag_pkt_handle_t *pkt, diag_pkt_process_param_t *process_param)
{
    diag_pkt_proc_t cur_proc = process_param->cur_proc;

    if (cur_proc == DIAG_PKT_PPOC_OUT_PUT_Q || cur_proc == DIAG_PKT_PROC_USR_SYNC_CMD_IND) {
        return diag_pkt_router_forwarding_exec(pkt, process_param);
    } else {
        return diag_pkt_router_forwarding_queue(pkt, process_param);
    }
}

STATIC errcode_t diag_pkt_router_local(diag_pkt_handle_t *pkt, diag_pkt_process_param_t *process_param)
{
    switch (process_param->cur_proc) {
        case DIAG_PKT_PROC_PORT_PKT:
        case DIAG_PKT_PROC_USR_ASYNC_CMD_IND:
            return diag_pkt_router_local_queue(pkt, process_param);
        case DIAG_PKT_PPOC_LOCAL_EXEC_Q:
        case DIAG_PKT_PROC_USR_SYNC_CMD_IND:
            return diag_pkt_router_local_exec(pkt, process_param);
        default:
            return ERRCODE_FAIL;
    }
}

errcode_t diag_pkt_router(diag_pkt_handle_t *pkt, diag_pkt_process_param_t *process_param)
{
    errcode_t ret;
    diag_addr dst_addr, local_addr;
    msp_mux_packet_head_stru_t *mux = diag_pkt_handle_get_mux(pkt);

    dfx_log_debug("%s\r\n", __func__);
    dfx_log_debug("cur_proc=%d src=%d dst=%d\r\n", process_param->cur_proc, mux->src, mux->dst);
    dfx_log_debug("type=%d cmd_id=%d need_free=%d\r\n", mux->type, mux->cmd_id, pkt->need_free);

    dst_addr = mux->dst;
    local_addr = diag_adapt_get_local_addr();
    if (dst_addr != local_addr) {
        ret = diag_pkt_router_out_put(pkt, process_param);
    } else {
        ret = diag_pkt_router_local(pkt, process_param);
    }

    diag_pkt_free(pkt);
    return ret;
}

void diag_pkt_msg_proc(uint32_t msg_id, uint8_t *msg, uint32_t msg_len)
{
    diag_pkt_msg_t *pkt_msg = (diag_pkt_msg_t *)msg;
    diag_pkt_process_param_t process_param;
    process_param.cur_proc = pkt_msg->cur_proc;
    diag_pkt_router(&pkt_msg->pkt, &process_param);
    unused(msg_id);
    unused(msg_len);
}
