/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ARCH cpu trace interface for project
 *
 * Create:  2021-10-25
 */
#ifndef ARCH_TRACE_H
#define ARCH_TRACE_H

#include <stdint.h>
#include "platform_core.h"
#ifdef SUPPORT_CPU_TRACE
#include "cpu_trace.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define  RISCV_TRACE_MEM_REGION_START   TRACE_MEM_REGION_START
#define  RISCV_TRACE_MEM_REGION_LENGTH  TRACE_MEM_REGION_LENGTH

#ifdef SUPPORT_CPU_TRACE
typedef cpu_trace_item_t  riscv_cpu_trace_item_t;
#endif

/**
 * @brief  Disable cpu trace for riscv.
 */
static inline void riscv_cpu_trace_disable(void)
{
#ifdef SUPPORT_CPU_TRACE
    cpu_trace_disable();
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
