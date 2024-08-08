/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library store access interface
 */

#ifndef NV_STORE_H
#define NV_STORE_H

#include "nv_page.h"
#include "platform_types.h"
#include "nv.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/** Store ids for each core, with a minimum hamming distance of 10 between each value */
#define KV_STORE_ID_SCPU    0xCB7E
#define KV_STORE_ID_ACPU    0x254D
#define KV_STORE_ID_BACKUP  0x34B2

typedef enum {
    KV_STORE_APPLICATION,
    KV_STORE_MAX_NUM,
} kv_store_t;

typedef struct {
    uint16_t kvalue_max_length;
    uint16_t kvalue_actual_length;
    uint8_t *kvalue;
} kv_store_key_data_t;

cores_t kv_store_to_core(kv_store_t store);
kv_store_t kv_store_from_core(cores_t core);
uint16_t kv_store_get_id(kv_store_t store);
uint8_t kv_store_get_page_count(kv_store_t store);
errcode_t kv_store_get_page_handle(kv_store_t store, uint32_t page_index, kv_page_handle_t *page);
errcode_t kv_store_find_valid_key(kv_store_t store, kv_key_id key_id, kv_key_handle_t *key);
errcode_t kv_store_get_backup_key(kv_key_id key_id, kv_store_key_data_t *key_data, kv_attributes_t *attributes,
    kv_page_location page_location);
errcode_t kv_store_get_key_attr(kv_store_t store, kv_key_id key_id, uint16_t *len, kv_attributes_t *attributes);
errcode_t kv_store_get_key(kv_store_t store, kv_key_id key_id, kv_store_key_data_t *key_data,
                           kv_attributes_t *attributes);
errcode_t kv_store_get_status(kv_store_t store, nv_store_status_t *store_status);
errcode_t kv_store_find_write_page(kv_store_t store, uint32_t required_space, kv_page_handle_t *page,
                                   kv_page_status_t *page_status);
errcode_t kv_store_read_backup_key(kv_key_id key_id, kv_store_key_data_t *key_data, kv_attributes_t *attributes);
errcode_t kv_store_get_backup_key_attr(kv_key_id key_id, uint16_t *len,
    kv_attributes_t *attributes, kv_key_handle_t *backup_key);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_STORE_H_ */
