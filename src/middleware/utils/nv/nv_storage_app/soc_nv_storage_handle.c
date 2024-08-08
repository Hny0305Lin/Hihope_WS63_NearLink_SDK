/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2016-2022. All rights reserved.
 * Description: SOC KEY VALUE STORAGE IMPLEMENTATION
 */

#include "nv.h"
#include "nv_storage.h"
#include "nv_store.h"
#include "nv_upg.h"
#include "securec.h"
#include "nv_porting.h"
#include "common_def.h"
#include "nv_reset.h"
#include "nv_async_store.h"
#include "osal_semaphore.h"
#include "nv_update.h"
#include "nv_notify.h"
#include "nv_task_adapt.h"
#include "nv_storage_handle.h"

osal_semaphore nv_sem;
#define MAX_BINARY_VAL 1

STATIC nv_attributes_t nv_helper_convert_key_attr(const nv_key_attr_t *attr)
{
    nv_attributes_t attribute = NV_ATTRIBUTE_NORMAL;
    bool is_support_crypt = false;

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    is_support_crypt = attr->encrypted;
#endif

    if (attr->permanent && is_support_crypt) {
        attribute = NV_ATTRIBUTE_ENCRYPTED | NV_ATTRIBUTE_PERMANENT | NV_ATTRIBUTE_NON_UPGRADE;
    } else if (attr->permanent) {
        attribute = NV_ATTRIBUTE_PERMANENT | NV_ATTRIBUTE_NON_UPGRADE;
    } else if (is_support_crypt) {
        attribute = NV_ATTRIBUTE_ENCRYPTED | NV_ATTRIBUTE_NON_UPGRADE;
    } else if (attr->non_upgrade) {
        attribute = NV_ATTRIBUTE_NON_UPGRADE;
    }
    return attribute;
}

static uint16_t nv_storage_max_key_space(const nv_key_attr_t *attr)
{
    if (attr == NULL) {
        return NV_NORMAL_KVALUE_MAX_LEN;
    }
    return (attr->encrypted ? NV_ENCRYPTED_KVALUE_MAX_LEN : NV_NORMAL_KVALUE_MAX_LEN);
}

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
nv_direct_ctrl_t g_nv_direct_ctrl = { 0 };
nv_direct_ctrl_t *nv_direct_get_nv_ctrl(void)
{
    return &g_nv_direct_ctrl;
}
#endif

errcode_t nv_direct_get_key_attr(uint16_t key_id, uint16_t *len, nv_key_attr_t *attr)
{
    kv_attributes_t get_attribute = 0;
    errcode_t ret = ERRCODE_SUCC;

    if (len == NULL || attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    *len = 0;
    memset_s(attr, sizeof(nv_key_attr_t), 0, sizeof(nv_key_attr_t));
    ret = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, len, &get_attribute);
    if (ret == ERRCODE_SUCC) {
        if (((uint32_t)get_attribute & NV_ATTRIBUTE_PERMANENT) != 0) {
            attr->permanent = true;
            attr->non_upgrade = true;
        }
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
        if (((uint32_t)get_attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) {
            attr->encrypted = true;
            attr->non_upgrade = true;
        }
#endif
        if (((uint32_t)get_attribute & NV_ATTRIBUTE_NON_UPGRADE) != 0) {
            attr->non_upgrade = true;
        }
    }
    osal_sem_up(&nv_sem);
    return ret;
}

errcode_t nv_direct_write_with_attr(uint16_t key_id, const uint8_t *kvalue, uint16_t kvalue_length,
    nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    errcode_t ret;
    flash_task_node task_node = {0};
    uint16_t len;
    kv_attributes_t attribute = (kv_attributes_t)0;
    unused(func);

    if (kvalue_length > nv_storage_max_key_space(attr)) {
        nv_log_err("[NV] nv_direct_write_with_attr: len exceeds the max size. key_id = 0x%x\r\n", key_id);
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        nv_log_err("[NV] nv_direct_write_with_attr: semaphore error!\r\n");
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, &len, &attribute);
    if (ret == ERRCODE_SUCC) {
        bool check_encry = false;
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
        check_encry = ((((uint32_t)attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) &&
                      (attr == NULL || (attr->encrypted == false)));
#endif
        if ((((uint32_t)attribute & NV_ATTRIBUTE_PERMANENT) != 0) || check_encry) {
            /* When old key is permanent, not permit to write */
            /* When old key is encrypted, new key is non-encrypted, not permit to write */
            /* When old key is non upgrade, new key is write by ordinary write, not permit to write */
            nv_log_err("[NV] nv_direct_write_with_attr: operation not allowed! key_id = 0x%x\r\n", key_id);
            osal_sem_up(&nv_sem);
            return ERRCODE_NV_ILLEGAL_OPERATION;
        }
    }
    if (attr != NULL) { attribute = (uint8_t)nv_helper_convert_key_attr(attr); }
    task_node.state_code = FLASH_TASK_READY;
    task_node.data.kv.key = key_id;
    task_node.data.kv.kvalue = kvalue;
    task_node.data.kv.kvalue_length = kvalue_length;
    task_node.data.kv.force_write = false;
    task_node.data.kv.attribute = attribute;
    ret = kv_update_write_key(KV_STORE_APPLICATION, &task_node);
    osal_sem_up(&nv_sem);
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] nv_direct_write_with_attr failed. key_id = 0x%x, ret = 0x%x\r\n", key_id, ret);
        return ret;
    }
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    nv_change_notify(key_id);
#endif
    return ERRCODE_SUCC;
}

