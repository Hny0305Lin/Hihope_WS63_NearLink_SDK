/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx trace
 * This file should be changed only infrequently and with great care.
 */

#ifndef DFX_TRACE_H
#define DFX_TRACE_H

#include "errcode.h"

errcode_t trace_pipeline_register(void);
errcode_t dfx_trace_start(void);
errcode_t dfx_trace_stop(void);

#endif