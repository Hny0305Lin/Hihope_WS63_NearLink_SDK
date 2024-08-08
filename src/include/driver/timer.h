/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides Timer driver api \n
 *
 * History: \n
 * 2022-11-07， Create file. \n
 */
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "errcode.h"
#include "hal_drv_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_timer Timer
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Handle of Timer.
 * @else
 * @brief  定时器句柄。
 * @endif
 */
typedef void *timer_handle_t;

/**
 * @if Eng
 * @brief  Timer callback function.
 * @else
 * @brief  定时器回调函数。
 * @endif
 */
typedef void (*timer_callback_t)(uintptr_t data);

/**
 * @if Eng
 * @brief  Initialize the Timer.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化定时器。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_init(void);

/**
 * @if Eng
 * @brief  Adapt the Timer.
 * @param  [in] index Index of the hardware Timer.
 * @param  [in] int_id Interrupt ID of the hardware Timer.
 * @param  [in] int_priority Hardware Timer interrupt priority.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  适配定时器。
 * @param  [in] index 硬件定时器索引。
 * @param  [in] int_id 硬件定时器中断ID。
 * @param  [in] int_priority 硬件定时器中断优先级。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_adapter(timer_index_t index, uint32_t int_id, uint16_t int_priority);

/**
 * @if Eng
 * @brief  Deinitialize the Timer.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化定时器。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_deinit(void);

/**
 * @if Eng
 * @brief  Create a Timer.
 * @param  [in] index Index of the hardware Timer.
 * @param  [out] timer Timer handle that returned.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建定时器。
 * @param  [in] index 硬件定时器索引。
 * @param  [out] timer 定时器处理返回值。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_create(timer_index_t index, timer_handle_t *timer);

/**
 * @if Eng
 * @brief  Delete a Timer.
 * @param  [in]  timer Timer handle which created by @ref uapi_timer_create.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  删除定时器。
 * @param  [in]  timer 被 @ref uapi_timer_create 创建的定时器。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_delete(timer_handle_t timer);

/**
 * @if Eng
 * @brief  Access the maximum settable delay time of the Timer(us).
 * @retval uint32_t  delay time(us).
 * @else
 * @brief  用户可以获取到timer最大可以设置的延时时间(us)。
 * @retval uint32_t  延时时间(us)。
 * @endif
 */
uint32_t uapi_timer_get_max_us(void);

/**
 * @if Eng
 * @brief  Start a Timer.
 * @param  [in]  timer Timer handle which created by @ref uapi_timer_create.
 * @param  [in]  time_us Timer expiration time. see @ref uapi_timer_get_max_us obtains the maximum time.
 * @param  [in]  callback The callback function of the Timer.
 * @param  [in]  data Input parameter of the Timer callback function.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启动指定的定时器。
 * @param  [in]  timer 被 @ref uapi_timer_create 创建的定时器。
 * @param  [in]  time_us 定时器超时时间。参考 @ref uapi_timer_get_max_us 函数获取最大时间。
 * @param  [in]  callback 定时器回调函数。
 * @param  [in]  data 定时器参数，用于传递给定时器回调函数。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_start(timer_handle_t timer, uint32_t time_us, timer_callback_t callback, uintptr_t data);

/**
 * @if Eng
 * @brief  Stop a Timer, the callback passed in by the user will not be called.
 * @param  [in]  timer Timer handle which created by @ref uapi_timer_create.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止指定的定时器, 不会调用用户传入的callback.
 * @param  [in]  timer 被 @ref uapi_timer_create 创建的定时器。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_stop(timer_handle_t timer);

/**
 * @if Eng
 * @brief  Get current time of the low layer Timer.
 * @param  [in]  index Index of the low layer Timer.
 * @param  [out] current_time_us Current time (us) of the low layer Timer.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取指定底层Timer定时器的当前时间。
 * @param  [in]  index 底层Timer定时器索引。
 * @param  [out] current_time_us 底层Timer定时器当前时间us值。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_get_current_time_us(timer_index_t index, uint32_t *current_time_us);

#if defined(CONFIG_TIMER_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend the Timer.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起Timer。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_suspend(uintptr_t val);

/**
 * @if Eng
 * @brief  Resume the Timer.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复Timer。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_timer_resume(uintptr_t val);
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