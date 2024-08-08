/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: NON OS Utils
 * Author:
 * Create:  2018-10-15
 */

#include "product.h"
#include "non_os.h"
#include "core.h"
#ifdef SUPPORT_DFX_PANIC
#include "panic.h"
#endif
#include "chip_io.h"
#if FIXED_IN_ROM == YES
#include "core_cm3.h"
#else
#include "arch_barrier.h"
#endif
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
#include "arch_encoding.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if FIXED_IN_ROM == YES
#define int_enter_lock() __asm volatile(" cpsid i ")
#define int_exit_lock()  __asm volatile(" cpsie i ")

#define int_disable_fault_exception() __asm volatile(" cpsid f ")
#define int_enable_fault_exception()  __asm volatile(" cpsie f ")
#endif

#define FLASH_INIT_FLAG_REG 0x5700001c
#define NON_OS_BT_ROM_VERSION_LEN   16

static volatile uint64_t g_driver_init_bits = 0;

/** Variable used to store the nesting of critical sections.
 * If it reaches 0 interrupts are disabled, when it reaches 0 interrupts are enabled again.  */
static volatile uint16_t g_non_os_critical_nesting = 0;

static critical_statistic_handler g_cs_stat_handler = NULL;
static critical_record_handler g_cs_record_handler = NULL;

#if CRITICAL_INT_RESTORE == YES
static volatile uint32_t g_lock_stat = 0;
#endif

#if defined CHIP_LIBRA
static const char g_bt_rom_version[NON_OS_BT_ROM_VERSION_LEN] = "LIBRAV100";
#elif defined CHIP_SOCMN1
const char g_bt_rom_version[NON_OS_BT_ROM_VERSION_LEN] = "SOCMN1V100";
#elif defined CHIP_BRANDY
const char g_bt_rom_version[NON_OS_BT_ROM_VERSION_LEN] = "BRANDYV100";
#else
const char g_bt_rom_version[NON_OS_BT_ROM_VERSION_LEN] = "XXXXXXV100";
#endif

static void non_os_panic(uint32_t code)
{
#ifdef SUPPORT_DFX_PANIC
    panic(PANIC_CRITICLA, code);
#else
    UNUSED(code);
#endif
}
void non_os_register_critical_statistic(critical_statistic_handler handler)
{
    if (handler != NULL) {
        g_cs_stat_handler = handler;
    }
}

void non_os_unregister_critical_statistic(void)
{
    g_cs_stat_handler = NULL;
}

void non_os_register_critical_record(critical_record_handler handler)
{
    if (handler != NULL) {
        g_cs_record_handler = handler;
    }
}

void non_os_unregister_critical_record(void)
{
    g_cs_record_handler = NULL;
}

void non_os_enter_critical(void)
{
#if CRITICAL_INT_RESTORE == YES
    uint32_t lock_stat = get_int_status();
#endif
    int_enter_lock();  //lint !e40 !e718 !e746
    if (!g_non_os_critical_nesting && (g_cs_stat_handler != NULL)) {
#if defined(__GNUC__)
        g_cs_stat_handler(CRITICAL_ENTER, (uint32_t)(uintptr_t)__builtin_return_address(0));
#elif defined(__ICCARM__)
        g_cs_stat_handler(CRITICAL_ENTER, (uint32_t)__get_LR());
#endif
    }

    if (g_cs_record_handler != NULL) {
        uint32_t temp_lr = 0;
#if FIXED_IN_ROM == YES
        __asm volatile("MOV %0, LR" : "=r"(temp_lr));
#else
        get_temp_lr(temp_lr);
#endif
        g_cs_record_handler(CRITICAL_ENTER, temp_lr, g_non_os_critical_nesting);
    }

#if CRITICAL_INT_RESTORE == YES
    if (g_non_os_critical_nesting == 0) {
        g_lock_stat = lock_stat;
    }
#endif

    g_non_os_critical_nesting++;
    if (!g_non_os_critical_nesting) {  // panic we have not overflowed.
        non_os_panic(0x63);
        return;  //lint !e527  unreachable code
    }

#if FIXED_IN_ROM == YES
    __asm volatile("dsb");
    __asm volatile("isb");
#else
    dsb();
    isb();
#endif
}

void non_os_exit_critical(void)
{
    // panic if we are trying to exit critical when no enter critical has been called
    if (!g_non_os_critical_nesting) {
        non_os_panic(0x6f);
        return;  //lint !e527  unreachable code
    }
    g_non_os_critical_nesting--;

    if (g_cs_record_handler != NULL) {
        uint32_t temp_lr = 0;
#if FIXED_IN_ROM == YES
        __asm volatile("MOV %0, LR" : "=r"(temp_lr));
#else
        get_temp_lr(temp_lr);
#endif
        g_cs_record_handler(CRITICAL_EXIT, temp_lr, g_non_os_critical_nesting);
    }

    if (g_non_os_critical_nesting == 0) {
        if (g_cs_stat_handler != NULL) {
#if defined(__GNUC__)
            g_cs_stat_handler(CRITICAL_EXIT, (uint32_t)(uintptr_t)__builtin_return_address(0));
#elif defined(__ICCARM__)
            g_cs_stat_handler(CRITICAL_EXIT, (uint32_t)__get_LR());
#endif
        }
#if CRITICAL_INT_RESTORE == NO
        int_exit_lock();  //lint !e40 !e718 !e746
#else
        if (g_lock_stat == 0) { int_exit_lock(); }
#endif
        dsb();
        isb();
    }
}

void non_os_critical_section_init(void)
{
    int_enter_lock();  /*lint !e40*/
    g_non_os_critical_nesting = 0;
    int_exit_lock();  /*lint !e40*/
}

bool non_os_is_in_critical_section(void)
{
    return g_non_os_critical_nesting > 0;
}

bool non_os_is_driver_initialised(driver_init_bit_e driver)
{
    return ((g_driver_init_bits & (uint64_t)((uint64_t)0x01UL << (uint32_t)driver)) != 0);
}

void non_os_set_driver_initalised(driver_init_bit_e driver, bool value)
{
    non_os_enter_critical();
    uint64_t mask = (uint64_t)0x01UL << (uint32_t)driver;
    if (value) {
        g_driver_init_bits |= mask;
    } else {
        g_driver_init_bits &= (~mask);
    }
    non_os_exit_critical();
}

/**
* check aon flag is set
* flag save on aon only rst when all rst(not only cpu reset)
* @return true set already
*         false not set
*/
bool non_os_is_aon_flag_initialised(aon_init_bit_e driver)
{
    return (reg16_getbits(FLASH_INIT_FLAG_REG, (uint8_t)driver, 1) != 0) ? true : false;
}

/**
* set aon flag
* flag save on aon only rst when all rst(not only cpu reset)
* @return NULL
*/
void non_os_set_aon_flag_initalised(aon_init_bit_e driver, bool value)
{
    non_os_enter_critical();
    if (value) {
        reg16_setbit(FLASH_INIT_FLAG_REG, driver);
    } else {
        reg16_clrbit(FLASH_INIT_FLAG_REG, driver);
    }
    non_os_exit_critical();
}

const char *non_os_get_bt_rom_version(void)
{
    return g_bt_rom_version;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
