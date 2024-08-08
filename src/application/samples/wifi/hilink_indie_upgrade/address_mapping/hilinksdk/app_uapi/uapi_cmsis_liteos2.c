/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the cmsis liteos2, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "cmsis_os2.h"

/* Only applicable to the partially used interfaces */
uint32_t osKernelGetTickCount(void)
{
    return app_call0(APP_CALL_OS_KERNEL_GET_TICK_COUNT, uint32_t);
}

uint32_t osKernelGetTickFreq(void)
{
    return app_call0(APP_CALL_OS_KERNEL_GET_TICK_FREQ, uint32_t);
}

osStatus_t osDelay(uint32_t ticks)
{
    return app_call1(APP_CALL_OS_DELAY, osStatus_t, uint32_t, ticks);
}

osThreadId_t osThreadNew(osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
{
    return app_call3(APP_CALL_OS_THREAD_NEW, osThreadId_t, osThreadFunc_t, func, void *, argument,
        const osThreadAttr_t *, attr);
}

osStatus_t osThreadTerminate(osThreadId_t thread_id)
{
    return app_call1(APP_CALL_OS_THREAD_TERMINATE, osStatus_t, osThreadId_t, thread_id);
}

osThreadId_t osThreadGetId(void)
{
    return app_call0(APP_CALL_OS_THREAD_GET_ID, osThreadId_t);
}

osMutexId_t osMutexNew(const osMutexAttr_t *attr)
{
    return app_call1(APP_CALL_OS_MUTEX_NEW, osMutexId_t, const osMutexAttr_t *, attr);
}

osStatus_t osMutexDelete(osMutexId_t mutex_id)
{
    return app_call1(APP_CALL_OS_MUTEX_DELETE, osStatus_t, osMutexId_t, mutex_id);
}

osStatus_t osMutexAcquire(osMutexId_t mutex_id, uint32_t timeout)
{
    return app_call2(APP_CALL_OS_MUTEX_ACQUIRE, osStatus_t, osMutexId_t, mutex_id, uint32_t, timeout);
}

osStatus_t osMutexRelease(osMutexId_t mutex_id)
{
    return app_call1(APP_CALL_OS_MUTEX_RELEASE, osStatus_t, osMutexId_t, mutex_id);
}

osSemaphoreId_t osSemaphoreNew(uint32_t max_count, uint32_t initial_count,
    const osSemaphoreAttr_t *attr)
{
    return app_call3(APP_CALL_OS_SEMAPHORE_NEW, osSemaphoreId_t, uint32_t, max_count,
        uint32_t, initial_count, const osSemaphoreAttr_t *, attr);
}

osStatus_t osSemaphoreAcquire(osSemaphoreId_t semaphore_id, uint32_t timeout)
{
    return app_call2(APP_CALL_OS_SEMAPHORE_ACQUIRE, osStatus_t,
        osSemaphoreId_t, semaphore_id, uint32_t, timeout);
}

osStatus_t osSemaphoreRelease(osSemaphoreId_t semaphore_id)
{
    return app_call1(APP_CALL_OS_SEMAPHORE_RELEASE, osStatus_t, osSemaphoreId_t, semaphore_id);
}

osStatus_t osSemaphoreDelete(osSemaphoreId_t semaphore_id)
{
    return app_call1(APP_CALL_OS_SEMAPHORE_DELETE, osStatus_t, osSemaphoreId_t, semaphore_id);
}

osStatus_t osThreadSuspend(osThreadId_t thread_id)
{
    return app_call1(APP_CALL_OS_THREAD_SUSPEND, osStatus_t, osThreadId_t, thread_id);
}

osStatus_t osThreadResume(osThreadId_t thread_id)
{
    return app_call1(APP_CALL_OS_THREAD_RESUME, osStatus_t, osThreadId_t, thread_id);
}