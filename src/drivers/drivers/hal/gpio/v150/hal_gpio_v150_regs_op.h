/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides V150 gpio register operation api \n
 *
 * History: \n
 * 2022-07-27， Create file. \n
 */
#ifndef HAL_GPIO_V150_REGS_OP_H
#define HAL_GPIO_V150_REGS_OP_H

#include <stdint.h>
#include "common_def.h"
#include "hal_gpio_v150_regs_def.h"
#include "gpio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_gpio_v150_regs_op GPIO V150 Regs Operation
 * @ingroup  drivers_hal_gpio
 * @{
 */

/** sets the bit y in x to the value z */
#define hal_gpio_set_bit(x, y, z) (((uint32_t)(x) & ~bit(y)) | (((uint32_t)(z) & 1U) << (uint32_t)(y)))

/** Toggle the bit y in x */
#define hal_gpio_toggle_bit(x, y) (((x) & ~bit(y)) | ((~(x)) & bit(y)))

/** Read bit y in x */
#define hal_gpio_read_bit(x, y) (((uint32_t)(x) >> (uint32_t)(y)) & 1U)

extern uintptr_t g_gpios_regs[GPIO_CHANNEL_MAX_NUM];

/**
 * @brief  Get GPIO regs struct of target channel.
 * @param  [in]  channel The channel id of gpio.
 * @return GPIO regs struct of target channel. See @ref gpio_v150_regs_t
 */
static inline gpio_v150_regs_t *gpios_v150_regs(uint32_t channel)
{
    return (gpio_v150_regs_t *)g_gpios_regs[channel];
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_sw_out.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_sw_out
 */
static inline void hal_gpio_gpio_sw_out_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_out =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_out, group_pin, val);
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_sw_out.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return The value of @ref gpio_info_regs.gpio_sw_out.
 */
static inline uint32_t hal_gpio_gpio_sw_out_get_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_read_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_out, group_pin);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_sw_out.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 */
static inline void hal_gpio_gpio_sw_out_toggle_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_out =
        hal_gpio_toggle_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_out, group_pin);
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_sw_oen.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return The value of @ref gpio_info_regs.gpio_sw_oen.
 */
static inline uint32_t hal_gpio_gpio_sw_oen_get_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_read_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_oen, group_pin);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_sw_oen.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_sw_oen.
 */
static inline void hal_gpio_gpio_sw_oen_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_oen =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_oen, group_pin, val);
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_sw_ctl.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return The value of @ref gpio_info_regs.gpio_sw_ctl.
 */
static inline uint32_t hal_gpio_gpio_sw_ctl_get_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_read_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_ctl, group_pin);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_sw_ctl.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_sw_ctl.
 */
static inline void hal_gpio_gpio_sw_ctl_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_ctl =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_sw_ctl, group_pin, val);
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_int_en.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return The value of @ref gpio_info_regs.gpio_int_en.
 */
static inline uint32_t hal_gpio_gpio_int_en_get_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_read_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_en, group_pin);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_int_en.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_int_en.
 */
static inline void hal_gpio_gpio_int_en_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_en =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_en, group_pin, val);
}

/**
 * @brief  Disable all interrupt of target GPIO channel and group.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 */
static inline void hal_gpio_gpio_int_en_disable_all(uint32_t channel, uint32_t group)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_en = 0x0;
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_int_mask.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return The value of @ref gpio_info_regs.gpio_int_en.
 */
static inline uint32_t hal_gpio_gpio_int_mask_get_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_read_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_mask, group_pin);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_int_mask.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_int_en.
 */
static inline void hal_gpio_gpio_int_mask_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_mask =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_mask, group_pin, val);
}

/**
 * @brief  Unmask all interrupt of target GPIO channel and group.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 */
static inline void hal_gpio_gpio_int_mask_unmask_all(uint32_t channel, uint32_t group)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_mask = 0x0;
}

