/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  Default memory configurations
 *
 * Create:  2021-06-16
 */

#ifndef MEMORY_CONFIG_COMMON_H
#define MEMORY_CONFIG_COMMON_H

/**
 * @defgroup connectivity_config_memory MEMORY
 * @ingroup  connectivity_config
 * @{
 */

#if defined(CONFIG_RADAR_SENSOR_RX_MEM_8K)
#define RADAR_SENSOR_RX_MEM_SIZE    0x2000
#elif defined(CONFIG_RADAR_SENSOR_RX_MEM_16K)
#define RADAR_SENSOR_RX_MEM_SIZE    0x4000
#elif defined(CONFIG_RADAR_SENSOR_RX_MEM_24K)
#define RADAR_SENSOR_RX_MEM_SIZE    0x6000
#elif defined(CONFIG_RADAR_SENSOR_RX_MEM_32K)
#define RADAR_SENSOR_RX_MEM_SIZE    0x8000
#else
#define RADAR_SENSOR_RX_MEM_SIZE    0x2000  /* default 8K */
#endif

/* Bootrom config */
#define BOOTROM_START  0x100000
#define BOOTROM_LENGTH 0x9000           /* 36K */

/* ROM config */
#define ROM_START  0x109000
#define ROM_LENGTH 0x43000              /* 268K = 304K - 36K(bootrom) */

/* APP ITCM config */
#define APP_ITCM_ORIGIN     0x14C000
#define APP_ITCM_REGION_LEN 0x34000     /* 208K */

/* APP DTCM config */
#define APP_DTCM_ORIGIN 0x180000
#define APP_DTCM_REGION_LEN 0x48000     /* 288K */

#define PRESERVED_REGION_LENGTH (0x100)

/* APP share_ram config */
#define APP_SRAM_ORIGIN  0xA00000

#if defined(CONFIG_BGLE_RAM_SIZE_16K) && defined(WIFI_TCM_OPTIMIZE)
// pkt_ram 512K  itcm: 64K  dtcm: 16K  bgle: 16K
#define APP_ITCM_LENGTH 0x10000         /* 64K */
#define APP_DTCM_LENGTH 0x4000          /* 16K */
#define APP_SRAM_LENGTH (0x80000 - RADAR_SENSOR_RX_MEM_SIZE - PRESERVED_REGION_LENGTH)    /* 512K - RADAR - PRESERVED */

#elif defined(CONFIG_BGLE_RAM_SIZE_16K)
// pkt_ram 544K  itcm: 32K  dtcm: 16K  bgle: 16K
#define APP_ITCM_LENGTH 0x4000          /* 16K */
#define APP_DTCM_LENGTH 0x8000          /* 32K */
#define APP_SRAM_LENGTH (0x88000 - RADAR_SENSOR_RX_MEM_SIZE - PRESERVED_REGION_LENGTH)    /* 544K - RADAR - PRESERVED */

#elif defined(CONFIG_BGLE_RAM_SIZE_32K)
// pkt_ram 544K  itcm: 16K  dtcm: 16K  bgle: 32K
#define APP_ITCM_LENGTH 0x4000          /* 16K */
#define APP_DTCM_LENGTH 0x4000          /* 16K */
#define APP_SRAM_LENGTH (0x88000 - RADAR_SENSOR_RX_MEM_SIZE - PRESERVED_REGION_LENGTH)    /* 544K - RADAR - PRESERVED */

#elif defined(CONFIG_BGLE_RAM_SIZE_64K) // btc-only target
// pkt_ram 448K  itcm: 32K  dtcm: 64K  bgle: 64K
#define APP_ITCM_LENGTH 0x8000          /* 32K */
#define APP_DTCM_LENGTH 0x10000         /* 64K */
#define APP_SRAM_LENGTH (0x70000 - RADAR_SENSOR_RX_MEM_SIZE - PRESERVED_REGION_LENGTH)    /* 448K - RADAR - PRESERVED */

#elif defined(WIFI_TCM_OPTIMIZE) // wifi-only target
// pkt_ram 512K  itcm: 64K  dtcm: 32K  bgle: 0K
#define APP_ITCM_LENGTH 0x10000         /* 64K */
#define APP_DTCM_LENGTH 0x8000          /* 32K */
#define APP_SRAM_LENGTH (0x80000 - RADAR_SENSOR_RX_MEM_SIZE - PRESERVED_REGION_LENGTH)    /* 512K - RADAR - PRESERVED */

#else
// pkt_ram 576K  itcm: 16K  dtcm: 16K  bgle: 0K
#define APP_ITCM_LENGTH 0x4000          /* 16K */
#define APP_DTCM_LENGTH 0x4000          /* 16K */
#define APP_SRAM_LENGTH (0x90000 - RADAR_SENSOR_RX_MEM_SIZE - PRESERVED_REGION_LENGTH)    /* 576K - RADAR - PRESERVED */
#endif

