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
 * @brief  Definition of the contorl ID of hal systick.
 * @else
 * @brief  SYSTICK控制ID定义
 * @endif
 */
typedef enum hal_systick_ctrl_id {
    SYSTICK_CTRL_COUNT_CLEAR = 0,       /*!< @if Eng Set systick count clear.
                                        @else   设置计数清除 @endif */
    SYSTICK_CTRL_COUNT_CLEARED,         /*!< @if Eng Get systick count cleared.
                                        @else   判断计数已经清除 @endif */
    SYSTICK_CTRL_COUNT_GET,             /*!< @if Get systick count.
                                        @else   获取计数 @endif */
    SYSTICK_CTRL_MAX,
    SYSTICK_CTRL_ID_INVALID = 0xFF
} hal_systick_ctrl_id_t;

/**
 * @if Eng
 * @brief  Initialize device for hal systick.
 * @else
 * @brief  HAL层SYSTICK的初始化接口
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef void (*hal_systick_init_t)(void);

/**
 * @if Eng
 * @brief  Deinitialize device for hal systick.
 * @else
 * @brief  HAL层SYSTICK的去初始化接口
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef void (*hal_systick_deinit_t)(void);

/**
 * @if Eng
 * @brief  Control interface for hal systick.
 * @param  [in]  id ID of the systick control.
 * @param  [in]  param param of the systick control.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层SYSTICK控制接口
 * @param  [in]  id SYSTICK控制请求ID
 * @param  [in]  param SYSTICK控制请求参数
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_systick_ctrl_t)(hal_systick_ctrl_id_t id, uintptr_t param);

/**
 * @if Eng
 * @brief  Interface between systick driver and systick hal.
 * @else
 * @brief  Driver层SYSTICK和HAL层SYSTICK的接口
 * @endif
 */
typedef struct {
    hal_systick_init_t   init;               /*!< @if Eng Init device interface.
                                               @else   HAL层SYSTICK的初始化接口 @endif */
    hal_systick_deinit_t deinit;             /*!< @if Eng Deinit device interface.
                                               @else   HAL层SYSTICK去初始化接口 @endif */
    hal_systick_ctrl_t   ctrl;               /*!< @if Eng Control device interface.
                                               @else   HAL层SYSTICK控制接口 @endif */
} hal_systick_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_systick_funcs_t into the g_hal_systicks_funcs.
 * @param  [in]  funcs Interface between systick driver and systick hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_systick_funcs_t 到 g_hal_systicks_funcs
 * @param  [in]  funcs Driver层systick和HAL层systick的接口实例
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_systick_register_funcs(hal_systick_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_systick_funcs_t from the g_hal_systicks_funcs.
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  从g_hal_systicks_funcs注销 @ref hal_systick_funcs_t
 * @return ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_systick_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between systick driver and systick hal, see @ref hal_systick_funcs_t.
 * @return Interface between systick driver and systick hal, see @ref hal_systick_funcs_t.
 * @else
 * @brief  获取Driver层systick和HAL层systick的接口实例，参考 @ref hal_systick_funcs_t.
 * @return Driver层systick和HAL层systick的接口实例，参考 @ref hal_systick_funcs_t.
 * @endif
 */
hal_systick_funcs_t *hal_systick_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif