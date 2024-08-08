/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: sha512 harden adapt functions.
 *
 * Create: 2024-07-18
*/

#include "mbedtls/sha512.h"
#include "mbedtls_harden_adapt_api.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"
#include "hash_harden_common.h"
#include "dfx.h"
#include <string.h>
#include <securec.h>

#define SHA384_LEN      48
#define SHA512_LEN      64

#define SHA512_VALIDATE_RET(cond)                           \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA512_BAD_INPUT_DATA)
#define SHA512_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE(cond)

#if defined(MBEDTLS_SHA512_ALT)

void mbedtls_sha512_init(mbedtls_sha512_context *ctx)
{
    SHA512_VALIDATE(ctx != NULL);
    (void)memset_s(ctx, sizeof(mbedtls_sha512_context), 0, sizeof(mbedtls_sha512_context));
}

void mbedtls_sha512_free(mbedtls_sha512_context *ctx)
{
    SHA512_VALIDATE(ctx != NULL);
    (void)memset_s(ctx, sizeof(mbedtls_sha512_context), 0, sizeof(mbedtls_sha512_context));
}

void mbedtls_sha512_clone(mbedtls_sha512_context *dst,
                           const mbedtls_sha512_context *src)
{
    SHA512_VALIDATE(dst != NULL);
    SHA512_VALIDATE(src != NULL);

    (void)memcpy_s(dst, sizeof(mbedtls_sha512_context), src, sizeof(mbedtls_sha512_context));
}

int mbedtls_sha512_starts(mbedtls_sha512_context *ctx, int is384)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();

    SHA512_VALIDATE_RET(ctx != NULL);
#if defined(MBEDTLS_SHA384_C)
    SHA512_VALIDATE_RET(is384 == 0 || is384 == 1);
#else
    SHA512_VALIDATE_RET(is384 == 0);
#endif
#if defined(MBEDTLS_SHA384_C)
    if(is384 == 1) {
        ctx->result_size = SHA384_LEN;
        ctx->clone_ctx.hash_type = MBEDTLS_ALT_HASH_TYPE_SHA384;
    }
#endif
    if(is384 == 0) {
        ctx->result_size = SHA512_LEN;
        ctx->clone_ctx.hash_type = MBEDTLS_ALT_HASH_TYPE_SHA512;
    }

    ret = mbedtls_alt_hash_start(&ctx->clone_ctx, ctx->clone_ctx.hash_type);

    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_sha512_update(mbedtls_sha512_context *ctx, const unsigned char *input, size_t ilen)
{
    int ret = -1;

    mbedtls_harden_log_func_enter();
    SHA512_VALIDATE_RET(ctx != NULL);
    SHA512_VALIDATE_RET(ilen == 0 || input != NULL);

    if(ilen == 0) {
        return 0;
    }

    ret = mbedtls_alt_hash_update(&ctx->clone_ctx, input, ilen);

    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_sha512_finish(mbedtls_sha512_context *ctx, unsigned char *output)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    SHA512_VALIDATE_RET(ctx != NULL);
    SHA512_VALIDATE_RET((unsigned char *)output != NULL);

    ret = mbedtls_alt_hash_finish(&ctx->clone_ctx, output, ctx->result_size);

    mbedtls_harden_log_func_exit();
    return ret;
}

#endif /* MBEDTLS_SHA512_ALT */
