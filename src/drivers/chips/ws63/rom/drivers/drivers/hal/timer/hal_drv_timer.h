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
typedef errcode_t (*hal_timer_init_t)(timer_index_t index, hal_timer_callback_t callback);

/**
 * @if Eng
 * @brief  HAL timer deinit interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  HAL层Timer的去初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
typedef void (*hal_timer_deinit_t)(timer_index_t index);

/**
 * @if Eng
 * @brief  HAL timer start the load count of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  HAL层启动硬件定时器计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
typedef void (*hal_timer_start_t)(timer_index_t index);

/**
 * @if Eng
 * @brief  HAL timer stop the load count of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  HAL层停止硬件定时器计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
typedef void (*hal_timer_stop_t)(timer_index_t index);

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
typedef void (*hal_timer_set_load_count_t)(timer_index_t index, uint64_t delay_count);

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
typedef uint64_t (*hal_timer_get_current_value_t)(timer_index_t index);

/**
 * @if Eng
 * @brief  Functions interface between timer driver and timer HAL.
 * @else
 * @brief  Driver层Timer和HAL层Timer的函数接口。
 * @endif
 */
typedef struct {
    hal_timer_init_t                 init;                       /*!< @if Eng Init timer interface.
                                                                      @else   HAL层TIMER的初始化接口。 @endif */
    hal_timer_deinit_t               deinit;                     /*!< @if Eng Deinit timer interface.
                                                                      @else   HAL层TIMER去初始化接口。 @endif */
    hal_timer_start_t                start;                      /*!< @if Eng Start timer interface.
                                                                      @else   HAL层Timer启动接口。 @endif */
    hal_timer_stop_t                 stop;                       /*!< @if Eng Stop timer interface.
                                                                      @else   HAL层Timer停止接口。 @endif */
    hal_timer_set_load_count_t       config_load;                /*!< @if Eng Config init interface.
                                                                      @else   HAL层Timer载入计数值并使能。 @endif */
    hal_timer_get_current_value_t    get_current_value;          /*!< @if Eng get current value.
                                                                      @else   HAL层Timer获取当前剩余计数值。 @endif */
} hal_timer_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_timer_funcs_t into the g_hal_timers_funcs.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  funcs Interface between timer driver and timer HAL.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册 @ref hal_timer_funcs_t 到 g_hal_timers_funcs
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @param  [in]  funcs Driver层Timer和HAL层Timer的接口实例。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_timer_register_funcs(timer_index_t index, hal_timer_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_timer_funcs_t from the g_hal_timers_funcs.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从g_hal_timers_funcs注销 @ref hal_timer_funcs_t 。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void hal_timer_unregister_funcs(timer_index_t index);

/**
 * @if Eng
 * @brief  Get interface between timer driver and timer HAL. For detail, see @ref hal_timer_funcs_t.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @return Interface between timer driver and timer HAL. For detail, see @ref hal_timer_funcs_t.
 * @else
 * @brief  获取Driver层Timer和HAL层Timer的接口实例，参考 @ref hal_timer_funcs_t 。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @return Driver层Timer和HAL层Timer的接口实例，参考 @ref hal_timer_funcs_t 。
 * @endif
 */
hal_timer_funcs_t *hal_timer_get_funcs(timer_index_t index);

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
