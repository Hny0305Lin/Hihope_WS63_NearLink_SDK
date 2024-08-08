/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides systick driver source \n
 *
 * History: \n
 * 2022-08-01, Create file. \n
 */
#include <stdbool.h>
#include "securec.h"
#include "interrupt/osal_interrupt.h"

#include "systick_porting.h"
#include "hal_systick.h"
#include "systick.h"

static bool g_systick_hw_inited = false;

#if defined(CONFIG_SYSTICK_SUPPORT_LPM)
static uint64_t g_systick_sleep_compensation_count = 0;
static uint64_t g_systick_sleep_suspend_current_count = 0;
#endif

void uapi_systick_init(void)
{
    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(g_systick_hw_inited)) {
        osal_irq_restore(irq_sts);
        return;
    }

    hal_systick_init();
    g_systick_hw_inited = true;
    osal_irq_restore(irq_sts);
}

void uapi_systick_deinit(void)
{
    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(!g_systick_hw_inited)) {
        osal_irq_restore(irq_sts);
        return;
    }

    hal_systick_deinit();
    g_systick_hw_inited = false;
    osal_irq_restore(irq_sts);
}

errcode_t uapi_systick_count_clear(void)
{
    errcode_t ret;
    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(!g_systick_hw_inited)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SYSTICK_NOT_INIT;
    }

    ret = hal_systick_count_clear();
    osal_irq_restore(irq_sts);

    return ret;
}

uint64_t uapi_systick_get_count(void)
{
    uint64_t count = 0;

    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(!g_systick_hw_inited)) {
        osal_irq_restore(irq_sts);
        return count;
    }

    count = hal_systick_get_count();
    osal_irq_restore(irq_sts);

#if defined(CONFIG_SYSTICK_SUPPORT_LPM)
    return count + g_systick_sleep_compensation_count;
#else
    return count;
#endif
}

uint64_t uapi_systick_get_s(void)
{
    return convert_count_2_s(uapi_systick_get_count());
}

uint64_t uapi_systick_get_ms(void)
{
    return convert_count_2_ms(uapi_systick_get_count());
}

uint64_t uapi_systick_get_us(void)
{
    return convert_count_2_us(uapi_systick_get_count());
}

errcode_t uapi_systick_delay_count(uint64_t c_delay)
{
    if (unlikely(!g_systick_hw_inited)) {
        return ERRCODE_SYSTICK_NOT_INIT;
    }

    uint64_t current_ticks;
    uint64_t origin_ticks;

    origin_ticks = uapi_systick_get_count();
    for (;;) {
        current_ticks = uapi_systick_get_count();
        if ((current_ticks - origin_ticks) >= c_delay) {
            break;
        }
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_systick_delay_s(uint32_t s_delay)
{
    if (unlikely(!g_systick_hw_inited)) {
        return ERRCODE_SYSTICK_NOT_INIT;
    }

    uapi_systick_delay_count((uint64_t)convert_s_2_count((uint64_t)s_delay));

    return ERRCODE_SUCC;
}

errcode_t uapi_systick_delay_ms(uint32_t m_delay)
{
    if (unlikely(!g_systick_hw_inited)) {
        return ERRCODE_SYSTICK_NOT_INIT;
    }

    uapi_systick_delay_count((uint64_t)convert_ms_2_count((uint64_t)m_delay));

    return ERRCODE_SUCC;
}

errcode_t uapi_systick_delay_us(uint32_t u_delay)
{
    if (unlikely(!g_systick_hw_inited)) {
        return ERRCODE_SYSTICK_NOT_INIT;
    }

    uapi_systick_delay_count((uint64_t)convert_us_2_count((uint64_t)u_delay));
    return ERRCODE_SUCC;
}

#if defined(CONFIG_SYSTICK_SUPPORT_LPM)
errcode_t uapi_systick_suspend(uintptr_t arg)
{
    uint64_t sleep_compensation_count = uapi_systick_get_count();
    uint32_t irq_sts = osal_irq_lock();
    g_systick_sleep_suspend_current_count = sleep_compensation_count;
    osal_irq_restore(irq_sts);

    if ((uint64_t *)arg != NULL) {
        *(uint64_t *)arg = sleep_compensation_count;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_systick_resume(uintptr_t arg)
{
    errcode_t ret;
    uint32_t irq_sts;
    uint64_t sleep_compensation_count = *(uint64_t *)arg;
    uapi_systick_deinit();
    uapi_systick_init();
    ret = uapi_systick_count_clear();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    irq_sts = osal_irq_lock();
    g_systick_sleep_compensation_count = g_systick_sleep_suspend_current_count + sleep_compensation_count;
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}
#endif  /* CONFIG_SYSTICK_SUPPORT_LPM */