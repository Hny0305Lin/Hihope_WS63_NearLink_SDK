/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: diag debug
 * This file should be changed only infrequently and with great care.
 */

#include "zdiag_debug.h"
#include "diag_adapt_layer.h"
#include "soc_diag_util.h"

#define DIAG_TYPE_CMD 0
#define DIAG_TYPE_IND 1
#define DIAG_TYPE_CMD_ACK 0x02
#define DIAG_TYPE_MGR_ACK 0x03 /* system ack */

uint32_t g_dfx_debug_level = DIAG_LOG_LEVEL_ERROR;

STATIC void printf_mux_packet_ack(const char *str, msp_mux_packet_head_stru_t *mux_packet)
{
    msp_diag_head_cnf_stru_t *cnf = (msp_diag_head_cnf_stru_t *)mux_packet->puc_packet_data;
    dfx_log_debug("[%s][type is ack]\r\n", str);
    dfx_log_debug("[%s][req][sn=0x%04x][param_size=0x%04x][cmd_id=0x%04x][ctrl=0x%02x][pad=0x%02x]\r\n",
                  str, cnf->sn, cnf->param_size, cnf->cmd_id, cnf->ctrl, cnf->pad);
    unused(cnf);
    unused(str);
}

STATIC void printf_mux_packet_ind(const char *str, msp_mux_packet_head_stru_t *mux_packet)
{
    msp_diag_head_ind_stru_t *ind = (msp_diag_head_ind_stru_t *)mux_packet->puc_packet_data;
    dfx_log_debug("[%s][type is ind]\r\n", str);
    dfx_log_debug("[%s][req][cmd_id=0x%04x][param_size=0x%04x]\r\n", str, ind->cmd_id, ind->param_size);
    unused(ind);
    unused(str);
}

STATIC void printf_mux_packet_cmd(const char *str, msp_mux_packet_head_stru_t *mux_packet)
{
    msp_diag_head_req_stru_t *req = (msp_diag_head_req_stru_t *)mux_packet->puc_packet_data;
    dfx_log_debug("[%s][type is cmd]\r\n", str);

    dfx_log_debug("[%s][req][sn=0x%04x][crc16=0x%04x][cmd_id=0x%04x][param_size=0x%04x]\r\n",
                  str, req->sn, req->crc16, req->cmd_id, req->param_size);
    unused(req);
    unused(str);
}

STATIC void printf_mux_packet_unknown(const char *str, const msp_mux_packet_head_stru_t *mux_packet)
{
    unused(str);
    unused(mux_packet);
    dfx_log_debug("[%s][type is unknown]\r\n", str);
}

STATIC void printf_mux_packet_new(const char *str, msp_mux_packet_head_stru_t *mux_packet)
{
    dfx_log_debug("[%s][mux][cmd_id=0x%04x][start_flag=0x%08x][au_id=0x%08x][type=0x%02x][ver=0x%02x][crc16=0x%04x]["
                  "packet_data_size=0x%04x]\r\n",
                  str, mux_packet->cmd_id, mux_packet->start_flag, mux_packet->au_id, mux_packet->type, mux_packet->ver,
                  mux_packet->crc16, mux_packet->packet_data_size);

    switch (mux_packet->type) {
        case DIAG_TYPE_CMD_ACK:
        case DIAG_TYPE_MGR_ACK:
            printf_mux_packet_ack(str, mux_packet);
            break;
        case MUX_PKT_CMD:
            printf_mux_packet_cmd(str, mux_packet);
            break;
        case MUX_PKT_IND:
            printf_mux_packet_ind(str, mux_packet);
            break;
        default:
            printf_mux_packet_unknown(str, mux_packet);
            break;
    }
}

void zdiag_debug_print_pkt_info(const char *str, uint8_t *pkt)
{
    bool remote_pkt = false;
    msp_mux_packet_head_stru_t *mux_head = (msp_mux_packet_head_stru_t *)pkt;
    msp_diag_head_ind_stru_t *ind_head = NULL;
    msp_diag_head_req_stru_t *cmd_head = NULL;

    dfx_log_debug("---------------------------------------- cmd_id=%d S\r\n", mux_head->cmd_id);

    if (mux_head->type == MUX_PKT_CMD) {
        cmd_head = (msp_diag_head_req_stru_t *)mux_head->puc_packet_data;
    } else {
        ind_head = (msp_diag_head_ind_stru_t *)mux_head->puc_packet_data;
    }

    if (remote_pkt) {
        dfx_log_debug("[%s][HCC_HEAD]YES\r\n", str);
    } else {
        dfx_log_debug("[%s][HCC_HEAD]NO\r\n", str);
    }

    dfx_log_debug("[%s][MUX_HEAD]start_flag=0x%x au_id=0x%x type=0x%x ver=0x%x cmd_id=0x%x src=0x%02x dst=0x%02x\r\n",
                  str, mux_head->start_flag, mux_head->au_id, mux_head->type,
                  mux_head->ver, mux_head->cmd_id, mux_head->src, mux_head->dst);

    dfx_log_debug("[%s][MUX_HEAD]src=0x%x dst=0x%x ctrl=0x%x crc16=0x%x packet_data_size=0x%x\r\n",
                  str, mux_head->src, mux_head->dst, mux_head->ctrl, mux_head->crc16, mux_head->packet_data_size);

    if (cmd_head) {
        unused(ind_head);
        dfx_log_debug("[%s][cmd_head]cmd_id=0x%x param_size=0x%x sn=0x%x crc16=0x%x\r\n",
                      str, cmd_head->cmd_id, cmd_head->param_size, cmd_head->sn, cmd_head->crc16);
    } else {
        dfx_log_debug("[%s][ind_head]cmd_id=0x%x param_size=0x%x\r\n", str, ind_head->cmd_id, ind_head->param_size);
    }

    printf_mux_packet_new(str, mux_head);
    dfx_log_debug("---------------------------------------------------E\r\n");
}

void zdiag_pkt_printf(const char *str, const diag_pkt_handle_t *pkt)
{
    msp_mux_packet_head_stru_t *mux = diag_pkt_handle_get_mux(pkt);
    dfx_log_debug("%s cnt=%d data_len[0]=%d data_len[1]=%d data_len[2]=%d\r\n",
                  str, pkt->data_cnt,
                  pkt->data_len[DIAG_PKT_DATA_ID_DATA_0],
                  pkt->data_len[DIAG_PKT_DATA_ID_DATA_1],
                  pkt->data_len[DIAG_PKT_DATA_ID_DATA_2]);
    dfx_log_debug("%s src=%d dst=%d\r\n", str, mux->src, mux->dst);
    unused(mux);
    unused(str);
}
