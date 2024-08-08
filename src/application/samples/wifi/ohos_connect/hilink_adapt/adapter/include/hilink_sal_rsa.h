/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: RSA加解密适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SAL_RSA_H
#define HILINK_SAL_RSA_H

#include "hilink_sal_defines.h"
#include "hilink_sal_md.h"
#include "hilink_sal_mpi.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief RSA PKCS填充模式 */
typedef enum HiLinkRsaPkcs1Mode {
    HILINK_RSA_PKCS1_V15 = 0,                     /**< PKCS#1 v1.5 */
    HILINK_RSA_PKCS1_V21,                         /**< PKCS#1 v2.1 */
    HILINK_RSA_PKCS1_RESERVED = ENUM_INT_REVERSED /**< 防止编译器优化 */
} HiLinkRsaPkcs1Mode;

/** @brief RSA 操作模式 */
typedef enum HiLinkRsaOpMode {
    HILINK_RSA_OP_PUBLIC = 0,                  /**< 公钥操作 */
    HILINK_RSA_OP_PRIVATE,                     /**< 私钥操作 */
    HILINK_RSA_OP_RESERVED = ENUM_INT_REVERSED /**< 防止编译器优化 */
} HiLinkRsaOpMode;

/** @brief RAS上下文 */
typedef void* HiLinkRsaContext;

/** @brief RSA核心参数 */
typedef struct HiLinkRsaParam {
    HiLinkMpi n; /**< 模数 */
    HiLinkMpi p; /**< 第一个素数因子 */
    HiLinkMpi q; /**< 第二个素数因子 */
    HiLinkMpi d; /**< 私有指数 */
    HiLinkMpi e; /**< 公有指数 */
} HiLinkRsaParam;

/** @brief RSA解密参数 */
typedef struct HiLinkRsaCryptParam {
    HiLinkRsaContext ctx;                             /**< RSA上下文 */
    HiLinkRsaOpMode mode;                             /**< 操作模式 */
    int (*rng)(unsigned char *out, unsigned int len); /**< 随机数发生器，私钥解密、公钥加密、HILINK_RSA_PKCS1_V21加密时应该提供 */
    const unsigned char *input;                       /**< 密文，可写长度至少为inLen */
    unsigned int inLen;                               /**< 密文长度，解密时应该与模数长度一致 */
} HiLinkRsaCryptParam;

/**
 * @brief RSA上下文初始化
 *
 * @param padding [IN] 填充模式
 * @param md [IN] 摘要算法类型，padding为HILINK_RSA_PKCS_V21时使用
 * @return NULL失败，非NULL为RSA上下文指针
 * @attention 返回的上下文使用完毕后应该使用HILINK_SAL_RsaFree释放
 */
HiLinkRsaContext HILINK_SAL_RsaInit(HiLinkRsaPkcs1Mode padding, HiLinkMdType md);

/**
 * @brief RSA上下文释放
 *
 * @param ctx [IN] RSA上下文
 */
void HILINK_SAL_RsaFree(HiLinkRsaContext ctx);

/**
 * @brief RSA导入核心参数
 *
 * @param ctx [IN] RSA上下文
 * @param param [IN] RSA参数
 * @return 0成功，非0失败
 */
int HILINK_SAL_RsaParamImport(HiLinkRsaContext ctx, const HiLinkRsaParam *param);

/**
 * @brief 使用RSA公钥校验签名
 *
 * @param ctx [IN] RSA上下文
 * @param md [IN] 签名摘要算法
 * @param hash [IN] 消息摘要，可读长度至少为hashLen
 * @param hashLen [IN] 消息摘要长度
 * @param sig [IN] 签名，可读长度至少为sigLen
 * @param sigLen [IN] 签名长度，应为RSA模数长度
 * @return 0成功，非0失败
 */
int HILINK_RsaPkcs1Verify(HiLinkRsaContext ctx, HiLinkMdType md, const unsigned char *hash,
    unsigned int hashLen, const unsigned char *sig, unsigned int sigLen);

/**
 * @brief RSA解密
 *
 * @param param [IN] 解密参数
 * @param buf [OUT] 解密缓冲区，可写长度至少为*len
 * @param len [IN,OUT] 输入为缓冲区长度，输出为解密数据长度
 * @return 0成功，非0失败
 */
int HILINK_RsaPkcs1Decrypt(const HiLinkRsaCryptParam *param, unsigned char *buf, unsigned int *len);

/**
 * @brief RSA加密
 *
 * @param param [IN] 加密参数
 * @param buf [OUT] 加密数据输出缓冲区，可写长度至少为*len
 * @param len [IN] 输出缓冲区长度，应该大于等于模数长度
 * @return int
 */
int HILINK_RsaPkcs1Encrypt(const HiLinkRsaCryptParam *param, unsigned char *buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_SAL_RSA_H */