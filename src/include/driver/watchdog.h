/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides Watchdog driver \n
 *
 * History: \n
 * 2022-07-26, Create file. \n
 */
#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_watchdog Watchdog
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Watchdog trigger mode.
 * @else
 * @brief  看门狗触发模式。
 * @endif
 */
typedef enum {
    WDT_MODE_RESET = 0,     /** @if Eng Will reset core direcotry, when Watchdog trigger.
                             *  @else   当看门狗触发时，将重启系统。
                             *  @endif */
    WDT_MODE_INTERRUPT,     /** @if Eng Will enter interrupt, when WDT trigger. If WDT not kick in interrupt, \n
                             *          core will reset.
                             *  @else   当看门狗触发时，将进入中断。如果在中断中没有喂狗，系统将重启。
                             *  @endif */
    WDT_MODE_MAX
} wdt_mode_t;

/**
 * @if Eng
 * @brief  Init Watchdog.
 * @param  [in]  timeout The timeout of Watchdog.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化Watchdog。
 * @param  [in]  timeout 看门狗超时时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_watchdog_init(uint32_t timeout);

/**
 * @if Eng
 * @brief  Deinit Watchdog.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化Watchdog。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_watchdog_deinit(void);

/**
 * @if Eng
 * @brief  Enable Watchdog.
 * @param  [in]  mode The mode of watchdog， see @ref wdt_mode_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使能Watchdog。
 * @param  [in]  mode 看门狗模式，参考 @ref wdt_mode_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_watchdog_enable(wdt_mode_t mode);

/**
 * @if Eng
 * @brief  Disable Watchdog.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去使能Watchdog。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_watchdog_disable(void);

/**
 * @if Eng
 * @brief  Kick Watchdog.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  喂狗。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_watchdog_kick(void);

/**
 * @if Eng
 * @brief  Set timeout of Watchdog.
 * @param  [in]  timeout The timeout of Watchdog, unit s.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置Watchdog超时时间。
 * @param  [in]  timeout 看门狗超时时间，单位秒。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_watchdog_set_time(uint32_t timeout);

/**
 * @if Eng
 * @brief  Get the time left of Watchdog counter.
 * @param  [in]  timeout The time left.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  获取看门狗计数器的剩余值。
 * @param  [in]  timeout 时间剩余值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_watchdog_get_left_time(uint32_t *timeout);

/**
 * @if Eng
 * @brief  Watchdog timeout callback.
 * @param  [in]  param Parameters of the callback function.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  Watchdog超时回调。
 * @param  [in]  param 回调函数的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*watchdog_callback_t)(uintptr_t param);

/**
 * @if Eng
 * @brief  Register Watchdog callback.
 * @param  [in]  callback If Watchdog timeout, the callback is called to handle the exception.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册看门狗回调。
 * @param  [in]  callback 如果看门狗超时，则调用回调来处理异常。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_register_watchdog_callback(watchdog_callback_t callback);

#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
/**
 * @if Eng
 * @brief  resume Watchdog.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  恢复看门狗模块。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_watchdog_resume(uintptr_t arg);

/**
 * @if Eng
 * @brief  suspend Watchdog.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  挂起看门狗模块。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_watchdog_suspend(uintptr_t arg);
#endif
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif