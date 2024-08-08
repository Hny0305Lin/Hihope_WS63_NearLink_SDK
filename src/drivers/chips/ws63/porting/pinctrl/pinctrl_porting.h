/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pinctrl port \n
 *
 * History: \n
 * 2022-08-25， Create file. \n
 */
#ifndef PINCTRL_PORTING_H
#define PINCTRL_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "securec.h"
#include "platform_core_rom.h"
#include "chip_io.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_pinctrl Pinctrl
 * @ingroup  drivers_port
 * @{
 */

#define HAL_PIO_PULL_NONE PIN_PULL_TYPE_DISABLE
#define HAL_PIO_FUNC_GPIO PIN_MODE_0

/**
 * @brief  Definition of mode-multiplexing.
 */
typedef enum {
    PIN_MODE_0        = 0,
    PIN_MODE_1        = 1,
    PIN_MODE_2        = 2,
    PIN_MODE_3        = 3,
    PIN_MODE_4        = 4,
    PIN_MODE_5        = 5,
    PIN_MODE_6        = 6,
    PIN_MODE_7        = 7,
    PIN_MODE_MAX      = 8
} pin_mode_t;

/**
 * @brief  Definition of drive-strength.
 */
typedef enum {
    PIN_DS_0 = 0,
    PIN_DS_1 = 1,
    PIN_DS_2 = 2,
    PIN_DS_3 = 3,
    PIN_DS_4 = 4,
    PIN_DS_5 = 5,
    PIN_DS_6 = 6,
    PIN_DS_7 = 7,
    PIN_DS_MAX = 8
} pin_drive_strength_t;

/**
 * @brief  Definition of pull-up/pull-down.
 */
typedef enum {
    PIN_PULL_TYPE_DISABLE   = 0,
    PIN_PULL_TYPE_DOWN      = 1,
    PIN_PULL_TYPE_STRONG_UP = 2,
    PIN_PULL_TYPE_UP        = 3,
    PIN_PULL_MAX            = 4
} pin_pull_t;

/**
 * @brief  Definition of input-enable.
 */
typedef enum {
    PIN_IE_DISABLE = 0,
    PIN_IE_ENABLE = 1,
    PIN_IE_MAX,
} pin_input_enable_t;

/**
 * @brief  Definition of schmitt-trigger.
 */
typedef enum {
    PIN_ST_DISABLE = 0,
    PIN_ST_ENABLE = 1,
    PIN_ST_MAX,
} pin_schmitt_trigger_t;

/**
 * @brief  Check whether the mode configured for the pin is valid.
 * @param  [in]  pin  The index of pins. see @ref pin_t
 * @param  [in]  mode The Multiplexing mode. see @ref pin_mode_t
 * @return The value 'true' indicates that the mode is valid and the value 'false' indicates that the mode is invalid.
 */
bool pin_check_mode_is_valid(pin_t pin, pin_mode_t mode);

/**
 * @brief  Register hal funcs objects into hal_pinctrl module.
 */
void pin_port_register_hal_funcs(void);

/**
 * @brief  Unregister hal funcs objects from hal_pinctrl module.
 */
void pin_port_unregister_hal_funcs(void);

/**
 * @}
 */

#define HAL_PIO_FUNC_INVALID        HAL_PIO_FUNC_MAX

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
