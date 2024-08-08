/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  HAL Reboot functionality.
 *
 * Create:  2018-10-15
 */
#include "hal_reboot.h"
#include "soc_osal.h"
#include "core.h"
#include "non_os_reboot.h"
#include "debug_print.h"
#include "chip_io.h"
#include "reboot_porting.h"
#if (CORE == MASTER_BY_ALL) && (CORE != WIFI)
#ifdef SUPPORT_GPIO
#include "hal_gpio.h"
#endif
#include "preserve.h"
#endif

static uint16_t g_reset_cause = REBOOT_CAUSE_UNKNOWN;

void hal_reboot_set_ulp_aon_no_poweroff_flag(void)
{
    reg16_write(HAL_ULP_AON_GENERAL_REG, HAL_ULP_AON_NO_POWEROFF_FLAG);
}

bool hal_reboot_get_ulp_aon_no_poweroff_flag(void)
{
    if (reg16(HAL_ULP_AON_GENERAL_REG) == HAL_ULP_AON_NO_POWEROFF_FLAG) {
        return true;
    } else {
        return false;
    }
}

bool hal_reboot_hard_wdg_timeout(uint16_t cause)
{
    bool ret = false;
    uint16_t wdg_timeout_flag = 0;
    uint16_t i = 0;
    uint16_t stat = 0;
    wdg_timeout_flag = readw(HAL_PMU_PROTECT_STATUS_REG);
    if ((wdg_timeout_flag & BIT(HAL_PMU_PROTECT_CHIP_WDG_BIT)) != 0) {
        g_reset_cause = REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST;
        do {
            reg16_setbit(HAL_PMU_PROTECT_STATUS_CLR_REG, HAL_PMU_PROTECT_CHIP_WDG_BIT_CLR);
            stat = reg16_getbit(HAL_PMU_PROTECT_STATUS_REG, HAL_PMU_PROTECT_CHIP_WDG_BIT);
            i++;
        } while ((stat == 1) && (i < UINT16_MAX));
        if (stat == 1) {
            PRINT("PMU_PROTECT_STATUS_REG clear fail = %x\r\n", readl(HAL_PMU_PROTECT_STATUS_REG));
        }
        ret = true;
    } else {
        g_reset_cause = cause;
    }
    if (wdg_timeout_flag != 0) {
        PRINT("Chip boot err,"NEWLINE);
    }
    return ret;
}

void hal_reboot_deinit(void)
{
}

#if CORE == MASTER_BY_ALL && (CORE != WIFI)
__attribute__((weak)) void reboot_port_reboot_chip(void)
{
    regw_clrbit(HAL_CHIP_RESET_REG, HAL_CHIP_RESET_REG_OFFSET, HAL_CHIP_RESET_REG_ENABLE_RESET_BIT);
}
#endif

void hal_reboot_chip(void)
{
#if CORE == MASTER_BY_ALL && (CORE != WIFI)
    uint32_t irq_sts = osal_irq_lock();

    set_cpu_utils_system_boot_magic();
#ifdef SUPPORT_GPIO
    gpio_ulp_int_en(false);  // when soft reset, ulp area is not cleared.
#endif
    // Request the reset & wait for it...
    reboot_port_reboot_chip();
    for (;;) {}
#if defined(__GNUC__)
    osal_irq_restore(irq_sts);
#endif
#endif
}

void hal_reboot_clear_history(void)
{
#if (CORE == MASTER_BY_ALL) && (CORE != WIFI)
    writew(HAL_RESET_STS_CLEAR_REG, HAL_RESET_STS_CLEAR_ALL);
    reg16_setbit(HAL_PMU_PROTECT_STATUS_CLR_REG, HAL_PMU_PROTECT_CHIP_WDG_BIT_CLR);
#endif
}

/**
 * Return reset code for the last time this core was rebooted
 */
uint16_t hal_reboot_get_reset_reason(void)
{
    return g_reset_cause;
}
