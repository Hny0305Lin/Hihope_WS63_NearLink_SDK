/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pwm port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "chip_io.h"
#include "tcxo.h"
#include "watchdog.h"
#ifdef CONFIG_MEMORY_CUSTOMIZE_RSV
#include "share_mem_config.h"
#include "memory_config_common.h"
#include "securec.h"
#include "sfc.h"
#include "debug_print.h"
#endif
#include "platform_core.h"
#include "reboot_porting.h"

#define REG_SYS_RST_RECORD  0x40000098
#define SYS_HARD_RST    (0x1 << 2)
#define SYS_SOFT_RST    (0x1 << 1)
#define SYS_WDT_RST     (0x1 << 0)

#ifdef CONFIG_MEMORY_CUSTOMIZE_RSV
#define MEM_RSV_MAX_LEN 0x100   // 256 Bytes

#define BYTE_4K_MASK 0xFFF
#define BYTE_4K 0x1000
#endif

#ifdef CONFIG_MEMORY_CUSTOMIZE_RSV
void reboot_mem_flash_erase(void)
{
    uint32_t load_addr = (uint32_t)mem_rsv_load_addr_get();
    uint32_t start_sector = (load_addr - FLASH_START) & ~BYTE_4K_MASK; // 地址向前4K对齐
    uint32_t mem_size = (uint32_t)mem_rsv_size_get();
    if (mem_size > BYTE_4K) {
        PRINT("mem rsv size err!\r\n");
        return;
    }

    uint8_t *buffer = (uint8_t *)malloc(mem_size);
    if (buffer == NULL) {
        return;
    }
    (void)memset_s((void *)buffer, mem_size, 0, mem_size);

    uapi_sfc_reg_read(start_sector, buffer, mem_size);
    for (uint32_t i = 0; i < mem_size; i++) {
        if (buffer[i] != 0xFF) {
            uapi_sfc_reg_erase(start_sector, BYTE_4K);
            break;
        }
    }

    free(buffer);
    return;
}

void reboot_mem_save_to_flash(void)
{
    uint32_t load_addr = (uint32_t)mem_rsv_load_addr_get();
    uintptr_t start_addr = mem_rsv_start_addr_get();
    uint32_t mem_size = (uint32_t)mem_rsv_size_get();
    if (mem_size > BYTE_4K) {
        PRINT("mem rsv size err!\r\n");
        return;
    }

    if (uapi_sfc_reg_write(load_addr - FLASH_START, (uint8_t *)start_addr, mem_size) != ERRCODE_SUCC) {
        PRINT("mem save failed!\r\n");
    }

    return;
}
#endif

void reboot_port_reboot_chip(void)
{
#ifdef CONFIG_MEMORY_CUSTOMIZE_RSV
    reboot_mem_save_to_flash();
#endif
    uapi_watchdog_deinit();

    uapi_tcxo_delay_us(3000); /* Wait for 3000 μs until the AT_RESPONSE_OK message is displayed. */
    reg32_setbit(HAL_CHIP_RESET_REG, HAL_CHIP_RESET_REG_ENABLE_RESET_BIT);
}

uint32_t reboot_port_get_rst_reason(void)
{
#define GLB_CTL_RB        0x40000000
#define SYS_RST_RECORD_0  0x00A0
#define SYS_DIAG_CLR_1    0x00A4
#define SYS_WDT_RST_HIS    0x1
#define SYS_SOFT_RST_HIS   0x2
#define POR_RST_FILTER_HIS 0x8

    uint32_t val;
    uint32_t rst_reason = RST_REASON_INVALID;
    uint32_t sys_diag_clr = 0;

    val = readl(GLB_CTL_RB + SYS_RST_RECORD_0);
    if ((val & SYS_WDT_RST_HIS) == SYS_WDT_RST_HIS) {
        rst_reason = RST_REASON_WDT_RST;
        sys_diag_clr = SYS_WDT_RST_HIS;
    } else if ((val & SYS_SOFT_RST_HIS) == SYS_SOFT_RST_HIS) {
        rst_reason = RST_REASON_SOFT_RST;
        sys_diag_clr = SYS_SOFT_RST_HIS;
    } else if ((val & POR_RST_FILTER_HIS) == POR_RST_FILTER_HIS) {
        rst_reason = RST_REASON_HARD_RST;
        sys_diag_clr = POR_RST_FILTER_HIS;
    }

    if (sys_diag_clr != 0) {
        writel(GLB_CTL_RB + SYS_DIAG_CLR_1, sys_diag_clr);
    }
    return rst_reason;
}
