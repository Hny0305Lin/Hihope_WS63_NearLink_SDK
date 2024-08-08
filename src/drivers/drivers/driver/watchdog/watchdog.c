
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides watchdog driver source \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include <stdio.h>
#include <stdbool.h>
#include "securec.h"
#include "soc_osal.h"
#include "errcode.h"
#include "watchdog_porting.h"
#include "hal_watchdog.h"
#include "watchdog.h"

static bool g_watchdog_inited = false;
static bool g_watchdog_enabled = false;
static hal_watchdog_funcs_t *g_hal_funcs = NULL;
#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
static wdt_mode_t g_wdt_mode = WDT_MODE_INTERRUPT;
static uint32_t g_wdt_timeout = 0;
#endif
errcode_t uapi_watchdog_init(uint32_t timeout)
{
    unused(timeout);

    errcode_t ret = ERRCODE_FAIL;

    watchdog_port_register_hal_funcs();
    watchdog_port_register_irq();

    unsigned int irq_sts = osal_irq_lock();
    g_hal_funcs = hal_watchdog_get_funcs();
    ret = g_hal_funcs->init();
    if (ret != ERRCODE_SUCC) {
        osal_irq_restore(irq_sts);
        return ret;
    }
#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
    g_wdt_timeout = timeout;
#endif
#if !defined(CONFIG_WATCHDOG_ALREADY_START)
    ret = g_hal_funcs->set_attr(timeout);
#endif  /* CONFIG_WATCHDOG_ALREADY_START */
    osal_irq_restore(irq_sts);

    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    g_watchdog_inited = true;

    return ret;
}

errcode_t uapi_watchdog_deinit(void)
{
    if (unlikely(g_watchdog_inited == 0)) {
        return ERRCODE_SUCC;
    }

    if (unlikely(g_watchdog_enabled != 0)) {
        uapi_watchdog_disable();
    }

    if (g_hal_funcs != NULL) {
        g_hal_funcs->deinit();
    }

    watchdog_port_unregister_hal_funcs();

    g_watchdog_inited = false;

    return ERRCODE_SUCC;
}

#if !defined(CONFIG_WATCHDOG_ALREADY_START)
errcode_t uapi_watchdog_enable(wdt_mode_t mode)
{
    if (unlikely((g_watchdog_inited == 0) || (mode >= WDT_MODE_MAX))) {
        return ERRCODE_FAIL;
    }

#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
    g_wdt_mode = mode;
#endif
    unsigned int irq_sts = osal_irq_lock();
    g_hal_funcs->enable((hal_wdt_mode_t)mode);
    osal_irq_restore(irq_sts);

    g_watchdog_enabled = true;

    return ERRCODE_SUCC;
}
#else
errcode_t uapi_watchdog_enable(wdt_mode_t mode)
{
    if (unlikely((g_watchdog_inited == 0) || (mode >= WDT_MODE_MAX))) {
        return ERRCODE_FAIL;
    }
#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
    g_wdt_mode = mode;
#endif
    g_watchdog_enabled = true;

    return ERRCODE_SUCC;
}
#endif  /* CONFIG_WATCHDOG_ALREADY_START */

errcode_t uapi_watchdog_disable(void)
{
    if (unlikely(g_watchdog_inited == 0)) {
        return ERRCODE_FAIL;
    }

    unsigned int irq_sts = osal_irq_lock();
    g_hal_funcs->disable();
    osal_irq_restore(irq_sts);

    g_watchdog_enabled = false;

    return ERRCODE_SUCC;
}

errcode_t uapi_watchdog_set_time(uint32_t timeout)
{
    errcode_t ret = ERRCODE_FAIL;

    if (unlikely(g_watchdog_inited == 0)) {
        return ERRCODE_FAIL;
    }

    if (unlikely(g_watchdog_enabled != 0)) {
        uapi_watchdog_disable();
    }
#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
    g_wdt_timeout = timeout;
#endif
    unsigned int irq_sts = osal_irq_lock();
    ret = g_hal_funcs->set_attr(timeout);
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t uapi_watchdog_get_left_time(uint32_t *timeout)
{
    if (unlikely(g_watchdog_enabled == 0)) {
        return ERRCODE_FAIL;
    }

    *timeout = g_hal_funcs->get_left_time();
    if (unlikely(*timeout == 0)) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_watchdog_kick(void)
{
    if (unlikely(g_watchdog_enabled == 0)) {
        return ERRCODE_FAIL;
    }

    unsigned int irq_sts = osal_irq_lock();
#if defined(CONFIG_WATCHDOG_SUPPORT_ULP_WDT) && (CONFIG_WATCHDOG_SUPPORT_ULP_WDT == 1)
    ulp_wdt_kick();
#endif
    g_hal_funcs->kick();
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

errcode_t uapi_register_watchdog_callback(watchdog_callback_t callback)
{
    if (unlikely((g_watchdog_inited == 0) || (callback == NULL))) {
        return ERRCODE_FAIL;
    }

    unsigned int irq_sts = osal_irq_lock();
    g_hal_funcs->register_callback(callback);
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
errcode_t uapi_watchdog_suspend(uintptr_t arg)
{
    unused(arg);
    return ERRCODE_SUCC;
}

errcode_t uapi_watchdog_resume(uintptr_t arg)
{
    errcode_t ret = ERRCODE_FAIL;
    unused(arg);
    if (unlikely(g_watchdog_inited == 0)) {
        return ERRCODE_SUCC;
    }
    ret = g_hal_funcs->set_attr(g_wdt_timeout);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    g_hal_funcs->enable((hal_wdt_mode_t)g_wdt_mode);
    return ERRCODE_SUCC;
}
#endif
