/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides cipher driver source \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "cipher.h"
#include "kapi_symc.h"
#include "kapi_hash.h"

#include "sl_common.h"
#include "crypto_common_macro.h"

errcode_t uapi_drv_cipher_symc_init(void)
{
    int32_t ret = kapi_cipher_symc_init();
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_deinit(void)
{
    int32_t ret = kapi_cipher_symc_deinit();
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_create(uint32_t *symc_handle, const uapi_drv_cipher_symc_attr_t *symc_attr)
{
    int32_t ret = 0;
    crypto_symc_attr symc_attr_to = { 0 };

    crypto_chk_return(symc_attr == NULL, ERRCODE_INVALID_PARAM, "hash_attr is NULL\n");

    symc_attr_to.work_mode = symc_attr->work_mode;
    symc_attr_to.symc_type = symc_attr->symc_type;
    symc_attr_to.symc_alg = symc_attr->symc_alg;
    symc_attr_to.is_long_term = symc_attr->is_long_term;

    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    ret = kapi_cipher_symc_create(symc_handle, &symc_attr_to);
    if (ret != TD_SUCCESS) {
        uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    }
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_destroy(uint32_t symc_handle)
{
    int32_t ret = kapi_cipher_symc_destroy(symc_handle);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_set_config(uint32_t symc_handle, const uapi_drv_cipher_symc_ctrl_t *symc_ctrl)
{
    int32_t ret = kapi_cipher_symc_set_config(symc_handle, (const crypto_symc_ctrl_t *)symc_ctrl);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_get_config(uint32_t symc_handle, const uapi_drv_cipher_symc_ctrl_t *symc_ctrl)
{
    int32_t ret = kapi_cipher_symc_get_config(symc_handle, (crypto_symc_ctrl_t *)symc_ctrl);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_attach(uint32_t symc_handle, uint32_t keyslot_handle)
{
    int32_t ret = kapi_cipher_symc_attach(symc_handle, keyslot_handle);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_detach(uint32_t symc_handle, uint32_t keyslot_handle)
{
    int32_t ret = kapi_cipher_symc_detach(symc_handle, keyslot_handle);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_encrypt(uint32_t symc_handle, const uapi_drv_cipher_buf_attr_t *src_buf,
    const uapi_drv_cipher_buf_attr_t *dst_buf, uint32_t length)
{
    int32_t ret = kapi_cipher_symc_encrypt(symc_handle, (const crypto_buf_attr *)src_buf,
        (const crypto_buf_attr *)dst_buf, length);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_decrypt(uint32_t symc_handle, const uapi_drv_cipher_buf_attr_t *src_buf,
    const uapi_drv_cipher_buf_attr_t *dst_buf, uint32_t length)
{
    int32_t ret = kapi_cipher_symc_decrypt(symc_handle, (const crypto_buf_attr *)src_buf,
        (const crypto_buf_attr *)dst_buf, length);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_symc_get_tag(uint32_t symc_handle, uint8_t *tag, uint32_t tag_length)
{
    int32_t ret = kapi_cipher_symc_get_tag(symc_handle, tag, tag_length);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_mac_start(uint32_t *symc_handle, const uapi_drv_cipher_symc_mac_attr_t *mac_attr)
{
    int32_t ret;
    crypto_symc_mac_attr mac_attr_to = {0};

    crypto_chk_return(mac_attr == NULL, ERRCODE_INVALID_PARAM, "hash_attr is NULL\n");

    mac_attr_to.work_mode = mac_attr->work_mode;
    mac_attr_to.symc_key_length = mac_attr->symc_key_length;
    mac_attr_to.symc_alg = mac_attr->symc_alg;
    mac_attr_to.keyslot_chn = mac_attr->keyslot_chn;
    mac_attr_to.is_long_term = mac_attr->is_long_term;

    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    ret = kapi_cipher_mac_start(symc_handle, &mac_attr_to);
    if (ret != TD_SUCCESS) {
        uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    }
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_mac_update(uint32_t symc_handle, const uapi_drv_cipher_buf_attr_t *src_buf, uint32_t length)
{
    int32_t ret = kapi_cipher_mac_update(symc_handle, (const crypto_buf_attr *)src_buf, length);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_mac_finish(uint32_t symc_handle, uint8_t *mac, uint32_t *mac_length)
{
    int32_t ret = kapi_cipher_mac_finish(symc_handle, mac, mac_length);
    if (ret == TD_SUCCESS) {
        uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    }
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_init(void)
{
    int32_t ret = kapi_cipher_hash_init();
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_deinit(void)
{
    int32_t ret = kapi_cipher_hash_deinit();
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_start(uint32_t *hash_handle, const uapi_drv_cipher_hash_attr_t *hash_attr)
{
    int32_t ret;
    crypto_hash_attr hash_attr_to = {0};

    crypto_chk_return(hash_attr == NULL, ERRCODE_INVALID_PARAM, "hash_attr is NULL\n");

    hash_attr_to.key = hash_attr->key;
    hash_attr_to.key_len = hash_attr->key_len;
    hash_attr_to.drv_keyslot_handle = hash_attr->keyslot_handle;
    hash_attr_to.is_keyslot = hash_attr->is_keyslot;
    hash_attr_to.is_long_term = hash_attr->is_long_term;
    hash_attr_to.hash_type = hash_attr->hash_type;

    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    ret = kapi_cipher_hash_start(hash_handle, &hash_attr_to);
    if (ret != TD_SUCCESS) {
        uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    }
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_update(uint32_t hash_handle,  const uapi_drv_cipher_buf_attr_t *src_buf,
    const uint32_t len)
{
    crypto_buf_attr drv_src_buf = {0};
    crypto_chk_return(src_buf == NULL, ERROR_SECURITY_INVALID_PARAM, "hash_attr is NULL\n");

    drv_src_buf.virt_addr = src_buf->virt_addr;
    int32_t ret = kapi_cipher_hash_update(hash_handle, &drv_src_buf, len);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_finish(uint32_t hash_handle, uint8_t *out, uint32_t *out_len)
{
    int32_t ret;

    ret = kapi_cipher_hash_finish(hash_handle, out, out_len);
    if (ret == TD_SUCCESS) {
        uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    }
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_get(uint32_t hash_handle, uapi_drv_cipher_hash_clone_ctx_t *hash_clone_ctx)
{
    int32_t ret = kapi_cipher_hash_get(hash_handle, (crypto_hash_clone_ctx *)hash_clone_ctx);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_set(uint32_t hash_handle, const uapi_drv_cipher_hash_clone_ctx_t *hash_clone_ctx)
{
    int32_t ret = kapi_cipher_hash_set(hash_handle, (crypto_hash_clone_ctx *)hash_clone_ctx);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hash_destroy(uint32_t hash_handle)
{
    int32_t ret = kapi_cipher_hash_destroy(hash_handle);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pbkdf2(const uapi_drv_cipher_kdf_pbkdf2_param_t *param, uint8_t *out, const uint32_t out_len)
{
    int32_t ret;

    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    ret = kapi_cipher_pbkdf2((const crypto_kdf_pbkdf2_param *)param, out, out_len);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hkdf_extract(uapi_drv_cipher_hkdf_extract_t *extract_param,
    uint8_t *prk, uint32_t *prk_length)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_cipher_hkdf_extract((crypto_hkdf_extract_t *)extract_param, prk, prk_length);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hkdf_expand(const uapi_drv_cipher_hkdf_expand_t *expand_param,
    uint8_t *okm, uint32_t okm_length)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_cipher_hkdf_expand((crypto_hkdf_expand_t *)expand_param, okm, okm_length);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_hkdf(uapi_drv_cipher_hkdf_t *hkdf_param, uint8_t *okm, uint32_t okm_length)
{
    uapi_pm_add_sleep_veto(PM_SECURITY_VETO_ID);
    int32_t ret = kapi_cipher_hkdf((crypto_hkdf_t *)hkdf_param, okm, okm_length);
    uapi_pm_remove_sleep_veto(PM_SECURITY_VETO_ID);
    return crypto_sl_common_get_errcode(ret);
}