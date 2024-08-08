/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  Default memory configurations
 *
 * Create: 2021-03-09
 */
#ifndef MEMORY_CONFIG_H
#define MEMORY_CONFIG_H

/*
 * ********************* ROM ALLOCATION ***********************
 *
 * Used solely by the Security Core ROM image built into the chip.
 */
/* 32K ROM */
#define ROM_START  0x100000
#define ROM_LENGTH 0x4C000

#define ROMBOOT_ORIGIN  (ROM_START)
#define ROMBOOT_LENGTH  0x8000
#define ROMBOOT_END     (ROMBOOT_ORIGIN + ROMBOOT_LENGTH)
/*
 * ********************* FLASH ALLOCATION *********************
 * Flash is shared between the cores, and also handles a limited number of
 * non-volatile storage areas.
 */
#define FLASH_START     0x200000
#define FLASH_LEN       0x800000
#define FLASH_MAX_END   (FLASH_START + FLASH_LEN)
#define FLASH_PAGE_SIZE 4096

/* ----------------------------------------------------------------------------------------------------------------- */
#define APP_SHARE_RAM_ORIGIN 0xA00000
#define APP_SHARE_RAM_LENGTH 0x20000

/* reserve 128KB ram for romboot */
#define ROMBOOT_RAM_START APP_SHARE_RAM_ORIGIN
#define ROMBOOT_RAM_LEN  0x20000
#define ROMBOOT_RAM_END  (ROMBOOT_RAM_START + ROMBOOT_RAM_LEN) // 0xA20000 hiburn loaderboot address

#define BOOT_HEAD_LEN   0x300

#define SSB_ORIGIN   ROMBOOT_RAM_END
#define SSB_LENGTH   0x8000
#define SSB_END      (SSB_ORIGIN + SSB_LENGTH) // 0xA28000

#define FLASHBOOT_ORIGIN  (SSB_END + BOOT_HEAD_LEN)
#define FLASHBOOT_LENGTH  0x10000
#define FLASHBOOT_END     (FLASHBOOT_ORIGIN + FLASHBOOT_LENGTH)

/* MCU DTCM config currently use share ram */
#define MCU_SHARE_RAM_END    (APP_SHARE_RAM_ORIGIN + APP_SHARE_RAM_LENGTH)

/* MCU DTCM config currently use share ram */
#define ROMBOOT_USE_RAM_OFFSET 0x3100
#define APP_SRAM_ORIGIN (APP_SHARE_RAM_ORIGIN + ROMBOOT_USE_RAM_OFFSET)
#define APP_SRAM_LENGTH APP_SHARE_RAM_LENGTH
#define MCU_DTCM_END    (APP_SRAM_ORIGIN + APP_SRAM_LENGTH)

/* stack for normal 7k */
#define USER_STACK_BASEADDR APP_SRAM_ORIGIN
#define USER_STACK_LEN      0x1c00
#define USER_STACK_LIMIT    (USER_STACK_BASEADDR + USER_STACK_LEN)

/* stack for irq 1k */
#define IRQ_STACK_BASEADDR USER_STACK_LIMIT
#define IRQ_STACK_LEN      0x400
#define IRQ_STACK_LIMIT    (IRQ_STACK_BASEADDR + IRQ_STACK_LEN)

/* stack for exception 1k */
#define EXCP_STACK_BASEADDR IRQ_STACK_LIMIT
#define EXCP_STACK_LEN      0x400
#define EXCP_STACK_LIMIT    (EXCP_STACK_BASEADDR + EXCP_STACK_LEN)

#define FLASH_STACK_LEN       (USER_STACK_LEN + IRQ_STACK_LEN + EXCP_STACK_LEN)

#endif
