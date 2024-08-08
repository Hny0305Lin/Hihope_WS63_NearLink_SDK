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
 * @brief  Initialize device for hal tcxo.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层TCXO的初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_tcxo_init_t)(void);

/**
 * @if Eng
 * @brief  Deinitialize device for hal tcxo.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层TCXO的去初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_tcxo_deinit_t)(void);

 /**
 * @if Eng
 * @brief  get interface for hal tcxo.
 * @retval tcxo count.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层TCXO读取接口。
 * @retval TCXO计数。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef uint64_t (*hal_tcxo_get_t)(void);

/**
 * @if Eng
 * @brief  Interface between TCXO driver and TCXO hal.
 * @else
 * @brief  Driver层TCXO和HAL层TCXO的接口。
 * @endif
 */
typedef struct {
    hal_tcxo_init_t   init;               /*!< @if Eng Init device interface.
                                               @else   HAL层TCXO的初始化接口。 @endif */
    hal_tcxo_deinit_t deinit;             /*!< @if Eng Deinit device interface.
                                               @else   HAL层TCXO去初始化接口。 @endif */
    hal_tcxo_get_t   get;               /*!< @if Eng Control device interface.
                                               @else   HAL层TCXO控制接口。 @endif */
} hal_tcxo_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_tcxo_funcs_t into the g_hal_tcxos_funcs.
 * @param  [out] funcs Interface between tcxo driver and tcxo hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册 @ref hal_tcxo_funcs_t 到 g_hal_tcxos_funcs 。
 * @param  [out] funcs Driver层TCXO和HAL层TCXO的接口实例。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_tcxo_register_funcs(hal_tcxo_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_tcxo_funcs_t from the g_hal_tcxos_funcs.
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从g_hal_tcxos_funcs注销 @ref hal_tcxo_funcs_t 。
 * @return ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_tcxo_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between tcxo driver and tcxo hal, see @ref hal_tcxo_funcs_t.
 * @return Interface between tcxo driver and tcxo hal, see @ref hal_tcxo_funcs_t.
 * @else
 * @brief  获取Driver层TCXO和HAL层TCXO的接口实例，参考 @ref hal_tcxo_funcs_t 。
 * @return Driver层TCXO和HAL层TCXO的接口实例，参考 @ref hal_tcxo_funcs_t 。
 * @endif
 */
hal_tcxo_funcs_t *hal_tcxo_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif