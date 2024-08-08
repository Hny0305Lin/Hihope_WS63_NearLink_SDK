/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 密钥派生算法适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SAL_KDF_H
#define HILINK_SAL_KDF_H

#include "hilink_sal_defines.h"
#include "hilink_sal_md.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 基于HMAC的PKCS#5 PBKDF2密钥派生参数 */
typedef struct HiLinkPbkdf2HmacParam {
    HiLinkMdType md;               /**< 摘要算法类型 */
    const unsigned char *password; /**< 密码，可读长度至少为passwordLen */
    unsigned int passwordLen;      /**< 密码长度 */
    const unsigned char *salt;     /**< 盐值，可读长度至少为saltLen */
    unsigned int saltLen;          /**< 盐值长度 */
    unsigned int iterCount;        /**< 迭代次数 */
} HiLinkPbkdf2HmacParam;

/** @brief HKDF密钥派生参数 */
typedef struct HiLinkHkdfParam {
    HiLinkMdType md;               /**< 摘要算法类型 */
    const unsigned char *salt;     /**< 盐值，可读长度至少为saltLen */
    unsigned int saltLen;          /**< 盐值长度，可以为0 */
    const unsigned char *info;     /**< 可选字符串，可读长度至少为infoLen */
    unsigned int infoLen;          /**< 可选字符串长度，可以为0 */
    const unsigned char *material; /**< 密钥派生材料，可读长度至少为material */
    unsigned int materialLen;      /**< 密钥派生材料长度 */
} HiLinkHkdfParam;

/**
 * @brief 基于HMAC的PKCS#5 PBKDF2密钥派生
 *
 * @param param [IN] 密钥派生参数
 * @param key [OUT] 生成密钥缓冲区，可写长度至少为keyLen
 * @param keyLen [IN] 生成密钥的长度，取决于param->md摘要算法
 * @return 0成功，其他失败
 */
int HILINK_SAL_Pkcs5Pbkdf2Hmac(const HiLinkPbkdf2HmacParam *param, unsigned char *key, unsigned int keyLen);

/**
 * @brief HKDF密钥派生
 *
 * @param param [IN] 密钥派生参数
 * @param key [OUT] 生成密钥缓冲区，可写长度至少为keyLen
 * @param keyLen [IN] 生成密钥的长度，不超过摘要算法类型字节数的255倍
 * @return 0成功，其他失败
 */
int HILINK_SAL_Hkdf(const HiLinkHkdfParam *param, unsigned char *key, unsigned int keyLen);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_SAL_MD_H */