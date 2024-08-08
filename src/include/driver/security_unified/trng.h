/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides trng driver header \n
 *
 * History: \n
 * 2023-03-14, Create file. \n
 */
#ifndef TRNG_H
#define TRNG_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_trng Trng
 * @ingroup  drivers_driver_security_unified
 * @{
 */

/**
 * @if Eng
 * @brief  obtaining hardware random numbers.
 * @note   random number of the size of the uint32_t type,generate random numbers of other sizes, which need to be
           called cyclically.
 * @param  [in]  randnum Pointer to the buffer where the generated random number is stored
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取硬件随机数。
 * @note   生成uint32_t类型大小的随机数,生成其他大小的随机数,需要循环调用该接口。
 * @param  [in]  randnum 指向存储生成的随机数的缓冲区的指针。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_trng_get_random(uint32_t *randnum);

/**
 * @if Eng
 * @brief  obtains a hardware random number of a specified size.
 * @param  [in]  randnum Pointer to the buffer where the generated random number is stored.
 * @param  [in]  size Size of the generated random number.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取指定大小的硬件随机数。
 * @param  [in]  randnum 指向存储生成的随机数的缓冲区的指针。
 * @param  [in]  size 生成随机数的大小。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_trng_get_random_bytes(uint8_t *randnum, uint32_t size);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