/**
 * @brief  Mask all interrupt of target GPIO channel and group.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 */
static inline void hal_gpio_gpio_int_mask_mask_all(uint32_t channel, uint32_t group)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_mask = 0xFFFFFFFF;
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_int_type.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return The value of @ref gpio_info_regs.gpio_int_type.
 */
static inline uint32_t hal_gpio_gpio_int_type_get_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_read_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_type, group_pin);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_int_type.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_int_type.
 */
static inline void hal_gpio_gpio_int_type_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_type =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_type, group_pin, val);
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_int_polarity.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return The value of @ref gpio_info_regs.gpio_int_polarity.
 */
static inline uint32_t hal_gpio_gpio_int_polarity_get_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_read_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_polarity, group_pin);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_int_polarity.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_int_polarity.
 */
static inline void hal_gpio_v150_gpio_int_polarity_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin,
                                                           uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_polarity =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_polarity, group_pin, val);
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_int_dedge.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return The value of @ref gpio_info_regs.gpio_int_dedge.
 */
static inline uint32_t hal_gpio_gpio_int_dedge_get_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_read_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_dedge, group_pin);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_int_dedge.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_int_dedge.
 */
static inline void hal_gpio_gpio_int_dedge_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_dedge =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_dedge, group_pin, val);
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_int_debounce.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @return The value of @ref gpio_info_regs.gpio_int_debounce.
 */
static inline uint32_t hal_gpio_gpio_int_debounce_get_bit(uint32_t channel, uint32_t group, uint32_t group_pin)
{
    return hal_gpio_read_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_debounce, group_pin);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_int_debounce.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_int_debounce.
 */
static inline void hal_gpio_gpio_int_debounce_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin,
                                                      uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_debounce =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_debounce, group_pin, val);
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_int_raw.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @return The value of @ref gpio_info_regs.gpio_int_raw.
 */
static inline uint32_t hal_gpio_gpio_int_raw_get_data(uint32_t channel, uint32_t group)
{
    return gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_raw;
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_intr.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @return The value of @ref gpio_info_regs.gpio_intr.
 */
static inline uint32_t hal_gpio_gpio_intr_get_data(uint32_t channel, uint32_t group)
{
    return gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_intr;
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_int_eoi.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_int_eoi.
 */
static inline void hal_gpio_gpio_int_eoi_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_eoi =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_eoi, group_pin, val);
}

/**
 * @brief  Clean all interrupt status of target GPIO channel and group.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 */
static inline void hal_gpio_gpio_int_eoi_clr_all(uint32_t channel, uint32_t group)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_eoi = 0xFFFFFFFF;
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_data_set.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_data_set.
 */
static inline void hal_gpio_gpio_data_set_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_data_set =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_data_set, group_pin, val);
}

/**
 * @brief  Set the value of @ref gpio_info_regs.gpio_data_clr.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @param  [in]  group_pin The bit of gpio in group.
 * @param  [in]  val The value of @ref gpio_info_regs.gpio_data_clr.
 */
static inline void hal_gpio_gpio_data_clr_set_bit(uint32_t channel, uint32_t group, uint32_t group_pin, uint32_t val)
{
    gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_data_clr =
        hal_gpio_set_bit(gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_data_clr, group_pin, val);
}

/**
 * @brief  Get the value of @ref gpio_info_regs.gpio_int_en.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 * @return The value of @ref gpio_info_regs.gpio_int_en.
 */
static inline uint32_t hal_gpio_gpio_get_int_en(uint32_t channel, uint32_t group)
{
    return gpios_v150_regs(channel)->gpio_group_cfg[group].gpio_int_en;
}

/**
 * @brief  Init all GPIO interrupt relative regs. Disable, mask and clean all interruption status.
 * @param  [in]  channel The channel id of gpio.
 * @param  [in]  group The group id of gpio.
 */
void hal_gpio_v150_intr_rebase(uint32_t channel, uint32_t group);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
