/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: Risc-v Hw Runstop Implementation
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
 * ---------------------------------------------------------------------------- */

#if defined(LOSCFG_KERNEL_DEEPSLEEP) || defined(LOSCFG_KERNEL_RUNSTOP)

.extern g_saveSRContext

.global OsSRSaveRegister
.global OsSRRestoreRegister

#ifdef __riscv64
#define LREG ld
#define SREG sd
#define REGBYTES 8
#else
#define LREG lw
#define SREG sw
#define REGBYTES 4
#endif

.section .text
.option rvc

/* Save CPU-Registers to RAM */
OsSRSaveRegister:
    csrrw a0, mepc, a0
    la a0, g_saveSRContext
    SREG ra, 0 * REGBYTES(a0)
    SREG sp, 1 * REGBYTES(a0)
    SREG gp, 2 * REGBYTES(a0)
    SREG tp, 3 * REGBYTES(a0)
    SREG t0, 4 * REGBYTES(a0)
    SREG t1, 5 * REGBYTES(a0)
    SREG t2, 6 * REGBYTES(a0)
    SREG s0, 7 * REGBYTES(a0)
    SREG s1, 8 * REGBYTES(a0)
    SREG a1, 10 * REGBYTES(a0)
    csrr a1, mepc
    SREG a1, 9 * REGBYTES(a0)
    LREG a1, 10 * REGBYTES(a0)
    SREG a2, 11 * REGBYTES(a0)
    SREG a3, 12 * REGBYTES(a0)
    SREG a4, 13 * REGBYTES(a0)
    SREG a5, 14 * REGBYTES(a0)
    SREG a6, 15 * REGBYTES(a0)
    SREG a7, 16 * REGBYTES(a0)
    SREG s2, 17 * REGBYTES(a0)
    SREG s3, 18 * REGBYTES(a0)
    SREG s4, 19 * REGBYTES(a0)
    SREG s5, 20 * REGBYTES(a0)
    SREG s6, 21 * REGBYTES(a0)
    SREG s7, 22 * REGBYTES(a0)
    SREG s8, 23 * REGBYTES(a0)
    SREG s9, 24 * REGBYTES(a0)
    SREG s10, 25 * REGBYTES(a0)
    SREG s11, 26 * REGBYTES(a0)
    SREG t3, 27 * REGBYTES(a0)
    SREG t4, 28 * REGBYTES(a0)
    SREG t5, 29 * REGBYTES(a0)
    SREG t6, 30 * REGBYTES(a0)

    addi a0, a0, 124

    csrr a7, 0xBC0
    csrr a6, 0xBC1
    csrr a5, 0xBC2
    csrr a4, 0xBC3
    csrr a3, 0xBE0
    csrr a2, 0xBE8
    csrr a1, 0xBFE
    csrr t0, 0x7C0
    stmia {a1-a7, t0}, (a0)
    addi a0, a0, 32

    csrr a7, 0x7C1
    csrr a6, 0x7C2
    csrr a5, 0x7C3
    csrr a4, 0x7C4
    csrr a3, 0x7C5
    csrr a2, 0x7D8
    csrr a1, 0x7D9
    csrr t0, mstatus
    stmia {a1-a7, t0}, (a0)
    addi a0, a0, 32

    csrr a7, misa
    csrr a6, mie
    csrr a5, mtvec
    csrr a4, mscratch
    csrr a3, mepc
    csrr a2, mcause
    csrr a1, pmpaddr0
    csrr t0, pmpaddr1
    stmia {a1-a7, t0}, (a0)
    addi a0, a0, 32

    csrr a7, pmpaddr2
    csrr a6, pmpaddr3
    csrr a5, pmpaddr4
    csrr a4, pmpaddr5
    csrr a3, pmpaddr6
    csrr a2, pmpaddr7
    csrr a1, pmpaddr8
    csrr t0, pmpaddr9
    stmia {a1-a7, t0}, (a0)
    addi a0, a0, 32

    csrr a7, pmpaddr10
    csrr a6, pmpaddr11
    csrr a5, pmpaddr12
    csrr a4, pmpaddr13
    csrr a3, pmpaddr14
    csrr a2, pmpaddr15
    csrr a1, pmpcfg0
    csrr t0, pmpcfg1
    stmia {a1-a7, t0}, (a0)
    addi a0, a0, 32

    csrr a2, pmpcfg2
    csrr a1, pmpcfg3
    stmia {a1-a2}, (a0)
    addi a0, a0, 8

    addi a0, a0, -168
    auipc a1,  0
    addi a1, a1,  16
    SREG a1, 20 * REGBYTES(a0)
    addi a0, a0, -124
    LREG a1, 10 * REGBYTES(a0)
    LREG a0, 9 * REGBYTES(a0)
    csrrw a0 ,mepc,a0

    ret


