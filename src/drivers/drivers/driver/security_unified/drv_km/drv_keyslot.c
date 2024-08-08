/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver keyslot. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "drv_keyslot.h"

#include "crypto_drv_common.h"
#include "crypto_common_macro.h"

#define EXT_ID_KM      0x55

#define id_2_handle(id, key)    \
    (td_handle)((((EXT_ID_KM) & 0xff) << 24) | ((((key) & 0xff)<< 16)) | (((id) & 0xffff)))
#define handle_2_modid(handle)      (((handle) >> 24) & 0xff)
#define handle_2_id(handle)         (((handle)) & 0xffff)
#define handle_get_type(handle)     (((handle) >> 16) & 0xff)

#define KEYSLOT_MCIPHER_KEYSLOT_CNT     8
#define KEYSLOT_HMAC_KEYSLOT_CNT        2

td_s32 drv_keyslot_init(td_void)
{
    return TD_SUCCESS;
}

td_s32 drv_keyslot_deinit(td_void)
{
    return TD_SUCCESS;
}

td_s32 drv_keyslot_create(td_handle *keyslot_handle, crypto_keyslot_type keyslot_type)
{
    volatile td_s32 ret = TD_FAILURE;
    td_u32 i;
    td_u32 keyslot_cnt = 0;
    crypto_drv_func_enter();

    crypto_chk_return(keyslot_handle == TD_NULL, TD_FAILURE, "keyslot_handle is NULL\n");

    if (keyslot_type == CRYPTO_KEYSLOT_TYPE_FLASH) {
        *keyslot_handle = 0;
        return TD_SUCCESS;
    }

    if (keyslot_type == CRYPTO_KEYSLOT_TYPE_MCIPHER) {
        keyslot_cnt = KEYSLOT_MCIPHER_KEYSLOT_CNT;
    } else if (keyslot_type == CRYPTO_KEYSLOT_TYPE_HMAC) {
        keyslot_cnt = KEYSLOT_HMAC_KEYSLOT_CNT;
    } else {
        crypto_log_err("invalid keyslot_type\n");
        return TD_FAILURE;
    }

    for (i = 0; i < keyslot_cnt; i++) {
        ret = hal_keyslot_lock(i, keyslot_type);
        if (ret == TD_SUCCESS) {
            break;
        }
    }
    if (i >= keyslot_cnt) {
        crypto_log_err("all keyslot channels are busy\n");
        return TD_FAILURE;
    }

    *keyslot_handle = id_2_handle(i, (td_u32)keyslot_type);
    crypto_drv_func_exit();
    return ret;
}

#define EXT_MODULE_ID_KM      0x55
static td_s32 inner_keyslot_handle_chk(td_handle keyslot_handle)
{
    /* CRYPTO_KEYSLOT_TYPE_FLASH */
    if (keyslot_handle == 0) {
        return TD_SUCCESS;
    }

    crypto_chk_return(handle_2_modid(keyslot_handle) != EXT_MODULE_ID_KM, TD_FAILURE, "invalid keyslot_handle\n");
    crypto_chk_return(handle_get_type(keyslot_handle) >= CRYPTO_KEYSLOT_TYPE_FLASH, TD_FAILURE,
        "invalid keyslot_handle\n");
    crypto_chk_return(handle_get_type(keyslot_handle) == CRYPTO_KEYSLOT_TYPE_MCIPHER &&
        handle_2_id(keyslot_handle) >= KEYSLOT_MCIPHER_KEYSLOT_CNT, TD_FAILURE, "invalid keyslot_handle\n");
    crypto_chk_return(handle_get_type(keyslot_handle) == CRYPTO_KEYSLOT_TYPE_HMAC &&
        handle_2_id(keyslot_handle) >= KEYSLOT_HMAC_KEYSLOT_CNT, TD_FAILURE, "invalid keyslot_handle\n");
    return TD_SUCCESS;
}

td_s32 drv_keyslot_destroy(td_handle keyslot_handle)
{
    int ret;
    crypto_keyslot_type keyslot_type_get;
    crypto_drv_func_enter();
    ret = inner_keyslot_handle_chk(keyslot_handle);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    keyslot_type_get = handle_get_type(keyslot_handle);

    ret = hal_keyslot_unlock(handle_2_id(keyslot_handle), keyslot_type_get);
    crypto_drv_func_exit();
    return ret;
}