/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 HAL pinctrl \n
 *
 * History: \n
 * 2022-09-09, Create file. \n
 */
#ifndef HAL_PINCTRL_V150_H
#define HAL_PINCTRL_V150_H

#include <stdint.h>
#include "hal_pinctrl_v150_regs_op.h"
#include "hal_pinctrl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pinctrl_v150 Pinctrl V150
 * @ingroup  drivers_hal_pinctrl
 * @{
 */

/**
 * @brief Get configuration functions of pins.
 * @return Configuration functions of pins. see @ref hal_pin_funcs_t
 */
hal_pin_funcs_t *hal_pin_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif