/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: atomic
 *
 * Create: 2021-12-16
 */

#include <los_memory.h>
#include "soc_osal.h"
#include "osal_errno.h"
#include "los_atomic.h"
#include "securec.h"
#include "osal_inner.h"

int osal_atomic_read(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    return LOS_AtomicRead((Atomic *)&(atomic->counter));
}

void osal_atomic_set(osal_atomic *atomic, int i)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    LOS_AtomicSet((Atomic *)&(atomic->counter), i);
}

int osal_atomic_inc_return(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    return LOS_AtomicIncRet((Atomic *)&(atomic->counter));
}

int osal_atomic_dec_return(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    return LOS_AtomicDecRet((Atomic *)&(atomic->counter));
}

void osal_atomic_inc(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    LOS_AtomicInc((Atomic *)&(atomic->counter));
}

void osal_atomic_dec(osal_atomic *atomic)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    LOS_AtomicDec((Atomic *)&(atomic->counter));
}

void osal_atomic_add(osal_atomic *atomic, int count)
{
    if (atomic == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    LOS_AtomicAdd((Atomic *)&(atomic->counter), count);
}

void osal_atomic_sub(osal_atomic *atomic, unsigned int count)
{
    LOS_AtomicSub((Atomic *)&(atomic->counter), count);
}

int osal_atomic_add_return(osal_atomic *atomic, int count)
{
    return LOS_AtomicAdd((Atomic *)&(atomic->counter), count);
}

int osal_atomic_dec_and_test(osal_atomic *atomic)
{
    return (LOS_AtomicDecRet((Atomic *)&(atomic->counter)) == 0);
}