/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides V150 HAL GPIO \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */

#ifndef HAL_GPIO_V150_H
#define HAL_GPIO_V150_H

#include "hal_gpio.h"
#include "hal_gpio_v150_regs_op.h"
#include "hal_gpio_v150_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_gpio_v150 GPIO V150
 * @ingroup  drivers_hal_gpio
 * @{
 */

/**
 * @if Eng
 * @brief  GPIO LEVEL.
 * @else
 * @brief  GPIO高低电平。
 * @endif
 */
typedef enum hal_gpio_level {
    HAL_GPIO_LEVEL_LOW,
    HAL_GPIO_LEVEL_HIGH,
} hal_gpio_level_t;

/**
 * @if Eng
 * @brief  GPIO directions.
 * @else
 * @brief  GPIO输入输出方向。
 * @endif
 */
typedef enum hal_gpio_direction {
    HAL_GPIO_DIRECTION_OUTPUT,
    HAL_GPIO_DIRECTION_INPUT
} hal_gpio_direction_t;

/**
 * @if Eng
 * @brief  GPIO ctrl mode.
 * @else
 * @brief  GPIO控制模式。
 * @endif
 */
typedef enum hal_gpio_ctrl_mode {
    HAL_GPIO_CTRL_MODE_SW,
    HAL_GPIO_CTRL_MODE_HW
} hal_gpio_ctrl_mode_t;

/**
 * @if Eng
 * @brief  GPIO interrupt enable.
 * @else
 * @brief  GPIO中断使能。
 * @endif
 */
typedef enum hal_gpio_intr_enable {
    HAL_GPIO_INTR_DISABLE,
    HAL_GPIO_INTR_ENABLE
} hal_gpio_intr_enable_t;

/**
 * @if Eng
 * @brief  GPIO interrupt mask.
 * @else
 * @brief  GPIO中断屏蔽。
 * @endif
 */
typedef enum hal_gpio_intr_mask {
    HAL_GPIO_INTR_UNMASK,
    HAL_GPIO_INTR_MASK
} hal_gpio_intr_mask_t;

/**
 * @if Eng
 * @brief  GPIO debounce enable/disable.
 * @else
 * @brief  GPIO去毛刺使能或去使能。
 * @endif
 */
typedef enum hal_gpio_debounce {
    HAL_GPIO_DEBOUNCE_DISABLED,     /*!< @if Eng GPIO debounce disabled.
                                         @else   GPIO去毛刺不使能。 @endif */
    HAL_GPIO_DEBOUNCE_ENABLED,      /*!< @if Eng GPIO debounce enabled.
                                         @else   GPIO去毛刺使能。 @endif */
} hal_gpio_debounce_t;

/**
 * @if Eng
 * @brief  GPIO debounce enable/disable.
 * @else
 * @brief  GPIO中断清除。
 * @endif
 */
typedef enum hal_gpio_intr_clr {
    HAL_GPIO_INTR_NOT_CLEAR,        /*!< @if Eng GPIO interrupt not clear.
                                         @else   GPIO中断不清除。 @endif */
    HAL_GPIO_INTR_CLEAR,            /*!< @if Eng GPIO interrupt clear.
                                         @else   GPIO中断清除。 @endif */
} hal_gpio_intr_clr_t;


/**
 * @if Eng
 * @brief  Handler of the GPIO interrupt request.
 * @else
 * @brief  GPIO中断处理函数。
 * @endif
 */
void hal_gpio_v150_irq_handler(void);

/**
 * @if Eng
 * @brief  Get the context of current module, see @ref gpio_context_t.
 * @return The context of GPIO module, see @ref gpio_context_t.
 * @else
 * @brief  获取当前模块的上下文描述，参考 @ref gpio_context_t 。
 * @return 当前模块的上下文描述，参考 @ref gpio_context_t 。
 * @endif
 */
gpio_context_t *gpio_v150_context_get(void);

/**
 * @if Eng
 * @brief  Get the HAL context of current module, see @ref hal_gpio_context_t.
 * @return The HAL context of GPIO module, see @ref hal_gpio_context_t.
 * @else
 * @brief  获取当前HAL层模块的上下文描述，参考 @ref hal_gpio_context_t 。
 * @return 当前HAL层模块的上下文描述，参考 @ref hal_gpio_context_t 。
 * @endif
 */
hal_gpio_context_t *hal_gpio_v150_context_get(void);

/**
 * @if Eng
 * @brief  HAL GPIO init interface.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层GPIO的初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void hal_gpio_v150_init(void);

/**
 * @if Eng
 * @brief  HAL GPIO deinit interface.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层GPIO去初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void hal_gpio_v150_deinit(void);

/**
 * @if Eng
 * @brief  HAL GPIO setdir device interface.
 * @param  [in]  pin PIN to use. see @ref pin_t.
 * @param  [in]  dir Input or output direction. see @ref gpio_direction_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层GPIO设置输入输出方向接口。
 * @param  [in]  pin io引脚， 参考 @ref pin_t 。
 * @param  [in]  dir 输入输出方向， 参考 @ref gpio_direction_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_gpio_v150_setdir(pin_t pin, gpio_direction_t dir);

/**
 * @if Eng
 * @brief  HAL GPIO setdir device interface.
 * @param  [in]  pin PIN to use. see @ref pin_t.
 * @retval GPIO input or output direction.
 * @else
 * @brief  HAL层GPIO获取输入输出方向接口。
 * @param  [in]  pin io引脚， 参考 @ref pin_t 。
 * @retval GPIO输入输出方向 。
 * @endif
 */
gpio_direction_t hal_gpio_v150_getdir(pin_t pin);

/**
 * @if Eng
 * @brief  HAL GPIO control interface.
 * @param  [in]  pin PIN to use. see @ref pin_t
 * @param  [in]  id ID of the GPIO control.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层GPIO控制接口。
 * @param  [in]  pin io引脚， 参考 @ref pin_t 。
 * @param  [in]  id GPIO控制请求ID。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_gpio_v150_ctrl(pin_t pin, hal_gpio_ctrl_id_t id);

/**
 * @if Eng
 * @brief  HAL GPIO output interface.
 * @param  [in]  pin PIN to use. see @ref pin_t.
 * @param  [in]  level  Set to HIGH or LOW the output of a GPIO previously claimed for output. see @ref gpio_level_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层GPIO的output接口 。
 * @param  [in]  pin io引脚， 参考 @ref pin_t 。
 * @param  [in]  level 设置output输出HIGH or LOW，参考 @ref gpio_level_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_gpio_v150_output(pin_t pin, gpio_level_t level);

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
gpio_level_t hal_gpio_v150_get_outval(pin_t pin);

/**
 * @if Eng
 * @brief  HAL GPIO get input level interface.
 * @param  [in]  pin PIN to use. see @ref pin_t.
 * @retval GPIO level, see @ref gpio_level_t.
 * @else
 * @brief  HAL层GPIO的input接口 。
 * @param  [in]  pin io引脚， 参考 @ref pin_t 。
 * @retval GPIO电平，参考 @ref gpio_level_t 。
 * @endif
 */
gpio_level_t hal_gpio_v150_get_inval(pin_t pin);

/**
 * @if Eng
 * @brief  HAL GPIO register a callback for the PIN.
 * @param  [in]  pin PIN to use. see @ref pin_t.
 * @param  [in]  trigger Trigger under which the callback will be invoked.
 * @param  [in]  callback Pointer to callback. see @ref gpio_callback_t.
 * @param  [in]  need_callback Whether callback is needed.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层GPIO的注册中断接口。
 * @param  [in]  pin io引脚， 参考 @ref pin_t 。
 * @param  [in]  trigger 触发回调的触发器。
 * @param  [in]  callback 指向回调的指针，参考 @ref gpio_callback_t 。
 * @param  [in]  need_callback 是否需要回调 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_gpio_v150_register(pin_t pin, uint32_t trigger, gpio_callback_t callback, bool need_callback);

/**
 * @if Eng
 * @brief  HAL GPIO unregister a callback for the PIN.
 * @param  [in]  pin PIN to use. see @ref pin_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层GPIO的注册中断接口。
 * @param  [in]  pin io引脚， 参考 @ref pin_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_gpio_v150_unregister(pin_t pin);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
