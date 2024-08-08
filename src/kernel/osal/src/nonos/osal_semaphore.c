/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2021. All rights reserved.
 */

#include "soc_osal.h"
#include "osal_inner.h"

int osal_sem_init(osal_semaphore *sem, int val)
{
    osal_unused(sem, val);
    return 0;
}

void osal_sem_destroy(osal_semaphore *sem)
{
    osal_unused(sem);
    return;
}