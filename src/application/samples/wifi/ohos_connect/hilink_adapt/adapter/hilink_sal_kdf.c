/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 密钥派生算法适配层接口mbedTLS实现（此文件为DEMO，需集成方适配修改）
 */
#include "hilink_sal_kdf.h"

#include <stddef.h>
#include <stdbool.h>
#include "mbedtls/md.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/pkcs5.h"
#include "hilink_mbedtls_utils.h"

int HILINK_SAL_Hkdf(const HiLinkHkdfParam *param, unsigned char *key, unsigned int keyLen)
{
    bool isInvalid = ((param == NULL) || (param->material == NULL) || (key == NULL) ||
        (param->materialLen == 0) || (keyLen == 0));
    if (isInvalid) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    const mbedtls_md_info_t *mdInfo = mbedtls_md_info_from_type(GetMbedtlsMdType(param->md));
    if (mdInfo == NULL) {
        return HILINK_SAL_NOK;
    }

    int ret = mbedtls_hkdf(mdInfo, param->salt, param->saltLen, param->material,
        param->materialLen, param->info, param->infoLen, key, keyLen);
    if (ret != 0) {
        HILINK_SAL_WARN("hkdf error %d\r\n", ret);
        return ret;
    }

    return HILINK_SAL_OK;
}

int HILINK_SAL_Pkcs5Pbkdf2Hmac(const HiLinkPbkdf2HmacParam *param, unsigned char *key, unsigned int keyLen)
{
    bool isInvalid = ((param == NULL) || (param->password == NULL) || (param->passwordLen == 0) ||
        (param->salt == NULL) || (param->saltLen == 0) || (key == NULL) || (keyLen == 0));
    if (isInvalid) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    const mbedtls_md_info_t *mdInfo = mbedtls_md_info_from_type(GetMbedtlsMdType(param->md));
    if (mdInfo == NULL) {
        HILINK_SAL_WARN("invalid md info\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    mbedtls_md_context_t context;
    mbedtls_md_init(&context);
    int ret;

    do {
        /* 数字1表示使用HMAC */
        ret = mbedtls_md_setup(&context, mdInfo, 1);
        if (ret != 0) {
            HILINK_SAL_ERROR("md setup error %d\r\n", ret);
            break;
        }

        ret = mbedtls_pkcs5_pbkdf2_hmac(&context, param->password, param->passwordLen,
            param->salt, param->saltLen, param->iterCount, keyLen, key);
        if (ret != 0) {
            HILINK_SAL_ERROR("mbedtls_pkcs5_pbkdf2_hmac error %d\r\n", ret);
            break;
        }
    } while (false);

    mbedtls_md_free(&context);
    return ret;
}