/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 HAL pinctrl register operation api \n
 *
 * History: \n
 * 2022-09-09, Create file. \n
 */
#ifndef HAL_PINCTRL_V150_REGS_OP_H
#define HAL_PINCTRL_V150_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "pinctrl_porting.h"
#include "hal_pinctrl_v150_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pinctrl_v150_regs_def Pinctrl V150 Regs Operation
 * @ingroup  drivers_hal_pinctrl
 * @{
 */

extern pinctrl_config_t *g_pinctrl_regs[PIN_MAX_NUMBER];

/**
 * @brief  Set the value of @ref pinctrl_config.mode.
 * @param  [in]  pin The index of io. @ref pin_t
 * @param  [in]  mode The value of @ref pinctrl_config.mode
 */
static inline void hal_pinctrl_v150_set_mode(pin_t pin, pin_mode_t mode)
{
    g_pinctrl_regs[pin]->mode = (uint16_t)mode;
}

/**
 * @brief  Get the value of @ref pinctrl_config.mode.
 * @param  [in]  pin The index of io. @ref pin_t
 * @return The value of @ref pinctrl_config.mode.
 */
static inline pin_mode_t hal_pinctrl_v150_get_mode(pin_t pin)
{
    return (pin_mode_t)g_pinctrl_regs[pin]->mode;
}

/**
 * @brief  Set the value of @ref pinctrl_config.ds.
 * @param  [in]  pin The index of io. @ref pin_t
 * @param  [in]  ds The value of @ref pinctrl_config.ds
 */
static inline void hal_pinctrl_v150_set_ds(pin_t pin, pin_drive_strength_t ds)
{
    g_pinctrl_regs[pin]->ds = (uint16_t)ds;
}

/**
 * @brief  Get the value of @ref pinctrl_config.ds.
 * @param  [in]  pin The index of io. @ref pin_t
 * @return The value of @ref pinctrl_config.ds.
 */
static inline pin_drive_strength_t hal_pinctrl_v150_get_ds(pin_t pin)
{
    return (pin_drive_strength_t)g_pinctrl_regs[pin]->ds;
}

/**
 * @brief  Set the value of @ref pinctrl_config.pull.
 * @param  [in]  pin The index of io. @ref pin_t
 * @param  [in]  pull The value of @ref pinctrl_config.pull
 */
static inline void hal_pinctrl_v150_set_pull(pin_t pin, pin_pull_t pull)
{
    g_pinctrl_regs[pin]->pull = pull;
}

/**
 * @brief  Get the value of @ref pinctrl_config.pull.
 * @param  [in]  pin The index of io. @ref pin_t
 * @return The value of @ref pinctrl_config.pull.
 */
static inline pin_pull_t hal_pinctrl_v150_get_pull(pin_t pin)
{
    return (pin_pull_t)g_pinctrl_regs[pin]->pull;
}

/**
 * @brief  Init register addresses for all pins.
 */
void hal_pinctrl_regs_init(void);

/**
 * @brief  Deinit register addresses for all pins.
 */
void hal_pinctrl_regs_deinit(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif