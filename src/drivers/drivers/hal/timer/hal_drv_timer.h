/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL drv timer \n
 *
 * History: \n
 * 2022-06-02, Create file. \n
 */
#ifndef HAL_DRV_TIMER_H
#define HAL_DRV_TIMER_H

#include <stdint.h>
#include "errcode.h"
#include "timer_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_timer_api Timer
 * @ingroup  drivers_hal_timer
 * @{
 */

/**
 * @if Eng
 * @brief  Callback of timer.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  Timer的回调函数。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
typedef void (*hal_timer_callback_t)(timer_index_t index);

/**
 * @if Eng
 * @brief  HAL timer init interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  callback Callback of timer.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层Timer的初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @param  [in]  callback Timer的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_timer_init(timer_index_t index, hal_timer_callback_t callback);

/**
 * @if Eng
 * @brief  HAL timer deinit interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  HAL层Timer的去初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_deinit(timer_index_t index);

/**
 * @if Eng
 * @brief  HAL timer start the load count of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  HAL层启动硬件定时器计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_start(timer_index_t index);

/**
 * @if Eng
 * @brief  HAL timer stop the load count of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  HAL层停止硬件定时器计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_stop(timer_index_t index);

/**
 * @if Eng
 * @brief  HAL timer set the load count of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  delay_count Time for load count.
 * @else
 * @brief  HAL层设置硬件计时器计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @param  [in]  delay_count 计时的时间。
 * @endif
 */
void hal_timer_config_load(timer_index_t index, uint64_t delay_count);

/**
 * @if Eng
 * @brief  HAL timer get the current value of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @return Timer load count.
 * @else
 * @brief  HAL层获取硬件当前计时器剩余计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @return Timer计数值。
 * @endif
 */
uint64_t hal_timer_get_current_value(timer_index_t index);

/**
 * @if Eng
 * @brief  Init the timer which will set the base address of registers.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化Timer，设置寄存器的基地址。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_timer_regs_init(timer_index_t index);

/**
 * @if Eng
 * @brief  Deinit the hal_drv_timer which will clear the base address of registers has been
 *         set by @ref hal_timer_regs_init.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  去初始化，然后清除在 @ref hal_timer_regs_init 中设置的寄存器地址。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_regs_deinit(timer_index_t index);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
