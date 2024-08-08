/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: LiteOS Timer Header File
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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
/* ----------------------------------------------------------------------------
 * This file has been modified in accordance with Misra-C specifications.
 * ---------------------------------------------------------------------------- */

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OS_CYCLE_PER_TICK (GET_SYS_CLOCK() / (UINT32)LOSCFG_BASE_CORE_TICK_PER_SECOND)

#ifdef LOSCFG_KERNEL_LOWPOWER
#if defined(LOSCFG_ARM_GENERIC_TIMER) || defined(LOSCFG_ARM_SP804_TIMER)
#define TIMER_MAX_CYCLE     0xFFFFFFFF
#elif defined(LOSCFG_CORTEX_M_SYSTICK)
#define TIMER_MAX_CYCLE     0x00FFFFFF
#endif

#define TIMER_MAX_TICK      (TIMER_MAX_CYCLE / OS_CYCLE_PER_TICK)
#endif

#ifndef NOP
#define NOP()       __asm__ volatile ("nop")
#endif

extern UINT64 hi_sched_clock(VOID);
extern VOID HalClockFreqWrite(UINT32 freq);
extern VOID HalClockStart(VOID);
extern VOID HalClockInitStart(VOID);
extern VOID HalClockEnable(VOID);
extern VOID HalClockDisable(VOID);
extern VOID HalClockInit(VOID);

extern UINT64 HalClockGetCycles(VOID);
extern UINT64 HalClockGetCurrCycles(VOID);
extern VOID HalDelayUs(UINT32 usecs);
extern UINT32 HalClockGetTickTimerCycles(VOID);
extern VOID HalClockTickTimerReload(UINT32 cycles);
extern UINT32 HalGetRemainingCycles(UINT32 *ticks);
extern UINT32 HalTickAdjust(UINT32 ticks);
extern VOID HalTickReload(VOID);

extern UINT32 HrtimersInit(VOID);
extern VOID HrtimerClockIrqClear(VOID);
extern VOID HrtimerClockStart(UINT32 period);
extern VOID HrtimerClockStop(VOID);
extern UINT32 HrtimerClockValueGet(VOID);
extern VOID HrtimerClockInit(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_TIMER_H */
