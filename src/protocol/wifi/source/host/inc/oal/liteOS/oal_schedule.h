/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Create: 2022-4-27
 */

// [ZBL] HCC/OAM
#ifndef __OAL_SCHEDULE_H__
#define __OAL_SCHEDULE_H__

#include "oal_spinlock.h"
#include "oal_time.h"
#include "oal_timer.h"
#include "oal_schedule_hcm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct _oal_wakelock_stru_ {
#ifdef CONFIG_HAS_WAKELOCK
    struct wake_lock        wakelock; /* wakelock锁 */
    osal_spinlock           lock; /* wakelock锁操作spinlock锁 */
#endif
    osal_ulong              lock_count; /* 持有wakelock锁的次数 */
    osal_ulong              locked_addr; /* the locked address */
} oal_wakelock_stru;

#define oal_in_interrupt()  in_interrupt()

typedef osal_u32 (*oal_module_func_t)(osal_void);

#define OAL_S_IRUGO         S_IRUGO

#define oal_module_init(_module_name)
#define oal_module_exit(_module_name)
#define oal_module_symbol(_symbol)

#ifdef CONFIG_RW_LOCK
typedef rwlock_t  oal_rwlock_stru;
#else
typedef struct _oal_rwlock_stru {
    unsigned long           rwlock_id;
} oal_rwlock_stru;
#endif

/*****************************************************************************
 函 数 名  : oal_rw_lock_init
 功能描述  : 读写锁初始化，把读写锁设置为1（未锁状态）。
 输入参数  : lock: 读写锁结构体地址
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年10月12日
    修改内容   : 新生成函数

*****************************************************************************/
OAL_STATIC OAL_INLINE osal_void  oal_rw_lock_init(oal_rwlock_stru *lock)
{
#ifdef CONFIG_RW_LOCK
    rwlock_init(lock);
#else
    unref_param(lock);
#endif
}

/*****************************************************************************
 函 数 名  : oal_rw_lock_read_lock
 功能描述  : 获得指定的读锁
 输入参数  : lock: 读写锁结构体地址
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年10月12日
    修改内容   : 新生成函数

*****************************************************************************/
OAL_STATIC OAL_INLINE osal_void  oal_rw_lock_read_lock(oal_rwlock_stru *lock)
{
#ifdef CONFIG_RW_LOCK
    read_lock(lock);
#else
    unref_param(lock);
#endif
}

/*****************************************************************************
 函 数 名  : oal_rw_lock_read_unlock
 功能描述  : 释放指定的读锁
 输入参数  : lock: 读写锁结构体地址
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年10月12日
    修改内容   : 新生成函数

*****************************************************************************/
OAL_STATIC OAL_INLINE osal_void  oal_rw_lock_read_unlock(oal_rwlock_stru *lock)
{
#ifdef CONFIG_RW_LOCK
    read_unlock(lock);
#else
    unref_param(lock);
#endif
}

/*****************************************************************************
 函 数 名  : oal_rw_lock_write_lock
 功能描述  : 获得指定的写锁
 输入参数  : lock: 读写锁结构体地址
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年10月12日
    修改内容   : 新生成函数

*****************************************************************************/
OAL_STATIC OAL_INLINE osal_void  oal_rw_lock_write_lock(oal_rwlock_stru *lock)
{
#ifdef CONFIG_RW_LOCK
    write_lock(lock);
#else
    unref_param(lock);
#endif
}

/*****************************************************************************
 函 数 名  : oal_rw_lock_write_unlock
 功能描述  : 释放指定的写锁
 输入参数  : lock: 读写锁结构体地址
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年10月12日
    修改内容   : 新生成函数

*****************************************************************************/
OAL_STATIC OAL_INLINE osal_void  oal_rw_lock_write_unlock(oal_rwlock_stru *lock)
{
#ifdef CONFIG_RW_LOCK
    write_unlock(lock);
#else
    unref_param(lock);
#endif
}

OAL_STATIC OAL_INLINE osal_void oal_wake_lock_init(oal_wakelock_stru *wakelock, char* name)
{
#if (defined CONFIG_HAS_WAKELOCK)
    memset_s((osal_void*)wakelock, sizeof(oal_wakelock_stru), 0, sizeof(oal_wakelock_stru));

    wakeup_source_init(&wakelock->wakelock, name ? name : "wake_lock_null");
    osal_spin_lock_init(&wakelock->lock);
    wakelock->lock_count = 0;
    wakelock->locked_addr = 0;
    /* add to local list */
    osal_list_add_tail(&wakelock->list, oal_get_wakelock_head());
#else
    unref_param(wakelock);
    unref_param(name);
#endif
}

OAL_STATIC OAL_INLINE osal_void oal_wake_lock_exit(oal_wakelock_stru *wakelock)
{
#if (defined CONFIG_HAS_WAKELOCK)
    osal_list_del(&wakelock->list);
    osal_spin_lock_destroy(&wakelock->lock);
    wakeup_source_trash(&wakelock->wakelock);
#else
    unref_param(wakelock);
#endif
}

OAL_STATIC OAL_INLINE void oal_wake_lock(oal_wakelock_stru *wakelock)
{
#if (defined CONFIG_HAS_WAKELOCK)
    osal_ulong flags;

    osal_spin_lock_irqsave(&wakelock->lock, &flags);
    if (!wakelock->lock_count) {
        __pm_stay_awake(&wakelock->wakelock);
        wakelock->locked_addr = (osal_ulong)_RET_IP_;
    }
    wakelock->lock_count++;
    if (OAL_UNLIKELY(wakelock->debug)) {
        wifi_printf(KERN_DEBUG"wakelock[%s] lockcnt:%lu <==%pf\n", wakelock->wakelock.name, wakelock->lock_count,
            (osal_void *)_RET_IP_);
    }
    osal_spin_unlock_irqrestore(&wakelock->lock, &flags);
#else
    unref_param(wakelock);
#endif
}

OAL_STATIC OAL_INLINE  void oal_wake_unlock(oal_wakelock_stru *wakelock)
{
#if (defined CONFIG_HAS_WAKELOCK)
    osal_ulong flags;

    osal_spin_lock_irqsave(&wakelock->lock, &flags);
    if (wakelock->lock_count) {
        wakelock->lock_count--;
        if (!wakelock->lock_count) {
            __pm_relax(&wakelock->wakelock);
            wakelock->locked_addr = (osal_ulong)0x0;
        }

        if (OAL_UNLIKELY(wakelock->debug)) {
            wifi_printf(KERN_DEBUG"wakeunlock[%s] lockcnt:%lu <==%pf\n", wakelock->wakelock.name, wakelock->lock_count,
                (osal_void *)_RET_IP_);
        }
    }
    osal_spin_unlock_irqrestore(&wakelock->lock, &flags);
#else
    unref_param(wakelock);
#endif
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of oal_schedule.h */
