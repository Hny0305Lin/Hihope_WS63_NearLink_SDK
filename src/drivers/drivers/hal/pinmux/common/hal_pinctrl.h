/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides HAL pinctrl \n
 *
 * History: \n
 * 2022-08-25, Create file. \n
 */
#ifndef HAL_PINCTRL_H
#define HAL_PINCTRL_H

#include <stdint.h>
#include "errcode.h"
#include "pinctrl_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pinctrl_api Pinctrl
 * @ingroup  drivers_hal_pinctrl
 * @{
 */

/**
 * @if Eng
 * @brief  Set mode for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @param  [in] mode Multiplexing mode. see @ref pin_mode_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层pin设置复用模式。
 * @param  [in] pin io， 参考 @ref pin_t
 * @param  [in] mode 复用模式。参考 @ref pin_mode_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_pin_set_mode)(pin_t pin, pin_mode_t mode);

/**
 * @if Eng
 * @brief  Get mode for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @return Multiplexing mode. see @ref pin_mode_t
 * @else
 * @brief  HAL层pin获取复用模式。
 * @param  [in] pin io， 参考 @ref pin_t
 * @return 复用模式。参考 @ref pin_mode_t
 * @endif
 */
typedef pin_mode_t (*hal_pin_get_mode)(pin_t pin);

/**
 * @if Eng
 * @brief  Set drive-strength for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @param  [in] ds Drive strength. see @ref pin_drive_strength_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层pin设置驱动能力。
 * @param  [in] pin io. 参考 @ref pin_t
 * @param  [in] ds 驱动能力。参考 @ref pin_drive_strength_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_pin_set_ds)(pin_t pin, pin_drive_strength_t ds);

/**
 * @if Eng
 * @brief  Get drive-strength for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @return Drive strength. see @ref pin_drive_strength_t
 * @else
 * @brief  HAL层pin获取驱动能力。
 * @param  [in] pin io. 参考 @ref pin_t
 * @return 驱动能力。参考 @ref pin_drive_strength_t
 * @endif
 */
typedef pin_drive_strength_t (*hal_pin_get_ds)(pin_t pin);

/**
 * @if Eng
 * @brief  Set pull-up/down for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @param  [in] pull_type pull-up/down type. see @ref pin_pull_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层pin设置上下拉。
 * @param  [in] pin io， 参考 @ref pin_t
 * @param  [in] pull_type 上下拉类型。参考 @ref pin_pull_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_pin_set_pull)(pin_t pin, pin_pull_t pull_type);

/**
 * @if Eng
 * @brief  Get pull-up/down status for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @return pull-up/down status. see @ref pin_pull_t
 * @else
 * @brief  HAL层pin获取上下拉状态。
 * @param  [in] pin io， 参考 @ref pin_t
 * @return 上下拉状态。参考 @ref pin_pull_t
 * @endif
 */
typedef pin_pull_t (*hal_pin_get_pull)(pin_t pin);

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
/**
 * @if Eng
 * @brief  Set INPUT-Enabled status for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @param  [in] ie INPUT-Enabled status. see @ref pin_input_enable_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层pin设置输入使能状态。
 * @param  [in] pin io， 参考 @ref pin_t
 * @param  [in] ie 输入使能状态。参考 @ref pin_input_enable_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_pin_set_ie)(pin_t pin, pin_input_enable_t ie);

/**
 * @if Eng
 * @brief  Get INPUT-Enabled status for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @return INPUT-Enabled status. see @ref pin_input_enable_t
 * @else
 * @brief  HAL层pin获取输入使能状态。
 * @param  [in] pin io， 参考 @ref pin_t
 * @return 输入使能状态。参考 @ref pin_input_enable_t
 * @endif
 */
typedef pin_input_enable_t (*hal_pin_get_ie)(pin_t pin);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

#if defined(CONFIG_PINCTRL_SUPPORT_ST)
/**
 * @if Eng
 * @brief  Set schmitt-trigger status for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @param  [in] st Schmitt-trigger status. see @ref pin_schmitt_trigger_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层pin设置施密特触发状态。
 * @param  [in] pin io， 参考 @ref pin_t
 * @param  [in] st 施密特触发状态。参考 @ref pin_schmitt_trigger_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_pin_set_st)(pin_t pin, pin_schmitt_trigger_t st);

/**
 * @if Eng
 * @brief  Get schmitt-trigger status for hal pin.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @return Schmitt-trigger status. see @ref pin_schmitt_trigger_t
 * @else
 * @brief  HAL层pin获取施密特触发状态。
 * @param  [in] pin io， 参考 @ref pin_t
 * @return 施密特触发状态。参考 @ref pin_schmitt_trigger_t
 * @endif
 */
typedef pin_schmitt_trigger_t (*hal_pin_get_st)(pin_t pin);
#endif /* CONFIG_PINCTRL_SUPPORT_ST */


typedef struct {
    hal_pin_set_mode set_mode;      /*!< @if Eng Set multiplexing mode interface.
                                         @else   设置复用模式接口。 @endif */
    hal_pin_get_mode get_mode;      /*!< @if Eng Get mode interface.
                                         @else   获取复用模式接口 @endif */
    hal_pin_set_ds   set_ds;        /*!< @if Eng Set drive strength interface.
                                         @else   设置驱动能力接口。 @endif */
    hal_pin_get_ds   get_ds;        /*!< @if Eng Get drive strength interface.
                                         @else   获取驱动能力接口。 @endif */
    hal_pin_set_pull set_pull;      /*!< @if Eng Set pull-up/down interface.
                                         @else   获取上下拉接口。 @endif */
    hal_pin_get_pull get_pull;      /*!< @if Eng Get pull-up/down interface.
                                         @else   获取上下拉接口。 @endif */
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    hal_pin_set_ie   set_ie;        /*!< @if Eng Set input enable interface.
                                         @else   设置输入使能接口。 @endif */
    hal_pin_get_ie   get_ie;        /*!< @if Eng Get input enable interface.
                                         @else   获取输入使能接口。 @endif */
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
#if defined(CONFIG_PINCTRL_SUPPORT_ST)
    hal_pin_set_st   set_st;        /*!< @if Eng Set schmitt trigger interface.
                                         @else   设置施密特触发接口。 @endif */
    hal_pin_get_st   get_st;        /*!< @if Eng Get schmitt trigger interface.
                                         @else   获取施密特触发接口。 @endif */
#endif /* CONFIG_PINCTRL_SUPPORT_ST */
} hal_pin_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_pin_funcs_t into the g_hal_pins_funcs.
 * @param  [in]  funcs Interface between pin driver and pin hal.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_pin_funcs_t 到 g_hal_pins_funcs
 * @param  [in]  funcs Driver层pin和HAL层pin的接口实例。
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_pin_register_funcs(hal_pin_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_pin_funcs_t from the g_hal_pins_funcs.
 * @return ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  g_hal_pins_funcs @ref hal_pin_funcs_t
 * @return ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_pin_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between pin driver and pin hal, see @ref hal_pin_funcs_t
 * @return Interface between pin driver and pin hal, see @ref hal_pin_funcs_t
 * @else
 * @brief  获取Driver层pin和HAL层pin的接口实例，参考 @ref hal_pin_funcs_t
 * @return Driver层pin和HAL层pin的接口实例，参考 @ref hal_pin_funcs_t
 * @endif
 */
hal_pin_funcs_t *hal_pin_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
