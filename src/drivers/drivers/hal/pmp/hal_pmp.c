/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides hal mpu \n
 *
 * History: \n
 * 2022-09-28, Create file. \n
 */
#include "common_def.h"
#include "hal_pmp.h"

hal_pmp_funcs_t *g_hal_pmp_funcs;

errcode_t hal_pmp_register_funcs(hal_pmp_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_pmp_funcs = funcs;
    return ERRCODE_SUCC;
}

hal_pmp_funcs_t *hal_pmp_get_funcs(void)
{
    return g_hal_pmp_funcs;
}

errcode_t hal_pmp_unregister_funcs(void)
{
    g_hal_pmp_funcs = NULL;
    return ERRCODE_SUCC;
}