#define PRESERVED_REGION_ORIGIN (APP_SRAM_ORIGIN + APP_SRAM_LENGTH)

#define RADAR_SENSOR_RX_MEM_START   (APP_SRAM_ORIGIN + APP_SRAM_LENGTH + PRESERVED_REGION_LENGTH)
#define RADAR_SENSOR_RX_MEM_END     (RADAR_SENSOR_RX_MEM_START + RADAR_SENSOR_RX_MEM_SIZE)

/* Flash config */
#define FLASH_START     0x200000
#define FLASH_LEN       0x800000        /* 8M */
#define FLASH_MAX_END   (FLASH_START + FLASH_LEN)

#define APP_PROGRAM_ORIGIN (0x230000 + 0x300)
#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
#define APP_PROGRAM_LENGTH (0x240000 - 0x300)
#else
#define APP_PROGRAM_LENGTH (0x200000 - 0x300)
#endif

#ifdef CONFIG_SUPPORT_HILINK_INDIE_UPGRADE
#define HILINK_SRAM_LENGTH          (0x16000 - 36 * 1024)
#define HILINK_STARTUP_LENGTH       0x6A000
#else
#define HILINK_SRAM_LENGTH          0x0
#define HILINK_STARTUP_LENGTH       0x0
#endif

#define HILINK_SRAM_ORIGIN          (APP_SRAM_ORIGIN + APP_SRAM_LENGTH - HILINK_SRAM_LENGTH)
#define HILINK_STARTUP_ORIGIN       (APP_PROGRAM_ORIGIN + APP_PROGRAM_LENGTH - HILINK_STARTUP_LENGTH)

#define HILINK_PROGRAM_ORIGIN       (HILINK_STARTUP_ORIGIN + 0x300)
#define HILINK_PROGRAM_LENGTH       (HILINK_STARTUP_LENGTH - 0x300)

/* ----------------------------------------------------------------------------------------------------------------- */
/* APP RAM defines
 * APP has base 512K ITCM (Instruction TCM) for code
 *              256K DTCM (Data TCM) for DATA.
 * APP ITCM                 VECTORS TABLE
 *                          RAM TEXT
 *
 * APP DTCM                 STACK
 *                          RAM
 */
/* 256K DTCM for APP core data */
/* stack for normal 8k */
#define APP_USER_STACK_BASEADDR APP_SRAM_ORIGIN
#define APP_USER_STACK_LEN      0x1000
#define APP_USER_STACK_LIMIT    (APP_USER_STACK_BASEADDR + APP_USER_STACK_LEN)

/* stack for irq 1k */
#define APP_IRQ_STACK_BASEADDR APP_USER_STACK_LIMIT
#define APP_IRQ_STACK_LEN      0x800
#define APP_IRQ_STACK_LIMIT    (APP_IRQ_STACK_BASEADDR + APP_IRQ_STACK_LEN)

/* stack for exception 1k */
#define APP_EXCP_STACK_BASEADDR APP_IRQ_STACK_LIMIT
#define APP_EXCP_STACK_LEN      0x800
#define APP_EXCP_STACK_LIMIT    (APP_EXCP_STACK_BASEADDR + APP_EXCP_STACK_LEN)

/* stack for nmi 1k */
#define APP_NMI_STACK_BASEADDR APP_EXCP_STACK_LIMIT
#define APP_NMI_STACK_LEN      0x400
#define APP_NMI_STACK_LIMIT    (APP_NMI_STACK_BASEADDR + APP_NMI_STACK_LEN)

#define APP_RAM_ORIGIN (APP_NMI_STACK_LIMIT)
#define APP_RAM_END    (APP_SRAM_ORIGIN + APP_SRAM_LENGTH)
#define APP_RAM_LENGTH (APP_RAM_END - APP_RAM_ORIGIN)

#define SHARED_MEM_START                    0
#define SHARED_MEM_LENGTH                   0
#define MCPU_TRACE_MEM_REGION_START         0
#define CPU_TRACE_MEM_REGION_LENGTH         0
#define BT_RAM_ORIGIN_APP_MAPPING           0
#define BT_RAM_ORIGIN_APP_MAPPING_LENGTH    0
#define BCPU_TRACE_MEM_REGION_START         0
#define CPU_TRACE_MEM_REGION_LENGTH         0

#ifdef CONFIG_MEMORY_CUSTOMIZE_RSV
#define APP_PROGRAM_MEM_RSV_ORIGIN 0x5FA000 // must be 4K aligned
#endif

/**
 * @}
 */
#endif