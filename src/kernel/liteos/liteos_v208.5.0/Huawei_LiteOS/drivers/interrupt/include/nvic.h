/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: NVIC(Nested Vectored Interrupt Controller) for Cortex-M.
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

/**
 * @defgroup los_hwi Hardware interrupt
 * @ingroup kernel
 */
#ifndef NVIC_H
#define NVIC_H

#include "los_hwi.h"
#include "asm/platform.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_ARCH_CORTEX_M3
#define CORTEX_M_VERSION  3U
#elif defined(LOSCFG_ARCH_CORTEX_M4)
#define CORTEX_M_VERSION  4U
#elif defined(LOSCFG_ARCH_CORTEX_M7)
#define CORTEX_M_VERSION  7U
#elif defined(LOSCFG_ARCH_CORTEX_M55)
#define CORTEX_M_VERSION  55U
#else
#error "Unsupported Arch"
#endif

#if defined(LOSCFG_ARCH_CORTEX_M3) || defined(LOSCFG_ARCH_CORTEX_M4) || \
    defined(LOSCFG_ARCH_CORTEX_M7) || defined(LOSCFG_ARCH_CORTEX_M55)
#ifndef __NVIC_PRIO_BITS
#define __NVIC_PRIO_BITS            3U   /* Compatible With Cmsis */
#endif

/* (irqNum / 32) * sizeof(int) */
#define IRQ2OFFSET(irqNum)          (((irqNum) >> 5U) << 2U)
#define NVIC_PRIO_MASK              0xFFU
#define NVIC_PRIO_BITS              8U
#define NVIC_CALC_PRIOVALUE(prio)   (((prio) << (NVIC_PRIO_BITS - (UINT8)__NVIC_PRIO_BITS)) & NVIC_PRIO_MASK)

/* Interrupt Set-enable Registers */
#define NVIC_ISERx(irqNum)          (0xE000E100U + IRQ2OFFSET(irqNum))

/* Interrupt Priority Registers */
#define NVIC_IPRx(irqNum)           (0xE000E400U + (irqNum))

/* Interrupt Clear-enable Registers */
#define NVIC_ICERx(irqNum)          (0xE000E180U + IRQ2OFFSET(irqNum))

/* Interrupt Set-pending Registers */
#define NVIC_ISPRx(irqNum)          (0xE000E200U + IRQ2OFFSET(irqNum))

/* Interrupt Clear-pending Registers */
#define NVIC_ICPRx(irqNum)          (0xE000E280U + IRQ2OFFSET(irqNum))

/* Interrupt Control and State Register */
#define SCB_ICSR                    0xE000ED04U

/* Vector Table Offset Register */
#define SCB_VTOR                    0xE000ED08U

/*
 * Application Interrupt and Reset Control Register
 * | 31 - 16 | 15 | 14 - 11 | 10 - 8 | 7 - 3 | 2 | 1 | 0 |
 *  VECTKEY          RES      PRIGROUP  RES
 */
#define SCB_AIRCR                   0xE000ED0CU
#define SCB_AIRCR_VECTKEY_IDX       16U
#define SCB_AIRCR_PRIGROUP_IDX      8U
#define SCB_AIRCR_VECTKEY_MASK      ((UINT32)0xFFFF << SCB_AIRCR_VECTKEY_IDX)
#define SCB_AIRCR_PRIGROUP_MASK     ((UINT32)0x7 << SCB_AIRCR_PRIGROUP_IDX)
#define SCB_AIRCR_VECTKEY_KEY       (UINT32)0x5FA

/* System Handler Priority Register. Begin Of Irq 4 */
#define SCB_SHPRx(irqNum)           (0xE000ED18U + (irqNum) - 4U)
#endif

/**
 * @ingroup los_hwi
 * Highest priority of a hardware interrupt.
 */
#ifndef OS_HWI_PRIO_HIGHEST
#define OS_HWI_PRIO_HIGHEST         0
#endif

/**
 * @ingroup los_hwi
 * Lowest priority of a hardware interrupt.
 */
#ifndef OS_HWI_PRIO_LOWEST
#define OS_HWI_PRIO_LOWEST          7
#endif

/**
 * @ingroup los_hwi
 * Count of M-Core system interrupt vector.
 */
#define OS_SYS_VECTOR_CNT           16

/**
 * @ingroup los_hwi
 * AIRCR register priority group parameter .
 */
#define OS_NVIC_AIRCR_PRIGROUP      7

/**
 * @ingroup los_hwi
 * SysTick control and status register.
 */
#define OS_SYSTICK_CONTROL_REG      0xE000E010

/**
 * @ingroup los_hw
 * SysTick current value register.
 */
#define OS_SYSTICK_CURRENT_REG      0xE000E018

/**
 * @ingroup los_hwi
 * Interrupt Priority-Level Registers.
 */
#define OS_NVIC_PRI_BASE            0xE000E400

/**
 * @ingroup los_hwi
 * Interrupt enable register for 0-31.
 */
#define OS_NVIC_SETENA_BASE         0xE000E100

/**
 * @ingroup los_hwi
 * interrupt pending register.
 */
#define OS_NVIC_SETPEND_BASE        0xE000E200

/**
 * @ingroup los_hwi
 * interrupt activity status register
 */
#define OS_NVIC_INT_ACT_BASE        0xE000E300

/**
 * @ingroup los_hwi
 * Interrupt disable register for 0-31.
 */
#define OS_NVIC_CLRENA_BASE         0xE000E180

/**
 * @ingroup los_hwi
 * Interrupt control and status register.
 */
#define OS_NVIC_INT_CTRL            0xE000ED04

/**
 * @ingroup los_hwi
 * Vector table offset register.
 */
#define OS_NVIC_VTOR                0xE000ED08

/**
 * @ingroup los_hwi
 * Application interrupt and reset control register
 */
#define OS_NVIC_AIRCR               0xE000ED0C

/**
 * @ingroup los_hwi
 * System exception priority register.
 */
#define OS_NVIC_EXCPRI_BASE         0xE000ED18

/**
 * @ingroup los_hwi
 * Interrupt No. 1 :reset.
 */
#define OS_EXC_RESET                1

/**
 * @ingroup los_hwi
 * Interrupt No. 2 :Non-Maskable Interrupt.
 */
#define OS_EXC_NMI                  2

/**
 * @ingroup los_hwi
 * Interrupt No. 3 :(hard)fault.
 */
#define OS_EXC_HARD_FAULT           3

/**
 * @ingroup los_hwi
 * Interrupt No. 4 :MemManage fault.
 */
#define OS_EXC_MPU_FAULT            4

/**
 * @ingroup los_hwi
 * Interrupt No. 5 :Bus fault.
 */
#define OS_EXC_BUS_FAULT            5

/**
 * @ingroup los_hwi
 * Interrupt No. 6 :Usage fault.
 */
#define OS_EXC_USAGE_FAULT          6

/**
 * @ingroup los_hwi
 * Interrupt No. 11 :SVCall.
 */
#define OS_EXC_SVC_CALL             11

/**
 * @ingroup los_hwi
 * Interrupt No. 12 :Debug monitor.
 */
#define OS_EXC_DBG_MONITOR          12

/**
 * @ingroup los_hwi
 * Interrupt No. 14 :PendSV.
 */
#define OS_EXC_PEND_SV              14

/* Reset handle entry */
extern VOID Reset_Handler(VOID);

extern VOID OsPendSV(VOID);

/* hardware interrupt entry */
extern VOID IrqEntryV7M(VOID);

#ifdef LOSCFG_ARCH_INTERRUPT_TAKEOVER
extern HWI_PROC_FUNC g_hwiVec[LOSCFG_PLATFORM_HWI_LIMIT];
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NVIC_H */
