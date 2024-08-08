/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides V154 HAL adc \n
 *
 * History: \n
 * 2024-02-01， Create file. \n
 */
#ifndef HAL_ADC_V154_H
#define HAL_ADC_V154_H

#include "hal_adc.h"
#include "hal_adc_v154_regs_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_adc_v154 ADC V154
 * @ingroup  drivers_hal_adc
 * @{
 */

/**
 * @if Eng
 * @brief  ADC type infomation.
 * @else
 * @brief  adc类型信息。
 * @endif
 */
typedef struct {
    adc_channel_t channel;
    hal_adc_channel_type_t channel_type;
    uint32_t scan_fifo_start_addr;
} hal_adc_type_info_t;

/**
 * @if Eng
 * @brief  Get the instance of v154.
 * @return The instance of v154.
 * @else
 * @brief  获取v154实例。
 * @return v154实例。
 * @endif
 */
hal_adc_funcs_t *hal_adc_v154_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the adc interrupt request.
 * @else
 * @brief  adc中断处理函数。
 * @endif
 */
void hal_adc_irq_handler(void);

/**
 * @if Eng
 * @brief  ADC get channel config.
 * @retval ADC type infomation. For details, see @ref hal_adc_type_info_t.
 * @else
 * @brief  adc获取通道配置。
 * @retval adc类型信息 参考 @ref hal_adc_type_info_t 。
 * @endif
 */
hal_adc_type_info_t *adc_port_get_cfg(void);

/**
 * @if Eng
 * @brief  ADC get channel config.
 * @param  [in]  ch The adc channel.
 * @param  [in]  on Set or clear of adc channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @param  [in]  ch adc通道。
 * @param  [in]  on 设置或清除通道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败 参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_adc_v154_channel_set(adc_channel_t ch, bool on);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif