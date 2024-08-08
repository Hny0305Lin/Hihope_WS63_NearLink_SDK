/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 消息摘要算法适配层接口mbedTLS实现（此文件为DEMO，需集成方适配修改）
 */
#include "hilink_sal_md.h"

#include <stddef.h>
#include <stdbool.h>
#include "mbedtls/sha256.h"
#ifdef SHA_384_512_SUPPROT
#include "mbedtls/sha512.h"
#endif
#include "mbedtls/md.h"
#include "securec.h"
#include "hilink_mem_adapter.h"

typedef struct HiLinkMdContextInner {
    HiLinkMdType type;
    void *mbedtlsMdContext;
} HiLinkMdContextInner;

mbedtls_md_type_t GetMbedtlsMdType(HiLinkMdType type)
{
    switch (type) {
        case HILINK_MD_NONE:
            return MBEDTLS_MD_NONE;
        case HILINK_MD_SHA256:
            return MBEDTLS_MD_SHA256;
        case HILINK_MD_SHA384:
            return MBEDTLS_MD_SHA384;
        case HILINK_MD_SHA512:
            return MBEDTLS_MD_SHA512;
        default:
            HILINK_SAL_WARN("invalid mode\r\n");
            return MBEDTLS_MD_NONE;
    }
}

bool IsMdLenInvalid(HiLinkMdType type, unsigned int len)
{
    if (type == HILINK_MD_SHA256) {
        return len >= HILINK_MD_SHA256_BYTE_LEN;
    } else if (type == HILINK_MD_SHA384) {
        return len >= HILINK_MD_SHA384_BYTE_LEN;
    } else if (type == HILINK_MD_SHA512) {
        return len >= HILINK_MD_SHA512_BYTE_LEN;
    }
    HILINK_SAL_WARN("invalid type\r\n");
    return false;
}

