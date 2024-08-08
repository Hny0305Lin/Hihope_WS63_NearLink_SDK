/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: spinlock
 *
 * Create: 2021-12-16
 */

#include <los_task.h>
#include <los_memory.h>
#include "los_spinlock.h"
#include "soc_osal.h"
#include "osal_errno.h"
#include "osal_inner.h"

int osal_spin_lock_init(osal_spinlock *lock)
{
    SPIN_LOCK_S *p = NULL;
    SPIN_LOCK_INIT(tmp_lock);
    if (lock == NULL || lock->lock != NULL) {
        osal_log("ERROR: parameter invalid, %p!\r\n", __builtin_return_address(0));
        return OSAL_FAILURE;
    }
    p = (SPIN_LOCK_S *)LOS_MemAlloc((void *)m_aucSysMem0, sizeof(SPIN_LOCK_S));
    if (p == NULL) {
        osal_log("LOS_MemAlloc failed!\n");
        return OSAL_FAILURE;
    }

    errno_t ret = memcpy_s(p, sizeof(SPIN_LOCK_S), &tmp_lock, sizeof(tmp_lock));
    if (ret != EOK) {
        LOS_MemFree((void *)m_aucSysMem0, (void *)p);
        return OSAL_FAILURE;
    }
    lock->lock = p;
    return OSAL_SUCCESS;
}

void osal_spin_lock(osal_spinlock *lock)
{
    if (lock == NULL || lock->lock == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    SPIN_LOCK_S *p = (SPIN_LOCK_S *)(lock->lock);
    LOS_SpinLock(p);
}

int osal_spin_trylock(osal_spinlock *lock)
{
    int ret;
    SPIN_LOCK_S *p = NULL;
    if (lock == NULL || lock->lock == NULL) {
        osal_log("parameter invalid!\n");
        return FALSE;
    }
    p = (SPIN_LOCK_S *)(lock->lock);

    ret = LOS_SpinTrylock(p);
    if (ret != LOS_OK) {
        return FALSE;
    }
    return TRUE;
}

void osal_spin_unlock(osal_spinlock *lock)
{
    if (lock == NULL || lock->lock == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    SPIN_LOCK_S *p = (SPIN_LOCK_S *)(lock->lock);
    LOS_SpinUnlock(p);
}

void osal_spin_lock_bh(osal_spinlock *lock)
{
    (void)lock;
    LOS_TaskLock();
}

void osal_spin_unlock_bh(osal_spinlock *lock)
{
    (void)lock;
    LOS_TaskUnlock();
}

void osal_spin_lock_irqsave(osal_spinlock *lock, unsigned long *flags)
{
    if (lock == NULL || lock->lock == NULL || flags == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    SPIN_LOCK_S *p = (SPIN_LOCK_S *)(lock->lock);
    uint32_t f;
    LOS_SpinLockSave(p, &f);

    *flags = (unsigned long)f;
}

void osal_spin_unlock_irqrestore(osal_spinlock *lock, unsigned long *flags)
{
    if (lock == NULL || lock->lock == NULL || flags == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    SPIN_LOCK_S *p = (SPIN_LOCK_S *)NULL;

    p = (SPIN_LOCK_S *)(lock->lock);
    LOS_SpinUnlockRestore(p, *flags);
}

void osal_spin_lock_destroy(osal_spinlock *lock)
{
    if (lock == NULL || lock->lock == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    SPIN_LOCK_S *p = (SPIN_LOCK_S *)(lock->lock);
    LOS_MemFree((void *)m_aucSysMem0, (void *)p);
    lock->lock = NULL;
}
