/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: key manager kernel API function implementation.
 *
 * Create: 2023-05-26
*/

#include "kapi_km.h"
#include "kapi_inner.h"

#include "crypto_common_def.h"
#include "crypto_common_macro.h"
#include "crypto_drv_common.h"
#include "drv_klad.h"
#include "drv_keyslot.h"

#define handle_2_id(handle)         (((handle)) & 0xffff)

typedef struct {
    crypto_owner owner;
    td_bool is_open;
    td_handle keyslot_handle;
    crypto_keyslot_type type;
} crypto_kapi_keyslot_ctx;

typedef struct {
    crypto_owner owner;
    td_handle drv_klad_handle;
    td_handle drv_keyslot_handle;
    crypto_klad_dest klad_type;
    crypto_klad_attr klad_attr;
    crypto_klad_session_key session_key;
    td_handle keyslot_handle;
    unsigned int is_open            : 1;
    unsigned int is_attached        : 1;
    unsigned int is_set_attr        : 1;
    unsigned int is_set_session_key : 1;
} crypto_kapi_klad_ctx;

crypto_mutex g_klad_mutex;
crypto_mutex g_keyslot_mutex;

#define CRYPTO_MCIPHER_KEYSLOT_NUM      8
#define CRYPTO_HMAC_KEYSLOT_NUM         2
#define CRYPTO_KLAD_VIRT_NUM            4

static crypto_kapi_keyslot_ctx g_keyslot_symc_ctx_list[CRYPTO_MCIPHER_KEYSLOT_NUM] = {0};
static crypto_kapi_keyslot_ctx g_keyslot_hmac_ctx_list[CRYPTO_HMAC_KEYSLOT_NUM] = {0};

static crypto_kapi_klad_ctx g_klad_ctx_list[CRYPTO_KLAD_VIRT_NUM] = {0};

#define kapi_klad_mutex_lock() crypto_mutex_lock(&g_klad_mutex)

#define kapi_klad_mutex_unlock() crypto_mutex_unlock(&g_klad_mutex)

#define kapi_keyslot_mutex_lock() crypto_mutex_lock(&g_keyslot_mutex)

#define kapi_keyslot_mutex_unlock() crypto_mutex_unlock(&g_keyslot_mutex)

