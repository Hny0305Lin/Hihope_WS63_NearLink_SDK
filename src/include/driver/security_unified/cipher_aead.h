/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides cipher aead driver header \n
 *
 * History: \n
 * 2023-03-14, Create file. \n
 */
#ifndef CIPHER_AEAD_H
#define CIPHER_AEAD_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_cipher_aead Cipher AEAD
 * @ingroup  drivers_driver_security_unified
 * @{
 */

/**
 * @if Eng
 * @brief  gcm encrypt and compute tag.
 * @param  [in]    key Encryption key.
 * @param  [in]    key_len Encryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the encryption key.
 * @param  [in]    iv The initial vector.
 * @param  [in]    iv_len The initial vector lenght in byte.
 * @param  [in]    aad The additional authentication message. NULL is support.
 * @param  [in]    aad_len The additional authentication message length in byte.
 * @param  [in]    plain_text Input plain text.
 * @param  [out]   cipher_text Output cipher text.
 * @param  [in]    data_len data length in byte.
 * @param  [out]   tag Output tag.
 * @param  [in]    tag_len The expected tag length in byte.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two methods to set the key:
            1）Using key/key_len, in which case, the keyslot_handle is set to 0.
            2) Calling keyslot api to create one keyslot_handle and set it, in which case,
                the key is set to NULL and key_len is 0.
            When keyslot is supported, you must use method 2, otherwise you should use method 1.
 * @else
 * @brief  GCM 加密并计算 tag.
 * @param  [in]    key 加密密钥.
 * @param  [in]    key_len 加密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存加密密钥的 keyslot 句柄.
 * @param  [in]    iv 初始向量
 * @param  [in]    iv_len 初始向量长度，单位是 Byte.
 * @param  [in]    aad 附加认证信息，支持传入为 NULL
 * @param  [in]    aad_len 附加认证信息，单位是 Byte.
 * @param  [in]    plain_text 输入的明文.
 * @param  [out]   cipher_text 输出的密文.
 * @param  [in]    data_len 数据长度，单位是 Byte.
 * @param  [out]   tag 输出的 tag
 * @param  [in]    tag_len 期望的 tag 长度，单位是 Byte.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 0.
            2）使用 keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL，key_len 传入为 0.
            若支持 keyslot，则必须使用方法2，否则只能使用方法1.
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_encrypt(
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len
);

/**
 * @if Eng
 * @brief  gcm decrypt and verify tag.
 * @param  [in]    key Decryption key.
 * @param  [in]    key_len Decryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the decryption key.
 * @param  [in]    iv The initial vector.
 * @param  [in]    iv_len The initial vector lenght in byte.
 * @param  [in]    aad The additional authentication message. NULL is support.
 * @param  [in]    aad_len The additional authentication message length in byte.
 * @param  [in]    cipher_text Input cipher text.
 * @param  [out]   plain_text Output plain text.
 * @param  [in]    data_len data length in byte.
 * @param  [in]    tag Input tag to be verified.
 * @param  [in]    tag_len The tag length in byte.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two methods to set the key:
            1）Using key/key_len, in which case, the keyslot_handle is set to 0.
            2) Calling keyslot api to create one keyslot_handle and set it, in which case,
                the key is set to NULL and key_len is 0.
            When keyslot is supported, you must use method 2, otherwise you should use method 1.
 * @else
 * @brief  GCM 解密并校验 tag.
 * @param  [in]    key 解密密钥.
 * @param  [in]    key_len 解密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存解密密钥的 keyslot 句柄.
 * @param  [in]    iv 初始向量
 * @param  [in]    iv_len 初始向量长度，单位是 Byte.
 * @param  [in]    aad 附加认证信息，支持传入为 NULL
 * @param  [in]    aad_len 附加认证信息，单位是 Byte.
 * @param  [in]    cipher_text 输入的密文.
 * @param  [out]   plain_text 输出的明文.
 * @param  [in]    data_len 数据长度，单位是 Byte.
 * @param  [out]   tag 待校验的 tag.
 * @param  [in]    tag_len tag 长度，单位是 Byte.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 0.
            2）使用 keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL，key_len 传入为 0.
            若支持 keyslot，则必须使用方法2，否则只能使用方法1.
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_decrypt_verify(
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len
);

/**
 * @if Eng
 * @brief  GCM create channel handle.
 * @param  [out]   handle Pointer to the created channel handle.
 * @param  [in]    key Encryption/Decryption key.
 * @param  [in]    key_len Encryption/Decryption key length in byte. Only support 16,24,32.
 * @param  [in]    keyslot_handle The keyslot handle which store the encryption/decryption key.
 * @param  [in]    iv The initial vector.
 * @param  [in]    iv_len The initial vector lenght in byte.
 * @param  [in]    aad The additional authentication message. NULL is support.
 * @param  [in]    aad_len The additional authentication message length in byte.
 * @param  [in]    tag_len The expected tag length in byte.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   There are two methods to set the key:
            1）Using key/key_len, in which case, the keyslot_handle is set to 0.
            2) Calling keyslot api to create one keyslot_handle and set it, in which case,
                the key is set to NULL and key_len is 0.
            When keyslot is supported, you must use method 2, otherwise you should use method 1.
 * @else
 * @brief  GCM 创建通道句柄.
 * @param  [out]   handle 指向创建的通道句柄的指针.
 * @param  [in]    key 加解密密钥.
 * @param  [in]    key_len 加解密密钥长度，单位是 Byte. 只支持 16,24,32.
 * @param  [in]    keyslot_handle 保存加解密密钥的 keyslot 句柄.
 * @param  [in]    iv 初始向量
 * @param  [in]    iv_len 初始向量长度，单位是 Byte.
 * @param  [in]    aad 附加认证信息，支持传入为 NULL
 * @param  [in]    aad_len 附加认证信息，单位是 Byte.
 * @param  [in]    tag_len 期望的 tag 长度，单位是 Byte.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   有两种方法设置密钥:
            1）使用 key/key_len 组合，在这种情况下，keyslot_handle 传入 0.
            2）使用 keyslot 接口创建 keyslot 句柄传入，在这种情况下，key 传入为 NULL，key_len 传入为 0.
            若支持 keyslot，则必须使用方法2，否则只能使用方法1.
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_create(uint32_t *handle,
    uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    uint32_t tag_len
);

/**
 * @if Eng
 * @brief  GCM destroy channel handle.
 * @param  [out]   handle Channel handle to be destroyed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  GCM 销毁通道句柄.
 * @param  [in]    handle 待销毁的通道句柄.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_destroy(uint32_t handle);

/**
 * @if Eng
 * @brief  GCM encrypt.
 * @param  [in]    handle The created channel handle.
 * @param  [in]    plain_text Input plain text.
 * @param  [out]   cipher_text Output cipher text.
 * @param  [in]    data_len data length in byte.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   User can call this function more than one times to process data in fragments.
 * @else
 * @brief  GCM 加密.
 * @param  [in]    handle 已创建的通道句柄.
 * @param  [in]    plain_text 输入的明文.
 * @param  [out]   cipher_text 输出的密文.
 * @param  [in]    data_len 数据长度，单位是 Byte.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   用户可以调用该函数多次来处理分段的数据.
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_encrypt_update(uint32_t handle,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len
);

/**
 * @if Eng
 * @brief  GCM get tag after encryption.
 * @param  [in]    handle The created channel handle.
 * @param  [out]   tag Output tag.
 * @param  [in]    tag_len The expected tag length in byte. The value must be the same as that of the
                    uapi_drv_cipher_symc_gcm_create interface.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  GCM 加密完获取 tag.
 * @param  [in]    handle 已创建的通道句柄.
 * @param  [out]   tag 输出的 tag
 * @param  [in]    tag_len 期望的 tag 长度，单位是 Byte. 必须和传入 uapi_drv_cipher_symc_gcm_create 接口的值一致.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_encrypt_get_tag(uint32_t handle,
    uint8_t *tag, uint32_t tag_len
);

/**
 * @if Eng
 * @brief  GCM set tag before verify.
 * @param  [in]    handle The created channel handle.
 * @param  [in]    tag Input tag to be verified.
 * @param  [in]    tag_len The tag length in byte.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  GCM 校验前设置 tag.
 * @param  [in]    handle 已创建的通道句柄.
 * @param  [out]   tag 待校验的 tag.
 * @param  [in]    tag_len tag 长度，单位是 Byte.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_decrypt_set_tag(uint32_t handle,
    const uint8_t *tag, uint32_t tag_len
);

/**
 * @if Eng
 * @brief  GCM decrypt.
 * @param  [in]    handle The created channel handle.
 * @param  [in]    cipher_text Input cipher text.
 * @param  [out]   plain_text Output plain text.
 * @param  [in]    data_len data length in byte.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   User can call this function more than one times to process data in fragments.
 * @else
 * @brief  GCM 解密.
 * @param  [in]    handle 已创建的通道句柄.
 * @param  [in]    cipher_text 输入的密文.
 * @param  [out]   plain_text 输出的明文.
 * @param  [in]    data_len 数据长度，单位是 Byte.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   用户可以调用该函数多次来处理分段的数据.
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_decrypt_update(uint32_t handle,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len
);

/**
 * @if Eng
 * @brief  GCM verify tag after decrypt.
 * @param  [in]    handle The created channel handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @note   Before invoking this interface, you must invoke uapi_drv_cipher_symc_gcm_decrypt_set_tag.
 * @else
 * @brief  GCM 解密完校验 tag.
 * @param  [in]    handle 已创建的通道句柄.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @note   调用该接口前必须先调用 uapi_drv_cipher_symc_gcm_decrypt_set_tag.
 * @endif
 */
errcode_t uapi_drv_cipher_symc_gcm_decrypt_verify_tag(uint32_t handle);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
