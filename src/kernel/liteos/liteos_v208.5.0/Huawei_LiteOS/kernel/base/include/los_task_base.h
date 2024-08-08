/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: LiteOS Task Base HeadFile.
 * Author: Huawei LiteOS Team
 * Create: 2023-02-15
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

#ifndef _LOS_TASK_BASE_H
#define _LOS_TASK_BASE_H

#include "los_list.h"
#include "los_task.h"
#include "los_sortlink_pri.h"
#include "los_spinlock.h"
#include "los_typedef.h"

#ifdef LOSCFG_BASE_IPC_EVENT
#include "los_event.h"
#endif

#ifdef LOSCFG_KERNEL_LOCKDEP
#include "los_lockdep.h"
#endif

#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
#include "los_sched_debug_pri.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* The task is automatically deleted */
#define OS_TASK_FLAG_DETACHED       0x0001U

/* The resources of the task are released by other task call the join func. */
#define OS_TASK_FLAG_JOINABLE       0x0000U

/* The task is system-level task, like idle, swtmr and etc */
#define OS_TASK_FLAG_SYSTEM         0x0002U

/* The task control block is unused */
#define OS_TASK_STATUS_UNUSED       0x0001U

/* The task is suspended */
#define OS_TASK_STATUS_SUSPEND      0x0002U

/* The task is ready */
#define OS_TASK_STATUS_READY        0x0004U

/* The task is blocked */
#define OS_TASK_STATUS_PEND         0x0008U

/* The task is running */
#define OS_TASK_STATUS_RUNNING      0x0010U

/* The task is delayed */
#define OS_TASK_STATUS_DELAY        0x0020U

/* The time for waiting for an event to occur expires */
#define OS_TASK_STATUS_TIMEOUT      0x0040U

/* The task is pend for a period of time */
#define OS_TASK_STATUS_PEND_TIME    0x0080U

#ifdef LOSCFG_SCHED_MQ
/*
 * These status were added for SCHED_MQ where per-cpu locks are introduced.
 * When task is being scheduled, enqueued, dequeued or woken up,
 * the global scheduler lock will be switched to the per-cpu lock,
 * where there's need for these status to represent the schedule process.
 */
#define OS_TASK_STATUS_SCHED        0x0100U /* The task is being scheduled. */
#define OS_TASK_STATUS_ENQUE        0x0200U /* The Task is being enqueued. */
#define OS_TASK_STATUS_DEQUE        0x0400U /* The task is being dequeued. */
#define OS_TASK_STATUS_WAKEUP       0x1000U /* The task is being woken up. */

#define OS_TASK_IS_INOPERABLE       (OS_TASK_STATUS_SCHED |    \
                                     OS_TASK_STATUS_ENQUE |    \
                                     OS_TASK_STATUS_DEQUE |    \
                                     OS_TASK_STATUS_WAKEUP)
#else /* LOSCFG_SCHED_SQ */
#define OS_TASK_STATUS_SCHED        0x0U
#define OS_TASK_STATUS_ENQUE        0x0U
#define OS_TASK_STATUS_DEQUE        0x0U
#define OS_TASK_STATUS_WAKEUP       0x0U

#define OS_TASK_IS_INOPERABLE       0x0U
#endif

/* This task is waiting for resources to be reclaimed. */
#define OS_TASK_STATUS_ZOMBIE       0x0100U

#define OS_TASK_IS_EXIT             (OS_TASK_STATUS_ZOMBIE | OS_TASK_STATUS_UNUSED)

/* This Macro is used to determine the task attribute detach or join. */
#ifdef LOSCFG_TASK_JOINABLE
#define OS_TASK_IS_JOINABLE(taskCB)         ((taskCB->taskFlags & OS_TASK_FLAG_DETACHED) == 0)
#define OS_TASK_IS_DETACHED(taskCB)         ((taskCB->taskFlags & OS_TASK_FLAG_DETACHED) == OS_TASK_FLAG_DETACHED)
#define OS_TASK_IS_ZOMBIE(taskStatus)       (taskStatus & OS_TASK_STATUS_ZOMBIE)
#define OS_TASK_IS_ALREADY_JOIN(taskCB)     (taskCB->joinner != NULL)
#define OS_TASK_IS_JOINING(taskCB)          (taskCB->joined != NULL)
#else
#define OS_TASK_IS_JOINING(taskCB)          FALSE
#define OS_TASK_IS_JOINABLE(taskCB)         FALSE
#define OS_TASK_IS_DETACHED(taskCB)         TRUE
#define OS_TASK_IS_ZOMBIE(taskStatus)       FALSE
#endif

