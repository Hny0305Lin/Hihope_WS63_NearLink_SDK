/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: LiteOS Task Module Implementation Private HeadFile
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

#ifndef _LOS_TASK_PRI_H
#define _LOS_TASK_PRI_H

#include "los_task_base.h"
#include "los_task.h"
#include "los_sched_pri.h"
#include "los_stackinfo_pri.h"
#include "los_lockdep.h"
#include "arch/task.h"
#ifdef LOSCFG_KERNEL_SMP
#include "los_mp_pri.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
#if ((LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE & (LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) != 0)
#error "idle task stack not aligned"
#endif
#if ((LOSCFG_BASE_CORE_TSK_SWTMR_STACK_SIZE & (LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) != 0)
#error "swtmr task stack not aligned"
#endif
#if ((LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE & (LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) != 0)
#error "default task stack not aligned"
#endif
#endif

/* Task signal types */
#define SIGNAL_NONE                 0U
#define SIGNAL_KILL                 (1U << 0)
#define SIGNAL_SUSPEND              (1U << 1)
#define SIGNAL_AFFI                 (1U << 2)

#define OS_INVALID_VALUE            0xFFFFFFFF

/* default and non-running task's ownership id */
#define OS_TASK_INVALID_CPUID       0xFFFF

/* Null task ID */
#define OS_TASK_ERRORID             0xFFFFFFFF

#define OS_CHECK_TASK_BLOCK         (OS_TASK_STATUS_DELAY |    \
                                     OS_TASK_STATUS_PEND |     \
                                     OS_TASK_STATUS_SUSPEND)

/* Number of usable task priorities */
#define OS_TSK_PRINUM               (LOS_TASK_PRIORITY_LOWEST - LOS_TASK_PRIORITY_HIGHEST + 1)

/* This Macro is used to get task control block index */
#define OS_TSK_GET_INDEX(taskId)    (taskId)

/* This Macro is used to check task id */
#define OS_TASK_ID_CHECK_INVALID(taskId) (OS_TSK_GET_INDEX(taskId) >= g_taskMaxNum)

/* This Macro is used to obtain the pointer to a task control block using a corresponding parameter */
#define OS_TCB_FROM_PENDLIST(ptr) LOS_DL_LIST_ENTRY(ptr, LosTaskCB, pendList)

/* This Macro is used to obtain the pointer to a task control block that has a specified task ID */
#define OS_TCB_FROM_TID(taskId) (((LosTaskCB *)g_osTaskCBArray) + (taskId))

/* This Macro set default task stack memory pool */
#ifndef OS_TASK_STACK_POOL
#define OS_TASK_STACK_POOL m_aucSysMem1
#endif

/* Maximum number of tasks */
extern UINT32 g_taskMaxNum;

/* Starting address of a task */
extern LosTaskCB *g_osTaskCBArray;
extern UINT8     *g_osIdleTaskStack[LOSCFG_KERNEL_CORE_NUM];

/* Releasing a resource hook function */
typedef VOID (*TASK_RECYCLE_HOOK)(UINT32 taskId);
extern TASK_RECYCLE_HOOK g_taskRecycleHook;

/* delete task hook function */
typedef VOID (*TASK_DELETE_HOOK)(UINT32 taskId);
extern TASK_DELETE_HOOK g_taskDeleteHook;

/* Time slice structure */
typedef struct {
    LosTaskCB *task; /* Current running task */
    UINT16 time;     /* Expiration time point */
    UINT16 timeout;  /* Expiration duration */
} OsTaskRobin;

STATIC INLINE LosTaskCB *OsCurrTaskGet(VOID)
{
    return (LosTaskCB *)ArchCurrTaskGet();
}

STATIC INLINE VOID OsCurrTaskSet(LosTaskCB *task)
{
    ArchCurrTaskSet(task);
}

