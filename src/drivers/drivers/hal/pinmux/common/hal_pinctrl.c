/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL pinctrl \n
 *
 * History: \n
 * 2022-08-29, Create file. \n
 */

#include "common_def.h"
#include "hal_pinctrl.h"

hal_pin_funcs_t *g_hal_pins_funcs = NULL;

errcode_t hal_pin_register_funcs(hal_pin_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_pins_funcs = funcs;
    return ERRCODE_SUCC;
}

errcode_t hal_pin_unregister_funcs(void)
{
    g_hal_pins_funcs = NULL;
    return ERRCODE_SUCC;
}

hal_pin_funcs_t *hal_pin_get_funcs(void)
{
    return g_hal_pins_funcs;
}