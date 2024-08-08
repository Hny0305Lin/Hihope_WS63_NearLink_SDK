/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: OS Abstract Layer.
 */

#include "soc_osal.h"
#include "osal_inner.h"

osal_task *osal_kthread_create(osal_kthread_handler handler, void *data, const char *name, unsigned int stack_size)
{
    osal_unused(handler, data, name, stack_size);
    return NULL;
}

void osal_udelay(unsigned int usecs)
{
    osal_unused(usecs);
}