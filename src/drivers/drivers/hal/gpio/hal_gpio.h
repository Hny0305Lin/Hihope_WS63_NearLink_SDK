/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL GPIO \n
 *
 * History: \n
 * 2022-07-22, Create file. \n
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "platform_core.h"
#include "gpio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_gpio_api GPIO
 * @ingroup  drivers_hal_gpio
 * @{
 */

#define HAL_GPIO_MAX_CHANNEL GPIO_CHANNELS_NUM
#define HAL_GPIO_CHANNEL_OFFSET 32

/**
 * @if Eng
 * @brief  GPIO MAX PIN numbers.
 * @else
 * @brief  GPIO最大PIN管脚数量。
 * @endif
 */
#define PIN_MAX_NUMBER                    PIN_NONE

/**
 * @if Eng
 * @brief  GPIO Interrupt type.
 * @else
 * @brief  GPIO中断类型。
 * @endif
 */
#define GPIO_INTERRUPT_RISING_EDGE            0x00000001
#define GPIO_INTERRUPT_FALLING_EDGE           0x00000002
#define GPIO_INTERRUPT_LOW                    0x00000004
#define GPIO_INTERRUPT_HIGH                   0x00000008
#define GPIO_INTERRUPT_DEDGE                  (GPIO_INTERRUPT_RISING_EDGE | GPIO_INTERRUPT_FALLING_EDGE)

/**
 * @if Eng
 * @brief  GPIO directions.
 * @else
 * @brief  GPIO输入输出方向。
 * @endif
 */
typedef enum gpio_direction {
    GPIO_DIRECTION_INPUT,
    GPIO_DIRECTION_OUTPUT
} gpio_direction_t;

/**
 * @if Eng
 * @brief  GPIO LEVEL.
 * @else
 * @brief  GPIO高低电平。
 * @endif
 */
typedef enum gpio_level {
    GPIO_LEVEL_LOW,
    GPIO_LEVEL_HIGH
} gpio_level_t;

/**
 * @brief  GPIO INTTYPE.
 */
typedef enum gpio_intr_type {
    GPIO_LVL_LEVEL_SENSITIVE,     /*!< GPIO Interrupt is level sensitive   (used for input/output). */
    GPIO_LVL_EDGE_SENSITIVE,      /*!< GPIO Interrupt is edge sensitive  (used for input/output). */
} gpio_intr_type_t;

/**
 * @brief  GPIO POLARITY.
 */
typedef enum gpio_polarity {
    GPIO_ACTIVE_LOW,     /*!< GPIO is at a low logic level   (used for input/output). */
    GPIO_ACTIVE_HIGH,    /*!< GPIO is at a high logic level  (used for input/output). */
} gpio_polarity_t;

/**
 * @if Eng
 * @brief  GPIO both edge enable/disable.
 * @else
 * @brief  GPIO双边沿使能或去使能。
 * @endif
 */
typedef enum gpio_dedge {
    GPIO_DEDGE_DISABLED,        /*!< @if Eng GPIO single edge sensitive.
                                         @else   GPIO单边沿中断。 @endif */
    GPIO_DEDGE_ENABLED,         /*!< @if Eng GPIO dual edge sensitive.
                                         @else   GPIO双边沿中断。 @endif */
} gpio_dedge_t;

/**
 * @if Eng
 * @brief  Definition of the contorl ID of hal GPIO.
 * @else
 * @brief  GPIO控制ID定义。
 * @endif
 */
typedef enum hal_gpio_ctrl_id {
    GPIO_CTRL_TOGGLE = 0,                /*!< @if Eng Toggle the value of a GPIO previously claimed for output.
                                              @else   GPIO输出电平状态反转 @endif */
    GPIO_CTRL_ENABLE_INTERRUPT,          /*!< @if Eng enable interrupt for a specific PIN.
                                              @else   GPIO使能指定端口的中断 @endif */
    GPIO_CTRL_DISABLE_INTERRUPT,         /*!< @if Eng Disable interrupt for a specific PIN.
                                              @else   GPIO去使能指定端口的中断 @endif */
    GPIO_CTRL_CLEAR_INTERRUPT,           /*!< @if Eng Clear the interrupt for a pin configured as GPIO input.
                                              @else   GPIO清理中断 @endif */
    GPIO_CTRL_SUSPEND,                   /*!< @if Eng Suspend all of the GPIO channels.
                                              @else   挂起所有GPIO通道。 @endif */
    GPIO_CTRL_RESUME,                    /*!< @if Eng Suspend all of the GPIO channels.
                                              @else   挂起所有GPIO通道。 @endif */
    GPIO_CTRL_MAX
} hal_gpio_ctrl_id_t;

/**
 * @if Eng
 * @brief  Context of GPIO module.
 * @else
 * @brief  GPIO模块配置信息。
 * @endif
 */
typedef struct {
    pin_t pin_max_num;                          /*!< @if Eng Indicates the max number of PIN.
                                                     @else   最大PIN数。 @endif */
    pin_t ulp_pin_start;                        /*!< @if Eng Indicates the start pin of ulp.
                                                     @else   ULP的起始管脚。 @endif */
    pin_t ulp_pin_end;                          /*!< @if Eng Indicates the end pin of ulp.
                                                     @else   ULP的终止管脚。 @endif */
    bool is_ulp_enable;                         /*!< @if Eng Indicates the enabled state of ulp.
                                                     @else   ULP的使能状态。 @endif */
    bool is_irq_enable;                         /*!< @if Eng Indicates the enabled irq.
                                                     @else   中断的使能状态。 @endif */
    uint32_t irq_list[GPIO_CHANNELS_NUM];       /*!< @if Eng Indicates the list of irq index foreach channel.
                                                     @else   每个通道的中断号索引列表。 @endif */
    uint32_t claimed[GPIO_CHANNELS_NUM];        /*!< @if Eng Indicates which PIN have been claimed as GPIO.
                                                     @else   指示哪个PIN已被声明为GPIO。 @endif */
    uint32_t direction[GPIO_CHANNELS_NUM];      /*!< @if Eng Indicates if the PIN has been claimed as Input or Output.
                                                     @else   指示PIN是否已声明为输入或输出。 @endif */
} gpio_context_t;

/**
 * @if Eng
 * @brief  Typedef for the GPIO Callback.
 *         It returns the number of the PIN which originated the interrupt.
 * @param  [in]  pin PIN that caused the interrupt.
 * @param  [in]  param Parameter sent to callback.
 * @else
 * @brief  GPIO回调的类型定义，它返回发起中断的PIN的编号。
 * @param  [in]  pin 中断的PIN的编号.
 * @param  [in]  param 回调函数.
 * @endif
 */
typedef void (*gpio_callback_t)(pin_t pin, uintptr_t param);

/**
 * @if Eng
 * @brief  Context of GPIO HAL module.
 * @else
 * @brief  GPIO HAL层模块配置信息。
 * @endif
 */
typedef struct {
    pin_t pin_max_num;
    bool is_ulp_enable;
    pin_t ulp_pin_start;
    pin_t ulp_pin_end;
    gpio_channel_t ulp_pin_channel;
    gpio_callback_t callback[PIN_MAX_NUMBER];
} hal_gpio_context_t;

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
void hal_gpio_init(void);

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
void hal_gpio_deinit(void);

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
errcode_t hal_gpio_setdir(pin_t pin, gpio_direction_t dir);

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
gpio_direction_t hal_gpio_getdir(pin_t pin);

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
typedef errcode_t (*hal_gpio_ctrl_t)(pin_t pin, hal_gpio_ctrl_id_t id);

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
errcode_t hal_gpio_ctrl(pin_t pin, hal_gpio_ctrl_id_t id);

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
errcode_t hal_gpio_output(pin_t pin, gpio_level_t level);

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
gpio_level_t hal_gpio_get_outval(pin_t pin);

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
gpio_level_t hal_gpio_get_inval(pin_t pin);

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
errcode_t hal_gpio_register(pin_t pin, uint32_t trigger, gpio_callback_t callback, bool need_callback);

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
errcode_t hal_gpio_unregister(pin_t pin);

/**
 * @if Eng
 * @brief  Get the base address of specified GPIO registers.
 * @else
 * @brief  获取指定Channel GPIO寄存器的基地址。
 * @endif
 */
uintptr_t hal_gpio_base_addrs_get(uint32_t i);

/**
 * @if Eng
 * @brief  Init the GPIO which will set the base address of registers.
 * @else
 * @brief  初始化GPIO，设置寄存器的基地址。
 * @endif
 */
void hal_gpio_regs_init(void);

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
    }
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif