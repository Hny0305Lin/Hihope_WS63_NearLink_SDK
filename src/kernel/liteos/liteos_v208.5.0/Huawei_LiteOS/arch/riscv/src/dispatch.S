/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: LiteOS task schedule module implementation.
 * Author: Huawei LiteOS Team
 * Create: 2022-12-20
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

#include "arch/stack_ops.h"

.global  ArchIntLock
.global  ArchIntUnlock
.global  ArchIntRestore
.global  ArchStartToRun
.global  ArchTaskSchedule
.global  OsDisableIRQ

/*
 * a0: new task
 * a1: run task
 */
.section .itcm.text
ArchTaskSchedule:
    li      t0, LOS_MSTATUS_MIE | LOS_MSTATUS_MPIE
    csrrc   t1, mstatus, t0
    addi    sp, sp, -INT_SIZE_ON_STACK
    andi    t1, t1, LOS_MSTATUS_MIE
    ori     t1, t1, 0x180
    slli    t1, t1, 0x4
    csrs    mstatus, t1
    csrw    mepc, ra

SaveRunTask:
    PUSH_CALLEE_REG
#ifdef LOSCFG_ARCH_FPU_ENABLE
    addi   t0, sp, FCSR_STACK_OFFSET
    frcsr  t2
    SREG   t2,  0 * REGBYTES(t0)
#endif
    csrr    t0, mstatus
    SREG    t0, 0 * REGBYTES(sp)
    csrr    t0, mepc
    SREG    t0, 1 * REGBYTES(sp)
#ifdef LOSCFG_LCMP_CUSTOM_INST16_EXTENSIONS
    li      t0, LOS_MCAUSE_UOP
    csrrc   zero, mcause, t0
#endif

    sw      sp, TASK_CB_KERNEL_SP(a1)

/* a0: new task */
SwitchNewTask:
    /* retireve stack pointer */
    lw      sp, TASK_CB_KERNEL_SP(a0)

    /* retrieve the registers */
    POP_ALL_REG

    mret

/*
 * a0: new task
 */
.section .text
ArchStartToRun:
    /* disable interrupts */
    li      t0, LOS_MSTATUS_MIE
    csrrc   zero, mstatus, t0

    /* a0 is new task, save it on g_newTask */
    la      t1, g_newTask
    sw      a0, 0(t1)

    tail    SwitchNewTask

OsDisableIRQ:
    li      t0, (LOS_MSTATUS_MPIE | LOS_MSTATUS_MIE) /* mpie | mie */
    csrrc   zero, mstatus, t0
    ret

ArchIntLock:
    csrr    a0, mstatus           /* return value */
    li      t0, LOS_MSTATUS_MIE   /* mie */
    csrrc   zero, mstatus, t0
    ret

ArchIntUnlock:
    csrr    a0, mstatus           /* return value */
    li      t0, LOS_MSTATUS_MIE   /* mie */
    csrrs   zero, mstatus, t0
    ret

ArchIntRestore:
    csrw mstatus, a0
    ret