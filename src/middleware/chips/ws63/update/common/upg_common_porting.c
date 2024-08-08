/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG common functions for different chip
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "common_def.h"
#include "upg_debug.h"
#include "partition.h"
#include "securec.h"
#include "upg_porting.h"
#include "tcxo.h"
#include "non_os.h"
#include "cpu_utils.h"
#if (UPG_CFG_VERIFICATION_SUPPORT == YES)
#include "efuse.h"
#endif
#include "efuse_porting.h"
#include "sfc.h"
#include "watchdog.h"
#include "upg_alloc.h"
#include "upg_config.h"
#include "upg_definitions_porting.h"
#include "upg_common.h"
#include "upg_common_porting.h"
#ifndef WS63_PRODUCT_NONE
#include "osal_addr.h"
#endif
#ifdef CONFIG_MIDDLEWARE_SUPPORT_UPG_SAMPLE_VERIFY
#include "secure_verify_boot.h"
#endif

#define MS_ID_ADDR        0xF8
#define CHIP_ID_ADDR      OTP_CHIP_ID_START
#define TCXO_REBOOT_DELAY 500ULL

#define uapi_array_size(_array)  (sizeof(_array) / sizeof((_array)[0]))

static upg_image_partition_ids_map_t g_img_partition_map[] = {
    {FLASH_BOOT_IMAGE_ID, PARTITION_FLASH_BOOT_IMAGE},
    {APPLICATION_IMAGE_ID, PARTITION_APP_IMAGE},
    {HILINK_IMAGE_ID, PARTITION_HILINK_IMAGE},
};

/* 获取镜像ID和分区ID的map表
 * map: 镜像ID和分区ID的映射表
 * 返回map表中映射数量
 */
uint32_t upg_get_ids_map(upg_image_partition_ids_map_t **map)
{
    *map = g_img_partition_map;
    return uapi_array_size(g_img_partition_map);
}

/*
 * recovery/APP支持升级的镜像ID
 * 注意增删ID时同步更新升级ID的数量
 */
#define UPDATE_IMAGE_SET \
    { FLASH_BOOT_IMAGE_ID, APPLICATION_IMAGE_ID, UPG_IMAGE_ID_NV, HILINK_IMAGE_ID}
#define UPDATE_IMAGE_SET_CNT 4 /* 镜像ID列表长度 */

static uint32_t g_ws63_support_upg_id[] = UPDATE_IMAGE_SET;
static upg_image_collections_t g_ws63_upg_collect = { g_ws63_support_upg_id, UPDATE_IMAGE_SET_CNT };
/* 获取当前程序支持的升级镜像 */
upg_image_collections_t *uapi_upg_get_image_id_collection(void)
{
    return &g_ws63_upg_collect;
}

/* 获取升级包路径 */
char *upg_get_pkg_file_path(void)
{
    return UPG_FILE_NAME;
}

/* 获取资源索引路径 */
char *upg_get_res_file_index_path(void)
{
    return UPG_RES_INDEX_PATH;
}

/* 获取升级包所在目录 */
char *upg_get_pkg_file_dir(void)
{
    return UPG_FILE_PATH;
}

/* 本次支持升级的镜像ID集合，若为空则无镜像限制 */
bool upg_img_in_set(uint32_t img_id)
{
    upg_image_collections_t *collect = uapi_upg_get_image_id_collection();
    if (collect == NULL || collect->img_ids_cnt == 0) {
        return true;
    }

    for (uint32_t i = 0; i < collect->img_ids_cnt; i++) {
        if (collect->img_ids[i] == img_id) {
            return true;
        }
    }
    return false;
}

/*
 * 获取FOTA升级标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回升级标记区的起始地址
 */
errcode_t upg_get_upgrade_flag_flash_start_addr(uint32_t *start_address)
{
    errcode_t ret_val;
    partition_information_t info;

    ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    /* FOTA升级标记区在FOTA分区的最后 */
    *start_address = info.part_info.addr_info.addr + info.part_info.addr_info.size - FOTA_DATA_FLAG_AREA_LEN;
    return ERRCODE_SUCC;
}

/*
 * 获取FOTA升级进度恢复标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回标记区的起始地址
 * size 返回包含标记区和flag区的总长度
 */
errcode_t upg_get_progress_status_start_addr(uint32_t *start_address, uint32_t *size)
{
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    *start_address = info.part_info.addr_info.addr + info.part_info.addr_info.size - UPG_UPGRADE_FLAG_LENGTH;
    *size = UPG_UPGRADE_FLAG_LENGTH;
    return ERRCODE_SUCC;
}

