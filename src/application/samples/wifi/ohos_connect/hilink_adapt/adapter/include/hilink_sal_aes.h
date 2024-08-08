/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: AES加解密适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_SAL_AES_H
#define HILINK_SAL_AES_H

#include "hilink_sal_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** AES-CBC加解密过程中的IV长度 */
#define HILINK_AES_CBC_IV_LEN      16
/** AES-GCM加解密过程中tag最长长度 */
#define HILINK_AES_GCM_TAG_MAX_LEN 16
/** AES-GCM加解密过程中tag最短长度 */
#define HILINK_AES_GCM_TAG_MIN_LEN 4

/** @brief AES加解密密钥长度 */
enum HiLinkAesKeyByteLen {
    HILINK_AES_128_KEY_BYTE_LEN = 16, /**< AES-128 密钥字节数 */
    HILINK_AES_192_KEY_BYTE_LEN = 24, /**< AES-192 密钥字节数 */
    HILINK_AES_256_KEY_BYTE_LEN = 32, /**< AES-256 密钥字节数 */
};

/** @brief AES-GCM加解密所需的数据 */
typedef struct HiLinkAesGcmParam {
    const unsigned char *key;   /**< 加解密密钥，有效可读长度至少为keyLen */
    unsigned int keyLen;        /**< 密钥长度，应该是HiLinkAesCryptKeyByteLen中的数值 */
    const unsigned char *iv;    /**< 初始向量，有效可读长度至少为ivLen */
    unsigned int ivLen;         /**< 初始向量长度 */
    const unsigned char *add;   /**< 附加值，有效可读长度至少为addLen */
    unsigned int addLen;        /**< 附加值长度，可以为0 */
    const unsigned char *data;  /**< 待加解密的数据，有效可读长度至少为dataLen */
    unsigned int dataLen;       /**< 数据长度 */
} HiLinkAesGcmParam;

/** @brief 填充模式 */
typedef enum HiLinkPaddingMode {
    HILINK_PADDING_PKCS7 = 0,                   /**< PKCS7填充模式，使用填充长度进行填充 */
    HILINK_PADDING_ONE_AND_ZEROS,               /**< 使用0x80 0x00 ... 0x00进行填充 */
    HILINK_PADDING_ZEROS_AND_LEN,               /**< 使用0x00 ... 0x00 0xll进行填充，其中0xll为填充长度 */
    HILINK_PADDING_ZEROS,                       /**< 使用0x00进行填充 */
    HILINK_PADDING_RESERVED = ENUM_INT_REVERSED /**< 避免编译器优化 */
} HiLinkPaddingMode;

/** @brief AES-CBC加解密所需数据 */
typedef struct HiLinkAesCbcParam {
    const unsigned char *key;   /**< 加解密密钥，有效可读长度至少为keyLen */
    unsigned int keyLen;        /**< 密钥长度，应该是HiLinkAesCryptKeyByteLen中的数值 */
    const unsigned char *iv;    /**< 初始向量，有效可读长度至少为16 */
    const unsigned char *data;  /**< 待加解密的数据，有效可读长度至少为dataLen */
    unsigned int dataLen;       /**< 数据长度，应为16的倍数 */
} HiLinkAesCbcParam;

/** @brief AES-CCM加解密所需的数据 */
typedef struct HiLinkAesCcmParam {
    const unsigned char *key;   /**< 加解密密钥，长度为keyLen */
    unsigned int keyLen;        /**< 密钥长度 */
    const unsigned char *iv;    /**< 加解密向量，长度为ivLen */
    unsigned int ivLen;         /**< 加解密向量长度 */
    const unsigned char *add;         /**< 附加值，长度为addLen */
    unsigned int addLen;        /**< 附加值长度，为0 */
    const unsigned char *data;  /**< 待解密的数据，长度为dataLen */
    unsigned int dataLen;       /**< 待解密数据长度 */
} HiLinkAesCcmParam;

/**
 * @brief AES-GCM加密
 *
 * @param param [IN] AES-GCM加密必要的参数
 * @param tag [OUT] 消息验证码输出缓冲区，可写长度至少为tagLen
 * @param tagLen [IN] 消息验证码缓冲区长度，范围为[4,16]
 * @param buf [OUT] 加密数据输出缓冲区，可写长度至少为param->dataLen
 * @return 0成功，非0失败
 */
int HILINK_SAL_AesGcmEncrypt(const HiLinkAesGcmParam *param, unsigned char *tag, unsigned int tagLen,
    unsigned char *buf);

/**
 * @brief AES-GCM解密并校验消息验证码
 *
 * @param param [IN] AES-GCM解密必要的参数
 * @param tag [OUT] 消息验证码，可读长度至少为tagLen
 * @param tagLen [IN] 消息验证码长度，范围应为[4,16]
 * @param buf [OUT] 解密密数据输出缓冲区，可写长度至少为param->dataLen
 * @return 0成功，非0失败
 */
int HILINK_SAL_AesGcmDecrypt(const HiLinkAesGcmParam *param, const unsigned char *tag, unsigned int tagLen,
    unsigned char *buf);

/**
 * @brief AES-CBC加密
 *
 * @param param [IN] AES-CBC加密必要的参数
 * @param buf [OUT] 加密数据输出缓冲区，可写长度至少为param->dataLen
 * @return 0成功，非0失败
 */
int HILINK_SAL_AesCbcEncrypt(const HiLinkAesCbcParam *param, unsigned char *buf);

/**
 * @brief AES-CBC解密
 *
 * @param param [IN] AES-CBC解密必要的参数
 * @param buf [OUT] 解密数据输出缓冲区，可写长度至少为param->dataLen
 * @return 0成功，非0失败
 */
int HILINK_SAL_AesCbcDecrypt(const HiLinkAesCbcParam *param, unsigned char *buf);

/**
 * @brief 填充数据
 *
 * @param mode [IN] 填充模式
 * @param out [IN,OUT] 待填充的数据缓冲区，可读可写长度至少为outLen
 * @param outLen [IN] 填充后的长度
 * @param dataLen [IN] 缓冲区中有效数据的长度
 * @return 0成功，其他失败
 */
int HILINK_SAL_AddPadding(HiLinkPaddingMode mode, unsigned char *out, unsigned int outLen, unsigned int dataLen);

/**
 * @brief 从填充数据获取有效数据长度
 *
 * @param mode [IN] 填充模式
 * @param input [IN] 填充的数据，可读长度至少为inputLen
 * @param inputLen [IN] 填充的数据的长度
 * @param dataLen [OUT] 填充的数据中有效数据的长度
 * @return 0成功，其他失败
 */
int HILINK_SAL_GetPadding(HiLinkPaddingMode mode, const unsigned char *input, unsigned int inputLen,
    unsigned int *dataLen);

int HILINK_SAL_AesCcmDecrypt(const HiLinkAesCcmParam *param, const unsigned char *tag,
    unsigned int tagLen, unsigned char *buf);

/**
 * @brief AES-CCM加密
 *
 * @param param [IN] AES-CCM加密必要的参数
 * @param tag [OUT] 消息验证码输入缓冲区，可写长度至少为tagLen
 * @param tagLen [IN] 消息验证码缓冲区长度
 * @param buf [OUT] 加密数据输出缓冲区，可写长度至少为param->dataLen
 * @return 0成功，非0失败
 */
int HILINK_SAL_AesCcmEncrypt(const HiLinkAesCcmParam *param, unsigned char *tag,
    unsigned int tagLen, unsigned char *buf);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_SAL_AES_H */