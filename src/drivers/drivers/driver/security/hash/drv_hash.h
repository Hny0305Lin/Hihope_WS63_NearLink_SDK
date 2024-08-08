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
 * @brief HASH algorithm type
 * @note  SHA1 is not secure, and we advise not to use it.
 * @else
 * @brief HASH算法类型
 * @note  SHA1不安全，不建议使用
 * @endif
 */
typedef enum {
    DRV_CIPHER_HASH_TYPE_SHA1 = 0xf690a0,
    DRV_CIPHER_HASH_TYPE_SHA224 = 0x10690e0,
    DRV_CIPHER_HASH_TYPE_SHA256 = 0x1169100,
    DRV_CIPHER_HASH_TYPE_SHA384 = 0x127a180,
    DRV_CIPHER_HASH_TYPE_SHA512 = 0x137a200,
    DRV_CIPHER_HASH_TYPE_SM3 = 0x2169100,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA1 = 0x10f690a0,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA224 = 0x110690e0,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA256 = 0x11169100,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA384 = 0x1127a180,
    DRV_CIPHER_HASH_TYPE_HMAC_SHA512 = 0x1137a200,
    DRV_CIPHER_HASH_TYPE_HMAC_SM3 = 0x12169100,
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
 * @brief  Definition of HASH handle attribute
 * @else
 * @brief  HASH 句柄属性
 * @endif
 */
typedef struct {
    drv_cipher_hash_type_t hash_type;           /*!< @if Eng HASH algorithm type
                                                     @else   HASH 算法类型 @endif */
    uint32_t keyslot;                           /*!< @if Eng This attribute is verified during the keylot handle
                                                             HMAC calculation.
                                                     @else   keyslot句柄,hmac计算时会验证该属性 @endif */
} drv_cipher_hash_attr_t;

/**
 * @if Eng
 * @brief  Creates a HASH handle and binds the keyslot channel.
 * @param  [out] hhash Handle for operating the HASH algorithm.
 * @param  [in]  hash_attr Configures the basic attributes of the HASH handle, including the HASH type and the
           bound keylot channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  创建HASH句柄，并绑定keyslot通道
 * @param  [out] hhash HASH操作句柄
 * @param  [in]  hash_attr 配置HASH句柄基本属性，包括HASH类型和绑定的keyslot通道.
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t drv_hash_start(uintptr_t *hhash, const drv_cipher_hash_attr_t *hash_attr);

/**
 * @if Eng
 * @brief  HASH calculation update.
 * @note   The hash handle must have been created before this interface is invoked. If the uapi_drv_cipher_hash_final
           interface has been invoked to obtain the digest information, the calculation cannot be performed again.
           This interface supports retry. That is, if a process calculation fails, this interface can be invoked to
           recalculate the process until the calculation succeeds without restarting the calculation process.
 * @param  [out] hhash Handle for operating the HASH algorithm.
 * @param  [in] src_buf Source buffer attributes, including the buffer address and buffer security type.
 * @param  [in] len Buffer size.
 * @param  [in] timeout_ms Timeout in milliseconds.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HASH计算更新
 * @note   调用该接口前必须已经创建了HASH句柄,如已经调用了uapi_drv_cipher_hash_final接口获取摘要信息，则不能再次进行该计算
           该接口支持重试，即若某次过程计算失败，仍可调用该接口重新计算直至成功，而不需要重新开始计算流程
 * @param  [out] hhash HASH操作句柄
 * @param  [in] src_buf 源缓冲区属性，包括缓冲区地址与缓冲区安全类型.
 * @param  [in] len 缓冲区大小。
 * @param  [in] timeout_ms 毫秒记时的超时时间。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t drv_hash_update(uintptr_t hhash, const drv_cipher_buf_attr_t *src_buf, const uint32_t len,
    const uint32_t timeout_ms);

/**
 * @if Eng
 * @brief  The HASH calculation obtains the digest information and destroys the HASH handle when the calculation
           succeeds.
 * @note   The hash handle must have been created.
           This interface supports retry. That is, if a process calculation fails, this interface can be invoked to
           recalculate, the process until the calculation succeeds without restarting the calculation process.
 * @param  [in] hhash Handle for operating the HASH algorithm.
 * @param  [out] out Pointer to the address of the buffer for storing summary information.
 * @param  [out] out_len Pointer to the size of the buffer for storing summary information.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief   HASH计算获取摘要信息，并在计算成功的时候销毁HASH句柄。
 * @note   必须已经创建了HASH句柄。该接口支持重试，即若某次过程计算失败，仍可调用该接口重新计算直至成功，而不需要重新开始计算流程
 * @param  [in] hhash HASH句柄.
 * @param  [out] out 存储摘要信息的缓冲区地址指针。
 * @param  [out] out_len 存储摘要信息的缓冲区大小指针。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t drv_hash_final(uintptr_t hhash, uint8_t *out, uint32_t *out_len);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
