/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS exception module implementation.
 * Author: Huawei LiteOS Team
 * Create : 2022-12-20
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

#include "arch/cpu.h"
#include "arch/stack_ops.h"
#include "asm/interrupt_config.h"

.extern   OsExcHandleEntry

.global TrapHandle
.global NmiEntry
.global g_excInfo
.section .itcm.text
.align 4
/************** riscv ccause Register *******************/
.equ ccause, 0xFC2

TrapHandle:
    PUSH_CALLEE_REG
    addi  sp, sp, -EXC_SPECIAL_REG_SIZE
    csrr  s4, ccause
    SREG  s4, 0 * REGBYTES(sp)
    csrr  s3, mcause
    SREG  s3, 1 * REGBYTES(sp)
    csrr  s2, mbadaddr
    SREG  s2, 2 * REGBYTES(sp)
    SREG  gp, 3 * REGBYTES(sp)
    csrr  s1, mstatus
    SREG  s1, 4 * REGBYTES(sp)
    csrr  s0, mepc
    SREG  s0, 5 * REGBYTES(sp)
    SREG  tp, 6 * REGBYTES(sp)
    SREG  sp, 7 * REGBYTES(sp)
    csrw  mscratch, sp      /* save exc stack */
#ifdef LOSCFG_ARCH_LINX_M
    li    a1, NUM_HAL_INTERRUPT_NMI
    bne   a0, a1, 1f
#else
    beqz  a1, 1f
#endif
    tail  NmiEntry
1:
    tail  trapEntry;

NmiEntry:
#ifdef LOSCFG_LIB_CONFIGURABLE
    la    sp, g_nmiStackTop
    lw    sp, 0(sp)
#else
    la    sp, __nmi_stack_top
#endif
    tail nmiHandle;

.section .text
.align 4
nmiHandle:
# Remain in M-mode after mret
    csrr a0, mscratch
    la   t0, g_osNmiHook
    lw   t0, 0(t0)
    jalr t0

    csrr sp, mscratch       /* restore exc stack */
# Remain in M-mode after mret
    li   t0, LOS_MSTATUS_MPP
    csrs mstatus, t0
    RESTORE_ALL
    mret

.align 4
trapEntry:
    la   t0, g_excInfo
    lh   t1, 12(t0)
    bnez t1, 1f
#ifdef LOSCFG_LIB_CONFIGURABLE
    la    sp, g_excStackTop
    lw    sp, 0(sp)
#else
    la    sp, __exc_stack_top
#endif
1:
    addi t1, t1, 0x1        /* g_excInfo.nestCnt++ */
    sh   t1, 12(t0)
    li   t1, REG_CCAUSE_HARD_FAULT_MASK
    and  t2, s4, t1         /* s4 = ccause */
    beqz t2, 2f
    li   t0, 16
    j    4f
2:
    li   t1, REG_CCAUSE_LOCK_UP_MASK
    and  t2, s4, t1         /* s4 = ccause */
    beqz t2, 3f
    li   t0, 17
    j    4f
3:
    /* Handle other exceptions */
    li    t0, MCAUSE_MASK_INT_NUM
    and   t0, s3, t0        /* s3 = mcause */
4:
    mv    a0, t0            /* a0 = t0 = excType */
    csrr  a1, mscratch      /* a1 = excBufAddr */

    la    t0, OsExcHandleEntry
    jalr  t0
    j .
