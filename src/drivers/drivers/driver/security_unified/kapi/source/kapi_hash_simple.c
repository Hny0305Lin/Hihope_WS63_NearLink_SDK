/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hash kernel API function implementation.
 *
 * Create: 2023-12-23
*/

#include <securec.h>
#include "kapi_hash.h"
#include "kapi_inner.h"

#include "drv_hash.h"
#include "crypto_common_macro.h"
#include "crypto_common_def.h"
#include "crypto_errno.h"
#include "crypto_drv_common.h"

#define INVALID_HANDLE      0xFFFFFFFF

#define HASH_COMPAT_ERRNO(err_code)     KAPI_COMPAT_ERRNO(ERROR_MODULE_HASH, err_code)
#define hash_null_ptr_chk(ptr)   \
    crypto_chk_return((ptr) == TD_NULL, HASH_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

typedef struct {
    crypto_hash_clone_ctx clone_ctx;
    crypto_owner owner;
    td_bool is_keyslot;
    td_handle drv_hash_handle;
} kapi_hash_context;

static crypto_mutex g_hash_mutex;
static kapi_hash_context *g_hash_ctx[CRYPTO_HASH_VIRT_CHN_NUM];
static td_bool g_hash_ctx_used[CRYPTO_HASH_VIRT_CHN_NUM];

#define kapi_hash_lock() do {                   \
    crypto_mutex_lock(&g_hash_mutex);           \
} while (0)
#define kapi_hash_unlock() do {                 \
    crypto_mutex_unlock(&g_hash_mutex);         \
} while (0)

static td_s32 inner_kapi_hash_handle_check(td_handle kapi_hash_handle)
{
    crypto_chk_return(kapi_get_module_id(kapi_hash_handle) != KAPI_HASH_MODULE_ID,
        HASH_COMPAT_ERRNO(ERROR_INVALID_HANDLE), "Invalid Hash Handle! 0x%x\n", kapi_hash_handle);
    crypto_chk_return(kapi_get_ctx_idx(kapi_hash_handle) >= CRYPTO_HASH_VIRT_CHN_NUM,
        HASH_COMPAT_ERRNO(ERROR_INVALID_HANDLE), "Invalid Hash Handle! 0x%x\n", kapi_hash_handle);
    return TD_SUCCESS;
}

static int inner_kapi_alloc_hash_ctx(td_handle *hash_handle, kapi_hash_context **hash_ctx)
{
    int ret = TD_FAILURE;
    unsigned int i;
    kapi_hash_lock();
    for (i = 0; i < CRYPTO_HASH_VIRT_CHN_NUM; i++) {
        if (g_hash_ctx_used[i] == TD_FALSE) {
            break;
        }
    }
    if (i >= CRYPTO_HASH_VIRT_CHN_NUM) {
        crypto_log_err("All hash contexts are busy!\n");
        ret = HASH_COMPAT_ERRNO(ERROR_CHN_BUSY);
        goto exit_unlock;
    }

    *hash_ctx = crypto_malloc(sizeof(kapi_hash_context));
    crypto_chk_goto_with_ret(ret, *hash_ctx == NULL, exit_unlock,
        HASH_COMPAT_ERRNO(ERROR_MALLOC), "crypto_malloc failed\n");
    (void)memset_s(*hash_ctx, sizeof(kapi_hash_context), 0, sizeof(kapi_hash_context));

    g_hash_ctx_used[i] = TD_TRUE;
    g_hash_ctx[i] = *hash_ctx;
    crypto_get_owner(&(g_hash_ctx[i]->owner));
    *hash_handle = synthesize_kapi_handle(KAPI_HASH_MODULE_ID, i);
    ret = CRYPTO_SUCCESS;
exit_unlock:
    kapi_hash_unlock();
    return ret;
}

