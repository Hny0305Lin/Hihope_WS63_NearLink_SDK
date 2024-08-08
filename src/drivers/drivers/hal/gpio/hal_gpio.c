/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL gpio \n
 *
 * History: \n
 * 2022-07-26, Create file. \n
 */

#include <stdint.h>
#include "hal_gpio.h"

uintptr_t g_gpios_regs[GPIO_CHANNEL_MAX_NUM] = { 0 };

uintptr_t hal_gpio_base_addrs_get(uint32_t i)
{
    return gpio_porting_base_addr_get((gpio_channel_t)i);
}

void hal_gpio_regs_init(void)
{
    for (uint32_t i = 0; i < GPIO_CHANNEL_MAX_NUM; i++) {
        g_gpios_regs[i] = hal_gpio_base_addrs_get(i);
    }
}