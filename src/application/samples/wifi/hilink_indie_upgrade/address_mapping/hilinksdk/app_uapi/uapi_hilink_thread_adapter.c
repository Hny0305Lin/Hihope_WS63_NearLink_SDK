/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Thread adaptation layer interface. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_thread_adapter.h"

HiLinkTaskId HILINK_CreateTask(HiLinkTaskParam *param)
{
    return app_call1(APP_CALL_HILINK_CREATE_TASK, HiLinkTaskId, HiLinkTaskParam *, param);
}

int HILINK_ThreadSuspend(HiLinkTaskId handle)
{
    return app_call1(APP_CALL_HILINK_THREAD_SUSPEND, int, HiLinkTaskId, handle);
}

int HILINK_ThreadResume(HiLinkTaskId handle)
{
    return app_call1(APP_CALL_HILINK_THREAD_RESUME, int, HiLinkTaskId, handle);
}

void HILINK_DeleteTask(HiLinkTaskId handle)
{
    app_call1(APP_CALL_HILINK_DELETE_TASK, void, HiLinkTaskId, handle);
}

HiLinkTaskId HILINK_GetCurrentTaskId(void)
{
    return app_call0(APP_CALL_HILINK_GET_CURRENT_TASK_ID, HiLinkTaskId);
}

HiLinkMutexId HILINK_MutexCreate(void)
{
    return app_call0(APP_CALL_HILINK_MUTEX_CREATE, HiLinkMutexId);
}

int HILINK_MutexLock(HiLinkMutexId mutex, unsigned int ms)
{
    return app_call2(APP_CALL_HILINK_MUTEX_LOCK, int, HiLinkMutexId, mutex, unsigned int, ms);
}

int HILINK_MutexUnlock(HiLinkMutexId mutex)
{
    return app_call1(APP_CALL_HILINK_MUTEX_UNLOCK, int, HiLinkMutexId, mutex);
}

void HILINK_MutexDestroy(HiLinkMutexId mutex)
{
    app_call1(APP_CALL_HILINK_MUTEX_DESTROY, void, HiLinkMutexId, mutex);
}

HiLinkSemId HILINK_SemCreate(unsigned int count)
{
    return app_call1(APP_CALL_HILINK_SEM_CREATE, HiLinkSemId, unsigned int, count);
}

int HILINK_SemWait(HiLinkSemId handle, unsigned int ms)
{
    return app_call2(APP_CALL_HILINK_SEM_WAIT, int, HiLinkSemId, handle, unsigned int, ms);
}

int HILINK_SemPost(HiLinkSemId handle)
{
    return app_call1(APP_CALL_HILINK_SEM_POST, int, HiLinkSemId, handle);
}

void HILINK_SemDestroy(HiLinkSemId handle)
{
    app_call1(APP_CALL_HILINK_SEM_DESTROY, int, HiLinkSemId, handle);
}

int HILINK_MilliSleep(unsigned int ms)
{
    return app_call1(APP_CALL_HILINK_MILLI_SLEEP, int, unsigned int, ms);
}

void HILINK_SchedYield(void)
{
    app_call0(APP_CALL_HILINK_SCHED_YIELD, void);
}