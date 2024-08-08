/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: AES加解密适配层接口mbedTLS实现（此文件为DEMO，需集成方适配修改）
 */
#include "hilink_sal_aes.h"
#include <stdbool.h>
#include "securec.h"
#include "mbedtls/gcm.h"
#include "mbedtls/aes.h"
#include "osal_debug.h"

#if defined(HILINK_MBEDTLS_CCM_SUPPORT) || !defined(HILINK_SDK_BUILD_IN)
#include "mbedtls/cipher.h"
#include "mbedtls/ccm.h"
#endif

#define BITS_PER_BYTES 8

static bool IsAesGcmParamValid(const HiLinkAesGcmParam *param)
{
    if (param == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return false;
    }

    if ((param->iv == NULL) || (param->ivLen == 0)) {
        HILINK_SAL_WARN("invalid iv\r\n");
        return false;
    }

    if ((param->key == NULL) ||
        ((param->keyLen != HILINK_AES_128_KEY_BYTE_LEN) &&
        (param->keyLen != HILINK_AES_192_KEY_BYTE_LEN) &&
        (param->keyLen != HILINK_AES_256_KEY_BYTE_LEN))) {
        HILINK_SAL_WARN("invalid key\r\n");
        return false;
    }

    if ((param->data == NULL) || (param->dataLen == 0)) {
        HILINK_SAL_WARN("invalid data\r\n");
        return false;
    }

    if ((param->addLen != 0) && (param->add == NULL)) {
        HILINK_SAL_WARN("invalid add\r\n");
        return false;
    }

    return true;
}
static void hilink_dump_gcm(mbedtls_gcm_context *ctx, mbedtls_cipher_id_t id, const unsigned char *key,
    unsigned int keybits)
{
    int i;
    osal_printk("\r\nhilink mbedtls_gcm_context:\r\n");
    char *tmp = (char *)ctx;
    for (i = 0; i < sizeof(mbedtls_gcm_context); i++) {
        osal_printk("%02x ", tmp[i]);
    }
    osal_printk("\r\nid: %d\r\n", id);
    osal_printk("\r\nkey:\r\n");
    for (i = 0; i < keybits / BITS_PER_BYTES; i++) {
        osal_printk("%02x ", key[i]);
    }
    osal_printk("\r\nkeylen: %d\r\n", keybits / BITS_PER_BYTES);
}
int HILINK_SAL_AesGcmEncrypt(const HiLinkAesGcmParam *param, unsigned char *tag,
    unsigned int tagLen, unsigned char *buf)
{
    if (!IsAesGcmParamValid(param)) {
        return HILINK_SAL_PARAM_INVALID;
    }

    mbedtls_gcm_context context;
    mbedtls_gcm_init(&context);
    int ret;
    do {
        hilink_dump_gcm(&context, MBEDTLS_CIPHER_ID_AES,
            param->key, param->keyLen * BITS_PER_BYTES);
        ret = mbedtls_gcm_setkey(&context, MBEDTLS_CIPHER_ID_AES,
            param->key, param->keyLen * BITS_PER_BYTES);
        if (ret != 0) {
            HILINK_SAL_ERROR("set key err %d\r\n", ret);
            break;
        }

        ret = mbedtls_gcm_crypt_and_tag(&context, MBEDTLS_GCM_ENCRYPT, param->dataLen,
            param->iv, param->ivLen, param->add, param->addLen, param->data,
            buf, tagLen, tag);
        if (ret != 0) {
            HILINK_SAL_ERROR("encrypt err %d\r\n", ret);
            break;
        }
    } while (false);

    mbedtls_gcm_free(&context);
    return ret;
}

int HILINK_SAL_AesGcmDecrypt(const HiLinkAesGcmParam *param, const unsigned char *tag,
    unsigned int tagLen, unsigned char *buf)
{
    if (!IsAesGcmParamValid(param)) {
        return HILINK_SAL_PARAM_INVALID;
    }

    mbedtls_gcm_context context;
    mbedtls_gcm_init(&context);
    int ret;
    do {
        hilink_dump_gcm(&context, MBEDTLS_CIPHER_ID_AES,
            param->key, param->keyLen * BITS_PER_BYTES);
        ret = mbedtls_gcm_setkey(&context, MBEDTLS_CIPHER_ID_AES,
            param->key, param->keyLen * BITS_PER_BYTES);
        if (ret != 0) {
            HILINK_SAL_ERROR("set key err %d\r\n", ret);
            break;
        }

        ret = mbedtls_gcm_auth_decrypt(&context, param->dataLen, param->iv, param->ivLen,
            param->add, param->addLen, tag, tagLen,
            param->data, buf);
        if (ret != 0) {
            HILINK_SAL_ERROR("decrypt err %d\r\n", ret);
            break;
        }
    } while (false);

    mbedtls_gcm_free(&context);
    return ret;
}

static mbedtls_cipher_padding_t GetMbedtlsPaddingMode(HiLinkPaddingMode mode)
{
    switch (mode) {
        case HILINK_PADDING_PKCS7:
            return MBEDTLS_PADDING_PKCS7;
        case HILINK_PADDING_ONE_AND_ZEROS:
            return MBEDTLS_PADDING_ONE_AND_ZEROS;
        case HILINK_PADDING_ZEROS_AND_LEN:
            return MBEDTLS_PADDING_ZEROS_AND_LEN;
        case HILINK_PADDING_ZEROS:
            return MBEDTLS_PADDING_ZEROS;
        default:
            HILINK_SAL_WARN("invalid mode\r\n");
            return MBEDTLS_PADDING_NONE;
    }
}

static int MbedtlsPaddingCipherInit(HiLinkPaddingMode mode, mbedtls_cipher_context_t *ctx,
    mbedtls_cipher_info_t *info)
{
    (void)memset_s(info, sizeof(mbedtls_cipher_info_t), 0, sizeof(mbedtls_cipher_info_t));
    mbedtls_cipher_init(ctx);
#ifdef MBEDTLS_VERSION_3
    ctx->MBEDTLS_PRIVATE(cipher_info) = info;
    info->MBEDTLS_PRIVATE(mode) = MBEDTLS_MODE_CBC;
#else
    ctx->cipher_info = info;
    info->mode = MBEDTLS_MODE_CBC;
#endif

    int ret = mbedtls_cipher_set_padding_mode(ctx, GetMbedtlsPaddingMode(mode));
    if (ret != 0) {
        HILINK_SAL_ERROR("set padding mode failed %d\r\n", ret);
        return ret;
    }
    return HILINK_SAL_OK;
}

