/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides hal sio \n
 *
 * History: \n
 * 2022-08-01, Create file. \n
 */
#include "common_def.h"
#include "hal_sio.h"

uintptr_t g_hal_sio_regs[I2S_MAX_NUMBER] = { 0 };
hal_sio_funcs_t *g_hal_sio_funcs[I2S_MAX_NUMBER] = { 0 };

errcode_t hal_sio_regs_init(sio_bus_t bus)
{
    if (sio_porting_base_addr_get(bus) == 0) {
        return ERRCODE_SIO_REG_ADDR_INVALID;
    }
    g_hal_sio_regs[bus] = sio_porting_base_addr_get(bus);
    return ERRCODE_SUCC;
}

void hal_sio_regs_deinit(sio_bus_t bus)
{
    g_hal_sio_regs[bus] = 0;
}

errcode_t hal_sio_register_funcs(sio_bus_t bus, hal_sio_funcs_t *funcs)
{
    if (bus >= I2S_MAX_NUMBER || funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_sio_funcs[bus] = funcs;
    return ERRCODE_SUCC;
}

errcode_t hal_sio_unregister_funcs(sio_bus_t bus)
{
    if (bus >= I2S_MAX_NUMBER) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_sio_funcs[bus] = NULL;
    return ERRCODE_SUCC;
}

hal_sio_funcs_t *hal_sio_get_funcs(sio_bus_t bus)
{
    if (bus >= I2S_MAX_NUMBER) {
        return NULL;
    }

    return g_hal_sio_funcs[bus];
}
