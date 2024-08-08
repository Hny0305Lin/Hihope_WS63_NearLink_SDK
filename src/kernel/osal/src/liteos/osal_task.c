/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: task
 *
 * Create: 2021-12-16
 */

#include <los_task.h>
#include <los_base.h>
#include <los_tick.h>
#include <los_sched_pri.h>
#include <los_config.h>
#include <los_memory.h>
#include "soc_osal.h"
#include "osal_inner.h"

#define MINIMAL_STACK_SIZE LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE

osal_task *osal_kthread_create(osal_kthread_handler handler, void *data, const char *name, unsigned int stack_size)
{
    if (handler == NULL) {
        osal_log("parameter invalid!\n");
        return NULL;
    }

    osal_task *task = (osal_task *)LOS_MemAlloc((void*)m_aucSysMem0, sizeof(osal_task));
    if (task == NULL) {
        osal_log("LOS_MemAlloc failed!\n");
        return NULL;
    }

    TSK_INIT_PARAM_S my_task = { 0 };
    my_task.pcName       = (char *)name;
    my_task.uwStackSize  = (stack_size <= MINIMAL_STACK_SIZE) ? MINIMAL_STACK_SIZE : stack_size;
    my_task.usTaskPrio   = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    my_task.uwResved     = LOS_TASK_STATUS_DETACHED;
    my_task.pfnTaskEntry = (TSK_ENTRY_FUNC)handler;

#ifdef HW_LITEOS_OPEN_VERSION_NUM
    LOS_TASK_PARAM_INIT_ARG(my_task, data);
#else
    my_task.auwArgs[0] = (AARCHPTR)data;
#endif

    unsigned int temp_task_id = 0;
    unsigned int ret = LOS_TaskCreate(&temp_task_id, &my_task);
    if (ret != LOS_OK) {
        LOS_MemFree((void*)m_aucSysMem0, (void*)task);
        osal_log("LOS_TaskCreate failed! ret = %#x.\n", ret);
        return NULL;
    }
    task->task = (void *)(UINTPTR)temp_task_id;
    return task;
}

int osal_kthread_set_priority(osal_task *task, unsigned int priority)
{
    if (task == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    unsigned int ret = LOS_TaskPriSet((unsigned int)(UINTPTR)task->task, priority);
    if (ret != LOS_OK) {
        osal_log("LOS_TaskPriSet failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

void osal_kthread_lock(void)
{
    LOS_TaskLock();
}

void osal_kthread_unlock(void)
{
    LOS_TaskUnlock();
}

void osal_kthread_set_affinity(osal_task *task, int cpu_mask)
{
    osal_unused(task, cpu_mask);
    return;
}

int osal_kthread_should_stop(void)
{
    return 0;
}

void osal_kthread_destroy(osal_task *task, unsigned int stop_flag)
{
    unsigned int ret;
    if (task == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    unsigned int uwtask_id = (unsigned int)(UINTPTR)task->task;
    LOS_MemFree((void*)m_aucSysMem0, (void*)task);
    ret = LOS_TaskDelete(uwtask_id);
    if (ret != LOS_OK && ret != LOS_ERRNO_TSK_NOT_CREATED) {
        osal_log("LOS_TaskDelete failed! ret = %#x.\n", ret);
    }
}

void osal_schedule(void)
{
    LOS_Schedule();
}

void osal_kthread_suspend(osal_task *task)
{
    unsigned int ret;
    if (task == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    unsigned int uwtask_id = (unsigned int)(UINTPTR)task->task;
    ret = LOS_TaskSuspend(uwtask_id);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_TSK_ALREADY_SUSPENDED)) {
        osal_log("LOS_TaskSuspend failed! ret = %#x.\n", ret);
        return;
    }
}

void osal_kthread_resume(osal_task *task)
{
    unsigned int ret;
    if (task == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    unsigned int uwtask_id = (unsigned int)(UINTPTR)task->task;
    ret = LOS_TaskResume(uwtask_id);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_TSK_NOT_SUSPENDED)) {
        osal_log("LOS_TaskResume failed! ret = %#x.\n", ret);
        return;
    }
}

void osal_kthread_schedule(unsigned int sleep_ns)
{
    osal_unused(sleep_ns);
    return;
}

void osal_kthread_set_uninterrupt(void)
{
    return;
}

void osal_kthread_set_running(void)
{
    return;
}

void osal_cond_resched(void)
{
    return;
}

void osal_yield(void)
{
    // linux/sched.h:#define cond_resched() do {} while (0)
    return;
}

void osal_kneon_begin(void)
{
    return;
}

void osal_kneon_end(void)
{
    return;
}

/* Description: get cur thread id */
long osal_get_current_tid(void)
{
    unsigned int task_id = LOS_CurTaskIDGet();
    if (task_id == LOS_ERRNO_TSK_ID_INVALID) {
        osal_log("LOS_CurTaskIDGet failed!\n");
        return OSAL_FAILURE;
    }
    return (long)task_id;
}

/* Description: get pid of cur thread id */
long osal_get_current_pid(void)
{
    return osal_get_current_tid();
}

unsigned long osal_msleep(unsigned int msecs)
{
    LOS_Msleep(msecs);
    return 0;
}

void osal_msleep_uninterruptible(unsigned int msecs)
{
    LOS_Msleep(msecs);
}

void osal_udelay(unsigned int usecs)
{
    LOS_Udelay(usecs);
}

void osal_mdelay(unsigned int msecs)
{
    LOS_Mdelay(msecs);
}
