/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_DELAY_H
#define BOOT_DELAY_H

#include <stdint.h>

void delay_init(void);
void delay_deinit(void);
void mdelay(uint32_t msecs);
void udelay(uint32_t usecs);

#endif
