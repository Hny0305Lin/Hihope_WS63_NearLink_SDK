/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides KM driver source \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "km.h"
#include "kapi_km.h"
#include "securec.h"

#include "sl_common.h"
#include "crypto_common_macro.h"
#include "crypto_osal_adapt.h"

#define EFFECTIVE_KEY_LENGTH 28
#define CLEAR_KEY_LENGTH 128

errcode_t uapi_drv_km_init(void)
{
    return crypto_sl_common_get_errcode(0);
}
errcode_t uapi_drv_km_deinit(void)
{
    int32_t ret = kapi_km_deinit();
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_keyslot_create(uint32_t *keyslot_handle, uapi_drv_keyslot_type_t keyslot_type)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_keyslot_create(keyslot_handle, (crypto_keyslot_type)keyslot_type);
    if (ret != TD_SUCCESS) {
        uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    }
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_keyslot_destroy(uint32_t keyslot_handle)
{
    int32_t ret = kapi_keyslot_destroy(keyslot_handle);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}


errcode_t uapi_drv_klad_create(uint32_t *klad_handle)
{
    int32_t ret = kapi_klad_create(klad_handle);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_klad_destroy(uint32_t klad_handle)
{
    int32_t ret = kapi_klad_destroy(klad_handle);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_klad_attach(uint32_t klad_handle, uapi_drv_klad_dest_t klad_type, uint32_t keyslot_handle)
{
    int32_t ret = kapi_klad_attach(klad_handle, (crypto_klad_dest)klad_type, keyslot_handle);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_klad_detach(uint32_t klad_handle, uapi_drv_klad_dest_t klad_type, uint32_t keyslot_handle)
{
    int32_t ret = kapi_klad_detach(klad_handle, (crypto_klad_dest)klad_type, keyslot_handle);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_klad_set_attr(uint32_t klad_handle, const uapi_drv_klad_attr_t *attr)
{
    crypto_klad_attr attr_to = { 0 };
    int32_t ret = 0;

    crypto_chk_return(attr == NULL, ERRCODE_INVALID_PARAM, "klad_attr is NULL\n");

    attr_to.klad_cfg.rootkey_type = attr->klad_cfg.rootkey_type;

    attr_to.key_cfg.decrypt_support = attr->key_cfg.decrypt_support;
    attr_to.key_cfg.encrypt_support = attr->key_cfg.encrypt_support;
    attr_to.key_cfg.engine = attr->key_cfg.engine;

    attr_to.key_sec_cfg.key_sec = attr->key_sec_cfg.key_sec;
    attr_to.key_sec_cfg.master_only_enable = attr->key_sec_cfg.master_only_enable;
    attr_to.key_sec_cfg.src_buf_sec_support = attr->key_sec_cfg.src_buf_sec_support;
    attr_to.key_sec_cfg.src_buf_non_sec_support = attr->key_sec_cfg.src_buf_non_sec_support;
    attr_to.key_sec_cfg.dest_buf_sec_support = attr->key_sec_cfg.dest_buf_non_sec_support;
    attr_to.key_sec_cfg.dest_buf_non_sec_support = attr->key_sec_cfg.dest_buf_non_sec_support;

    ret = kapi_klad_set_attr(klad_handle, &attr_to);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_klad_get_attr(uint32_t klad_handle, uapi_drv_klad_attr_t *attr)
{
    crypto_klad_attr attr_from = { 0 };
    int32_t ret = 0;

    crypto_chk_return(attr == NULL, ERRCODE_INVALID_PARAM, "klad_attr is NULL\n");

    ret = kapi_klad_get_attr(klad_handle, &attr_from);

    attr->klad_cfg.rootkey_type = attr_from.klad_cfg.rootkey_type;

    attr->key_cfg.decrypt_support = attr_from.key_cfg.decrypt_support;
    attr->key_cfg.encrypt_support = attr_from.key_cfg.encrypt_support;
    attr->key_cfg.engine = attr_from.key_cfg.engine;

    attr->key_sec_cfg.key_sec = attr_from.key_sec_cfg.key_sec;
    attr->key_sec_cfg.master_only_enable = attr_from.key_sec_cfg.master_only_enable;
    attr->key_sec_cfg.src_buf_sec_support = attr_from.key_sec_cfg.src_buf_sec_support;
    attr->key_sec_cfg.src_buf_non_sec_support = attr_from.key_sec_cfg.src_buf_non_sec_support;
    attr->key_sec_cfg.dest_buf_non_sec_support = attr_from.key_sec_cfg.dest_buf_sec_support;
    attr->key_sec_cfg.dest_buf_non_sec_support = attr_from.key_sec_cfg.dest_buf_non_sec_support;

    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_klad_set_effective_key(uint32_t klad_handle, const uapi_drv_klad_effective_key_t *key)
{
    crypto_klad_effective_key key_to = { 0 };
    td_u8 salt[EFFECTIVE_KEY_LENGTH] = { 0 };
    int32_t ret = 0;

    crypto_chk_return(key == NULL, ERRCODE_INVALID_PARAM, "effective_key is NULL\n");

    key_to.kdf_hard_alg = key->kdf_hard_alg;
    key_to.key_parity = key->key_parity;
    key_to.key_size = key->key_size;
    key_to.oneway = key->oneway;
    key_to.salt_length = key->salt_length;

    ret = memcpy_s(salt, sizeof(salt), key->salt, key->salt_length);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    key_to.salt = salt;

    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    ret = kapi_klad_set_effective_key(klad_handle, &key_to);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_klad_set_clear_key(uint32_t klad_handle, const uapi_drv_klad_clear_key_t *key)
{
    crypto_klad_clear_key key_to = { 0 };
    td_u8 key_temp[CLEAR_KEY_LENGTH] = { 0 };
    int32_t ret = 0;

    crypto_chk_return(key == NULL, ERRCODE_INVALID_PARAM, "clear_key is NULL\n");

    key_to.key_length = key->key_length;
    key_to.key_parity = key->key_parity;
    key_to.hmac_type = key->hmac_type;

    ret = memcpy_s(key_temp, sizeof(key_temp), key->key, key->key_length);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    key_to.key = key_temp;

    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    ret = kapi_klad_set_clear_key(klad_handle, &key_to);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_kdf_update(uapi_drv_kdf_otp_key_t otp_key, uapi_drv_kdf_update_alg_t alg)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_kdf_update((crypto_kdf_otp_key)otp_key, (crypto_kdf_update_alg)alg);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}