extern VOID ArchTaskSchedule(LosTaskCB *, LosTaskCB *);
extern VOID ArchStartToRun(LosTaskCB *);
extern VOID OsTaskScan(VOID);
extern VOID OsIdleTask(VOID);
extern UINT32 OsIdleTaskCreate(VOID);
extern UINT32 OsTaskInit(VOID);
#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
extern UINT32 OsTaskMonInit(VOID);
extern VOID OsTaskSwitchCheck(const LosTaskCB *oldTask, const LosTaskCB *newTask);
#endif
extern UINT32 OsShellCmdDumpTask(INT32 argc, const CHAR **argv);

/* get task info */
#define OS_ALL_TASK_MASK  0xFFFFFFFF
extern UINT32 OsDbgTskInfoGet(UINT32 taskId);
extern UINT32 OsGetIdleTaskId(VOID);
extern CHAR *OsCurTaskNameGet(VOID);
extern VOID OsTaskAdd2TimerList(LosTaskCB *taskCB, UINT32 timeout);
extern VOID OsTimerListDelete(LosTaskCB *taskCB);
extern VOID OsTaskEntry(UINT32 taskId);
extern VOID OsTaskProcSignal(VOID);

#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
extern VOID OsSchedStat(LosTaskCB *runTask, LosTaskCB *newTask);
#ifdef LOSCFG_KERNEL_SMP
extern VOID OsSchedStatMgr(LosTaskCB *task);
#endif /* LOSCFG_KERNEL_SMP */
#endif /* LOSCFG_DEBUG_SCHED_STATISTICS */
#if defined(LOSCFG_EXC_INTERACTION) || defined(LOSCFG_DEBUG_SCHED_STATISTICS)
extern BOOL IsIdleTask(UINT32 taskId);
#endif

STATIC INLINE UINT32 OsSysTaskSuspend(LosTaskCB *taskCB)
{
    return OsSchedSuspend(taskCB);
}

extern VOID OsIdleHandler(VOID);

/*
 * After SCHED_MQ introduce per-cpu locks and when task is still protected by the global schedule lock,
 * We need to deal with some task status in different ways from SCHED_SQ.
 *
 * 1) OsTaskIsSchedAfterDelete  : The task is still scheduing when it's been added to the recycle list.
 * 2) OsTaskIsSchedCanBeAborted : The task scheduling can be aborted when the global schedule lock is switched
                                  to per-cpu lock in SCHED_MQ. This function is often used when the task is being
                                  put to sleep.
 * 3) OsTaskIsRunOrSched        : The task is running or scheduling.
 */

#ifdef LOSCFG_SCHED_MQ
STATIC INLINE BOOL OsTaskIsSchedAfterDelete(LosTaskCB *taskCB)
{
    return !!((taskCB->taskStatus & OS_TASK_STATUS_RUNNING) | (taskCB->taskExtStatus & OS_TASK_STATUS_SCHED));
}

STATIC INLINE BOOL OsTaskIsSchedCanBeAborted(LosTaskCB *taskCB)
{
    return !!(taskCB->taskStatus & OS_TASK_STATUS_RUNNING);
}

STATIC INLINE BOOL OsTaskIsRunOrSched(LosTaskCB *taskCB)
{
    return !!((taskCB->taskStatus & OS_TASK_STATUS_RUNNING) | (taskCB->taskExtStatus & OS_TASK_STATUS_SCHED));
}

STATIC INLINE VOID OsTaskExtStatusSet(LosTaskCB *task, UINT32 extStatus)
{
    task->taskExtStatus = extStatus;
}

STATIC INLINE VOID OsTaskExtStatusClear(LosTaskCB *task, UINT32 extStatus)
{
    task->taskExtStatus &= ~extStatus;
}

STATIC INLINE VOID OsTaskWaitCanOperate(LosTaskCB *task)
{
    do { } while ((task->taskExtStatus & OS_TASK_IS_INOPERABLE) != 0);
}

STATIC INLINE BOOL OsTaskIsInoperable(LosTaskCB *task)
{
    return !!(task->taskExtStatus & OS_TASK_IS_INOPERABLE);
}

#else /* LOSCFG_SCHED_SQ */

