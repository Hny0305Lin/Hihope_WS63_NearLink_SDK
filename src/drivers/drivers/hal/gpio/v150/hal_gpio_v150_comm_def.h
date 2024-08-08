/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V150 HAL GPIO common info definition header. \n
 *
 * History: \n
 * 2023-06-01, Create file. \n
 */

#ifndef HAL_GPIO_V150_COMM_DEF_H
#define HAL_GPIO_V150_COMM_DEF_H

#include <stdint.h>
#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_gpio_v150_comm_def GPIO V150 Common Info Definition
 * @ingroup  drivers_hal_gpio
 * @{
 */

/**
 * @brief  GPIO group context definition.
 */
typedef struct hal_gpio_group_context {
    uint32_t cb_registered;             /*!< Indicates whether callback is registered for each GPIO in the group. */
} hal_gpio_group_context_t;

/**
 * @brief  GPIO group info definition.
 */
typedef struct hal_gpio_group_info {
    uint32_t start_pin_id;              /*!< The start ID of GPIO in this group. */
    uint32_t pin_num;                   /*!< GPIO number in this group. */
    uint32_t start_callback_id;         /*!< The start ID of GPIO callback in g_hal_gpio_callback_list. */
} hal_gpio_group_info_t;

/**
 * @brief  GPIO channel info definition.
 */
typedef struct hal_gpio_channel_info {
    uint32_t start_pin_id;              /*!< The start ID of GPIO in this channel. */
    uint32_t pin_num;                   /*!< GPIO number in this channel. */
    uint32_t group_num;                 /*!< GPIO group number in this channel. */
    uint32_t irq_num;                   /*!< IRQ number of this GPIO channel. */
    uintptr_t base_addr;                /*!< Register base addr of this GPIO channel. */
    hal_gpio_group_info_t *group_list;  /*!< GPIO group info list. See @ref hal_gpio_group_info_t */
} hal_gpio_channel_info_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
