/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: Riscv Assembly Defines and Macros HeadFile
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

#ifndef _ARCH_STACK_OPS_H
#define _ARCH_STACK_OPS_H

#include "arch/cpu.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_LIB_CONFIGURABLE
.extern g_irqStackTop
#else
.extern __irq_stack_top
#endif

.extern TrapHandle
.extern OsIntEntry
.extern OsSchedProcSchedFlag

/* task TCB offset */
#define TASK_CB_KERNEL_SP       0x0
#define TASK_CB_STATUS          0x4
#define EXC_SIZE_ON_STACK       (INT_SIZE_ON_STACK + EXC_SPECIAL_REG_SIZE)

.equ        REG_CCAUSE_HARD_FAULT_MASK, 0x40000000
.equ        REG_CCAUSE_LOCK_UP_MASK,    0x80000000
.equ        MCAUSE_MASK_INT_BIT,        0x80000000

#ifdef LOSCFG_ARCH_LINX_M
.equ        MCAUSE_MASK_INT_NUM,        0x00000FFF /* bit 0-11 represent exception/interrupt code */
#else
.equ        MCAUSE_MASK_INT_NUM,        0x7FFFFFFF
#endif

.macro  PUSH_FPU_CALLER_REG
    FSREG  ft11,  0 * FPUREGBYTES(sp)
    FSREG  ft10,  1 * FPUREGBYTES(sp)
    FSREG  ft9,   2 * FPUREGBYTES(sp)
    FSREG  ft8,   3 * FPUREGBYTES(sp)
    FSREG  fa7,   4 * FPUREGBYTES(sp)
    FSREG  fa6,   5 * FPUREGBYTES(sp)
    FSREG  fa5,   6 * FPUREGBYTES(sp)
    FSREG  fa4,   7 * FPUREGBYTES(sp)
    FSREG  fa3,   8 * FPUREGBYTES(sp)
    FSREG  fa2,   9 * FPUREGBYTES(sp)
    FSREG  fa1,  10 * FPUREGBYTES(sp)
    FSREG  fa0,  11 * FPUREGBYTES(sp)
    FSREG  ft7,  12 * FPUREGBYTES(sp)
    FSREG  ft6,  13 * FPUREGBYTES(sp)
    FSREG  ft5,  14 * FPUREGBYTES(sp)
    FSREG  ft4,  15 * FPUREGBYTES(sp)
    FSREG  ft3,  16 * FPUREGBYTES(sp)
    FSREG  ft2,  17 * FPUREGBYTES(sp)
    FSREG  ft1,  18 * FPUREGBYTES(sp)
    FSREG  ft0,  19 * FPUREGBYTES(sp)
.endm

.macro PUSH_INTEGER_CALLER_REG
    SREG   t6,    0 * REGBYTES(sp)
    SREG   t5,    1 * REGBYTES(sp)
    SREG   t4,    2 * REGBYTES(sp)
    SREG   t3,    3 * REGBYTES(sp)
    SREG   a7,    4 * REGBYTES(sp)
    SREG   a6,    5 * REGBYTES(sp)
    SREG   a5,    6 * REGBYTES(sp)
    SREG   a4,    7 * REGBYTES(sp)
    SREG   a3,    8 * REGBYTES(sp)
    SREG   a2,    9 * REGBYTES(sp)
    SREG   a1,   10 * REGBYTES(sp)
    SREG   a0,   11 * REGBYTES(sp)
    SREG   t2,   12 * REGBYTES(sp)
    SREG   t1,   13 * REGBYTES(sp)
    SREG   t0,   14 * REGBYTES(sp)
    SREG   ra,   15 * REGBYTES(sp)
.endm

