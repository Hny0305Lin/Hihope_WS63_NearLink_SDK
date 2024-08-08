/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver klad. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "drv_klad.h"

#include "crypto_drv_common.h"
#include "crypto_common_macro.h"

typedef struct {
    crypto_kdf_hard_key_type hard_key_type;
    crypto_klad_dest klad_dest;
    crypto_klad_attr klad_attr;
    td_handle keyslot_handle;
    td_bool is_open;
    td_bool is_attached;
    td_bool is_set_attr;
} drv_klad_context;

static drv_klad_context g_klad_ctx = {0};

#define KLAD_VALID_HANDLE       0x2D3C4B5A

td_s32 drv_klad_create(td_handle *klad_handle)
{
    crypto_chk_return(klad_handle == TD_NULL, TD_FAILURE, "klad_handle is NULL\n");

    (td_void)memset_s(&g_klad_ctx, sizeof(drv_klad_context), 0, sizeof(drv_klad_context));

    g_klad_ctx.is_open = TD_TRUE;

    *klad_handle = KLAD_VALID_HANDLE;
    return TD_SUCCESS;
}

td_s32 drv_klad_destroy(td_handle klad_handle)
{
    crypto_chk_return(klad_handle != KLAD_VALID_HANDLE, TD_FAILURE, "invalid klad_handle\n");
    if (g_klad_ctx.is_open == TD_FALSE) {
        return TD_SUCCESS;
    }
    (td_void)memset_s(&g_klad_ctx, sizeof(drv_klad_context), 0, sizeof(drv_klad_context));

    return TD_SUCCESS;
}

td_s32 drv_klad_attach(td_handle klad_handle, crypto_klad_dest klad_dest, td_handle keyslot_handle)
{
    volatile td_s32 ret = TD_FAILURE;
    crypto_klad_flash_key_type flash_key_type = CRYPTO_KLAD_FLASH_KEY_TYPE_INVALID;

    crypto_chk_return(klad_handle != KLAD_VALID_HANDLE, TD_FAILURE, "invalid klad_handle\n");
    crypto_chk_return(g_klad_ctx.is_open == TD_FALSE, TD_FAILURE, "call create first\n");

    if (klad_dest == CRYPTO_KLAD_DEST_FLASH) {
        flash_key_type = (crypto_klad_flash_key_type)keyslot_handle;
    } else {
        ret = hal_klad_set_key_addr(klad_dest, keyslot_handle);
        crypto_chk_return(ret != TD_SUCCESS, ret, "hal_klad_set_key_addr failed\n");
    }

    ret = hal_klad_set_key_dest_cfg(klad_dest, flash_key_type);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_klad_set_key_dest_cfg failed\n");

    g_klad_ctx.keyslot_handle = keyslot_handle;
    g_klad_ctx.klad_dest = klad_dest;
    g_klad_ctx.is_attached = TD_TRUE;
    return ret;
}

td_s32 drv_klad_detach(td_handle klad_handle, crypto_klad_dest klad_dest, td_handle keyslot_handle)
{
    crypto_unused(keyslot_handle);
    crypto_chk_return(klad_handle != KLAD_VALID_HANDLE, TD_FAILURE, "invalid klad_handle\n");
    crypto_chk_return(g_klad_ctx.is_open == TD_FALSE, TD_FAILURE, "call create first\n");
    crypto_chk_return(g_klad_ctx.klad_dest != klad_dest, TD_FAILURE, "invalid klad_dest\n");
    crypto_chk_return(g_klad_ctx.keyslot_handle != keyslot_handle, TD_FAILURE, "invalid keyslot_handle\n");

    if (g_klad_ctx.is_attached == TD_FALSE) {
        return TD_SUCCESS;
    }

    g_klad_ctx.is_attached = TD_FALSE;
    g_klad_ctx.keyslot_handle = 0;
    g_klad_ctx.klad_dest = 0;

    return TD_SUCCESS;
}

td_s32 drv_klad_set_attr(td_handle klad_handle, const crypto_klad_attr *klad_attr)
{
    volatile td_s32 ret = TD_FAILURE;
    const crypto_klad_key_config *key_cfg = TD_NULL;
    const crypto_klad_key_secure_config *key_sec_cfg = TD_NULL;

    crypto_chk_return(klad_attr == TD_NULL, TD_FAILURE, "klad_attr is NULL\n");
    crypto_chk_return(klad_handle != KLAD_VALID_HANDLE, TD_FAILURE, "invalid klad_handle\n");
    crypto_chk_return(g_klad_ctx.is_open == TD_FALSE, TD_FAILURE, "call create first\n");

    ret = memcpy_s(&g_klad_ctx.klad_attr, sizeof(crypto_klad_attr), klad_attr, sizeof(crypto_klad_attr));
    crypto_chk_return(ret != EOK, TD_FAILURE, "memcpy_s failed\n");

    key_cfg = &(klad_attr->key_cfg);
    key_sec_cfg = &(klad_attr->key_sec_cfg);

    ret = hal_klad_set_key_crypto_cfg(key_cfg->encrypt_support, key_cfg->decrypt_support, key_cfg->engine);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_klad_set_key_crypto_cfg failed\n");

    ret = hal_klad_set_key_secure_cfg(key_sec_cfg);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_klad_set_key_secure_cfg failed\n");

    g_klad_ctx.hard_key_type = klad_attr->klad_cfg.rootkey_type;
    g_klad_ctx.is_set_attr = TD_TRUE;

    return ret;
}

