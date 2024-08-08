/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: pkt router
 * This file should be changed only infrequently and with great care.
 */

#ifndef __ZDIAG_PKT_ROUTER_H__
#define __ZDIAG_PKT_ROUTER_H__

#include "errcode.h"
#include "diag_pkt.h"
#include "diag_channel.h"

typedef struct {
    diag_pkt_proc_t cur_proc;
} diag_pkt_process_param_t;

typedef struct {
    uint8_t cur_proc; /* diag_pkt_process_param_t */
    diag_pkt_handle_t pkt;
} diag_pkt_msg_t;

errcode_t diag_pkt_router(diag_pkt_handle_t *pkt, diag_pkt_process_param_t *process_param);
void diag_pkt_msg_proc(uint32_t msg_id, uint8_t *msg, uint32_t msg_len);
#endif
