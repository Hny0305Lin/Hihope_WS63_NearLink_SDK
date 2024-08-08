/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: OS Abstract Layer.
 */

#include "soc_osal.h"
#include "osal_inner.h"

int osal_mutex_init(osal_mutex *mutex)
{
    osal_unused(mutex);
    return OSAL_NONEOS_DEFAULT_RET;
}

int osal_mutex_lock(osal_mutex *mutex)
{
    osal_unused(mutex);

    return OSAL_NONEOS_DEFAULT_RET;
}

void osal_mutex_unlock(osal_mutex *mutex)
{
    osal_unused(mutex);
}

void osal_mutex_destroy(osal_mutex *mutex)
{
    osal_unused(mutex);
}
