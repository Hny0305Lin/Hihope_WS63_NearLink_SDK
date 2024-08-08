/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  NON OS Utils Header
 * Author:
 * Create:
 */

#ifndef NON_OS_H
#define NON_OS_H

#include "core.h"
#include "memory_config.h"

/**
 * @defgroup connectivity_drivers_non_os_utils UTILS
 * @ingroup  connectivity_drivers_non_os
 * @{
 */
#define THREAD_MODE_PRIORITY 4

#ifndef __return_address
#if defined(__GNUC__)
#define __return_address() ((uintptr_t)__builtin_return_address(0))
#elif defined(__ICCARM__)
#define __return_address() ((uintptr_t)__get_LR())
#else
#define __return_address() ((uintptr_t)0)
#endif
#endif

typedef enum {
    // Shared
    DRIVER_INIT_WATCHDOG = 0,
    DRIVER_INIT_RTC,
    DRIVER_INIT_TIMER,
    DRIVER_INIT_GPIO,
    DRIVER_INIT_IPC,
    DRIVER_INIT_SYS_STATUS,
    DRIVER_INIT_IPC_FLASH,
    DRIVER_INIT_UART,
    DRIVER_INIT_REBOOT,
    DRIVER_INIT_LPC,
    DRIVER_INIT_SPI,
    DRIVER_INIT_QSPI,
    DRIVER_INIT_PWM,
    DRIVER_INIT_DMA,
    DRIVER_INIT_TSENSOR,
    DRIVER_INIT_ADC,

    // BT only
    DRIVER_INIT_IFLASH,
    DRIVER_INIT_IFLASH_CONFIG,
    DRIVER_INIT_FLASH,
    DRIVER_INIT_MEMORY,
    DRIVER_INIT_OTP,
    DRIVER_INIT_IPC_FLASH_BT,
    DRIVER_INIT_PMU,
    DRIVER_INIT_LOG_MEMORY,
    DRIVER_INIT_KV,
    DRIVER_INIT_TCXO,
    DRIVER_INIT_PLL,
    DRIVER_INIT_SEC_TRNG,
    DRIVER_INIT_PATCH,
    DRIVER_INTI_AP_COMMU,
    // HIFI Only
    // APPS ONLY
    DRIVER_INIT_I2C,
    DRIVER_INIT_CAP_SENSOR,
    DRIVER_INIT_PSRAM,
    // CAPACTIVE TOUCH
    // Temperature
    // Comparator
    DRIVER_INIT_MX25U32,
    DRIVER_INIT_SEC_KEY_DRIV,
    DRIVER_INIT_SEC_TRNG1,
    DRIVER_INIT_SEC_SHA,
    DRIVER_INIT_SEC_AES,
    DRIVER_INIT_SEC_RSA,
    DRIVER_INIT_SEC_TRNG2,
    DRIVER_INIT_CLOCKS_CORE,
    DRIVER_INIT_FLASH_TO_QSPI,
    DRIVER_INIT_RGB888,
    DRIVER_INIT_UICC,
    DRIVER_INIT_MAX_VALUE,
} driver_init_bit_e;

/**
* flag save on aon only rst when all rst(not only cpu reset)
*/
typedef enum AON_INIT_BIT_E {
    AON_FLASH_QSPI = 0,
    AON_XIP = 1,
    AON_DEBUG_MODE = 2,
    AON_ROMLOADER_DOWNLOAD_KEY_MODE = 3,
    AON_FLASH_BACKUP_MODE = 4,
} aon_init_bit_e;

cassert(DRIVER_INIT_MAX_VALUE <= 64, NON_OS_H_); //lint !e506 !e514 !e778

/**
 * @brief  flag mark the mode if enter or exit critical when call critical statistic hook function
 */
typedef enum {
    CRITICAL_ENTER = 0,
    CRITICAL_EXIT = 1,
} critical_statistic_mode_e;

typedef enum {
    NMI_GWDT = 0,
    NMI_CWDT = 1,
    NMI_BWDT = 2,
    NMI_XIP_CTRL = 2,
    NMI_XIP_CACHE = 3,
    NMI_MDMA = 4,
    NMI_SMDMA = 5,
    NMI_NONE = 6,
} nmi_types_e;

typedef enum {
    BNMI_BT_INT = 0,
    BNMI_BLE_INT = 1,
    BNMI_BWDT = 2,
} bnmi_types_e;

