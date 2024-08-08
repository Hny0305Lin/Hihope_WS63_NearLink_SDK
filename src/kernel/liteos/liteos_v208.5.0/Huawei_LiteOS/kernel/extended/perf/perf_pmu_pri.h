/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: LiteOS Perf Pmu Manager Implementation Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2020-07-29
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef _PERF_PMU_PRI_H
#define _PERF_PMU_PRI_H

#include "los_perf_pri.h"

#ifdef LOSCFG_COMPAT_LINUX_HRTIMER
#include "linux/hrtimer.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    Pmu pmu;
    BOOL canDivided;
    UINT32 cntDivided;
    VOID (*enable)(Event *event);
    VOID (*disable)(Event *event);
    VOID (*start)(VOID);
    VOID (*stop)(VOID);
    VOID (*clear)(VOID);
    VOID (*setPeriod)(Event *event);
    UINTPTR (*readCnt)(Event *event);
    UINT32 (*mapEvent)(UINT32 eventType, UINT32 *result, UINT32 len, BOOL reverse);
} HwPmu;

typedef struct {
    Pmu pmu;
    union {
        struct { /* trace event */
            BOOL enable[LOSCFG_KERNEL_CORE_NUM];
        };
#ifdef LOSCFG_COMPAT_LINUX_HRTIMER
        struct { /* timer event */
            struct hrtimer hrtimer;
            union ktime time;
            union ktime cfgTime;
        };
#endif
    };
} SwPmu;

#define GET_HW_PMU(item)                    LOS_DL_LIST_ENTRY(item, HwPmu, pmu)

#define TIMER_PERIOD_LOWER_BOUND_US         100

#define PERF_HW_INVALID_EVENT_TYPE          0xFFFFFFFF

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define PMU_LABEL_INT_1                \
    NUM_HAL_INTERRUPT_PMU_0,
#define PMU_LABEL_INT_2                \
    PMU_LABEL_INT_1                    \
    NUM_HAL_INTERRUPT_PMU_1,
#define PMU_LABEL_INT_3                \
    PMU_LABEL_INT_2                    \
    NUM_HAL_INTERRUPT_PMU_2,
#define PMU_LABEL_INT_4                \
    PMU_LABEL_INT_3                    \
    NUM_HAL_INTERRUPT_PMU_3,

#define PMU_INT(num)    PMU_LABEL_INT_##num

#define OS_PMU_INTS(num, pmuInts)       \
    STATIC const UINT32 pmuInts[num] = {      \
        PMU_INT(num)                    \
    }

extern UINT32 OsPerfPmuRegister(Pmu *pmu);
extern VOID OsPerfPmuRm(UINT32 type);
extern Pmu *OsPerfPmuGet(UINT32 type);

extern UINT32 OsHwPmuInit(VOID);
extern VOID OsHwPmuDeinit(VOID);
extern UINT32 OsSwPmuInit(VOID);
extern VOID OsSwPmuDeinit(VOID);
extern UINT32 OsTimedPmuInit(VOID);
extern VOID OsTimedPmuDeinit(VOID);

extern UINT32 OsGetPmuCounter0(VOID);
extern UINT32 OsGetPmuMaxCounter(VOID);
extern UINT32 OsGetPmuCycleCounter(VOID);
extern UINT32 OsPerfHwInit(HwPmu *hwPmu);
extern VOID OsPerfHwDeinit(HwPmu *hwPmu);
extern VOID OsPerfHwPmuHandleOverflow(Event *event, PerfRegs *regs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PERF_PMU_PRI_H */
