/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2021-03-09
 */

#ifndef _BOOT_INIT_H_
#define _BOOT_INIT_H_

#include <stddef.h>

#define FLASH_HEAD_ADDR             0x200000
#define FLASH_MEM_SIZE              0x00400000

#define FLASH_START                 0x200000
#define FLASH_LEN                   0x800000 /* 8M */

#define KERNEL_START_ADDR           0x90110000

#define BOOT_HEAD_ADDR              0x200000
#define LOAD_BOOT_ADDR              0xA20000

#define BOOT_PORTING_RESET_REG      0x40002110
#define BOOT_PORTING_RESET_VALUE    0x4

#define RESET_COUNT_REG             0x40000008
#define STATUS_DEBUG_REG_ADDR       0x44000014  // gp reg0

#define HIBURN_CODELOADER_UART              UART_BUS_0
#define HIBURN_UART_TX                      CHIP_FIXED_TX_PIN   // 当前驱动没有用到，只有检查
#define HIBURN_UART_RX                      CHIP_FIXED_RX_PIN   // 当前驱动没有用到，只有检查
#define UART_ROMBOOT_INT_TIMEOUT_DEFAULT    32

#define FLASHBOOT_MAX_SIZE          0x10000
#define SRAM_START_ADDR             0x00100000  /* SRAM 起始地址 */
#define LOADY_MAX_ADDR              0x1000000   /* loady 命令可存储的最大地址(不包括该地址) */

#define RAM_MIN_ADDR                BOOT_RAM_MIN  /* SRAM 可设置的最小起始地址 */
#define RAM_MAX_ADDR                BOOT_RAM_MAX

#define LOAD_MAX_RAM_ADDR           (RAM_MAX_ADDR - 0x40)
#define LOAD_MIN_SIZE               0x0      /* 0x40(sha256 header len) + 0x20(hash len) + 0x10(tail len) */
#define LOAD_MAX_SIZE               0x1000000   /* 16M */

#define RESET_DELAY_MS              100

#define REPET_TIMES_2               2
#define REPET_TIMES_3               3
#define REBOOT_MAX_CNT              15
#define BOOT_STEP                   13

typedef union {
    struct {
        uint32_t fota_fix_app_cnt  : 4;     /* fota修复app计数使用 */
        uint32_t reset_cnt         : 4;     /* boot阶段软复位次数记录 */
        uint32_t resv              : 24;
    } bits;

    uint32_t u32;
} gp_reg1_union;

void start_fastboot(void);
void irq_handler(void);

#endif
