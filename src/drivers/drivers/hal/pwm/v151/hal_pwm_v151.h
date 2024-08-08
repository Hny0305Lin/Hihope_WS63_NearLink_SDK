/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 HAL pwm \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#ifndef HAL_PWM_V151_H
#define HAL_PWM_V151_H

#include "hal_pwm.h"
#include "hal_pwm_v151_regs_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pwm_v151 PWM V151
 * @ingroup  drivers_hal_pwm
 * @{
 */

/**
 * @if Eng
 * @brief  Get the instance of v151.
 * @return The instance of v151.
 * @else
 * @brief  获取v151实例。
 * @return v151实例。
 * @endif
 */
hal_pwm_funcs_t *hal_pwm_v151_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the PWM interrupt request.
 * @else
 * @brief  PWM中断处理函数。
 * @endif
 */
void hal_pwm_v151_irq_handler(void);


/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif