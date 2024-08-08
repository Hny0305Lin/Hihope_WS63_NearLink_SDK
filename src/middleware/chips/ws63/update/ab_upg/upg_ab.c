/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: UPG ab mode c file
 */

#include <stddef.h>
#include <stdint.h>
#include "errcode.h"
#include "common_def.h"
#include "sfc.h"
#include "partition.h"
#include "upg_debug.h"
#include "upg_porting.h"
#include "upg_ab.h"

#define UPG_AB_REGION_CONFIG_SIZE 0x1000
#define UPG_AB_DEFAULT_REGION 0
#define UPG_AB_CONFIG_CHECK 0x70746C6C

typedef struct {
    uint32_t  check_num;    /* check number */
    uint32_t  run_region;   /* run region */
} upg_ab_config_t;

typedef struct {
    partition_information_t  a_info;
    partition_information_t  b_info;
} upg_ab_region_config_t;

/**
 * @brief 获取镜像分区配置参数
 * @param region_cfg 传出的镜像分区配置参数
 * @return 返回相关错误码
 */
static errcode_t upg_get_region_config(upg_ab_region_config_t *region_cfg)
{
    errcode_t ret_val = ERRCODE_SUCC;
    ret_val |= uapi_partition_get_info(PARTITION_APP_IMAGE, &(region_cfg->a_info));
    ret_val |= uapi_partition_get_info(PARTITION_FOTA_DATA, &(region_cfg->b_info));
    return ret_val;
}

/**
 * @brief 获取镜像分区配置的地址
 * @param region_cfg 传出的镜像分区配置参数
 * @return 返回相关错误码
 */
static errcode_t upg_get_ab_config_addr(uint32_t *addr)
{
    partition_information_t b_info;
    errcode_t ret;

    ret = uapi_partition_get_info(PARTITION_FOTA_DATA, &b_info);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    *addr = b_info.part_info.addr_info.addr + b_info.part_info.addr_info.size - UPG_AB_REGION_CONFIG_SIZE;

    return ERRCODE_SUCC;
}

/**
 * @brief 初始化ab面升级方式的配置参数区。
 * @param upg_cfg 需要返回的配置参数
 * @return 返回相关错误码
 */
static errcode_t upg_ab_config_init(void)
{
    uint32_t upg_config_addr;
    errcode_t ret = upg_get_ab_config_addr(&upg_config_addr);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    upg_ab_config_t upg_cfg = {
        .check_num = UPG_AB_CONFIG_CHECK,
        .run_region = UPG_AB_DEFAULT_REGION,
    };

    ret = upg_flash_erase(upg_config_addr, UPG_AB_REGION_CONFIG_SIZE);
    ret |= upg_flash_write(upg_config_addr, sizeof(upg_ab_config_t), (const uint8_t *)(&upg_cfg), false);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] ab cfg init write failed, ret = 0x%x\r\n", ret);
        return ERRCODE_UPG_FLASH_WRITE_ERROR;
    }
    return ERRCODE_SUCC;
}

/**
 * @brief 获取ab面升级方式的配置参数。
 * @param upg_cfg 需要返回的配置参数
 * @return 返回相关错误码
 */