/*
 * 获取在Flash上预留的FOTA分区的地址和长度，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回FOTA分区的起始地址
 * size          返回FOTA分区大小（包含升级标记区和缓存区、状态区）
 */
errcode_t upg_get_fota_partiton_area_addr(uint32_t *start_address, uint32_t *size)
{
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }
    *start_address = info.part_info.addr_info.addr;
    *size = info.part_info.addr_info.size;
    return ERRCODE_SUCC;
}

/*
 * 重启
 */
void upg_reboot(void)
{
    uapi_tcxo_delay_ms(TCXO_REBOOT_DELAY);
    hal_reboot_chip();
}

#ifdef WS63_PRODUCT_NONE
/*
 * 升级进度上报回调
 */
#define UPG_PROCESS_REPORT_DEC 10
#define UPG_PROCESS_REPORT_OVER 100
#define UPG_PROCESS_REPORT_PERCENT_LEN 3

STATIC void upg_progress_callbck(uint32_t percent)
{
    serial_puts("[UPG] upg percent : ");
    char tmp_char[UPG_PROCESS_REPORT_PERCENT_LEN];
    if (percent == UPG_PROCESS_REPORT_OVER) {
        serial_puts("1");
    }
    int index = 0;
    tmp_char[index++] = '0' + ((percent / UPG_PROCESS_REPORT_DEC) % UPG_PROCESS_REPORT_DEC);
    tmp_char[index++] = '0' + (percent % UPG_PROCESS_REPORT_DEC);
    tmp_char[index++] = '\0';
    serial_puts(tmp_char);
    serial_puts(" %\r");
    if (percent == UPG_PROCESS_REPORT_OVER) {
        serial_puts("\n");
    }
}

void upg_progress_callback_register(void)
{
    errcode_t ret = uapi_upg_register_progress_callback(upg_progress_callbck);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upgrade progress callback regist failed\r\n");
    }
}
#endif

/*
 * 防止看门狗超时，踢狗
 */
void upg_watchdog_kick(void)
{
    uapi_watchdog_kick();
}

uint32_t upg_get_flash_base_addr(void)
{
    return FLASH_START;
}

uint32_t upg_get_flash_size(void)
{
    return UPG_FLASH_SIZE;
}
void non_os_enter_critical(void);

