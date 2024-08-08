/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * File          trace_info.c
 * Description:  trace information interact with tool
 * Create:       2023/01
 * Version       V1.0
 */

#include <stdlib.h>
#include <time.h>
#include "errcode.h"
#include "securec.h"
#include "diag_common.h"
#include "dfx_trace.h"
#include "diag_cmd_trace_info.h"

static trace_status_t g_trace_status;
static diag_option_t g_option;
static uint16_t g_cmd_id;

void trace_send_data(uint16_t len, uint8_t *data)
{
    uapi_diag_report_packets_critical(g_cmd_id, &g_option, (uint8_t **)&data, (uint16_t *)&len, 1);
    return;
}

errcode_t diag_cmd_trace_start(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param);
    unused(cmd_param_size);
    diag_option_t *tmp_option = &g_option;
    g_cmd_id = cmd_id;
    memcpy_s(tmp_option, sizeof(diag_option_t), option, sizeof(diag_option_t));

    if (g_trace_status == TRACE_INIT) {
        if (trace_pipeline_register() != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
    }
    if (dfx_trace_start() != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    g_trace_status = TRACE_START;

    return ERRCODE_SUCC;
}

errcode_t diag_cmd_trace_stop(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);

    if (dfx_trace_stop() != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    g_trace_status = TRACE_STOP;

    return ERRCODE_SUCC;
}

errcode_t diag_cmd_get_time_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param);
    unused(cmd_param_size);
    uint8_t *data[1];
    uint16_t len[1];
    uint64_t cycle;
    uint8_t size = (uint8_t)sizeof(uint64_t);
#ifdef LOSCFG_TRACE_ADAPT
    uint32_t high;
    uint32_t low;
    LOS_GetCpuCycle(&high, &low);
    cycle = ((uint64_t)high << CYCLE_HIGH_BITS) + low;
    cycle = cycle / CYCLE_PER_US;
#else
    cycle = 0;
#endif
    data[0] = (uint8_t *)&cycle;
    len[0] = size;

    return uapi_diag_report_packets_critical(cmd_id, option, data, len, 1);
}

errcode_t diag_cmd_set_time_info(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param_size);
#if (!defined(UT_TEST) && !defined(FUZZ_TEST))
    uint8_t *data[1];
    uint16_t len[1];
    struct timespec tp;
    uint8_t size = (uint8_t)sizeof(struct timespec);
    uint32_t *time = cmd_param;

    tp.tv_sec = (long)time[0];
    tp.tv_nsec = (long)time[1];
    clock_settime(CLOCK_REALTIME, &tp);
    data[0] = (uint8_t *)&tp;
    len[0] = size;

    return uapi_diag_report_packets_critical(cmd_id, option, data, len, 1);
#else
    unused(cmd_id);
    unused(cmd_param);
    unused(option);
    return ERRCODE_SUCC;
#endif
}
