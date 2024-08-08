/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Priority Queue Private HeadFile
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

#ifndef _LOS_PRIQUEUE_PRI_H
#define _LOS_PRIQUEUE_PRI_H

#include "los_list.h"
#include "los_typedef.h"
#include "los_spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    PRI_QUEUE_HEAD = 0,
    PRI_QUEUE_TAIL = 1
} PriQueueHeadTail;

/* Number of priority queues. Now LiteOS only supports 32 priorities. */
#define OS_PRIORITY_QUEUE_NUM 32
#define PRIQUEUE_PRIOR0_BIT   0x80000000U

/* This API is used to initialize the priority queue. */
extern VOID OsPriQueueInit(VOID);

/* This API is used to delete a item from the priority queue. */
extern VOID OsPriQueueDequeue(LOS_DL_LIST *priqueueItem);

/*
 * These two functions are protect version of OsPriQueueEnqueue and OsPriQueueDequeue.
 * In SCHED_SQ, the task status and priqueue are protected by the global schedule lock, this two function
 * are just the same as the previous ones.
 */
#ifdef LOSCFG_SCHED_MQ

struct tagTaskCB;

typedef struct {
    UINT32              bitmap;                         /* priority-queue bitmap */
    LOS_DL_LIST         queues[OS_PRIORITY_QUEUE_NUM];  /* one for each priority */
    INT32               readyTasks;                     /* number of ready tasks */
    SPIN_LOCK_S         lock;                           /* private spinlock */
    struct tagTaskCB    *prevTask;                      /* previously ran task */
} PriQueue;

/* Per-cpu priority queue */
extern PriQueue g_priQueue[LOSCFG_KERNEL_CORE_NUM];

/*
 * This API is used to get whether the priority queue is empty.
 */
STATIC INLINE BOOL OsPriQueueIsEmpty(UINT32 priority)
{
    PriQueue *priQueue = &g_priQueue[ArchCurrCpuid()];

    return ((priQueue->bitmap & (PRIQUEUE_PRIOR0_BIT >> priority)) == 0);
}

extern VOID OsPriQueueSimpleEnqueue(LOS_DL_LIST *queueNode, PriQueueHeadTail mode);
extern VOID OsPriQueueEnqueueProtect(LOS_DL_LIST *queueNode, UINT32 priority, PriQueueHeadTail mode);
extern BOOL OsPriQueueDequeueProtect(LOS_DL_LIST *priqueueItem);

#else /* LOSCFG_SCHED_SQ */
/* This API is used to insert a item to the priority queue according to the priority of this item. */
extern VOID OsPriQueueEnqueue(LOS_DL_LIST *priqueueItem, UINT32 priority, PriQueueHeadTail mode);

/*
 * This API is used to get whether the priority queue is empty.
 */
extern BOOL OsPriQueueIsEmpty(UINT32 priority);

STATIC INLINE VOID OsPriQueueEnqueueProtect(LOS_DL_LIST *queueNode, UINT32 priority, PriQueueHeadTail mode)
{
    OsPriQueueEnqueue(queueNode, priority, mode);
}

STATIC INLINE BOOL OsPriQueueDequeueProtect(LOS_DL_LIST *priqueueItem)
{
    OsPriQueueDequeue(priqueueItem);
    return TRUE;
}
#endif /* LOSCFG_SCHED_MQ */

/*
 * This API is used to obtain the item with highest priority in the priority queue.
 */
extern LOS_DL_LIST *OsPriQueueTop(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_PRIQUEUE_PRI_H */