/* Restore CPU-Registers from RAM */
OsSRRestoreRegister:
    la a0, g_saveSRContext
    addi a0, a0, 124
    ldmia {a1-a7, t0}, (a0)
    csrw 0xBC0, a7
    csrw 0xBC1, a6
    csrw 0xBC2, a5
    csrw 0xBC3, a4
    csrw 0xBE0, a3
    csrw 0xBE8, a2
    csrw 0xBFE, a1
    csrw 0x7C0, t0
    addi a0, a0, 32

    ldmia {a1-a7, t0}, (a0)
    csrw 0x7C1, a7
    csrw 0x7C2, a6
    csrw 0x7C3, a5
    csrw 0x7C4, a4
    csrw 0x7C5, a3
    csrw 0x7D8, a2
    csrw 0x7D9, a1
    csrw mstatus, t0
    addi a0, a0, 32

    ldmia {a1-a7, t0}, (a0)
    csrw misa, a7
    csrw mie, a6
    csrw mtvec,a5
    csrw mscratch, a4
    csrw mepc, a3
    csrw mcause, a2
    csrw pmpaddr0, a1
    csrw pmpaddr1, t0
    addi a0, a0, 32

    ldmia {a1-a7, t0}, (a0)
    csrw pmpaddr2, a7
    csrw pmpaddr3, a6
    csrw pmpaddr4, a5
    csrw pmpaddr5, a4
    csrw pmpaddr6, a3
    csrw pmpaddr7, a2
    csrw pmpaddr8, a1
    csrw pmpaddr9, t0
    addi a0, a0, 32

    ldmia {a1-a7, t0}, (a0)
    csrw pmpaddr10, a7
    csrw pmpaddr11, a6
    csrw pmpaddr12, a5
    csrw pmpaddr13, a4
    csrw pmpaddr14, a3
    csrw pmpaddr15, a2
    csrw pmpcfg0, a1
    csrw pmpcfg1, t0
    addi a0, a0, 32

    ldmia {a1-a2}, (a0)
    csrw pmpcfg2, a2
    csrw pmpcfg3, a1

    la a0,  g_saveSRContext
    LREG x1, 0 * REGBYTES(a0)
    LREG x2, 1 * REGBYTES(a0)
    LREG x3, 2 * REGBYTES(a0)
    LREG x4, 3 * REGBYTES(a0)
    LREG x5, 4 * REGBYTES(a0)
    LREG x6, 5 * REGBYTES(a0)
    LREG x7, 6 * REGBYTES(a0)
    LREG x8, 7 * REGBYTES(a0)
    LREG x9, 8 * REGBYTES(a0)
    LREG x11, 10 * REGBYTES(a0)
    LREG x12, 11 * REGBYTES(a0)
    LREG x13, 12 * REGBYTES(a0)
    LREG x14, 13 * REGBYTES(a0)
    LREG x15, 14 * REGBYTES(a0)
    LREG x16, 15 * REGBYTES(a0)
    LREG x17, 16 * REGBYTES(a0)
    LREG x18, 17 * REGBYTES(a0)
    LREG x19, 18 * REGBYTES(a0)
    LREG x20, 19 * REGBYTES(a0)
    LREG x21, 20 * REGBYTES(a0)
    LREG x22, 21 * REGBYTES(a0)
    LREG x23, 22 * REGBYTES(a0)
    LREG x24, 23 * REGBYTES(a0)
    LREG x25, 24 * REGBYTES(a0)
    LREG x26, 25 * REGBYTES(a0)
    LREG x27, 26 * REGBYTES(a0)
    LREG x28, 27 * REGBYTES(a0)
    LREG x29, 28 * REGBYTES(a0)
    LREG x30, 29 * REGBYTES(a0)
    LREG x31, 30 * REGBYTES(a0)
    LREG x10, 9 * REGBYTES(a0)
    csrrw a0 ,mepc,a0
    jr a0

#endif