static errcode_t upg_get_ab_config(upg_ab_config_t *upg_cfg)
{
    uint32_t upg_config_addr;
    errcode_t ret = upg_get_ab_config_addr(&upg_config_addr);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (upg_flash_read(upg_config_addr, sizeof(upg_ab_config_t), (uint8_t *)upg_cfg) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (upg_cfg->check_num != UPG_AB_CONFIG_CHECK) {
        upg_log_err("[UPG] upg ab config check failed, will init, check_num = 0x%x\r\n", upg_cfg->check_num);
        if (upg_ab_config_init() != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}

/**
 * @brief 设置ab面升级方式的配置参数。
 * @param upg_cfg 需要设置的配置参数
 * @return 返回相关错误码
 */
#define UPG_SET_AB_CFG_TRY_MAX 3
static errcode_t upg_set_ab_config(upg_ab_config_t *upg_cfg)
{
    upg_ab_region_config_t region_cfg = {0};
    uint32_t try_times = 0;
    errcode_t ret = upg_get_region_config(&region_cfg);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    uint32_t upg_config_addr = region_cfg.b_info.part_info.addr_info.addr + region_cfg.b_info.part_info.addr_info.size -
        UPG_AB_REGION_CONFIG_SIZE;
    while (try_times < UPG_SET_AB_CFG_TRY_MAX) {
        ret = upg_flash_erase(upg_config_addr, UPG_AB_REGION_CONFIG_SIZE);
        ret |= upg_flash_write(upg_config_addr, sizeof(upg_ab_config_t), (const uint8_t *)upg_cfg, false);
        if (ret == ERRCODE_SUCC) {
            break;
        }
        try_times++;
    }
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg set config failed, ret = 0x%x\r\n", ret);
        return ERRCODE_UPG_FLASH_WRITE_ERROR;
    }
    return ERRCODE_SUCC;
}

/**
 * @brief 获取当前运行的镜像分区。
 * @return 成功返回分区序号，失败 默认从A区启动。
 */
upg_region_index upg_get_run_region(void)
{
    upg_ab_config_t upg_cfg = {0};
    if (upg_get_ab_config(&upg_cfg) != ERRCODE_SUCC) {
        return UPG_AB_DEFAULT_REGION;
    }
    if (upg_cfg.run_region != UPG_REGION_A && upg_cfg.run_region != UPG_REGION_B) {
        return UPG_AB_DEFAULT_REGION;
    }
    return upg_cfg.run_region;
}

/**
 * @brief 获取待升级的镜像分区。
 * @return 返回分区序号。
 */
upg_region_index upg_get_upg_region(void)
{
    if (upg_get_run_region() == UPG_REGION_B) {
        return UPG_REGION_A;
    } else {
        return UPG_REGION_B;
    }
}

/**
 * @brief 获取镜像分区的起始地址。
 * @param upg_region 需要获取的镜像分区
 * @return 成功返回分区地址，失败返回 0
 */
#define UPG_SIZE_4K_ALIGN 4095
#define UPG_SIZE_4K (UPG_SIZE_4K_ALIGN + 1)
uint32_t upg_get_region_addr(upg_region_index upg_region)
{
    upg_ab_region_config_t info = {0};
    if (upg_get_region_config(&info) != ERRCODE_SUCC) {
        upg_log_err("[UPG] upg get region addr failed\r\n");
        return 0;
    }
    uint32_t image_addr = info.a_info.part_info.addr_info.addr;
    uint32_t all_size = info.a_info.part_info.addr_info.size + info.b_info.part_info.addr_info.size;
    uint32_t single_size = (all_size - UPG_AB_REGION_CONFIG_SIZE) / UPG_REGION_COUNT;
    single_size = (single_size + UPG_SIZE_4K_ALIGN) & ~(UPG_SIZE_4K_ALIGN);
    switch (upg_region) {
        case UPG_REGION_A:
            return image_addr;
        case UPG_REGION_B:
            return image_addr + single_size;
        default:
            return 0;
    }
}

/**
 * @brief 获取镜像分区的大小。
 * @param upg_region 需要获取的镜像分区
 * @return 成功返回分区大小，失败返回 0
 */
uint32_t upg_get_region_size(upg_region_index upg_region)
{
    unused(upg_region);
    upg_ab_region_config_t info = {0};
    if (upg_get_region_config(&info) != ERRCODE_SUCC) {
        return 0;
    }
    uint32_t all_size = info.a_info.part_info.addr_info.size + info.b_info.part_info.addr_info.size;
    uint32_t single_size = (all_size - UPG_AB_REGION_CONFIG_SIZE) / UPG_REGION_COUNT;
    single_size = (single_size + UPG_SIZE_4K_ALIGN) & ~(UPG_SIZE_4K_ALIGN);
    switch (upg_region) {
        case UPG_REGION_A:
            return single_size;
        case UPG_REGION_B:
            return all_size - UPG_AB_REGION_CONFIG_SIZE - single_size;
        default:
            return 0;
    }
}

/**
 * @brief 读取镜像数据。
 * @param upg_region，镜像区域，UPG_REGION_XXX。
 * @param offset 偏移量。
 * @param buf 内存指针。
 * @param len 读取的长度。
 * @return 成功返回ERRCODE_SUCC，失败返回错误码。
 */
uint32_t upg_ab_image_read(upg_region_index upg_region, uint32_t offset, uint8_t *buf, size_t len)
{
    uint32_t addr = upg_get_region_addr(upg_region);
    uint32_t size = upg_get_region_size(upg_region);
    if (addr == 0 || size == 0 || (offset + len) > size) {
        return ERRCODE_INVALID_PARAM;
    }
    return upg_flash_read(addr + offset, len, buf);
}

/**
 * @brief 写入镜像数据。
 * @param upg_region，镜像区域，UPG_REGION_XXX。
 * @param offset 偏移量。
 * @param buf 待写入数据的内存指针。
 * @param len 写入的长度。
 * @return 成功返回ERRCODE_SUCC，失败返回错误码。
 */
uint32_t upg_ab_image_write(upg_region_index upg_region, uint32_t offset, uint8_t *buf, size_t len)
{
    uint32_t addr = upg_get_region_addr(upg_region);
    uint32_t size = upg_get_region_size(upg_region);
    if (addr == 0 || size == 0 || (offset + len) > size) {
        return ERRCODE_INVALID_PARAM;
    }
    /* false: start过程已擦除整个待升级镜像区，此处不需写前擦 */
    return upg_flash_write(addr + offset, len, buf, false);
}

/**
 * @brief 镜像版本切换
 * @param upg_region 镜像区域，UPG_REGION_A/UPG_REGION_B。下一次启动的版本区域是upg_region。
 * @return 成功返回ERRCODE_SUCC，失败返回错误码。
 */
errcode_t upg_set_run_region(upg_region_index upg_region)
{
    upg_ab_config_t upg_cfg = {0};
    errcode_t ret = upg_get_ab_config(&upg_cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if (upg_cfg.run_region == upg_region) {
        upg_log_err("[UPG] set the same region with the current,please check\r\n");
        return ERRCODE_INVALID_PARAM;
    }
    upg_cfg.run_region = upg_region;
    ret = upg_set_ab_config(&upg_cfg);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] set the config failed\r\n");
        return ret;
    }
    return ERRCODE_SUCC;
}

/**
 * @brief 擦除待升级的镜像区
 * @param region 待擦除的镜像区域，UPG_REGION_A/UPG_REGION_B。
 * @return 成功返回ERRCODE_SUCC，失败返回错误码。
 */
errcode_t upg_region_erase(upg_region_index region)
{
    uint32_t addr = upg_get_region_addr(region);
    uint32_t size = upg_get_region_size(region);
    upg_log_info("[UPG] erase region addr:0x%08x, size:0x%08x\r\n", addr, size);

    if ((addr & UPG_SIZE_4K_ALIGN) || (size % UPG_SIZE_4K)) {
        upg_log_err("[UPG] region addr:0x%08x or size:0x%08x is not 4K aligned\r\n", addr, size);
        return ERRCODE_INVALID_PARAM;
    }
    if (uapi_sfc_reg_erase(addr, size) != ERRCODE_SUCC) {
        upg_log_err("[UPG] erase region failed\r\n");
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

/**
 * @brief 镜像升级开始，准备写入镜像
 * @param upg_region 镜像区域，UPG_REGION_A/UPG_REGION_B
 * @return 成功返回ERRCODE_SUCC，失败返回错误码。
 */
errcode_t upg_ab_start(upg_region_index upg_region)
{
    if (upg_region == UPG_REGION_A || upg_region == UPG_REGION_B) {
        return ERRCODE_INVALID_PARAM;
    }

    if (ws63_upg_init() != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return upg_region_erase(upg_get_upg_region());
}
