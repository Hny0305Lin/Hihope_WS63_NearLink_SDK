/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Scheduler Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2018-09-10
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

#ifndef _LOS_SCHED_PRI_H
#define _LOS_SCHED_PRI_H

#include "los_task_base.h"
#include "los_priqueue_pri.h"
#include "los_percpu_pri.h"
#include "los_hwi.h"
#include "los_spinlock_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern UINT32 g_taskScheduled;

/*
 * Schedule flag, one bit represents one core.
 * This flag is used to prevent kernel scheduling before OSStartToRun.
 */
#define OS_SCHEDULER_SET(cpuid) do {     \
    g_taskScheduled |= (1U << (cpuid));  \
} while (0)

#define OS_SCHEDULER_CLR(cpuid) do {     \
    g_taskScheduled &= ~(1U << (cpuid)); \
} while (0)

#define OS_SCHEDULER_ACTIVE ((g_taskScheduled & (1U << ArchCurrCpuid())) != 0)

#define OS_SCHEDULER_ALL_ACTIVE (g_taskScheduled == LOSCFG_KERNEL_CPU_MASK)

typedef enum {
    INT_NO_RESCH = 0,   /* no needs to schedule */
    INT_PEND_RESCH,     /* pending schedule flag */
    INT_SUSPEND_DELETE_RESCH, /* pending flag in Int in suspend and delete interface */
} SchedFlag;

/*
 * This function simply picks the next task and switches to it.
 * Current task needs to already be in the right state or the right
 * queues it needs to be in.
 */
extern VOID OsSchedResched(VOID);

/*
 * This function put the current task back to the ready queue and
 * try to do the schedule. However, the schedule won't be definitely
 * taken place while there're no other higher priority tasks or locked.
 */
extern VOID OsSchedPreempt(VOID);

#ifndef LOSCFG_SCHED_LATENCY
extern VOID OsSchedProcSchedFlag(VOID);

STATIC INLINE VOID OsSetSchedFlag(UINT32 schedFlag)
{
    /* Set schedule flag if preemption is disabled */
    OsPercpuGet()->schedFlag = schedFlag;
}

/*
 * Just like OsSchedPreempt, except this function will do the OS_INT_ACTIVE
 * check, in case the schedule taken place in the middle of an interrupt.
 */
STATIC INLINE VOID LOS_Schedule(VOID)
{
    if (OS_INT_ACTIVE) {
#ifndef LOSCFG_KERNEL_SMP
        if (OsPercpuGet()->schedFlag == INT_SUSPEND_DELETE_RESCH) {
            return;
        }
#endif
        OsSetSchedFlag(INT_PEND_RESCH);
        return;
    }

    /*
     * trigger schedule in task will also do the slice check
     * if necessary, it will give up the timeslice more in time.
     * otherwise, there's no other side effects.
     */
    OsSchedPreempt();
}
#else
/* There is no need to support latency sched interface after interrupt in Cortex-M */
STATIC INLINE VOID OsSchedProcSchedFlag(VOID)
{
}

STATIC INLINE VOID OsSetSchedFlag(UINT32 schedFlag)
{
    (VOID)schedFlag;
}

STATIC INLINE VOID LOS_Schedule(VOID)
{
    /*
     * trigger schedule in task will also do the slice check
     * if necessary, it will give up the timeslice more in time.
     * otherwise, there's no other side effects.
     */
    OsSchedPreempt();
}
#endif

/* Check if preemptible with counter flag */
STATIC INLINE BOOL OsPreemptable(VOID)
{
    /*
     * Unlike OsPreemptableInSched, the int may be not disabled when OsPreemptable
     * is called, needs manually disable interrupt, to prevent current task from
     * being migrated to another core, and get the wrong preemptible status.
     */
    UINT32 intSave = LOS_IntLock();
    BOOL preemptible = (OsPercpuGet()->taskLockCnt == 0);
    if (!preemptible) {
        OsSetSchedFlag(INT_PEND_RESCH);
    }

    LOS_IntRestore(intSave);
    return preemptible;
}

