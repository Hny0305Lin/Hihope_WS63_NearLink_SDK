/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: PWM Sample Source. \n
 *
 * History: \n
 * 2023-06-27, Create file. \n
 */
#include "pinctrl.h"
#include "pwm.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "tcxo.h"

#define PWM_GPIO                   2
#define PWM_PIN_MODE               1
#define PWM_CHANNEL                2
#define PWM_GROUP_ID               0
#define TEST_MAX_TIMES             5
#define DALAY_MS                   100
#define TEST_TCXO_DELAY_1000MS     1000
#define PWM_TASK_STACK_SIZE        0x1000
#define PWM_TASK_PRIO              (osPriority_t)(17)

static errcode_t pwm_sample_callback(uint8_t channel)
{
    osal_printk("PWM %d, cycle done. \r\n", channel);
    return ERRCODE_SUCC;
}

static void *pwm_task(const char *arg)
{
    UNUSED(arg);
    pwm_config_t cfg_no_repeat = {
        50,     //低电平
        200,    //高电平
        0,
        0,
        true
    };

    uapi_pin_set_mode(PWM_GPIO, PWM_PIN_MODE);
    uapi_pwm_deinit();
    uapi_pwm_init();
    uapi_pwm_open(PWM_CHANNEL, &cfg_no_repeat);

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_unregister_interrupt(PWM_CHANNEL);
    uapi_pwm_register_interrupt(PWM_CHANNEL, pwm_sample_callback);

    uint8_t channel_id = PWM_CHANNEL;
    /* channel_id can also choose to configure multiple channels, and the third parameter also needs to be adjusted
        accordingly. */
    uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);
    /* Here you can also call the uapi_pwm_start interface to open each channel individually. */
    uapi_pwm_start_group(PWM_GROUP_ID);

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);

    uapi_pwm_close(PWM_GROUP_ID);


    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_deinit();
    return NULL;
}

static void pwm_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "PWMTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = PWM_TASK_STACK_SIZE;
    attr.priority = PWM_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)pwm_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the pwm_entry. */
app_run(pwm_entry);