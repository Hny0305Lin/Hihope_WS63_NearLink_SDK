/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2023-03-02, Create file. \n
 */
#ifndef HAL_RTC_H
#define HAL_RTC_H

#include <stdint.h>
#include "errcode.h"
#include "rtc_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_rtc_api RTC
 * @ingroup  drivers_hal_rtc
 * @{
 */

/**
 * @if Eng
 * @brief  Callback of RTC.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @else
 * @brief  RTC的回调函数。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @endif
 */
typedef void (*hal_rtc_callback_t)(rtc_index_t index);

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
errcode_t hal_rtc_init(rtc_index_t index, hal_rtc_callback_t callback);

/**
 * @if Eng
 * @brief  HAL RTC deinitialize interface.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @else
 * @brief  HAL层RTC的去初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_deinit(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC start the load count of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @else
 * @brief  HAL层启动硬件定时器计数的接口。
 * @param  [in]  index RTC底层索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_start(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC stop the load count of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @else
 * @brief  HAL层停止硬件定时器计数的接口。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_stop(rtc_index_t index);

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
void hal_rtc_config_load(rtc_index_t index, uint64_t delay_count);

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
uint64_t hal_rtc_get_current_value(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC get the count of RTC IRQ.
 * @return RTC irq count.
 * @else
 * @brief  HAL层获取RTC发生中断的次数的值。
 * @return rtc中断计数值。
 * @endif
 */
uint32_t hal_rtc_get_int_cnt_record(void);

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
uint32_t hal_rtc_get_int_status(rtc_index_t index);

/**
 * @if Eng
 * @brief  Init the RTC which will set the base address of registers.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化rtc，设置寄存器的基地址。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_rtc_regs_init(rtc_index_t index);

/**
 * @if Eng
 * @brief  Deinit the hal_drv_rtc which will clear the base address of registers has been
 *         set by @ref hal_rtc_regs_init.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @else
 * @brief  去初始化，然后清除在 @ref hal_rtc_regs_init 中设置的寄存器地址。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_regs_deinit(rtc_index_t index);


/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif