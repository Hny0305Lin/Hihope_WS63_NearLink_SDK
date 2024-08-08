/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides hash driver header file\n
 *
 * History: \n
 * 2022-08-09, Create file. \n
 */
#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup drivers_driver_hash Hash
 * @ingroup  drivers_driver
 * @{
 */

#define SHA256_HASH_SIZE      32
#if SHA512_SUPPORT == YES
#define SHA512_HASH_SIZE      64
#endif

/**
 * @if Eng
 * @brief Hash algorithm type
 * @note  SHA1 is not secure, and we advise not to use it.
 * @else
 * @brief hash算法类型
 * @note  SHA1不安全，不建议使用
 * @endif
 */
typedef enum {
    DRV_CIPHER_HASH_TYPE_SHA1 = 0x00,
    DRV_CIPHER_HASH_TYPE_SHA224,
    DRV_CIPHER_HASH_TYPE_SHA256,
    DRV_CIPHER_HASH_TYPE_SHA384,
    DRV_CIPHER_HASH_TYPE_SHA512,
    DRV_CIPHER_HASH_TYPE_SM3 = 0x10,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA1 = 0x20,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA224,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA256,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA384,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA512,
    DRV_CIPHER_HASH_TYPE_HMAC_SM3 = 0x30,
    DRV_CIPHER_HASH_TYPE_MAX,
    DRV_CIPHER_HASH_TYPE_INVALID = 0xffffffff,
} drv_cipher_hash_type_t;

/**
 * @if Eng
 * @brief  Defines the security type of the buffer.
 * @else
 * @brief  定义缓冲区的安全类型
 * @endif
 */
typedef enum {
    DRV_CIPHER_BUF_NONSECURE = 0x00,
    DRV_CIPHER_BUF_SECURE,
    DRV_CIPHER_BUF_UNDEFINED,
    DRV_CIPHER_BUF_INVALID = 0xffffffff,
} drv_cipher_buffer_secure_t;

/**
 * @if Eng
 * @brief  Attributes of the buffer that stores encrypted and decrypted data.
 * @else
 * @brief  存储加解密数据的缓冲区属性
 * @endif
 */
typedef struct {
    uint8_t *address;                           /*!< @if Eng Buffer address, which should be 4-byte aligned
                                                     @else   缓冲区地址，应该4字节对齐 @endif */
    drv_cipher_buffer_secure_t buf_sec;         /*!< @if Eng Buffer security attribute.
                                                     @else   缓冲区安全属性。 @endif */
} drv_cipher_buf_attr_t;

/**
 * @if Eng
 * @brief  Definition of hash handle attribute
 * @else
 * @brief  hash 句柄属性
 * @endif
 */
typedef struct {
    drv_cipher_hash_type_t hash_type;           /*!< @if Eng Hash algorithm type
                                                     @else   hash 算法类型 @endif */
    uint32_t keyslot;                           /*!< @if Eng This attribute is verified during the keylot handle
                                                             HMAC calculation.
                                                     @else   keyslot句柄,hmac计算时会验证该属性 @endif */
} drv_cipher_hash_attr_t;

/**
 * @if Eng
 * @brief  Creates a hash handle and binds the keyslot channel.
 * @param  [out] hhash Handle for operating the hash algorithm.
 * @param  [in]  hash_attr Configures the basic attributes of the hash handle, including the hash type and the
           bound keylot channel.
 * @param  [in]  check_word The XOR result of the preceding parameter.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  创建hash句柄，并绑定keyslot通道
 * @param  [out] hhash hash操作句柄
 * @param  [in]  hash_attr 配置hash句柄基本属性，包括hash类型和绑定的keyslot通道.
 * @param  [in]  check_word 前面参数的异或结果.
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_drv_cipher_hash_init(uintptr_t *hhash, const drv_cipher_hash_attr_t *hash_attr,
    const uintptr_t check_word);

/**
 * @if Eng
 * @brief  Hash calculation.
 * @note   The hash handle must have been created before this interface is invoked. If the uapi_drv_cipher_hash_final
           interface has been invoked to obtain the digest information, the calculation cannot be performed again.
           This interface supports retry. That is, if a process calculation fails, this interface can be invoked to
           recalculate the process until the calculation succeeds without restarting the calculation process.
 * @param  [out] hhash Handle for operating the hash algorithm.
 * @param  [in] src_buf Source buffer attributes, including the buffer address and buffer security type.
 * @param  [in] len Buffer size.
 * @param  [in] timeout_ms Timeout in milliseconds.
 * @param  [in] check_word The XOR result of the preceding parameter.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HASH计算
 * @note   调用该接口前必须已经创建了hash句柄,如已经调用了uapi_drv_cipher_hash_final接口获取摘要信息，则不能再次进行该计算
           该接口支持重试，即若某次过程计算失败，仍可调用该接口重新计算直至成功，而不需要重新开始计算流程
 * @param  [out] hhash hash操作句柄
 * @param  [in] src_buf 源缓冲区属性，包括缓冲区地址与缓冲区安全类型.
 * @param  [in] len 缓冲区大小。
 * @param  [in] timeout_ms 毫秒记时的超时时间。
 * @param  [in] check_word 前面参数的异或结果。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_drv_cipher_hash_update(uintptr_t hhash, const drv_cipher_buf_attr_t *src_buf, const uint32_t len,
    const uint32_t timeout_ms, const uintptr_t check_word);

/**
 * @if Eng
 * @brief  The hash calculation obtains the digest information and destroys the hash handle when the calculation
           succeeds.
 * @note   The hash handle must have been created.
           This interface supports retry. That is, if a process calculation fails, this interface can be invoked to
           recalculate, the process until the calculation succeeds without restarting the calculation process.
 * @param  [out] hhash Handle for operating the hash algorithm.
 * @param  [out] out Pointer to the address of the buffer for storing summary information.
 * @param  [out] out_len Pointer to the size of the buffer for storing summary information.
 * @param  [in] check_word The XOR result of the preceding parameter.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief   HASH计算获取摘要信息，并在计算成功的时候销毁hash句柄。
 * @note   必须已经创建了hash句柄。该接口支持重试，即若某次过程计算失败，仍可调用该接口重新计算直至成功，而不需要重新开始计算流程
 * @param  [out] hhash hash句柄.
 * @param  [out] out 存储摘要信息的缓冲区地址指针。
 * @param  [out] out_len 存储摘要信息的缓冲区大小指针。
 * @param  [in] check_word 前面参数的异或结果。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_drv_cipher_hash_final(uintptr_t hhash, uint8_t *out, uint32_t *out_len, const uintptr_t check_word);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
