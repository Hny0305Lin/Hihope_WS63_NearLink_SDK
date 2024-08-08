/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : Task Low Level Impelmentations Headfile
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
 * ---------------------------------------------------------------------------- */

#ifndef _ARCH_TASK_H
#define _ARCH_TASK_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern VOID *g_newTask;
#define LOSCFG_STACK_POINT_ALIGN_SIZE  16 /* must be 16 bytes aligned when compile -mpush-pop is on */
#define TP_INIT_VALUE                  0x02020202L
#define SP_INIT_VALUE                  0x03030303L
#define S11_INIT_VALUE                 0x04040404L
#define S10_INIT_VALUE                 0x05050505L
#define S9_INIT_VALUE                  0x06060606L
#define S8_INIT_VALUE                  0x07070707L
#define S7_INIT_VALUE                  0x08080808L
#define S6_INIT_VALUE                  0x09090909L
#define S5_INIT_VALUE                  0x0a0a0a0aL
#define S4_INIT_VALUE                  0x0b0b0b0bL
#define S3_INIT_VALUE                  0x0c0c0c0cL
#define S2_INIT_VALUE                  0x0d0d0d0dL
#define S1_INIT_VALUE                  0x0e0e0e0eL
#define FP_INIT_VALUE                  0x0f0f0f0fL
#define T6_INIT_VALUE                  0x10101010L
#define T5_INIT_VALUE                  0x11111111L
#define T4_INIT_VALUE                  0x12121212L
#define T3_INIT_VALUE                  0x13131313L
#define A7_INIT_VALUE                  0x14141414L
#define A6_INIT_VALUE                  0x15151515L
#define A5_INIT_VALUE                  0x16161616L
#define A4_INIT_VALUE                  0x17171717L
#define A3_INIT_VALUE                  0x18181818L
#define A2_INIT_VALUE                  0x19191919L
#define A1_INIT_VALUE                  0x1a1a1a1aL
#define T2_INIT_VALUE                  0x1c1c1c1cL
#define T1_INIT_VALUE                  0x1d1d1d1dL
#define T0_INIT_VALUE                  0x1e1e1e1eL

#ifdef LOSCFG_ARCH_FPU_DOUBLE
typedef UINT64 FLOATREG_TYPE;
#else
typedef UINT32 FLOATREG_TYPE;
#endif

/* Define the type of a task context control block. */
typedef struct {
    UINT32 mstatus;
    UINT32 mepc;
    UINT32 tp;    /* X4 */
    UINT32 sp;    /* X2 */

    UINT32 s11;   /* X27 */
    UINT32 s10;   /* X26 */
    UINT32 s9;    /* X25 */
    UINT32 s8;    /* X24 */
    UINT32 s7;    /* X23 */
    UINT32 s6;    /* X22 */
    UINT32 s5;    /* X21 */
    UINT32 s4;    /* X20 */
    UINT32 s3;    /* X19 */
    UINT32 s2;    /* X18 */

    UINT32 s1;    /* X9 */
    UINT32 s0;    /* X8 */

    UINT32 t6;    /* X31 */
    UINT32 t5;    /* X30 */
    UINT32 t4;    /* X29 */
    UINT32 t3;    /* X28 */

    UINT32 a7;    /* X17 */
    UINT32 a6;    /* X16 */
    UINT32 a5;    /* X15 */
    UINT32 a4;    /* X14 */
    UINT32 a3;    /* X13 */
    UINT32 a2;    /* X12 */
    UINT32 a1;    /* X11 */
    UINT32 a0;    /* X10 */

    UINT32 t2;    /* X7 */
    UINT32 t1;    /* X6 */
    UINT32 t0;    /* X5 */

    UINT32 ra;    /* X1 */

#ifdef LOSCFG_ARCH_FPU_ENABLE
    FLOATREG_TYPE fs11;  /* F27 */
    FLOATREG_TYPE fs10;  /* F26 */
    FLOATREG_TYPE fs9;   /* F25 */
    FLOATREG_TYPE fs8;   /* F24 */
    FLOATREG_TYPE fs7;   /* F23 */
    FLOATREG_TYPE fs6;   /* F22 */
    FLOATREG_TYPE fs5;   /* F21 */
    FLOATREG_TYPE fs4;   /* F20 */
    FLOATREG_TYPE fs3;   /* F19 */
    FLOATREG_TYPE fs2;   /* F18 */
    FLOATREG_TYPE fs1;   /* F9 */
    FLOATREG_TYPE fs0;   /* F8 */

    FLOATREG_TYPE ft11;  /* F31 */
    FLOATREG_TYPE ft10;  /* F30 */
    FLOATREG_TYPE ft9;   /* F29 */
    FLOATREG_TYPE ft8;   /* F28 */
    FLOATREG_TYPE fa7;   /* F17 */
    FLOATREG_TYPE fa6;   /* F16 */
    FLOATREG_TYPE fa5;   /* F15 */
    FLOATREG_TYPE fa4;   /* F14 */
    FLOATREG_TYPE fa3;   /* F13 */
    FLOATREG_TYPE fa2;   /* F12 */
    FLOATREG_TYPE fa1;   /* F11 */
    FLOATREG_TYPE fa0;   /* F10 */
    FLOATREG_TYPE ft7;   /* F7 */
    FLOATREG_TYPE ft6;   /* F6 */
    FLOATREG_TYPE ft5;   /* F5 */
    FLOATREG_TYPE ft4;   /* F4 */
    FLOATREG_TYPE ft3;   /* F3 */
    FLOATREG_TYPE ft2;   /* F2 */
    FLOATREG_TYPE ft1;   /* F1 */
    FLOATREG_TYPE ft0;   /* F0 */

    UINT32 fcsr;  /* FCSR */
    UINT32 reserved[3]; /* 3: aligned 16 bytes */
#endif
} TaskContext;

STATIC INLINE VOID *ArchCurrTaskGet(VOID)
{
    return g_newTask;
}

STATIC INLINE VOID ArchCurrTaskSet(VOID *val)
{
    g_newTask = val;
}

STATIC INLINE UINTPTR ArchGetTaskFp(const VOID *stackPointer)
{
    return ((TaskContext *)(stackPointer))->s0; /* s0: FP */
}

STATIC INLINE UINTPTR ArchGetTaskRa(const VOID *stackPointer)
{
    return ((TaskContext *)(stackPointer))->ra; /* s0: FP */
}

extern VOID *ArchTaskStackInit(UINT32 taskID, UINT32 stackSize, VOID *topStack);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
