/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: osal atomic source file.
 */

#include "soc_osal.h"
#include "osal_inner.h"

void osal_atomic_set(osal_atomic *atomic, int i)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    unsigned int irq = osal_irq_lock();
    atomic->counter = i;
    osal_irq_restore(irq);
}

int osal_atomic_inc_return(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    unsigned int irq = osal_irq_lock();
    atomic->counter += 1;
    osal_irq_restore(irq);
    return atomic->counter;
}

int osal_atomic_read(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    int value;
    unsigned int irq = osal_irq_lock();
    value = atomic->counter;
    osal_irq_restore(irq);
    return value;
}

void osal_atomic_inc(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    unsigned int irq = osal_irq_lock();
    atomic->counter += 1;
    osal_irq_restore(irq);
}

void osal_atomic_dec(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    unsigned int irq = osal_irq_lock();
    atomic->counter -= 1;
    osal_irq_restore(irq);
}

void osal_atomic_add(osal_atomic *atomic, int count)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    unsigned int irq = osal_irq_lock();
    atomic->counter += count;
    osal_irq_restore(irq);
}