#define NON_OS_NMI_CWDT_INT_FLAG      BIT(NMI_CWDT)
#define NON_OS_NMI_XIP_CTRL_INT_FLAG  BIT(NMI_XIP_CTRL)
#define NON_OS_NMI_XIP_CACHE_INT_FLAG BIT(NMI_XIP_CACHE)
#define NON_OS_NMI_MDMA_INT_FLAG      BIT(NMI_MDMA)
#define NON_OS_NMI_SMDMAINT_FLAG      BIT(NMI_SMDMA)
#define NON_OS_BT_NMI_MASK_REG        0x59000300
#define NON_OS_BT_NMI_STATUS_REG      0x59000304
#define NON_OS_BNMI_BT_INT_FLAG       BIT(BNMI_BT_INT)
#define NON_OS_BNMI_BLE_INT_FLAG      BIT(BNMI_BLE_INT)
#define NON_OS_BNMI_BWDT_INT_FLAG     BIT(BNMI_BWDT)

/**
 * @brief  hook function type for crititcal interval statisitic tool
 * @param  mode Mark the mode if enter or exit critical
 * @param  address Record LR address for locating which function entered critical
 */
typedef void(*critical_statistic_handler)(critical_statistic_mode_e mode, uint32_t address);

/**
 * @brief  hook function type for crititcal interval record tool
 * @param  mode Mark the mode if enter or exit critical
 * @param  address Record LR address for locating which function entered critical
 * @param  nestings the nesting number of enter critical.
 */
typedef void(*critical_record_handler)(critical_statistic_mode_e mode, uint32_t address, uint16_t nestings);

/**
 * @brief  Enter in a critical section.
 */
void non_os_enter_critical(void);

/**
 * @brief  Exit a critical section.
 */
void non_os_exit_critical(void);

/**
 * @brief  Checks if the system is running on a critical section.
 * @return true if the system is running in a critical section.
 */
bool non_os_is_in_critical_section(void);

/**
 * @brief  Ensures interrupts are enabled and the interrupt nested counter is reset.
 */
void non_os_critical_section_init(void);

/**
 * @brief  Gets the value of the drivers flag bit, then converts into a bool
 * @param  driver The driver that you wish to check is initialised
 * @return True if it has
 */
bool non_os_is_driver_initialised(driver_init_bit_e driver);

/**
 * @brief  Sets the value of the specified bit
 * @param  driver The driver whose bit you wish to set
 * @param  value The value to set the bit to
 */
void non_os_set_driver_initalised(driver_init_bit_e driver, bool value);

/**
 * @brief  check aon flag is set
 * flag save on aon only rst when all rst(not only cpu reset)
 * @return true set already
 *         false not set
*/
bool non_os_is_aon_flag_initialised(aon_init_bit_e driver);

/**
 * @brief  set aon flag
 * flag save on aon only rst when all rst(not only cpu reset)
 * @return NULL
 */
void non_os_set_aon_flag_initalised(aon_init_bit_e driver, bool value);

/**
 * @brief  register critical statistic hook function
 * @param  handler Function pointer of critical statistic
 */
void non_os_register_critical_statistic(critical_statistic_handler handler);

/**
 * @brief  unregister critical statistic hook function
 */
void non_os_unregister_critical_statistic(void);

/**
 * @brief  register critical record hook function
 * @param  handler Function pointer of critical record
 */
void non_os_register_critical_record(critical_record_handler handler);

/**
 * @brief  unregister critical record hook function
 */
void non_os_unregister_critical_record(void);

/**
 * @brief  Config the nmi.
 * @param  nmi Types of the nmi.
 * @param  on True for on, false for off.
 */
void non_os_nmi_config(nmi_types_e nmi, bool on);

/**
 * @brief  Get NMI interrupt status after mask.
 */
uint16_t non_os_get_nmi_mask_status(void);

/**
 * @brief  get NMI raw interrupt status.
 */
uint16_t non_os_get_nmi_raw_status(void);

/**
 * @brief  get NMI mask interrupt type.
 */
nmi_types_e non_os_get_nmi_type(void);

/**
 * @brief  get bt rom version.
 */
const char* non_os_get_bt_rom_version(void);

/**
 * @brief  Checks if the DCache is enabled.
 * @return true if the DCache is enabled.
 */
#if CORE == APPS
bool non_os_is_dcache_enabled(void);
#endif

/**
 * @}
 */
#endif