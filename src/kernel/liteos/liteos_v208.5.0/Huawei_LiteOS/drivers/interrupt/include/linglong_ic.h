/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LingLong Interrupt Controller Driver Header
 * Author: Huawei LiteOS Team
 * Create: 2021-11-30
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

#ifndef _LINGLONG_IC_H
#define _LINGLONG_IC_H

#include "los_typedef.h"
#include "arch/barrier.h"
#include "arch/regs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LLIC_INT_ID_NMI             0
#define LLIC_INTERNAL_PI_MIN        1
#define LLIC_INTERNAL_PI_MAX        31

#define LLIC_INTERNAL_SWI_MIN       32
#define LLIC_INTERNAL_SWI_MAX       63

#define LLIC_EXTERNAL_PI_MIN        64
#define LLIC_EXTERNAL_PI_MAX        127

#define LLIC_EXTERNAL_SI_MIN        64
#define LLIC_EXTERNAL_SI_MAX        191

#ifdef LOSCFG_KERNEL_SMP
#define LLIC_INT_ID_PRIV_MAX        LLIC_INTERNAL_SWI_MAX
#else
#define LLIC_INT_ID_PRIV_MAX        LLIC_EXTERNAL_PI_MAX
#endif

/************************** PRIV INT ID **************************/
#define LLIC_THREAD_SSP_OOR_INT_ID  1
#define LLIC_THREAD_TIMER0_INT_ID   14
#define LLIC_OCD_OV_INT_ID          22

/************************** ICU REGS **************************/

#define ICU_BASE                    (DSS_PRIVATE_REGS_BASE + 0x4000)

#define ICU_SET_BASE                (ICU_BASE + 0x00)
#define ICU_MASK_BASE               (ICU_BASE + 0x40)
#define ICU_STATMASK_BASE           (ICU_BASE + 0x80)
#define ICU_CLEAR_BASE              (ICU_BASE + 0xC0)
#define ICU_PRIOLEVEL_BASE          (ICU_BASE + 0x100)

#define ICU_SET(n)                  (ICU_SET_BASE + ((n) * 4))
#define ICU_MASK(n)                 (ICU_MASK_BASE + ((n) * 4))
#define ICU_STATMASK(n)             (ICU_STATMASK_BASE + ((n) * 4))
#define ICU_CLEAR(n)                (ICU_CLEAR_BASE + ((n) * 4))
#define ICU_PRIOLEVEL(n)            (ICU_PRIOLEVEL_BASE + ((n) * 4))

#define ICU_SET_N(id)               ((id) >> 5)
#define ICU_MASK_N(id)              ((id) >> 5)
#define ICU_STATMASK_N(id)          ((id) >> 5)
#define ICU_CLEAR_N(id)             ((id) >> 5)
#define ICU_PRIOLEVEL_N(id)         ((id) >> 2)

/************************** ICD REGS **************************/

#ifdef LOSCFG_KERNEL_SMP
#define ICD_BASE                    (MPSCU_MP_REG_BASE + 0x54000)

#define ICD_SET_BASE                (ICD_BASE + 0x00)
#define ICD_MASK_BASE               (ICD_BASE + 0x80)
#define ICD_STATMASK_BASE           (ICD_BASE + 0x100)
#define ICD_SW_INT_GEN_BASE         (ICD_BASE + 0x280)
#define ICD_LOCK_CLEAR_BASE         (ICD_BASE + 0x300)
#define ICD_PRIOLEVEL_BASE          (ICD_BASE + 0x1300)
#define ICD_SEMAPHORE_BASE          (ICD_BASE + 0x1700)
#define ICD_TARGET_BASE             (ICD_BASE + 0x1780)


#define ICD_SET(n)                  (ICD_SET_BASE + ((n) * 4))
#define ICD_MASK(n)                 (ICD_MASK_BASE + ((n) * 4))
#define ICD_STATMASK(n)             (ICD_STATMASK_BASE + ((n) * 4))
#define ICD_SW_INT_GEN              (ICD_SW_INT_GEN_BASE)
#define ICD_LOCK_CLEAR(n)           (ICD_LOCK_CLEAR_BASE + ((n) * 4))
#define ICD_PRIOLEVEL(n)            (ICD_PRIOLEVEL_BASE + ((n) * 4))
#define ICD_SEMAPHORE(n)            (ICD_SEMAPHORE_BASE + ((n) * 4))
#define ICD_TARGET(n)               (ICD_TARGET_BASE + ((n) * 4))

#define ICD_SET_N(id)               (((id) >> 5) - 2)
#define ICD_MASK_N(id)              (((id) >> 5) - 2)
#define ICD_STATMASK_N(id)          (((id) >> 5) - 2)
#define ICD_LOCK_CLEAR_N(id)        ((id) - 64)
#define ICD_PRIOLEVEL_N(id)         (((id) >> 2) - 16)
#define ICD_TARGET_N(id)            (((id) >> 1) - 32)

#endif

#define LLIC_REG_WIDTH              32
#define LLIC_PER_PRIO_WIDTH         8
#define LLIC_PER_PRIO_MASK          ((0x01U << LLIC_PER_PRIO_WIDTH) - 1)
#define LLIC_PRIO_NUM_PER_REG       (LLIC_REG_WIDTH / LLIC_PER_PRIO_WIDTH)

#define ICD_SW_GEN_TARGET_POS       16
#define ICD_TARGET_OFFSET(id)       (((id) % 2) * 16)
#define ICD_TARGET_MASK             0x0000000F
#define ICU_CLEAR_ALL_MASK          0xFFFFFFFF

#if (LOSCFG_HWI_PRIO_LIMIT > 15)
#error "LOSCFG_HWI_PRIO_LIMIT config error!"
#endif

#ifdef LOSCFG_KERNEL_SMP
#if (LOSCFG_PLATFORM_HWI_LIMIT > (LLIC_EXTERNAL_SI_MAX + 1))
#error "LOSCFG_PLATFORM_HWI_LIMIT config error!"
#endif
#else
#if (LOSCFG_PLATFORM_HWI_LIMIT > (LLIC_EXTERNAL_PI_MAX + 1))
#error "LOSCFG_PLATFORM_HWI_LIMIT config error!"
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
