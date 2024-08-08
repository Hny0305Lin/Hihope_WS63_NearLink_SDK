/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 线程适配层接口cmsis2实现源文件（此文件为DEMO，需集成方适配修改）
 */

#include "hilink_thread_adapter.h"
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "cmsis_os2.h"
#include "securec.h"
#include "hilink_sal_defines.h"

#ifdef SUPPORT_MUTEX_DEBUG
#undef HILINK_MutexLock
#undef HILINK_MutexUnlock
#undef HILINK_MutexCreate
#undef HILINK_MutexDestroy
#endif

#ifndef MS_PER_SECOND
#define MS_PER_SECOND   1000
#endif

HiLinkTaskId HILINK_CreateTask(HiLinkTaskParam *param)
{
    if ((param == NULL) || (param->func == NULL) || (param->prio > HILINK_TASK_PRIORITY_MAX) ||
        (param->stackSize == 0)) {
        HILINK_SAL_WARN("invalid param");
        return NULL;
    }

    const osPriority_t prioMap[] = {
        osPriorityLow1,
        osPriorityBelowNormal1,
        osPriorityNormal2,
        osPriorityNormal7,
        osPriorityHigh
    };

    osThreadAttr_t attr;
    (void)memset_s(&attr, sizeof(osThreadAttr_t), 0, sizeof(osThreadAttr_t));
    attr.name = param->name;
    attr.priority = prioMap[param->prio];
    attr.stack_size = param->stackSize;

    return (HiLinkTaskId)osThreadNew((osThreadFunc_t)param->func, param->arg, &attr);
}

int HILINK_ThreadSuspend(HiLinkTaskId handle)
{
    if (handle == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    osStatus_t status = osThreadSuspend((osThreadId_t)handle);
    if (status != osOK) {
        HILINK_SAL_WARN("suspend error %d\r\n", status);
        return HILINK_SAL_THREAD_ERR;
    }
    return HILINK_SAL_OK;
}

int HILINK_ThreadResume(HiLinkTaskId handle)
{
    if (handle == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    osStatus_t status = osThreadResume((osThreadId_t)handle);
    if (status != osOK) {
        HILINK_SAL_WARN("resume error %d\r\n", status);
        return HILINK_SAL_THREAD_ERR;
    }
    return HILINK_SAL_OK;
}

void HILINK_DeleteTask(HiLinkTaskId handle)
{
    if (handle == NULL) {
        HILINK_SAL_NOTICE("invalid param\r\n");
        return;
    }

    osStatus_t status = osThreadTerminate((osThreadId_t)handle);
    if (status != osOK) {
        HILINK_SAL_NOTICE("delete task error %d\r\n", status);
    }
}

HiLinkTaskId HILINK_GetCurrentTaskId(void)
{
    return (HiLinkTaskId)osThreadGetId();
}

HiLinkMutexId HILINK_MutexCreate(void)
{
    return (HiLinkMutexId)osMutexNew(NULL);
}

static inline unsigned int MsToTick(unsigned int ms)
{
    uint64_t tick = ms * osKernelGetTickFreq() / MS_PER_SECOND;
    if (tick > UINT32_MAX) {
        return UINT32_MAX;
    }
    return (unsigned int)tick;
}

int HILINK_MutexLock(HiLinkMutexId mutex, unsigned int ms)
{
    if (mutex == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    osStatus_t status = osMutexAcquire((osMutexId_t)mutex, MsToTick(ms));
    if (status != osOK) {
        if (status == osErrorTimeout) {
            return HILINK_SAL_TIMEOUT;
        }
        HILINK_SAL_WARN("mutex lock error %d\r\n", status);
        return HILINK_SAL_MUTEX_ERR;
    }

    return HILINK_SAL_OK;
}

int HILINK_MutexUnlock(HiLinkMutexId mutex)
{
    if (mutex == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    osStatus_t status = osMutexRelease((osMutexId_t)mutex);
    if (status != osOK) {
        HILINK_SAL_WARN("mutex unlock error %d\r\n", status);
        return HILINK_SAL_MUTEX_ERR;
    }
    return HILINK_SAL_OK;
}

void HILINK_MutexDestroy(HiLinkMutexId mutex)
{
    if (mutex == NULL) {
        HILINK_SAL_NOTICE("invalid param\r\n");
        return;
    }

    osStatus_t status = osMutexDelete((osMutexId_t)mutex);
    if (status != osOK) {
        HILINK_SAL_NOTICE("mutex delete error %d\r\n", status);
    }
}

HiLinkSemId HILINK_SemCreate(unsigned int count)
{
    /* count为0是默认为二元信号量 */
    return (HiLinkSemId)osSemaphoreNew(count > 0 ? count : 1, count, NULL);
}

int HILINK_SemWait(HiLinkSemId handle, unsigned int ms)
{
    if (handle == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    osStatus_t status = osSemaphoreAcquire((osSemaphoreId_t)handle, MsToTick(ms));
    if (status != osOK) {
        return HILINK_SAL_SEM_ERR;
    }

    return HILINK_SAL_OK;
}

int HILINK_SemPost(HiLinkSemId handle)
{
    if (handle == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    osStatus_t status = osSemaphoreRelease((osSemaphoreId_t)handle);
    if (status != osOK) {
        HILINK_SAL_WARN("post sem error %d\r\n", status);
        return HILINK_SAL_SEM_ERR;
    }

    return HILINK_SAL_OK;
}

void HILINK_SemDestroy(HiLinkSemId handle)
{
    if (handle == NULL) {
        HILINK_SAL_NOTICE("invalid param\r\n");
        return;
    }

    osStatus_t status = osSemaphoreDelete((osSemaphoreId_t)handle);
    if (status != osOK) {
        HILINK_SAL_NOTICE("sem delete error %d\r\n", status);
    }
}

int HILINK_MilliSleep(unsigned int ms)
{
    osStatus_t status = osDelay(MsToTick(ms));
    if (status != osOK) {
        HILINK_SAL_WARN("delay %u ms error %d\r\n", ms, status);
        return -1;
    }
    return HILINK_SAL_SLEEP_ERR;
}

void HILINK_SchedYield(void)
{
    (void)osDelay(0);
}