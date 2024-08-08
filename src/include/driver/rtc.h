/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides RTC driver api \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */
#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include "errcode.h"
#include "hal_rtc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_rtc RTC
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Handle of RTC.
 * @else
 * @brief  RTC定时器句柄。
 * @endif
 */
typedef void *rtc_handle_t;

/**
 * @if Eng
 * @brief  RTC callback function.
 * @param  [in] data RTC callback function.
 * @else
 * @brief  RTC定时器回调函数。
 * @param  [in] data RTC定时器的回调函数。
 * @endif
 */
typedef void (*rtc_callback_t)(uintptr_t data);

/**
 * @if Eng
 * @brief  Initialize the RTC.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化RTC定时器。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_init(void);

/**
 * @if Eng
 * @brief  Adapt the RTC.
 * @param  [in] index Index of the low layer RTC.
 * @param  [in] int_id Interrupt ID of the low layer RTC.
 * @param  [in] int_priority RTC interrupt priority.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  适配RTC定时器。
 * @param  [in] index 底层RTC定时器索引。
 * @param  [in] int_id 底层RTC定时器中断ID。
 * @param  [in] int_priority RTC定时器中断优先级。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_adapter(rtc_index_t index, uint32_t int_id, uint16_t int_priority);

/**
 * @if Eng
 * @brief  Deinitialize the RTC.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化RTC定时器。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_deinit(void);

/**
 * @if Eng
 * @brief  Create a RTC.
 * @param  [in] index Index of the low layer RTC.
 * @param  [out] rtc RTC handle that returned.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建RTC定时器。
 * @param  [in] index 底层RTC定时器索引。
 * @param  [out] rtc RTC定时器处理返回值。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_create(rtc_index_t index, rtc_handle_t *rtc);

/**
 * @if Eng
 * @brief  Delete a RTC.
 * @param  [in]  rtc RTC handle which created by @ref uapi_rtc_create.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  删除RTC定时器。
 * @param  [in]  rtc 被 @ref uapi_rtc_create 创建的RTC定时器。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_delete(rtc_handle_t rtc);

/**
 * @if Eng
 * @brief  Start a RTC.
 * @param  [in]  rtc RTC handle which created by @ref uapi_rtc_create.
 * @param  [in]  rtc_ms RTC expiration time. see @ref uapi_rtc_get_max_ms obtains the maximum time.
 * @param  [in]  callback The callback function of the RTC.
 * @param  [in]  data Input parameter of the RTC callback function.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启动指定的RTC定时器。
 * @param  [in]  rtc 被 @ref uapi_rtc_create 创建的RTC定时器。
 * @param  [in]  rtc_ms RTC定时器超时时间。参考 @ref uapi_rtc_get_max_ms 函数获取最大时间。
 * @param  [in]  callback RTC定时器回调函数。
 * @param  [in]  data RTC定时器参数，用于传递给RTC定时器回调函数。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_start(rtc_handle_t rtc, uint32_t rtc_ms, rtc_callback_t callback, uintptr_t data);

/**
 * @if Eng
 * @brief  Stop a RTC. the callback passed in by the user will not be called.
 * @param  [in]  rtc RTC handle which created by @ref uapi_rtc_create.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止指定的RTC定时器, 不会调用用户传入的callback。
 * @param  [in]  rtc 被 @ref uapi_rtc_create 创建的RTC定时器。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_stop(rtc_handle_t rtc);

/**
 * @if Eng
 * @brief  Access the maximum settable delay time of the RTC(ms).
 * @retval Delay time(ms).
 * @else
 * @brief  用户可以获取到RTC最大可以设置的延时时间(ms)。
 * @retval 延时时间(ms)。
 * @endif
 */
uint32_t uapi_rtc_get_max_ms(void);

/**
 * @if Eng
 * @brief  Obtains the number of interrupts.
 * @retval IRQ counts.
 * @else
 * @brief  获取发生中断的次数。
 * @retval 中断次数。
 * @endif
 */
uint32_t uapi_rtc_int_cnt_record_get(rtc_index_t index);

/**
 * @if Eng
 * @brief  Get current count of the low layer RTC.
 * @param  [in]  index Index of the low layer RTC.
 * @param  [in]  current_time_count Current count of the low layer RTC.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取指定底层RTC定时器的当前计数。
 * @param  [in]  index 底层RTC定时器索引。
 * @param  [in]  current_time_count 底层RTC定时器当前计数值。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_get_current_time_count(rtc_index_t index, uint64_t *current_time_count);

/**
 * @if Eng
 * @brief  Get current time of the low layer RTC.
 * @param  [in]  index Index of the low layer RTC.
 * @param  [in]  current_time_us Current time (us) of the low layer RTC.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取指定底层RTC定时器的当前时间。
 * @param  [in]  index 底层RTC定时器索引。
 * @param  [in]  current_time_us 底层RTC定时器当前时间us值。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_get_current_time_us(rtc_index_t index, uint32_t *current_time_us);

/**
 * @if Eng
 * @brief  Start a low layer RTC for timing, will not process interrupt.
 * @attention  When using low layer RTC for timing, the index that is already in use cannot be used.
 * @param  [in]  index Index of the low layer RTC.
 * @param  [in]  rtc_ms RTC expiration time. RTC_HW_MAX_MS obtains the maximum time.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启动指定底层RTC定时器，用于计时，不会处理中断。
 * @attention  直接使用底层RTC计时的时候，不能使用已经在使用中的index。
 * @param  [in]  index 底层RTC定时器索引。
 * @param  [in]  rtc_ms RTC定时器超时时间。RTC_HW_MAX_MS 函数获取最大时间。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_start_hw_rtc(rtc_index_t index, uint64_t rtc_ms);

/**
 * @if Eng
 * @brief  Stop a low layer RTC.
 * @param  [in]  index Index of the low layer RTC.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止指定底层RTC定时器。
 * @param  [in]  index 底层RTC定时器索引。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_stop_hw_rtc(rtc_index_t index);

#if defined(CONFIG_RTC_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Obtains the latest timeout interval of all RTCs.
 * @retval The latest timeout, in milliseconds.
 * @else
 * @brief  获取所有RTC最近的超时时间。
 * @retval 最近的超时时间(毫秒)。
 * @endif
 */
uint32_t uapi_rtc_get_latest_timeout(void);

/**
 * @if Eng
 * @brief  Suspend the RTC.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起RTC。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_suspend(uintptr_t val);

/**
 * @if Eng
 * @brief  Resume the RTC.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复RTC。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_rtc_resume(uintptr_t val);
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