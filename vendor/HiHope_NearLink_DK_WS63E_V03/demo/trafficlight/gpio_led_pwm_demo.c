/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pinctrl.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "watchdog.h"
#include "pwm.h"
#include "test_suite_log.h"
#include "soc_osal.h"
#include "app_init.h"

#define BLINKY_TASK_STACK_SIZE 0x2000
#define CONFIG_BLINKY_DURATION_MS 10
#define BLINKY_TASK_PRIO 17
#define CONFIG_GPIO14_PIN 14 // 按键
#define CONFIG_GPIO9_PIN 9   // 蜂鸣器
#define CONFIG_GPIO7_PIN 7   // RED
#define CONFIG_GPIO11_PIN 11 // GREEN
#define CONFIG_GPIO10_PIN 10 // YELLOW
#define PWM1_CHANNEL 1
#define CONFIG_PWM_PIN_MODE 1
#define PWM0_GROUP_ID 1
static int g_beepState = 0;

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    g_beepState = !g_beepState;
    printf("Button pressed.\r\n");
}

void gpio_set_value(pin_t pin)
{
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(pin, GPIO_LEVEL_LOW);
}

static void *beep_led_task(const char *arg)
{
    unused(arg);
    pwm_config_t cfg_no_repeat = {20000, 10000, 0xFF, 0xFF, false};
    uapi_pin_set_mode(CONFIG_GPIO9_PIN, CONFIG_PWM_PIN_MODE);
    gpio_set_value(CONFIG_GPIO7_PIN);
    gpio_set_value(CONFIG_GPIO11_PIN);
    gpio_set_value(CONFIG_GPIO10_PIN);
    uapi_pin_set_mode(CONFIG_GPIO14_PIN, HAL_PIO_FUNC_GPIO);
    gpio_select_core(CONFIG_GPIO14_PIN, CORES_APPS_CORE);
    uapi_gpio_set_dir(CONFIG_GPIO14_PIN, GPIO_DIRECTION_INPUT);
    errcode_t ret = uapi_gpio_register_isr_func(CONFIG_GPIO14_PIN, GPIO_INTERRUPT_FALLING_EDGE, gpio_callback_func);
    if (ret != 0) {
        uapi_gpio_unregister_isr_func(CONFIG_GPIO14_PIN);
    }
    while (1) {
        uapi_watchdog_kick(); // 防止系统挂失
        osal_mdelay(CONFIG_BLINKY_DURATION_MS);
        uapi_gpio_toggle(CONFIG_GPIO7_PIN);
        uapi_gpio_toggle(CONFIG_GPIO11_PIN);
        uapi_gpio_toggle(CONFIG_GPIO10_PIN);
        if (g_beepState) {
            uapi_pwm_init();
            uapi_pwm_open(PWM1_CHANNEL, &cfg_no_repeat);
            uint8_t channel_id = PWM1_CHANNEL;
            uapi_pwm_set_group(PWM0_GROUP_ID, &channel_id, 1); // 每个通道对应一个bit位
            uapi_pwm_start(PWM0_GROUP_ID);
        } else {
            uapi_pwm_close(PWM1_CHANNEL);
            uapi_pwm_deinit();
        }
    }
    return NULL;
}

static void beep_led_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)beep_led_task, 0, "BeepLedTask", BLINKY_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLINKY_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the blinky_entry. */
app_run(beep_led_entry);