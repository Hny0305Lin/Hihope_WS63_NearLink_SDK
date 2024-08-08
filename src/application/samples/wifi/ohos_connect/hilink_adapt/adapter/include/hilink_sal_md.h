/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 消息摘要算法适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SAL_MD_H
#define HILINK_SAL_MD_H

#include "hilink_sal_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 消息摘要计算上下文 */
typedef void* HiLinkMdContext;

/** @brief 消息摘要算法类型 */
typedef enum HiLinkMdType {
    HILINK_MD_NONE = 0,
    HILINK_MD_SHA256,                      /**< SHA-256消息摘要算法 */
    HILINK_MD_SHA384,                      /**< SHA-384消息摘要算法 */
    HILINK_MD_SHA512,                      /**< SHA-512消息摘要算法 */
    HILINK_MD_RESERVED = ENUM_INT_REVERSED /**< 避免编译器优化 */
} HiLinkMdType;

/** @brief 不同消息摘要算法摘要字节数 */
enum HiLinkMdByteLen {
    HILINK_MD_SHA256_BYTE_LEN = 32, /**< SHA-256消息摘要字节数 */
    HILINK_MD_SHA384_BYTE_LEN = 48, /**< SHA-384消息摘要字节数 */
    HILINK_MD_SHA512_BYTE_LEN = 64, /**< SHA-512消息摘要字节数 */
};

/** @brief HMAC计算参数 */
typedef struct HiLinkHmacParam {
    HiLinkMdType md;           /**< 消息摘要算法 */
    const unsigned char *key;  /**< 密钥，可读长度至少为keyLen */
    unsigned int keyLen;       /**< 密钥长度 */
    const unsigned char *data; /**< 待计算HMAC数据，可读长度至少为dataLen */
    unsigned int dataLen;      /**< 数据长度 */
} HiLinkHmacParam;

/**
 * @brief 计算消息摘要
 *
 * @param type [IN] 消息摘要算法
 * @param inData [IN] 待计算摘要的数据，可读长度至少为inLen
 * @param inLen [IN] 待计算摘要的数据长度
 * @param md [OUT] 生成的消息摘要缓冲区，可写长度至少为mdLen
 * @param mdLen [IN] 消息摘要缓冲区长度，长度根据type参考HiLinkMdByteLen
 * @return 0成功，其他失败
 */
int HILINK_SAL_MdCalc(HiLinkMdType type, const unsigned char *inData, unsigned int inLen,
    unsigned char *md, unsigned int mdLen);

/**
 * @brief 哈希运算消息认证码计算
 *
 * @param param [IN] 计算参数
 * @param hmac [OUT] hmac缓冲区，可写长度至少为hmacLen
 * @param hmacLen [IN] hmac缓冲区长度，长度根据param->md参考HiLinkMdByteLen
 * @return 0成功，其他失败
 */
int HILINK_SAL_HmacCalc(const HiLinkHmacParam *param, unsigned char *hmac, unsigned int hmacLen);

/**
 * @brief 初始化摘要计算上下文，用于持续的哈希计算
 *
 * @param type [IN] 消息摘要算法类型
 * @return NULL表示失败，其他表示成功
 * @attention 返回的上下文计算结束后需要使用HILINK_SAL_MdFree释放
 */
HiLinkMdContext HILINK_SAL_MdInit(HiLinkMdType type);

/**
 * @brief 为持续的摘要计算导入数据
 *
 * @param ctx [IN] 摘要计算上下文
 * @param inData [IN] 输入数据，可读长度至少为inLen
 * @param inLen [IN] 输入数据长度
 * @return 0成功，其他失败
 */
int HILINK_SAL_MdUpdate(HiLinkMdContext ctx, const unsigned char *inData, unsigned int inLen);

/**
 * @brief 结束持续的摘要计算，并输出结果
 *
 * @param ctx [IN] 摘要计算上下文
 * @param outData [OUT] 输出数据缓冲区，有效长度至少为outLen
 * @param outLen [IN] 输出数据缓冲区长度，根据摘要算法类型有最小长度要求，参考HiLinkMdByteLen
 * @return 0成功，其他失败
 */
int HILINK_SAL_MdFinish(HiLinkMdContext ctx, unsigned char *outData, unsigned int outLen);

/**
 * @brief 释放摘要计算上下文
 *
 * @param ctx [IN] 摘要计算上下文
 */
void HILINK_SAL_MdFree(HiLinkMdContext ctx);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_SAL_MD_H */