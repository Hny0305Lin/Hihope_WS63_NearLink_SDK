/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides V150 gpio register operation api \n
 *
 * History: \n
 * 2022-11-27, Create file. \n
 */

#include <stdint.h>
#include "common_def.h"
#include "hal_gpio_v150_regs_op.h"

void hal_gpio_v150_intr_rebase(uint32_t channel, uint32_t group)
{
    hal_gpio_gpio_int_en_disable_all(channel, group);
    hal_gpio_gpio_int_mask_mask_all(channel, group);
    hal_gpio_gpio_int_eoi_clr_all(channel, group);
}