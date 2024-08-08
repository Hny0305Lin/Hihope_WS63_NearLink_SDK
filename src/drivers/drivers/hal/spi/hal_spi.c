/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL SPI \n
 *
 * History: \n
 * 2022-08-12, Create file. \n
 */
#include "common_def.h"
#include "hal_spi.h"

uintptr_t g_hal_spis_regs[SPI_BUS_MAX_NUM] = { 0 };

errcode_t hal_spi_regs_init(void)
{
    for (int32_t i = 0; i < SPI_BUS_MAX_NUM; i++) {
        if (spi_porting_base_addr_get(i) == 0) {
            return ERRCODE_UART_REG_ADDR_INVALID;
        }
        g_hal_spis_regs[i] = spi_porting_base_addr_get(i);
    }
    return ERRCODE_SUCC;
}