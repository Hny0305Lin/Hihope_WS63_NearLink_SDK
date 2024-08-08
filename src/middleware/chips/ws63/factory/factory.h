/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: UPG ab mode config file for product.
 */
#ifndef FACTORY_H
#define FACTORY_H

#include <stdbool.h>
#include "partition.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define FLASH_PAGE_SIZE_BIT_LENGTH 12
#define MFG_FACTORY_DEFAULT_MODE 1
#define MFG_DEFAULT_REGION 0
#define FACTORY_INFO_OFFSET 2
#define BYTE_4K 0x1000
#define BYTE_4K_MASK 0xFFF
#define MFG_FACTORY_REGION_CONFIG_SIZE 0x1000
#define MFG_FACTORY_CONFIG_CHECK 0x8F45BA2F
#define FLASH_MAPPED_END 0x400000
#define FACTORYBOOT_KEY_AREA_IMAGE_ID 0x4B7CF333
#define MFG_FACTORY_VALID 0x0
#define MFG_FACTORY_INVALID 0x1
#define FLASH_PRIVATE_CUSTOM_CONFIG_TIMES 4
#define FLASH_PRIVATE_CUSTOM_CONFIG_OFFSET 0x200
#define IMAGE_HEADER_LEN ((KEY_AREA_STRUCTURE_LENGTH) + (CODE_INFO_STRUCTURE_LENGTH))

typedef enum  {
    FTM_REGION_SERVICE,
    FTM_REGION_FACTORY,
    FTM_REGION_COUNT
} mfg_region_index;

typedef struct {
    uint32_t factory_mode;          /* 0:normal_mode;1:factory_mode */
    uint32_t factory_addr_start;    /* factory bin start address */
    uint32_t factory_addr_switch;   /* factory bin switch address */
    uint32_t factory_size;          /* factory bin size */
    uint32_t factory_switch_size;   /* factory bin size */
    uint32_t factory_valid;         /* 0:erase 1:invalid;2:valid */
    uint32_t check_num;             /* check number */
} mfg_factory_config_t;

typedef struct {
    partition_information_t  app_info;
    partition_information_t  mfg_info;
} mfg_region_config_t;

uint32_t global_interrupt_lock(void);
void global_interrupt_restore(uint32_t);
#ifdef WS63_PRODUCT_NONE
uint32_t mfg_get_ftm_run_region(mfg_factory_config_t *mfg_factory_cfg);
#endif
uint32_t mfg_get_region_config(mfg_region_config_t *region_cfg);
errcode_t mfg_flash_erase(void);
errcode_t mfg_flash_read(uint8_t *ram_data, const uint32_t size);
errcode_t mfg_flash_write(const uint8_t *ram_data, uint32_t size);
errcode_t mfg_factory_mode_switch(mfg_region_config_t img_info, uint32_t switch_mode, mfg_factory_config_t *config);
uint32_t mfg_set_factory_invalid(mfg_factory_config_t *config);
errcode_t mfg_flash_write_info(const uint8_t *ram_data, uint32_t flash_offset, uint32_t size, bool do_erase);
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
errcode_t plt_flash_read_data(uint32_t addr, uint32_t size, uint8_t *data);
errcode_t plt_flash_write_data(uint32_t addr, uint32_t size, uint8_t *data, uint32_t *left);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* FACTORY_H */