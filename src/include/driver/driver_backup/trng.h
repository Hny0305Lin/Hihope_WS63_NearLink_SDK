/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides trng driver header \n
 *
 * History: \n
 * 2022-08-09, Create file. \n
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
 * @defgroup drivers_driver_trng Trng
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  trng rand buffer updata type
 * @else
 * @brief  随机数缓冲区更新类型
 * @endif
 */
typedef enum {
    TRNG_RAND_BUF_UPDATA_ALL,
    TRNG_RAND_BUF_UPDATA_UP_SIDE,
    TRNG_RAND_BUF_UPDATA_DOWN_SIDE,
} trng_rand_buf_updata_t;

/**
 * @if Eng
 * @brief  obtaining hardware random numbers.
 * @note   random number of the size of the uint32_t type,generate random numbers of other sizes, which need to be
           called cyclically.
 * @param  [in]  randnum Pointer to the buffer where the generated random number is stored
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取硬件随机数
 * @note   生成uint32_t类型大小的随机数,生成其他大小的随机数,需要循环调用该接口
 * @param  [in]  randnum 指向存储生成的随机数的缓冲区的指针
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_drv_cipher_trng_get_random(uint32_t *randnum);

/**
 * @if Eng
 * @brief  obtains a hardware random number of a specified size.
 * @param  [in]  randnum Pointer to the buffer where the generated random number is stored
 * @param  [in]  size size of the generated random number
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取指定大小的硬件随机数
 * @param  [in]  randnum 指向存储生成的随机数的缓冲区的指针
 * @param  [in]  size 生成随机数的大小
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_drv_cipher_trng_get_random_bytes(uint8_t *randnum, uint32_t size);

/**
 * @if Eng
 * @brief  updata the random fuffer in multi-core scenarios
 * @param  [in]  updata random number buffer update type
 * @else
 * @brief  多核场景下随机数缓冲区更新
 * @param  [in]  updata 随机数缓冲区更新类型
 * @endif
 */
void uapi_drv_cipher_trng_random_buffer_init(trng_rand_buf_updata_t updata);

/**
 * @if Eng
 * @brief  register the random ipc update handle in multi-core scenarios
 * @else
 * @brief  多核场景下随机数IPC更新句柄注册
 * @endif
 */
void uapi_drv_cipher_trng_ipc_updata_reigister(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
