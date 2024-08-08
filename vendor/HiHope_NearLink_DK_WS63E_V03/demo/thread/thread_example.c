/*
 * Copyright (c) 2024 HiSilicon (Shanghai) Technologies CO., LIMITED.
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
#include "osal_debug.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define TASKS_TEST_TASK_STACK_SIZE 0x1000
#define TASKS_TEST_TASK1_PRIO 20
#define TASKS_TEST_TASK2_PRIO 19
#define THREAD_DELAY_1S 1000
#define THREAD_DELAY_500MS 500

/**
 * @brief Thread1 entry
 *
 */
void Thread1(void)
{
    int sum = 0;

    while (1) {
        printf("This is Thread1----%d\n", sum++);
        osal_msleep(THREAD_DELAY_1S);
    }
}

/**
 * @brief Thread2 entry
 *
 */
void Thread2(void)
{
    int sum = 0;

    while (1) {
        printf("This is Thread2----%d\n", sum++);
        osal_msleep(THREAD_DELAY_500MS);
    }
}

/**
 * @brief Main Entry of the Thread Example
 *
 */
static void ThreadExample(void)
{
    int32_t ret = 0;
    osal_task *example_task1_info, *example_task2_info;
    osal_kthread_lock();
    // 创建任务1
    example_task1_info =
        osal_kthread_create((osal_kthread_handler)Thread1, 0, "example_task1", TASKS_TEST_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(example_task1_info, TASKS_TEST_TASK1_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("example_task creat failed\r\n");
    }
    example_task2_info =
        osal_kthread_create((osal_kthread_handler)Thread2, 0, "example_task2", TASKS_TEST_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(example_task2_info, TASKS_TEST_TASK2_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("example_task creat failed\r\n");
    }
    osal_kthread_unlock();
}

app_run(ThreadExample);
