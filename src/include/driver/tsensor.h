/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides Tsensor driver api \n
 *
 * History: \n
 * 2022-09-16, Create file. \n
 */

#ifndef TSENSOR_H
#define TSENSOR_H

#include <stdint.h>
#include "errcode.h"
#include "tsensor_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_tsensor Tsensor
 * @ingroup  drivers_driver
 * @{
 */

#if defined(CONFIG_TSENSOR_TEMP_COMPENSATION)
/**
 * @if Eng
 * @brief  Tsensor calibration configuration.
 * @else
 * @brief  Tsensor温度补偿点定义。
 * @endif
 */
typedef struct tsensor_calibration_point {
    int8_t tsensor_temp;            /*!< @if Eng tsensor temp.
                                          @else   tsensor传感器温度。  @endif */
    int8_t environment_temp;        /*!< @if Eng environment temp.
                                          @else   真实环境温度。  @endif */
} tsensor_calibration_point_t;
#endif /* CONFIG_TSENSOR_TEMP_COMPENSATION */

/**
 * @if Eng
 * @brief  Definition of the samping mode of Tsensor.
 * @else
 * @brief  Tsensor设置采样模式定义。
 * @endif
 */
typedef enum tsensor_samp_mode {
    TSENSOR_SAMP_MODE_AVERAGE_ONCE,        /*!< @if Eng 16-point average single report mode.
                                                @else   16点平均单次上报模式。  @endif */
    TSENSOR_SAMP_MODE_AVERAGE_CYCLE,       /*!< @if Eng 16-point average cyclic reporting mode.
                                                @else   16点平均循环上报模式。  @endif */
    TSENSOR_SAMP_MODE_SINGLE_POINT_CYCLE,  /*!< @if Eng single point cyclic reporting mode.
                                                @else   单点循环上报模式。  @endif */
    TSENSOR_SAMP_MODE_MAX_NUM,
    TSENSOR_SAMP_MODE_NONE = TSENSOR_SAMP_MODE_MAX_NUM
} tsensor_samp_mode_t;

/**
 * @if Eng
 * @brief  Tsensor callback functions.
 * @param  [in]  temp Tsensor temperature.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  Tsensor 回调类型定义。
 * @param  [in]  temp Tsensor温度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*uapi_tsensor_callback_t)(int8_t temp);

/**
 * @if Eng
 * @brief  Initialize Tsensor.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  初始化Tsensor。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tsensor_init(void);

/**
 * @if Eng
 * @brief  Deinitialize Tsensor.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  去初始化Tsensor。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tsensor_deinit(void);

/**
 * @if Eng
 * @brief  Enable Tsensor inquire mode.
 * @param  [in]  mode The Tsensor samp mode. see @ref tsensor_samp_mode_t
 * @param  [in]  period Tsensor samping period.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  配置Tsensor的采样模式。
 * @param  [in]  mode 平均单次上报模式 or 平均循环上报模式。
 * @param  [in]  period Tsensor采样周期。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tsensor_start_inquire_mode(tsensor_samp_mode_t mode, uint32_t period);

/**
 * @if Eng
 * @brief  Enable tsensor inquire mode, when The temperature exceeds the threshold completion interrupt.
 * @param  [in]  callback: Interrupt callback function pointer.
 * @param  [in]  temp_threshold_low: Tsensor low limit, minimum value is -40C.
 * @param  [in]  temp_threshold_high: Tsensor high limit, maximum value is 125C.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  启用tsensor，当温度超出阈值时触发中断。
 * @param  [in]  callback: 中断回调函数指针。
 * @param  [in]  temp_threshold_low: 温度下限，最低-40C。
 * @param  [in]  temp_threshold_high: 温度上限，最高125C。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tsensor_enable_outtemp_interrupt(uapi_tsensor_callback_t callback,
                                                int8_t temp_threshold_low,
                                                int8_t temp_threshold_high);

/**
 * @if Eng
 * @brief  Enable Tsensor interrupt mode, when The temperature exceeds the over_temp completion interrupt.
 * @param  [in]  callback: Interrupt callback function pointer.
 * @param  [in]  overtemp: Tsensor high limit, maximum value is 125C.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  启用Tsensor，当温度过温时触发中断。
 * @param  [in]  callback: 中断回调函数指针。
 * @param  [in]  overtemp: 温度上限，最高125C。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tsensor_enable_overtemp_interrupt(uapi_tsensor_callback_t callback, int8_t overtemp);

/**
 * @if Eng
 * @brief  Enable Tsensor interrupt mode, when the temperature collection completion interrupt.
 * @param  [in]  callback: Interrupt callback function pointer.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  启用Tsensor，当温度采集完毕时触发中断。
 * @param  [in]  callback: 中断回调函数指针。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tsensor_enable_done_interrupt(uapi_tsensor_callback_t callback);

/**
 * @if Eng
 * @brief  Get Tsensor current temperature.
 * @param  [in]  temp: Temperature pointer, output parameter.
 * @retval ERRCODE_SUCC: Get Tsensor temperature success, temperature is valid.
 * @retval Other:        Get Tsensor temperature faild, temperature is invalid.
 * @else
 * @brief  获取当前温度值。
 * @param  [in]  temp: 温度指针，输出参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tsensor_get_current_temp(int8_t *temp);

#if defined(CONFIG_TSENSOR_TEMP_COMPENSATION)
/**
 * @if Eng
 * @brief  Setting single-point calibration parameters.
 * @param  [in]  point: Temperature pointer, abscissa is tsensor_temp, ordinate is environment_temp.
 *         @ref tsensor_calibration_point_t
 * @else
 * @brief  设置单点校准参数。
 * @param  [in]  point: 横坐标为传感器温度，纵坐标为环境温度。
 * @endif
 */
void uapi_tsensor_set_calibration_single_point(tsensor_calibration_point_t *point);

/**
 * @if Eng
 * @brief  Setting two-point calibration parameters.
 * @param  [in]  point_first: Temperature pointer, abscissa is tsensor_temp, ordinate is environment_temp.
 *         @ref tsensor_calibration_point_t
 * @param  [in]  point_second: Temperature pointer, abscissa is tsensor_temp, ordinate is environment_temp.
 *         @ref tsensor_calibration_point_t
 * @else
 * @brief  设置两点校准参数。
 * @param  [in]  point_first: 横坐标为传感器温度，纵坐标为环境温度。
 * @param  [in]  point_second: 横坐标为传感器温度，纵坐标为环境温度。
 * @endif
 */
void uapi_tsensor_set_calibration_two_points(const tsensor_calibration_point_t *point_first,
                                             const tsensor_calibration_point_t *point_second);
#endif /* CONFIG_TSENSOR_TEMP_COMPENSATION */

#if defined(CONFIG_TSENSOR_MULTILEVEL)
/**
 * @if Eng
 * @brief  To set the threshold for the multi-step temperature threshold.
 * @param  [in]  level Multi-step temperature range value
 * @param  [in]  temp  Temperature threshold.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置多步温度阈值的阈值。
 * @param  [in]  level 多级温度范围值
 * @param  [in]  temp  温度阈值.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tsensor_set_multilevel_threshold_value(tsensor_multilevel_value_t level, int16_t temp);

/**
 * @if Eng
 * @brief  Setting two-point calibration parameters.
 * @param  [in]  level: Temperature pointer, abscissa is tsensor_temp, ordinate is environment_temp.
 *         @ref tsensor_calibration_point_t
 * @param  [in]  callback: Temperature pointer, abscissa is tsensor_temp, ordinate is environment_temp.
 *         @ref tsensor_calibration_point_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置两点校准参数。
 * @param  [in]  level: 横坐标为传感器温度，纵坐标为环境温度。
 * @param  [in]  callback: 横坐标为传感器温度，纵坐标为环境温度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tsensor_set_multilevel_threshold_en(tsensor_multilevel_en_t level,
                                                   uapi_tsensor_callback_t callback);
#endif /* CONFIG_TSENSOR_MULTILEVEL */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
