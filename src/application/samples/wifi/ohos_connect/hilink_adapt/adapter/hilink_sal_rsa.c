/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: RSA加解密适配层接口mbedTLS实现（此文件为DEMO，需集成方适配修改）
 */

#include "hilink_sal_rsa.h"

#include <stddef.h>
#include <stdbool.h>
#include "mbedtls/rsa.h"
#include "hilink_mem_adapter.h"
#include "securec.h"
#include "hilink_mbedtls_utils.h"

#define MBEDTLS_RADIX_NUM_BASE 16

typedef struct HiLinkRsaContextInner {
    HiLinkRsaPkcs1Mode padding;
    HiLinkMdType md;
    mbedtls_rsa_context ctx;
    int (*rng)(unsigned char *out, unsigned int len);
} HiLinkRsaContextInner;

HiLinkRsaContext HILINK_SAL_RsaInit(HiLinkRsaPkcs1Mode padding, HiLinkMdType md)
{
    HiLinkRsaContextInner *ctx = HILINK_Malloc(sizeof(HiLinkRsaContextInner));
    if (ctx == NULL) {
        HILINK_SAL_WARN("malloc error\r\n");
        return NULL;
    }
    (void)memset_s(ctx, sizeof(HiLinkRsaContextInner), 0, sizeof(HiLinkRsaContextInner));
    ctx->padding = padding;
    ctx->md = md;
#ifdef MBEDTLS_VERSION_3
    mbedtls_rsa_init(&ctx->ctx);
    mbedtls_rsa_set_padding(&ctx->ctx, padding, GetMbedtlsMdType(md));
#else
    mbedtls_rsa_init(&ctx->ctx, padding, GetMbedtlsMdType(md));
#endif
    return ctx;
}

void HILINK_SAL_RsaFree(HiLinkRsaContext ctx)
{
    if (ctx == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return;
    }
    mbedtls_rsa_free(&((HiLinkRsaContextInner *)ctx)->ctx);
    HILINK_Free(ctx);
}

