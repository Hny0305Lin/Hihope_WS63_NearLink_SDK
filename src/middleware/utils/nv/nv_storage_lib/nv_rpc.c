/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library remote RPC handler implementations
 */
#ifndef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
#include "nv_rpc.h"
#include "rpc_auto_generated.h"
#include "rpc_interface.h"
#include "nv_store.h"
#include "nv_porting.h"

errcode_t uapi_rpc_remote_cmd_kv_store_get_status(cores core, uint16_t *store_status_length,
                                                  rpc_auto_generated_release_func *store_status_free_mem_callback,
                                                  uint8_t **store_status)
{
    errcode_t call_err;
    kv_store_t store;
    unused(call_err);
    /* Initialise pass-back values in case we fail early */
    *store_status = NULL;
    *store_status_length = 0;

    uint32_t status_size = sizeof(nv_store_status_t);
    *store_status = osal_kmalloc(sizeof(nv_store_status_t), OSAL_GFP_KERNEL);
    if (*store_status == NULL) {
        return ERRCODE_MALLOC;
    }
    *store_status_length = status_size;
    *store_status_free_mem_callback = osal_kfree;
    store = kv_store_from_core(core);
    return kv_store_get_status(store, *store_status);
}

errcode_t uapi_rpc_remote_cmd_kv_store_get_key(cores core, uint16_t key_id, uint16_t *kvalue_length,
                                               rpc_auto_generated_release_func *kvalue_free_mem_callback,
                                               uint8_t **kvalue)
{
    errcode_t call_err;
    kv_store_t store;
    kv_key_handle_t key;

    /* Initialise pass-back values in case we fail early */
    *kvalue = NULL;
    *kvalue_length = 0;

    store = kv_store_from_core(core);
    call_err = kv_store_find_valid_key(store, key_id, &key);
    if (call_err != ERRCODE_SUCC) {
        return call_err;
    }

    /* Attempt to obtain key data from store */
    *kvalue = osal_kmalloc(key.header.length, OSAL_GFP_KERNEL);
    if (*kvalue == NULL) {
        return ERRCODE_MALLOC;
    }
    *kvalue_length = key.header.length;
    *kvalue_free_mem_callback = osal_kfree;

    return kv_key_read_data(&key, *kvalue);
}

errcode_t uapi_rpc_remote_cmd_kv_store_get_key_by_addr(cores core,
                                                       uint16_t key_id,
                                                       uint32_t kvalue_addr,
                                                       uint16_t length)
{
    errcode_t call_err;
    kv_store_t store;
    kv_key_handle_t key;

    store = kv_store_from_core(core);
    call_err = kv_store_find_valid_key(store, key_id, &key);
    if (call_err != ERRCODE_SUCC) {
        return call_err;
    }

    /* Attempt to obtain key data from store */
    if (length < key.header.length) {
        return ERRCODE_NV_GET_BUFFER_TOO_SMALL;
    }
    return kv_key_read_data(&key, (uint8_t *)(uintptr_t)kvalue_addr);
}

errcode_t uapi_rpc_remote_cmd_kv_store_get_key_attr(cores core, uint16_t key_id, uint16_t *len, uint8_t *attributes)
{
    kv_store_t store;

    /* Initialise pass-back value in case we fail early */
    *attributes = 0;
    *len = 0;

    store = kv_store_from_core(core);
    return kv_store_get_key_attr(store, (kv_key_id)key_id, len, (kv_attributes_t *)attributes);
}
#endif
