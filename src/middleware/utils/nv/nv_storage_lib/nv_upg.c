/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: UPG KV functions source file
 */

#include "nv_porting.h"
#if (defined(CONFIG_OTA_UPDATE_SUPPORT) && (CONFIG_NV_SUPPORT_OTA_UPDATE == NV_YES))
#include "upg_common.h"
#include "upg_definitions.h"
#include "nv_key.h"
#include "nv_update.h"
#include "nv_store.h"
#include "nv_reset.h"
#include "nv.h"
#include "nv_storage.h"
#include "upg_porting.h"
#include "upg_alloc.h"
#include "sha256.h"
#include "nv_async_store.h"
#include "nv_upg.h"

#define KV_INDEX 0
#define KV_CRC_DATA_LENGTH 4

#ifndef UPG_VERIFY_CIPHER_BUF_ATTR
#define UPG_VERIFY_CIPHER_BUF_ATTR    DRV_CIPHER_BUF_NONSECURE
#endif
#ifndef STATIC_UT
#define STATIC_UT static
#endif

static inline uint32_t kv_aligned(uint32_t len, uint32_t align)
{
    return ((uint32_t)(len) + ((align) - 1)) & ~((align) - 1);
}

STATIC_UT bool kv_upg_upgrade_is_update_requested(fota_upgrade_flag_area_t *flag_info)
{
    upg_image_status_t status;
    status = upg_get_image_update_status(flag_info, KV_INDEX, UPG_IMAGE_ID_NV);
    if (status == UPG_IMAGE_STATUS_FINISHED || status == UPG_IMAGE_STATUS_NOT_STARTED) {
        return false;
    }
    return true;
}

STATIC_UT errcode_t kv_upg_copy_pkg_image_data(uint32_t data_addr, uint32_t data_length,
                                               uint32_t data_offset, uint32_t *data_len, uint8_t *img_data)
{
    errcode_t ret_val;
    uint32_t aligned_image_len = kv_aligned(data_length, 16); /* 16-byte alignment */
    uint32_t actual_len;

    if (data_offset >= aligned_image_len || img_data == NULL ||
        data_len == NULL || *data_len == 0) {
        return ERRCODE_UPG_INVALID_PARAMETER;
    }

    actual_len = (data_offset + *data_len > aligned_image_len) ? (aligned_image_len - data_offset) : *data_len;
    ret_val = upg_read_fota_pkg_data(data_addr + data_offset, img_data, &actual_len);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }
    *data_len = actual_len;
    return ERRCODE_SUCC;
}