#ifdef CONFIG_NV_SUPPORT_WRITE_FORCE
errcode_t nv_direct_write_force(uint16_t key_id, const uint8_t *kvalue, uint16_t kvalue_length)
{
    errcode_t ret;
    flash_task_node task_node = {0};
    uint16_t len;
    kv_attributes_t attribute = 0;

    if (kvalue_length > NV_NORMAL_KVALUE_MAX_LEN) {
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, &len, &attribute);
    if (ret == ERRCODE_SUCC) {
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
        if (((uint32_t)attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) {
            /* When old key is encrypted, not permit to write */
            osal_sem_up(&nv_sem);
            return ERRCODE_NV_ILLEGAL_OPERATION;
        }
#endif
    }

    task_node.state_code = FLASH_TASK_READY;
    task_node.data.kv.key = key_id;
    task_node.data.kv.kvalue = kvalue;
    task_node.data.kv.kvalue_length = kvalue_length;
    task_node.data.kv.force_write = true;
    task_node.data.kv.attribute = (uint8_t)NV_ATTRIBUTE_NORMAL;
    ret = kv_update_write_key(KV_STORE_APPLICATION, &task_node);
    osal_sem_up(&nv_sem);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    nv_change_notify(key_id);
#endif
    return ERRCODE_SUCC;
}
#endif

errcode_t nv_direct_get_key_data(uint16_t key_id, uint16_t kvalue_max_length, uint16_t *kvalue_length,
    uint8_t *kvalue, nv_key_attr_t *attr)
{
    kv_attributes_t data_attribute = 0;

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    kv_store_key_data_t key_data = {kvalue_max_length, 0, kvalue};
    errcode_t ret_val = kv_store_get_key(KV_STORE_APPLICATION, key_id, &key_data, &data_attribute);
    /* 如果在工作区没有找到该数据，可能存在数据被破坏情况，去备份区去找该数据 */
    if (ret_val != ERRCODE_SUCC) {
        ret_val = kv_store_read_backup_key(key_id, &key_data, &data_attribute);
    }

    if (ret_val == ERRCODE_SUCC) {
        if (((uint32_t)data_attribute & NV_ATTRIBUTE_PERMANENT) != 0) {
            attr->permanent = true;
            attr->non_upgrade = true;
        }
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
        if (((uint32_t)data_attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) {
            attr->encrypted = true;
            attr->non_upgrade = true;
        }
#endif
        if (((uint32_t)data_attribute & NV_ATTRIBUTE_NON_UPGRADE) != 0) {
            attr->non_upgrade = true;
        }
    }

    *kvalue_length = key_data.kvalue_actual_length;
    osal_sem_up(&nv_sem);
    return ret_val;
}

#ifdef CONFIG_NV_SUPPORT_UPDATE_ATTR
errcode_t nv_direct_update_key_attr(uint16_t key_id, nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    errcode_t ret_val;
    flash_task_node task_node = {0};
    uint16_t len;
    kv_attributes_t attribute = 0;
    nv_key_attr_t new_attr;
    unused(func);

    memcpy_s(&new_attr, sizeof(nv_key_attr_t), attr, sizeof(nv_key_attr_t));
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret_val = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, &len, &attribute);
    if (ret_val != ERRCODE_SUCC) {
        osal_sem_up(&nv_sem);
        return ret_val;
    }

    if (((uint32_t)attribute & NV_ATTRIBUTE_PERMANENT) != 0) {
        osal_sem_up(&nv_sem);
        return ERRCODE_SUCC;
    }
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) {
        /* if old key is encrypted, the new key must be encrypted too */
        new_attr.encrypted = true;
    }
#endif
    task_node.state_code = FLASH_TASK_READY;
    task_node.data.kv_attribute.key = key_id;
    task_node.data.kv_attribute.attribute = (uint8_t)nv_helper_convert_key_attr(&new_attr);
    ret_val = kv_update_modify_attribute(KV_STORE_APPLICATION, &task_node);
    osal_sem_up(&nv_sem);
    return ret_val;
}
#endif

#ifdef CONFIG_NV_SUPPORT_DELETE_KEY
errcode_t nv_direct_erase(uint16_t key_id)
{
    errcode_t ret_val;
    uint16_t len = 0;
    kv_attributes_t attribute = 0;
    flash_task_node task_node = {0};
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret_val = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, &len, &attribute);
    if (ret_val != ERRCODE_SUCC) {
        osal_sem_up(&nv_sem);
        return ret_val;
    }

    if (((uint32_t)attribute & KV_ATTRIBUTE_PERMANENT) != 0) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }

    task_node.data.kv_erase.key = key_id;
    ret_val = kv_update_erase_key(KV_STORE_APPLICATION, &task_node);
    osal_sem_up(&nv_sem);
    return ret_val;
}
#endif

errcode_t nv_direct_get_store_status(nv_store_status_t *status)
{
    errcode_t ret_val = ERRCODE_FAIL;
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    memset_s(status, sizeof(nv_store_status_t), 0, sizeof(nv_store_status_t));
    ret_val = kv_store_get_status(KV_STORE_APPLICATION, status);
    osal_sem_up(&nv_sem);
    return ret_val;
}

errcode_t nv_direct_stored(uint16_t key_id, uint16_t kvalue_length, const uint8_t *kvalue)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t *stored_kvalue = NULL;
    kv_attributes_t attribute = 0;

    if (kvalue_length > NV_NORMAL_KVALUE_MAX_LEN) {
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }

    stored_kvalue = (uint8_t *)(uintptr_t)kv_malloc(kvalue_length);
    if (stored_kvalue == NULL) {
        return ERRCODE_MALLOC;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        kv_free(stored_kvalue);
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    kv_store_key_data_t key_data = {kvalue_length, 0, stored_kvalue};
    if (kv_store_get_key(KV_STORE_APPLICATION, key_id, &key_data, &attribute) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    } else if ((key_data.kvalue_actual_length != kvalue_length) ||
               (memcmp((void *)stored_kvalue, (void *)kvalue, kvalue_length) != 0)) {
        ret = ERRCODE_FAIL;
    }
    osal_sem_up(&nv_sem);
    kv_free(stored_kvalue);
    return ret;
}

errcode_t nv_direct_backup_keys(const nv_backup_mode_t *backup_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    errcode_t ret;

#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    /* 保证备份前，NV消息队列中数据写入flash */
    ret = uapi_nv_flush();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret = kv_restore_set_region_flag(backup_mode->region_mode);
    if (ret != ERRCODE_SUCC) {
        osal_sem_up(&nv_sem);
        return ret;
    }
    ret = kv_backup_delete_repeat_key();
    if (ret != ERRCODE_SUCC) {
        osal_sem_up(&nv_sem);
        return ret;
    }
    ret = kv_backup_write_key();
    osal_sem_up(&nv_sem);
    return ret;
#else
    unused(backup_mode);
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t nv_direct_set_restore_flag_all(void)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    nv_reset_mode_t nv_reset_mode = {0};
    nv_reset_mode.mode = RESET_MODE_A;
    return kv_enable_restore_flag(&nv_reset_mode);
#else
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t nv_direct_set_restore_flag_partitial(const nv_restore_mode_t *nv_restore_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if (nv_restore_mode == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }
    nv_reset_mode_t nv_reset_mode = {0};
    if (memcpy_s(nv_reset_mode.region_flag, KEY_ID_REGION_MAX_NUM, nv_restore_mode, KEY_ID_REGION_MAX_NUM) != EOK) {
        return ERRCODE_MEMCPY;
    }
    nv_reset_mode.mode = RESET_MODE_B;
    return kv_enable_restore_flag(&nv_reset_mode);
#else
    unused(nv_restore_mode);
    return ERRCODE_NOT_SUPPORT;
#endif
}

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
errcode_t nv_direct_add_func_to_notify_list(uint16_t min_key, uint16_t max_key, nv_changed_notify_func func)
{
    nv_direct_ctrl_t *nv_ctrl = nv_direct_get_nv_ctrl();
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    nv_changed_proc_t *notify_list = nv_ctrl->nv_change_notify_list;
    if (notify_list == NULL) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_NOT_INITIALISED;
    }

    if (nv_ctrl->notify_registered_nums >= nv_ctrl->notify_regitser_max_nums) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_NOTIFY_LIST_FULL;
    }

    if (!nv_change_notify_segment_is_valid(nv_ctrl, min_key, max_key)) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_NOTIFY_SEGMENT_ERR;
    }

    uint8_t new_index = nv_ctrl->notify_registered_nums;
    notify_list[new_index].min_key = min_key;
    notify_list[new_index].max_key = max_key;
    notify_list[new_index].func = func;
    nv_ctrl->notify_registered_nums++;
    osal_sem_up(&nv_sem);
    return ERRCODE_SUCC;
}
#endif

void nv_direct_ctrl_init(void)
{
    (void)kv_update_init((cores_t)KV_STORE_APPLICATION);

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    nv_direct_ctrl_t *nv_ctrl = nv_direct_get_nv_ctrl();
    memset_s(nv_ctrl, sizeof(nv_direct_ctrl_t), 0, sizeof(nv_direct_ctrl_t));
#endif

    if (osal_sem_binary_sem_init(&nv_sem, MAX_BINARY_VAL) != ERRCODE_SUCC) {
        return;
    }
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    (void)nv_direct_notify_list_init();
#endif
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    (void)kv_update_backup_init();
#endif
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    (void)kv_restore_all_keys();
#endif
#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    (void)nv_async_init(KV_STORE_APPLICATION, kv_store_get_page_count(KV_STORE_APPLICATION));
#endif
    nv_log_info("nv init success!\r\n");
}