int HILINK_SAL_RsaParamImport(HiLinkRsaContext ctx, const HiLinkRsaParam *param)
{
    if ((ctx == NULL) || (param == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    int ret = mbedtls_rsa_import(&((HiLinkRsaContextInner *)ctx)->ctx,
        GetMbedtlsMpi(param->n), GetMbedtlsMpi(param->p),
        GetMbedtlsMpi(param->q), GetMbedtlsMpi(param->d), GetMbedtlsMpi(param->e));
    if (ret != 0) {
        HILINK_SAL_WARN("import error %d\r\n", ret);
    }
    return ret;
}

int HILINK_RsaPkcs1Verify(HiLinkRsaContext ctx, HiLinkMdType md, const unsigned char *hash,
    unsigned int hashLen, const unsigned char *sig, unsigned int sigLen)
{
#ifdef MBEDTLS_VERSION_3
    if ((ctx == NULL) || (hash == NULL) || (hashLen == 0) ||
        (sig == NULL) || (sigLen != ((HiLinkRsaContextInner *)ctx)->ctx.MBEDTLS_PRIVATE(len)) ||
        ((md != HILINK_MD_NONE) && !IsMdLenInvalid(md, hashLen))) {
#else
    if ((ctx == NULL) || (hash == NULL) || (hashLen == 0) ||
        (sig == NULL) || (sigLen != ((HiLinkRsaContextInner *)ctx)->ctx.len) ||
        ((md != HILINK_MD_NONE) && !IsMdLenInvalid(md, hashLen))) {
#endif
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    mbedtls_rsa_context *rsa = &((HiLinkRsaContextInner *)ctx)->ctx;
    int ret = mbedtls_rsa_check_pubkey(rsa);
    if (ret != 0) {
        HILINK_SAL_WARN("check error\r\n");
        return ret;
    }

    ret = mbedtls_rsa_pkcs1_verify(rsa,
#ifndef MBEDTLS_VERSION_3
        NULL, NULL, MBEDTLS_RSA_PUBLIC,
#endif
        GetMbedtlsMdType(md), hashLen, hash, sig);
    if (ret != 0) {
        HILINK_SAL_WARN("verify error\r\n");
        return ret;
    }

    return HILINK_SAL_OK;
}

static int RngForMbedtls(void *param, unsigned char *out, size_t len)
{
    HiLinkRsaContextInner *ctx = (HiLinkRsaContextInner *)param;
    if ((ctx == NULL) || (ctx->rng == NULL) || (out == NULL) || (len == 0)) {
        return -1;
    }
    return ctx->rng(out, len);
}

int HILINK_RsaPkcs1Decrypt(const HiLinkRsaCryptParam *param, unsigned char *buf, unsigned int *len)
{
#ifdef MBEDTLS_VERSION_3
    if ((param == NULL) || (param->ctx == NULL) || (param->input == NULL) ||
        (buf == NULL) || (param->inLen != ((HiLinkRsaContextInner *)(param->ctx))->ctx.MBEDTLS_PRIVATE(len)) ||
        (len == NULL) || (*len == 0)) {
#else
    if ((param == NULL) || (param->ctx == NULL) || (param->input == NULL) ||
        (buf == NULL) || (param->inLen != ((HiLinkRsaContextInner *)(param->ctx))->ctx.len) ||
        (len == NULL) || (*len == 0)) {
#endif
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    HiLinkRsaContextInner *ctx = (HiLinkRsaContextInner *)param->ctx;
    int ret;
    if (param->mode == HILINK_RSA_OP_PRIVATE) {
        if (param->rng == NULL) {
            HILINK_SAL_WARN("invalid rng\r\n");
            return HILINK_SAL_PARAM_INVALID;
        }
        ctx->rng = param->rng;
        ret = mbedtls_rsa_check_privkey(&ctx->ctx);
    } else {
#ifdef MBEDTLS_VERSION_3
        HILINK_SAL_WARN("mode not support public key\r\n");
        ret = HILINK_SAL_NOT_SUPPORT;
#else
        ret = mbedtls_rsa_check_pubkey(&ctx->ctx);
#endif
    }
    if (ret != 0) {
        HILINK_SAL_WARN("check error\r\n");
        return ret;
    }

    size_t oLen = 0;
    ret = mbedtls_rsa_pkcs1_decrypt(&ctx->ctx, RngForMbedtls, ctx,
#ifndef MBEDTLS_VERSION_3
        param->mode,
#endif
        &oLen, param->input, buf, *len);
    if (ret != 0) {
        HILINK_SAL_WARN("decrypt error\r\n");
        return ret;
    }
    *len = oLen;
    return HILINK_SAL_OK;
}

int HILINK_RsaPkcs1Encrypt(const HiLinkRsaCryptParam *param, unsigned char *buf, unsigned int len)
{
#ifdef MBEDTLS_VERSION_3
    if ((param == NULL) || (param->ctx == NULL) || (param->input == NULL) ||
        (buf == NULL) || (len < ((HiLinkRsaContextInner *)(param->ctx))->ctx.MBEDTLS_PRIVATE(len))) {
#else
    if ((param == NULL) || (param->ctx == NULL) || (param->input == NULL) ||
        (buf == NULL) || (len < ((HiLinkRsaContextInner *)(param->ctx))->ctx.len)) {
#endif
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    HiLinkRsaContextInner *ctx = (HiLinkRsaContextInner *)param->ctx;
    int ret;
    if (param->mode == HILINK_RSA_OP_PRIVATE) {
#ifdef MBEDTLS_VERSION_3
        HILINK_SAL_WARN("mode not support private key\r\n");
        ret = HILINK_SAL_NOT_SUPPORT;
#else
        ret = mbedtls_rsa_check_privkey(&ctx->ctx);
#endif
    } else {
        ret = mbedtls_rsa_check_pubkey(&ctx->ctx);
    }
    if (ret != 0) {
        HILINK_SAL_WARN("check error\r\n");
        return ret;
    }
    bool isNeedRng = false;
    if ((param->mode == HILINK_RSA_OP_PUBLIC) || (ctx->padding == HILINK_RSA_PKCS1_V21)) {
        if (param->rng == NULL) {
            HILINK_SAL_WARN("invalid rng\r\n");
            return HILINK_SAL_PARAM_INVALID;
        }
        ctx->rng = param->rng;
        isNeedRng = true;
    }

    ret = mbedtls_rsa_pkcs1_encrypt(&ctx->ctx, isNeedRng ? RngForMbedtls : NULL, isNeedRng ? ctx : NULL,
#ifndef MBEDTLS_VERSION_3
        param->mode,
#endif
        param->inLen, param->input, buf);
    if (ret != 0) {
        HILINK_SAL_WARN("encrypt error\r\n");
        return ret;
    }
    return HILINK_SAL_OK;
}