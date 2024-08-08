/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides PIN driver api \n
 *
 * History: \n
 * 2022-08-25, Create file. \n
 */
#ifndef PINCTRL_H
#define PINCTRL_H

#include <stdint.h>
#include <stddef.h>
#include "errcode.h"
#include "pinctrl_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_pinctrl Pinctrl
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize the Pinctrl.
 * @note   This function should be called before other functions of this module are called.
 * @else
 * @brief  初始化Pinctrl.
 * @note   该函数应该在其他本模块函数被调用前执行。
 * @endif
 */
void uapi_pin_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the Pinctrl.
 * @else
 * @brief  去初始化Pinctrl.
 * @endif
 */
void uapi_pin_deinit(void);

/**
 * @if Eng
 * @brief  Set the PIN mode.
 * @param  [in] pin Definition of IO. see @ref pin_t.
 * @param  [in] mode Multiplexing mode. see @ref pin_mode_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置引脚复用模式。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @param  [in] mode 复用模式，参考 @ref pin_mode_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pin_set_mode(pin_t pin, pin_mode_t mode);

/**
 * @if Eng
 * @brief  Get the PIN mode.
 * @param  [in] pin Definition of IO, see @ref pin_t.
 * @return  Multiplexing mode, see @ref pin_mode_t.
 * @else
 * @brief  获取引脚复用模式。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @return 复用模式，参考 @ref pin_mode_t 。
 * @endif
 */
pin_mode_t uapi_pin_get_mode(pin_t pin);

/**
 * @if Eng
 * @brief  Set the drive-strength of PIN.
 * @param  [in] pin Definition of IO, see @ref pin_t.
 * @param  [in] ds Drive strength, see @ref pin_drive_strength_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置引脚驱动能力。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @param  [in] ds 驱动能力，参考 @ref pin_drive_strength_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pin_set_ds(pin_t pin, pin_drive_strength_t ds);

/**
 * @if Eng
 * @brief  Get the drive-strength of PIN.
 * @param  [in] pin Definition of IO, see @ref pin_t.
 * @return  Drive strength, see @ref pin_drive_strength_t.
 * @else
 * @brief  获取引脚驱动能力。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @return 驱动能力，参考 @ref pin_drive_strength_t 。
 * @endif
 */
pin_drive_strength_t uapi_pin_get_ds(pin_t pin);

/**
 * @if Eng
 * @brief  Set the pull-up/down status of PIN.
 * @param  [in] pin Definition of IO, see @ref pin_t.
 * @param  [in] pull_type pull-up/down type, see @ref pin_pull_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置引脚上下拉。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @param  [in] pull_type 上下拉类型，参考 @ref pin_pull_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pin_set_pull(pin_t pin, pin_pull_t pull_type);

/**
 * @if Eng
 * @brief  Get the pull-up/down status of PIN.
 * @param  [in] pin Definition of IO, see @ref pin_t.
 * @return  Pull-up/down status, see @ref pin_pull_t.
 * @else
 * @brief  获取引脚上下拉状态。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @return 上下拉状态，参考 @ref pin_pull_t 。
 * @endif
 */
pin_pull_t uapi_pin_get_pull(pin_t pin);

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
/**
 * @if Eng
 * @brief  Set the INPUT-Enabled status of PIN.
 * @param  [in] pin Definition of IO, see @ref pin_t.
 * @param  [in] ie INPUT-Enabled status, see @ref pin_input_enable_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置引脚输入使能状态。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @param  [in] ie 输入使能状态，参考 @ref pin_input_enable_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pin_set_ie(pin_t pin, pin_input_enable_t ie);

/**
 * @if Eng
 * @brief  Get the INPUT-Enabled status of PIN.
 * @param  [in] pin Definition of IO. see @ref pin_t.
 * @return INPUT-Enabled status. see @ref pin_input_enable_t.
 * @else
 * @brief  获取引脚输入使能状态。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @return 输入使能状态，参考 @ref pin_input_enable_t 。
 * @endif
 */
pin_input_enable_t uapi_pin_get_ie(pin_t pin);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

#if defined(CONFIG_PINCTRL_SUPPORT_ST)
/**
 * @if Eng
 * @brief  Set the Schmitt-Trigger status of PIN.
 * @param  [in] pin Definition of IO, see @ref pin_t.
 * @param  [in] st Schmitt-Trigger status, see @ref pin_schmitt_trigger_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置引脚施密特触发状态。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @param  [in] st 施密特触发状态，参考 @ref pin_schmitt_trigger_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pin_set_st(pin_t pin, pin_schmitt_trigger_t st);

/**
 * @if Eng
 * @brief  Get the Schmitt-Trigger status of PIN.
 * @param  [in] pin Definition of IO. see @ref pin_t
 * @return Schmitt-Trigger status. see @ref pin_schmitt_trigger_t
 * @else
 * @brief  获取引脚施密特触发状态。
 * @param  [in] pin io，参考 @ref pin_t 。
 * @return 施密特触发状态，参考 @ref pin_schmitt_trigger_t 。
 * @endif
 */
pin_schmitt_trigger_t uapi_pin_get_st(pin_t pin);
#endif /* CONFIG_PINCTRL_SUPPORT_ST */

#if defined(CONFIG_PINCTRL_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend the PINCTRL.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起PINCTRL。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pin_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  Resume the PINCTRL.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复PINCTRL。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pin_resume(uintptr_t arg);
#endif  /* CONFIG_PINCTRL_SUPPORT_LPM */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif