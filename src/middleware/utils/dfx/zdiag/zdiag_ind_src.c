/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag ind producer
 * This file should be changed only infrequently and with great care.
 */
#include "diag_ind_src.h"
#include "securec.h"
#include "zdiag_mem.h"
#include "zdiag_pkt_router.h"
#include "zdiag_debug.h"
#include "soc_diag_cmd_id.h"
#include "diag_adapt_layer.h"
#include "diag_dfx.h"
#include "diag_filter.h"
#include "dfx_adapt_layer.h"
#include "diag_pkt.h"
#include "errcode.h"
#include "log_file.h"
#include "diag_msg.h"
#include "uapi_crc.h"

#ifndef DIAG_CMD_MSG_RPT_SYS
#define DIAG_CMD_MSG_RPT_SYS DIAG_CMD_MSG_RPT_SYS_FULLY_LOG
#endif
#define DIAG_LOG_HEADER_SIZE DIAG_FULL_LOG_HEADER_SIZE

STATIC bool zdiag_msg_permit(uint32_t module_id, uint8_t level)
{
    if ((zdiag_is_enable() == false) && (uapi_zdiag_offline_log_is_enable() != true)) {
        return false;
    }

    return zdiag_log_enable(level, module_id);
}

errcode_t uapi_diag_report_packet(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet, uint16_t packet_size,
    bool sync)
{
    errcode_t ret = ERRCODE_SUCC;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param;
    uint16_t crc16 = 0;
    diag_addr dst = 0;
    if (option != NULL) {
        dst = option->peer_addr;
    }

    dfx_log_debug("uapi_diag_report_packet in\r\n");
    if (zdiag_is_enable() == false) {
        dfx_log_err("diag_unconnect\r\n");
        return ERRCODE_FAIL;
    }

    uint8_t *buf = dfx_malloc(0, DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE);
    msp_mux_packet_head_stru_t *mux = (msp_mux_packet_head_stru_t *)buf;
    msp_diag_head_ind_stru_t *ind = (msp_diag_head_ind_stru_t *)(buf + DIAG_MUX_HEADER_SIZE);

    diag_pkt_handle_init(&pkt, 2); /* data_cnt 为2 */
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_0, (uint8_t *)mux, DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE,
        DIAG_PKT_DATA_STACK);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_1, (uint8_t *)packet, packet_size, DIAG_PKT_DATA_STACK);

    diag_mk_ind_header(ind, cmd_id, packet_size);
    crc16 = uapi_crc16(crc16, (uint8_t *)ind, DIAG_IND_HEADER_SIZE);
    crc16 = uapi_crc16(crc16, packet, packet_size);
    diag_mk_mux_header_1(mux, MUX_PKT_IND, cmd_id, DIAG_IND_HEADER_SIZE + packet_size);
    diag_mk_mux_header_2(mux, dst, crc16);
    if (sync) {
        process_param.cur_proc = DIAG_PKT_PROC_USR_SYNC_CMD_IND;
    } else {
        process_param.cur_proc = DIAG_PKT_PROC_USR_ASYNC_CMD_IND;
    }
    diag_pkt_router(&pkt, &process_param);
    dfx_free(0, buf);
    return ret;
}

STATIC errcode_t zdiag_report_packets(uint16_t cmd_id, diag_option_t *option, diag_report_packet *report_packet,
    uint8_t critical)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t **packet = report_packet->packet;
    uint16_t *packet_size = report_packet->packet_size;
    uint8_t pkt_cnt = report_packet->pkt_cnt;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param;
    uint8_t buf[DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE];
    msp_mux_packet_head_stru_t *mux = (msp_mux_packet_head_stru_t *)buf;
    msp_diag_head_ind_stru_t *ind = (msp_diag_head_ind_stru_t *)(buf + DIAG_MUX_HEADER_SIZE);
    uint16_t crc16 = 0;
    uint16_t usr_data_size = 0;
    diag_addr dst = 0;
    if (option != NULL) {
        dst = option->peer_addr;
    }

    dfx_log_debug("%s in\r\n", __func__);
    if (zdiag_is_enable() == false) {
        dfx_log_err("diag_unconnect\r\n");
        return ERRCODE_FAIL;
    }

    if (pkt_cnt > DIAG_PKT_DATA_ID_USR_MAX - 1) {
        dfx_log_err("[ERROR][%s][%d]\r\n", __func__, __LINE__);
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt, pkt_cnt + 1);
    if (critical != 0) {
        diag_pkt_set_critical(&pkt);
    }
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_0, (uint8_t *)mux, DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE,
        DIAG_PKT_DATA_STACK);
    for (int i = 0; i < pkt_cnt; i++) {
        diag_pkt_handle_set_data(&pkt, (uint8_t)(i + 1), (uint8_t *)packet[i], packet_size[i], DIAG_PKT_DATA_STACK);
        usr_data_size += packet_size[i];
    }

    diag_mk_ind_header(ind, cmd_id, usr_data_size);
    crc16 = uapi_crc16(crc16, (uint8_t *)ind, DIAG_IND_HEADER_SIZE);
    for (int i = 0; i < pkt_cnt; i++) {
        crc16 = uapi_crc16(crc16, packet[i], packet_size[i]);
    }
    diag_mk_mux_header_1(mux, MUX_PKT_IND, cmd_id, DIAG_IND_HEADER_SIZE + usr_data_size);
    diag_mk_mux_header_2(mux, dst, crc16);

    process_param.cur_proc = DIAG_PKT_PROC_USR_SYNC_CMD_IND;
    diag_pkt_router(&pkt, &process_param);
    return ret;
}