void non_os_exit_critical(void);
// 升级读flash接口需支持跨页读取
errcode_t upg_flash_read(const uint32_t flash_offset, const uint32_t size, uint8_t *ram_data)
{
    errcode_t ret = ERRCODE_FAIL;
#if defined (CONFIG_DRIVER_SUPPORT_SFC)
    ret = uapi_sfc_reg_read(flash_offset, (uint8_t *)ram_data, size);
    if (ret != ERRCODE_SUCC) {
#else
    ret = (errcode_t)memcpy_s((uint8_t *)(uintptr_t)(flash_offset + FLASH_START), size, (void *)ram_data, size);
    if (ret != EOK) {
#endif
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

// 升级写flash接口需支持跨页写入和写前擦功能
errcode_t upg_flash_write(const uint32_t flash_offset, uint32_t size, const uint8_t *ram_data, bool do_erase)
{
    errcode_t ret = ERRCODE_FAIL;
    if (do_erase == true) {
        ret = upg_flash_erase(flash_offset, size);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }
    /* 写入flash */
    uint32_t lock = (uint32_t)osal_irq_lock();

#if defined (CONFIG_DRIVER_SUPPORT_SFC)
    ret = uapi_sfc_reg_write(flash_offset, (uint8_t *)ram_data, size);
    if (ret != ERRCODE_SUCC) {
#else
    ret = (errcode_t)memcpy_s((uint8_t *)(uintptr_t)(flash_offset + FLASH_START), size, (void *)ram_data, size);
    if (ret != EOK) {
#endif
        osal_irq_restore(lock);
        goto write_failed;
    }
    osal_irq_restore(lock);

#if UPG_FLASH_FUNC_DEBUG == YES
    uint8_t *cmp_data = upg_malloc(size);
    if (cmp_data == NULL) {
        return ERRCODE_MALLOC;
    }
    /* 回读比较 */
    ret = (errcode_t)memcpy_s(cmp_data, size, (uint8_t *)(uintptr_t)(flash_offset + FLASH_START), size);
    if (ret != EOK) {
        goto write_failed;
    }
    ret = (errcode_t)memcmp(cmp_data, ram_data, size);
    if (ret != EOK) {
        goto write_failed;
    }
#endif
write_failed:
#if UPG_FLASH_FUNC_DEBUG == YES
    upg_free(cmp_data);
#endif
    return ret;
}

#define FLASH_PAGE_SIZE_BIT_LENGTH 12
#define BYTE_4K_MASK              0xFFF
#define BYTE_4K                   0x1000

errcode_t upg_flash_erase(const uint32_t flash_offset, const uint32_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    // 地址向前4K对齐，大小向上4K补齐
    uint32_t end_addr = flash_offset + size;
    uint32_t start_sector = flash_offset & ~BYTE_4K_MASK;
    uint32_t end_sector = (end_addr & BYTE_4K_MASK) == 0 ? end_addr : (end_addr & ~BYTE_4K_MASK) + BYTE_4K;
    uint32_t hal_erase_size = end_sector - start_sector;
    uint32_t first_size = flash_offset - start_sector;
    uint32_t last_size = end_sector - end_addr;

    uint8_t *writeback_first_data = upg_malloc(BYTE_4K);
    if (writeback_first_data == NULL) {
        return ERRCODE_MALLOC;
    }
    uint8_t *writeback_last_data = upg_malloc(BYTE_4K);
    if (writeback_last_data == NULL) {
        upg_free(writeback_first_data);
        return ERRCODE_MALLOC;
    }
    if (first_size != 0) {
        ret = upg_flash_read(start_sector, first_size, writeback_first_data);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
    if (last_size != 0) {
        ret = upg_flash_read(end_addr, last_size, writeback_last_data);
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
    if (first_size != 0) {
        ret = upg_flash_write(start_sector, first_size, writeback_first_data, false);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
    if (last_size != 0) {
        ret = upg_flash_write(end_addr, last_size, writeback_last_data, false);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
end:
    upg_free(writeback_first_data);
    upg_free(writeback_last_data);
    return ret;
}

#if (UPG_CFG_VERIFICATION_SUPPORT == YES)
/*
 * 获取校验用的root_public_key
 */
uint8_t *upg_get_root_public_key(void)
{
#if (UPG_CFG_DIRECT_FLASH_ACCESS == YES)
    /* 使用Upgrader_External_Public_Key校验Key Area的签名  */
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FLASH_ROOT_PUBLIC_KEYS_AREA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return NULL;
    }
    uint32_t rootkey_buff_addr = info.part_info.addr_info.addr + upg_get_flash_base_addr();
    root_public_key *rootkey_buff = (root_public_key *)(uintptr_t)rootkey_buff_addr;
    return (uint8_t *)(rootkey_buff->root_key_area);
#else
    upg_package_header_t *pkg_header = NULL;
    errcode_t ret;
    ret = upg_get_package_header(&pkg_header);
    if (ret != ERRCODE_SUCC || pkg_header == NULL) {
        upg_log_err("[UPG] upg_get_package_header fail\r\n");
        return NULL;
    }
    static uint8_t public_key[PUBLIC_KEY_LEN];
    memcpy_s(public_key, sizeof(public_key), (pkg_header->key_area.fota_external_public_key), PUBLIC_KEY_LEN);
    upg_free(pkg_header);
    return public_key;
#endif
}

STATIC errcode_t check_fota_msid(const uint32_t msid_ext, const uint32_t mask_msid_ext)
{
    unused(msid_ext);
    unused(mask_msid_ext);
    uint32_t msid = 0;
    errcode_t ret = uapi_efuse_read_buffer((uint8_t *)&msid, MS_ID_ADDR, sizeof(uint32_t));
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] get msid failed. ret = 0x%x\r\n", ret);
    }

    if ((msid_ext & mask_msid_ext) != (msid & mask_msid_ext)) {
        upg_log_err("[UPG] upg verify: msid is wrong!\r\n");
    }

    return ERRCODE_SUCC; // msid地址未确定，先返回成功
}

/*
 * 检查FOTA升级包中的信息与板端是否匹配
 * pkg_header 升级包包头指针
 * 检查成功，返回 ERRCODE_SUCC
 */
errcode_t upg_check_fota_information(const upg_package_header_t *pkg_header)
{
    upg_key_area_data_t *key_area = (upg_key_area_data_t *)&(pkg_header->key_area);
    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);
    errcode_t ret;

    ret = check_fota_msid(key_area->msid_ext, key_area->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = check_fota_msid(fota_info->msid_ext, fota_info->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}
#endif

errcode_t upg_get_efuse_addr(uint32_t image_id, uint32_t *start_addr)
{
    switch (image_id) {
        case FLASH_BOOT_IMAGE_ID:
            *start_addr = EFUSE_FLASHBOOT_ROLLBACK_VERSION;
            break;
        case APPLICATION_IMAGE_ID:
            *start_addr = EFUSE_APP_ROLLBACK_VERSION;
            break;
        default:
            return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t upg_get_board_rollback_version(uint32_t image_id, uint32_t *anti_rollback_ver)
{
    uint32_t efuse_offset;
    errcode_t ret = upg_get_efuse_addr(image_id, &efuse_offset);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return (errcode_t)uapi_efuse_read_buffer((uint8_t *)anti_rollback_ver, efuse_offset, sizeof(uint32_t));
}

errcode_t upg_get_board_version_mask(uint32_t image_id, uint32_t *key_mask, uint32_t *code_mask)
{
    unused(image_id);
    *key_mask = 0x0;
    *code_mask = 0xFFFFFFFF;
    return ERRCODE_SUCC;
}

errcode_t upg_get_board_version(uint32_t image_id, uint32_t *key_ver, uint32_t *code_ver)
{
    unused(image_id);
    upg_package_header_t     *pkg_header = NULL;
    upg_image_hash_node_t *img_hash_table = NULL;
    upg_image_header_t *img_header = NULL;
    errcode_t ret = upg_get_package_header(&pkg_header);
    if (ret != ERRCODE_SUCC || pkg_header == NULL) {
        upg_log_err("[UPG] upg_get_package_header fail\r\n");
        return ret;
    }
    ret = upg_get_pkg_image_hash_table(pkg_header, &img_hash_table);
    if (ret != ERRCODE_SUCC || img_hash_table == NULL) {
        upg_log_err("[UPG] upg_get_pkg_image_hash_table fail\r\n");
        upg_free(pkg_header);
        return ret;
    }
    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);
    for (uint32_t i = 0; i < fota_info->image_num; i++) {
        ret = upg_get_pkg_image_header((const upg_image_hash_node_t *)&(img_hash_table[i]), &img_header);
        if (ret != ERRCODE_SUCC || img_header == NULL) {
            upg_log_err("[UPG] upg_get_pkg_image_header fail\r\n");
            upg_free(pkg_header);
            upg_free(img_hash_table);
            break;
        }
        if (img_header->image_id == image_id) {
            break;
        }
    }
    *key_ver = pkg_header->key_area.fota_key_version_ext;
    *code_ver = img_header->version_ext;
    upg_free(pkg_header);
    upg_free(img_hash_table);
    upg_free(img_header);
    return ret;
}

errcode_t upg_set_board_rollback_version(uint32_t image_id, uint32_t *anti_rollback_ver)
{
    uint32_t efuse_offset;
    errcode_t ret = upg_get_efuse_addr(image_id, &efuse_offset);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint32_t version = *anti_rollback_ver;
    return (errcode_t)uapi_efuse_write_buffer(efuse_offset, (uint8_t *)&version, sizeof(uint32_t));
}

uint32_t global_interrupt_lock(void);
void global_interrupt_restore(uint32_t);

static void *upg_malloc_port(uint32_t size)
{
    void *upg_ptr = NULL;
#ifndef WS63_PRODUCT_NONE
    upg_ptr = osal_kmalloc(size, 0);
#else
    uint32_t upg_irq = global_interrupt_lock();
    upg_ptr = malloc(size);
    global_interrupt_restore(upg_irq);
#endif
    return upg_ptr;
}

static void upg_free_port(void *addr)
{
#ifndef WS63_PRODUCT_NONE
    osal_kfree(addr);
#else
    uint32_t irq = global_interrupt_lock();
    free(addr);
    global_interrupt_restore(irq);
#endif
}

static void ws63_upg_putc(const char c)
{
#ifndef WS63_PRODUCT_NONE
    unused(c);
#else
    serial_putc(c);
#endif
}

errcode_t ws63_upg_init(void)
{
    errcode_t ret;
    upg_func_t upg_func = {0};
    upg_func.malloc = upg_malloc_port;
    upg_func.free = upg_free_port;
    upg_func.serial_putc = ws63_upg_putc;
    ret = uapi_upg_init(&upg_func);
    if (ret != ERRCODE_SUCC) {
        upg_log_err("[UPG] upgrade init failed!\r\n");
        return ret;
    }
#ifdef WS63_PRODUCT_NONE
    upg_progress_callback_register();
#endif
    upg_log_info("[UPG] upgrade init OK!\r\n");
    return ret;
}

#if defined(UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE) && defined(YES) && (UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE == YES)
errcode_t upg_erase_whole_image(const upg_image_header_t *img_header)
{
    errcode_t ret;
    partition_information_t info;
    ret = upg_get_image_info(img_header->image_id, &info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = upg_flash_erase(info.part_info.addr_info.addr, info.part_info.addr_info.size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}
#endif