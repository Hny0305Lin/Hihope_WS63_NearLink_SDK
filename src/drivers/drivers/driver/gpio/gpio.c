/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides uart driver source \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */
#include <stdbool.h>
#include "soc_osal.h"
#include "securec.h"
#include "gpio_porting.h"
#include "gpio.h"

#define GPIO_DIRECTION_MAX_PARAM 2

static bool g_gpio_inited = false;

void uapi_gpio_init(void)
{
    if (unlikely(g_gpio_inited)) {
        return;
    }
    hal_gpio_init();
    g_gpio_inited = true;
}

void uapi_gpio_deinit(void)
{
    if (unlikely(!g_gpio_inited)) {
        return;
    }
    g_gpio_inited = false;
    hal_gpio_deinit();
}

errcode_t uapi_gpio_set_dir(pin_t pin, gpio_direction_t dir)
{
    if (unlikely(!g_gpio_inited)) {
        return ERRCODE_GPIO_NOT_INIT;
    }

    if (unlikely(dir >= GPIO_DIRECTION_MAX_PARAM)) {
        return ERRCODE_GPIO_DIR_SET_FAIL;
    }

    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    ret = hal_gpio_setdir(pin, dir);
    osal_irq_restore(irq_sts);

    return ret;
}

gpio_direction_t uapi_gpio_get_dir(pin_t pin)
{
    if (unlikely(!g_gpio_inited)) {
        return GPIO_DIRECTION_INPUT;
    }

    gpio_direction_t dir = hal_gpio_getdir(pin);

    return dir;
}

errcode_t uapi_gpio_set_val(pin_t pin, gpio_level_t level)
{
    if (unlikely(!g_gpio_inited)) {
        return ERRCODE_GPIO_NOT_INIT;
    }

    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    ret = hal_gpio_output(pin, level);
    osal_irq_restore(irq_sts);

    return ret;
}

gpio_level_t uapi_gpio_get_output_val(pin_t pin)
{
    if (unlikely(!g_gpio_inited)) {
        return GPIO_LEVEL_LOW;
    }

    gpio_level_t level = hal_gpio_get_outval(pin);

    return level;
}

errcode_t uapi_gpio_toggle(pin_t pin)
{
    if (unlikely(!g_gpio_inited)) {
        return ERRCODE_GPIO_NOT_INIT;
    }

    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    ret = hal_gpio_ctrl(pin, GPIO_CTRL_TOGGLE);
    osal_irq_restore(irq_sts);

    return ret;
}

gpio_level_t uapi_gpio_get_val(pin_t pin)
{
    if (unlikely(!g_gpio_inited)) {
        return GPIO_LEVEL_LOW;
    }

    gpio_level_t level = hal_gpio_get_inval(pin);

    return level;
}

errcode_t uapi_gpio_set_isr_mode(pin_t pin, uint32_t trigger)
{
    if (unlikely(!g_gpio_inited)) {
        return ERRCODE_GPIO_NOT_INIT;
    }

    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    ret = hal_gpio_register(pin, trigger, NULL, false);
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t uapi_gpio_register_isr_func(pin_t pin, uint32_t trigger, gpio_callback_t callback)
{
    if (unlikely(!g_gpio_inited)) {
        return ERRCODE_GPIO_NOT_INIT;
    }

    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    ret = hal_gpio_register(pin, trigger, callback, true);
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t uapi_gpio_unregister_isr_func(pin_t pin)
{
    if (unlikely(!g_gpio_inited)) {
        return ERRCODE_GPIO_NOT_INIT;
    }

    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    ret = hal_gpio_unregister(pin);
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t uapi_gpio_enable_interrupt(pin_t pin)
{
    if (unlikely(!g_gpio_inited)) {
        return ERRCODE_GPIO_NOT_INIT;
    }

    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    ret = hal_gpio_ctrl(pin, GPIO_CTRL_ENABLE_INTERRUPT);
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t uapi_gpio_disable_interrupt(pin_t pin)
{
    if (unlikely(!g_gpio_inited)) {
        return ERRCODE_GPIO_NOT_INIT;
    }

    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    ret = hal_gpio_ctrl(pin, GPIO_CTRL_DISABLE_INTERRUPT);
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t uapi_gpio_clear_interrupt(pin_t pin)
{
    if (unlikely(!g_gpio_inited)) {
        return ERRCODE_GPIO_NOT_INIT;
    }

    errcode_t ret = ERRCODE_FAIL;

    uint32_t irq_sts = osal_irq_lock();
    ret = hal_gpio_ctrl(pin, GPIO_CTRL_CLEAR_INTERRUPT);
    osal_irq_restore(irq_sts);

    return ret;
}

#if defined(CONFIG_GPIO_SUPPORT_LPM)
errcode_t uapi_gpio_suspend(uintptr_t arg)
{
    unused(arg);
    return hal_gpio_ctrl(0, GPIO_CTRL_SUSPEND);
}

errcode_t uapi_gpio_resume(uintptr_t arg)
{
    unused(arg);
    return hal_gpio_ctrl(0, GPIO_CTRL_RESUME);
}
#endif

#if defined(CONFIG_GPIO_SELECT_CORE)
void uapi_gpio_select_core(pin_t pin, cores_t core)
{
    gpio_select_core(pin_t pin, cores_t core);
}
#endif