#define km_compat_errno(err_code)     KAPI_COMPAT_ERRNO(ERROR_MODULE_KM, err_code)
#define km_null_ptr_chk(ptr)   \
    crypto_chk_return((ptr) == TD_NULL, km_compat_errno(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

td_s32 kapi_km_env_init(td_void)
{
    td_s32 ret;

    ret = crypto_mutex_init(&g_klad_mutex);
    crypto_chk_return(ret != TD_SUCCESS, ret, "crypto_mutex_init failed, ret is 0x%x\n", ret);

    ret = crypto_mutex_init(&g_keyslot_mutex);
    crypto_chk_goto(ret != TD_SUCCESS, klad_mutex_destroy_exit, "crypto_mutex_init failed, ret is 0x%x\n", ret);

    ret = drv_keyslot_init();
    crypto_chk_goto(ret != TD_SUCCESS, keyslot_mutex_destroy_exit, "drv_keyslot_init failed, ret is 0x%x\n", ret);

    return TD_SUCCESS;

keyslot_mutex_destroy_exit:
    crypto_mutex_destroy(&g_keyslot_mutex);
klad_mutex_destroy_exit:
    crypto_mutex_destroy(&g_klad_mutex);
    return TD_FAILURE;
}

td_s32 kapi_km_env_deinit(td_void)
{
    crypto_mutex_destroy(&g_keyslot_mutex);
    crypto_mutex_destroy(&g_klad_mutex);

    (td_void)memset_s(g_keyslot_symc_ctx_list, sizeof(g_keyslot_symc_ctx_list), 0, sizeof(g_keyslot_symc_ctx_list));
    (td_void)memset_s(g_keyslot_hmac_ctx_list, sizeof(g_keyslot_hmac_ctx_list), 0, sizeof(g_keyslot_hmac_ctx_list));

    (td_void)drv_keyslot_deinit();

    (td_void)memset_s(g_klad_ctx_list, sizeof(g_klad_ctx_list), 0, sizeof(g_klad_ctx_list));

    return TD_SUCCESS;
}

td_s32 kapi_km_deinit(td_void)
{
    td_s32 i;
    crypto_owner owner;
    crypto_kapi_keyslot_ctx *keyslot_ctx = TD_NULL;
    crypto_kapi_klad_ctx *klad_ctx = TD_NULL;

    crypto_get_owner(&owner);
    /* mcipher keyslot. */
    for (i = 0; i < CRYPTO_MCIPHER_KEYSLOT_NUM; i++) {
        keyslot_ctx = &g_keyslot_symc_ctx_list[i];
        if (memcmp(&owner, &keyslot_ctx->owner, sizeof(crypto_owner)) == 0) {
            (td_void)drv_keyslot_destroy(keyslot_ctx->keyslot_handle);
            (td_void)memset_s(keyslot_ctx, sizeof(crypto_kapi_keyslot_ctx), 0, sizeof(crypto_kapi_keyslot_ctx));
        }
    }
    /* hmac keyslot. */
    for (i = 0; i < CRYPTO_HMAC_KEYSLOT_NUM; i++) {
        keyslot_ctx = &g_keyslot_hmac_ctx_list[i];
        if (memcmp(&owner, &keyslot_ctx->owner, sizeof(crypto_owner)) == 0) {
            (td_void)drv_keyslot_destroy(keyslot_ctx->keyslot_handle);
            (td_void)memset_s(keyslot_ctx, sizeof(crypto_kapi_keyslot_ctx), 0, sizeof(crypto_kapi_keyslot_ctx));
        }
    }

    /* klad. */
    for (i = 0; i < CRYPTO_HMAC_KEYSLOT_NUM; i++) {
        klad_ctx = &g_klad_ctx_list[i];
        if (memcmp(&owner, &klad_ctx->owner, sizeof(crypto_owner)) == 0) {
            (td_void)memset_s(klad_ctx, sizeof(crypto_kapi_klad_ctx), 0, sizeof(crypto_kapi_klad_ctx));
        }
    }

    return TD_SUCCESS;
}

static td_s32 inner_keyslot_handle_chk(td_handle keyslot_handle)
{
    td_u32 idx = 0;
    if (crypto_keyslot_get_module_id(keyslot_handle) != KAPI_KEYSLOT_MODULE_ID) {
        crypto_log_err("invliad module id\n");
        return km_compat_errno(ERROR_INVALID_HANDLE);
    }
    idx = crypto_keyslot_get_idx(keyslot_handle);
    if (crypto_keyslot_is_hmac(keyslot_handle) == TD_TRUE) {
        crypto_chk_return(idx >= CRYPTO_HMAC_KEYSLOT_NUM, km_compat_errno(ERROR_INVALID_HANDLE),
            "idx overflow for hmac keyslot\n");
    } else if (crypto_keyslot_is_mcipher(keyslot_handle) == TD_TRUE) {
        crypto_chk_return(idx >= CRYPTO_MCIPHER_KEYSLOT_NUM, km_compat_errno(ERROR_INVALID_HANDLE),
            "idx overflow for mcipher keyslot\n");
    } else {
        crypto_log_err("invalid keyslot_type, keyslot_handle is 0x%x\n", keyslot_handle);
        return km_compat_errno(ERROR_INVALID_HANDLE);
    }
    return TD_SUCCESS;
}

static crypto_kapi_keyslot_ctx *inner_keyslot_get_ctx(td_handle keyslot_handle)
{
    td_s32 ret;
    td_u32 idx = 0;
    crypto_kapi_keyslot_ctx *ctx = TD_NULL;

    ret = inner_keyslot_handle_chk(keyslot_handle);
    crypto_chk_return(ret != TD_SUCCESS, TD_NULL, "inner_keyslot_handle_chk failed\n");

    idx = crypto_keyslot_get_idx(keyslot_handle);
    if (crypto_keyslot_is_mcipher(keyslot_handle)) {
        ctx = &g_keyslot_symc_ctx_list[idx];
    } else {
        ctx = &g_keyslot_hmac_ctx_list[idx];
    }
    return ctx;
}

static td_s32 inner_klad_handle_chk(td_handle klad_handle)
{
    td_u32 idx = 0;
    if (kapi_get_module_id(klad_handle) != KAPI_KLAD_MODULE_ID) {
        crypto_log_err("invalid module id\n");
        return km_compat_errno(ERROR_INVALID_HANDLE);
    }
    idx = kapi_get_ctx_idx(klad_handle);
    crypto_chk_return(idx >= CRYPTO_KLAD_VIRT_NUM, km_compat_errno(ERROR_INVALID_HANDLE),
        "idx overflow for klad\n");
    
    return TD_SUCCESS;
}

#define HASH_BLOCK_SIZE_64BYTE 64
#define HASH_BLOCK_SIZE_128BYTE 128
static td_bool inner_hmac_clear_key_size_check(crypto_klad_hmac_type hmac_type, td_u32 key_length)
{
    td_u32 block_size = 0;

    switch (hmac_type) {
        case CRYPTO_KLAD_HMAC_TYPE_SHA1:
        case CRYPTO_KLAD_HMAC_TYPE_SHA224:
        case CRYPTO_KLAD_HMAC_TYPE_SHA256:
        case CRYPTO_KLAD_HMAC_TYPE_SM3:
            block_size = HASH_BLOCK_SIZE_64BYTE;
            break;
        case CRYPTO_KLAD_HMAC_TYPE_SHA384:
        case CRYPTO_KLAD_HMAC_TYPE_SHA512:
            block_size = HASH_BLOCK_SIZE_128BYTE;
            break;
        default:
            crypto_log_err("Invalid Hmac Mode!\n");
            return TD_FALSE;
    }

    if (key_length > block_size) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

/* Keyslot. Long-term occupation by default */
td_s32 kapi_keyslot_create(td_handle *kapi_keyslot_handle, crypto_keyslot_type keyslot_type)
{
    td_u32 idx;
    td_s32 ret = TD_SUCCESS;
    td_handle keyslot_handle = 0;
    crypto_kapi_keyslot_ctx *ctx_list = TD_NULL;
    crypto_kapi_keyslot_ctx *current_ctx = TD_NULL;
    td_u32 is_hmac = 0;

    km_null_ptr_chk(kapi_keyslot_handle);
    if (keyslot_type == (crypto_keyslot_type)CRYPTO_KEYSLOT_TYPE_MCIPHER) {
        ctx_list = g_keyslot_symc_ctx_list;
    } else if (keyslot_type == (crypto_keyslot_type)CRYPTO_KEYSLOT_TYPE_HMAC) {
        ctx_list = g_keyslot_hmac_ctx_list;
        is_hmac = 1;
    } else {
        crypto_log_err("invalid keyslot_type\n");
        return km_compat_errno(ERROR_INVALID_PARAM);
    }

    kapi_keyslot_mutex_lock();

    ret = drv_keyslot_create(&keyslot_handle, keyslot_type);
    crypto_chk_goto(ret != TD_SUCCESS, exit_unlock, "drv_keyslot_create failed, ret is 0x%x\n", ret);
    idx = handle_2_id(keyslot_handle);

    current_ctx = &ctx_list[idx];
    current_ctx->keyslot_handle = keyslot_handle;
    current_ctx->type = keyslot_type;
    current_ctx->is_open = TD_TRUE;
    crypto_get_owner(&current_ctx->owner);

    *kapi_keyslot_handle = crypto_keyslot_compat_handle(is_hmac, idx);

exit_unlock:
    kapi_keyslot_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_keyslot_create);

td_s32 kapi_keyslot_destroy(td_handle kapi_keyslot_handle)
{
    td_s32 ret = TD_SUCCESS;
    crypto_owner owner;
    crypto_kapi_keyslot_ctx *current_ctx = TD_NULL;

    current_ctx = inner_keyslot_get_ctx(kapi_keyslot_handle);
    crypto_chk_return(current_ctx == TD_NULL, km_compat_errno(ERROR_INVALID_HANDLE), "inner_keyslot_get_ctx failed\n");

    kapi_keyslot_mutex_lock();
    if (current_ctx->is_open == TD_FALSE) {
        ret = TD_SUCCESS;
        goto exit_unlock;
    }

    crypto_get_owner(&owner);
    if (memcmp(&owner, &current_ctx->owner, sizeof(crypto_owner)) != 0) {
        crypto_log_err("invalid process\n");
        ret = km_compat_errno(ERROR_INVALID_PROCESS);
        goto exit_unlock;
    }

    ret = drv_keyslot_destroy(current_ctx->keyslot_handle);
    crypto_chk_goto(ret != TD_SUCCESS, exit_unlock, "drv_keyslot_destroy failed, ret is 0x%x\n", ret);

    (td_void)memset_s(current_ctx, sizeof(crypto_kapi_keyslot_ctx), 0, sizeof(crypto_kapi_keyslot_ctx));

exit_unlock:
    kapi_keyslot_mutex_unlock();

    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_keyslot_destroy);

/* Klad. Short-term occupation by default */
td_s32 kapi_klad_create(td_handle *kapi_klad_handle)
{
    td_u32 i;
    td_s32 ret = TD_SUCCESS;
    crypto_kapi_klad_ctx *ctx = TD_NULL;

    km_null_ptr_chk(kapi_klad_handle);

    kapi_klad_mutex_lock();
    for (i = 0; i < CRYPTO_KLAD_VIRT_NUM; i++) {
        if (g_klad_ctx_list[i].is_open == TD_FALSE) {
            ctx = &g_klad_ctx_list[i];
            break;
        }
    }
    if (ctx == TD_NULL) {
        crypto_log_err("all klad contexts are busy\n");
        ret = km_compat_errno(ERROR_CHN_BUSY);
        goto exit_unlock;
    }

    crypto_get_owner(&ctx->owner);
    ctx->is_open = TD_TRUE;
    *kapi_klad_handle = synthesize_kapi_handle(KAPI_KLAD_MODULE_ID, i);

exit_unlock:
    kapi_klad_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_klad_create);

td_s32 kapi_klad_destroy(td_handle kapi_klad_handle)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 idx = 0;
    crypto_kapi_klad_ctx *ctx = TD_NULL;
    crypto_owner owner;

    ret = inner_klad_handle_chk(kapi_klad_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_klad_handle_chk failed\n");

    idx = kapi_get_ctx_idx(kapi_klad_handle);
    ctx = &g_klad_ctx_list[idx];
    if (ctx->is_open == TD_FALSE) {
        return TD_SUCCESS;
    }
    kapi_klad_mutex_lock();

    crypto_get_owner(&owner);
    if (memcmp(&owner, &ctx->owner, sizeof(crypto_owner)) != 0) {
        crypto_log_err("invalid process\n");
        ret = km_compat_errno(ERROR_INVALID_PROCESS);
        goto exit_unlock;
    }

    (td_void)memset_s(ctx, sizeof(crypto_kapi_klad_ctx), 0, sizeof(crypto_kapi_klad_ctx));

exit_unlock:
    kapi_klad_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_klad_destroy);

td_s32 kapi_klad_attach(td_handle kapi_klad_handle, crypto_klad_dest klad_type,
    td_handle kapi_keyslot_handle)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 idx = 0;
    crypto_kapi_klad_ctx *klad_ctx = TD_NULL;
    crypto_kapi_keyslot_ctx *keyslot_ctx = TD_NULL;
    crypto_owner owner;
    crypto_get_owner(&owner);

    crypto_chk_return (klad_type >= CRYPTO_KLAD_DEST_MAX, km_compat_errno(ERROR_INVALID_PARAM),
        "Invalid klad_dest_type\n");

    ret = inner_klad_handle_chk(kapi_klad_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_klad_handle_chk failed\n");

    idx = kapi_get_ctx_idx(kapi_klad_handle);
    klad_ctx = &g_klad_ctx_list[idx];

    kapi_klad_mutex_lock();
    crypto_chk_goto_with_ret(ret, klad_ctx->is_open == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_CTX_CLOSED), "call klad_create first\n");
    crypto_chk_goto_with_ret(ret, memcmp(&owner, &klad_ctx->owner, sizeof(crypto_owner)) != 0, exit_unlock,
        km_compat_errno(ERROR_INVALID_PROCESS), "invalid process\n");

    if (klad_type != CRYPTO_KLAD_DEST_NPU) {
        keyslot_ctx = inner_keyslot_get_ctx(kapi_keyslot_handle);
        crypto_chk_goto_with_ret(ret, keyslot_ctx == TD_NULL, exit_unlock, km_compat_errno(ERROR_INVALID_HANDLE),
            "inner_keyslot_get_ctx failed\n");
        crypto_chk_goto_with_ret(ret, keyslot_ctx->is_open == TD_FALSE, exit_unlock, km_compat_errno(ERROR_CTX_CLOSED),
            "call keyslot_create first\n");
        if (memcmp(&owner, &keyslot_ctx->owner, sizeof(crypto_owner)) != 0) {
            crypto_log_err("invalid process\n");
            ret = km_compat_errno(ERROR_INVALID_PROCESS);
            goto exit_unlock;
        }
        klad_ctx->keyslot_handle = keyslot_ctx->keyslot_handle;
    } else {
        klad_ctx->keyslot_handle = kapi_keyslot_handle;
    }

    klad_ctx->is_attached = TD_TRUE;
    klad_ctx->klad_type = klad_type;

exit_unlock:
    kapi_klad_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_klad_attach);

td_s32 kapi_klad_detach(td_handle kapi_klad_handle, crypto_klad_dest klad_type,
    td_handle kapi_keyslot_handle)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 idx = 0;
    crypto_kapi_klad_ctx *klad_ctx = TD_NULL;
    crypto_kapi_keyslot_ctx *keyslot_ctx = TD_NULL;
    crypto_owner owner;

    crypto_chk_return (klad_type >= CRYPTO_KLAD_DEST_MAX, km_compat_errno(ERROR_INVALID_PARAM),
        "Invalid klad_dest_type\n");

    ret = inner_klad_handle_chk(kapi_klad_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_klad_handle_chk failed\n");

    crypto_get_owner(&owner);
    idx = kapi_get_ctx_idx(kapi_klad_handle);
    klad_ctx = &g_klad_ctx_list[idx];
    kapi_klad_mutex_lock();

    crypto_chk_goto_with_ret(ret, memcmp(&owner, &klad_ctx->owner, sizeof(crypto_owner)) != 0, exit_unlock,
        km_compat_errno(ERROR_INVALID_PROCESS), "invalid process\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_open == TD_FALSE, exit_unlock, km_compat_errno(ERROR_CTX_CLOSED),
        "call klad_create first\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_attached == TD_FALSE, exit_unlock, km_compat_errno(ERROR_NOT_ATTACHED),
        "call klad_attach first\n");

    crypto_chk_goto_with_ret(ret, klad_ctx->klad_type != (crypto_klad_dest)klad_type, exit_unlock,
        km_compat_errno(ERROR_INVALID_PARAM), "invalid klad_type\n");
    if (klad_type != CRYPTO_KLAD_DEST_NPU) {
        keyslot_ctx = inner_keyslot_get_ctx(kapi_keyslot_handle);
        crypto_chk_goto_with_ret(ret, keyslot_ctx == TD_NULL, exit_unlock, km_compat_errno(ERROR_INVALID_HANDLE),
            "inner_keyslot_get_ctx failed\n");
        crypto_chk_goto_with_ret(ret, keyslot_ctx->keyslot_handle != klad_ctx->keyslot_handle, exit_unlock,
            km_compat_errno(ERROR_INVALID_HANDLE), "invalid keyslot_handle\n");
    } else {
        crypto_chk_goto_with_ret(ret, klad_ctx->keyslot_handle != kapi_keyslot_handle, exit_unlock,
            km_compat_errno(ERROR_INVALID_HANDLE), "invalid npu keyslot_handle\n");
    }

    klad_ctx->is_attached = TD_FALSE;
    klad_ctx->keyslot_handle = 0;
    klad_ctx->klad_type = 0;

exit_unlock:
    kapi_klad_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_klad_detach);

td_s32 kapi_klad_set_attr(td_handle kapi_klad_handle, const crypto_klad_attr *attr)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 idx = 0;
    crypto_kapi_klad_ctx *klad_ctx = TD_NULL;
    crypto_owner owner;

    km_null_ptr_chk(attr);
    crypto_chk_return(attr->key_sec_cfg.key_sec >= CRYPTO_KLAD_SEC_MAX, km_compat_errno(ERROR_INVALID_PARAM),
        "crypto_klad_attr.key_sec_cfg.key_sec >= CRYPTO_KLAD_SEC_MAX\n");
    crypto_chk_return(attr->key_cfg.engine >= CRYPTO_KLAD_ENGINE_MAX, km_compat_errno(ERROR_INVALID_PARAM),
        "crypto_klad_attr.key_cfg.engine >= CRYPTO_KLAD_ENGINE_MAX\n");

    ret = inner_klad_handle_chk(kapi_klad_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_klad_handle_chk failed\n");

    crypto_get_owner(&owner);
    idx = kapi_get_ctx_idx(kapi_klad_handle);
    klad_ctx = &g_klad_ctx_list[idx];
    kapi_klad_mutex_lock();

    crypto_chk_goto_with_ret(ret, memcmp(&owner, &klad_ctx->owner, sizeof(crypto_owner)) != 0, exit_unlock,
        km_compat_errno(ERROR_INVALID_PROCESS), "invalid process\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_open == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_CTX_CLOSED), "call klad_create first\n");

    ret = memcpy_s(&klad_ctx->klad_attr, sizeof(crypto_klad_attr), attr, sizeof(crypto_klad_attr));
    crypto_chk_goto_with_ret(ret, ret != EOK, exit_unlock, km_compat_errno(ERROR_MEMCPY_S), "memcpy_s failed\n");

    klad_ctx->is_set_attr = TD_TRUE;

exit_unlock:
    kapi_klad_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_klad_set_attr);

td_s32 kapi_klad_get_attr(td_handle kapi_klad_handle, crypto_klad_attr *attr)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 idx = 0;
    crypto_kapi_klad_ctx *klad_ctx = TD_NULL;
    crypto_owner owner;

    km_null_ptr_chk(attr);
    ret = inner_klad_handle_chk(kapi_klad_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_klad_handle_chk failed\n");

    crypto_get_owner(&owner);
    idx = kapi_get_ctx_idx(kapi_klad_handle);
    klad_ctx = &g_klad_ctx_list[idx];
    kapi_klad_mutex_lock();

    crypto_chk_goto_with_ret(ret, memcmp(&owner, &klad_ctx->owner, sizeof(crypto_owner)) != 0, exit_unlock,
        km_compat_errno(ERROR_INVALID_PROCESS), "invalid process\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_open == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_CTX_CLOSED), "call klad_create first\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_set_attr == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_NOT_SET_CONFIG), "call klad_set_attr first\n");

    ret = memcpy_s(attr, sizeof(crypto_klad_attr), &klad_ctx->klad_attr, sizeof(crypto_klad_attr));
    crypto_chk_goto_with_ret(ret, ret != EOK, exit_unlock, km_compat_errno(ERROR_MEMCPY_S), "memcpy_s failed\n");

exit_unlock:
    kapi_klad_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_klad_get_attr);

td_s32 kapi_klad_set_clear_key(td_handle kapi_klad_handle, const crypto_klad_clear_key *key)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 idx = 0;
    td_handle hard_klad_handle;
    crypto_kapi_klad_ctx *klad_ctx = TD_NULL;
    crypto_owner owner;
    crypto_klad_clear_key clear_key;

    km_null_ptr_chk(key);
    crypto_chk_return(key->key_parity >= CRYPTO_KLAD_KEY_PARITY_MAX, km_compat_errno(ERROR_INVALID_PARAM),
        "klad_clear_key.key_parity >= CRYPTO_KLAD_KEY_PARITY_MAX\n");

    clear_key.key = key->key;
    clear_key.key_parity = key->key_parity;
    clear_key.key_length = key->key_length;
    clear_key.hmac_type = key->hmac_type;

    ret = inner_klad_handle_chk(kapi_klad_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_klad_handle_chk failed\n");

    crypto_get_owner(&owner);
    idx = kapi_get_ctx_idx(kapi_klad_handle);
    klad_ctx = &g_klad_ctx_list[idx];
    kapi_klad_mutex_lock();

    crypto_chk_goto_with_ret(ret, memcmp(&owner, &klad_ctx->owner, sizeof(crypto_owner)) != 0, exit_unlock,
        km_compat_errno(ERROR_INVALID_PROCESS), "invalid process\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_open == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_CTX_CLOSED), "call klad_create first\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_attached == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_NOT_ATTACHED), "call klad_attach first\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_set_attr == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_NOT_SET_CONFIG), "call klad_set_attr first\n");

    crypto_chk_goto_with_ret(ret, (klad_ctx->klad_type == CRYPTO_KLAD_DEST_HMAC) &&
        (inner_hmac_clear_key_size_check(clear_key.hmac_type, clear_key.key_length) == TD_FALSE), exit_unlock,
        km_compat_errno(ERROR_INVALID_PARAM), "invalid key_size, please do hash first\n");

    ret = drv_klad_create(&hard_klad_handle);
    crypto_chk_goto(ret != TD_SUCCESS, exit_unlock, "drv_klad_create failed, ret is 0x%x\n", ret);

    ret = drv_klad_set_attr(hard_klad_handle, &klad_ctx->klad_attr);
    crypto_chk_goto(ret != TD_SUCCESS, exit_destroy, "drv_klad_set_attr failed, ret is 0x%x\n", ret);

    ret = drv_klad_attach(hard_klad_handle, klad_ctx->klad_type, klad_ctx->keyslot_handle);
    crypto_chk_goto(ret != TD_SUCCESS, exit_destroy, "drv_klad_attach failed, ret is 0x%x\n", ret);

    ret = drv_klad_set_clear_key(hard_klad_handle, &clear_key);
    crypto_chk_goto(ret != TD_SUCCESS, exit_detach, "drv_klad_set_clear_key failed, ret is 0x%x\n", ret);

exit_detach:
    drv_klad_detach(hard_klad_handle, klad_ctx->klad_type, klad_ctx->keyslot_handle);
exit_destroy:
    drv_klad_destroy(hard_klad_handle);
exit_unlock:
    kapi_klad_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_klad_set_clear_key);

#define CRYPTO_EFFECTIVE_KEY_SALT_LENGTH_MAX 28
td_s32 kapi_klad_set_effective_key(td_handle kapi_klad_handle, const crypto_klad_effective_key *effective_key)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 idx = 0;
    crypto_kapi_klad_ctx *klad_ctx = TD_NULL;
    crypto_owner owner;
    td_handle hard_klad_handle = 0;

    km_null_ptr_chk(effective_key);
    km_null_ptr_chk(effective_key->salt);
    crypto_chk_return(effective_key->salt_length > CRYPTO_EFFECTIVE_KEY_SALT_LENGTH_MAX,
        km_compat_errno(ERROR_INVALID_PARAM), "effective_key.kdf_hard_alg >= CRYPTO_KDF_HARD_ALG_MAX\n");
    crypto_chk_return(effective_key->kdf_hard_alg >= CRYPTO_KDF_HARD_ALG_MAX, km_compat_errno(ERROR_INVALID_PARAM),
        "effective_key.kdf_hard_alg >= CRYPTO_KDF_HARD_ALG_MAX\n");
    crypto_chk_return(effective_key->key_size != CRYPTO_KLAD_KEY_SIZE_128BIT &&
        effective_key->key_size != CRYPTO_KLAD_KEY_SIZE_192BIT &&
        effective_key->key_size != CRYPTO_KLAD_KEY_SIZE_256BIT,
        km_compat_errno(ERROR_INVALID_PARAM), "Content key's size is invalid\n");

    ret = inner_klad_handle_chk(kapi_klad_handle);
    crypto_chk_return(ret != TD_SUCCESS, ret, "inner_klad_handle_chk failed\n");

    crypto_get_owner(&owner);
    idx = kapi_get_ctx_idx(kapi_klad_handle);
    klad_ctx = &g_klad_ctx_list[idx];
    kapi_klad_mutex_lock();

    crypto_chk_goto_with_ret(ret, memcmp(&owner, &klad_ctx->owner, sizeof(crypto_owner)) != 0, exit_unlock,
        km_compat_errno(ERROR_INVALID_PROCESS), "invalid process\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_open == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_CTX_CLOSED), "call klad_create first\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_attached == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_NOT_ATTACHED), "call klad_attach first\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->is_set_attr == TD_FALSE, exit_unlock,
        km_compat_errno(ERROR_NOT_SET_CONFIG), "call klad_set_attr first\n");
    crypto_chk_goto_with_ret(ret, klad_ctx->klad_attr.klad_cfg.rootkey_type == CRYPTO_KDF_HARD_KEY_TYPE_ODRK1 &&
        effective_key->key_size != CRYPTO_KLAD_KEY_SIZE_128BIT, exit_unlock,
        km_compat_errno(ERROR_INVALID_PARAM), "ODRK1 only support 128 key_size\n");

    ret = drv_klad_create(&hard_klad_handle);
    crypto_chk_goto(ret != TD_SUCCESS, exit_unlock, "drv_klad_create failed, ret is 0x%x\n", ret);

    ret = drv_klad_set_attr(hard_klad_handle, (crypto_klad_attr *)&klad_ctx->klad_attr);
    crypto_chk_goto(ret != TD_SUCCESS, destroy_exit, "drv_klad_set_attr failed, ret is 0x%x\n", ret);

    ret = drv_klad_attach(hard_klad_handle, klad_ctx->klad_type, klad_ctx->keyslot_handle);
    crypto_chk_goto(ret != TD_SUCCESS, destroy_exit, "drv_klad_attach failed, ret is 0x%x\n", ret);

    ret = drv_klad_set_effective_key(hard_klad_handle, effective_key);
    crypto_chk_goto(ret != TD_SUCCESS, detach_exit, "drv_klad_set_session_key failed, ret is 0x%x\n", ret);
detach_exit:
    drv_klad_detach(hard_klad_handle, klad_ctx->klad_type, klad_ctx->keyslot_handle);
destroy_exit:
    drv_klad_destroy(hard_klad_handle);
exit_unlock:
    kapi_klad_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_klad_set_effective_key);

td_s32 kapi_kdf_update(crypto_kdf_otp_key otp_key, crypto_kdf_update_alg alg)
{
    td_s32 ret = TD_SUCCESS;

    crypto_chk_return(otp_key >= CRYPTO_KDF_OTP_KEY_MAX,
        km_compat_errno(ERROR_INVALID_PARAM), "kdf_otp_key >= CRYPTO_KDF_OTP_KEY_MAX\n");
    crypto_chk_return(alg >= CRYPTO_KDF_UPDATE_ALG_MAX,
        km_compat_errno(ERROR_INVALID_PARAM), "update_alg >= CRYPTO_KDF_UPDATE_ALG_MAX\n");

    kapi_klad_mutex_lock();
    ret = drv_kdf_update(otp_key, alg);
    crypto_chk_goto(ret != TD_SUCCESS, unlock_exit, "drv_kdf_update failed, ret is 0x%x\n", ret);

unlock_exit:
    kapi_klad_mutex_unlock();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_kdf_update);