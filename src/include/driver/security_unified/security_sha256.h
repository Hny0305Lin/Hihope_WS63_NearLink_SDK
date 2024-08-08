/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Provides SHA256 header
 *
 * Create: 2023-12-13
*/

#ifndef SECURITY_SHA256_H
#define SECURITY_SHA256_H

#include <stdint.h>
#include "errcode.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_sha256 SHA256
 * @ingroup  drivers_driver_security_unified
 * @{
 */

#ifndef SHA256_HASH_SIZE
#define SHA256_HASH_SIZE    32
#endif

/**
 * @if Eng
 * @brief  Create a sha256 channel.
 * @param  [out]  hash_handle Pointer to the created sha256 channel handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  创建sha256通道。
 * @param  [out]  hash_handle 指向创建的hash通道句柄的指针。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_sha256_start(uint32_t *hash_handle);

/**
 * @if Eng
 * @brief  Sha256 calculation
 * @note   A sha256 handle must have been created before this API is called.
           If the uapi_drv_cipher_sha256_finish() API has been called to obtain the digest information,
           the calculation cannot be performed again. When calculating the abstract of a piece of data,
           you can invoke this interface at a time.
           You can also split data into multiple parts and invoke this interface for multiple times.
           The results obtained by the two methods are the same.
 * @param  [in]   hash_handle Handle of the created sha256 channel.
 * @param  [in]   buf Source buffer pointer.
 * @param  [in]   len Buffer Size.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  Sha256计算
 * @note   调用该接口前必须已经创建了sha256句柄，如已经调用了uapi_drv_cipher_sha256_finish()接口获取摘要信息，
           则不能再次进行该计算。
           计算一段数据的摘要时，可以单次调用该接口；也可以将数据拆成多段，多次调用该接口。两种方式得到的结果相同。
 * @param  [in]   hash_handle 已创建的hash通道句柄。
 * @param  [in]   buf 源缓冲区指针。
 * @param  [in]   len 缓冲区大小。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_sha256_update(uint32_t hash_handle, const uint8_t *buf, uint32_t len);

/**
 * @if Eng
 * @brief  Sha256 calculation obtains digest information and destroys the handle when the calculation is successful.
 * @note   The sha256 handle must have been created.
 * @param  [in]    hash_handle Handle of the created sha256 channel.
 * @param  [out]   out Pointer to the buffer address for storing the summary information.
 * @param  [inout] out_len Pointer to the size of the buffer for storing the summary information.
                           The input is the buffer length, and the output is the actual digest length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  Sha256计算获取摘要信息，并在计算成功的时候销毁hash句柄。
 * @note   必须已经创建了hash句柄。
 * @param  [in]    hash_handle 已创建的hash通道句柄
 * @param  [out]   out 存储摘要信息的缓冲区地址指针。
 * @param  [inout] out_len 存储摘要信息的缓冲区大小指针。输入为缓冲区长度，输出为实际摘要长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_sha256_finish(uint32_t hash_handle, uint8_t *out, uint32_t *out_len);

/**
 * @if Eng
 * @brief  Sha256 calculation
 * @param  [in]    buf Source buffer pointer.
 * @param  [in]    len Source buffer size.
 * @param  [out]   out Pointer to the buffer address for storing the summary information.
 * @param  [inout] out_len Pointer to the size of the buffer for storing the summary information.
                           The input is the buffer length, and the output is the actual digest length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  Sha256计算
 * @param  [in]    buf 源缓冲区指针。
 * @param  [in]    len 缓冲区大小。
 * @param  [out]   out 存储摘要信息的缓冲区地址指针。
 * @param  [in]    out_len 存储摘要信息的缓冲区大小指针。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_sha256(const uint8_t *buf, uint32_t len, uint8_t *out, uint32_t out_len);


/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
