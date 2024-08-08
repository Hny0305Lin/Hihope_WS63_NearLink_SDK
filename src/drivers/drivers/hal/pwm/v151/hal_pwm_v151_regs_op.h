/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 PWM register operation api \n
 * Author: HiSilicon (Shanghai) Technologies Co., Ltd. \n
 * History: \n
 * 2022-12-9， Create file. \n
 */

#ifndef HAL_PWM_V151_REGS_OP_H
#define HAL_PWM_V151_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "hal_pwm_v151_regs_def.h"
#include "pwm_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pwm_v151_regs_op PWM V151 Regs Operation
 * @ingroup  drivers_hal_pwm
 * @{
 */

extern uintptr_t g_pwm_regs;

/**
 * @brief  Set the value of @ref pwm_sel_data_t.pwm_sel_i.
 * @param  [in]  group The value of @ref pwm_v151_regs_t.pwm_v151_sel.
 * @param  [in]  val The value of @ref pwm_sel_data_t.pwm_sel_i.
 */
static inline void hal_pwm_sel_set_pwm_sel_i(pwm_v151_group_t group, uint16_t val)
{
    pwm_sel_data_t sel;
    sel.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_v151_sel[group].pwm_sel;
    sel.b.pwm_sel_i = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_v151_sel[group].pwm_sel = sel.d32;
}

/**
 * @brief  Get the value of @ref pwm_sel_data_t.pwm_sel_i.
 * @param  [in]  group The value of @ref pwm_v151_regs_t.pwm_v151_sel.
 * @return The value of @ref pwm_sel_data_t.pwm_sel_i.
 */
static inline uint16_t hal_pwm_sel_get_pwm_sel_i(pwm_v151_group_t group)
{
    pwm_sel_data_t sel;
    sel.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_v151_sel[group].pwm_sel;
    return sel.b.pwm_sel_i;
}

/**
 * @brief  Set the value of @ref pwm_startclrcnt_en_data_t.pwm_startclrcnt_en_i.
 * @param  [in]  group The value of @ref pwm_v151_regs_t.pwm_v151_sel.
 * @param  [in]  val The value of @ref pwm_startclrcnt_en_data_t.pwm_startclrcnt_en_i.
 */
static inline void hal_pwm_startclrcnt_en_set_pwm_startclrcnt_en_i(pwm_v151_group_t group, uint32_t val)
{
    pwm_startclrcnt_en_data_t startclrcnt_en;
    startclrcnt_en.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_v151_sel[group].pwm_startclrcnt_en;
    startclrcnt_en.b.pwm_startclrcnt_en_i = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_v151_sel[group].pwm_startclrcnt_en = startclrcnt_en.d32;
}

/**
 * @brief  Get the value of @ref pwm_startclrcnt_en_data_t.pwm_startclrcnt_en_i.
 * @param  [in]  group The value of @ref pwm_v151_regs_t.pwm_v151_sel.
 * @return The value of @ref pwm_startclrcnt_en_data_t.pwm_startclrcnt_en_i.
 */
static inline uint32_t hal_pwm_startclrcnt_en_get_pwm_startclrcnt_en_i(pwm_v151_group_t group)
{
    pwm_startclrcnt_en_data_t startclrcnt_en;
    startclrcnt_en.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_v151_sel[group].pwm_startclrcnt_en;
    return startclrcnt_en.b.pwm_startclrcnt_en_i;
}

/**
 * @brief  Set the value of @ref pwm_start_data_t.pwm_start_i.
 * @param  [in]  group The value of @ref pwm_v151_regs_t.pwm_v151_sel.
 * @param  [in]  val The value of @ref pwm_start_data_t.pwm_start_i.
 */
static inline void hal_pwm_start_set_pwm_start_i(pwm_v151_group_t group, uint32_t val)
{
    pwm_start_data_t start;
    start.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_v151_sel[group].pwm_start;
    start.b.pwm_start_i = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_v151_sel[group].pwm_start = start.d32;
}

/**
 * @brief  Set the value of @ref pwm_en_data_t.pwm_en_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_en_data_t.pwm_en_j.
 */
static inline void hal_pwm_en_set_pwm_en_j(pwm_channel_t channel, uint32_t val)
{
    pwm_en_data_t en;
    en.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_en;
    en.b.pwm_en_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_en = en.d32;
}

/**
 * @brief  Get the value of @ref pwm_en_data_t.pwm_en_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_en_data_t.pwm_en_j.
 */
static inline uint32_t hal_pwm_en_get_pwm_en_j(pwm_channel_t channel)
{
    pwm_en_data_t en;
    en.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_en;
    return en.b.pwm_en_j;
}

/**
 * @brief  Set the value of @ref pwm_portity_data_t.pwm_portity_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_portity_data_t.pwm_portity_j.
 */
static inline void hal_pwm_portity_set_pwm_portity_j(pwm_channel_t channel, uint32_t val)
{
    pwm_portity_data_t portity;
    portity.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_portity;
    portity.b.pwm_portity_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_portity = portity.d32;
}

/**
 * @brief  Get the value of @ref pwm_portity_data_t.pwm_portity_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_portity_data_t.pwm_portity_j.
 */
static inline uint32_t hal_pwm_portity_get_pwm_portity_j(pwm_channel_t channel)
{
    pwm_portity_data_t portity;
    portity.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_portity;
    return portity.b.pwm_portity_j;
}

/**
 * @brief  Set the value of @ref pwm_oen_cfg_data_t.pwm_oen_cfg_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_oen_cfg_data_t.pwm_oen_cfg_j.
 */
static inline void hal_pwm_oen_cfg_set_pwm_oen_cfg_j(pwm_channel_t channel, uint32_t val)
{
    pwm_oen_cfg_data_t oen_cfg;
    oen_cfg.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_oen_cfg;
    oen_cfg.b.pwm_oen_cfg_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_oen_cfg = oen_cfg.d32;
}

/**
 * @brief  Get the value of @ref pwm_oen_cfg_data_t.pwm_oen_cfg_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_oen_cfg_data_t.pwm_oen_cfg_j.
 */
static inline uint32_t hal_pwm_oen_cfg_get_pwm_oen_cfg_j(pwm_channel_t channel)
{
    pwm_oen_cfg_data_t oen_cfg;
    oen_cfg.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_oen_cfg;
    return oen_cfg.b.pwm_oen_cfg_j;
}

/**
 * @brief  Set the value of @ref pwm_offset_l_data_t.pwm_offset_l_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_offset_l_data_t.pwm_offset_l_j.
 */
static inline void hal_pwm_offset_l_set_pwm_offset_l_j(pwm_channel_t channel, uint32_t val)
{
    pwm_offset_l_data_t offset_l;
    offset_l.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_offset_l;
    offset_l.b.pwm_offset_l_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_offset_l = offset_l.d32;
}

/**
 * @brief  Get the value of @ref pwm_offset_l_data_t.pwm_offset_l_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_offset_l_data_t.pwm_offset_l_j.
 */
static inline uint32_t hal_pwm_offset_l_get_pwm_offset_l_j(pwm_channel_t channel)
{
    pwm_offset_l_data_t offset_l;
    offset_l.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_offset_l;
    return offset_l.b.pwm_offset_l_j;
}

/**
 * @brief  Set the value of @ref pwm_offset_h_data_t.pwm_offset_h_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_offset_h_data_t.pwm_offset_h_j.
 */
static inline void hal_pwm_offset_h_set_pwm_offset_h_j(pwm_channel_t channel, uint32_t val)
{
    pwm_offset_h_data_t offset_h;
    offset_h.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_offset_h;
    offset_h.b.pwm_offset_h_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_offset_h = offset_h.d32;
}

/**
 * @brief  Get the value of @ref pwm_offset_h_data_t.pwm_offset_h_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_offset_h_data_t.pwm_offset_h_j.
 */
static inline uint32_t hal_pwm_offset_h_get_pwm_offset_h_j(pwm_channel_t channel)
{
    pwm_offset_h_data_t offset_h;
    offset_h.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_offset_h;
    return offset_h.b.pwm_offset_h_j;
}

/**
 * @brief  Set the value of @ref pwm_freq_l_data_t.pwm_freq_l_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_freq_l_data_t.pwm_freq_l_j.
 */
static inline void hal_pwm_freq_l_set_pwm_freq_l_j(pwm_channel_t channel, uint32_t val)
{
    pwm_freq_l_data_t freq_l;
    freq_l.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_freq_l;
    freq_l.b.pwm_freq_l_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_freq_l = freq_l.d32;
}

/**
 * @brief  Get the value of @ref pwm_freq_l_data_t.pwm_freq_l_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_freq_l_data_t.pwm_freq_l_j.
 */
static inline uint32_t hal_pwm_freq_l_get_pwm_freq_l_j(pwm_channel_t channel)
{
    pwm_freq_l_data_t freq_l;
    freq_l.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_freq_l;
    return freq_l.b.pwm_freq_l_j;
}

/**
 * @brief  Set the value of @ref pwm_freq_h_data_t.pwm_freq_h_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_freq_h_data_t.pwm_freq_h_j.
 */
static inline void hal_pwm_freq_h_set_pwm_freq_h_j(pwm_channel_t channel, uint32_t val)
{
    pwm_freq_h_data_t freq_h;
    freq_h.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_freq_h;
    freq_h.b.pwm_freq_h_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_freq_h = freq_h.d32;
}

/**
 * @brief  Get the value of @ref pwm_freq_h_data_t.pwm_freq_h_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_freq_h_data_t.pwm_freq_h_j.
 */
static inline uint32_t hal_pwm_freq_h_get_pwm_freq_h_j(pwm_channel_t channel)
{
    pwm_freq_h_data_t freq_h;
    freq_h.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_freq_h;
    return freq_h.b.pwm_freq_h_j;
}

/**
 * @brief  Set the value of @ref pwm_duty_l_data_t.pwm_duty_l_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_duty_l_data_t.pwm_duty_l_j.
 */
static inline void hal_pwm_duty_l_set_pwm_duty_l_j(pwm_channel_t channel, uint32_t val)
{
    pwm_duty_l_data_t duty_l;
    duty_l.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_duty_l;
    duty_l.b.pwm_duty_l_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_duty_l = duty_l.d32;
}

/**
 * @brief  Get the value of @ref pwm_duty_l_data_t.pwm_duty_l_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_duty_l_data_t.pwm_duty_l_j.
 */
static inline uint32_t hal_pwm_duty_l_get_pwm_duty_l_j(pwm_channel_t channel)
{
    pwm_duty_l_data_t duty_l;
    duty_l.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_duty_l;
    return duty_l.b.pwm_duty_l_j;
}

/**
 * @brief  Set the value of @ref pwm_duty_h_data_t.pwm_duty_h_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_duty_h_data_t.pwm_duty_h_j.
 */
static inline void hal_pwm_duty_h_set_pwm_duty_h_j(pwm_channel_t channel, uint32_t val)
{
    pwm_duty_h_data_t duty_h;
    duty_h.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_duty_h;
    duty_h.b.pwm_duty_h_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_duty_h = duty_h.d32;
}

/**
 * @brief  Get the value of @ref pwm_duty_h_data_t.pwm_duty_h_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_duty_h_data_t.pwm_duty_h_j.
 */
static inline uint32_t hal_pwm_duty_h_get_pwm_duty_h_j(pwm_channel_t channel)
{
    pwm_duty_h_data_t duty_h;
    duty_h.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_duty_h;
    return duty_h.b.pwm_duty_h_j;
}

/**
 * @brief  Get the value of @ref pwm_periodload_flag_data_t.pwm_periodload_flag_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_periodload_flag_data_t.pwm_periodload_flag_j.
 */
static inline uint32_t hal_pwm_periodload_flag_get_pwm_periodload_flag_j(pwm_channel_t channel)
{
    pwm_periodload_flag_data_t periodload_flag;
    periodload_flag.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_periodload_flag;
    return periodload_flag.b.pwm_periodload_flag_j;
}

/**
 * @brief  Set the value of @ref pwm_period_val_data_t.pwm_period_val_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @param  [in]  val The value of @ref pwm_period_val_data_t.pwm_period_val_j.
 */
static inline void hal_pwm_period_val_set_pwm_period_val_j(pwm_channel_t channel, uint16_t val)
{
    pwm_period_val_data_t period_val;
    period_val.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_period_val;
    period_val.b.pwm_period_val_j = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_period_val = period_val.d32;
}

/**
 * @brief  Get the value of @ref pwm_period_val_data_t.pwm_period_val_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_period_val_data_t.pwm_period_val_j.
 */
static inline uint32_t hal_pwm_period_val_get_pwm_period_val_j(pwm_channel_t channel)
{
    pwm_period_val_data_t period_val;
    period_val.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_period_val;
    return period_val.b.pwm_period_val_j;
}

/**
 * @brief  Get the value of @ref pwm_periodcnt_data_t.pwm_periodcnt_j.
 * @param  [in]  channel The value of @ref pwm_v151_regs_t.pwm_ip.
 * @return The value of @ref pwm_periodcnt_data_t.pwm_periodcnt_j.
 */
static inline uint32_t hal_pwm_periodcnt_get_pwm_periodcnt_j(pwm_channel_t channel)
{
    pwm_periodcnt_data_t periodcnt;
    periodcnt.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_ip[channel].pwm_periodcnt;
    return periodcnt.b.pwm_periodcnt_j;
}

/**
 * @brief  Get the value of @ref pwm_abnor_state0_data_t.pwm_abnor_state0.
 * @return The value of @ref pwm_abnor_state0_data_t.pwm_abnor_state0.
 */
static inline uint32_t hal_pwm_abnor_state0_get_pwm_abnor_state0(void)
{
    pwm_abnor_state0_data_t abnor_state0;
    abnor_state0.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_abnor_state0;
    return abnor_state0.b.pwm_abnor_state0;
}

/**
 * @brief  Get the value of @ref pwm_abnor_state1_data_t.pwm_abnor_state1.
 * @return The value of @ref pwm_abnor_state1_data_t.pwm_abnor_state1.
 */
static inline uint32_t hal_pwm_abnor_state1_get_pwm_abnor_state1(void)
{
    pwm_abnor_state1_data_t abnor_state1;
    abnor_state1.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_abnor_state1;
    return abnor_state1.b.pwm_abnor_state1;
}

/**
 * @brief  Get the value of @ref pwm_abnor_state1_data_t.reserved16_31.
 * @return The value of @ref pwm_abnor_state1_data_t.reserved16_31.
 */
static inline uint32_t hal_pwm_abnor_state1_get_reserved(void)
{
    pwm_abnor_state1_data_t abnor_state1;
    abnor_state1.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_abnor_state1;
    return abnor_state1.b.reserved16_31;
}

/**
 * @brief  Set the value of @ref pwm_abnor_state_clr0_data_t.pwm_abnor_state_clr0.
 * @param  [in]  val The value of @ref pwm_abnor_state_clr0_data_t.pwm_abnor_state_clr0.
 */
static inline void hal_pwm_abnor_state_clr0_set_pwm_abnor_state_clr0(uint32_t val)
{
    pwm_abnor_state_clr0_data_t abnor_state_clr0;
    abnor_state_clr0.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_abnor_state_clr0;
    abnor_state_clr0.b.pwm_abnor_state_clr0 = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_abnor_state_clr0 = abnor_state_clr0.d32;
}

/**
 * @brief  Get the value of @ref pwm_abnor_state_clr0_data_t.reserved16_31.
 * @return The value of @ref pwm_abnor_state_clr0_data_t.reserved16_31.
 */
static inline uint32_t hal_pwm_abnor_state_clr0_get_reserved(void)
{
    pwm_abnor_state_clr0_data_t abnor_state_clr0;
    abnor_state_clr0.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_abnor_state_clr0;
    return abnor_state_clr0.b.reserved16_31;
}

/**
 * @brief  Set the value of @ref pwm_abnor_state_clr1_data_t.pwm_abnor_state_clr1.
 * @param  [in]  val The value of @ref pwm_abnor_state_clr1_data_t.pwm_abnor_state_clr1.
 */
static inline void hal_pwm_abnor_state_clr1_set_pwm_abnor_state_clr1(uint32_t val)
{
    pwm_abnor_state_clr1_data_t abnor_state_clr1;
    abnor_state_clr1.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_abnor_state_clr1;
    abnor_state_clr1.b.pwm_abnor_state_clr1 = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_abnor_state_clr1 = abnor_state_clr1.d32;
}

/**
 * @brief  Get the value of @ref pwm_abnor_state_clr1_data_t.reserved16_31.
 * @return The value of @ref pwm_abnor_state_clr1_data_t.reserved16_31.
 */
static inline uint32_t hal_pwm_abnor_state_clr1_get_reserved(void)
{
    pwm_abnor_state_clr1_data_t abnor_state_clr1;
    abnor_state_clr1.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_abnor_state_clr1;
    return abnor_state_clr1.b.reserved16_31;
}

/**
 * @brief  Set the value of @ref pwm_int_mask_data_t.pwm_int_mask.
 * @param  [in]  val The value of @ref pwm_int_mask_data_t.pwm_int_mask.
 */
static inline void hal_pwm_int_mask_set_pwm_int_mask(uint32_t val)
{
    pwm_int_mask_data_t int_mask;
    int_mask.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_int_mask;
    int_mask.b.pwm_int_mask = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_int_mask = int_mask.d32;
}

/**
 * @brief  Get the value of @ref pwm_int_mask_data_t.pwm_int_mask.
 * @return The value of @ref pwm_int_mask_data_t.pwm_int_mask.
 */
static inline uint32_t hal_pwm_int_mask_get_pwm_int_mask(void)
{
    pwm_int_mask_data_t int_mask;
    int_mask.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_int_mask;
    return int_mask.b.pwm_int_mask;
}

/**
 * @brief  Set the value of @ref pwm_dma_en_data_t.pwm_dma_en.
 * @param  [in]  val The value of @ref pwm_dma_en_data_t.pwm_dma_en.
 */
static inline void hal_pwm_dma_en_set_pwm_dma_en(uint32_t val)
{
    pwm_dma_en_data_t dma_en;
    dma_en.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_dma_en;
    dma_en.b.pwm_dma_en = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_dma_en = dma_en.d32;
}

/**
 * @brief  Get the value of @ref pwm_dma_en_data_t.pwm_dma_en.
 * @return The value of @ref pwm_dma_en_data_t.pwm_dma_en.
 */
static inline uint32_t hal_pwm_dma_en_get_pwm_dma_en(void)
{
    pwm_dma_en_data_t dma_en;
    dma_en.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_dma_en;
    return dma_en.b.pwm_dma_en;
}

/**
 * @brief  Set the value of @ref pwm_cfg_int_clr0_data_t.pwm_cfg_int_clr0.
 * @param  [in]  val The value of @ref pwm_cfg_int_clr0_data_t.pwm_cfg_int_clr0.
 */
static inline void hal_pwm_cfg_int_clr0_set_pwm_cfg_int_clr0(uint32_t val)
{
    pwm_cfg_int_clr0_data_t cfg_int_clr0;
    cfg_int_clr0.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_cfg_int_clr0;
    cfg_int_clr0.b.pwm_cfg_int_clr0 = val;
    ((pwm_v151_regs_t *)g_pwm_regs)->pwm_cfg_int_clr0 = cfg_int_clr0.d32;
}

/**
 * @brief  Get the value of @ref pwm_cfg_int_clr0_data_t.reserved16_31.
 * @return The value of @ref pwm_cfg_int_clr0_data_t.reserved16_31.
 */
static inline uint32_t hal_pwm_cfg_int_clr0_get_reserved(void)
{
    pwm_cfg_int_clr0_data_t cfg_int_clr0;
    cfg_int_clr0.d32 = ((pwm_v151_regs_t *)g_pwm_regs)->pwm_cfg_int_clr0;
    return cfg_int_clr0.b.reserved16_31;
}


/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif