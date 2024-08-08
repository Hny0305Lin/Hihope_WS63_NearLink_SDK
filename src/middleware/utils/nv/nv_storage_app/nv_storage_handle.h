/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: KEY VALUE STORAGE INTERNAL INTERFACE
 */

#ifndef NV_STORAGE_HANDLE_H
#define NV_STORAGE_HANDLE_H

#include "nv_notify.h"
#include "nv.h"

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

errcode_t nv_direct_get_key_attr(uint16_t key_id, uint16_t *len, nv_key_attr_t *attr);

errcode_t nv_direct_write_with_attr(uint16_t key_id, const uint8_t *kvalue, uint16_t kvalue_length,
    nv_key_attr_t *attr, nv_storage_completed_callback func);

errcode_t nv_direct_write_force(uint16_t key_id, const uint8_t *kvalue, uint16_t kvalue_length);

errcode_t nv_direct_get_key_data(uint16_t key_id, uint16_t kvalue_max_length, uint16_t *kvalue_length,
    uint8_t *kvalue, nv_key_attr_t *attr);

errcode_t nv_direct_update_key_attr(uint16_t key_id, nv_key_attr_t *attr, nv_storage_completed_callback func);

errcode_t nv_direct_erase(uint16_t key_id);

errcode_t nv_direct_get_store_status(nv_store_status_t *status);

errcode_t nv_direct_stored(uint16_t key_id, uint16_t kvalue_length, const uint8_t *kvalue);

errcode_t nv_direct_backup_keys(const nv_backup_mode_t *backup_mode);

errcode_t nv_direct_set_restore_flag_all(void);

errcode_t nv_direct_set_restore_flag_partitial(const nv_restore_mode_t *nv_restore_mode);

errcode_t nv_direct_add_func_to_notify_list(uint16_t min_key, uint16_t max_key, nv_changed_notify_func func);

void nv_direct_ctrl_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_STORAGE_HANDLE_H */