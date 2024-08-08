/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides WS63 HAL systick \n
 *
 * History: \n
 * 2023-04-06, Create file. \n
 */
#ifndef HAL_SYSTICK_WS63_H
#define HAL_SYSTICK_WS63_H

#include "hal_systick.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_systick_ws63 Systick
 * @ingroup  drivers_hal_systick
 * @{
 */

/**
 * @if Eng
 * @brief  Get interface between systick driver and systick hal.
 * @else
 * @brief  获取Driver层SYSTICK和HAL层SYSTICK的接口
 * @endif
 */
hal_systick_funcs_t *hal_systick_ws63_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif