/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag pkt
 * This file should be changed only infrequently and with great care.
 */
#include "diag_pkt.h"
#include "diag_adapt_layer.h"
#include "diag.h"
#include "diag_filter.h"
#include "errcode.h"
#include "uapi_crc.h"

errcode_t zdiag_check_mux_pkt(msp_mux_packet_head_stru_t *mux, uint16_t size)
{
    uint16_t crc_val;
    unused(size);
    if (mux->start_flag != MUX_START_FLAG) {
        return ERRCODE_FAIL;
    }

    crc_val = uapi_crc16(0, mux->puc_packet_data, mux->packet_data_size);
    if (crc_val != mux->crc16) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t zdiag_check_hcc_pkt(const uint8_t *data, uint16_t size)
{
    unused(data);
    unused(size);

    return ERRCODE_SUCC;
}

void diag_pkt_handle_init(diag_pkt_handle_t *pkt, uint8_t data_cnt)
{
    memset_s(pkt, sizeof(diag_pkt_handle_t), 0, sizeof(diag_pkt_handle_t));
    pkt->data_cnt = data_cnt;
}

void diag_pkt_set_critical(diag_pkt_handle_t *pkt)
{
    pkt->critical = 1;
}

void diag_pkt_handle_set_data(diag_pkt_handle_t *pkt, uint8_t idx, uint8_t *data, uint16_t data_len,
    diag_pkt_data_type_t attribute)
{
    unused(attribute);
    pkt->data[idx] = data;
    pkt->data_len[idx] = data_len;

    if (((uint32_t)attribute & DIAG_PKT_DATA_ATTRIBUTE_SINGLE_TASK) != 0) {
        pkt->single_task = true;
    }

    if (((uint32_t)attribute & DIAG_PKT_DATA_ATTRIBUTE_DYN_MEM) != 0) {
        pkt->need_free = true;
    }
    return;
}


void zdiag_mk_log_pkt(diag_cmd_log_layer_ind_stru_t *log_pkt, uint32_t module_id, uint32_t msg_id)
{
    STATIC uint32_t msg_sn = 0;
    log_pkt->module = module_id;
    log_pkt->dest_mod = diag_adapt_get_local_addr();
    log_pkt->no = msg_sn++;
    log_pkt->id = msg_id;
    log_pkt->time = diag_adapt_get_msg_time();
}

void zdiag_mk_log_pkt_sn(diag_cmd_log_layer_ind_stru_t *log_pkt, uint32_t module_id, uint32_t msg_id, uint32_t sn)
{
    log_pkt->module = module_id;
    log_pkt->dest_mod = diag_adapt_get_local_addr();
    log_pkt->no = sn;
    log_pkt->id = msg_id;
    log_pkt->time = diag_adapt_get_msg_time();
}

void diag_mk_cnf_header(msp_diag_head_cnf_stru_t *cnf, msp_diag_ack_param_t *ack)
{
    cnf->sn = ack->sn;     /* cmd sn */
    cnf->ctrl = ack->ctrl; /* auto ack:0xfe,initiative ack:0 */
    cnf->pad = ack->pad;
    cnf->cmd_id = ack->cmd_id;         /* cmd id */
    cnf->param_size = ack->param_size; /* param size */
    return;
}

void diag_mk_req_header(msp_diag_head_req_stru_t *req, uint16_t cmd_id, uint16_t packet_size)
{
    req->cmd_id = cmd_id;
    req->param_size = packet_size;
    req->sn = 0;
    req->crc16 = 0;
    return;
}

void diag_mk_ind_header(msp_diag_head_ind_stru_t *ind, uint16_t cmd_id, uint16_t packet_size)
{
    ind->cmd_id = cmd_id;
    ind->param_size = packet_size;
    return;
}

void diag_mk_mux_header_1(msp_mux_packet_head_stru_t *mux, uint8_t type, uint16_t cmd_id, uint16_t pkt_size)
{
    mux->start_flag = MUX_START_FLAG;
    mux->type = type;
    mux->ver = MUX_PKT_VER;
    mux->crc16 = 0;
    mux->src = diag_adapt_get_local_addr();
    mux->cmd_id = cmd_id;
    mux->packet_data_size = pkt_size;
    mux->ctrl = 0;
    mux->pad[0] = 0;
    return;
}

void diag_mk_mux_header_2(msp_mux_packet_head_stru_t *mux, diag_addr dst, uint16_t crc16)
{
    diag_addr dst_tmp = dst;
    if (dst_tmp == 0) {
        dst_tmp = zdiag_get_connect_tool_addr();
    }
    mux->dst = dst_tmp;
    mux->crc16 = crc16;
    return;
}