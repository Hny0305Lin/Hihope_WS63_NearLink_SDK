/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Software Timer Manager Private HeadFile
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

#ifndef _LOS_SWTMR_PRI_H
#define _LOS_SWTMR_PRI_H

#include "los_swtmr.h"
#include "los_spinlock.h"
#include "los_sortlink_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_BASE_CORE_SWTMR

enum SwtmrState {
    OS_SWTMR_STATUS_UNUSED,     /* The software timer is not used. */
    OS_SWTMR_STATUS_DELETING,   /* The software timer is deleting. */
    OS_SWTMR_STATUS_CREATED,    /* The software timer is created. */
    OS_SWTMR_STATUS_TICKING     /* The software timer is timing. */
};

/* Structure of the callback function that handles software timer timeout */
typedef struct {
    SWTMR_PROC_FUNC handler;    /* Callback function that handles software timer timeout */
    UINTPTR arg;                /* Parameter passed in when the callback function
                                     that handles software timer timeout is called */
} SwtmrHandlerItem;

typedef struct {
    SortLinkList sortList;
    UINT8 state;      /* Software timer state */
    UINT8 mode;       /* Software timer mode */
    UINT8 overrun;    /* Times that a software timer repeats timing */
    UINT8 inProcess; /* Software timer handler is in process */
    UINT16 timerId;   /* Software timer ID */
    UINT32 interval;  /* Timeout interval of a periodic software timer (unit: tick) */
    UINT32 expiry;    /* Timeout interval of an one-off software timer (unit: tick) */
#ifdef LOSCFG_KERNEL_SMP
    UINT32 cpuid;     /* The cpu where the timer running on */
#endif
    UINTPTR arg;      /* Parameter passed in when the callback function
                             that handles software timer timeout is called */
    SWTMR_PROC_FUNC handler; /* Callback function that handles software timer timeout */
} LosSwtmrCB;

/* Type of the pointer to the structure of the callback function that handles software timer timeout */
typedef SwtmrHandlerItem *SwtmrHandlerItemPtr;

extern LosSwtmrCB *g_osSwtmrCBArray;
extern UINT8 *g_osSwtmrTaskStack[LOSCFG_KERNEL_CORE_NUM];

/* The software timer count list */
extern SortLinkAttribute g_swtmrSortLink;

#ifdef LOSCFG_BASE_CORE_SYS_RES_CHECK
#define OS_SWT_FROM_SWTID(swtmrId) ((LosSwtmrCB *)g_osSwtmrCBArray + ((swtmrId) % KERNEL_SWTMR_LIMIT))
#else
#define OS_SWT_FROM_SWTID(swtmrId) ((LosSwtmrCB *)g_osSwtmrCBArray + swtmrId)
#endif

/* This API is used to scan a software timer when a Tick interrupt occurs and determine whether
 * the software timer expires. */
extern VOID OsSwtmrScan(VOID);
extern UINT32 OsSwtmrInit(VOID);
extern VOID OsSwtmrTask(VOID);
extern SPIN_LOCK_S g_swtmrSpin;
#ifdef LOSCFG_EXC_INTERACTION
extern BOOL IsSwtmrTask(UINT32 taskId);
#endif
#ifdef LOSCFG_DEBUG_SWTMR
#define OS_ALL_SWTMR_MASK 0xffffffff
extern UINT32 OsDbgSwtmrInfoGet(UINT32 timerId);
#endif
#endif /* LOSCFG_BASE_CORE_SWTMR */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_SWTMR_PRI_H */
