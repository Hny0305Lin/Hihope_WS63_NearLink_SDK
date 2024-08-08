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
#include "pwm.h"
#include "tcxo.h"
#include "soc_osal.h"
#include "app_init.h"

#define PWM_CHANNEL 1
#define PWM_GROUP_ID 1
#define TEST_TCXO_DELAY_1000MS 1000

#define PWM_TASK_PRIO 24
#define PWM_TASK_STACK_SIZE 0x1000
#define CONFIG_PWM_PIN 9
#define CONFIG_PWM_PIN_MODE 1

static errcode_t pwm_sample_callback(uint8_t channel)
{
    osal_printk("PWM %d, cycle done. \r\n", channel);
    return ERRCODE_SUCC;
}

static void *pwm_task(const char *arg)
{
    UNUSED(arg);
    pwm_config_t cfg_no_repeat = {20000,
                                  10000, // 高电平持续tick 时间 = tick * (1/32000000)
                                  0,     // 相位偏移位
                                  1,     // 发多少个波形
                                  true}; // 是否循环

    uapi_pin_set_mode(CONFIG_PWM_PIN, CONFIG_PWM_PIN_MODE);
    uapi_pwm_init();
    uapi_pwm_open(PWM_CHANNEL, &cfg_no_repeat);
    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_unregister_interrupt(PWM_GROUP_ID);
    uapi_pwm_register_interrupt(PWM_GROUP_ID, pwm_sample_callback);
    uint8_t channel_id = PWM_CHANNEL;
    uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1); // 每个通道对应一个bit位
    uapi_pwm_start(PWM_GROUP_ID);
    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_close(PWM_CHANNEL); // 波形发送完毕停止
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
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the pwm_entry. */
app_run(pwm_entry);