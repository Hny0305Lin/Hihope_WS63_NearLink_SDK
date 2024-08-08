/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2022. All rights reserved.
 * Description: OS Abstract Layer.
 */

#ifndef __SOC_OSAL_H__
#define __SOC_OSAL_H__

#include "osal_list.h"
#include "osal_errno.h"

/* osal api header begin */
#include "atomic/osal_atomic.h"
#include "debug/osal_debug.h"
#include "interrupt/osal_interrupt.h"
#include "lock/osal_mutex.h"
#include "schedule/osal_task.h"
#include "event/osal_event.h"
#include "msgqueue/osal_msgqueue.h"

#if defined(__FREERTOS__)
#include "memory/osal_addr.h"
#include "memory/osal_barrier.h"
#include "memory/osal_cache.h"
#include "schedule/osal_completion.h"
#include "device/osal_device.h"
#include "fileops/osal_fileops.h"
#include "math/osal_math.h"
#include "proc/osal_proc.h"
#include "semaphore/osal_semaphore.h"
#include "lock/osal_spinlock.h"
#include "string/osal_string.h"
#include "time/osal_timer.h"
#include "schedule/osal_wait.h"
#include "schedule/osal_workqueue.h"
#endif

#if defined(__linux__)
#include "memory/osal_addr.h"
#include "memory/osal_barrier.h"
#include "device/osal_device.h"
#include "fileops/osal_fileops.h"
#include "math/osal_math.h"
#include "proc/osal_proc.h"
#include "semaphore/osal_semaphore.h"
#include "lock/osal_spinlock.h"
#include "string/osal_string.h"
#include "time/osal_timer.h"
#include "schedule/osal_wait.h"
#include "schedule/osal_workqueue.h"
#if defined(__KERNEL__)
#include "memory/osal_cache.h"
#include "math/osal_bitmap.h"
#include "schedule/osal_completion.h"
#include "schedule/osal_delaywork.h"
#include "lock/osal_rwlock.h"
#endif
#endif

#if defined(__LITEOS__)
#include "memory/osal_addr.h"
#include "memory/osal_barrier.h"
#include "memory/osal_cache.h"
#include "schedule/osal_completion.h"
#include "device/osal_device.h"
#include "event/osal_event.h"
#include "fileops/osal_fileops.h"
#include "math/osal_math.h"
#include "msgqueue/osal_msgqueue.h"
#include "proc/osal_proc.h"
#include "semaphore/osal_semaphore.h"
#include "lock/osal_spinlock.h"
#include "string/osal_string.h"
#include "time/osal_timer.h"
#include "schedule/osal_wait.h"
#include "schedule/osal_workqueue.h"
#endif

#if defined(__NONOS__)
#include "lock/osal_spinlock.h"
#include "time/osal_timer.h"
#include "schedule/osal_wait.h"
#include "semaphore/osal_semaphore.h"
#endif

#if defined(CONFIG_SEC_CORE)
#include "memory/osal_addr.h"
#include "memory/osal_cache.h"
#include "drvbox/osal_drvbox.h"
#include "fileops/osal_fileops.h"
#include "schedule/osal_wait.h"
#endif
/* osal api header end */
#ifdef OSAL_IRQ_RECORD_DEBUG
#include "osal_adapt_debug.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_SUCCESS 0
#define OSAL_FAILURE (-1)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __SOC_OSAL_H__ */
