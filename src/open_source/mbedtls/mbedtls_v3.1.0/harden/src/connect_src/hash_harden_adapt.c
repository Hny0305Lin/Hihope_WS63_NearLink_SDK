/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: hash harden adapt functions.
 *
 * Create: 2024-07-18
*/

#include "mbedtls_harden_adapt_api.h"
#include "dfx.h"

static mbedtls_alt_hash_harden_func g_hash_func;

int mbedtls_alt_hash_start(mbedtls_alt_hash_clone_ctx *clone_ctx, mbedtls_alt_hash_type hash_type)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_hash_func.start == NULL) {
        mbedtls_printf("Error: hash start unregister!\n");
        return -1;
    }
    ret = g_hash_func.start(clone_ctx, hash_type);
    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_alt_hash_update(mbedtls_alt_hash_clone_ctx *clone_ctx, const unsigned char *data, unsigned int data_len)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_hash_func.update == NULL) {
        mbedtls_printf("Error: hash update unregister!\n");
        return -1;
    }
    ret = g_hash_func.update(clone_ctx, data, data_len);
    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_alt_hash_finish(mbedtls_alt_hash_clone_ctx *clone_ctx, unsigned char *out, unsigned int out_len)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    if (g_hash_func.finish == NULL) {
        mbedtls_printf("Error: hash finish unregister!\n");
        return -1;
    }
    ret = g_hash_func.finish(clone_ctx, out, out_len);
    mbedtls_harden_log_func_exit();
    return ret;
}

void mbedtls_alt_hash_register(const mbedtls_alt_hash_harden_func *hash_func)
{
    if (hash_func == NULL) {
        return;
    }
    if (hash_func->start == NULL ||
        hash_func->update == NULL ||
        hash_func->finish == NULL) {
        return;
    }
    g_hash_func.start = hash_func->start;
    g_hash_func.update = hash_func->update;
    g_hash_func.finish = hash_func->finish;
}