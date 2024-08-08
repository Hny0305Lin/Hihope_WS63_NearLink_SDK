/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: lowpower debug private head file
 * Author: Huawei LiteOS Team
 * Create: 2021-11-12
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
#ifndef _LOS_LOWPOWER_DEBUG_PRI_H
#define _LOS_LOWPOWER_DEBUG_PRI_H

#include "los_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
* @ingroup  los_lowpower
*
* Sleep info Item
*/
typedef struct {
    UINT64 startTicks;
    UINT64 endTicks;
    UINT32 intNum;
    UINT32 cpuId;
    LosSleepMode sleepMode;
} PowerMgrInfoItem;

/**
 * @ingroup  los_lowpower
 *
 * Sleep info
 */
typedef struct {
    UINT64 startTicks;
    UINT64 endTicks;
    UINT32 intNum;    /* irq num is valid when mode is not deep sleep */
    UINT32 sleepFlag;
    LosSleepMode sleepMode;
} PowerMgrInfo;

extern PowerMgrInfo g_pmMgrInfo[LOSCFG_KERNEL_CORE_NUM];
extern UINT32 g_lowpowerFlag;
extern volatile BOOL g_lowpowerEnterFlag[LOSCFG_KERNEL_CORE_NUM];

extern UINT32 OsPowerMgrInfoInit(VOID);
extern VOID OsPowerMgrInfoPut(PowerMgrInfoItem *powerMgrInfo, UINT32 cpuid);
extern VOID OsPowerMgrInfoDbg(UINT32 cpuId);
extern VOID OsPowerMgrInfoTitle(VOID);
extern VOID OsSleepBlockCBInit(VOID);
extern VOID OsSwtmrTaskBlockInfoStat(VOID);
extern VOID OsSleepBlockInfoPrint(UINT32 cpuId);
extern VOID OsResetSleepBlockCB(UINT32 cpuId);
#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
extern VOID OsVetoBlockInfoStat(VOID);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // _LOS_LOWPOWER_DEBUG_PRI_H

