/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2016-2022. All rights reserved.
 * Description: SOC KEY VALUE STORAGE IMPLEMENTATION
 */

#include "nv.h"
#include "nv_storage.h"
#include "nv_store.h"
#include "nv_reset.h"
#include "securec.h"
#include "nv_porting.h"
#include "common_def.h"
#include "nv_config.h"
#include "systick.h"
#include "nv_async_store.h"
#include "nv_update.h"
#include "nv_notify.h"
#include "nv_task_adapt.h"
#include "nv_storage_handle.h"

void uapi_nv_init(void)
{
    (void)nv_direct_ctrl_init();
}

errcode_t uapi_nv_write_with_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length, nv_key_attr_t *attr,
                                  nv_storage_completed_callback func)
{
    errcode_t ret;

    if (kvalue == NULL || kvalue_length == 0) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT != NV_YES)
    if ((attr != NULL && attr->encrypted == true)) {
        nv_log_err("[NV] nv_direct_write_with_attr: encryption not support!\r\n");
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }
#endif

#ifdef KV_DEBUG
    uint64_t start_time = uapi_systick_get_ms();
#endif

#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    ret = nv_async_write_with_attr(key, kvalue, kvalue_length, attr, func);
#else
    ret = nv_direct_write_with_attr(key, kvalue, kvalue_length, attr, func);
#endif

#ifdef KV_DEBUG
    uint64_t end_time = uapi_systick_get_ms();
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] write key fail! key_id= 0x%x, ret 0x%x\r\n", key, ret);
    }

    nv_log_debug("[NV] write key (key_id = 0x%x len = %d) ret = %d. takes %lld ms\r\n",
        key, kvalue_length, ret, (end_time - start_time));
    unused(start_time);
    unused(end_time);
#endif
    return ret;
}

errcode_t uapi_nv_write(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    return uapi_nv_write_with_attr(key, kvalue, kvalue_length, NULL, NULL);
}

errcode_t uapi_nv_write_force(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    if (kvalue == NULL || kvalue_length == 0) {
        return ERRCODE_NV_INVALID_PARAMS;
    }
#ifdef CONFIG_NV_SUPPORT_WRITE_FORCE
    return nv_direct_write_force(key, kvalue, kvalue_length);
#endif
    unused(key);
    return ERRCODE_NOT_SUPPORT;
}

errcode_t uapi_nv_update_key_attr(uint16_t key, nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    if (attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }
#ifdef CONFIG_NV_SUPPORT_UPDATE_ATTR
    return nv_direct_update_key_attr(key, attr, func);
#endif
    unused(key);
    unused(func);
    return ERRCODE_NOT_SUPPORT;
}

errcode_t uapi_nv_get_key_attr(uint16_t key, uint16_t *length, nv_key_attr_t *attr)
{
    if (attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    return nv_direct_get_key_attr(key, length, attr);
}

errcode_t uapi_nv_read_with_attr(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue,
                                 nv_key_attr_t *attr)
{
    errcode_t ret;
    if (kvalue_length == NULL || kvalue == NULL || attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

#ifdef KV_DEBUG
    uint64_t start_time = uapi_systick_get_ms();
#endif

#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    ret = nv_async_read_with_attr(key, kvalue_max_length, kvalue_length, kvalue, attr);
#else
    ret = nv_direct_get_key_data(key, kvalue_max_length, kvalue_length, kvalue, attr);
#endif

#ifdef KV_DEBUG
    uint64_t end_time = uapi_systick_get_ms();
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] read key fail! key_id= 0x%x, ret 0x%x\r\n", key, ret);
    }
    nv_log_debug("[NV] read key (key_id = 0x%x len = %d) ret = 0x%x. takes %lld ms\r\n",
        key, *kvalue_length, ret, (end_time - start_time));
    unused(start_time);
    unused(end_time);
#endif
    return ret;
}

errcode_t uapi_nv_read(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue)
{
    nv_key_attr_t attr;
    return uapi_nv_read_with_attr(key, kvalue_max_length, kvalue_length, kvalue, &attr);
}

errcode_t uapi_nv_delete_key(uint16_t key)
{
#ifdef CONFIG_NV_SUPPORT_DELETE_KEY
    return nv_direct_erase(key);
#else
    unused(key);
    return ERRCODE_NOT_SUPPORT;
#endif
}

bool uapi_nv_is_stored(uint16_t key, uint16_t kvalue_length, const uint8_t *kvalue)
{
    if (kvalue_length == 0 || kvalue == NULL) {
        return false;
    }

    errcode_t ret = nv_direct_stored(key, kvalue_length, kvalue);
    if (ret == ERRCODE_SUCC) {
        return true;
    } else {
        return false;
    }
}

errcode_t uapi_nv_get_store_status(nv_store_status_t *status)
{
    if (status == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

#ifdef KV_DEBUG
    uint64_t start_time = uapi_systick_get_ms();
#endif
    errcode_t ret = nv_direct_get_store_status(status);
#ifdef KV_DEBUG
    uint64_t end_time = uapi_systick_get_ms();
    nv_log_debug("[NV] get store status take %lld ms\r\n", (end_time - start_time));
    unused(start_time);
    unused(end_time);
#endif
    return ret;
}

errcode_t uapi_nv_backup(const nv_backup_mode_t *backup_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if (backup_mode == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    return nv_direct_backup_keys(backup_mode);
#else
    unused(backup_mode);
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t uapi_nv_set_restore_mode_all(void)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    return nv_direct_set_restore_flag_all();
#else
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t uapi_nv_set_restore_mode_partitial(const nv_restore_mode_t *restore_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    return nv_direct_set_restore_flag_partitial(restore_mode);
#else
    unused(restore_mode);
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t uapi_nv_flush(void)
{
#if (CONFIG_NV_SUPPORT_ASYNCHRONOUS_STORE == NV_YES)
    return nv_async_flush();
#else
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t uapi_nv_register_change_notify_proc(uint16_t min_key, uint16_t max_key, nv_changed_notify_func func)
{
    if ((min_key > max_key) || (func == NULL)) {
        return ERRCODE_NV_INVALID_PARAMS;
    }
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    if (nv_direct_get_nv_ctrl()->notify_regitser_max_nums == 0) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    return nv_direct_add_func_to_notify_list(min_key, max_key, func);
#endif
    return ERRCODE_NOT_SUPPORT;
}