STATIC_UT errcode_t kv_upg_upgrade_data_verify(uint32_t kv_data_offset, uint32_t kv_data_len, const uint8_t* hash_data)
{
    uint32_t handle;
    uint32_t out_length = SHA_256_LENGTH;
    uint32_t image_offset = 0;
    uint8_t data_sha[SHA_256_LENGTH] = {0};
    uint32_t aligned_image_len = kv_aligned(kv_data_len, 16); /* 16-byte alignment */

    /* 考虑内存资源按0x1000处理 */
    uint8_t *image_data = (uint8_t *)kv_malloc(VERIFY_BUFF_LEN);
    if (image_data == NULL) {
        return ERRCODE_MALLOC;
    }

    errcode_t ret = uapi_drv_cipher_sha256_start(&handle);
    if (ret != ERRCODE_SUCC) {
        kv_free(image_data);
        return ret;
    }

    do {
        uint32_t read_len = (image_offset + VERIFY_BUFF_LEN < aligned_image_len) ?
            VERIFY_BUFF_LEN : (aligned_image_len - image_offset);
        ret = kv_upg_copy_pkg_image_data(kv_data_offset, kv_data_len,
                                         image_offset, &read_len, image_data);
        if (ret != ERRCODE_SUCC) {
            nv_log_err("[NV] kv upgrade kv_upg_copy_pkg_image_data error\r\n");
            break;
        }

        ret = uapi_drv_cipher_sha256_update(handle, image_data, read_len);
        if (ret != ERRCODE_SUCC) {
            nv_log_err("[NV] kv upgrade uapi_drv_cipher_sha256_update error\r\n");
            break;
        }
        image_offset += read_len;
    } while (image_offset < aligned_image_len);

    kv_free(image_data);
    ret = uapi_drv_cipher_sha256_finish(handle, data_sha, &out_length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (memcmp(hash_data, data_sha, SHA_256_LENGTH) != 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

STATIC_UT errcode_t kv_upg_upgrade_key_helper(kv_store_t core, kv_key_header_t* key_header)
{
    uint8_t attributes;
    flash_task_node task_node = {0};
    /* 如果升级包传入的key是无效的，则不能进行升级 */
    if (key_header->valid == 0 || key_header->length == 0) {
        return ERRCODE_FLASH_TASK_COMPLETED;
    } else {
        attributes = 0;
        if (key_header->upgrade == 0) {
            attributes = NV_ATTRIBUTE_NON_UPGRADE;
        }
        /* 如果既是不可升级又是永久的，属性设置为永久和不可升级 */
        if (key_header->type == 0) {
            attributes = NV_ATTRIBUTE_PERMANENT | NV_ATTRIBUTE_NON_UPGRADE;
        }

        task_node.data.kv.key = key_header->key_id;
        task_node.data.kv.kvalue = (uint8_t *)(key_header + 1);
        task_node.data.kv.kvalue_length = key_header->length;
        task_node.data.kv.attribute = attributes;
        (void)kv_update_write_key(core, &task_node);
    }
    return task_node.state_code;
}

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
#if !defined(CONFIG_NV_SUPPORT_BACKUP_UPGRADE) || (CONFIG_NV_SUPPORT_BACKUP_UPGRADE == NV_YES)
STATIC_UT errcode_t kv_upg_upgrade_backup_key(kv_key_header_t* key_header)
{
    errcode_t ret;
    /* 如果升级包传入的key是无效的，则不能进行升级 */
    if (key_header->valid == 0 || key_header->length == 0) {
        return ERRCODE_SUCC;
    }
    /* 获取升级包数据写入到备份区 */
    uint16_t key_size;
    kv_key_handle_t key;
    uint32_t write_position = 0;

    key.header = *key_header;
    key_size = kv_key_flash_size(&key);
    /* 找可写入的位置 */
    ret = kv_backup_find_write_position(key_size, &write_position);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* 写入备份区数据 */
    ret = kv_key_write_flash(write_position, key_size, (uint8_t*)key_header);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

STATIC_UT bool kv_upg_check_backup_upgradeable(kv_key_header_t* key_header, kv_key_handle_t *pre_backup_key)
{
    errcode_t ret;
    kv_attributes_t attr = 0;
    uint16_t key_length = 0;

    ret = kv_store_get_backup_key_attr(key_header->key_id, &key_length, &attr, pre_backup_key);
    /* 备份区没有该数据，升级会在备份区新增一个数据 */
    if (ret == ERRCODE_NV_KEY_NOT_FOUND) {
        /* 此时并没有找到先前的备份区key，因此将pre_backup_key的location置为0默认值，后续不需要无效该key */
        pre_backup_key->key_location = 0;
        return true;
    }

    /* 永久、加密属性的NV不可以升级,其他属性的数据随着工作区一起升级 */
    if ((((uint32_t)attr & NV_ATTRIBUTE_PERMANENT) != 0) || (((uint32_t)attr & NV_ATTRIBUTE_ENCRYPTED) != 0)) {
        return false;
    }
    return true;
}
#endif

#endif

STATIC_UT bool kv_upg_check_upgradeable(kv_key_header_t* key_header)
{
    errcode_t ret;
    kv_attributes_t attr = 0;
    uint16_t key_length = 0;

    ret = kv_store_get_key_attr(KV_STORE_APPLICATION, key_header->key_id, &key_length, &attr);
    /* 工作区没有该数据 说明升级的目的是新增一个NV项 */
    if (ret == ERRCODE_NV_KEY_NOT_FOUND) {
        return true;
    }

    /* 永久、加密、不可升级属性的NV都不可以升级 */
    if ((((uint32_t)attr & NV_ATTRIBUTE_PERMANENT) != 0) || (((uint32_t)attr & NV_ATTRIBUTE_ENCRYPTED) != 0) ||
        (((uint32_t)attr & NV_ATTRIBUTE_NON_UPGRADE) != 0)) {
        return false;
    }
    return true;
}

STATIC_UT kv_store_t kv_upg_convert_core_id(uint16_t core_id)
{
    switch (core_id) {
        case KV_STORE_ID_ACPU:
            return KV_STORE_APPLICATION;
        default:
            return KV_STORE_MAX_NUM;
    }
}

STATIC_UT errcode_t kv_upg_process_one_page(kv_store_t core, uint8_t* image_data, uint32_t data_len)
{
    errcode_t ret;
    uint32_t offset = 0;
    kv_key_header_t* key_header = NULL;
    uint32_t key_flash_size;
    uint32_t key_length;
    key_header = (kv_key_header_t *)image_data;
    while (offset + sizeof(kv_key_header_t) + key_header->length <= data_len) {
        bool store_upgradeable = false;
        /* 升级工作区数据 */
        if (kv_upg_check_upgradeable(key_header)) {
            store_upgradeable = true;
            ret = kv_upg_upgrade_key_helper(core, key_header);
            if (ret != ERRCODE_FLASH_TASK_COMPLETED && ret != ERRCODE_NV_KEY_NOT_FOUND) {
                nv_log_err("[NV] kv upgrade error keyid:0x%x\r\n", key_header->key_id);
                return ret;
            }
        }

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
#if !defined(CONFIG_NV_SUPPORT_BACKUP_UPGRADE) || (CONFIG_NV_SUPPORT_BACKUP_UPGRADE == NV_YES)
        /* 只有工作区数据可升级时，才可升级备份区数据 */
        kv_key_handle_t pre_backup_key;
        if (store_upgradeable && kv_upg_check_backup_upgradeable(key_header, &pre_backup_key)) {
            /* 1、在备份区写入该数据 */
            ret = kv_upg_upgrade_backup_key(key_header);
            if (ret != ERRCODE_SUCC) {
                nv_log_err("[NV] kv backup upgrade error keyid:0x%x\r\n", key_header->key_id);
                return ret;
            }
            /* 2、将备份区原先的数据置为无效 */
            ret = kv_backup_set_invalid_key(&pre_backup_key);
            if (ret != ERRCODE_SUCC) {
                nv_log_err("[NV] kv backup invalid key  error keyid:0x%x\r\n", key_header->key_id);
                return ret;
            }
        }
#endif
#endif
        key_length = kv_aligned(key_header->length, 4); /* 4-byte alignment */
        key_flash_size = (uint32_t)sizeof(kv_key_header_t) + key_length + KV_CRC_DATA_LENGTH;
        offset += key_flash_size;
        key_header = (kv_key_header_t *)((uintptr_t)key_header + key_flash_size);
        unused(store_upgradeable);
    }

    return ERRCODE_SUCC;
}

STATIC_UT errcode_t kv_upg_process_kv_data(uint32_t kv_data_offset, uint32_t length)
{
    errcode_t ret;
    uint32_t offset = 0;
    uint32_t read_len;
    kv_page_header_t *page_head = NULL;
    kv_store_t core;

    if (length % VERIFY_BUFF_LEN != 0) {
        nv_log_err("[NV] kv upgrade data length error!\r\n");
        return ERRCODE_FAIL;
    }

    uint8_t *image_data = (uint8_t *)kv_malloc(VERIFY_BUFF_LEN);
    if (image_data == NULL) {
        return ERRCODE_MALLOC;
    }

    while (offset < length) {
        read_len = VERIFY_BUFF_LEN;
        ret = kv_upg_copy_pkg_image_data(kv_data_offset, length, offset, &read_len, image_data);
        if (ret != ERRCODE_SUCC) {
            kv_free(image_data);
            return ret;
        }

        page_head = (kv_page_header_t*)image_data;
        core = kv_upg_convert_core_id(page_head->details.store_id);
        if (core == KV_STORE_MAX_NUM) {
            offset += VERIFY_BUFF_LEN;
            continue;
        }

        ret = kv_upg_process_one_page(core, image_data + sizeof(kv_page_header_t), read_len - sizeof(kv_page_header_t));
        if (ret != ERRCODE_SUCC) {
            nv_log_err("[NV] kv upgrade data error read_len_:%d offset:%d\r\n", read_len, offset);
            kv_free(image_data);
            return ret;
        }

        offset += VERIFY_BUFF_LEN;
    }
    kv_free(image_data);
    return ERRCODE_SUCC;
}

STATIC_UT errcode_t kv_upg_upgrade_process(fota_upgrade_flag_area_t* upg_flag)
{
    errcode_t ret;
    uint32_t hash_read_len = upg_flag->nv_hash_len;

    uint8_t *hash_data = (uint8_t *)kv_malloc(upg_flag->nv_hash_len);
    if (hash_data == NULL) {
        return ERRCODE_MALLOC;
    }

    ret = kv_upg_copy_pkg_image_data(upg_flag->nv_hash_offset, upg_flag->nv_hash_len,
                                     0, &hash_read_len, hash_data);
    if (ret != ERRCODE_SUCC) {
        kv_free(hash_data);
        return ret;
    }

    ret = kv_upg_upgrade_data_verify(upg_flag->nv_data_offset, upg_flag->nv_data_len, hash_data);
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] upg verify fail!\r\n");
        kv_free(hash_data);
        return ret;
    }

    kv_free(hash_data);

    ret = kv_upg_process_kv_data(upg_flag->nv_data_offset, upg_flag->nv_data_len);

    return ret;
}

#endif /* (defined(CONFIG_OTA_UPDATE_SUPPORT) && (CONFIG_NV_SUPPORT_OTA_UPDATE == NV_YES)) */

errcode_t nv_upg_upgrade_task_process(void)
{
#if (defined(CONFIG_OTA_UPDATE_SUPPORT) && (CONFIG_NV_SUPPORT_OTA_UPDATE == NV_YES))

    errcode_t ret;
    upg_image_status_switch_t status;
    fota_upgrade_flag_area_t *upg_flag_info = NULL;

    ret = upg_alloc_and_get_upgrade_flag(&upg_flag_info);
    if (ret != ERRCODE_SUCC || upg_flag_info == NULL) {
        nv_log_err("[NV] get upgrade flag error!\r\n");
        return ret;
    }

    if (kv_upg_upgrade_is_update_requested(upg_flag_info) == false) {
        nv_log_info("[NV] Not need to upgrade NV ...\r\n");
        upg_free(upg_flag_info);
        return ERRCODE_SUCC;
    }

    ret = upg_set_firmware_update_status(upg_flag_info, UPG_IMAGE_ID_NV, UPG_IMAGE_STATUS_SWITCH_TO_STARTED);
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] upg set update status error\r\n");
    }

    ret = kv_upg_upgrade_process(upg_flag_info);
    if (ret == ERRCODE_SUCC) {
        nv_log_info("[NV] kv upgrade success!\r\n");
        status = UPG_IMAGE_STATUS_SWITCH_TO_FINISHED;
    } else {
        nv_log_err("[NV] kv upgrade fail! error code:0x%x", ret);
        status = UPG_IMAGE_STATUS_SWITCH_TO_RETRY;
    }

    ret = upg_set_firmware_update_status(upg_flag_info, UPG_IMAGE_ID_NV, status);
    upg_free(upg_flag_info);

    /* 升级成功之后再进行NV-Hashtable的初始化 */
#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    ret = nv_key_map_init(KV_STORE_APPLICATION, kv_store_get_page_count(KV_STORE_APPLICATION));
#endif
    return ret;

#else
    return ERRCODE_SUCC;
#endif
}