int HILINK_SAL_MdCalc(HiLinkMdType type, const unsigned char *inData, unsigned int inLen,
    unsigned char *md, unsigned int mdLen)
{
    if ((inData == NULL) || (inLen == 0) || (md == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    int ret;
    switch (type) {
        case HILINK_MD_SHA256:
            if (mdLen != HILINK_MD_SHA256_BYTE_LEN) {
                HILINK_SAL_WARN("invalid md len\r\n");
                return HILINK_SAL_PARAM_INVALID;
            }
#ifdef MBEDTLS_VERSION_3
            ret = mbedtls_sha256(inData, inLen, md, 0);
#else
            ret = mbedtls_sha256_ret(inData, inLen, md, 0);
#endif
            if (ret != 0) {
                HILINK_SAL_ERROR("error, ret %d\r\n", ret);
            }
            return ret;
#ifdef SHA_384_512_SUPPROT
        case HILINK_MD_SHA384:
            if (mdLen != HILINK_MD_SHA384_BYTE_LEN) {
                HILINK_SAL_WARN("invalid md len\r\n");
                return HILINK_SAL_PARAM_INVALID;
            }
#ifdef MBEDTLS_VERSION_3
            ret = mbedtls_sha512(inData, inLen, md, 1);
#else
            ret = mbedtls_sha512_ret(inData, inLen, md, 1);
#endif
            if (ret != 0) {
                HILINK_SAL_ERROR("error, ret %d\r\n", ret);
            }
            return ret;
        case HILINK_MD_SHA512:
            if (mdLen != HILINK_MD_SHA512_BYTE_LEN) {
                HILINK_SAL_WARN("invalid md len\r\n");
                return HILINK_SAL_PARAM_INVALID;
            }
#ifdef MBEDTLS_VERSION_3
            ret = mbedtls_sha512(inData, inLen, md, 0);
#else
            ret = mbedtls_sha512_ret(inData, inLen, md, 0);
#endif
            if (ret != 0) {
                HILINK_SAL_ERROR("error, ret %d\r\n", ret);
            }
            return ret;
#endif
        default:
            HILINK_SAL_WARN("invalid param\r\n");
            return HILINK_SAL_NOT_SUPPORT;
    }
}

int HILINK_SAL_HmacCalc(const HiLinkHmacParam *param, unsigned char *hmac, unsigned int hmacLen)
{
    if ((param == NULL) || (param->key == NULL) || (param->keyLen == 0) ||
        (param->data == NULL) || (param->dataLen == 0) || (hmac == NULL) ||
        (!IsMdLenInvalid(param->md, hmacLen))) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    const mbedtls_md_info_t *mdInfo = mbedtls_md_info_from_type(GetMbedtlsMdType(param->md));
    if (mdInfo == NULL) {
        HILINK_SAL_WARN("invalid md type\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    int ret = mbedtls_md_hmac(mdInfo, param->key, param->keyLen, param->data, param->dataLen, hmac);
    if (ret != 0) {
        HILINK_SAL_ERROR("hmac error, ret %d\r\n", ret);
    }
    return ret;
}

HiLinkMdContext HILINK_SAL_MdInit(HiLinkMdType type)
{
    HiLinkMdContextInner *ctx = (HiLinkMdContextInner *)HILINK_Malloc(sizeof(HiLinkMdContextInner));
    if (ctx == NULL) {
        HILINK_SAL_ERROR("malloc error\r\n");
        return NULL;
    }
    (void)memset_s(ctx, sizeof(HiLinkMdContextInner), 0, sizeof(HiLinkMdContextInner));
    ctx->type = type;
    if (ctx->type == HILINK_MD_SHA256) {
        ctx->mbedtlsMdContext = (void *)HILINK_Malloc(sizeof(mbedtls_sha256_context));
#ifdef SHA_384_512_SUPPROT
    } else if ((ctx->type == HILINK_MD_SHA384) || (ctx->type == HILINK_MD_SHA512)) {
        ctx->mbedtlsMdContext = (void *)HILINK_Malloc(sizeof(mbedtls_sha512_context));
#endif
    }
    if (ctx->mbedtlsMdContext == NULL) {
        HILINK_SAL_ERROR("invalid md type or malloc error\r\n");
        HILINK_Free(ctx);
        return NULL;
    }
    int ret = HILINK_SAL_NOT_SUPPORT;
    if (ctx->type == HILINK_MD_SHA256) {
        mbedtls_sha256_init(ctx->mbedtlsMdContext);
#ifdef MBEDTLS_VERSION_3
        ret = mbedtls_sha256_starts(ctx->mbedtlsMdContext, 0);
#else
        ret = mbedtls_sha256_starts_ret(ctx->mbedtlsMdContext, 0);
#endif
#ifdef SHA_384_512_SUPPROT
    } else if (ctx->type == HILINK_MD_SHA384) {
        mbedtls_sha512_init(ctx->mbedtlsMdContext);
#ifdef MBEDTLS_VERSION_3
        ret = mbedtls_sha512_starts(ctx->mbedtlsMdContext, 1);
#else
        ret = mbedtls_sha512_starts_ret(ctx->mbedtlsMdContext, 1);
#endif
    } else if (ctx->type == HILINK_MD_SHA512) {
        mbedtls_sha512_init(ctx->mbedtlsMdContext);
#ifdef MBEDTLS_VERSION_3
        ret = mbedtls_sha512_starts(ctx->mbedtlsMdContext, 0);
#else
        ret = mbedtls_sha512_starts_ret(ctx->mbedtlsMdContext, 0);
#endif
#endif
    }
    if (ret != 0) {
        HILINK_SAL_ERROR("md init error %d\r\n", ret);
        HILINK_Free(ctx->mbedtlsMdContext);
        HILINK_Free(ctx);
        return NULL;
    }
    return ctx;
}

int HILINK_SAL_MdUpdate(HiLinkMdContext ctx, const unsigned char *inData, unsigned int inLen)
{
    if (ctx == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    HiLinkMdContextInner *ctxInner = (HiLinkMdContextInner *)ctx;
    int ret = HILINK_SAL_NOT_SUPPORT;
    if (ctxInner->type == HILINK_MD_SHA256) {
#ifdef MBEDTLS_VERSION_3
        ret = mbedtls_sha256_update(ctxInner->mbedtlsMdContext, inData, inLen);
#else
        ret = mbedtls_sha256_update_ret(ctxInner->mbedtlsMdContext, inData, inLen);
#endif
#ifdef SHA_384_512_SUPPROT
    } else if ((ctxInner->type == HILINK_MD_SHA384) || (ctxInner->type == HILINK_MD_SHA512)) {
#ifdef MBEDTLS_VERSION_3
        ret = mbedtls_sha512_update(ctxInner->mbedtlsMdContext, inData, inLen);
#else
        ret = mbedtls_sha512_update_ret(ctxInner->mbedtlsMdContext, inData, inLen);
#endif
#endif
    }
    return ret;
}

int HILINK_SAL_MdFinish(HiLinkMdContext ctx, unsigned char *outData, unsigned int outLen)
{
    if (ctx == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    HiLinkMdContextInner *ctxInner = (HiLinkMdContextInner *)ctx;
    if (!IsMdLenInvalid(ctxInner->type, outLen)) {
        HILINK_SAL_WARN("invalid outlen\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    int ret = HILINK_SAL_NOT_SUPPORT;
    if (ctxInner->type == HILINK_MD_SHA256) {
#ifdef MBEDTLS_VERSION_3
        ret = mbedtls_sha256_finish(ctxInner->mbedtlsMdContext, outData);
#else
        ret = mbedtls_sha256_finish_ret(ctxInner->mbedtlsMdContext, outData);
#endif
#ifdef SHA_384_512_SUPPROT
    } else if ((ctxInner->type == HILINK_MD_SHA384) || (ctxInner->type == HILINK_MD_SHA512)) {
#ifdef MBEDTLS_VERSION_3
        ret = mbedtls_sha512_finish(ctxInner->mbedtlsMdContext, outData);
#else
        ret = mbedtls_sha512_finish_ret(ctxInner->mbedtlsMdContext, outData);
#endif
#endif
    }
    return ret;
}

void HILINK_SAL_MdFree(HiLinkMdContext ctx)
{
    if (ctx == NULL) {
        return;
    }
    HiLinkMdContextInner *ctxInner = (HiLinkMdContextInner *)ctx;
    if (ctxInner->mbedtlsMdContext != NULL) {
        if (ctxInner->type == HILINK_MD_SHA256) {
            mbedtls_sha256_free(ctxInner->mbedtlsMdContext);
#ifdef SHA_384_512_SUPPROT
        } else if ((ctxInner->type == HILINK_MD_SHA384) || (ctxInner->type == HILINK_MD_SHA512)) {
            mbedtls_sha512_free(ctxInner->mbedtlsMdContext);
#endif
        }
        HILINK_Free(ctxInner->mbedtlsMdContext);
    }

    HILINK_Free(ctxInner);
}