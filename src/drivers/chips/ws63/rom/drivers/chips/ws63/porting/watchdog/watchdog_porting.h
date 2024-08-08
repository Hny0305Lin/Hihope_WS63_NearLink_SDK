/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides watchdog port template \n
 *
 * History: \n
 * 2022-07-25， Create file. \n
 */

#ifndef WATCHDOG_PORTING_H
#define WATCHDOG_PORTING_H

#include <stdint.h>

#define CONFIG_WDT_CLOCK    24000000 /* wdg的fpga时钟周期为24mhz */
#define WDT_CNT_WIDTH       32

typedef void (*hal_chip_watchdog_dump_callback_t)(void);
typedef enum {
    CHIP_WDT_TIMEOUT_2S = 2,  // WDT period is 2s.
    CHIP_WDT_TIMEOUT_4S = 4,
    CHIP_WDT_TIMEOUT_8S = 8,
    CHIP_WDT_TIMEOUT_16S = 16,
    CHIP_WDT_TIMEOUT_32S = 32,
    CHIP_WDT_TIMEOUT_64S = 64,
    CHIP_WDT_TIMEOUT_128S = 128,
    CHIP_WDT_TIMEOUT_256S = 256,
    CHIP_WDT_TIMEOUT_512S = 512,
    CHIP_WDT_TIMEOUT_1024S = 1024,
    CHIP_WDT_TIMEOUT_2048S = 2048,
    CHIP_WDT_TIMEOUT_4096S = 4096,
    CHIP_WDT_TIMEOUT_8192S = 8192,
    CHIP_WDT_TIMEOUT_16384S = 16384,
    CHIP_WDT_TIMEOUT_32768S = 32768,
    CHIP_WDT_TIMEOUT_MAX,
} chip_wdt_timeout_t;

typedef enum {
    CHIP_WDT_MODE_RESET = 0,  // Will reset core direcotry, when chip watchdog trigger.
    // Will enter interrupt, when chip watchdog trigger. If chip watchdog not kick in interrupt, core will reset.
    CHIP_WDT_MODE_INTERRUPT,
    CHIP_WDT_MODE_MAX,
} chip_wdt_mode_t;


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_watchdog Watchdog
 * @ingroup  drivers_port
 * @{
 */

/**
 * @if Eng
 * @brief  Base address for the IP.
 * @else
 * @brief  IP的基地址
 * @endif
 */
extern uintptr_t g_watchdog_base_addr;

/**
 * @if Eng
 * @brief  Register hal funcs objects into hal_watchdog module.
 * @else
 * @brief  将hal funcs对象注册到hal_watchdog模块中
 * @endif
 */
void watchdog_port_register_hal_funcs(void);

/**
 * @if Eng
 * @brief  Unregister hal funcs objects from hal_watchdog module.
 * @else
 * @brief  从hal_watchdog模块注销hal funcs对象
 * @endif
 */
void watchdog_port_unregister_hal_funcs(void);

/**
 * @if Eng
 * @brief  Register the interrupt of watchdog.
 * @else
 * @brief  注册看门狗中断
 * @endif
 */
void watchdog_port_register_irq(void);

/**
 * @if Eng
 * @brief  Handler of the watchdog interrupt request.
 * @else
 * @brief  看门狗中断请求的处理程序
 * @endif
 */
void irq_wdt_handler(void);

void watchdog_turnon_clk(void);

void watchdog_turnoff_clk(void);

uint32_t watchdog_get_conifgtime(void);

/**
 * @if Eng
 * @brief  Set the working clock of Watchdog.
 * @else
 * @brief  设置watchdog的工作时钟。
 * @endif
 */
void watchdog_port_set_clock(uint32_t wdt_clock);

/**
 * @if Eng
 * @brief  Get the working clock of Watchdog.
 * @else
 * @brief  获取watchdog的工作时钟。
 * @endif
 */
uint32_t watchdog_port_get_clock(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif