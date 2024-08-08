/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 安全随机数适配层接口mbedTLS实现（此文件为DEMO，需集成方适配修改）
 */

#include "hilink_sal_drbg.h"
#include <stddef.h>
#include <stdbool.h>
#include "mbedtls/entropy.h"
#ifndef MBEDTLS_VERSION_3
#include "mbedtls/entropy_poll.h"
#endif
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/platform.h"
#include "hilink_sal_defines.h"
#include "hilink_thread_adapter.h"
#include "hilink_time_adapter.h"
#include "hilink_stdio_adapter.h"
#include "hilink_str_adapter.h"
#include "securec.h"
#include "hilink_mem_adapter.h"

#ifdef MBEDTLS_VERSION_3
#define HILINK_MBEDTLS_ENTROPY_MIN_HARDCLOCK 4
#endif

typedef struct HiLinkDrbgContextInner {
    HiLinkMutexId mutex;
    mbedtls_ctr_drbg_context drbg;
    mbedtls_entropy_context entropy;
} HiLinkDrbgContextInner;

/* 此函数为注册到mbedtls的熵源回调函数，按照mbedtls_entropy_f_source_ptr的定义进行实现 */
static int HiLinkMbedtlsEntropySourceCallback(void *data, unsigned char *output, size_t len, size_t *outLen)
{
    (void)data;
    if ((output == NULL) || (outLen == NULL)) {
        HILINK_SAL_ERROR("invalid param\r\n");
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    /* 支持安全随机数的平台直接使用安全随机数做熵源 */
    if (HILINK_Trng(output, len) == 0) {
        *outLen = len;
        return 0;
    }

    HiLinkTimeval timeval = {0, 0};
    if (HILINK_GetOsTime(&timeval) != 0) {
        HILINK_SAL_ERROR("get time err\r\n");
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    if (memcpy_s(output, len, &timeval, sizeof(HiLinkTimeval)) != EOK) {
        HILINK_SAL_ERROR("memcpy_s error\r\n");
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
    *outLen = sizeof(HiLinkTimeval);

    return 0;
}

static void *PlatformCalloc(size_t n, size_t size)
{
    if (size == 0) {
        return NULL;
    }
    if (n > (UINT_MAX / size)) {
        return NULL;
    }

    /* 上层调用保证不会出现溢出 */
    unsigned int len = (unsigned int)(n * size);
    if (len == 0) {
        return NULL;
    }

    void *data = HILINK_Malloc(len);
    if (data != NULL) {
        (void)memset_s((char *)data, len, 0, len);
    }

    return data;
}

HiLinkDrbgContext HILINK_SAL_DrbgInit(const char *custom)
{
    static bool isMbedtlsMemHookInit = false;
    if (!isMbedtlsMemHookInit) {
        (void)mbedtls_platform_set_calloc_free(PlatformCalloc, HILINK_Free);
        isMbedtlsMemHookInit = true;
    }

    HiLinkDrbgContextInner *ctx = (HiLinkDrbgContextInner *)HILINK_Malloc(sizeof(HiLinkDrbgContextInner));
    if (ctx == NULL) {
        HILINK_SAL_ERROR("malloc err\r\n");
        return NULL;
    }
    (void)memset_s(ctx, sizeof(HiLinkDrbgContextInner), 0, sizeof(HiLinkDrbgContextInner));

    do {
        ctx->mutex = HILINK_MutexCreate();
        if (ctx->mutex == NULL) {
            HILINK_SAL_ERROR("create mutex err\r\n");
            break;
        }

        mbedtls_entropy_init(&ctx->entropy);
        mbedtls_ctr_drbg_init(&ctx->drbg);

#ifdef MBEDTLS_VERSION_3
        int ret = mbedtls_entropy_add_source(&ctx->entropy, HiLinkMbedtlsEntropySourceCallback,
            NULL, HILINK_MBEDTLS_ENTROPY_MIN_HARDCLOCK, MBEDTLS_ENTROPY_SOURCE_STRONG);
#else
        int ret = mbedtls_entropy_add_source(&ctx->entropy, HiLinkMbedtlsEntropySourceCallback,
            NULL, MBEDTLS_ENTROPY_MIN_HARDCLOCK, MBEDTLS_ENTROPY_SOURCE_STRONG);
#endif
        if (ret != 0) {
            HILINK_SAL_ERROR("add entropy source error -0x%04x\r\n", -ret);
            break;
        }

        ret = mbedtls_ctr_drbg_seed(&ctx->drbg, mbedtls_entropy_func, &ctx->entropy,
            (const unsigned char *)custom, custom == NULL ? 0 : HILINK_Strlen(custom));
        if (ret != 0) {
            HILINK_SAL_ERROR("seed error -0x%04x\r\n", -ret);
            break;
        }

        mbedtls_ctr_drbg_set_prediction_resistance(&ctx->drbg, MBEDTLS_CTR_DRBG_PR_OFF);

        return ctx;
    } while (false);

    HILINK_SAL_DrbgDeinit(ctx);
    return NULL;
}

void HILINK_SAL_DrbgDeinit(HiLinkDrbgContext ctx)
{
    if (ctx == NULL) {
        return;
    }

    HiLinkDrbgContextInner *innerCtx = (HiLinkDrbgContextInner *)ctx;
    mbedtls_entropy_free(&innerCtx->entropy);
    mbedtls_ctr_drbg_free(&innerCtx->drbg);
    if (innerCtx->mutex != NULL) {
        HILINK_MutexDestroy(innerCtx->mutex);
        innerCtx->mutex = NULL;
    }
    HILINK_Free(innerCtx);
}

int HILINK_SAL_DrbgRandom(HiLinkDrbgContext ctx, unsigned char *out, unsigned int outLen)
{
    if ((ctx == NULL) || (out == NULL) || (outLen == 0)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    HiLinkDrbgContextInner *innerCtx = (HiLinkDrbgContextInner *)ctx;
    if (HILINK_MutexLock(innerCtx->mutex, HILINK_WAIT_FOREVER) != 0) {
        HILINK_SAL_ERROR("mutex lock fail\r\n");
        return HILINK_SAL_MUTEX_ERR;
    }

    int ret = mbedtls_ctr_drbg_random((void *)&innerCtx->drbg, out, outLen);
    (void)HILINK_MutexUnlock(innerCtx->mutex);
    if (ret != 0) {
        HILINK_SAL_ERROR("get mbedtls random error -0x%04x\r\n", -ret);
        return ret;
    }

    return HILINK_SAL_OK;
}