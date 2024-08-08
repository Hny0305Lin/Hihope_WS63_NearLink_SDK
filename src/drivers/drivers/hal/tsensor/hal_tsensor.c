/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides hal tsensor \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */
#include "common_def.h"
#include "hal_tsensor.h"

hal_tsensor_funcs_t *g_hal_tsensor_funcs = NULL;
uintptr_t g_hal_tsensor_regs = 0;

errcode_t hal_tsensor_regs_init(void)
{
    if (tsensor_port_base_addr_get() == 0) {
        return ERRCODE_TSENSOR_REG_ADDR_INVALID;
    }

    g_hal_tsensor_regs = tsensor_port_base_addr_get();

    return ERRCODE_SUCC;
}

void hal_tsensor_regs_deinit(void)
{
    g_hal_tsensor_regs = 0;
}

errcode_t hal_tsensor_register_funcs(hal_tsensor_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_tsensor_funcs = funcs;

    return ERRCODE_SUCC;
}

errcode_t hal_tsensor_unregister_funcs(void)
{
    g_hal_tsensor_funcs = NULL;
    return ERRCODE_SUCC;
}

hal_tsensor_funcs_t *hal_tsensor_get_funcs(void)
{
    return g_hal_tsensor_funcs;
}