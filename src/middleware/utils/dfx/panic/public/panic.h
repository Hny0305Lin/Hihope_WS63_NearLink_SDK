/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  PANIC module interface
 * Author:
 * Create:
 */

#ifndef LIB_PANIC_PANIC_H
#define LIB_PANIC_PANIC_H

#include "std_def.h"
#include "stdint.h"

/**
 * @defgroup connectivity_libs_panic PANIC
 * @ingroup  connectivity_libs
 * @{
 */
typedef enum panic_id {
    // No panic
    PANIC_NONE = 0,
    PANIC_ARM_CORE = 1,
    PANIC_HAL = 2,
    PANIC_DSP = 3,
    PANIC_IRMALLOC_FAIL = 4,
    PANIC_UART = 5,
    PANIC_IFLASH = 6,
    PANIC_ADC = 7,
    PANIC_WATCHDOG = 8,
    PANIC_LOG = 9,
    PANIC_DMA = 10,
    PANIC_ASSERT = 11,
    PANIC_REBOOT = 12,
    PANIC_UPDATEPKG = 13,
    PANIC_I2C = 14,
    PANIC_SPI = 15,
    PANIC_RPC = 22,
    PANIC_MEM_MONITOR = 23,

    // Someone calling abort() / exit()
    PANIC_EXIT = 24,

    // RTOS related panics
    PANIC_TASK_CREATE_FAILED = 25,

    PANIC_MEMCPY_BUFFER_OVERLAP = 26,
    PANIC_XIP = 27,
    PANIC_BTC_MALLOC_FAIL = 28,
    PANIC_LOW_POWER = 29,
    PANIC_SYSTEM_STATUS = 30,
    PANIC_BTC_INT_ERR = 31,
    PANIC_LPC_WKUP_FAIL = 32,
    PANIC_CLOCKS_GLB_ERR = 33,
    PANIC_PMU_CMU = 34,
    PANIC_PMU_LDO = 35,
    PANIC_CHIP_WDT_FRST = 36,
    PANIC_NON_OS = 37,
    PANIC_LPC_VETO = 38,
    PANIC_TIMER = 39,
    PANIC_LPC = 40,
    PANIC_RTC = 41,
    PANIC_MEMORY = 42,
    PANIC_CPU_HIFI = 43,
    PANIC_EXCEPTION_TEST = 44,
    PANIC_KV = 45,
    PANIC_EFLASH = 46,
    PANIC_LIB = 47,
    PANIC_CODELOADER = 48,
    PANIC_CRITICLA = 49,
    PANIC_IPC = 50,
    PANIC_INT = 51,
    PANIC_OS = 52,
    PANIC_LPC_WAKEUP_TIME = 53,
    PANIC_BTC_BT_FRM_DISMATCH1 = 54,
    PANIC_BTC_BT_FRM_DISMATCH2 = 55,
    PANIC_BTC_BLE_FRM_DISMATCH = 56,
    PANIC_BTC_WAKEUP_TO = 57,
    PANIC_BTH_MEMORY = 58,
    PANIC_MPU_CONFIG_ERR = 59,
    PANIC_GPU = 60,
    PANIC_SDIO_DEVICE = 61,
    PANIC_IR = 62,
    PANIC_BTC_CMD_NUM_ZERO = 63,
    PANIC_CAP = 64,
    PANIC_EPMU = 65,
    PANIC_BTS_WRITE_PROPERTY_FAIL = 66,
    PANIC_MMC_HOST = 67,
    PANIC_BTC_OSCEN_TO = 68,
    PANIC_RGB888 = 70,
    PANIC_AP_COMMU = 71,
    PANIC_CPU_CLOCKS = 72,
    PANIC_CLOCKS_SWITCH = 73,
    PANIC_PMU = 74,
    PANIC_LOG_DUMP = 75,
    PANIC_COUL = 76,
    PANIC_DSP0_POWER = 77,
    PANIC_DSP1_POWER = 78,
    PANIC_BT_DEBUG = 79,
    PANIC_PIN_NULL = 80,
    PANIC_UNITTEST = 98,
    PANIC_TESTSUIT = 99,
    PANIC_DSS_MIPI_ERR = 100,
    PANIC_GNSS_DFR_ERR = 101,
    PANIC_BT_SEC_BOOT_ERR = 102,
    PANIC_ROM_WEAK_FUNCTION = 103,
    PANIC_GLP = 104,
    PANIC_UICC = 105,
    PANIC_CRYPTO = 106,
} panic_id_t;

typedef struct {
    panic_id_t origin;
    uint32_t code;
    uint32_t timestamp_ms;
    uint32_t caller;
} panic_desc_t;

/**
 * @brief  Panic callback
 * @param  none
 */
typedef void (*lib_panic_dump_callback)(void);

/**
 * @brief  hook function panic handler
 * @param  source the origin of the panic
 * @param  code panic origin specific information
 * @param  caller who call panic
 */
typedef void(*panic_deal_handler)(panic_id_t source, uint32_t code, uint32_t caller);

/**
 * @brief  Register panic deal callback
 * @param  callback: if system panic, the callback is called to handle the exception.
 */
void panic_register_deal_callback(panic_deal_handler callback);

/**
 * @brief  Ungister panic deal callback
 */
void panic_unregister_deal_callback(void);

/**
 * @brief  Register panic callback
 * @param  callback: if system panic, the callback is called to handle the exception.
 */
void register_panic_dump_callback(lib_panic_dump_callback callback);

/**
 * @brief  Panic callback
 * @param  str display which type leads to panic
 */
typedef void (*lib_panic_dump_callback_with_param)(char *str);

/**
 * @brief  Register panic callback
 * @param  callback: if system panic, the callback is called to handle the exception.
 */
void register_panic_dump_callback_with_param(lib_panic_dump_callback_with_param callback);

/** Initialise the panic module */
void panic_init(void);

/** De-initialise the panic module */
void panic_deinit(void);

/**
 * @brief  Get var already_panicking
 * @return already_panicking
 */
bool get_already_panicking(void);

/**
 * @brief  Enter into infinite loop so that a debugger can break in and see the stack trace
 * @param  source the origin of the panic
 * @param  code panic origin specific information
 * @note   This can be called before the panic_init is called to support panics during start-up and it will cope
 */
void panic(panic_id_t source, uint32_t code);


/** Wait forever */
void panic_wait_forever(void);

/**
 * @brief  Enter into infinite loop so that a debugger can break in and see the stack trace
 * @param  origin the origin of the panic
 * @param  code panic origin specific information
 * @param  caller who call panic.
 */
void panic_deal(panic_id_t origin, uint32_t code, uint32_t caller);

/**
 * @}
 */
#endif
