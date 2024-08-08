/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  Header file containing struct prototypes of trace info.
 * Create:       2023-01
 */
#ifndef __TRACE_INFO_H__
#define __TRACE_INFO_H__

#include "errcode.h"
#include "diag.h"

#define CYCLE_PER_US 64
#define CYCLE_HIGH_BITS 32

typedef enum {
    TRACE_INIT,
    TRACE_START,
    TRACE_STOP,
} trace_status_t;

void trace_send_data(uint16_t len, uint8_t *data);
errcode_t diag_cmd_trace_start(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_cmd_trace_stop(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_cmd_get_time_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                 diag_option_t *option);
errcode_t diag_cmd_set_time_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

#endif
