/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_WATCHDOG_H
#define BOOT_WATCHDOG_H

void boot_wdt_init(void);
void boot_wdt_kick(void);
void boot_wdt_deinit(void);

#endif