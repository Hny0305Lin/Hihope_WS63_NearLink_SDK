/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Create: 2022-4-27
 */

#ifndef __OAL_SPINLOCK_H__
#define __OAL_SPINLOCK_H__

#ifdef _PRE_WLAN_FEATURE_WS73
#include <los_task.h>
#endif
#include "oal_types.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
锁线程调度，支持嵌套, 73 liteos缺少这两个osal接口
*/
#ifdef _PRE_WLAN_FEATURE_WS73
#define osal_spin_lock_bh(lock) do { (void)(lock); LOS_TaskLock(); } while (0)
#define osal_spin_unlock_bh(lock) do { (void)(lock); LOS_TaskUnlock(); } while (0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_spinlock.h */
