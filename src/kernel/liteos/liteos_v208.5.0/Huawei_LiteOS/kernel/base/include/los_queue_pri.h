/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Queue Private HeadFile
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

#ifndef _LOS_QUEUE_PRI_H
#define _LOS_QUEUE_PRI_H

#include "los_queue.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    OS_QUEUE_READ = 0,
    OS_QUEUE_WRITE = 1,
    OS_QUEUE_N_RW = 2
} QueueReadWrite;

typedef enum {
    OS_QUEUE_HEAD = 0,
    OS_QUEUE_TAIL = 1
} QueueHeadTail;

typedef UINT32 QueueMsgHead;

#define OS_QUEUE_OPERATE_TYPE(ReadOrWrite, HeadOrTail) (((UINT32)(HeadOrTail) << 1) | (ReadOrWrite))
#define OS_QUEUE_READ_WRITE_GET(type) ((type) & 0x01U)
#define OS_QUEUE_READ_HEAD     (OS_QUEUE_READ | (OS_QUEUE_HEAD << 1))
#define OS_QUEUE_READ_TAIL     (OS_QUEUE_READ | (OS_QUEUE_TAIL << 1))
#define OS_QUEUE_WRITE_HEAD    (OS_QUEUE_WRITE | (OS_QUEUE_HEAD << 1))
#define OS_QUEUE_WRITE_TAIL    (OS_QUEUE_WRITE | (OS_QUEUE_TAIL << 1))
#define OS_QUEUE_OPERATE_GET(type) ((type) & 0x03U)
#define OS_QUEUE_IS_READ(type) (OS_QUEUE_READ_WRITE_GET(type) == OS_QUEUE_READ)
#define OS_QUEUE_IS_WRITE(type) (OS_QUEUE_READ_WRITE_GET(type) == OS_QUEUE_WRITE)

/* queue memory type */
#define OS_QUEUE_ALLOC_DYNAMIC 0
#define OS_QUEUE_ALLOC_STATIC 1
#define OS_QUEUE_NORMAL 0

/* COUNT | INDEX  split bit */
#ifdef LOSCFG_BASE_CORE_SYS_RES_CHECK
#define QUEUE_SPLIT_BIT 16
#define SET_QUEUE_ID(count, queueId) (((count) << QUEUE_SPLIT_BIT) | (queueId))
#define GET_QUEUE_INDEX(queueId) ((queueId) & ((1U << QUEUE_SPLIT_BIT) - 1))
#define GET_QUEUE_COUNT(queueId) ((queueId) >> QUEUE_SPLIT_BIT)
#else
#define GET_QUEUE_INDEX(queueId) (queueId)
#endif
#define GET_QUEUE_HANDLE(queueId) (((LosQueueCB *)g_osAllQueue) + GET_QUEUE_INDEX(queueId))
/* Obtain the head node in a queue doubly linked list. */
#define GET_QUEUE_LIST(ptr) LOS_DL_LIST_ENTRY(ptr, LosQueueCB, readWriteList[OS_QUEUE_WRITE])

/* Queue information block structure */
typedef struct {
    UINT8 *queueHandle;                     /* Pointer to a queue handle */
    UINT8 queueState;                       /* state */
    UINT8 queueMemType;                     /* memory type */
    UINT16 queueLen;                        /* length */
    UINT16 queueSize;                       /* Node size */
    UINT32 queueId;                         /* queueId */
    UINT16 queueHead;                       /* Node head */
    UINT16 queueTail;                       /* Node tail */
    UINT16 readWriteableCnt[OS_QUEUE_N_RW];     /* Count of readable or writable resources, 0:readable, 1:writable */
    LOS_DL_LIST readWriteList[OS_QUEUE_N_RW];   /* the linked list to be read or written, 0:readlist, 1:writelist */
    LOS_DL_LIST memList;                        /* Pointer to the memory linked list */
} LosQueueCB;

/*  Queue information control block */
extern LosQueueCB *g_osAllQueue;

/* alloc a stationary memory for a mail according to queueId */
extern VOID *OsQueueMailAlloc(UINT32 queueId, VOID *mailPool, UINT32 timeout);
/* free a stationary memory for a mail according to queueId. */
extern UINT32 OsQueueMailFree(UINT32 queueId, VOID *mailPool, VOID *mailMem);
extern UINT32 OsQueueInit(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_QUEUE_PRI_H */
