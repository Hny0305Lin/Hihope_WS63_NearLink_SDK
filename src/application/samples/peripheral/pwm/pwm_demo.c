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
#include "tcxo.h"
#include "soc_osal.h"
#include "app_init.h"

#define TEST_TCXO_DELAY_1000MS     1000

#define PWM_TASK_PRIO              24
#define PWM_TASK_STACK_SIZE        0x1000

static errcode_t pwm_sample_callback(uint8_t channel)
{
    osal_printk("PWM %d, cycle done. \r\n", channel);
    return ERRCODE_SUCC;
}

static void *pwm_task(const char *arg)
{
    UNUSED(arg);
    pwm_config_t cfg_no_repeat = {
        100,
        100,
        0,
        0xFF,
        false
    };

    uapi_pin_set_mode(CONFIG_PWM_PIN, CONFIG_PWM_PIN_MODE);
    uapi_pwm_deinit();
    uapi_pwm_init();
    uapi_pwm_open(CONFIG_PWM_CHANNEL, &cfg_no_repeat);

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_unregister_interrupt(CONFIG_PWM_CHANNEL);
    uapi_pwm_register_interrupt(CONFIG_PWM_CHANNEL, pwm_sample_callback);
#ifdef CONFIG_PWM_USING_V151
    uint8_t channel_id = CONFIG_PWM_CHANNEL;
    /* channel_id can also choose to configure multiple channels, and the third parameter also needs to be adjusted
        accordingly. */
    uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
    /* Here you can also call the uapi_pwm_start interface to open each channel individually. */
    uapi_pwm_start_group(CONFIG_PWM_GROUP_ID);
#else
    uapi_pwm_start(CONFIG_PWM_CHANNEL);
#endif

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
#ifdef CONFIG_PWM_USING_V151
    uapi_pwm_close(CONFIG_PWM_GROUP_ID);
#else
    uapi_pwm_close(CONFIG_PWM_CHANNEL);
#endif

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_deinit();
    return NULL;
}

static void pwm_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)pwm_task, 0, "PwmTask", PWM_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, PWM_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the pwm_entry. */
app_run(pwm_entry);