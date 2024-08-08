/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides at msg service header. Only for AT module. \n
 */

#ifndef    AT_MSG_H
#define    AT_MSG_H

#include "at_product.h"

typedef enum {
    AT_CMD_MSG                 = 0x0,
    AT_CMD_RESULT_MSG          = 0x1,
    AT_CMD_INTERACTIVITY_MSG   = 0x2,
    AT_CMD_TIMEOUT_MSG         = 0x3,
    AT_CMD_URC_REPORT_MSG      = 0x4
} at_msg_type_t;

typedef struct {
    uint16_t                     channel_id;
    uint16_t                     len;
} at_cmd_msg_t;

typedef struct {
    uint16_t                     err_code;
    uint16_t                     cmd_id;
} at_result_msg_t;

typedef struct {
    uint16_t                     urc_type;
} at_urc_report_msg_t;

typedef struct {
    uint16_t                     type;
    uint16_t                     cmd_id;
} at_timeout_msg_t;

typedef struct {
    at_msg_type_t                type;
    union {
        at_cmd_msg_t             cmd;
        at_result_msg_t          result;
        at_urc_report_msg_t      urc;
        at_timeout_msg_t         timeout;
        at_cmd_msg_t             interactivity;
    } sub_msg;
} at_msg_block_t;

errcode_t at_msg_send(at_msg_block_t *msg);

void at_msg_process(at_msg_block_t *msg);
#endif
