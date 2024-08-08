/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides iot_watchdog driver source \n
 *
 * History: \n
 * 2023-10-18， Create file. \n
 */
#include "watchdog.h"
#include "iot_watchdog.h"

#define WATCHDOG_TRIGGER_MODE     WDT_MODE_INTERRUPT
#define WATCHDOG_TIMEOUT          1024

void IoTWatchDogEnable(void)
{
    uapi_watchdog_init(WATCHDOG_TIMEOUT);
    uapi_watchdog_enable(WATCHDOG_TRIGGER_MODE);
}

void IoTWatchDogKick(void)
{
    uapi_watchdog_kick();
}

void IoTWatchDogDisable(void)
{
    uapi_watchdog_disable();
}