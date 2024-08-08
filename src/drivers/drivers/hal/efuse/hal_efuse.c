/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides efuse driver source \n
 *
 * History: \n
 * 2022-10-20, Create file. \n
 */
#include <stdio.h>
#include "hal_efuse.h"

hal_efuse_funcs_t *g_hal_efuses_funcs = NULL;

errcode_t hal_efuse_register_funcs(hal_efuse_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_efuses_funcs = funcs;
    return ERRCODE_SUCC;
}

errcode_t hal_efuse_unregister_funcs(void)
{
    g_hal_efuses_funcs = NULL;
    return ERRCODE_SUCC;
}

hal_efuse_funcs_t *hal_efuse_get_funcs(void)
{
    return g_hal_efuses_funcs;
}
