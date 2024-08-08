/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sio register operation api \n
 *
 * History: \n
 * 2022-08-01, Create file. \n
 */
#include <stdint.h>
#include "common_def.h"
#include "hal_sio_v150_regs_def.h"
#include "hal_sio_v150_regs_op.h"

#pragma weak hal_sio_set_tx_enable = hal_sio_v150_ct_set_set_tx_enable
void hal_sio_v150_ct_set_set_tx_enable(sio_bus_t bus, uint32_t val)
{
    if (val == 0) {
        sio_v150_ct_clr_data_t ct_clr;
        ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
        ct_clr.b.tx_enable = 1;
        sios_v150_regs(bus)->ct_clr = ct_clr.d32;
    } else {
        sio_v150_ct_set_data_t ct_set;
        ct_set.d32 = sios_v150_regs(bus)->ct_set;
        ct_set.b.tx_enable = val;
        sios_v150_regs(bus)->ct_set = ct_set.d32;
    }
}

#pragma weak hal_sio_set_rx_enable = hal_sio_v150_ct_set_set_rx_enable
void hal_sio_v150_ct_set_set_rx_enable(sio_bus_t bus, uint32_t val)
{
    if (val == 0) {
        sio_v150_ct_clr_data_t ct_clr;
        ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
        ct_clr.b.rx_enable = 1;
        sios_v150_regs(bus)->ct_clr = ct_clr.d32;
    } else {
        sio_v150_ct_set_data_t ct_set;
        ct_set.d32 = sios_v150_regs(bus)->ct_set;
        ct_set.b.rx_enable = val;
        sios_v150_regs(bus)->ct_set = ct_set.d32;
    }
}