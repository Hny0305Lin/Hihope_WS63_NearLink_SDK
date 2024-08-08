/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V150 HAL GPIO common header. \n
 *
 * History: \n
 * 2023-06-01, Create file. \n
 */

#ifndef HAL_GPIO_V150_COMM_H
#define HAL_GPIO_V150_COMM_H

#include <stdint.h>
#include "common_def.h"
#include "securec.h"
#include "hal_gpio_v150_comm_def.h"
#include "hal_gpio.h"
#include "gpio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_gpio_v150_comm GPIO V150 Common Operation
 * @ingroup  drivers_hal_gpio
 * @{
 */

/**
 * @brief  Get GPIO callback list.
 * @return GPIO callback list. See @ref gpio_callback_t
 */
gpio_callback_t *hal_gpio_v150_callback_list_get(void);

/**
 * @brief  Get GPIO group info.
 * @param  [in]  channel The channel id of GPIO.
 * @param  [in]  group The group id of GPIO.
 * @return GPIO group info of target channel and group. See @ref hal_gpio_group_info_t
 */
static inline hal_gpio_group_info_t *hal_gpio_v150_group_info_get(uint32_t channel, uint32_t group)
{
    return &(gpio_porting_channel_info_get(channel)->group_list[group]);
}

/**
 * @brief  Get GPIO callback register state of target channel and group.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @return GPIO callback register state of target channel and group.
 */
static inline uint32_t hal_gpio_v150_callback_registered_get(uint32_t channel, uint32_t group)
{
    return gpio_porting_group_context_get(channel, group)->cb_registered;
}

/**
 * @brief  Set GPIO callback register state of target pin to True.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 */
static inline void hal_gpio_v150_callback_registered_set_true(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    gpio_porting_group_context_get(channel, group)->cb_registered |= bit(group_pin);
}

/**
 * @brief  Set GPIO callback register state of target pin to False.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 */
static inline void hal_gpio_v150_callback_registered_set_false(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    gpio_porting_group_context_get(channel, group)->cb_registered &= ~(bit(group_pin));
}

/**
 * @brief  Get GPIO callback of target pin.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return GPIO callback of target pin. See @ref gpio_callback_t
 */
static inline gpio_callback_t hal_gpio_v150_callback_get(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_v150_callback_list_get()[
        hal_gpio_v150_group_info_get(channel, group)->start_callback_id + group_pin];
}

/**
 * @brief  Set GPIO callback of target pin.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  callback GPIO callback to be set to target pin.
 */
static inline void hal_gpio_v150_callback_set(uint32_t channel, uint32_t group, uint32_t group_pin,
                                              gpio_callback_t callback)
{
    hal_gpio_v150_callback_list_get()[
        hal_gpio_v150_group_info_get(channel, group)->start_callback_id + group_pin] = callback;
}

/**
 * @brief  Clean all GPIO callback in the list.
 */
static inline void hal_gpio_v150_callback_list_clean(void)
{
    memset_s(hal_gpio_v150_callback_list_get(), sizeof(gpio_callback_t) * GPIO_PIN_NUM, 0,
        sizeof(gpio_callback_t) * GPIO_PIN_NUM);
}

/**
 * @brief  Get GPIO irq number of of target channel.
 * @param  [in]  channel The channel id of gpio.
 * @return GPIO irq number of of target channel.
 */
static inline uintptr_t hal_gpio_v150_irq_num_get(uint32_t channel)
{
    return gpio_porting_channel_info_get(channel)->irq_num;
}

/**
 * @brief  Get GPIO group number of of target channel.
 * @param  [in]  channel The channel id of gpio.
 * @return GPIO group number of of target channel.
 */
static inline uint32_t hal_gpio_v150_group_num_get(uint32_t channel)
{
    return gpio_porting_channel_info_get(channel)->group_num;
}

/**
 * @brief  Register IRQ number of GPIO.
 * @param  [in]  int_id IRQ number of GPIO.
 */
void hal_gpio_v150_register_irq(uint32_t int_id);

/**
 * @brief  Unregister IRQ number of GPIO.
 * @param  [in]  int_id IRQ number of GPIO.
 */
void hal_gpio_v150_unregister_irq(uint32_t int_id);

/**
 * @brief  Get GPIO pin info of target pin, including channel ID, group ID and group_pin ID.
 * @param  [in]  pin Pin number of target GPIO pin.
 * @param  [in]  *channel  The channel ID of target GPIO pin.
 * @param  [in]  *group The group ID of target GPIO pin.
 * @param  [in]  *group_pin The group_pin ID of target GPIO pin.
 * @return Errcode. See @ref errcode_t
 */
errcode_t hal_gpio_v150_pin_info_get(pin_t pin, uint32_t *channel, uint32_t *group, uint32_t *group_pin);

/**
 * @brief  Get GPIO pin ID of target pin.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return Pin number of target GPIO pin.
 */
uint32_t hal_gpio_v150_pin_id_get(uint32_t channel, uint32_t group, uint32_t group_pin);

/**
 * @brief  Register GPIO callback.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  cb Callback to be registered.
 * @return Errcode. See @ref errcode_t
 */
errcode_t hal_gpio_v150_register_cb(uint32_t channel, uint32_t group, uint32_t group_pin, gpio_callback_t cb);

/**
 * @brief  Unregister GPIO callback.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 */
void hal_gpio_v150_unregister_cb(uint32_t channel, uint32_t group, uint32_t group_pin);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
