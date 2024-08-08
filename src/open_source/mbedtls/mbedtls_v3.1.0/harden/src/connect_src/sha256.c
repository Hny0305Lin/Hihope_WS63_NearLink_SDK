/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: sha256 harden adapt functions.
 *
 * Create: 2024-07-18
*/

#include "mbedtls/sha256.h"
#include "mbedtls_harden_adapt_api.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"
#include "hash_harden_common.h"
#include "dfx.h"
#include <string.h>
#include <securec.h>

#define SHA224_LEN      28
#define SHA256_LEN      32

#define SHA256_VALIDATE_RET(cond)                           \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA256_BAD_INPUT_DATA)
#define SHA256_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE(cond)

#if defined(MBEDTLS_SHA256_ALT)

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
    SHA256_VALIDATE(ctx != NULL);
    (void)memset_s(ctx, sizeof(mbedtls_sha256_context), 0, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
    SHA256_VALIDATE(ctx != NULL);
    (void)memset_s(ctx, sizeof(mbedtls_sha256_context), 0, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src)
{
    SHA256_VALIDATE(dst != NULL);
    SHA256_VALIDATE(src != NULL);

    (void)memcpy_s(dst, sizeof(mbedtls_sha256_context), src, sizeof(mbedtls_sha256_context));
}

int mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();

    SHA256_VALIDATE_RET(ctx != NULL);
#if defined(MBEDTLS_SHA224_C)
    SHA256_VALIDATE_RET(is224 == 0 || is224 == 1);
#else
    SHA256_VALIDATE_RET(is224 == 0);
#endif
#if defined(MBEDTLS_SHA224_C)
    if(is224 == 1) {
        ctx->result_size = SHA224_LEN;
        ctx->clone_ctx.hash_type = MBEDTLS_ALT_HASH_TYPE_SHA224;
    }
#endif
    if(is224 == 0) {
        ctx->result_size = SHA256_LEN;
        ctx->clone_ctx.hash_type = MBEDTLS_ALT_HASH_TYPE_SHA256;
    }

    ret = mbedtls_alt_hash_start(&ctx->clone_ctx, ctx->clone_ctx.hash_type);

    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    int ret = -1;

    mbedtls_harden_log_func_enter();
    SHA256_VALIDATE_RET(ctx != NULL);
    SHA256_VALIDATE_RET(ilen == 0 || input != NULL);

    if(ilen == 0) {
        return 0;
    }

    ret = mbedtls_alt_hash_update(&ctx->clone_ctx, input, ilen);

    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char *output)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    SHA256_VALIDATE_RET(ctx != NULL);
    SHA256_VALIDATE_RET((unsigned char *)output != NULL);

    ret = mbedtls_alt_hash_finish(&ctx->clone_ctx, output, ctx->result_size);

    mbedtls_harden_log_func_exit();
    return ret;
}

#endif /* MBEDTLS_SHA256_ALT */
