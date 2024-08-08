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
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "test_suite_log.h"
#include "watchdog.h"
#include "app_init.h"

#define BSP_LED 7      // RED
#define BUTTON_GPIO 12 // 按键
#define GPIO5_MODE 0
#define BUTTON_TASK_STACK_SIZE 0x1000
#define BUTTON_TASK_PRIO 17

static int g_ledState = 0;

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    g_ledState = !g_ledState;
    printf("Button pressed.\r\n");
}

static void *button_task(const char *arg)
{
    unused(arg);
    uapi_pin_set_mode(BSP_LED, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(BSP_LED, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(BSP_LED, GPIO_LEVEL_LOW);
    uapi_pin_set_mode(BUTTON_GPIO, HAL_PIO_FUNC_GPIO);
    gpio_select_core(BUTTON_GPIO, CORES_APPS_CORE);
    uapi_gpio_set_dir(BUTTON_GPIO, GPIO_DIRECTION_INPUT);
    errcode_t ret = uapi_gpio_register_isr_func(BUTTON_GPIO, GPIO_INTERRUPT_FALLING_EDGE, gpio_callback_func);
    if (ret != 0) {
        uapi_gpio_unregister_isr_func(BUTTON_GPIO);
    }
    while (1) {
        uapi_watchdog_kick(); // 喂狗，防止程序出现异常系统挂死
        if (g_ledState) {
            uapi_gpio_set_val(BSP_LED, GPIO_LEVEL_HIGH);
        } else {
            uapi_gpio_set_val(BSP_LED, GPIO_LEVEL_LOW);
        }
    }
    return NULL;
}

static void button_entry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)button_task, NULL, "led_task", BUTTON_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, BUTTON_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}

/* Run the blinky_entry. */
app_run(button_entry);