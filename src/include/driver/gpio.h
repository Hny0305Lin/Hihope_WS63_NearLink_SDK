/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides GPIO driver api \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */
#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "errcode.h"
#include "platform_core.h"
#include "hal_gpio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_gpio GPIO
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize the GPIO.
 * @note   This function should be called before any other on in the module.
 * @else
 * @brief  初始化GPIO。
 * @note   这个回调函数应该在所有函数之前执行。
 * @endif
 */
void uapi_gpio_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the GPIO.
 * @else
 * @brief  去初始化GPIO.
 * @endif
 */
void uapi_gpio_deinit(void);

/**
 * @if Eng
 * @brief  Set the GPIO direction.
 * @param  [in]  pin IO, see @ref pin_t.
 * @param  [in]  dir Input or output direction, see @ref gpio_direction_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置GPIO的输入输出方向函数。
 * @param  [in] pin IO， 参考 @ref pin_t 。
 * @param  [in] dir 输入输出方向， 参考 @ref gpio_direction_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_set_dir(pin_t pin, gpio_direction_t dir);

/**
 * @if Eng
 * @brief  Get the GPIO direction.
 * @param  [in]  pin IO, see @ref pin_t.
 * @retval GPIO Input or output direction.
 * @else
 * @brief  获取GPIO的输入输出方向函数。
 * @param  [in] pin IO， 参考 @ref pin_t 。
 * @retval GPIO输入输出方向 。
 * @endif
 */
gpio_direction_t uapi_gpio_get_dir(pin_t pin);

/**
 * @if Eng
 * @brief  Set the GPIO level.
 * @param  [in]  pin IO, see @ref pin_t.
 * @param  [in]  level The output as high or low, see @ref gpio_level_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置GPIO的输出状态。
 * @param  [in]  pin IO， 参考 @ref pin_t 。
 * @param  [in]  level GPIO 输出设置为高或低， 参考 @ref gpio_level_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_set_val(pin_t pin, gpio_level_t level);

/**
 * @if Eng
 * @brief  HAL GPIO get output value interface.
 * @param  [in]  pin PIN to use. see @ref pin_t.
 * @retval GPIO output level value(high or low).
 * @else
 * @brief  HAL层获取GPIO的output值接口 。
 * @param  [in]  pin io引脚， 参考 @ref pin_t 。
 * @retval GPIO输出电平值 。
 * @endif
 */
gpio_level_t uapi_gpio_get_output_val(pin_t pin);

/**
 * @if Eng
 * @brief  Read the GPIO value.
 * @param  [in]  pin IO, see @ref pin_t.
 * @retval GPIO input level value.
 * @else
 * @brief  读取GPIO的输入状态。
 * @param  [in]  pin IO， 参考 @ref pin_t 。
 * @retval GPIO输入电平值。
 * @endif
 */
gpio_level_t uapi_gpio_get_val(pin_t pin);

/**
 * @if Eng
 * @brief  Toggle the GPIO.
 * @param  [in]  pin IO, see @ref pin_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  翻转输出GPIO电平状态.
 * @param  [in]  pin IO， 参考 @ref pin_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_toggle(pin_t pin);

/**
 * @if Eng
 * @brief  HAL GPIO set isr mode.
 * @param  [in]  pin PIN to use. see @ref pin_t.
 * @param  [in]  trigger Interrupt type of the GPIO:
 *                       - 1 : Rising edge interrupt
 *                       - 2 : Falling edge interrupt
 *                       - 3 : Doublie edge interrupt
 *                       - 4 : Low level interrupt
 *                       - 8 : High level interrupt
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层GPIO设置中断模式。
 * @param  [in]  pin io引脚， 参考 @ref pin_t 。
 * @param  [in]  trigger GPIO中断类型：
 *                       - 1 : 上升沿中断
 *                       - 2 : 下降沿中断
 *                       - 3 : 双边沿中断
 *                       - 4 : 低电平中断
 *                       - 8 : 高电平中断
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_set_isr_mode(pin_t pin, uint32_t trigger);

/**
 * @if Eng
 * @brief  Clear the GPIO interrupt for the pin.
 * @param  [in]  pin IO, see @ref pin_t.
 * @param  [in]  trigger Interrupt type of the GPIO:
 *                       - 1 : Rising edge interrupt
 *                       - 2 : Falling edge interrupt
 *                       - 3 : Doublie edge interrupt
 *                       - 4 : Low level interrupt
 *                       - 8 : High level interrupt
 * @param  [in]  callback Pointer to callback, see @ref gpio_callback_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册GPIO的中断。
 * @param  [in]  pin IO， 参考 @ref pin_t 。
 * @param  [in]  trigger GPIO中断类型：
 *                       - 1 : 上升沿中断
 *                       - 2 : 下降沿中断
 *                       - 3 : 双边沿中断
 *                       - 4 : 低电平中断
 *                       - 8 : 高电平中断
 * @param  [in]  callback 指向回调的指针， 参考 @ref gpio_callback_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_register_isr_func(pin_t pin, uint32_t trigger, gpio_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister the GPIO interrupt for the pin.
 * @param  [in]  pin IO, see @ref pin_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去注册GPIO的中断。
 * @param  [in]  pin IO， 参考 @ref pin_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_unregister_isr_func(pin_t pin);

/**
 * @if Eng
 * @brief  Enable the GPIO interrupt for the pin.
 * @param  [in]  pin IO, see @ref pin_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  使能GPIO指定端口的中断。
 * @param  [in]  pin IO， 参考 @ref pin_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_enable_interrupt(pin_t pin);

/**
 * @if Eng
 * @brief  Disable GPIO interrupt for the pin.
 * @param  [in]  pin IO, see @ref pin_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去使能GPIO指定端口的中断。
 * @param  [in]  pin IO， 参考 @ref pin_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_disable_interrupt(pin_t pin);

/**
 * @if Eng
 * @brief  Clear GPIO interrupt for the pin.
 * @param  [in]  pin IO, see @ref pin_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  清除GPIO指定端口的中断。
 * @param  [in]  pin IO， 参考 @ref pin_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_clear_interrupt(pin_t pin);

#if defined(CONFIG_GPIO_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend all of the GPIO channels.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起所有GPIO通道。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  Resume all of the GPIO channels.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复所有GPIO通道。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_gpio_resume(uintptr_t arg);
#endif

#if defined(CONFIG_GPIO_SELECT_CORE)
/**
 * @if Eng
 * @brief  GPIO select core.
 * @else
 * @brief  GPIO选择核心。
 * @endif
 */
void uapi_gpio_select_core(pin_t pin, cores_t core);
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