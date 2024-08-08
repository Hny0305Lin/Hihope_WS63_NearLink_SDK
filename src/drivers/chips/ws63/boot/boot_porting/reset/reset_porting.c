/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: reset
 *
 * Create: 2023-04-20
 */
#include "boot_init.h"
#include "watchdog.h"
#include "chip_io.h"
#include "boot_serial.h"
#include "tcxo.h"

static void switch_to_wfi(void)
{
    __asm__ __volatile__("wfi;");
}

static uint8_t get_reset_count(void)
{
    gp_reg1_union gp;
    gp.u32 = readl(RESET_COUNT_REG);
    return gp.bits.reset_cnt;
}

void set_reset_count(uint8_t cnt)
{
    gp_reg1_union gp;
    gp.u32 = readl(RESET_COUNT_REG);
    gp.bits.reset_cnt = cnt & 0xF;
    writel(RESET_COUNT_REG, gp.u32);
}

void update_reset_count(void)
{
    uint8_t reset_cnt = get_reset_count();
    boot_msg1("update_reset_count ", reset_cnt + 1);
    uapi_tcxo_delay_ms(RESET_DELAY_MS);
    if (reset_cnt < REBOOT_MAX_CNT) {
        set_reset_count(reset_cnt + 1);
        return;
    }
    uapi_watchdog_disable();
    switch_to_wfi();
    for (;;) {
    }
}