int HILINK_SAL_AddPadding(HiLinkPaddingMode mode, unsigned char *out, unsigned int outLen, unsigned int dataLen)
{
    if ((out == NULL) || (outLen == 0) || (dataLen == 0) || (outLen < dataLen)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    mbedtls_cipher_context_t cipherContext;
    mbedtls_cipher_info_t cipherInfo;
    int ret = MbedtlsPaddingCipherInit(mode, &cipherContext, &cipherInfo);
    if (ret != 0) {
        return ret;
    }

#ifdef MBEDTLS_VERSION_3
    if (cipherContext.MBEDTLS_PRIVATE(add_padding) == NULL) {
#else
    if (cipherContext.add_padding == NULL) {
#endif
        HILINK_SAL_ERROR("add padding func err %d\r\n", mode);
        return HILINK_SAL_NOT_SUPPORT;
    }
#ifdef MBEDTLS_VERSION_3
    cipherContext.MBEDTLS_PRIVATE(add_padding)(out, outLen, dataLen);
#else
    cipherContext.add_padding(out, outLen, dataLen);
#endif
    return HILINK_SAL_OK;
}

int HILINK_SAL_GetPadding(HiLinkPaddingMode mode, const unsigned char *input, unsigned int inputLen,
    unsigned int *dataLen)
{
    if ((input == NULL) || (inputLen == 0) || (dataLen == NULL)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    mbedtls_cipher_context_t cipherContext;
    mbedtls_cipher_info_t cipherInfo;
    int ret = MbedtlsPaddingCipherInit(mode, &cipherContext, &cipherInfo);
    if (ret != 0) {
        return ret;
    }

#ifdef MBEDTLS_VERSION_3
    if (cipherContext.MBEDTLS_PRIVATE(get_padding) == NULL) {
#else
    if (cipherContext.get_padding == NULL) {
#endif
        HILINK_SAL_ERROR("get padding func err %d\r\n", mode);
        return HILINK_SAL_NOT_SUPPORT;
    }

    size_t outLen = *dataLen;
#ifdef MBEDTLS_VERSION_3
    ret = cipherContext.MBEDTLS_PRIVATE(get_padding)((unsigned char *)input, inputLen, &outLen);
#else
    ret = cipherContext.get_padding((unsigned char *)input, inputLen, &outLen);
#endif
    if (ret != 0) {
        HILINK_SAL_ERROR("get padding err %d\r\n", ret);
        return ret;
    }
    *dataLen = outLen;
    return HILINK_SAL_OK;
}

static bool IsAesCbcParamValid(const HiLinkAesCbcParam *param)
{
    if (param == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return false;
    }

    if (param->iv == NULL) {
        HILINK_SAL_WARN("invalid iv\r\n");
        return false;
    }

    if ((param->key == NULL) ||
        ((param->keyLen != HILINK_AES_128_KEY_BYTE_LEN) &&
        (param->keyLen != HILINK_AES_192_KEY_BYTE_LEN) &&
        (param->keyLen != HILINK_AES_256_KEY_BYTE_LEN))) {
        HILINK_SAL_WARN("invalid key\r\n");
        return false;
    }

    if ((param->data == NULL) || (param->dataLen == 0) ||
        /* cbc加解密要求数据长度为16的倍数 */
        (param->dataLen % 16 != 0)) {
        HILINK_SAL_WARN("invalid data\r\n");
        return false;
    }

    return true;
}

static int AesCbcCrypt(const HiLinkAesCbcParam *param, unsigned char *buf, int mode)
{
    mbedtls_aes_context context;
    mbedtls_aes_init(&context);

    int ret;
    if (mode == MBEDTLS_AES_DECRYPT) {
        ret = mbedtls_aes_setkey_dec(&context, param->key, param->keyLen * BITS_PER_BYTES);
    } else {
        ret = mbedtls_aes_setkey_enc(&context, param->key, param->keyLen * BITS_PER_BYTES);
    }
    if (ret != 0) {
        HILINK_SAL_ERROR("set key err %d\r\n", ret);
        return ret;
    }

    /* mbedtls_aes_crypt_cbc接口调用后会修改iv值，所以使用局部变量而非原始iv值 */
    unsigned char iv[HILINK_AES_CBC_IV_LEN] = {0};
    if (memcpy_s(iv, sizeof(iv), param->iv, HILINK_AES_CBC_IV_LEN) != EOK) {
        HILINK_SAL_ERROR("memcpy err\r\n");
        return HILINK_SAL_MEMCPY_ERR;
    }

    ret = mbedtls_aes_crypt_cbc(&context, mode, param->dataLen, iv, param->data, buf);
    (void)memset_s(iv, sizeof(iv), 0, sizeof(iv));
    if (ret != 0) {
        HILINK_SAL_ERROR("crypt err %d mode %d\r\n", ret, mode);
    }
#if defined(MBEDTLS_HARDEN_OPEN)
    mbedtls_aes_free(&context);
#endif
    return ret;
}

int HILINK_SAL_AesCbcEncrypt(const HiLinkAesCbcParam *param, unsigned char *buf)
{
    if (!IsAesCbcParamValid(param)) {
        return HILINK_SAL_PARAM_INVALID;
    }
    return AesCbcCrypt(param, buf, MBEDTLS_AES_ENCRYPT);
}

int HILINK_SAL_AesCbcDecrypt(const HiLinkAesCbcParam *param, unsigned char *buf)
{
    if (!IsAesCbcParamValid(param)) {
        return HILINK_SAL_PARAM_INVALID;
    }
    return AesCbcCrypt(param, buf, MBEDTLS_AES_DECRYPT);
}

#if defined(HILINK_MBEDTLS_CCM_SUPPORT) || !defined(HILINK_SDK_BUILD_IN)
static bool IsAesCcmParamValid(const HiLinkAesCcmParam *param)
{
    if (param == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return false;
    }

    if ((param->iv == NULL) || (param->ivLen == 0)) {
        HILINK_SAL_WARN("invalid iv\r\n");
        return false;
    }

    if ((param->key == NULL) ||
        ((param->keyLen != HILINK_AES_128_KEY_BYTE_LEN) &&
        (param->keyLen != HILINK_AES_192_KEY_BYTE_LEN) &&
        (param->keyLen != HILINK_AES_256_KEY_BYTE_LEN))) {
        HILINK_SAL_WARN("invalid key\r\n");
        return false;
    }

    if ((param->data == NULL) || (param->dataLen == 0)) {
        HILINK_SAL_WARN("invalid data\r\n");
        return false;
    }

    if ((param->addLen != 0) && (param->add == NULL)) {
        HILINK_SAL_WARN("invalid add\r\n");
        return false;
    }

    return true;
}

int HILINK_SAL_AesCcmDecrypt(const HiLinkAesCcmParam *param, const unsigned char *tag,
    unsigned int tagLen, unsigned char *buf)
{
    if (!IsAesCcmParamValid(param)) {
        return HILINK_SAL_PARAM_INVALID;
    }

    mbedtls_ccm_context context;
    mbedtls_ccm_init(&context);

    int ret;
    do {
        ret = mbedtls_ccm_setkey(&context, MBEDTLS_CIPHER_ID_AES,
            param->key, param->keyLen * BITS_PER_BYTES);
        if (ret != 0) {
            HILINK_SAL_ERROR("set key err %d\r\n", ret);
            break;
        }

        ret = mbedtls_ccm_auth_decrypt(&context, param->dataLen, param->iv, param->ivLen,
            param->add, param->addLen, param->data, buf,
            tag, tagLen);
        if (ret != 0) {
            HILINK_SAL_ERROR("decrypt err %d\r\n", ret);
            break;
        }
    } while (0);

    mbedtls_ccm_free(&context);
    HILINK_SAL_INFO("Decrypt outdata success\r\n");
    return ret;
}
int HILINK_SAL_AesCcmEncrypt(const HiLinkAesCcmParam *param, unsigned char *tag, unsigned int tagLen,
    unsigned char *buf)
{
    if (!IsAesCcmParamValid(param)) {
        return HILINK_SAL_PARAM_INVALID;
    }

    mbedtls_ccm_context context;
    mbedtls_ccm_init(&context);
    int ret;
    do {
        ret = mbedtls_ccm_setkey(&context, MBEDTLS_CIPHER_ID_AES,
            param->key, param->keyLen * BITS_PER_BYTES);
        if (ret != 0) {
            HILINK_SAL_ERROR("set key err %d\r\n", ret);
            break;
        }

        ret = mbedtls_ccm_encrypt_and_tag(&context, param->dataLen,
            param->iv, param->ivLen,
            param->add, param->addLen,
            param->data, buf,
            tag, tagLen);
        if (ret != 0) {
            HILINK_SAL_ERROR("encrypt err %d\r\n", ret);
            break;
        }
    } while (0);

    mbedtls_ccm_free(&context);
    return ret;
}
#endif