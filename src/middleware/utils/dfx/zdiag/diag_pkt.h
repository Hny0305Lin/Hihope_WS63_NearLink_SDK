/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag pkt
 * This file should be changed only infrequently and with great care.
 */
#ifndef __ZDIAG_PKT_H__
#define __ZDIAG_PKT_H__
#include "diag_common.h"
#include "diag.h"


typedef enum {
    DIAG_PKT_MEM_TYPE_HEAD_STACK_DATA_STACK,
    DIAG_PKT_MEM_TYPE_HEAD_DATA_DFX_MALLOC,
} diag_pkt_mem_type_t;

typedef enum {
    DIAG_PKT_DATA_ID_DATA_0,
    DIAG_PKT_DATA_ID_DATA_1,
    DIAG_PKT_DATA_ID_DATA_2,
    DIAG_PKT_DATA_ID_USR_MAX,
} diag_pkt_data_id_t;

#define DIAG_PKT_DATA_ATTRIBUTE_SINGLE_TASK 0x1
#define DIAG_PKT_DATA_ATTRIBUTE_DYN_MEM 0x2

typedef enum {
    DIAG_PKT_DATA_STACK = DIAG_PKT_DATA_ATTRIBUTE_SINGLE_TASK,
    DIAG_PKT_DATA_DFX_MALLOC = DIAG_PKT_DATA_ATTRIBUTE_DYN_MEM,
} diag_pkt_data_type_t;

typedef struct {
    uint8_t *data[DIAG_PKT_DATA_ID_USR_MAX];
    uint16_t data_len[DIAG_PKT_DATA_ID_USR_MAX];
    uint8_t data_cnt : 2;
    uint8_t need_free : 1;      /* true:pkt有数据需要释放 false:pkt无数据需要释放 */
    uint8_t single_task : 1;    /* true:pkt有数据无法跨任务传递 false:pkt无数据无法跨任务传递 */
    uint8_t critical : 1;       /* 非调度报文，如：死机 */
} diag_pkt_handle_t;

/**
 * @if Eng
 * @brief  Structure for diag ack.
 * @else
 * @brief  diag应答结构体
 * @endif
 */
typedef struct {
    uint16_t sn;               /*!< @if Eng cmd sn.
                                    @else   命令sn @endif */
    uint8_t ctrl;              /*!< @if Eng ack mode.
                                    @else   应答模式 @endif */
    uint8_t pad;
    uint16_t cmd_id;           /*!< @if Eng cmd id.
                                    @else   应答id @endif */
    uint16_t param_size;       /*!< @if Eng param size.
                                    @else   应答内容大小 @endif */
    uint8_t *param;            /*!< @if Eng data.
                                    @else   应答内容 @endif */
} msp_diag_ack_param_t;

errcode_t zdiag_check_mux_pkt(msp_mux_packet_head_stru_t *mux, uint16_t size);
errcode_t zdiag_check_hcc_pkt(const uint8_t *data, uint16_t size);
void diag_pkt_handle_init(diag_pkt_handle_t *pkt, uint8_t data_cnt);
void diag_pkt_set_critical(diag_pkt_handle_t *pkt);
void diag_pkt_handle_set_data(diag_pkt_handle_t *pkt, uint8_t idx, uint8_t *data,
                              uint16_t data_len, diag_pkt_data_type_t attribute);

void zdiag_mk_log_pkt(diag_cmd_log_layer_ind_stru_t *log_pkt, uint32_t module_id, uint32_t msg_id);
void zdiag_mk_log_pkt_sn(diag_cmd_log_layer_ind_stru_t *log_pkt, uint32_t module_id, uint32_t msg_id, uint32_t sn);
void diag_mk_ind_header(msp_diag_head_ind_stru_t *ind, uint16_t cmd_id, uint16_t packet_size);
void diag_mk_req_header(msp_diag_head_req_stru_t *req, uint16_t cmd_id, uint16_t packet_size);
void diag_mk_cnf_header(msp_diag_head_cnf_stru_t *cnf, msp_diag_ack_param_t *ack);
void diag_mk_mux_header_1(msp_mux_packet_head_stru_t *mux, uint8_t type, uint16_t cmd_id, uint16_t pkt_size);
void diag_mk_mux_header_2(msp_mux_packet_head_stru_t *mux, diag_addr dst, uint16_t crc16);

static inline msp_mux_packet_head_stru_t *diag_pkt_handle_get_mux(const diag_pkt_handle_t *pkt)
{
    return (msp_mux_packet_head_stru_t *)pkt->data[0];
}

static inline msp_diag_head_ind_stru_t *diag_receive_pkt_handle_get_ind(const diag_pkt_handle_t *pkt)
{
    return (msp_diag_head_ind_stru_t *)(pkt->data[0] + DIAG_MUX_HEADER_SIZE);
}

static inline uint8_t *diag_receive_pkt_handle_get_ind_data(const diag_pkt_handle_t *pkt)
{
    return pkt->data[0] + DIAG_MUX_HEADER_SIZE + DIAG_IND_HEADER_SIZE;
}

static inline msp_diag_head_req_stru_t *diag_receive_pkt_handle_get_req(const diag_pkt_handle_t *pkt)
{
    return (msp_diag_head_req_stru_t *)(pkt->data[0] + DIAG_MUX_HEADER_SIZE);
}

static inline uint8_t *diag_receive_pkt_handle_get_req_data(const diag_pkt_handle_t *pkt)
{
    return pkt->data[0] + DIAG_MUX_HEADER_SIZE + DIAG_REQ_HEADER_SIZE;
}

static inline uint32_t diag_pkt_handle_get_total_size(const diag_pkt_handle_t *pkt)
{
    uint32_t total_size = 0;
    for (int i = 0; i < pkt->data_cnt; i++) {
        total_size += (uint32_t)pkt->data_len[i];
    }
    return total_size;
}

static inline void diag_pkt_handle_clean(diag_pkt_handle_t *pkt)
{
    pkt->need_free = false;
    for (int i = 0; i < pkt->data_cnt; i++) {
        pkt->data[i] = NULL;
        pkt->data_len[i] = 0;
    }
    pkt->data_cnt = 0;
}

static inline void diag_pkt_cnt_increase(diag_pkt_handle_t *pkt, uint8_t cnt)
{
    pkt->data_cnt += cnt;
}

#endif
