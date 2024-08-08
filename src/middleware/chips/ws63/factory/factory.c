/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: UPG factory mode c file
 */

#include "factory.h"
#include <stddef.h>
#include <stdint.h>
#include "securec.h"
#include "common_def.h"
#include "memory_config_common.h"
#include "sfc.h"
#ifndef WS63_PRODUCT_NONE
#include "osal_addr.h"
#else
#include "secure_verify_boot.h"
#endif
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
#include "nv.h"
#endif

static void *mfg_malloc(uint32_t size)
{
    void *tmp_ptr = NULL;
#ifndef WS63_PRODUCT_NONE
    tmp_ptr = osal_kmalloc(size, 0);
#else
    uint32_t irq = global_interrupt_lock();
    tmp_ptr = malloc(size);
    global_interrupt_restore(irq);
#endif
    return tmp_ptr;
}

static void mfg_free(void *addr)
{
#ifndef WS63_PRODUCT_NONE
    osal_kfree(addr);
#else
    uint32_t irq = global_interrupt_lock();
    free(addr);
    global_interrupt_restore(irq);
#endif
}

static errcode_t check_factory_addr(uint32_t addr, uint32_t size)
{
    if ((addr > FLASH_MAPPED_END) || ((addr + size) > FLASH_MAPPED_END)) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static errcode_t mfg_flash_erase_etc(const uint32_t flash_offset, const uint32_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    // 地址向前4K对齐，大小向上4K补齐
    uint32_t end_addr = flash_offset + size;
    uint32_t start_sector = flash_offset & ~BYTE_4K_MASK;
    uint32_t end_sector = (end_addr & BYTE_4K_MASK) == 0 ? end_addr : (end_addr & ~BYTE_4K_MASK) + BYTE_4K;
    uint32_t hal_erase_size = end_sector - start_sector;
    uint32_t pre_size = flash_offset - start_sector;
    uint32_t last_size = end_sector - end_addr;

    uint8_t *writeback_pre_data = mfg_malloc(BYTE_4K);
    if (writeback_pre_data == NULL) {
        return ERRCODE_MALLOC;
    }
    uint8_t *writeback_last_data = mfg_malloc(BYTE_4K);
    if (writeback_last_data == NULL) {
        mfg_free(writeback_pre_data);
        return ERRCODE_MALLOC;
    }
    if (pre_size != 0) {
        ret = uapi_sfc_reg_read(start_sector, writeback_pre_data, pre_size);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
    if (last_size != 0) {
        ret = uapi_sfc_reg_read(end_addr, writeback_last_data, last_size);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
    // 擦除
    ret = uapi_sfc_reg_erase(start_sector, hal_erase_size);
    if (ret != ERRCODE_SUCC) {
        goto end;
    }
    // 回写
    if (pre_size != 0) {
        ret = mfg_flash_write_info(writeback_pre_data, start_sector, pre_size, false);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
    if (last_size != 0) {
        ret = mfg_flash_write_info(writeback_last_data, end_addr, last_size, false);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
end:
    mfg_free(writeback_pre_data);
    mfg_free(writeback_last_data);
    return ret;
}

errcode_t mfg_flash_erase(void)
{
    errcode_t ret = ERRCODE_FAIL;
    partition_information_t img_info;

    memset_s(&img_info, sizeof(img_info), 0, sizeof(img_info));
    ret = uapi_partition_get_info(PARTITION_FOTA_DATA, &img_info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint32_t factory_bin_addr = img_info.part_info.addr_info.addr;
    uint32_t single_size = img_info.part_info.addr_info.size - (MFG_FACTORY_REGION_CONFIG_SIZE * FACTORY_INFO_OFFSET);

    ret = check_factory_addr(factory_bin_addr, single_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_sfc_reg_erase(factory_bin_addr, single_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

errcode_t mfg_flash_read(uint8_t *ram_data, const uint32_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    partition_information_t img_info;

    memset_s(&img_info, sizeof(img_info), 0, sizeof(img_info));
    ret = uapi_partition_get_info(PARTITION_FOTA_DATA, &img_info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint32_t factory_info_addr = img_info.part_info.addr_info.addr +
        img_info.part_info.addr_info.size - (MFG_FACTORY_REGION_CONFIG_SIZE * FACTORY_INFO_OFFSET);
    ret = check_factory_addr(factory_info_addr, size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return uapi_sfc_reg_read(factory_info_addr, ram_data, size);
}

static errcode_t mfg_flash_check_erase(const uint32_t flash_offset, uint32_t size, bool do_erase)
{
    errcode_t ret = ERRCODE_FAIL;
    if (do_erase == true) {
        ret = mfg_flash_erase_etc(flash_offset, size);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }
    return ERRCODE_SUCC;
}

errcode_t mfg_flash_write_info(const uint8_t *ram_data, uint32_t flash_offset, uint32_t size, bool do_erase)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t bus_read_addr = flash_offset + FLASH_START;
    ret = mfg_flash_check_erase(flash_offset, size, do_erase);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint8_t *cmp_data = mfg_malloc(size);
    if (cmp_data == NULL) {
        return ERRCODE_MALLOC;
    }
    ret = uapi_sfc_reg_write(flash_offset, (uint8_t *)ram_data, size);
    if (ret != ERRCODE_SUCC) {
        goto write_failed;
    }
    /* 回读比较 */
    ret = (errcode_t)memcpy_s(cmp_data, size, (uint8_t *)(uintptr_t)bus_read_addr, size);
    if (ret != EOK) {
        goto write_failed;
    }
    ret = (errcode_t)memcmp(cmp_data, ram_data, size);
    if (ret != EOK) {
        goto write_failed;
    }
write_failed:
    mfg_free(cmp_data);
    return ret;
}

errcode_t mfg_flash_write(const uint8_t *ram_data, uint32_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    partition_information_t img_info;

    memset_s(&img_info, sizeof(img_info), 0, sizeof(img_info));
    ret = uapi_partition_get_info(PARTITION_FOTA_DATA, &img_info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint32_t flash_offset = img_info.part_info.addr_info.addr + img_info.part_info.addr_info.size -
        (MFG_FACTORY_REGION_CONFIG_SIZE * FACTORY_INFO_OFFSET);

    ret = check_factory_addr(flash_offset, size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return mfg_flash_write_info(ram_data, flash_offset, size, true);
}

uint32_t mfg_get_region_config(mfg_region_config_t *region_cfg)
{
    errcode_t ret = ERRCODE_FAIL;

    ret = uapi_partition_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret |= uapi_partition_get_info(PARTITION_APP_IMAGE, &(region_cfg->app_info));
    ret |= uapi_partition_get_info(PARTITION_FOTA_DATA, &(region_cfg->mfg_info));
    return ret;
}

#ifdef WS63_PRODUCT_NONE
static uint32_t mfg_get_ftm_config_init(const partition_information_t *img_info)
{
    errcode_t ret;
    uint32_t factory_addr = img_info->part_info.addr_info.addr + FLASH_START;

    image_code_info_t *image_code_info = (image_code_info_t *)(uintptr_t)(factory_addr + sizeof(image_key_area_t));
    uint32_t single_size = image_code_info->code_area_len + IMAGE_HEADER_LEN;

    mfg_factory_config_t mfg_ftm_cfg = {
        .factory_mode = MFG_FACTORY_DEFAULT_MODE,
        .factory_addr_start = factory_addr,
        .factory_addr_switch = img_info->part_info.addr_info.addr + FLASH_START,
        .factory_size = single_size,
        .factory_switch_size = single_size,
        .factory_valid = MFG_FACTORY_VALID,
        .check_num = MFG_FACTORY_CONFIG_CHECK
    };
    ret = check_factory_addr((mfg_ftm_cfg.factory_addr_start - FLASH_START), single_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = mfg_flash_write((const uint8_t *)(&mfg_ftm_cfg), sizeof(mfg_factory_config_t));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

/**
 * @brief 获取当前运行的镜像分区。
 * @return 成功返回分区序号，失败 默认从A区启动。
 */
uint32_t mfg_get_ftm_run_region(mfg_factory_config_t *mfg_factory_cfg)
{
    errcode_t ret;
    partition_information_t img_info = {0};
    uint32_t factory_addr = 0;

    if (mfg_factory_cfg == NULL) {
        return FTM_REGION_SERVICE;
    }
    ret = uapi_partition_get_info(PARTITION_FOTA_DATA, &img_info);
    if (ret != ERRCODE_SUCC) {
        return FTM_REGION_SERVICE;
    }
    ret = mfg_flash_read((uint8_t *)mfg_factory_cfg, sizeof(mfg_factory_config_t));
    if (ret != ERRCODE_SUCC) {
        return FTM_REGION_SERVICE;
    }

    factory_addr = img_info.part_info.addr_info.addr + FLASH_START;
    image_key_area_t *image_code_info = (image_key_area_t *)(factory_addr);
    if (mfg_factory_cfg->check_num != MFG_FACTORY_CONFIG_CHECK &&
        image_code_info->image_id == FACTORYBOOT_KEY_AREA_IMAGE_ID &&
        image_code_info->structure_length == sizeof(image_key_area_t)) {
        if (mfg_get_ftm_config_init(&img_info) != ERRCODE_SUCC) {
            return FTM_REGION_SERVICE;
        }
    }

    ret = mfg_flash_read((uint8_t *)mfg_factory_cfg, sizeof(mfg_factory_config_t));
    if (ret != ERRCODE_SUCC) {
        return FTM_REGION_SERVICE;
    }
    if (mfg_factory_cfg->factory_mode != FTM_REGION_SERVICE && mfg_factory_cfg->factory_mode != FTM_REGION_FACTORY) {
        return FTM_REGION_SERVICE;
    }
    return mfg_factory_cfg->factory_mode;
}
#endif

errcode_t mfg_factory_mode_switch(mfg_region_config_t img_info, uint32_t switch_mode, mfg_factory_config_t *config)
{
    if (config->factory_valid == MFG_FACTORY_INVALID) {
        return ERRCODE_FAIL;
    }
    if (switch_mode == 0x0) { /* switch to normal mode */
        if (config->factory_mode != 0x0) {
            config->factory_mode = 0x0;
            config->factory_valid = MFG_FACTORY_INVALID;
            config->factory_addr_switch = img_info.app_info.part_info.addr_info.addr + FLASH_START;
            config->factory_switch_size = img_info.app_info.part_info.addr_info.size;
            /* nv backup */
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
            nv_backup_mode_t backup_flag = {0};
            for (uint32_t i = 0; i < KEY_ID_REGION_MAX_NUM; i++) {
                backup_flag.region_mode[i] = 1;
            }
            errcode_t nv_ret = uapi_nv_backup((const nv_backup_mode_t *)&backup_flag);
            if (nv_ret != ERRCODE_SUCC) {
                return nv_ret;
            }
#endif
        }
    } else {
        return ERRCODE_FAIL;
    }
    return check_factory_addr((config->factory_addr_switch - FLASH_START), config->factory_switch_size);
}

#ifdef _PRE_WLAN_FEATURE_MFG_TEST
errcode_t plt_flash_read_data(uint32_t addr, uint32_t size, uint8_t *data)
{
    errcode_t ret;

    ret = check_factory_addr((addr - FLASH_START), size);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return uapi_sfc_reg_read((addr - FLASH_START), data, size);
}

errcode_t plt_flash_write_data(uint32_t addr, uint32_t size, uint8_t *data, uint32_t *left)
{
    errcode_t ret;
    partition_information_t img_info = {0};
    uint32_t info_addr = 0;
    uint32_t index = 0;
    uint32_t length = 0;
    uint32_t flash_block_rec[FLASH_PRIVATE_CUSTOM_CONFIG_TIMES] = {0};

    // 校验地址范围
    ret = check_factory_addr((addr - FLASH_START), size);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    ret = uapi_partition_get_info(PARTITION_FOTA_DATA, &img_info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    info_addr = img_info.part_info.addr_info.addr + img_info.part_info.addr_info.size -
        (MFG_FACTORY_REGION_CONFIG_SIZE * FACTORY_INFO_OFFSET) + FLASH_PRIVATE_CUSTOM_CONFIG_OFFSET;
    ret = uapi_sfc_reg_read(info_addr, (uint8_t *)flash_block_rec, sizeof(flash_block_rec));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    for (index = 0; index < (sizeof(flash_block_rec) / sizeof(uint32_t)); ++index) {
        if (flash_block_rec[index] == 0xFFFFFFFF) {
            flash_block_rec[index] = length + size;
            addr += length;
            ret = uapi_sfc_reg_write(info_addr, (uint8_t *)flash_block_rec, sizeof(flash_block_rec));
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
            *left = (FLASH_PRIVATE_CUSTOM_CONFIG_TIMES - index - 1);
            break;
        }
        length = flash_block_rec[index];
    }
    return uapi_sfc_reg_write((addr - FLASH_START), data, size);
}
#endif
