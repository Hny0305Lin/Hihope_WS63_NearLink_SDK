/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: boot watchdog
 *
 * Create: 2023-01-09
 */

#include "watchdog.h"

void boot_wdt_kick(void)
{
    uapi_watchdog_kick();
}