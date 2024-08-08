/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 hal tsensor \n
 *
 * History: \n
 * 2023-02-28， Create file. \n
 */
#ifndef HAL_TSENSOR_V151_H
#define HAL_TSENSOR_V151_H

#include "hal_tsensor.h"
#include "hal_tsensor_v151_regs_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_tsensor_v151 TSENSOR V151
 * @ingroup  drivers_hal_tsensor
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
hal_tsensor_funcs_t *hal_tsensor_v151_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the timer interrupt request.
 * @else
 * @brief  Tsensor中断处理函数。
 * @endif
 */
void hal_tsensor_irq_handler(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