STATIC INLINE BOOL OsTaskIsSchedAfterDelete(LosTaskCB *taskCB)
{
    (VOID)taskCB;
    return FALSE;
}

STATIC INLINE BOOL OsTaskIsSchedCanBeAborted(LosTaskCB *taskCB)
{
    (VOID)taskCB;
    return FALSE;
}

STATIC INLINE BOOL OsTaskIsRunOrSched(LosTaskCB *taskCB)
{
    return !!(taskCB->taskStatus & OS_TASK_STATUS_RUNNING);
}

STATIC INLINE VOID OsTaskExtStatusSet(LosTaskCB *task, UINT32 extStatus)
{
    (VOID)task;
    (VOID)extStatus;
}

STATIC INLINE VOID OsTaskExtStatusClear(LosTaskCB *task, UINT32 extStatus)
{
    (VOID)task;
    (VOID)extStatus;
}

STATIC INLINE VOID OsTaskWaitCanOperate(LosTaskCB *task)
{
    (VOID)task;
}

STATIC INLINE BOOL OsTaskIsInoperable(LosTaskCB *task)
{
    (VOID)task;
    return FALSE;
}
#endif /* LOSCFG_SCHED_MQ */

#ifndef LOSCFG_SCHED_LATENCY
STATIC INLINE VOID OsTaskReSched(VOID)
{
    if (OS_INT_ACTIVE) {
        OsPercpuGet()->schedFlag = INT_SUSPEND_DELETE_RESCH;
        return;
    }

    OsSchedResched();
}
#else
STATIC INLINE VOID OsTaskReSched(VOID)
{
    OsSchedResched();
}
#endif

STATIC INLINE VOID OsTaskCancelJoin(LosTaskCB *taskCB)
{
#ifdef LOSCFG_TASK_JOINABLE
    if (OS_TASK_IS_JOINING(taskCB)) {
        taskCB->joined->joinner = NULL;
        taskCB->joined = NULL;
    }
#else
    (VOID)taskCB;
#endif
}

#ifdef LOSCFG_KERNEL_SMP
STATIC INLINE VOID OsTaskSignalSet(LosTaskCB *taskCB, UINT32 mpSignal)
{
    taskCB->mpSignal = mpSignal;
}
#else
STATIC INLINE VOID OsTaskSignalSet(LosTaskCB *taskCB, UINT32 signal)
{
    (VOID)taskCB;
    (VOID)signal;
}
#endif

/*
 * Check if needs to do the suspend operation on the running task.
 * Return TRUE, if needs to do the suspension.
 * Return FALSE, if meets following circumstances:
 * 1. Do the suspension across cores, if SMP is enabled
 * 2. Do the suspension when preemption is disabled
 * 3. Do the suspension in hard-irq
 * then LOS_TaskSuspend will directly return with 'ret' value.
 */
STATIC INLINE BOOL OsTaskSuspendCheckOnRun(LosTaskCB *taskCB, UINT32 *ret)
{
    /* init default out return value */
    *ret = LOS_OK;

#ifdef LOSCFG_KERNEL_SMP
    /* ASYNCHRONIZED. No need to do task lock checking */
    if (taskCB->currCpu != ArchCurrCpuid()) {
        OsTaskSignalSet(taskCB, SIGNAL_SUSPEND);
        LOS_MpSchedule(CPUID_TO_AFFI_MASK(taskCB->currCpu));
        return FALSE;
    }
#else
    (VOID)taskCB;
#endif

    if (!OsPreemptableInSched()) {
        /* Suspending the current core's running task */
        *ret = LOS_ERRNO_TSK_SUSPEND_LOCKED;
        return FALSE;
    }

#ifdef LOSCFG_KERNEL_SMP
    if (OS_INT_ACTIVE) {
        /*
         * suspend running task in interrupt.
         * mask "suspend" signal and later suspend will be handled.
         */
        OsTaskSignalSet(taskCB, SIGNAL_SUSPEND);
        return FALSE;
    }
#endif

    return TRUE;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_TASK_PRI_H */
