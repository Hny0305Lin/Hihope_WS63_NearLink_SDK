/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL tcxo \n
 *
 * History: \n
 * 2022-08-16， Create file. \n
 */
#ifndef HAL_TCXO_H
#define HAL_TCXO_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_tcxo_api TCXO
 * @ingroup  drivers_hal_tcxo
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize TCXO HAL layer.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化TCXO。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_tcxo_init(void);

/**
 * @if Eng
 * @brief  Deinitialize TCXO HAL layer.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化TCXO。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_tcxo_deinit(void);

/**
 * @if Eng
 * @brief  Get TCXO count.
 * @retval TCXO current count.
 * @else
 * @brief  获取TCXO计数值。
 * @retval TCXO当前计数值。
 * @endif
 */
uint64_t hal_tcxo_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif