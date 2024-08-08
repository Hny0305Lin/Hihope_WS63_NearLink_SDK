/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : Exception HeadFile
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

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#include "arch/cpu.h"

#ifndef __ASSEMBLER__
#include "los_typedef.h"
#include "arch/task.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Initial bit32 stack value. */
#define OS_STACK_INIT            0xCACACACA
/* Bit32 stack top magic number. */
#define OS_STACK_MAGIC_WORD      0xCCCCCCCC

#define OS_EXC_STAGE_INIT_VALUE 0xFFFFFFFF
#define OS_EXC_STAGE_INIT       0
#define OS_EXC_STAGE_TASK       1
#define OS_EXC_STAGE_IRQ        2
#define OS_EXC_STAGE_EXC        3

#ifndef __ASSEMBLER__

extern UINTPTR __startup_stack_top;
extern UINTPTR __exc_stack_top;
extern UINTPTR __irq_stack_top;
extern UINTPTR __nmi_stack_top;

typedef struct {
    UINT32      ccause;
    UINT32      mcause;
    UINT32      mtval;
    UINT32      gp;
    TaskContext taskContext;
} ExcContext;

typedef struct {
    UINT16 phase;           /**< Exception occurrence phase: 0 indicates that the exception occurs during
                             *   initialization, 1 indicates that the exception occurs during task, 2
                             *   indicates that the exception occurs during interrupt, and 3 indicates that
                             *   the exception occurs during exception.
                             */
    UINT16 type;            /**< Type of exception, refer to no. 1-19 listed above for exceptions */
    UINT32 faultAddr;       /**< A precise address access error indicates the error access address where
                             *   the exception occurred.
                             */
    UINT32 thrdPid;         /**< An exception occurs in the interrupt, indicating the interrupt number. An
                             *   exception occurs in the task, indicating the task id, or 0xffffffff if it
                             *   occurs during initialization
                             */
    UINT16 nestCnt;         /**< The number of nested exceptions, currently only support the first time the
                             *   exception into the implementation of the registered hook function
                             */
    UINT16 reserved;        /**< Reserved */
    ExcContext *context; /**< The hardware context at which an exception to an automatic stack floating point
                             *   register occurs
                             */
} ExcInfo;

#define ArchGetFp() ({ \
    UINTPTR _fpSave; \
    __asm__ __volatile__("mv %0, s0" : "=r"(_fpSave)); \
    _fpSave; \
})

STATIC INLINE VOID ArchHaltCpu(VOID)
{
    __asm__ __volatile__("ecall");
}

extern VOID ArchBackTraceWithSp(const VOID *stackPointer);
extern VOID ArchBackTrace(VOID);
extern UINT32 ArchExcInit(VOID);
extern UINT32 ArchBackTraceGet(UINTPTR fp, UINTPTR *callChain, UINT32 maxDepth, UINT32 ignrDepth);

typedef VOID (*EXC_PROC_FUNC)(UINT32, const ExcContext *);
extern UINT32 ArchSetExcHook(EXC_PROC_FUNC excHook);
extern UINT32 ArchSetExcUserHook(EXC_PROC_FUNC excHook);
typedef VOID (*NMI_PROC_FUNC)(const ExcContext *);
extern UINT32 ArchSetNMIHook(NMI_PROC_FUNC nmiHook);
#define LOS_ExcRegHook ArchSetExcHook
extern EXC_PROC_FUNC ArchGetExcHook(VOID);
extern NMI_PROC_FUNC ArchGetNMIHook(VOID);
#endif /* __ASSEMBLER__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_EXCEPTION_H */
