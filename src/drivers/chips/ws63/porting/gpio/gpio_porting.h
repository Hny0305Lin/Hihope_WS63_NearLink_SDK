/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides gpio port template \n
 *
 * History: \n
 * 2022-07-26， Create file. \n
 */
#ifndef GPIO_PORTING_H
#define GPIO_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "platform_core.h"
#include "platform_types.h"
#include "hal_gpio_v150_comm_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_gpio_v150 GPIO porting info.
 * @ingroup  drivers_port_gpio
 * @{
 */

/**
 * @brief  GPIO info definition of each channel and group. Developer should adapt GPIO info here.
 * @note   GPIO_CHANNEL_X_GROUP_NUM:           GPIO group number of channel X.
 *         GPIO_CHANNEL_X_GROUP_Y_PIN_NUM:     GPIO pin number in channel X group Y.
 *         GPIO_CHANNEL_X_GROUP_Y_CB_START_ID: GPIO callback start ID in channel X group Y. The callback functions
 *                                             of all GPIO pins are registered in the same array. The array length
 *                                             is equal to the number of all GPIO pins. The value starts from 0 and
 *                                             is accumulated based on the number of pins in each group.
 *         GPIO_CHANNEL_X_PIN_NUM:             GPIO number of channel X.
 *
 *         GPIO_PIN_NUM:                       GPIO sum number of all channel.
 */
#define GPIO_CHANNEL_0_GROUP_NUM            1
#define GPIO_CHANNEL_0_GROUP_0_PIN_NUM      8
#define GPIO_CHANNEL_0_GROUP_0_CB_START_ID  0
#define GPIO_CHANNEL_0_PIN_NUM              (GPIO_CHANNEL_0_GROUP_0_PIN_NUM)

#define GPIO_CHANNEL_1_GROUP_NUM            1
#define GPIO_CHANNEL_1_GROUP_0_PIN_NUM      8
#define GPIO_CHANNEL_1_GROUP_0_CB_START_ID  (GPIO_CHANNEL_0_GROUP_0_CB_START_ID + GPIO_CHANNEL_0_PIN_NUM)
#define GPIO_CHANNEL_1_PIN_NUM              (GPIO_CHANNEL_1_GROUP_0_PIN_NUM)

#define GPIO_CHANNEL_2_GROUP_NUM            1
#define GPIO_CHANNEL_2_GROUP_0_PIN_NUM      3
#define GPIO_CHANNEL_2_GROUP_0_CB_START_ID  (GPIO_CHANNEL_1_GROUP_0_CB_START_ID + GPIO_CHANNEL_1_PIN_NUM)
#define GPIO_CHANNEL_2_PIN_NUM              (GPIO_CHANNEL_2_GROUP_0_PIN_NUM)

#define GPIO_PIN_NUM (GPIO_CHANNEL_0_PIN_NUM + GPIO_CHANNEL_1_PIN_NUM + GPIO_CHANNEL_2_PIN_NUM)

#define GPIO_CHANNELS_NUM GPIO_CHANNEL_MAX_NUM

/**
 * @brief  Definition of GPIO Channel index.
 * @note   GPIO Channels number might be different for each chip and core.
 */
typedef enum gpio_channel {
    GPIO_CHANNEL_0 = 0,         /*!< GPIO Channel 0 */
    GPIO_CHANNEL_1,             /*!< GPIO Channel 1 */
    GPIO_CHANNEL_2,             /*!< GPIO Channel 2 */
    GPIO_CHANNEL_MAX_NUM
} gpio_channel_t;

/**
 * @brief  Get GPIO channel info.
 * @param  [in]  channel The channel id of GPIO.
 * @return GPIO group info of target channel. See @ref hal_gpio_channel_info_t
 */
hal_gpio_channel_info_t *gpio_porting_channel_info_get(uint32_t channel);

/**
 * @brief  Get GPIO group context of target channel and group.
 * @param  [in]  channel The channel id of GPIO.
 * @param  [in]  group The group id of GPIO.
 * @return GPIO group context of target channel and group. See @ref hal_gpio_group_context_t
 */
hal_gpio_group_context_t *gpio_porting_group_context_get(uint32_t channel, uint32_t group);

/**
 * @brief  Clean all GPIO context of target channel.
 * @param  [in]  channel The channel id of GPIO.
 * @param  [in]  group_num Group number of GPIO channel.
 */
void gpio_porting_channel_context_clean(uint32_t channel, uint32_t group_num);

/**
 * @brief  Get GPIO base addr of of target channel.
 * @param  [in]  channel The channel id of gpio.
 * @return GPIO base addr of of target channel.
 */
uintptr_t gpio_porting_base_addr_get(uint32_t channel);

/**
 * @brief  Enable ulp gpio interrupt, set ulp gpio clk as 32K.
 * @param  on True enable and set clk as 32K, false disable ulp gpio interrupt.
 */
void gpio_ulp_int_en(bool on);

void gpio_select_core(pin_t pin, cores_t core);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif