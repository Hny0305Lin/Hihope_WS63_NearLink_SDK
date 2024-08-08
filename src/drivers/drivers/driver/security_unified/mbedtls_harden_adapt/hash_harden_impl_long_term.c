/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: mbedtls hash harden implement long term.
 *
 * Create: 2023-12-23
*/

#include "hash_harden_impl.h"

#include "drv_hash.h"

#include "crypto_drv_common.h"

#define INVALID_HANDLE      0xFFFFFFFF
static td_handle g_mbedtls_hash_handle = INVALID_HANDLE;
static crypto_mutex g_mbedtls_hash_lock;

int mbedtls_alt_hash_start_impl(mbedtls_alt_hash_clone_ctx *clone_ctx, mbedtls_alt_hash_type hash_type)
{
    int ret = TD_FAILURE;
    const unsigned int *sha_val = NULL;
    unsigned int sha_val_len = 0;
    crypto_drv_func_enter();

    (void)memset_s(clone_ctx, sizeof(mbedtls_alt_hash_clone_ctx), 0, sizeof(mbedtls_alt_hash_clone_ctx));

    sha_val = drv_hash_get_state_iv((crypto_hash_type)hash_type, &sha_val_len);
    crypto_chk_return(sha_val == TD_NULL, TD_FAILURE, "drv_hash_get_state_iv failed\n");

    ret = memcpy_s(clone_ctx->state, sizeof(clone_ctx->state), sha_val, sha_val_len);
    crypto_chk_return(ret != EOK, ret, "memcpy_s failed\n");

    clone_ctx->hash_type = hash_type;

    crypto_drv_func_exit();

    return 0;
}

int mbedtls_alt_hash_update_impl(mbedtls_alt_hash_clone_ctx *clone_ctx,
    const unsigned char *data, unsigned int data_len)
{
    int ret;
    crypto_buf_attr src_buf;
    crypto_drv_func_enter();

    src_buf.virt_addr = (unsigned char *)data;

    ret = crypto_mutex_lock(&g_mbedtls_hash_lock);
    crypto_chk_return(ret != 0, ret, "crypto_mutex_lock failed\n");

    ret = drv_cipher_hash_set(g_mbedtls_hash_handle, (crypto_hash_clone_ctx *)clone_ctx);
    crypto_chk_goto(ret != 0, exit_mutex_unlock, "drv_cipher_hash_set failed\n");

    ret = drv_cipher_hash_update(g_mbedtls_hash_handle, &src_buf, data_len);
    crypto_chk_goto(ret != 0, exit_mutex_unlock, "drv_cipher_hash_update failed\n");

    ret = drv_cipher_hash_get(g_mbedtls_hash_handle, (crypto_hash_clone_ctx *)clone_ctx);
    crypto_chk_goto(ret != 0, exit_mutex_unlock, "drv_cipher_hash_get failed\n");

exit_mutex_unlock:
    crypto_mutex_unlock(&g_mbedtls_hash_lock);
    crypto_drv_func_exit();
    return ret;
}

int mbedtls_alt_hash_finish_impl(mbedtls_alt_hash_clone_ctx *clone_ctx, unsigned char *out, unsigned int out_len)
{
    int ret;
    unsigned int hash_out_len = out_len;
    crypto_drv_func_enter();

    ret = crypto_mutex_lock(&g_mbedtls_hash_lock);
    crypto_chk_return(ret != 0, ret, "crypto_mutex_lock failed\n");

    ret = drv_cipher_hash_set(g_mbedtls_hash_handle, (crypto_hash_clone_ctx *)clone_ctx);
    crypto_chk_goto(ret != 0, exit_mutex_unlock, "drv_cipher_hash_set failed\n");

    ret = drv_cipher_hash_finish_data(g_mbedtls_hash_handle, out, &hash_out_len);
    crypto_chk_goto(ret != 0, exit_mutex_unlock, "drv_cipher_hash_finish_data failed\n");

exit_mutex_unlock:
    crypto_mutex_unlock(&g_mbedtls_hash_lock);
    crypto_drv_func_exit();

    return 0;
}

void __attribute__((weak)) mbedtls_alt_hash_register(const mbedtls_alt_hash_harden_func *hash_func)
{
    crypto_unused(hash_func);
    return;
}

int mbedtls_alt_hash_init(void)
{
    crypto_hash_attr hash_attr;

    mbedtls_alt_hash_harden_func func = {
        .start = mbedtls_alt_hash_start_impl,
        .update = mbedtls_alt_hash_update_impl,
        .finish = mbedtls_alt_hash_finish_impl
    };

    if (g_mbedtls_hash_handle == INVALID_HANDLE) {
        hash_attr.hash_type = CRYPTO_HASH_TYPE_SHA256;
        (void)drv_cipher_hash_start(&g_mbedtls_hash_handle, &hash_attr);
    }

    mbedtls_alt_hash_register(&func);
    (void)crypto_mutex_init(&g_mbedtls_hash_lock);
    return 0;
}

int mbedtls_alt_hash_deinit(void)
{
    mbedtls_alt_hash_harden_func func = { 0 };

    if (g_mbedtls_hash_handle != INVALID_HANDLE) {
        (void)drv_cipher_hash_destroy(g_mbedtls_hash_handle);
        g_mbedtls_hash_handle = INVALID_HANDLE;
    }

    mbedtls_alt_hash_register(&func);
    (void)crypto_mutex_destroy(&g_mbedtls_hash_lock);
    return 0;
}