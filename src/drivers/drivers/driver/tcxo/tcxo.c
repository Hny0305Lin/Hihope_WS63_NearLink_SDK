/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides tcxo driver source \n
 *
 * History: \n
 * 2022-08-16， Create file. \n
 */

#include <stdbool.h>
#include "common_def.h"
#include "soc_osal.h"
#include "hal_tcxo.h"
#include "tcxo_porting.h"
#include "tcxo.h"

#define USECS_PER_MSEC 1000
static bool g_tcxo_inited = false;

#if defined(CONFIG_TCXO_SUPPORT_LPM)
static uint64_t g_tcxo_sleep_compensation_count = 0;
static uint64_t g_tcxo_sleep_suspend_current_count = 0;
#endif

errcode_t uapi_tcxo_init(void)
{
    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(g_tcxo_inited)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    ret = hal_tcxo_init();
    if (ret != ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ret;
    }

    g_tcxo_inited = true;
    osal_irq_restore(irq_sts);
    return ret;
}

errcode_t uapi_tcxo_deinit(void)
{
    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(!g_tcxo_inited)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    ret = hal_tcxo_deinit();

    g_tcxo_inited = false;
    osal_irq_restore(irq_sts);
    return ret;
}

uint64_t uapi_tcxo_get_count(void)
{
    uint64_t count = 0;
    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(!g_tcxo_inited)) {
        osal_irq_restore(irq_sts);
        return count;
    }
    count = hal_tcxo_get();
    osal_irq_restore(irq_sts);

#if defined(CONFIG_TCXO_SUPPORT_LPM)
    return count + g_tcxo_sleep_compensation_count;
#else
    return count;
#endif
}

uint64_t uapi_tcxo_get_ms(void)
{
    return uapi_tcxo_get_count() / (tcxo_porting_ticks_per_usec_get() * USECS_PER_MSEC);
}

uint64_t uapi_tcxo_get_us(void)
{
    return uapi_tcxo_get_count() / tcxo_porting_ticks_per_usec_get();
}

STATIC errcode_t uapi_tcxo_delay_count(uint64_t ticks_delay)
{
    uint64_t target_ticks;
    uint64_t origin_ticks;

    origin_ticks = uapi_tcxo_get_count();
    target_ticks = origin_ticks + ticks_delay;

    while (uapi_tcxo_get_count() < target_ticks) {
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_tcxo_delay_ms(uint32_t m_delay)
{
    uint64_t ticks_to_delay;

    if (unlikely(!g_tcxo_inited)) {
        return ERRCODE_FAIL;
    }

    ticks_to_delay = (uint64_t)m_delay * USECS_PER_MSEC * tcxo_porting_ticks_per_usec_get();

    return uapi_tcxo_delay_count(ticks_to_delay);
}

errcode_t uapi_tcxo_delay_us(uint32_t u_delay)
{
    uint64_t ticks_to_delay;

    if (unlikely(!g_tcxo_inited)) {
        return ERRCODE_FAIL;
    }

    ticks_to_delay = (uint64_t)u_delay * tcxo_porting_ticks_per_usec_get();

    return uapi_tcxo_delay_count(ticks_to_delay);
}

#if defined(CONFIG_TCXO_SUPPORT_LPM)
errcode_t uapi_tcxo_suspend(uintptr_t arg)
{
    g_tcxo_sleep_suspend_current_count = uapi_tcxo_get_count();
    if ((uint64_t *)arg != NULL) {
        *(uint64_t *)arg = g_tcxo_sleep_suspend_current_count;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_tcxo_resume(uintptr_t arg)
{
    uint64_t sleep_compensation_us = *(uint64_t *)arg;
    uint64_t sleep_compensation_count = sleep_compensation_us * tcxo_porting_ticks_per_usec_get();
    g_tcxo_sleep_compensation_count = g_tcxo_sleep_suspend_current_count + sleep_compensation_count;
    uapi_tcxo_deinit();
    return uapi_tcxo_init();
}
#endif  /* CONFIG_TCXO_SUPPORT_LPM */