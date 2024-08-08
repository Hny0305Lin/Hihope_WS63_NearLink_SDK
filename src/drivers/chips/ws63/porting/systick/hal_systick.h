/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides HAL systick \n
 *
 * History: \n
 * 2023-04-06, Create file. \n
 */
#ifndef HAL_SYSTICK_H
#define HAL_SYSTICK_H

#include <stdint.h>
#include "common_def.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_systick_api Systick
 * @ingroup  drivers_hal_systick
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize the Systick.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化Systick。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void hal_systick_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the Systick.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化Systick。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void hal_systick_deinit(void);

/**
 * @if Eng
 * @brief  Clear Systick count.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  清除Systick计数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_systick_count_clear(void);

/**
 * @if Eng
 * @brief  Get Systick count.
 * @retval Systick current count.
 * @else
 * @brief  获取Systick计数值。
 * @retval Systick当前计数值。
 * @endif
 */
uint64_t hal_systick_get_count(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif