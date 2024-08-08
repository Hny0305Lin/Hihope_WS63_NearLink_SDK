/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL adc \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "common_def.h"
#include "hal_adc.h"

hal_adc_funcs_t *g_hal_adc_funcs = NULL;

errcode_t hal_adc_register_funcs(hal_adc_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_adc_funcs = funcs;

    return ERRCODE_SUCC;
}

errcode_t hal_adc_unregister_funcs(void)
{
    g_hal_adc_funcs = NULL;
    return ERRCODE_SUCC;
}

hal_adc_funcs_t *hal_adc_get_funcs(void)
{
    return g_hal_adc_funcs;
}