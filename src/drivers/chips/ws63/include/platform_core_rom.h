/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  WS63 Application Core Platform Definitions of romable
 *
 * Create:  2023-04-03
 */
#ifndef PLATFORM_CORE_ROM_H
#define PLATFORM_CORE_ROM_H

/**
 * @ingroup DRIVER_PLATFORM_CORE
 * @{
 */

// Timer reg base addr.
#define TIMER_BASE_ADDR                         0x44002000
#define TIMER_0_BASE_ADDR                       (TIMER_BASE_ADDR + 0x100)
#define TIMER_1_BASE_ADDR                       (TIMER_BASE_ADDR + 0x200)
#define TIMER_2_BASE_ADDR                       (TIMER_BASE_ADDR + 0x300)
#define TIMER_SYSCTL_BASE_ADDR                  (TIMER_BASE_ADDR + 0xA0)

#define TICK_TIMER_BASE_ADDR                    TIMER_0_BASE_ADDR

// Tcxo reg base addr
#define TCXO_COUNT_BASE_ADDR 0x440004C0

/**
 * @brief Definition of pin.
 */
typedef enum {
    GPIO_00 = 0,
    GPIO_01 = 1,
    GPIO_02 = 2,
    GPIO_03 = 3,
    GPIO_04 = 4,
    GPIO_05 = 5,
    GPIO_06 = 6,
    GPIO_07 = 7,
    GPIO_08 = 8,
    GPIO_09 = 9,
    GPIO_10 = 10,
    GPIO_11 = 11,
    GPIO_12 = 12,
    GPIO_13 = 13,
    GPIO_14 = 14,
    GPIO_15 = 15,
    GPIO_16 = 16,
    GPIO_17 = 17,
    GPIO_18 = 18,
    SFC_CLK = 19,
    SFC_CSN = 20,
    SFC_IO0 = 21,
    SFC_IO1 = 22,
    SFC_IO2 = 23,
    SFC_IO3 = 24,
    PIN_NONE = 25, // used as invalid/unused PIN number
} pin_t;

#define PIN_MAX_NUMBER PIN_NONE

// Watchdog reg base addr
#define CHIP_WDT_BASE_ADDRESS 0x40006000

/**
 * @}
 */

#endif