typedef struct tagTaskCB {
    VOID                *stackPointer;      /* Task stack pointer */
#ifdef LOSCFG_SCHED_MQ
    volatile UINT16     taskStatus;         /* The task status is not protected by the g_taskSpin,
                                             * so it needs to be read again before each use. */
#else
    UINT16              taskStatus;         /* Task status */
#endif
    UINT16              priority;           /* Task priority */
#ifdef LOSCFG_SCHED_MQ
    volatile UINT32     taskExtStatus;      /* Task extend status:used to protect concurrent access to the same task */
#endif
    UINT32              taskFlags : 31;     /* Task extend flags: taskFlags uses 8 bits now. 23 bits left */
    UINT32              usrStack : 1;       /* Usr Stack uses the last bit */
    UINT32              stackSize;          /* Task stack size */
    UINTPTR             topOfStack;         /* Task stack top */
    UINT32              taskId;             /* Task ID */
    TSK_ENTRY_FUNC      taskEntry;          /* Task entrance function */
    VOID                *taskSem;           /* Task-held semaphore */
#ifdef LOSCFG_TASK_JOINABLE
    struct tagTaskCB    *joinner;           /* Pointer to the task that
                                             * invokes LOS_TaskJoin func to wait for cur task */
    struct tagTaskCB    *joined;            /* Point to the task waiting
                                             * by the cur task invoking LOS_TaskJoin func */
    VOID                *threadJoinRetval;  /* Task join exit status */
#endif
    VOID                *taskMux;           /* Task-held mutex */
#ifdef LOSCFG_OBSOLETE_API
    UINTPTR             args[4];            /* Parameter, of which the maximum number is 4 */
#else
    VOID                *args;              /* Parameter, of which the type is void * */
#endif
    CHAR                *taskName;          /* Task name */
    LOS_DL_LIST         pendList;           /* Task pend node */
    SortLinkList        sortList;           /* Task sortlink node */
#ifdef LOSCFG_BASE_IPC_EVENT
    EVENT_CB_S          event;
    UINT32              eventMask;          /* Event mask */
    UINT32              eventMode;          /* Event mode */
#endif
    VOID                *msg;               /* Memory allocated to queues */
    UINT32              priBitMap;          /* BitMap for recording the change of task priority,
                                             the priority can not be greater than 31 */
#ifdef LOSCFG_KERNEL_SMP
    UINT32              mpSignal;           /* Mp Task signal */
#endif
#ifdef LOSCFG_BASE_CORE_TIMESLICE
    UINT16              timeSlice;          /* Remaining time slice */
#endif
#ifdef LOSCFG_BASE_IPC_RWSEM
    UINT16              waitType;           /* The type of the rwsem that the task waits for */
#endif
#ifdef LOSCFG_KERNEL_SMP
    volatile UINT16     currCpu;            /* CPU core number of this task is running on */
    volatile UINT16     lastCpu;            /* CPU core number of this task is running on last time */
    UINT32              timerCpu;           /* CPU core number of this task is delayed or pended */
    UINT16              cpuAffiMask;        /* CPU affinity mask, support up to 16 cores */
#ifdef LOSCFG_KERNEL_SMP_TASK_SYNC
    UINT32              syncSignal;         /* Synchronization for signal handling */
#endif
#endif
#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
    SchedStat           schedStat;          /* Schedule statistics */
#endif
#ifdef LOSCFG_KERNEL_LOCKDEP
    LockDep             lockDep[LOCK_TYPE_MAX];
#endif
#ifdef LOSCFG_KERNEL_PERF
    UINTPTR             pc;
    UINTPTR             fp;
#endif
#ifdef LOSCFG_TRUSTZONE
    void                *secureContextSP;
#endif
} LosTaskCB;

/* scheduler lock */
extern SPIN_LOCK_S g_taskSpin;
#define SCHEDULER_LOCK(state)       LOS_SpinLockSave(&g_taskSpin, &(state))
#define SCHEDULER_UNLOCK(state)     LOS_SpinUnlockRestore(&g_taskSpin, (state))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_TASK_BASE_H */
