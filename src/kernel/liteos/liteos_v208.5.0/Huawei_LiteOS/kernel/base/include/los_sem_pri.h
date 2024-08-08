/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Semaphore Private HeadFile
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

#ifndef _LOS_SEM_PRI_H
#define _LOS_SEM_PRI_H

#include "los_sem.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Semaphore control structure. */
typedef struct {
    UINT8 semStat;       /* Semaphore state, enum LosSemState */
    UINT8 semType;       /* Semaphore Type, enum LosSemType */
    UINT16 semCount;     /* number of available semaphores */
    UINT32 semId;        /* Semaphore control structure ID, COUNT(UINT16)|INDEX(UINT16) */
    LOS_DL_LIST semList; /* List of tasks that are waiting on a semaphore */
} LosSemCB;

/* Semaphore type */
enum {
    OS_SEM_COUNTING, /* The semaphore is a counting semaphore which max count is LOS_SEM_COUNT_MAX */
    OS_SEM_BINARY,   /* The semaphore is a binary semaphore which max count is OS_SEM_BINARY_COUNT_MAX */
};

/* Max count of binary semaphores */
#define OS_SEM_BINARY_COUNT_MAX  1

/*  Semaphore information control block */
extern LosSemCB *g_osAllSem;

#define GET_SEM_LIST(ptr)                   LOS_DL_LIST_ENTRY(ptr, LosSemCB, semList)

#ifdef LOSCFG_BASE_CORE_SYS_RES_CHECK
/* COUNT | INDEX  split bit */
#define SEM_SPLIT_BIT                       16
#define SET_SEM_ID(count, semId)            (((count) << SEM_SPLIT_BIT) | (semId))
#define GET_SEM_INDEX(semId)                ((semId) & ((1U << SEM_SPLIT_BIT) - 1))
#define GET_SEM_COUNT(semId)                ((semId) >> SEM_SPLIT_BIT)
#else
#define GET_SEM_INDEX(semId)                (semId)
#endif

#define GET_SEM(semId)                      (((LosSemCB *)g_osAllSem) + GET_SEM_INDEX(semId))

extern UINT32 OsSemInit(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_SEM_PRI_H */
