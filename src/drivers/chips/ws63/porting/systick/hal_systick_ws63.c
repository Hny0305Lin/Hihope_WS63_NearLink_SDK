/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides ws63 HAL systick \n
 *
 * History: \n
 * 2023-04-06, Create file. \n
 */
#include <stdint.h>
#include "systick_porting.h"
#include "hal_systick_ws63.h"

void hal_systick_init(void)
{
    return;
}

void hal_systick_deinit(void)
{
    return;
}

errcode_t hal_systick_count_clear(void)
{
    return ERRCODE_SUCC;
}

STATIC uint64_t rtc_read_once(void)
{
    uint32_t low = (*(volatile uint32_t*)(uintptr_t)RTC_TIME_OUT_L);
    uint32_t high = (*(volatile uint32_t*)(uintptr_t)(RTC_TIME_OUT_H));
    return low + (((uint64_t)((high & 0xFFFF))) << 32);      // H Move right 32 bit
}

STATIC uint64_t rtc_read_ts(void)
{
    uint64_t ts1 = rtc_read_once();
    uint64_t ts2 = rtc_read_once();

    return (ts1 == ts2) ? ts1 : ts2;
}

uint64_t hal_systick_get_count(void)
{
    return rtc_read_ts();
}