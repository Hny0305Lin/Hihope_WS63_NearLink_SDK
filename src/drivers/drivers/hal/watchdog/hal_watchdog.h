/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides hal watchdog \n
 *
 * History: \n
 * 2022-07-26, Create file. \n
 */

#ifndef HAL_WATCHDOG_H
#define HAL_WATCHDOG_H

#include <stdint.h>
#include "common_def.h"
#include "errcode.h"
#include "watchdog_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_watchdog_api Watchdog
 * @ingroup  drivers_hal_watchdog
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the watchdog mode.
 * @else
 * @brief  watchdog模式的定义
 * @endif
 */
typedef enum hal_wdt_mode {
    HAL_WDT_MODE_RESET,                            /*!< @if Eng Will reset core instructions, when watchdog trigger
                                                        @else   当watchdog触发时，将重置核心指令 @endif */
    HAL_WDT_MODE_INTERRUPT                         /*!< @if Eng Will enter interrupt, when watchdog trigger.
                                                                If watchdog not kick in interrupt, core will reset
                                                        @else   当看门狗触发时，将进入中断。如果看门狗没有进入中断，
                                                                内核将复位。@endif */
} hal_wdt_mode_t;

/**
 * @if Eng
 * @brief  Watchdog timeout callback.
 * @param  [in]  param Parameters of the callback function.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  Watchdog超时回调
 * @param  [in]  param 回调函数的参数
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_watchdog_callback_t)(uintptr_t param);

/**
 * @if Eng
 * @brief  Init device for hal watchdog.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层WATCHDOG的初始化接口
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_watchdog_init_t)(void);

/**
 * @if Eng
 * @brief  Deinit device for hal watchdog.
 * @else
 * @brief  HAL层WATCHDOG的去初始化接口
 * @endif
 */
typedef void (*hal_watchdog_deinit_t)(void);

/**
 * @if Eng
 * @brief  Set the attribute for hal watchdog.
 * @param  [in]  timeout The watchdog_timeout, the timeout period has only 16 types.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置Watchdog的属性。
 * @param  [in]  timeout 超时时间,超时时间只有16种类型。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_watchdog_set_attr_t)(uint32_t timeout);

/**
 * @if Eng
 * @brief  Get the attribute for hal watchdog.
 * @param  [in]  timeout The watchdog_timeout, the timeout period has only 16 types.
 * @return The watchdog_timeout, the timeout period has only 16 types.
 * @else
 * @brief  设置Watchdog的属性。
 * @param  [in]  timeout 超时时间，超时时间只有16种类型。
 * @return 超时时间，超时时间只有16种类型。
 * @endif
 */
typedef uint32_t (*hal_watchdog_get_attr_t)(void);

/**
 * @if Eng
 * @brief  Enable device for hal watchdog.
 * @param  [in]  mode The watchdog_mode. see @ref hal_wdt_mode_t
 * @else
 * @brief  HAL层WATCHDOG的使能接口
 * @param  [in]  mode watchdog模式，参考 @ref hal_wdt_mode_t
 * @endif
 */
typedef void (*hal_watchdog_enable_t)(hal_wdt_mode_t mode);

/**
 * @if Eng
 * @brief  Disable device for hal watchdog.
 * @else
 * @brief  HAL层WATCHDOG去使能接口
 * @endif
 */
typedef void (*hal_watchdog_disable_t)(void);

/**
 * @if Eng
 * @brief  Kick watchdog interface for hal watchdog.
 * @else
 * @brief  HAL层喂狗接口
 * @endif
 */
typedef void (*hal_watchdog_kick_t)(void);

/**
 * @if Eng
 * @brief  Get the time left interface for hal watchdog.
 * @return Time left in number of clock cycles.
 * @retval 0        Failure.
 * @else
 * @brief  HAL层获取剩余时间接口
 * @return 时钟周期数的剩余时间， 成功
 * @retval  0        失败
 * @endif
 */
typedef uint32_t (*hal_watchdog_get_time_t)(void);

/**
 * @if Eng
 * @brief  Register callback interface for hal watchdog.
 * @param  [in]  callback Callback function.
 * @else
 * @brief  HAL层注册回调接口
 * @param  [in]  callback 回调函数
 * @endif
 */
