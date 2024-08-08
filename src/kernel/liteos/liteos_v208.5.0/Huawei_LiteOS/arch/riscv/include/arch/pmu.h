/*----------------------------------------------------------------------------
* Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
* Description: LiteOS Perf Riscv Pmu Module HeadFile
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
*---------------------------------------------------------------------------*/

#ifndef _ARCH_PMU_H
#define _ARCH_PMU_H

#include "los_typedef.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * According to the spec, an implementation can support counter up to
 * mhpmcounter31, linx170 processors has at 4 general PMCs, we give
 * the definition to MHPMCOUNTER6 here
 */
#define RISCV_PMU_CYCLE         0
#define RISCV_PMU_INSTRET       2
#define RISCV_PMU_MHPMCOUNTER3  3
#define RISCV_PMU_MHPMCOUNTER4  4
#define RISCV_PMU_MHPMCOUNTER5  5
#define RISCV_PMU_MHPMCOUNTER6  6
#define RV32_IDX_MAX_COUNTER    7

/* Performance counter numbers */
#define RV32_IDX_CYCLE_COUNTER  0
#define RV32_IDX_COUNTER0       3

#define REG_MCOUNTINHIBIT       0x320

enum PmuEventType {
    RV32_PERF_HW_CYCLE                          = 0x000,    /* cycle */
    RV32_PERF_HW_INSTRET                        = 0x001,    /* instructions */
    RV32_PERF_HW_FLUSH_NO                       = 0x002,    /* CPU flushes initiated by Commit */
    RV32_PERF_HW_TOTAL_BRANCH                   = 0x040,    /* branch instructions */
    RV32_PERF_HW_MIS_BRANCH                     = 0x041,    /* mis-predicted branches */
    RV32_PERF_HW_UNCOND_DIRECT_BRANCH           = 0x042,
    RV32_PERF_HW_UNCOND_INDIRECT_BRANCH         = 0x043,
    RV32_PERF_HW_MMEM_FETCH                     = 0x080,
    RV32_PERF_HW_MMEM_LOAD                      = 0x081,
    RV32_PERF_HW_MMEM_STORE                     = 0x082,
    RV32_PERF_HW_ITCM_RD                        = 0x083,    /* ITCM read */
    RV32_PERF_HW_DTCM_RD                        = 0x084,    /* DTCM write */
    RV32_PERF_HW_ITCM_WR                        = 0x085,
    RV32_PERF_HW_DTCM_WR                        = 0x086,
    RV32_PERF_HW_PMEM_RD                        = 0x087,
    RV32_PERF_HW_PMEM_WR                        = 0x088,
    RV32_PERF_HW_UNALIGN_LS_ACC                 = 0x089,
    RV32_PERF_HW_ITCM_CONFLICTS                 = 0x08A,
    RV32_PERF_HW_DTCM_CONFLICTS                 = 0x08B,
    RV32_PERF_HW_DTCM_NARROW_WRITE              = 0x08C,
    RV32_PERF_HW_DTCM_RETRY                     = 0x08D,
    RV32_PERF_HW_DTCM_ERR                       = 0x08E,
    RV32_PERF_HW_DTCM_ECC_2BIT_ERR              = 0x08F,
    RV32_PERF_HW_ITCM_NARROW_WRITE              = 0x090,
    RV32_PERF_HW_ITCM_RETRY                     = 0x091,
    RV32_PERF_HW_ITCM_ERR                       = 0x092,
    RV32_PERF_HW_ITCM_ECC_2BIT_ERR              = 0x093,
    RV32_PERF_HW_IC_ACC                         = 0x0C0,    /* icache */
    RV32_PERF_HW_IC_MISS                        = 0x0C1,    /* icache-misses */
    RV32_PERF_HW_DC_ACC                         = 0x0C2,    /* dcache */
    RV32_PERF_HW_DC_MISS                        = 0x0C3,    /* dcache-misses */
    RV32_PERF_HW_IC_UNACC                       = 0x0C4,
    RV32_PERF_HW_DC_UNACC                       = 0x0C5,
    RV32_PERF_HW_IC_AUTO_PREF                   = 0x0C6,
    RV32_PERF_HW_DC_AUTO_PREF                   = 0x0C7,
    RV32_PERF_HW_IC_TAG_ARRAY_ECC_2BIT_ERR      = 0x0C8,
    RV32_PERF_HW_IC_DATA_ARRAY_ECC_2BIT_ERR     = 0x0C9,
    RV32_PERF_HW_IC_TAG_ARRAY_ECC_1BIT_ERR      = 0x0CA,
    RV32_PERF_HW_IC_DATA_ARRAY_ECC_1BIT_ERR     = 0x0CB,
    RV32_PERF_HW_DC_TAG_ARRAY_ECC_2BIT_ERR      = 0x0CC,
    RV32_PERF_HW_DC_DATA_ARRAY_ECC_2BIT_ERR     = 0x0CD,
    RV32_PERF_HW_DC_TAG_ARRAY_ECC_1BIT_ERR      = 0x0CE,
    RV32_PERF_HW_DC_DATA_ARRAY_ECC_1BIT_ERR     = 0x0CF,
    RV32_PERF_HW_DC_NARROW_WRITE                = 0x0D0,
    RV32_PERF_HW_FETCH_BUBBLE                   = 0x180,    /* fetch bubbles */
    RV32_PERF_HW_ISSUE_BUBBLE                   = 0x181,    /* issue bubbles */
    RV32_PERF_HW_MAX
};

VOID OsPerfCheckEventCount(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_PMU_H */
