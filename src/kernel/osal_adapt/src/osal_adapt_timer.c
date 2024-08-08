/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2022. All rights reserved.
 * Description: osal adapt atomic timer file.
 */
#include "osal_adapt.h"

int osal_adapt_timer_init(osal_timer *timer, void *func, unsigned long data, unsigned int interval)
{
    timer->timer    = NULL;
    timer->handler  = func;
    timer->data     = data;
    timer->interval = interval;
    return osal_timer_init(timer);
}

unsigned int osal_adapt_jiffies_to_msecs(const unsigned int n)
{
    return osal_jiffies_to_msecs(n);
}

int osal_adapt_timer_destroy(osal_timer *timer)
{
    return osal_timer_destroy(timer);
}

unsigned long long osal_adapt_get_jiffies(void)
{
    return osal_get_jiffies();
}

int osal_adapt_timer_mod(osal_timer *timer, unsigned int interval)
{
    return osal_timer_mod(timer, interval);
}