td_s32 drv_klad_set_clear_key(td_handle klad_handle, const crypto_klad_clear_key *clear_key)
{
    volatile td_s32 ret = TD_FAILURE;

    crypto_chk_return(klad_handle != KLAD_VALID_HANDLE, TD_FAILURE, "invalid klad_handle\n");
    crypto_chk_return(g_klad_ctx.is_open == TD_FALSE, TD_FAILURE, "call create first\n");
    crypto_chk_return(g_klad_ctx.is_set_attr == TD_FALSE, TD_FAILURE, "call set_attr first\n");

    crypto_chk_return(clear_key == TD_NULL, TD_FAILURE, "clear_key is NULL\n");

    ret = hal_klad_start_clr_route(g_klad_ctx.klad_dest, clear_key);
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_klad_start_clr_route failed\n");

    return ret;
}

td_s32 drv_klad_set_effective_key(td_handle klad_handle, const crypto_klad_effective_key *content_key)
{
    volatile td_s32 ret = TD_FAILURE;
    crypto_kdf_hard_calc_param param = {0};

    crypto_chk_return(content_key == TD_NULL, TD_FAILURE, "content_key is NULL\n");
    crypto_chk_return(content_key->salt == TD_NULL, TD_FAILURE, "content_key->salt is NULL\n");
    if (content_key->kdf_hard_alg == CRYPTO_KDF_HARD_ALG_SM3) {
        crypto_chk_return(crypto_sm_support(CRYPTO_SM_ALG_SM3) == TD_FALSE, TD_FAILURE,
            "alg is unsupport\n");
    }

    crypto_chk_return(klad_handle != KLAD_VALID_HANDLE, TD_FAILURE, "invalid klad_handle\n");
    crypto_chk_return(g_klad_ctx.is_open == TD_FALSE, TD_FAILURE, "call create first\n");
    crypto_chk_return(g_klad_ctx.is_set_attr == TD_FALSE, TD_FAILURE, "call set_attr first\n");

    switch (content_key->key_size) {
        case CRYPTO_KLAD_KEY_SIZE_128BIT:
            param.hard_key_size = CRYPTO_KDF_HARD_KEY_SIZE_128BIT;
            break;
        case CRYPTO_KLAD_KEY_SIZE_192BIT:
            param.hard_key_size = CRYPTO_KDF_HARD_KEY_SIZE_192BIT;
            break;
        case CRYPTO_KLAD_KEY_SIZE_256BIT:
            param.hard_key_size = CRYPTO_KDF_HARD_KEY_SIZE_256BIT;
            break;
        default:
            crypto_log_err("invalid key_size\n");
            return TD_FAILURE;
    }

    param.hard_key_type = g_klad_ctx.hard_key_type;
    param.hard_alg = content_key->kdf_hard_alg;
    param.salt = content_key->salt;
    param.salt_length = content_key->salt_length;
    param.is_oneway = content_key->oneway;

    /* klad lock. */
    ret = hal_klad_lock();
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_klad_lock failed\n");

    /* rkp lock. */
    ret = hal_rkp_lock();
    crypto_chk_goto(ret != TD_SUCCESS, exit_klad_unlock, "hal_rkp_lock failed\n");

    /* rkp hard calculation. */
    ret = hal_rkp_kdf_hard_calculation(&param);
    crypto_chk_goto(ret != TD_SUCCESS, exit_rkp_unlock, "hal_rkp_kdf_hard_calculation failed\n");

    ret = hal_klad_start_com_route(param.hard_key_type, content_key, g_klad_ctx.klad_dest);
    crypto_chk_goto(ret != TD_SUCCESS, exit_rkp_unlock, "hal_klad_start_com_route failed\n");

exit_rkp_unlock:
    (td_void)hal_rkp_unlock();
exit_klad_unlock:
    (td_void)hal_klad_unlock();
    return ret;
}

td_s32 drv_kdf_update(crypto_kdf_otp_key otp_key, crypto_kdf_update_alg alg)
{
    volatile td_s32 ret = TD_FAILURE;

    /* rkp lock. */
    ret = hal_rkp_lock();
    crypto_chk_return(ret != TD_SUCCESS, ret, "hal_rkp_lock failed\n");

    ret = hal_rkp_deob_update(otp_key, alg);
    crypto_chk_goto(ret != TD_SUCCESS, exit_rkp_unlock, "hal_rkp_deob_update failed\n");

exit_rkp_unlock:
    (td_void)hal_rkp_unlock();
    return ret;
}