.macro PUSH_CALLER_REG
#ifdef  LOS_COMPILE_LDM
#ifdef LOSCFG_ARCH_FPU_ENABLE
    addi   sp, sp, -FPU_CALLER_REG_SIZE
    PUSH_FPU_CALLER_REG
    addi   sp, sp, -(FPU_CALLEE_REG_SIZE + INTEGER_CALLER_REG_SIZE)
    stmia {ra, t0 - t2, a0 - a7, t3 - t6}, (sp)
    addi   sp, sp, -((INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES)
#else
    addi  sp, sp, -INTEGER_CALLER_REG_SIZE
    stmia {ra, t0 - t2, a0 - a7, t3 - t6}, (sp)
    addi  sp, sp, -((INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES)
#endif
#else
#ifdef LOSCFG_ARCH_FPU_ENABLE
    addi  sp, sp, -FPU_CALLER_REG_SIZE
    PUSH_FPU_CALLER_REG
    addi  sp, sp, -(FPU_CALLEE_REG_SIZE + INTEGER_CALLER_REG_SIZE)
    PUSH_INTEGER_CALLER_REG
    addi  sp, sp, -((INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES)
#else
    addi  sp, sp, -INTEGER_CALLER_REG_SIZE
    PUSH_INTEGER_CALLER_REG
    addi  sp, sp, -((INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES)
#endif
#endif
.endm

.macro POP_FPU_CALLER_REG
    FLREG  ft11,  0 * FPUREGBYTES(sp)
    FLREG  ft10,  1 * FPUREGBYTES(sp)
    FLREG  ft9,   2 * FPUREGBYTES(sp)
    FLREG  ft8,   3 * FPUREGBYTES(sp)
    FLREG  fa7,   4 * FPUREGBYTES(sp)
    FLREG  fa6,   5 * FPUREGBYTES(sp)
    FLREG  fa5,   6 * FPUREGBYTES(sp)
    FLREG  fa4,   7 * FPUREGBYTES(sp)
    FLREG  fa3,   8 * FPUREGBYTES(sp)
    FLREG  fa2,   9 * FPUREGBYTES(sp)
    FLREG  fa1,  10 * FPUREGBYTES(sp)
    FLREG  fa0,  11 * FPUREGBYTES(sp)
    FLREG  ft7,  12 * FPUREGBYTES(sp)
    FLREG  ft6,  13 * FPUREGBYTES(sp)
    FLREG  ft5,  14 * FPUREGBYTES(sp)
    FLREG  ft4,  15 * FPUREGBYTES(sp)
    FLREG  ft3,  16 * FPUREGBYTES(sp)
    FLREG  ft2,  17 * FPUREGBYTES(sp)
    FLREG  ft1,  18 * FPUREGBYTES(sp)
    FLREG  ft0,  19 * FPUREGBYTES(sp)
.endm

.macro POP_INTEGER_CALLER_REG
    LREG   t6,    0 * REGBYTES(sp)
    LREG   t5,    1 * REGBYTES(sp)
    LREG   t4,    2 * REGBYTES(sp)
    LREG   t3,    3 * REGBYTES(sp)
    LREG   a7,    4 * REGBYTES(sp)
    LREG   a6,    5 * REGBYTES(sp)
    LREG   a5,    6 * REGBYTES(sp)
    LREG   a4,    7 * REGBYTES(sp)
    LREG   a3,    8 * REGBYTES(sp)
    LREG   a2,    9 * REGBYTES(sp)
    LREG   a1,   10 * REGBYTES(sp)
    LREG   a0,   11 * REGBYTES(sp)
    LREG   t2,   12 * REGBYTES(sp)
    LREG   t1,   13 * REGBYTES(sp)
    LREG   t0,   14 * REGBYTES(sp)
    LREG   ra,   15 * REGBYTES(sp)
.endm

.macro POP_CALLER_REG
#ifdef  LOS_COMPILE_LDM
#ifdef LOSCFG_ARCH_FPU_ENABLE
    addi  sp, sp, (INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES
    ldmia {ra, t0 - t2, a0 - a7, t3 - t6}, (sp)
    addi  sp, sp, (FPU_CALLEE_REG_SIZE + INTEGER_CALLER_REG_SIZE)
    POP_FPU_CALLER_REG
    addi  sp, sp, (FPU_CALLER_REG_SIZE + FPU_STATUS_REG_SIZE)
#else
    addi  sp, sp, (INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES
    ldmia {ra, t0 - t2, a0 - a7, t3 - t6}, (sp)
    addi  sp, sp, INTEGER_CALLER_REG_SIZE
#endif
#else
#ifdef LOSCFG_ARCH_FPU_ENABLE
    addi  sp, sp, (INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES
    POP_INTEGER_CALLER_REG
    addi  sp, sp, (FPU_CALLEE_REG_SIZE + INTEGER_CALLER_REG_SIZE)
    POP_FPU_CALLER_REG
    addi  sp, sp, (FPU_CALLER_REG_SIZE + FPU_STATUS_REG_SIZE)
#else
    addi  sp, sp, (INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES
    POP_INTEGER_CALLER_REG
    addi  sp, sp, INTEGER_CALLER_REG_SIZE
#endif
#endif
.endm

.macro POP_IRQ_REG
    lw      t0, 1 * REGBYTES(sp)  /* retrieve the address at which exception happened */
    csrw    mepc, t0              /* load mepc from previously reserved (4 * REGBYTES) space */
    lw      t0, 0 * REGBYTES(sp)  /* enable global interrupts */
#ifdef LOSCFG_ARCH_LINX_M
    csrw    mcause, t0            /* load mcause from previously reserved (4 * REGBYTES) space */
#else
    csrw    mstatus, t0           /* load mstatus from previously reserved (4 * REGBYTES) space */
#endif
    POP_CALLER_REG
.endm

.macro  PUSH_FPU_CALLEE_REG
    FSREG  fs11, 0 * FPUREGBYTES(t0)
    FSREG  fs10, 1 * FPUREGBYTES(t0)
    FSREG  fs9,  2 * FPUREGBYTES(t0)
    FSREG  fs8,  3 * FPUREGBYTES(t0)
    FSREG  fs7,  4 * FPUREGBYTES(t0)
    FSREG  fs6,  5 * FPUREGBYTES(t0)
    FSREG  fs5,  6 * FPUREGBYTES(t0)
    FSREG  fs4,  7 * FPUREGBYTES(t0)
    FSREG  fs3,  8 * FPUREGBYTES(t0)
    FSREG  fs2,  9 * FPUREGBYTES(t0)
    FSREG  fs1, 10 * FPUREGBYTES(t0)
    FSREG  fs0, 11 * FPUREGBYTES(t0)
.endm

.macro  PUSH_INTEGER_CALLEE_REG
    SREG   s11,   4 * REGBYTES(sp)
    SREG   s10,   5 * REGBYTES(sp)
    SREG   s9,    6 * REGBYTES(sp)
    SREG   s8,    7 * REGBYTES(sp)
    SREG   s7,    8 * REGBYTES(sp)
    SREG   s6,    9 * REGBYTES(sp)
    SREG   s5,   10 * REGBYTES(sp)
    SREG   s4,   11 * REGBYTES(sp)
    SREG   s3,   12 * REGBYTES(sp)
    SREG   s2,   13 * REGBYTES(sp)
    SREG   s1,   14 * REGBYTES(sp)
    SREG   s0,   15 * REGBYTES(sp)
.endm

.macro  PUSH_CALLEE_REG
#ifdef  LOS_COMPILE_LDM
#ifdef LOSCFG_ARCH_FPU_ENABLE
    addi   t0, sp, CPU_SPECIAL_REG_SIZE
    stmia  {s0 - s11}, (t0)
    addi   t0, sp, INTEGER_REG_SIZE
    PUSH_FPU_CALLEE_REG
#else
    addi  t0, sp, CPU_SPECIAL_REG_SIZE
    stmia {s0 - s11}, (t0)
#endif
#else
#ifdef LOSCFG_ARCH_FPU_ENABLE
    PUSH_INTEGER_CALLEE_REG
    addi   t0,   sp, INTEGER_REG_SIZE
    PUSH_FPU_CALLEE_REG
#else
    PUSH_INTEGER_CALLEE_REG
#endif
#endif
.endm

.macro POP_FPU_GENERAL_REG
    FLREG  fs11,  0 * FPUREGBYTES(t0)
    FLREG  fs10,  1 * FPUREGBYTES(t0)
    FLREG  fs9,   2 * FPUREGBYTES(t0)
    FLREG  fs8,   3 * FPUREGBYTES(t0)
    FLREG  fs7,   4 * FPUREGBYTES(t0)
    FLREG  fs6,   5 * FPUREGBYTES(t0)
    FLREG  fs5,   6 * FPUREGBYTES(t0)
    FLREG  fs4,   7 * FPUREGBYTES(t0)
    FLREG  fs3,   8 * FPUREGBYTES(t0)
    FLREG  fs2,   9 * FPUREGBYTES(t0)
    FLREG  fs1,  10 * FPUREGBYTES(t0)
    FLREG  fs0,  11 * FPUREGBYTES(t0)
    FLREG  ft11, 12 * FPUREGBYTES(t0)
    FLREG  ft10, 13 * FPUREGBYTES(t0)
    FLREG  ft9,  14 * FPUREGBYTES(t0)
    FLREG  ft8,  15 * FPUREGBYTES(t0)
    FLREG  fa7,  16 * FPUREGBYTES(t0)
    FLREG  fa6,  17 * FPUREGBYTES(t0)
    FLREG  fa5,  18 * FPUREGBYTES(t0)
    FLREG  fa4,  19 * FPUREGBYTES(t0)
    FLREG  fa3,  20 * FPUREGBYTES(t0)
    FLREG  fa2,  21 * FPUREGBYTES(t0)
    FLREG  fa1,  22 * FPUREGBYTES(t0)
    FLREG  fa0,  23 * FPUREGBYTES(t0)
    FLREG  ft7,  24 * FPUREGBYTES(t0)
    FLREG  ft6,  25 * FPUREGBYTES(t0)
    FLREG  ft5,  26 * FPUREGBYTES(t0)
    FLREG  ft4,  27 * FPUREGBYTES(t0)
    FLREG  ft3,  28 * FPUREGBYTES(t0)
    FLREG  ft2,  29 * FPUREGBYTES(t0)
    FLREG  ft1,  30 * FPUREGBYTES(t0)
    FLREG  ft0,  31 * FPUREGBYTES(t0)
.endm

.macro POP_ALL_FPU_REG
    lw     t0, FCSR_STACK_OFFSET(sp)
    fscsr  t0
    addi   t0, sp, INTEGER_REG_SIZE
    POP_FPU_GENERAL_REG
.endm

.macro POP_ALL_INTEGER_REG
    LREG   s11,   4 * REGBYTES(sp)
    LREG   s10,   5 * REGBYTES(sp)
    LREG   s9,    6 * REGBYTES(sp)
    LREG   s8,    7 * REGBYTES(sp)
    LREG   s7,    8 * REGBYTES(sp)
    LREG   s6,    9 * REGBYTES(sp)
    LREG   s5,   10 * REGBYTES(sp)
    LREG   s4,   11 * REGBYTES(sp)
    LREG   s3,   12 * REGBYTES(sp)
    LREG   s2,   13 * REGBYTES(sp)
    LREG   s1,   14 * REGBYTES(sp)
    LREG   s0,   15 * REGBYTES(sp)
    LREG   t6,   16 * REGBYTES(sp)
    LREG   t5,   17 * REGBYTES(sp)
    LREG   t4,   18 * REGBYTES(sp)
    LREG   t3,   19 * REGBYTES(sp)
    LREG   a7,   20 * REGBYTES(sp)
    LREG   a6,   21 * REGBYTES(sp)
    LREG   a5,   22 * REGBYTES(sp)
    LREG   a4,   23 * REGBYTES(sp)
    LREG   a3,   24 * REGBYTES(sp)
    LREG   a2,   25 * REGBYTES(sp)
    LREG   a1,   26 * REGBYTES(sp)
    LREG   a0,   27 * REGBYTES(sp)
    LREG   t2,   28 * REGBYTES(sp)
    LREG   t1,   29 * REGBYTES(sp)
    LREG   t0,   30 * REGBYTES(sp)
    LREG   ra,   31 * REGBYTES(sp)
.endm

.macro POP_ALL_REG
    lw      t0, 1 * REGBYTES(sp)
    csrw    mepc, t0              /* load mepc from previously reserved (4 * REGBYTES) space */
    lw      t0, 0 * REGBYTES(sp)
    csrw    mstatus, t0           /* load mstatus from previously reserved (4 * REGBYTES) space */
#ifdef  LOS_COMPILE_LDM
#ifdef LOSCFG_ARCH_FPU_ENABLE
    POP_ALL_FPU_REG
    addi   t0, sp, CPU_SPECIAL_REG_SIZE
    ldmia  {s0 - s11}, (t0)
    addi   sp, sp, (INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES
    ldmia  {ra, t0 - t2, a0 - a7, t3 - t6}, (sp)
    addi   sp, sp, (INTEGER_CALLER_REG_NUM + FPU_EXTENSION_REG_NUM + FPU_STATUS_REG_NUM) * REGBYTES
#else
    addi  t0, sp, CPU_SPECIAL_REG_SIZE
    ldmia {s0 - s11}, (t0)
    addi  sp, sp, (INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM) * REGBYTES
    ldmia {ra, t0 - t2, a0 - a7, t3 - t6}, (sp)
    addi  sp, sp, INTEGER_CALLER_REG_SIZE
#endif
#else
#ifdef LOSCFG_ARCH_FPU_ENABLE
    POP_ALL_FPU_REG
    POP_ALL_INTEGER_REG
    addi   sp, sp, INT_SIZE_ON_STACK
#else
    POP_ALL_INTEGER_REG
    addi  sp, sp, INT_SIZE_ON_STACK
#endif
#endif
.endm

.macro RESTORE_ALL
#ifdef LOSCFG_ARCH_FPU_ENABLE
    lw     t0, (EXC_SPECIAL_REG_SIZE + FCSR_STACK_OFFSET)(sp)
    fscsr  t0
    addi   t0, sp, EXC_SPECIAL_REG_SIZE + INTEGER_REG_SIZE
    POP_FPU_GENERAL_REG
#endif
    LREG  t0, 4 * REGBYTES(sp)
    csrw  mstatus, t0
    LREG  t0, 5 * REGBYTES(sp)
    csrw  mepc, t0
#ifdef  LOS_COMPILE_LDM
    addi  sp, sp, CPU_SPECIAL_REG_SIZE + EXC_SPECIAL_REG_SIZE
    ldmia {s0-s11}, (sp)
    addi  sp, sp, INTEGER_CALLEE_REG_SIZE
    ldmia {ra, t0-t2, a0-a7, t3-t6}, (sp)
    addi  sp, sp, INTEGER_CALLER_REG_SIZE
#else
    LREG  s11, 8 * REGBYTES(sp)
    LREG  s10, 9 * REGBYTES(sp)
    LREG  s9, 10 * REGBYTES(sp)
    LREG  s8, 11 * REGBYTES(sp)
    LREG  s7, 12 * REGBYTES(sp)
    LREG  s6, 13 * REGBYTES(sp)
    LREG  s5, 14 * REGBYTES(sp)
    LREG  s4, 15 * REGBYTES(sp)
    LREG  s3, 16 * REGBYTES(sp)
    LREG  s2, 17 * REGBYTES(sp)
    LREG  s1, 18 * REGBYTES(sp)
    LREG  s0, 19 * REGBYTES(sp)
    LREG  t6, 20 * REGBYTES(sp)
    LREG  t5, 21 * REGBYTES(sp)
    LREG  t4, 22 * REGBYTES(sp)
    LREG  t3, 23 * REGBYTES(sp)
    LREG  a7, 24 * REGBYTES(sp)
    LREG  a6, 25 * REGBYTES(sp)
    LREG  a5, 26 * REGBYTES(sp)
    LREG  a4, 27 * REGBYTES(sp)
    LREG  a3, 28 * REGBYTES(sp)
    LREG  a2, 29 * REGBYTES(sp)
    LREG  a1, 30 * REGBYTES(sp)
    LREG  a0, 31 * REGBYTES(sp)
    LREG  t2, 32 * REGBYTES(sp)
    LREG  t1, 33 * REGBYTES(sp)
    LREG  t0, 34 * REGBYTES(sp)
    LREG  ra, 35 * REGBYTES(sp)
    addi  sp, sp, (EXC_SIZE_ON_STACK)
#endif
.endm
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_STACK_OPS_H */