errcode_t uapi_diag_report_packets_critical(uint16_t cmd_id, diag_option_t *option, uint8_t **packet,
    uint16_t *packet_size, uint8_t pkt_cnt)
{
    diag_report_packet pkt;
    pkt.packet = packet;
    pkt.packet_size = packet_size;
    pkt.pkt_cnt = pkt_cnt;
    return zdiag_report_packets(cmd_id, option, &pkt, 1);
}

errcode_t uapi_diag_report_packets_normal(uint16_t cmd_id, diag_option_t *option, uint8_t **packet,
    uint16_t *packet_size, uint8_t pkt_cnt)
{
    diag_report_packet pkt;
    pkt.packet = packet;
    pkt.packet_size = packet_size;
    pkt.pkt_cnt = pkt_cnt;
    return zdiag_report_packets(cmd_id, option, &pkt, 0);
}

errcode_t uapi_diag_report_ack(msp_diag_ack_param_t *ack, diag_option_t *option)
{
    errcode_t ret;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param;
    uint8_t buf[DIAG_MUX_HEADER_SIZE + DIAG_CNF_HEADER_SIZE];
    msp_mux_packet_head_stru_t *mux = (msp_mux_packet_head_stru_t *)buf;
    msp_diag_head_cnf_stru_t *cnf = (msp_diag_head_cnf_stru_t *)(buf + DIAG_MUX_HEADER_SIZE);
    uint16_t crc16 = 0;
    diag_addr dst = 0;
    if (option != NULL) {
        dst = option->peer_addr;
    }

    diag_pkt_handle_init(&pkt, 2); /* data_cnt 为2 */
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_0, (uint8_t *)mux, DIAG_MUX_HEADER_SIZE + DIAG_CNF_HEADER_SIZE,
        DIAG_PKT_DATA_STACK);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_1, (uint8_t *)ack->param, ack->param_size,
        DIAG_PKT_DATA_STACK);

    diag_mk_cnf_header(cnf, ack);
    crc16 = uapi_crc16(crc16, (uint8_t *)cnf, DIAG_CNF_HEADER_SIZE);
    crc16 = uapi_crc16(crc16, ack->param, ack->param_size);
    diag_mk_mux_header_1(mux, MUX_PKT_ACK, ack->cmd_id, DIAG_CNF_HEADER_SIZE + ack->param_size);
    diag_mk_mux_header_2(mux, dst, crc16);

    process_param.cur_proc = DIAG_PKT_PROC_USR_SYNC_CMD_IND;
    ret = diag_pkt_router(&pkt, &process_param);
    return ret;
}