static int inner_kapi_free_hash_ctx(td_handle hash_handle)
{
    unsigned int idx = kapi_get_ctx_idx(hash_handle);
    if (inner_kapi_hash_handle_check(hash_handle) != TD_SUCCESS) {
        return HASH_COMPAT_ERRNO(ERROR_INVALID_HANDLE);
    }
    kapi_hash_lock();
    g_hash_ctx_used[idx] = TD_FALSE;
    if (g_hash_ctx[idx] != NULL) {
        crypto_free(g_hash_ctx[idx]);
        g_hash_ctx[idx] = NULL;
    }
    kapi_hash_unlock();
    return CRYPTO_SUCCESS;
}

static kapi_hash_context *inner_kapi_get_hash_ctx(td_handle hash_handle)
{
    unsigned int idx = kapi_get_ctx_idx(hash_handle);
    if (inner_kapi_hash_handle_check(hash_handle) != TD_SUCCESS) {
        return NULL;
    }
    return g_hash_ctx[idx];
}

static td_s32 inner_drv_lock_start(kapi_hash_context *hash_ctx, const crypto_hash_attr *hash_attr)
{
    td_s32 ret = TD_SUCCESS;
    kapi_hash_lock();
    ret = drv_cipher_hash_start(&hash_ctx->drv_hash_handle, hash_attr);
    kapi_hash_unlock();
    return ret;
}

td_s32 kapi_cipher_hash_env_init(td_void)
{
    td_s32 ret = TD_SUCCESS;
    crypto_kapi_func_enter();

    ret = drv_cipher_hash_init();
    if (ret != TD_SUCCESS) {
        crypto_log_err("drv_cipher_hash_init failed\n");
        return ret;
    }

    ret = crypto_mutex_init(&g_hash_mutex);
    if (ret != TD_SUCCESS) {
        crypto_log_err("crypto_mutex_init failed\n");
        ret = HASH_COMPAT_ERRNO(ERROR_MUTEX_INIT);
        goto error_hash_deinit;
    }

    crypto_kapi_func_exit();
    return ret;

error_hash_deinit:
    drv_cipher_hash_deinit();
    return ret;
}

td_s32 kapi_cipher_hash_env_deinit(td_void)
{
    unsigned int i;
    td_s32 ret = TD_SUCCESS;
    crypto_kapi_func_enter();
    for (i = 0; i < CRYPTO_HASH_VIRT_CHN_NUM; i++) {
        if (g_hash_ctx_used[i] == TD_FALSE) {
            continue;
        }
        g_hash_ctx_used[i] = TD_FALSE;
        if (g_hash_ctx[i] != NULL) {
            crypto_free(g_hash_ctx[i]);
            g_hash_ctx[i] = NULL;
        }
    }
    crypto_mutex_destroy(&g_hash_mutex);
    drv_cipher_hash_deinit();
    crypto_kapi_func_exit();
    return ret;
}

td_s32 kapi_cipher_hash_init(td_void)
{
    td_s32 ret = TD_SUCCESS;
    crypto_kapi_func_enter();

    crypto_kapi_func_exit();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hash_init);

td_s32 kapi_cipher_hash_deinit(td_void)
{
    crypto_kapi_func_enter();
    crypto_kapi_func_exit();
    return TD_SUCCESS;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hash_deinit);

td_s32 kapi_cipher_hash_start(td_handle *kapi_hash_handle, const crypto_hash_attr *hash_attr)
{
    int ret = TD_FAILURE;
    const unsigned int *sha_val = NULL;
    unsigned int sha_val_len = 0;
    kapi_hash_context *hash_ctx = NULL;
    crypto_kapi_func_enter();

    hash_null_ptr_chk(kapi_hash_handle);
    hash_null_ptr_chk(hash_attr);

    ret = inner_kapi_alloc_hash_ctx(kapi_hash_handle, &hash_ctx);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_kapi_alloc_hash_ctx failed\n");

    if (crypto_hash_is_hmac(hash_attr->hash_type) == TD_TRUE) {
        if (hash_attr->is_keyslot == TD_FALSE) {
            crypto_log_err("hmac only support keyslot\n");
            ret = HASH_COMPAT_ERRNO(ERROR_UNSUPPORT);
            goto error_free_ctx;
        }
        ret = inner_drv_lock_start(hash_ctx, hash_attr);
        crypto_chk_goto(ret != TD_SUCCESS, error_free_ctx, "inner_drv_lock_start failed\n");
        hash_ctx->is_keyslot = hash_attr->is_keyslot;
        return CRYPTO_SUCCESS;
    }

    (void)memset_s(&hash_ctx->clone_ctx, sizeof(hash_ctx->clone_ctx), 0, sizeof(hash_ctx->clone_ctx));

    sha_val = drv_hash_get_state_iv(hash_attr->hash_type, &sha_val_len);
    crypto_chk_goto_with_ret(ret, sha_val == TD_NULL, error_free_ctx, TD_FAILURE, "drv_hash_get_state_iv failed\n");

    ret = memcpy_s(hash_ctx->clone_ctx.state, sizeof(hash_ctx->clone_ctx.state), sha_val, sha_val_len);
    crypto_chk_goto_with_ret(ret, ret != EOK, error_free_ctx, HASH_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed\n");

    hash_ctx->clone_ctx.hash_type = hash_attr->hash_type;
    crypto_kapi_func_exit();

    return 0;
error_free_ctx:
    inner_kapi_free_hash_ctx(*kapi_hash_handle);
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hash_start);

td_s32 kapi_cipher_hash_update(td_handle kapi_hash_handle,  const crypto_buf_attr *src_buf, const td_u32 len)
{
    int ret;
    td_handle hash_handle = INVALID_HANDLE;
    crypto_hash_attr hash_attr;
    kapi_hash_context *hash_ctx = NULL;
    crypto_kapi_func_enter();

    kapi_hash_lock();
    hash_ctx = inner_kapi_get_hash_ctx(kapi_hash_handle);
    crypto_chk_goto_with_ret(ret, hash_ctx == NULL, exit_mutex_unlock, HASH_COMPAT_ERRNO(ERROR_INVALID_HANDLE),
        "inner_kapi_get_hash_ctx failed\n");

    if (hash_ctx->is_keyslot == TD_TRUE) {
        ret = drv_cipher_hash_update(hash_ctx->drv_hash_handle, src_buf, len);
        crypto_chk_goto(ret != TD_SUCCESS, exit_mutex_unlock, "drv_cipher_hash_update failed\n");
        kapi_hash_unlock();
        return CRYPTO_SUCCESS;
    }

    hash_attr.hash_type = hash_ctx->clone_ctx.hash_type;
    hash_attr.is_keyslot = hash_ctx->is_keyslot;
    ret = drv_cipher_hash_start(&hash_handle, &hash_attr);
    crypto_chk_goto(ret != 0, exit_mutex_unlock, "drv_cipher_hash_set failed\n");

    ret = drv_cipher_hash_set(hash_handle, &hash_ctx->clone_ctx);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_set failed\n");

    ret = drv_cipher_hash_update(hash_handle, src_buf, len);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_update failed\n");

    ret = drv_cipher_hash_get(hash_handle, &hash_ctx->clone_ctx);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_get failed\n");

exit_hash_destroy:
    (void)drv_cipher_hash_destroy(hash_handle);
exit_mutex_unlock:
    kapi_hash_unlock();
    crypto_kapi_func_exit();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hash_update);

td_s32 kapi_cipher_hash_finish(td_handle kapi_hash_handle, td_u8 *out, td_u32 *out_len)
{
    int ret;
    td_handle hash_handle = INVALID_HANDLE;
    crypto_hash_attr hash_attr;
    kapi_hash_context *hash_ctx = NULL;
    crypto_kapi_func_enter();

    hash_ctx = inner_kapi_get_hash_ctx(kapi_hash_handle);
    crypto_chk_return(hash_ctx == NULL, HASH_COMPAT_ERRNO(ERROR_INVALID_HANDLE), "inner_kapi_get_hash_ctx failed\n");

    kapi_hash_lock();

    if (hash_ctx->is_keyslot == TD_TRUE) {
        ret = drv_cipher_hash_finish(hash_ctx->drv_hash_handle, out, out_len);
        if (ret != TD_SUCCESS) {
            crypto_log_err("drv_cipher_hash_finish failed\n");
            goto exit_hash_destroy;
        }
        ret = CRYPTO_SUCCESS;
        goto exit_mutex_unlock;
    }

    hash_attr.hash_type = hash_ctx->clone_ctx.hash_type;
    hash_attr.is_keyslot = hash_ctx->is_keyslot;
    ret = drv_cipher_hash_start(&hash_handle, &hash_attr);
    crypto_chk_goto(ret != 0, exit_mutex_unlock, "drv_cipher_hash_start failed\n");

    ret = drv_cipher_hash_set(hash_handle, &hash_ctx->clone_ctx);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_set failed\n");

    ret = drv_cipher_hash_finish(hash_handle, out, out_len);
    crypto_chk_goto(ret != 0, exit_hash_destroy, "drv_cipher_hash_finish failed\n");
    hash_handle = INVALID_HANDLE;

exit_hash_destroy:
    if (hash_handle != INVALID_HANDLE) {
        (void)drv_cipher_hash_destroy(hash_handle);
    }
exit_mutex_unlock:
    kapi_hash_unlock();
    inner_kapi_free_hash_ctx(kapi_hash_handle);
    crypto_kapi_func_exit();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hash_finish);

td_s32 kapi_cipher_hash_get(td_handle kapi_hash_handle, crypto_hash_clone_ctx *hash_clone_ctx)
{
    int ret;
    kapi_hash_context *hash_ctx = NULL;
    crypto_kapi_func_enter();

    hash_ctx = inner_kapi_get_hash_ctx(kapi_hash_handle);
    crypto_chk_return(hash_ctx == NULL, HASH_COMPAT_ERRNO(ERROR_INVALID_HANDLE), "inner_kapi_get_hash_ctx failed\n");

    if (hash_ctx->is_keyslot == TD_TRUE) {
        crypto_log_err("hmac unsupport hash get\n");
        return HASH_COMPAT_ERRNO(ERROR_UNSUPPORT);
    }

    ret = memcpy_s(hash_clone_ctx, sizeof(crypto_hash_clone_ctx), &hash_ctx->clone_ctx, sizeof(crypto_hash_clone_ctx));
    crypto_chk_return(ret != EOK, HASH_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed\n");

    crypto_kapi_func_exit();
    return 0;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hash_get);

td_s32 kapi_cipher_hash_set(td_handle kapi_hash_handle, const crypto_hash_clone_ctx *hash_clone_ctx)
{
    int ret;
    kapi_hash_context *hash_ctx = NULL;
    crypto_kapi_func_enter();

    hash_ctx = inner_kapi_get_hash_ctx(kapi_hash_handle);
    crypto_chk_return(hash_ctx == NULL, HASH_COMPAT_ERRNO(ERROR_INVALID_HANDLE), "inner_kapi_get_hash_ctx failed\n");

    if (hash_ctx->is_keyslot == TD_TRUE) {
        crypto_log_err("hmac unsupport hash set\n");
        return HASH_COMPAT_ERRNO(ERROR_UNSUPPORT);
    }

    ret = memcpy_s(&hash_ctx->clone_ctx, sizeof(crypto_hash_clone_ctx), hash_clone_ctx, sizeof(crypto_hash_clone_ctx));
    crypto_chk_return(ret != EOK, HASH_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed\n");

    crypto_kapi_func_exit();
    return 0;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hash_set);

td_s32 kapi_cipher_hash_destroy(td_handle kapi_hash_handle)
{
    int ret;
    crypto_kapi_func_enter();
    ret = inner_kapi_free_hash_ctx(kapi_hash_handle);
    crypto_kapi_func_exit();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_hash_destroy);

td_s32 kapi_cipher_pbkdf2(const crypto_kdf_pbkdf2_param *param, td_u8 *out, const td_u32 out_len)
{
    td_s32 ret = TD_SUCCESS;
    crypto_kapi_func_enter();

    kapi_hash_lock();
    ret = drv_cipher_pbkdf2(param, out, out_len);
    kapi_hash_unlock();
    crypto_kapi_func_exit();
    return ret;
}
CRYPTO_EXPORT_SYMBOL(kapi_cipher_pbkdf2);