/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides iot_pwm driver source \n
 *
 * History: \n
 * 2023-10-18， Create file. \n
 */
#include "common_def.h"
#include "iot_errno.h"
#include "pwm.h"
#include "iot_pwm.h"

#define IOT_PWM_MIN_DUTY           0
#define IOT_PWM_MAX_DUTY           100
#define IOT_PWM_DUTY_PART          10
#define IOT_PWM_FREQ_PART          2
#define IOT_PWM_MAX_DIV_NUM_MAX    8
#define IOT_PWM_CFG_OFFEST_TIME    0
#define IOT_PWM_CFG_REPEAT_CYCLE   100
#define IOT_PWM_CFG_REPEAT_STATE   true

static bool g_iot_pwm_inited = false;

unsigned int IoTPwmInit(unsigned int port)
{
    unused(port);
    if (!g_iot_pwm_inited) {
        uapi_pwm_init();
    }
    g_iot_pwm_inited = true;
    return IOT_SUCCESS;
}

unsigned int IoTPwmDeinit(unsigned int port)
{
    unused(port);
    if (g_iot_pwm_inited) {
        uapi_pwm_deinit();
    }
    g_iot_pwm_inited = false;
    return IOT_SUCCESS;
}

unsigned int IoTPwmStart(unsigned int port, unsigned short duty, unsigned int freq)
{
    if ((duty >= IOT_PWM_MAX_DUTY) || (duty == IOT_PWM_MIN_DUTY)) {
        return IOT_FAILURE;
    }

    uint32_t clk_freq = uapi_pwm_get_frequency((uint8_t)port);
    uint32_t div_num = (clk_freq + freq / IOT_PWM_FREQ_PART) / freq;   // 计算分频数
    if (div_num < IOT_PWM_MAX_DIV_NUM_MAX) {
        return IOT_FAILURE;
    }
    uint32_t high_time = div_num * duty / IOT_PWM_MAX_DUTY;            // 计算高电平时钟个数（向下取整）
    if (high_time * IOT_PWM_MAX_DUTY / div_num >= (uint32_t)(duty - duty * IOT_PWM_DUTY_PART / IOT_PWM_MAX_DUTY)) {
    } else if ((high_time + 1) * IOT_PWM_MAX_DUTY / div_num <=
                (uint32_t)(duty + duty * IOT_PWM_DUTY_PART / IOT_PWM_MAX_DUTY)) {
        high_time++;
    } else {
        return IOT_FAILURE;
    }
    uint32_t low_time = div_num - high_time;
    pwm_config_t cfg = {
        .low_time = low_time,
        .high_time = high_time,
        .offset_time = IOT_PWM_CFG_OFFEST_TIME,
        .cycles = IOT_PWM_CFG_REPEAT_CYCLE,
        .repeat = IOT_PWM_CFG_REPEAT_STATE
    };
    if (uapi_pwm_open((uint8_t)port, &cfg) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }

    if (uapi_pwm_start((uint8_t)port) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

unsigned int IoTPwmStop(unsigned int port)
{
    if (uapi_pwm_close((uint8_t)port) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}