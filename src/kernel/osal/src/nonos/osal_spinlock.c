/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: OS Abstract Layer.
 */

#include "soc_osal.h"
#include "osal_inner.h"

int osal_spin_lock_init(osal_spinlock *lock)
{
    osal_unused(lock);
    return 0;
}

void osal_spin_lock(osal_spinlock *lock)
{
    osal_unused(lock);
}

void osal_spin_unlock(osal_spinlock *lock)
{
    osal_unused(lock);
}
void osal_spin_lock_irqsave(osal_spinlock *lock, unsigned long *flags)
{
    osal_unused(lock, flags);
    (void)osal_irq_lock();
}

void osal_spin_unlock_irqrestore(osal_spinlock *lock, unsigned long *flags)
{
    osal_unused(lock, flags);
    osal_irq_unlock();
}

void osal_spin_lock_destroy(osal_spinlock *lock)
{
    osal_unused(lock);
}

