/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Hwi Implementation HeadFile
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

#ifndef HAL_HWI_H
#define HAL_HWI_H

#include "los_typedef.h"

#if defined(LOSCFG_LINGLONG_IC)
#include "linglong_ic.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(LOSCFG_CORTEX_M_NVIC)
#define OS_USER_HWI_MIN                 16
#elif defined(LOSCFG_ARCH_LINGLONG)
#define OS_USER_HWI_MIN                 1  /* 0: NMI */
#else
#define OS_USER_HWI_MIN                 0
#endif

#if defined(LOSCFG_LINGLONG_IC)
#define OS_HWI_IPI_BASE                 LLIC_INTERNAL_SWI_MIN
#else
#define OS_HWI_IPI_BASE                 0
#endif

#if (OS_USER_HWI_MIN == 0)
#define HWI_NUM_GE_MIN(num)             TRUE
#else
#define HWI_NUM_GE_MIN(num)             ((num) >= (UINT32)OS_USER_HWI_MIN)
#endif

#define OS_USER_HWI_MAX                 (LOSCFG_PLATFORM_HWI_LIMIT - 1)
#ifdef LOSCFG_ARM_GIC_LPI_ENABLE
#define HWI_NUM_VALID(num)              ((HWI_NUM_GE_MIN(num) && ((num) < (UINT32)OS_HWI_SPI_MAX)) || \
                                         (((num) >= (UINT32)OS_HWI_LPI_MIN) && ((num) <= (UINT32)OS_USER_HWI_MAX)))
#else
#define HWI_NUM_VALID(num)              (HWI_NUM_GE_MIN(num) && ((num) <= (UINT32)OS_USER_HWI_MAX))
#endif
extern VOID HalIrqInit(VOID);
extern VOID HalIrqInitPercpu(VOID);
extern UINT32 HalIrqMask(UINT32 hwiNum);
extern UINT32 HalIrqUnmask(UINT32 hwiNum);
extern UINT32 HalIrqTrigger(UINT32 hwiNum);
extern UINT32 HalIrqClear(UINT32 hwiNum);
extern CHAR *HalIrqVersion(VOID);
extern UINT32 HalCurIrqGet(VOID);
extern UINT32 HalIrqSetPrio(UINT32 hwiNum, UINT16 priority);
extern UINT32 HalIrqPendingGet(UINT32 hwiNum, UINT8 *isPending);
extern UINT32 HalIrqSetAffinity(UINT32 hwiNum, UINT32 cpuMask);
#ifdef LOSCFG_KERNEL_SMP
extern UINT32 HalIrqSendIpi(UINT32 target, UINT32 ipi);
#endif
#ifdef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
extern VOID HalIrqHandler(UINT32 hwiNum);
#endif
#ifdef LOSCFG_RISCV_LCMP_CLIC
extern UINT32 HalIrqSetClicIntAttr(UINT32 hwiNum, UINT8 clicintattr);
#endif

typedef VOID (*GIC_PROC_FUNC)(VOID);
GIC_PROC_FUNC HalGetGicdHook(VOID);
VOID HalSetGicdHook(GIC_PROC_FUNC gicdHook);

#ifdef LOSCFG_ARM_GIC_LPI_ENABLE
GIC_PROC_FUNC HalGetGicLpiHook(VOID);
VOID HalSetGicLpiHook(GIC_PROC_FUNC gicLpiHook);
#endif
/*
 * NOTE: HalIrqPending() is DEPRECATED and will be deleted in the future.
 * Please use HalIrqTrigger() or LOS_HwiTrigger() instead.
 */
#define HalIrqPending                    HalIrqTrigger

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HAL_HWI_H */