errcode_t uapi_zdiag_report_sys_msg_instance(uint32_t module_id, uint32_t msg_id, const uint8_t *packet,
    uint16_t packet_size, uint8_t level)
{
    errcode_t ret;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param;
    uint8_t buf[DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE];
    msp_mux_packet_head_stru_t *mux = (msp_mux_packet_head_stru_t *)buf;
    msp_diag_head_ind_stru_t *ind = (msp_diag_head_ind_stru_t *)(buf + DIAG_MUX_HEADER_SIZE);
    diag_cmd_log_layer_ind_stru_t *log =
        (diag_cmd_log_layer_ind_stru_t *)(buf + DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE);
    uint16_t crc16 = 0;

    if (zdiag_msg_permit(module_id, level) == false) {
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt, 1); /* data_cnt 为1 */
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_0, (uint8_t *)mux,
        DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE, DIAG_PKT_DATA_STACK);
    if (packet_size != 0) {
        diag_pkt_cnt_increase(&pkt, 1);
        diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_1, (uint8_t *)packet, packet_size, DIAG_PKT_DATA_STACK);
    }

    diag_mk_ind_header(ind, DIAG_CMD_MSG_RPT_SYS, DIAG_LOG_HEADER_SIZE + packet_size);
    zdiag_mk_log_pkt(log, module_id, msg_id);
    crc16 = uapi_crc16(crc16, (uint8_t *)ind, DIAG_IND_HEADER_SIZE);
    crc16 = uapi_crc16(crc16, (uint8_t *)log, DIAG_LOG_HEADER_SIZE);
    if (packet_size != 0) {
        crc16 = uapi_crc16(crc16, packet, packet_size);
    }

    diag_mk_mux_header_1(mux, MUX_PKT_IND, DIAG_CMD_MSG_RPT_SYS,
        DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE + packet_size);
    diag_mk_mux_header_2(mux, 0, crc16);

    process_param.cur_proc = DIAG_PKT_PROC_USR_SYNC_CMD_IND;

#if CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES
    if (uapi_zdiag_offline_log_is_enable()) {
        uint32_t data_len = DIAG_LOG_HEADER_SIZE + (uint32_t)packet_size;
        uint8_t *data = (uint8_t *)dfx_malloc(0, data_len);
        if (data == NULL) {
            return ERRCODE_MALLOC;
        }
        memcpy_s(data, data_len, log, DIAG_LOG_HEADER_SIZE);
        memcpy_s(data + DIAG_LOG_HEADER_SIZE, data_len - DIAG_LOG_HEADER_SIZE, packet, (uint32_t)packet_size);
        ret = uapi_logfile_write(STORE_DIAG, 0, data, data_len);
        if (ret != ERRCODE_SUCC && ret != ERRCODE_DFX_LOGFILE_SUSPENDED) {
            printf("offline log wrtie failed, ret = 0x%x\r\n", ret);
        }
        dfx_free(0, data);
    } else {
        ret = diag_pkt_router(&pkt, &process_param);
    }
#else
    ret = diag_pkt_router(&pkt, &process_param);
#endif
    return ret;
}

errcode_t uapi_zdiag_report_sys_msg_instance_sn(uint32_t module_id, uint32_t msg_id,
    diag_report_sys_msg_packet *report_sys_msg_packet, uint8_t level, uint32_t sn)
{
    uint8_t *packet = report_sys_msg_packet->packet;
    uint16_t packet_size = report_sys_msg_packet->packet_size;
    errcode_t ret;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param;
    uint8_t buf[DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE];
    msp_mux_packet_head_stru_t *mux = (msp_mux_packet_head_stru_t *)buf;
    msp_diag_head_ind_stru_t *ind = (msp_diag_head_ind_stru_t *)(buf + DIAG_MUX_HEADER_SIZE);
    diag_cmd_log_layer_ind_stru_t *log =
        (diag_cmd_log_layer_ind_stru_t *)(buf + DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE);
    uint16_t crc16 = 0;

    if (zdiag_msg_permit(module_id, level) == false) {
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt, 1); /* data_cnt 为1 */
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_0, (uint8_t *)mux,
        DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE, DIAG_PKT_DATA_STACK);
    if (packet_size != 0) {
        diag_pkt_cnt_increase(&pkt, 1);
        diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_DATA_1, (uint8_t *)packet, packet_size, DIAG_PKT_DATA_STACK);
    }

    diag_mk_ind_header(ind, DIAG_CMD_MSG_RPT_SYS, DIAG_LOG_HEADER_SIZE + packet_size);
    zdiag_mk_log_pkt_sn(log, module_id, msg_id, sn);
    crc16 = uapi_crc16(crc16, (uint8_t *)ind, DIAG_IND_HEADER_SIZE);
    crc16 = uapi_crc16(crc16, (uint8_t *)log, DIAG_LOG_HEADER_SIZE);
    if (packet_size != 0) {
        crc16 = uapi_crc16(crc16, packet, packet_size);
    }

    diag_mk_mux_header_1(mux, MUX_PKT_IND, DIAG_CMD_MSG_RPT_SYS,
        DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE + packet_size);
    diag_mk_mux_header_2(mux, 0, crc16);

    process_param.cur_proc = DIAG_PKT_PROC_USR_SYNC_CMD_IND;
    ret = diag_pkt_router(&pkt, &process_param);
    return ret;
}

errcode_t uapi_zdiag_report_packet(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
                                   uint16_t packet_size, bool sync)
{
    return uapi_diag_report_packet(cmd_id, option, packet, packet_size, sync);
}