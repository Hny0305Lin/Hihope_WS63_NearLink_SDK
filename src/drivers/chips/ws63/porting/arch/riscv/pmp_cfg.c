/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  PMP DRIVER.
 */

#include <stdbool.h>
#include "memory_config_common.h"
#include "share_mem_config.h"
#include "common_def.h"
#include "partition.h"
#include "partition_resource_id.h"
#include "drv_pmp.h"
#include "pmp_cfg.h"

typedef enum {
    REGION_GAP_ROM_ITCM,
    REGION_DTCM,
    REGION_FLASH_1,       // gap & partition id: 0-7 & NV & crash info
    REGION_FLASH_2_APP,   // app image
    REGION_FLASH_3,       // fota data & reserve
#ifndef __NON_OS__
    REGION_RAM_1,         // wifi pkt ram & sram
    REGION_RAM_2,         // sram text
    REGION_RAM_3,         // sram
    REGION_RAM_4,         // radar
    REGION_RAM_5,         // sram other
    REGION_GAP_PKE_ROM,
    REGION_REG,
#endif
    REGION_MAX_NUM
} region_index_t;

static pmp_conf_t g_region_attr[REGION_MAX_NUM] = {
    {
        .idx = REGION_GAP_ROM_ITCM,
        .addr = (uint32_t)APP_DTCM_ORIGIN,
        .conf.rwx_permission = PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
#ifndef __NON_OS__
        .conf.lock = true,
#else
        // APP 运行时, 在 do_relocation 函数中有I/DTCM的写操作, 所以 SSB 中, 不能lock
        .conf.lock = false,
#endif
        .conf.pmp_attr = PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE,
    },
    {
        .idx = REGION_DTCM,
        .addr = (uint32_t)(APP_DTCM_ORIGIN + APP_DTCM_REGION_LEN),
        .conf.rwx_permission = PMPCFG_RW_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
#ifndef __NON_OS__
        .conf.lock = true,
#else
        // APP 运行时, 在 do_relocation 函数中有I/DTCM的写操作, 所以 SSB 中, 不能lock
        .conf.lock = false,
#endif
        .conf.pmp_attr = PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE,
    },
    {
        .idx = REGION_FLASH_1, // gap & partition id: 0-7 & NV & crash info
        .addr = (uint32_t)0x0, // 从分区表中获取App image的起始地址
        .conf.rwx_permission = PMPCFG_READ_ONLY_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
    },
    {
        .idx = REGION_FLASH_2_APP, // app image
        .addr = (uint32_t)0x0, // 从分区表中获取FOTA Data的起始地址
        .conf.rwx_permission = PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
    },
    {
        .idx = REGION_FLASH_3, // fota data & reserve
        .addr = (uint32_t)APP_SRAM_ORIGIN,
        .conf.rwx_permission = PMPCFG_READ_ONLY_NEXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
    },
#ifndef __NON_OS__
    {
        .idx = REGION_RAM_1, // wifi pkt ram
        .addr = (uint32_t)0x0, // __sram_text_begin__,
        .conf.rwx_permission = PMPCFG_RW_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
    },
    {
        .idx = REGION_RAM_2, // sram text
        .addr = (uint32_t)0x0, // &__sram_text_end__,
        .conf.rwx_permission = PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
    },
    {
        .idx = REGION_RAM_3, // sram text end --- radar start
        .addr = (uint32_t)RADAR_SENSOR_RX_MEM_START,
        .conf.rwx_permission = PMPCFG_RW_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
    },
    {
        .idx = REGION_RAM_4, // radar
        .addr = (uint32_t)RADAR_SENSOR_RX_MEM_END,
        .conf.rwx_permission = PMPCFG_RW_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE,
    },
    {
        .idx = REGION_RAM_5, // sram other
        .addr = (uint32_t)0xA98800,
        .conf.rwx_permission = PMPCFG_RW_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_WRITEBACK_RALLOCATE,
    },
    {
        .idx = REGION_GAP_PKE_ROM,
        .addr = (uint32_t)0xC01000,
        .conf.rwx_permission = PMPCFG_READ_ONLY_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE,
    },
    {
        .idx = REGION_REG,
        .addr = (uint32_t)0x4A001000,
        .conf.rwx_permission = PMPCFG_RW_EXECUTE,
        .conf.addr_match = PMPCFG_ADDR_MATCH_TOR,
        .conf.lock = true,
        .conf.pmp_attr = PMP_ATTR_DEVICE_NO_BUFFERABLE,
    }
#endif
};

STATIC void pmp_region_cfg(void)
{
    partition_information_t img_info = {0};
    uint32_t addr = 0;

    uapi_partition_get_info(PARTITION_APP_IMAGE, &img_info);
    addr = img_info.part_info.addr_info.addr + FLASH_START;
    g_region_attr[REGION_FLASH_1].addr = addr;

    uapi_partition_get_info(PARTITION_FOTA_DATA, &img_info);
    addr = img_info.part_info.addr_info.addr + FLASH_START;
    g_region_attr[REGION_FLASH_2_APP].addr = addr;

#ifndef __NON_OS__
    g_region_attr[REGION_RAM_1].addr = (uint32_t)&__sram_text_begin__;
    g_region_attr[REGION_RAM_2].addr = (uint32_t)&__sram_text_end__ & ~0x1F; // 0x1F: 32字节对齐
#endif
}

void pmp_enable(void)
{
    pmp_region_cfg();
#ifndef __NON_OS__
    uapi_pmp_config(&g_region_attr[0], 2); // 2: 前两个 region 的 lock 配置
    // flashboot 中已经 lock 的 region 不能重复配置
    uapi_pmp_config(&g_region_attr[5], REGION_MAX_NUM - 5); // 5: APP 中补上后半段的保护
#else
    uapi_pmp_config(g_region_attr, REGION_MAX_NUM);
#endif
}

