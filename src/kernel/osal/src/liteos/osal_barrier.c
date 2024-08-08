/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: barrier
 *
 * Create: 2021-12-16
 */

#include <asm/barrier.h>
#include <los_config.h>

void osal_mb(void)
{
}
void osal_rmb(void)
{
}
void osal_wmb(void)
{
}
void osal_smp_mb(void)
{
}
void osal_smp_rmb(void)
{
}
void osal_smp_wmb(void)
{
}
void osal_isb(void)
{
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    ISB();
#else
    Isb();
#endif
}
void osal_dsb(void)
{
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    dsb();
#else
    Dsb();
#endif
}
void osal_dmb(void)
{
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    dmb();
#else
    Dmb();
#endif
}
