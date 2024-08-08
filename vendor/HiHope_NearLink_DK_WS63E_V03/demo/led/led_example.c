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
#include "app_init.h"

#define BLINKY_TASK_STACK_SIZE 0x1000
#define BLINKY_TASK_PRIO 24
#define BSP_LED 7                  // RED
#define CONFIG_BLINKY_DURATION_50MS 50

static void *led_task(const char *arg)
{
    unused(arg);
    uapi_pin_set_mode(BSP_LED, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(BSP_LED, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(BSP_LED, GPIO_LEVEL_LOW);

    while (1) {
        osal_msleep(CONFIG_BLINKY_DURATION_50MS);
        uapi_gpio_toggle(BSP_LED);
    }
    return NULL;
}

static void led_entry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务
    taskid = osal_kthread_create((osal_kthread_handler)led_task, NULL, "led_task", BLINKY_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, BLINKY_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}

/* Run the blinky_entry. */
app_run(led_entry);