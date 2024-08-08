/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: OS Abstract Layer.
 */

#include "soc_osal.h"
#include "osal_inner.h"

int osal_wait_init(osal_wait *wait)
{
    osal_unused(wait);
    return OSAL_NONEOS_DEFAULT_RET;
}

int osal_wait_timeout_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param, unsigned long ms)
{
    osal_unused(wait, func, param, ms);
    return OSAL_NONEOS_DEFAULT_RET;
}

void osal_wait_destroy(osal_wait *wait)
{
    osal_unused(wait);
}

void osal_wait_wakeup(osal_wait *wait)
{
    osal_unused(wait);
}
