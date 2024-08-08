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
#include "watchdog.h"
#include "tcxo.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"

#define TRICOLORED_TASK_PRIO 24
#define TRICOLORED_TASK_STACK_SIZE 0x1000
#define BSP_LED_0 5
#define BSP_LED_MODE 4

extern errcode_t uapi_tcxo_delay_count(uint16_t ticks_delay);

void one_num(void)
{
    uapi_reg_setbit(0x44028030, 5); // 0x44028030代表GPIO电平拉高寄存器，5代表GPIO5，将gpio5拉高,可以参考platform_core.h
    uapi_tcxo_delay_count(8);
    uapi_reg_setbit(0x44028034, 5); // 0x44028034代表GPIO电平拉低寄存器，5代表GPIO5，将gpio5拉低,可以参考platform_core.h
    uapi_tcxo_delay_us(2);
}

void zero_num(void)
{
    uint32_t preg = 0;
    preg = preg;
    uapi_reg_setbit(0x44028030, 5);
    for (int i = 0; i < 7; i++) {
        // 由于uapi_tcxo_delay_us在低于1us以后，延时不准确，通过测试发现读一次寄存器大概50ns
        uapi_reg_read32(0x44028030, preg);
    }
    uapi_reg_setbit(0x44028034, 5);
    uapi_tcxo_delay_us(2);
}

void green_led(void)
{
    for (int i = 0; i < 8; i++) {
        one_num();
    }
    for (int i = 0; i < 8; i++) {
        zero_num();
    }
    for (int i = 0; i < 8; i++) {
        zero_num();
    }
}

void red_led(void)
{
    for (int i = 0; i < 8; i++) {
        zero_num();
    }
    for (int i = 0; i < 8; i++) {
        one_num();
    }
    for (int i = 0; i < 8; i++) {
        zero_num();
    }
}

void blue_led(void)
{
    for (int i = 0; i < 8; i++) {
        zero_num();
    }
    for (int i = 0; i < 8; i++) {
        zero_num();
    }
    for (int i = 0; i < 8; i++) {
        one_num();
    }
}

static int tricolored_task(const char *arg)
{
    UNUSED(arg);
    uapi_pin_set_mode(BSP_LED_0, BSP_LED_MODE);
    uapi_gpio_set_dir(BSP_LED_0, GPIO_DIRECTION_OUTPUT);
    uapi_reg_setbit(0x44028034, 5);
    uapi_tcxo_delay_us(100); // 初始化三色灯，等待三色灯正常工作，延时需要大于80us，可以参考三色灯规格书
    green_led();
    uapi_tcxo_delay_ms(200);
    red_led();
    osal_mdelay(200);
    blue_led();
    return 0;
}

static void blinky_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)tricolored_task, 0, "TricoloredTask", TRICOLORED_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TRICOLORED_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the blinky_entry. */
app_run(blinky_entry);