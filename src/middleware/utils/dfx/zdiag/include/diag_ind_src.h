/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag ind producer
 * This file should be changed only infrequently and with great care.
 */
#ifndef __ZDIAG_IND_SRC_H__
#define __ZDIAG_IND_SRC_H__

#include "errcode.h"
#include "diag_common.h"
#include "diag.h"
#include "diag_pkt.h"

typedef struct {
    diag_addr src;
    diag_addr dst;
    uint16_t cmd_id;
    uint16_t ctrl;
    uint16_t len;
    uint8_t data[0];
} zdiag_ind_msg_t;

typedef struct {
    diag_addr src;
    diag_addr dst;
    uint16_t cmd_id;
    uint16_t ctrl;
    uint16_t len;
    uint8_t *data;
} diag_ind_param_t;

typedef struct {
    uint8_t *packet;
    uint16_t packet_size;
} diag_report_sys_msg_packet;

typedef struct {
    uint8_t **packet;
    uint16_t *packet_size;
    uint8_t pkt_cnt;
} diag_report_packet;

typedef errcode_t (*report_msg_func)(uint32_t module_id, uint32_t msg_id, const uint8_t *buf, uint16_t buf_size,
    uint8_t level);

errcode_t uapi_diag_report_ack(msp_diag_ack_param_t *ack, diag_option_t *option);
errcode_t uapi_zdiag_report_sys_msg_instance(uint32_t module_id, uint32_t msg_id, const uint8_t *packet,
    uint16_t packet_size, uint8_t level);
errcode_t uapi_zdiag_report_sys_msg_instance_sn(uint32_t module_id, uint32_t msg_id,
    diag_report_sys_msg_packet *report_sys_msg_packet, uint8_t level, uint32_t sn);
errcode_t uapi_zdiag_report_packet(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
                                   uint16_t packet_size, bool sync);
#endif
