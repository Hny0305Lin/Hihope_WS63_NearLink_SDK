/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides watchdog port template \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include "hal_watchdog_v151.h"
#include "watchdog_porting.h"
#include "watchdog.h"
#include "tcxo.h"
#include "platform_core_rom.h"
#include "chip_io.h"
#include "soc_osal.h"

uintptr_t g_watchdog_base_addr = (uintptr_t)CHIP_WDT_BASE_ADDRESS;
static uint32_t g_watchdog_clock = CONFIG_WDT_CLOCK;

void watchdog_port_register_hal_funcs(void)
{
    hal_watchdog_register_funcs(hal_watchdog_v151_funcs_get());
}

void watchdog_port_unregister_hal_funcs(void)
{
    hal_watchdog_unregister_funcs();
}

void watchdog_port_register_irq(void)
{
    return;
}

void watchdog_turnon_clk(void)
{
    return;
}

void watchdog_turnoff_clk(void)
{
    return;
}

void watchdog_port_set_clock(uint32_t wdt_clock)
{
    uint32_t irq_sts = osal_irq_lock();
    g_watchdog_clock = wdt_clock;
    osal_irq_restore(irq_sts);
}

uint32_t watchdog_port_get_clock(void)
{
    return g_watchdog_clock;
}

void irq_wdt_handler(void)
{
    hal_watchdog_v151_irq_handler(0);
}