STATIC INLINE BOOL OsPreemptableInSched(VOID)
{
    BOOL preemptible = FALSE;

#ifdef LOSCFG_KERNEL_SMP
    /*
     * For smp systems, schedule must hold the task spinlock, and this counter
     * will increase by 1 in that case.
     */
    preemptible = (OsPercpuGet()->taskLockCnt == 1);

#else
    preemptible = (OsPercpuGet()->taskLockCnt == 0);
#endif
    if (!preemptible) {
        OsSetSchedFlag(INT_PEND_RESCH);
    }

    return preemptible;
}

#ifdef LOSCFG_BASE_CORE_TIMESLICE
/*
 * This API is used to check time slices. If the number of Ticks equals to the time for task switch,
 * tasks are switched. Otherwise, the Tick counting continues.
 */
extern VOID OsTimesliceCheck(VOID);
#endif

extern VOID OsSchedWait(LosTaskCB *runTask, LOS_DL_LIST *list, UINT32 timeout);
extern VOID OsSchedWake(LosTaskCB *resumedTask);
extern VOID OsSchedPrioModify(LosTaskCB *taskCB, UINT16 priority);
extern UINT32 OsSchedSuspend(LosTaskCB *taskCB);

#ifdef LOSCFG_SCHED_MQ
extern LosTaskCB *OsGetTopTaskByPriQueue(PriQueue *priQueue, UINT32 cpuId);
STATIC INLINE LosTaskCB *OsGetTopTask(VOID)
{
    return OsGetTopTaskByPriQueue(&g_priQueue[ArchCurrCpuid()], ArchCurrCpuid());
}

extern VOID OsSchedLock(VOID);
extern VOID OsSchedUnlock(VOID);
extern SPIN_LOCK_S *OsSchedLockByTask(LosTaskCB *task);
extern UINT32 OsSchedPickCpu(LosTaskCB *task);

STATIC INLINE VOID OsSchedLockGlobal2Local(VOID)
{
    OsSpinUnlockRaw(&g_taskSpin);
    OsSpinLockRaw(&g_priQueue[ArchCurrCpuid()].lock);
}

STATIC INLINE VOID OsSchedLockLocal2Global(VOID)
{
    OsSpinUnlockRaw(&g_priQueue[ArchCurrCpuid()].lock);
    OsSpinLockRaw(&g_taskSpin);
}

STATIC INLINE VOID OsSchedUnlockLocalRestore(UINT32 intSave)
{
    LOS_SpinUnlockNoSched(&g_priQueue[ArchCurrCpuid()].lock);
    LOS_IntRestore(intSave);
}

STATIC INLINE VOID OsSchedLockTwoRaw(UINT32 cpu1, UINT32 cpu2)
{
    if (cpu1 == cpu2) {
        OsSpinLockRaw(&g_priQueue[cpu1].lock);
        return;
    }

    if (cpu1 < cpu2) {
        OsSpinLockRaw(&g_priQueue[cpu1].lock);
        OsSpinLockRaw(&g_priQueue[cpu2].lock);
        return;
    }

    /* cpu1 > cpu2 */
    OsSpinLockRaw(&g_priQueue[cpu2].lock);
    OsSpinLockRaw(&g_priQueue[cpu1].lock);
}

STATIC INLINE VOID OsSchedUnlockRemoteRaw(UINT32 cpu)
{
    if (cpu == ArchCurrCpuid()) {
        return;
    }

    OsSpinUnlockRaw(&g_priQueue[cpu].lock);
}

/*
 * Task set affinity may cause the ready task migrated to another cpu.
 * When the scheduler use the global priqueue, this func will do nothing.
 */
VOID OsSchedAffiChange(VOID *task);

STATIC INLINE VOID OsSchedResume(LosTaskCB *taskCB, UINT32 intSave)
{
    taskCB->taskExtStatus = OS_TASK_STATUS_ENQUE;
    LOS_SpinUnlockNoSched(&g_taskSpin);

    taskCB->taskStatus |= OS_TASK_STATUS_READY;
    OsPriQueueEnqueueProtect(&taskCB->pendList, taskCB->priority, PRI_QUEUE_TAIL);
    taskCB->taskExtStatus &= ~OS_TASK_STATUS_ENQUE;

    LOS_IntRestore(intSave);
}

