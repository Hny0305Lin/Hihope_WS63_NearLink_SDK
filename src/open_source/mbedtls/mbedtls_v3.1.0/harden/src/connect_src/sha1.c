/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: sha1 harden adapt functions.
 *
 * Create: 2024-07-18
*/

#include "mbedtls/sha1.h"
#include "mbedtls_harden_adapt_api.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"
#include "hash_harden_common.h"
#include "dfx.h"
#include <string.h>
#include <securec.h>

#define SHA1_LEN      20

#define SHA1_VALIDATE_RET(cond)                           \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA1_BAD_INPUT_DATA)
#define SHA1_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE(cond)

#if defined(MBEDTLS_SHA1_ALT)

void mbedtls_sha1_init(mbedtls_sha1_context *ctx)
{
    SHA1_VALIDATE(ctx != NULL);
    (void)memset_s(ctx, sizeof(mbedtls_sha1_context), 0, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_free(mbedtls_sha1_context *ctx)
{
    if (ctx == NULL) {
        return;
    }
    (void)memset_s(ctx, sizeof(mbedtls_sha1_context), 0, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_clone(mbedtls_sha1_context *dst,
                           const mbedtls_sha1_context *src)
{
    SHA1_VALIDATE(dst != NULL);
    SHA1_VALIDATE(src != NULL);

    (void)memcpy_s(dst, sizeof(mbedtls_sha1_context), src, sizeof(mbedtls_sha1_context));
}

int mbedtls_sha1_starts(mbedtls_sha1_context *ctx)
{
    int ret = -1;
    mbedtls_harden_log_func_enter();

    SHA1_VALIDATE_RET(ctx != NULL);

    ctx->result_size = SHA1_LEN;
    ctx->clone_ctx.hash_type = MBEDTLS_ALT_HASH_TYPE_SHA1;

    ret = mbedtls_alt_hash_start(&ctx->clone_ctx, ctx->clone_ctx.hash_type);

    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_sha1_update(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
    int ret = -1;

    mbedtls_harden_log_func_enter();
    SHA1_VALIDATE_RET(ctx != NULL);
    SHA1_VALIDATE_RET(ilen == 0 || input != NULL);

    if(ilen == 0) {
        return 0;
    }

    ret = mbedtls_alt_hash_update(&ctx->clone_ctx, input, ilen);

    mbedtls_harden_log_func_exit();
    return ret;
}

int mbedtls_sha1_finish(mbedtls_sha1_context *ctx, unsigned char output[20])
{
    int ret = -1;
    mbedtls_harden_log_func_enter();
    SHA1_VALIDATE_RET(ctx != NULL);
    SHA1_VALIDATE_RET((unsigned char *)output != NULL);

    ret = mbedtls_alt_hash_finish(&ctx->clone_ctx, output, ctx->result_size);

    mbedtls_harden_log_func_exit();
    return ret;
}

#endif /* MBEDTLS_SHA1_ALT */
