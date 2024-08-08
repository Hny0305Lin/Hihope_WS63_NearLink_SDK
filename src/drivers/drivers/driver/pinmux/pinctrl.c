/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pinctrl driver source \n
 *
 * History: \n
 * 2022-08-25, Create file. \n
 */
#include <stdbool.h>
#include "common_def.h"
#include "soc_osal.h"
#include "hal_pinctrl.h"
#include "pinctrl.h"

void uapi_pin_init(void)
{
    pin_port_register_hal_funcs();
}

void uapi_pin_deinit(void)
{
    pin_port_unregister_hal_funcs();
}

errcode_t uapi_pin_set_mode(pin_t pin, pin_mode_t mode)
{
    if (unlikely((pin >= PIN_MAX_NUMBER) || (mode >= PIN_MODE_MAX))) {
        return ERRCODE_PIN_INVALID_PARAMETER;
    }
    if (unlikely(pin_check_mode_is_valid(pin, mode) == false)) {
        return ERRCODE_PIN_MODE_NO_FUNC;
    }
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (unlikely((pin_funcs == NULL) || (pin_funcs->set_mode == NULL))) {
        return ERRCODE_PIN_NOT_INIT;
    }

    uint32_t irq_sts = osal_irq_lock();
    errcode_t ret = pin_funcs->set_mode(pin, mode);
    osal_irq_restore(irq_sts);

    return ret;
}

pin_mode_t uapi_pin_get_mode(pin_t pin)
{
    if (unlikely(pin >= PIN_MAX_NUMBER)) {
        return PIN_MODE_MAX;
    }
    pin_mode_t mode = PIN_MODE_MAX;
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (likely((pin_funcs != NULL) && (pin_funcs->get_mode != NULL))) {
        uint32_t irq_sts = osal_irq_lock();
        mode = pin_funcs->get_mode(pin);
        osal_irq_restore(irq_sts);
    }
    return mode;
}

errcode_t uapi_pin_set_ds(pin_t pin, pin_drive_strength_t ds)
{
    if (unlikely((pin >= PIN_MAX_NUMBER) || (ds >= PIN_DS_MAX))) {
        return ERRCODE_PIN_INVALID_PARAMETER;
    }
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (unlikely((pin_funcs == NULL) || (pin_funcs->set_ds == NULL))) {
        return ERRCODE_PIN_NOT_INIT;
    }

    uint32_t irq_sts = osal_irq_lock();
    errcode_t ret = pin_funcs->set_ds(pin, ds);
    osal_irq_restore(irq_sts);

    return ret;
}

pin_drive_strength_t uapi_pin_get_ds(pin_t pin)
{
    if (unlikely(pin >= PIN_MAX_NUMBER)) {
        return PIN_DS_MAX;
    }
    pin_drive_strength_t ds = PIN_DS_MAX;
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (likely((pin_funcs != NULL) && (pin_funcs->get_ds != NULL))) {
        uint32_t irq_sts = osal_irq_lock();
        ds = pin_funcs->get_ds(pin);
        osal_irq_restore(irq_sts);
    }
    return ds;
}

errcode_t uapi_pin_set_pull(pin_t pin, pin_pull_t pull_type)
{
    if (unlikely((pin >= PIN_MAX_NUMBER) || (pull_type >= PIN_PULL_MAX))) {
        return ERRCODE_PIN_INVALID_PARAMETER;
    }
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (unlikely((pin_funcs == NULL) || (pin_funcs->set_pull == NULL))) {
        return ERRCODE_PIN_NOT_INIT;
    }

    uint32_t irq_sts = osal_irq_lock();
    errcode_t ret = pin_funcs->set_pull(pin, pull_type);
    osal_irq_restore(irq_sts);

    return ret;
}

pin_pull_t uapi_pin_get_pull(pin_t pin)
{
    if (unlikely(pin >= PIN_MAX_NUMBER)) {
        return PIN_PULL_MAX;
    }
    pin_pull_t pull_type = PIN_PULL_MAX;
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (likely((pin_funcs != NULL) && (pin_funcs->get_pull != NULL))) {
        uint32_t irq_sts = osal_irq_lock();
        pull_type = pin_funcs->get_pull(pin);
        osal_irq_restore(irq_sts);
    }
    return pull_type;
}

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
errcode_t uapi_pin_set_ie(pin_t pin, pin_input_enable_t ie)
{
    if (unlikely((pin >= PIN_MAX_NUMBER) || (ie >= PIN_IE_MAX))) {
        return ERRCODE_PIN_INVALID_PARAMETER;
    }
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (unlikely((pin_funcs == NULL) || (pin_funcs->set_ie == NULL))) {
        return ERRCODE_PIN_NOT_INIT;
    }

    uint32_t irq_sts = osal_irq_lock();
    errcode_t ret = pin_funcs->set_ie(pin, ie);
    osal_irq_restore(irq_sts);

    return ret;
}

pin_input_enable_t uapi_pin_get_ie(pin_t pin)
{
    if (unlikely(pin >= PIN_MAX_NUMBER)) {
        return PIN_IE_MAX;
    }
    pin_input_enable_t ie = PIN_IE_MAX;
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (likely((pin_funcs != NULL) && (pin_funcs->get_ie != NULL))) {
        uint32_t irq_sts = osal_irq_lock();
        ie = pin_funcs->get_ie(pin);
        osal_irq_restore(irq_sts);
    }
    return ie;
}
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

#if defined(CONFIG_PINCTRL_SUPPORT_ST)
errcode_t uapi_pin_set_st(pin_t pin, pin_schmitt_trigger_t st)
{
    if (unlikely((pin >= PIN_MAX_NUMBER) || (st >= PIN_ST_MAX))) {
        return ERRCODE_PIN_INVALID_PARAMETER;
    }
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (unlikely((pin_funcs == NULL) || (pin_funcs->set_st == NULL))) {
        return ERRCODE_PIN_NOT_INIT;
    }

    uint32_t irq_sts = osal_irq_lock();
    errcode_t ret = pin_funcs->set_st(pin, st);
    osal_irq_restore(irq_sts);

    return ret;
}

pin_schmitt_trigger_t uapi_pin_get_st(pin_t pin)
{
    if (unlikely(pin >= PIN_MAX_NUMBER)) {
        return PIN_ST_MAX;
    }
    pin_schmitt_trigger_t st = PIN_ST_MAX;
    hal_pin_funcs_t *pin_funcs = hal_pin_get_funcs();
    if (likely((pin_funcs != NULL) && (pin_funcs->get_st != NULL))) {
        uint32_t irq_sts = osal_irq_lock();
        st = pin_funcs->get_st(pin);
        osal_irq_restore(irq_sts);
    }
    return st;
}
#endif /* CONFIG_PINCTRL_SUPPORT_ST */

#if defined(CONFIG_PINCTRL_SUPPORT_LPM)
errcode_t uapi_pin_suspend(uintptr_t arg)
{
    unused(arg);
    return ERRCODE_SUCC;
}

errcode_t uapi_pin_resume(uintptr_t arg)
{
    unused(arg);
    return ERRCODE_SUCC;
}
#endif  /* CONFIG_PINCTRL_SUPPORT_LPM */