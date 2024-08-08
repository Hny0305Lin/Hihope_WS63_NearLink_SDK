/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Low-power Framework.
 * Author: Huawei LiteOS Team
 * Create: 2020-09-19
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

#ifndef _LOS_LOWPOWER_IMPL_PRI_H
#define _LOS_LOWPOWER_IMPL_PRI_H

#include "los_spinlock.h"
#include "los_task.h"
#include "los_hwi.h"
#include "los_lowpower.h"

#ifdef LOSCFG_KERNEL_RUNSTOP
#include "los_runstop_pri.h"
#endif

#ifdef LOSCFG_KERNEL_TICKLESS
#include "los_tickless_pri.h"
#endif

#ifdef LOSCFG_KERNEL_DEEPSLEEP
#include "los_deepsleep_pri.h"
#endif

#ifdef LOSCFG_KERNEL_RAM_SAVE
extern VOID OsContextRestore(VOID);
#endif

#ifdef LOSCFG_KERNEL_DEEPSLEEP
extern VOID OsTickHandler(VOID);
#endif

#ifdef LOSCFG_KERNEL_LOWPOWER
#define CALL_FUNC_NO_RETURN(ops, func, ...) do { \
        if (((ops).func != NULL)) {              \
            (ops).func(__VA_ARGS__);             \
        }                                        \
    } while (0)

#define CALL_FUNC_WITH_RETURN(ops, ret, func, ...) do { \
        if (((ops).func != NULL)) {                     \
            (ret) = (ops).func(__VA_ARGS__);            \
        }                                               \
    } while (0)
#else
#define CALL_FUNC_NO_RETURN(ops, func, ...)
#define CALL_FUNC_WITH_RETURN(ops, ret, func, ...)

#endif

#define PANIC_TICK_ADJUST_RET(ret, retcode) do { \
        if ((ret) != (retcode)) {                 \
            LOS_PANIC("[%s, %d] tick adjust fail, max sleep time too big\n", __FUNCTION__, __LINE__); \
        }                                         \
    } while (0)

#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
#define MAX_SLEEP_VETO_SRC  32
#endif

typedef struct {
#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
    SPIN_LOCK_S requestSleepLock;
    UINT32 requestSleepMode[LOS_SLEEP_MAX];
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
    UINT32 requestSleepTbl[LOS_SLEEP_MAX][MAX_SLEEP_VETO_SRC];
#endif
#endif
    UINT32 maxSleepTicks;
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
    UINT32 minLightSleepTicks;
#endif
#ifdef LOSCFG_KERNEL_DEEPSLEEP
    UINT32 minDeepSleepTicks;
    UINT32 extTimerFreq;
    UINT32 deltaTicks;
#endif
#if defined(LOSCFG_LOWPOWER_SLEEP_DEBUG) || defined(LOSCFG_KERNEL_DEEPSLEEP)
    UINT32 sleepTime[LOSCFG_KERNEL_CORE_NUM];
#endif
#if defined(LOSCFG_KERNEL_SMP) || defined(LOSCFG_KERNEL_DEEPSLEEP)
    Atomic deepSleepCores;
    Atomic resumeSleepCores;
#endif
#ifdef LOSCFG_KERNEL_DYN_FREQ
    Atomic freq;
    Atomic freqPending;
    Atomic freeLock;
#endif
} PowerMgr;
extern PowerMgr g_pmMgr;


#ifdef LOSCFG_KERNEL_DEEPSLEEP
extern VOID OsLowpowerWakeupFromReset(VOID);
#endif

#if defined (LOSCFG_KERNEL_DEEPSLEEP) || defined (LOSCFG_KERNEL_TICKLESS)
extern VOID OsSysTimeUpdate(UINT32 sleepTicks);
#endif

#ifdef LOSCFG_KERNEL_DYN_FREQ
STATIC INLINE BOOL FreqHigher(LosFreqMode freq1, LosFreqMode freq2)
{
    return freq1 < freq2;
}
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // _LOS_LOWPOWER_IMPL_PRI_H