STATIC INLINE UINT32 OsSchedYield(LosTaskCB *runTask)
{
    UINT32 cpuId;

    runTask->taskExtStatus = OS_TASK_STATUS_ENQUE | OS_TASK_STATUS_SCHED;
    OsSpinUnlockRaw(&g_taskSpin);

    cpuId = OsSchedPickCpu(runTask);

    OsSchedLockTwoRaw(ArchCurrCpuid(), cpuId);

#ifdef LOSCFG_BASE_CORE_TIMESLICE
    /* reset timeslice of yielded task */
    runTask->timeSlice = 0;
#endif

    if (OsPriQueueIsEmpty(runTask->priority)) {
        OsSchedUnlockRemoteRaw(cpuId);
        runTask->taskExtStatus &= ~(OS_TASK_STATUS_ENQUE | OS_TASK_STATUS_SCHED);
        return LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK;
    }

    runTask->taskStatus |= OS_TASK_STATUS_READY;
    runTask->currCpu = cpuId;
    OsPriQueueSimpleEnqueue(&runTask->pendList, PRI_QUEUE_TAIL);
    OsSchedUnlockRemoteRaw(cpuId);

    runTask->taskExtStatus &= ~OS_TASK_STATUS_ENQUE;
    OsSchedResched();

    return LOS_OK;
}

STATIC INLINE UINT32 OsSchedPrioSet(LosTaskCB *taskCB, UINT16 priority, BOOL *needSched)
{
    BOOL needRetry;
    UINT16 tempStatus;

    do {
        /*
         * By default, the loop is executed only once. If priqueue dequeue fails, or the task is inoperable,
         * we need to re-judge the task status and perform the corresponding operation.
         */
        needRetry = FALSE;
        if (taskCB->taskExtStatus & OS_TASK_IS_INOPERABLE) {
            needRetry = TRUE;
            continue;
        }

        tempStatus = taskCB->taskStatus;
        if (tempStatus & OS_TASK_STATUS_UNUSED) {
            return LOS_ERRNO_TSK_NOT_CREATED;
        }

        if (OS_TASK_IS_ZOMBIE(tempStatus)) {
            return LOS_ERRNO_TSK_IS_ZOMBIE;
        }

        if (tempStatus & OS_TASK_STATUS_READY) {
            taskCB->taskExtStatus = OS_TASK_STATUS_ENQUE | OS_TASK_STATUS_DEQUE;
            OsSpinUnlockRaw(&g_taskSpin);

            OsSpinLockRaw(&g_priQueue[taskCB->currCpu].lock);

            if (!(taskCB->taskStatus & OS_TASK_STATUS_READY)) {
                OsSpinUnlockRaw(&g_priQueue[taskCB->currCpu].lock);
                taskCB->taskExtStatus &= ~(OS_TASK_STATUS_ENQUE | OS_TASK_STATUS_DEQUE);

                OsSpinLockRaw(&g_taskSpin);
                needRetry = TRUE;
                continue;
            }

            OsPriQueueDequeue(&taskCB->pendList);

            taskCB->priority = priority;
            OsPriQueueSimpleEnqueue(&taskCB->pendList, PRI_QUEUE_TAIL);
            *needSched = TRUE;
            OsSpinUnlockRaw(&g_priQueue[taskCB->currCpu].lock);
            taskCB->taskExtStatus &= ~(OS_TASK_STATUS_ENQUE | OS_TASK_STATUS_DEQUE);

            OsSpinLockRaw(&g_taskSpin);
        } else {
            taskCB->priority = priority;
            if (tempStatus & OS_TASK_STATUS_RUNNING) {
                *needSched = TRUE;
            }
        }
    } while (needRetry);

    return LOS_OK;
}

/*
 * Description : Removes a ready task from the priority queue.
 * Input       : taskCB     --- Task to be removed
 * Return      : TRUE on success or FALSE on failure
 */