typedef void (*hal_register_watchdog_callback_t)(hal_watchdog_callback_t callback);

/**
 * @if Eng
 * @brief  Interface between watchdog driver and watchdog hal.
 * @else
 * @brief  Driver层WATCHDOG和HAL层WATCHDOG的接口
 * @endif
 */
typedef struct {
    hal_watchdog_init_t    init;                             /*!< @if Eng Init watchdog interface.
                                                                  @else   HAL层WATCHDOG的初始化接口 @endif */
    hal_watchdog_deinit_t  deinit;                           /*!< @if Eng Deinit watchdog interface.
                                                                  @else   HAL层WATCHDOG去初始化接口 @endif */
    hal_watchdog_set_attr_t set_attr;                        /*!< @if Eng Set attribute interface.
                                                                  @else   HAL层WATCHDOG设置属性接口 @endif */
    hal_watchdog_get_attr_t get_attr;                        /*!< @if Eng Get attribute interface.
                                                                  @else   HAL层WATCHDOG获取属性接口 @endif */
    hal_watchdog_enable_t  enable;                           /*!< @if Eng Enable watchdog interface.
                                                                  @else   HAL层WATCHDOG的使能接口 @endif */
    hal_watchdog_disable_t disable;                          /*!< @if Eng Disable watchdog interface.
                                                                  @else   HAL层WATCHDOG去使能接口 @endif */
    hal_watchdog_kick_t    kick;                             /*!< @if Eng Kick watchdog interface.
                                                                  @else   HAL层WATCHDOG喂狗接口 @endif */
    hal_watchdog_get_time_t  get_left_time;                  /*!< @if Eng Get the timeout interface.
                                                                  @else   HAL层WATCHDOG获取超时时间接口 @endif */
    hal_register_watchdog_callback_t  register_callback;     /*!< @if Eng Register watchdog timeout callback.
                                                                  @else   HAL层注册WATCHDOG超时回调接口 @endif */
} hal_watchdog_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_watchdog_funcs_t into the g_hal_watchdogs_funcs.
 * @param  [in]  funcs Interface between watchdog driver and watchdog hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_watchdog_funcs_t 到 g_hal_watchdogs_funcs
 * @param  [in]  funcs Driver层WATCHDOG和HAL层WATCHDOG的接口实例
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_watchdog_register_funcs(hal_watchdog_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_watchdog_funcs_t into the g_hal_watchdogs_funcs.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_watchdog_funcs_t 到 g_hal_watchdogs_funcs
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_watchdog_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between watchdog driver and watchdog hal, see @ref hal_watchdog_funcs_t.
 * @return Interface between watchdog driver and watchdog hal, see @ref hal_watchdog_funcs_t.
 * @else
 * @brief  获取Driver层WATCHDOG和HAL层WATCHDOG的接口实例，参考 @ref hal_watchdog_funcs_t.
 * @return Driver层WATCHDOG和HAL层WATCHDOG的接口实例，参考 @ref hal_watchdog_funcs_t.
 * @endif
 */
hal_watchdog_funcs_t *hal_watchdog_get_funcs(void);

/**
 * @if Eng
 * @brief  Init the watchdog which will set the base address of registers.
 * @retval ERRCODE_SUCC   Success
 * @retval ERRCODE_WDT_REG_ADDR_INVALID   Failure, see @ref errcode_t
 * @else
 * @brief  初始化watchdog，watchdog将设置寄存器的基址
 * @retval ERRCODE_SUCC   成功
 * @retval ERRCODE_WDT_REG_ADDR_INVALID   失败, 参考 @ref errcode_t
 * @endif
 */
errcode_t hal_watchdog_regs_init(void);

/**
 * @if Eng
 * @brief  Deinit the hal_watchdog which will clear the base address of registers has been set
 *         by @ref hal_watchdog_regs_init.
 * @else
 * @brief  去初始化hal_watchdog，这将清除被 @ref hal_watchdog_regs_init 已经设置的寄存器的基址
 * @endif
 */
void hal_watchdog_regs_deinit(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif