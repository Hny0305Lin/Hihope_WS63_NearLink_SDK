/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V150 i2c register operation api \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */

#include <stdint.h>
#include "common_def.h"
#include "i2c_porting.h"
#include "hal_i2c_v150_regs_op.h"

i2c_regs_t *g_i2c_regs[I2C_BUS_MAX_NUM] = { 0 };

static inline uintptr_t hal_i2c_base_addrs_get(i2c_bus_t bus)
{
    return i2c_porting_base_addr_get(bus);
}

void hal_i2c_v150_regs_init(i2c_bus_t bus)
{
    g_i2c_regs[bus] = (i2c_regs_t *)hal_i2c_base_addrs_get(bus);
}

void hal_i2c_v150_regs_deinit(i2c_bus_t bus)
{
    g_i2c_regs[bus] = NULL;
}

void hal_i2c_v150_reset_all_regs(i2c_bus_t bus)
{
    hal_i2c_v150_clear_all_int(bus);
    g_i2c_regs[bus]->i2c_ctrl = I2C_CTRL_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_com = I2C_COM_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_icr = I2C_ICR_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_scl_h = I2C_COM_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_scl_l = I2C_SCL_L_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_txr = I2C_TXR_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_txcount = I2C_TXCOUNT_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_rxcount = I2C_RXCOUNT_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_rxtide = I2C_RXTIDE_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_txtide = I2C_TXTIDE_DEFAULT_VAL;
    g_i2c_regs[bus]->i2c_ftrper = I2C_FTRPER_DEFAULT_VAL;
}