STATIC INLINE BOOL OsSchedReadyTaskRemove(LosTaskCB *taskCB)
{
    taskCB->taskExtStatus = OS_TASK_STATUS_DEQUE;
    BARRIER();
    OsSpinUnlockRaw(&g_taskSpin);

    OsSpinLockRaw(&g_priQueue[taskCB->currCpu].lock);

    if (!(taskCB->taskStatus & OS_TASK_STATUS_READY)) {
        OsSpinUnlockRaw(&g_priQueue[taskCB->currCpu].lock);
        taskCB->taskExtStatus &= ~OS_TASK_STATUS_DEQUE;

        OsSpinLockRaw(&g_taskSpin);
        return FALSE;
    }

    OsPriQueueDequeue(&taskCB->pendList);

    /*
     * There is no lock held here, but the OS_TASK_STATUS_DEQUE protection is used,
     * so setting taskStatus needs to be done before clearing OS_TASK_STATUS_DEQUE.
     */
    taskCB->taskStatus &= ~OS_TASK_STATUS_READY;
    if (OS_TASK_IS_JOINABLE(taskCB)) {
        taskCB->taskStatus |= OS_TASK_STATUS_ZOMBIE;
    } else {
        taskCB->taskStatus |= OS_TASK_STATUS_UNUSED;
    }

    OsSpinUnlockRaw(&g_priQueue[taskCB->currCpu].lock);
    taskCB->taskExtStatus &= ~OS_TASK_STATUS_DEQUE;

    OsSpinLockRaw(&g_taskSpin);
    return TRUE;
}

#else /* LOSCFG_SCHED_SQ */
extern LosTaskCB *OsGetTopTask(VOID);

STATIC INLINE VOID OsSchedLock(VOID)
{
    LOS_SpinLock(&g_taskSpin);
}

STATIC INLINE VOID OsSchedUnlock(VOID)
{
    LOS_SpinUnlock(&g_taskSpin);
}

STATIC INLINE VOID OsSchedLockGlobal2Local(VOID) {}
STATIC INLINE VOID OsSchedLockLocal2Global(VOID) {}

STATIC INLINE VOID OsSchedUnlockLocalRestore(UINT32 intSave)
{
    SCHEDULER_UNLOCK(intSave);
}

STATIC INLINE VOID OsSchedAffiChange(VOID *task)
{
    (VOID)task;
}

STATIC INLINE VOID OsSchedResume(LosTaskCB *taskCB, UINT32 intSave)
{
    taskCB->taskStatus |= OS_TASK_STATUS_READY;
    OsPriQueueEnqueue(&taskCB->pendList, taskCB->priority, PRI_QUEUE_TAIL);

    SCHEDULER_UNLOCK(intSave);
}

STATIC INLINE UINT32 OsSchedYield(LosTaskCB *runTask)
{
#ifdef LOSCFG_BASE_CORE_TIMESLICE
    /* reset timeslice of yielded task */
    runTask->timeSlice = 0;
#endif

    if (OsPriQueueIsEmpty(runTask->priority)) {
        return LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK;
    }

    runTask->taskStatus |= OS_TASK_STATUS_READY;
    OsPriQueueEnqueue(&runTask->pendList, runTask->priority, PRI_QUEUE_TAIL);

    OsSchedResched();

    return LOS_OK;
}

STATIC INLINE UINT32 OsSchedPrioSet(LosTaskCB *taskCB, UINT16 priority, BOOL *needSched)
{
    UINT16 tempStatus = taskCB->taskStatus;

    if (tempStatus & OS_TASK_STATUS_UNUSED) {
        return LOS_ERRNO_TSK_NOT_CREATED;
    }

    if (OS_TASK_IS_ZOMBIE(tempStatus)) {
        return LOS_ERRNO_TSK_IS_ZOMBIE;
    }

    if (tempStatus & OS_TASK_STATUS_READY) {
        OsPriQueueDequeue(&taskCB->pendList);
        taskCB->priority = priority;
        OsPriQueueEnqueue(&taskCB->pendList, taskCB->priority, PRI_QUEUE_TAIL);
        *needSched = TRUE;
    } else {
        taskCB->priority = priority;
        if (tempStatus & OS_TASK_STATUS_RUNNING) {
            *needSched = TRUE;
        }
    }

    return LOS_OK;
}

/*
 * Description : Removes a ready task from the priority queue.
 * Input       : taskCB     --- Task to be removed
 * Return      : TRUE on success or FALSE on failure
 */
STATIC INLINE BOOL OsSchedReadyTaskRemove(LosTaskCB *taskCB)
{
    OsPriQueueDequeue(&taskCB->pendList);
    taskCB->taskStatus &= ~OS_TASK_STATUS_READY;
    return TRUE;
}

#endif /* LOSCFG_SCHED_MQ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_SCHED_PRI_H */
