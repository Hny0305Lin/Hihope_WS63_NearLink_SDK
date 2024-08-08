/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides hal Tsensor \n
 *
 * History: \n
 * 2023-03-06, Create file. \n
 */
#ifndef HAL_TSENSOR_H
#define HAL_TSENSOR_H

#include <stdint.h>
#include "errcode.h"
#include "tsensor_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_tsensor_api Tsensor
 * @ingroup  drivers_hal_tsensor
 * @{
 */

#define HAL_TSENSOR_TEMP_THRESHOLD_L_MAX ((int8_t)(-40))
#define HAL_TSENSOR_TEMP_THRESHOLD_H_MAX ((int8_t)(125))

/**
 * @if Eng
 * @brief  Definition of the samping mode of hal Tsensor.
 * @else
 * @brief  Tsensor设置采样模式定义。
 * @endif
 */
typedef enum hal_tsensor_samp_mode {
    HAL_TSENSOR_SAMP_MODE_AVERAGE_ONCE,        /*!< @if Eng 16-point average single report mode.
                                                    @else   16点平均单次上报模式。  @endif */
    HAL_TSENSOR_SAMP_MODE_AVERAGE_CYCLE,       /*!< @if Eng 16-point average cyclic reporting mode.
                                                    @else   16点平均循环上报模式。  @endif */
    HAL_TSENSOR_SAMP_MODE_SINGLE_POINT_CYCLE,  /*!< @if Eng single point cyclic reporting mode.
                                                    @else   单点循环上报模式。  @endif */
    HAL_TSENSOR_SAMP_MODE_MAX_NUM,
    HAL_TSENSOR_SAMP_MODE_NONE = HAL_TSENSOR_SAMP_MODE_MAX_NUM
} hal_tsensor_samp_mode_t;

/**
 * @if Eng
 * @brief  Definition of the interript type hal Tsensor.
 * @else
 * @brief  Tsensor中断类别定义。
 * @endif
 */
typedef enum hal_tsensor_interript_type {
    HAL_TSENSOR_INTERRIPT_TYPE_DONE,
    HAL_TSENSOR_INTERRIPT_TYPE_OUT_THRESH,
    HAL_TSENSOR_INTERRIPT_TYPE_OVERTEMP,
    HAL_TSENSOR_INTERRIPT_TYPE_MAX_NUM,
    HAL_TSENSOR_INTERRIPT_TYPE_NONE = HAL_TSENSOR_INTERRIPT_TYPE_MAX_NUM
} hal_tsensor_interript_type_t;

/**
 * @if Eng
 * @brief  Definition of the set low/high threshold ID of hal Tsensor.
 * @else
 * @brief  Tsensor设置高低温度阈值ID定义。
 * @endif
 */
typedef enum hal_tsensor_set_temp_id {
    TSENSOR_SET_LOW_TEMP,           /*!< @if Eng Set TSENSOR low temp threshold.
                                         @else   设置TSENSOR低温阈值。 @endif */
    TSENSOR_SET_HIGH_TEMP,          /*!< @if Eng Set  TSENSOR high temp threshold.
                                         @else   设置TSENSOR高温阈值。 @endif */
    TSENSOR_SET_OVER_TEMP,          /*!< @if Eng Set  TSENSOR over temp threshold.
                                         @else   设置TSENSOR过温阈值。 @endif */
    TSENSOR_SET_OFFSET_TEMP,        /*!< @if Eng TSENSOR offset threshold.
                                         @else   TSENSOR相位。 @endif */
    TSENSOR_SET_TEMP_MAX
} hal_tsensor_set_temp_id_t;

/**
 * @if Eng
 * @brief  Definition of the calibration point nums.
 * @else
 * @brief  Tsensor温度补偿点数量定义。
 * @endif
 */
typedef enum hal_tsensor_calibration_num {
    HAL_TSENSOR_CALIBRATION_NUM_ZERO,
    HAL_TSENSOR_CALIBRATION_NUM_SINGLE,
    HAL_TSENSOR_CALIBRATION_NUM_TWO,
    HAL_TSENSOR_CALIBRATION_NUM_MAX,
    HAL_TSENSOR_CALIBRATION_NUM_NONE = HAL_TSENSOR_CALIBRATION_NUM_MAX
} hal_tsensor_calibration_num_t;

#if defined(CONFIG_TSENSOR_TEMP_COMPENSATION)
/**
 * @if Eng
 * @brief  HAL Tsensor calibration configuration.
 * @else
 * @brief  HAL Tsensor温度补偿点定义。
 * @endif
 */
typedef struct {
    int8_t tsensor_temp;            /*!< @if Eng tsensor temp.
                                         @else   tsensor传感器温度。  @endif */
    int8_t environment_temp;        /*!< @if Eng environment temp.
                                         @else   真实环境温度。  @endif */
} hal_tsensor_calibration_point_t;
#endif /* CONFIG_TSENSOR_TEMP_COMPENSATION */

 /**
 * @if Eng
 * @brief  Definition of hal TSENSOR callback type.
 * @param  [in]  temp TSENSOR temperature.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  TSENSOR hal层回调类型定义。
 * @param  [in]  temp TSENSOR温度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_tsensor_callback_t)(int8_t temp);

/**
 * @if Eng
 * @brief  Initialize device for hal TSENSOR.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  HAL层TSENSOR的初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_tsensor_init_t)(void);

/**
 * @if Eng
 * @brief  Deinitialize device for hal TSENSOR.
 * @else
 * @brief  HAL层TSENSOR的去初始化接口。
 * @endif
 */
typedef void (*hal_tsensor_deinit_t)(void);

/**
 * @if Eng
 * @brief  Enable and config tsensor samping mode.
 * @param  [in] mode Tsensor samping mode.
 * @param  [in] period Tsensor samping period.
 * @else
 * @brief  使能并配置Tsensor采样模式。
 * @param  [in] mode Tsensor采样模式。
 * @param  [in] period Tsensor采样周期。
 * @endif
 */
typedef void (*hal_tsensor_set_samp_mode_t)(hal_tsensor_samp_mode_t mode, uint32_t period);

/**
 * @if Eng
 * @brief  set tsensor temperature high limit, only can be used in interrupt mode.
 * @param  [in]  id Low/high temp threshold ID.
 * @param  [in]  temp  The value to set the low/high temp of TSENSOR.
 * @else
 * @brief  设置TSENSOR传感器温度阈值，仅可在中断模式下使用。
 * @param  [in]  id 高低温度ID。
 * @param  [in]  temp  用于设置TSENSOR高低温度的阈值。
 * @endif
 */
typedef void (*hal_tsensor_set_temp_threshold_t)(hal_tsensor_set_temp_id_t id, int8_t temp);

/**
 * @if Eng
 * @brief  enable Interrupt for hal TSENSOR.
 * @param  [in] interrupt_type tsensor interrupt type.
 * @param  [in] value Control interrupt switch of the tsensor.
 * @else
 * @brief  HAL层TSENSOR的使能中断接口。
 * @param  [in] interrupt_type TSENSOR中断类型。
 * @param  [in] value 控制TSENSOR的中断开关。
 * @endif
 */
typedef void (*hal_tsensor_set_interrupt_t)(hal_tsensor_interript_type_t interrupt_type, bool value);

 /**
 * @if Eng
 * @brief  Register a callback asociated with a TSENSOR interrupt cause.
 * @param  [in]  interrupt_type tsensor interrupt type.
 * @param  [in]  callback  The interrupt callback to register.
 * @else
 * @brief  注册与TSENSOR中断原因关联的回调。
 * @param  [in]  interrupt_type TSENSOR中断类型。
 * @param  [in]  callback  寄存器的中断回调。
 * @endif
 */
typedef void (*hal_tsensor_set_callback_t)(hal_tsensor_interript_type_t interrupt_type,
                                           hal_tsensor_callback_t callback);

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
typedef errcode_t (*hal_tsensor_set_multilevel_value_t)(tsensor_multilevel_value_t level, int16_t temp);

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
typedef errcode_t (*hal_tsensor_set_multilevel_en_t)(tsensor_multilevel_en_t level,
                                                     hal_tsensor_callback_t callback);
#endif /* CONFIG_TSENSOR_MULTILEVEL */

#if defined(CONFIG_TSENSOR_TEMP_COMPENSATION)
/**
 * @if Eng
 * @brief  enable tsensor calibration.
 * @param  [in]  point_data: Temperature compensation point data.
 * @param  [in]  point_num: Number of temperature compensation points.
 * @else
 * @brief  HAL层TSENSOR的使能校准接口。
 * @param  [in]  point_data: 温度补偿点数据。
 * @param  [in]  point_num: 温度补偿点数量。
 * @endif
 */
typedef void (*hal_tsensor_enable_calibration_t)(hal_tsensor_calibration_point_t *point_data, int8_t point_num);
#endif /* CONFIG_TSENSOR_TEMP_COMPENSATION */

/**
 * @if Eng
 * @brief  refresh tsensor temperature.
 * @else
 * @brief  HAL层TSENSOR的温度刷新接口。
 * @endif
 */
typedef void (*hal_tsensor_refresh_temp_t)(void);

/**
 * @if Eng
 * @brief  get tsensor current temperature.
 * @param  [in] data temperature pointer, output parameter.
 * @retval ture: temperature is valid, get tsensor temperature success.
 * @retval false: temperature is invalid, get tsensor temperature faild.
 * @else
 * @brief  HAL层TSENSOR的获取当前温度接口。
 * @param  [in] data 温度指针，输出参数。
 * @retval ture  对: 温度有效，获取传感器温度成功。
 * @retval false 错：温度无效，获取传感器温度失败。
 * @endif
 */
typedef bool (*hal_tsensor_get_temp_t)(volatile int8_t *data);

/**
 * @if Eng
 * @brief  Interface between TSENSOR driver and TSENSOR hal.
 * @else
 * @brief  Driver层TSENSOR和HAL层TSENSOR的接口。
 * @endif
 */
typedef struct {
    hal_tsensor_init_t init;                                 /*!< @if Eng Init device interface.
                                                                  @else   HAL层TSENSOR的初始化接口 @endif */
    hal_tsensor_deinit_t deinit;                             /*!< @if Eng Deinit device interface.
                                                                  @else   HAL层TSENSOR去初始化接口 @endif */
    hal_tsensor_set_samp_mode_t set_samp_mode;               /*!< @if Eng Control tsensor set samping mode interface.
                                                                  @else   HAL层TSENSOR设置采样模式接口 @endif */
    hal_tsensor_set_temp_threshold_t set_temp_threshold;     /*!< @if Eng Control tsensor set temp limit interface.
                                                                  @else   HAL层TSENSOR设置温度上限接口 @endif */
    hal_tsensor_set_interrupt_t set_interrupt;               /*!< @if Eng Control device interrupt interface.
                                                                  @else   HAL层TSENSOR中断使能接口 @endif */
    hal_tsensor_set_callback_t set_callback;                 /*!< @if Eng Set Callback function.
                                                                  @else   HAL层TSENSOR设置回调函数接口 @endif */
#if defined(CONFIG_TSENSOR_MULTILEVEL)
    hal_tsensor_set_multilevel_value_t set_multilevel_value; /*!< @if Eng Control tsensor set multilevel threshold.
                                                                  @else   HAL层TSENSOR设置多级阈值接口 @endif */
    hal_tsensor_set_multilevel_en_t set_multilevel_en;       /*!< @if Eng Control tsensor set multilevel enable.
                                                                  @else   HAL层TSENSOR设置多级使能接口 @endif */
#endif /* CONFIG_TSENSOR_MULTILEVEL */
#if defined(CONFIG_TSENSOR_TEMP_COMPENSATION)
    hal_tsensor_enable_calibration_t enable_calibration;     /*!< @if Eng Control tsensor calibration interface.
                                                                  @else   HAL层TSENSOR设置校准使能接口 @endif */
#endif /* CONFIG_TSENSOR_TEMP_COMPENSATION */
    hal_tsensor_refresh_temp_t refresh_temp;                 /*!< @if Eng Control tsensor refresh temp interface.
                                                                  @else   HAL层TSENSOR温度刷新接口 @endif */
    hal_tsensor_get_temp_t get_temp;                         /*!< @if Eng Control tsensor get temp interface.
                                                                  @else   HAL层TSENSOR温度获取接口 @endif */
} hal_tsensor_funcs_t;

/**
 * @if Eng
 * @brief  Init the registers of Tsensor IPs.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  初始化寄存器基地址列表。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_tsensor_regs_init(void);

/**
 * @if Eng
 * @brief  Deinit the registers of Tsensor IPs.
 * @else
 * @brief  去初始化寄存器基地址列表。
 * @endif
 */
void hal_tsensor_regs_deinit(void);

/**
 * @if Eng
 * @brief  Register @ref hal_tsensor_funcs_t into the g_hal_tsensors_funcs.
 * @param  [out] funcs Interface between TSENSOR driver and TSENSOR hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  注册 @ref hal_tsensor_funcs_t 到 g_hal_tsensors_funcs 。
 * @param  [out] funcs Driver层TSENSOR和HAL层TSENSOR的接口实例。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_tsensor_register_funcs(hal_tsensor_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_tsensor_funcs_t from the g_hal_tsensors_funcs.
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  从g_hal_tsensors_funcs注销 @ref hal_tsensor_funcs_t 。
 * @return ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_tsensor_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between TSENSOR driver and TSENSOR hal, see @ref hal_tsensor_funcs_t.
 * @return Interface between TSENSOR driver and TSENSOR hal, see @ref hal_tsensor_funcs_t.
 * @else
 * @brief  获取Driver层tsensor和HAL层TSENSOR的接口实例，参考 @ref hal_tsensor_funcs_t 。
 * @return Driver层tsensor和HAL层TSENSOR的接口实例，参考 @ref hal_tsensor_funcs_t 。
 * @endif
 */
hal_tsensor_funcs_t *hal_tsensor_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif