/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2022. All rights reserved.
 * Description: osal adapt atomic source file.
 */

#include "osal_adapt.h"

int osal_adapt_atomic_init(osal_atomic *atomic)
{
    osal_atomic_set(atomic, 0);
    return OSAL_SUCCESS;
}

void osal_adapt_atomic_destroy(osal_atomic *atomic)
{
    osal_atomic_set(atomic, 0);
}

int osal_adapt_atomic_read(osal_atomic *atomic)
{
    return osal_atomic_read(atomic);
}

void osal_adapt_atomic_set(osal_atomic *atomic, int val)
{
    osal_atomic_set(atomic, val);
}

int osal_adapt_atomic_inc_return(osal_atomic *atomic)
{
    return osal_atomic_inc_return(atomic);
}

int osal_adapt_atomic_dec_return(osal_atomic *atomic)
{
    return osal_atomic_dec_return(atomic);
}

void osal_adapt_atomic_inc(osal_atomic *atomic)
{
    osal_atomic_inc(atomic);
}

void osal_adapt_atomic_dec(osal_atomic *atomic)
{
    osal_atomic_dec(atomic);
}
void osal_adapt_atomic_add(osal_atomic *atomic, int val)
{
    osal_atomic_add(atomic, val);
}