/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: CPU Operations HeadFile
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

#ifndef _ARCH_CPU_H
#define _ARCH_CPU_H

#ifndef __ASSEMBLER__
#include "los_typedef.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __riscv64
#define LREG                    ld
#define SREG                    sd
#define FLREG                   fld
#define FSREG                   fsd
#define REGBYTES                8
#define FPUREGBYTES             8
#else
#define LREG                    lw
#define SREG                    sw
#define REGBYTES                4
#ifdef LOSCFG_ARCH_FPU_DOUBLE
#define FLREG                   fld
#define FSREG                   fsd
#define FPUREGBYTES             8
#else
#define FLREG                   flw
#define FSREG                   fsw
#define FPUREGBYTES             4
#endif
#endif

/*  |-----------------------------| if define LOSCFG_ARCH_FPU_ENABLE
    |     FPU_STATUS_REG_NUM      |   fcsr, res[3]
    |-----------------------------|
    |     FPU_CALLER_REG_NUM      |   ft0 - ft11, fa0 - fa7
    |-----------------------------|
    |     FPU_CALLEE_REG_NUM      |   fs0 - fs11
    |-----------------------------| endif LOSCFG_ARCH_FPU_ENABLE
    |   INTEGER_CALLER_REG_NUM    |   t0 - t6, a0 - a7, ra
    |-----------------------------|
    |   INTEGER_CALLEE_REG_NUM    |   s0 - s11
    |-----------------------------| if in Exc context
    |    EXC_SPECIAL_REG_NUM      |   ccause,mcause,mtval,gp
    |-----------------------------| endif in Exc context
*/
#define TP_STACK_INDEX              2
#define SP_STACK_INDEX              3

#define EXC_SPECIAL_REG_NUM         4   // ccause,mcause,mtval,gp
#define EXC_SPECIAL_REG_SIZE        (4 * REGBYTES)

#define INTEGER_CALLER_REG_NUM      16
#define INTEGER_CALLEE_REG_NUM      12
#define CPU_SPECIAL_REG_NUM         4   // mstatus,mepc,tp,sp
#define INTEGER_REG_NUM             (INTEGER_CALLER_REG_NUM + INTEGER_CALLEE_REG_NUM + CPU_SPECIAL_REG_NUM)

#define INTEGER_CALLER_REG_SIZE     (INTEGER_CALLER_REG_NUM * REGBYTES)
#define INTEGER_CALLEE_REG_SIZE     (INTEGER_CALLEE_REG_NUM * REGBYTES)
#define CPU_SPECIAL_REG_SIZE        (CPU_SPECIAL_REG_NUM * REGBYTES)
#define INTEGER_REG_SIZE            (INTEGER_CALLER_REG_SIZE + INTEGER_CALLEE_REG_SIZE + CPU_SPECIAL_REG_SIZE)

#ifdef LOSCFG_ARCH_FPU_ENABLE
#define FPU_STATUS_REG_NUM          4   // fcsr,res[3]
#define FPU_CALLER_REG_NUM          20
#define FPU_CALLEE_REG_NUM          12
#define FPU_EXTENSION_REG_NUM       (FPU_CALLER_REG_NUM + FPU_CALLEE_REG_NUM)
#define FPU_STATUS_REG_SIZE         (FPU_STATUS_REG_NUM * REGBYTES)
#define FPU_CALLER_REG_SIZE         (FPU_CALLER_REG_NUM * FPUREGBYTES)
#define FPU_CALLEE_REG_SIZE         (FPU_CALLEE_REG_NUM * FPUREGBYTES)
#define FPU_EXTENSION_REG_SIZE      (FPU_CALLER_REG_SIZE + FPU_CALLEE_REG_SIZE)

#define FCSR_STACK_OFFSET           (INTEGER_REG_SIZE + FPU_EXTENSION_REG_SIZE)

#define INT_SIZE_ON_STACK           (INTEGER_REG_SIZE + FPU_EXTENSION_REG_SIZE + FPU_STATUS_REG_SIZE)
#else
#define INT_SIZE_ON_STACK           INTEGER_REG_SIZE
#endif

/* risc-v mstatus Register */
#define LOS_MSTATUS_MIE         0x00000008
#define LOS_MSTATUS_MPIE        0x00000080
#define LOS_MSTATUS_MPP         0x00001800
#define LOS_MSTATUS_FS          0x00006000
#ifdef LOSCFG_ARCH_LINX_M
#define LOS_MCAUSE_UOP          0x0100F000
#endif
#ifdef LOSCFG_ARCH_FPU_ENABLE
#define LOSCFG_MSTATUS_M        (LOS_MSTATUS_MPP | LOS_MSTATUS_MPIE | LOS_MSTATUS_FS)
#else
#define LOSCFG_MSTATUS_M        (LOS_MSTATUS_MPP | LOS_MSTATUS_MPIE)
#endif
#define UINT32_CUT_MASK         0xFFFFFFFF
#define UINT8_CUT_MASK          0xFF

#ifdef LOSCFG_ARCH_LINX_M
#define NUM_HAL_INTERRUPT_NMI   0xFFF
#endif

#ifndef __ASSEMBLER__
STATIC INLINE UINT32 ArchCurrCpuid(VOID)
{
    return 0;
}

STATIC INLINE UINT32 ArchSPGet(VOID)
{
    UINT32 val;
    __asm__ volatile("mv %0, sp" : "=r"(val));
    return val;
}

STATIC INLINE const CHAR *ArchCpuInfo(VOID)
{
    return "unknown";
}
#endif  /* __ASSEMBLER__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
