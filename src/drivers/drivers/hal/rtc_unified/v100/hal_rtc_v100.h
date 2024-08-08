/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V100 HAL rtc \n
 *
 * History: \n
 * 2023-03-03, Create file. \n
 */
#ifndef HAL_RTC_V100_H
#define HAL_RTC_V100_H

#include "hal_rtc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_rtc_v100 RTC V100
 * @ingroup  drivers_hal_rtc
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the RTC mode.
 * @else
 * @brief  RTC模式定义。
 * @endif
 */
typedef enum control_reg_mode {
    /** @if Eng  RTC mode: free run mode.
     *  @else    定时器控制模式：自由运行模式。
     *  @endif */
    RTC_MODE_FREE_RUN,
    /** @if Eng  RTC mode: user define mode.
     *  @else    定时器控制模式：用户自定义模式。
     *  @endif */
    RTC_MODE_USER_DEF
} hal_rtc_v100_ctrl_reg_mode_t;

/**
 * @if Eng
 * @brief  HAL RTC initialize interface.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @param  [in]  callback Callback of RTC.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层RTC的初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @param  [in]  callback RTC的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_rtc_v100_init(rtc_index_t index, hal_rtc_callback_t callback);

/**
 * @if Eng
 * @brief  HAL RTC deinitialize interface.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @else
 * @brief  HAL层RTC的去初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_v100_deinit(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC start the load count of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @else
 * @brief  HAL层启动硬件定时器计数的接口。
 * @param  [in]  index RTC底层索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_v100_start(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC stop the load count of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @else
 * @brief  HAL层停止硬件定时器计数的接口。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_v100_stop(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC set the load count of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @param  [in]  delay_count Time for load count.
 * @else
 * @brief  HAL层设置硬件计时器计数的接口。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @param  [in]  delay_count 计时的时间。
 * @endif
 */
void hal_rtc_v100_config_load(rtc_index_t index, uint64_t delay_count);

/**
 * @if Eng
 * @brief  HAL RTC get the current value of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @return RTC load count.
 * @else
 * @brief  HAL层获取硬件当时计时器剩余计数的接口。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @return rtc计数值。
 * @endif
 */
uint64_t hal_rtc_v100_get_current_value(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC get the count of RTC IRQ.
 * @return RTC irq count.
 * @else
 * @brief  HAL层获取RTC发生中断的次数的值。
 * @return rtc中断计数值。
 * @endif
 */
uint32_t hal_rtc_v100_get_int_cnt_record(void);

/**
 * @if Eng
 * @brief  HAL RTC get the current interrupt status of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @return RTC load count.
 * @else
 * @brief  HAL层获取硬件当时计时器中断状态。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @return rtc计数值。
 * @endif
 */
uint32_t hal_rtc_v100_get_int_status(rtc_index_t index);

/**
 * @if Eng
 * @brief  Handler of the RTC interrupt request.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @else
 * @brief  RTC中断处理函数。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_v100_irq_handler(rtc_index_t index);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif