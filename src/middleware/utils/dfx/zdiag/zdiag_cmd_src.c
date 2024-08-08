/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag cmd producer
 * This file should be changed only infrequently and with great care.
 */
#include "zdiag_cmd_src.h"
#include "zdiag_mem.h"
#include "zdiag_pkt_router.h"
#include "zdiag_debug.h"
#include "diag_adapt_layer.h"
#include "diag_pkt.h"
#include "uapi_crc.h"

errcode_t uapi_diag_run_cmd(uint16_t cmd_id, uint8_t *data, uint16_t data_size, diag_option_t *option)
{
    errcode_t ret;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param;
    uint8_t buf[DIAG_MUX_HEADER_SIZE + DIAG_REQ_HEADER_SIZE];
    msp_mux_packet_head_stru_t *mux = (msp_mux_packet_head_stru_t *)buf;
    msp_diag_head_req_stru_t *req = (msp_diag_head_req_stru_t *)(buf + DIAG_MUX_HEADER_SIZE);
    uint16_t crc16 = 0;

    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_0, (uint8_t *)mux,
                             DIAG_MUX_HEADER_SIZE + DIAG_REQ_HEADER_SIZE, DIAG_PKT_DATA_STACK);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_1, (uint8_t *)data,
                             data_size, DIAG_PKT_DATA_STACK);

    diag_mk_req_header(req, cmd_id, data_size);
    crc16 = uapi_crc16(crc16, (uint8_t*)req, DIAG_REQ_HEADER_SIZE);
    crc16 = uapi_crc16(crc16, data, data_size);
    diag_mk_mux_header_1(mux, MUX_PKT_CMD, cmd_id, DIAG_REQ_HEADER_SIZE + data_size);
    diag_mk_mux_header_2(mux, option->peer_addr, crc16);

    process_param.cur_proc = DIAG_PKT_PROC_USR_ASYNC_CMD_IND;
    ret = diag_pkt_router(&pkt, &process_param);
    return ret;
}
