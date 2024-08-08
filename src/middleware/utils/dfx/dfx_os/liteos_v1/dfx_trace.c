/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx trace
 * This file should be changed only infrequently and with great care.
 */

#include "errcode.h"
#include "los_trace.h"
#include "diag_cmd_trace_info.h"
#include "dfx_trace.h"

#ifdef LOSCFG_TRACE_CLIENT_INTERACT
typedef struct {
    UINT32 (*init)(VOID);
    VOID (*dataSend)(UINT16 len, UINT8 *data);
    UINT32 (*dataRecv)(UINT8 *data, UINT32 size, UINT32 timeout);
    UINT32 (*wait)(VOID);
} TracePipelineOps;

static const TracePipelineOps g_uart_ops = {
    .init = NULL,
    .dataSend = trace_send_data,
    .dataRecv = NULL,
    .wait = NULL,
};
#endif

errcode_t trace_pipeline_register(void)
{
#ifdef LOSCFG_TRACE_CLIENT_INTERACT
    OsTracePipelineReg(&g_uart_ops);
#endif
    return ERRCODE_SUCC;
}

errcode_t dfx_trace_start(void)
{
    LOS_TraceEventMaskSet(TASK_SWITCH);
    if (LOS_TraceStart() != LOS_OK) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t dfx_trace_stop(void)
{
    LOS_TraceStop();
    return ERRCODE_SUCC;
}