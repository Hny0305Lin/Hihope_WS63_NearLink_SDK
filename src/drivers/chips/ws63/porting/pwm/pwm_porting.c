/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pwm port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "chip_core_irq.h"
#include "soc_osal.h"
#include "common_def.h"
#include "hal_pwm_v151.h"
#include "platform_core.h"
#include "pwm_porting.h"

#define BUS_CLOCK_TIME_40M      40000000UL
#define BIT_WIDTH_LIMIT         0xFFFF
#define CLDO_CRG_CLK_SEL        0x44001134
#define PWM_CKSEL_BIT           7

uintptr_t g_pwm_base_addr =  (uintptr_t)PWM_0_BASE;

uintptr_t pwm_porting_base_addr_get(void)
{
    return g_pwm_base_addr;
}

static int pwm_handler(int a, const void *tmp)
{
    unused(a);
    unused(tmp);
    hal_pwm_v151_irq_handler();
    return 0;
}


void pwm_port_register_hal_funcs(void)
{
    hal_pwm_register_funcs(hal_pwm_v151_funcs_get());
}

void pwm_port_unregister_hal_funcs(void)
{
    hal_pwm_unregister_funcs();
}

void pwm_port_clock_enable(bool on)
{
    if (on == true) {
        uapi_reg_setbit(CLDO_CRG_CLK_SEL, PWM_CKSEL_BIT);  // chose the clock source :PLL
    }
}

void pwm_port_register_irq(pwm_channel_t channel)
{
    unused(channel);
    osal_irq_request((uintptr_t)PWM_ABNOR_IRQN, (osal_irq_handler)pwm_handler, NULL, NULL, NULL);

    osal_irq_enable((uintptr_t)PWM_ABNOR_IRQN);
}

void pwm_port_unregister_irq(pwm_channel_t channel)
{
    unused(channel);
    osal_irq_disable((uintptr_t)PWM_ABNOR_IRQN);
    osal_irq_disable((uintptr_t)PWM_CFG_IRQN);
    osal_irq_free((uintptr_t)PWM_ABNOR_IRQN, NULL);
    osal_irq_free((uintptr_t)PWM_CFG_IRQN, NULL);
}

void pwm_irq_lock(uint8_t channel)
{
    unused(channel);
    osal_irq_lock();
}

void pwm_irq_unlock(uint8_t channel)
{
    unused(channel);
    osal_irq_unlock();
}

uint32_t pwm_port_get_clock_value(pwm_channel_t channel)
{
    if (channel >= PWM_MAX_NUMBER) {
        return 0;
    }
    return BUS_CLOCK_TIME_40M;
}

errcode_t pwm_port_param_check(const pwm_config_t *cfg)
{
    if ((cfg->low_time + cfg->high_time > BIT_WIDTH_LIMIT) || (cfg->offset_time > cfg->low_time)) {
        return ERRCODE_PWM_INVALID_PARAMETER;
    }
    return ERRCODE_SUCC;
}