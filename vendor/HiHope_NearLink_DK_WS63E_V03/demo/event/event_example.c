/*
 * Copyright (c) 2024 HiSilicon Technologies CO., LIMITED.
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
#include "osal_wait.h"
#include "app_init.h"

#define TEST_EVENT (1 << 0) // 事件掩码类型
#define READ_TASK_PRI_EVENT 24
#define WRITE_TASK_PRI_EVENT 25
#define EVENT_TASK_STACK_SIZE 0x1000
#define EVENT_TASK_PRIO 17
#define TASK_STACK_SIZE 0xc00

static osal_event g_event_id;
uint32_t status = 0;

static void example_event_read(const char *arg)
{
    unused(arg);
    uint32_t ret;
    // 超时等待读事件，超时时间为永远等待
    printf("example_event wait event 0x%x\r\n", TEST_EVENT);
    ret = osal_event_read(&g_event_id, TEST_EVENT, OSAL_WAIT_FOREVER, OSAL_WAITMODE_AND);
    if (ret == 1) {
        printf("example_event read event 0x%x\r\n", TEST_EVENT);
    } else {
        printf("example_event read event failed\r\n");
    }
}

static void example_event_write(const char *arg)
{
    unused(arg);
    uint32_t ret = 0;
    // 写用例任务等待事件
    printf("example_task_entry_event write event\r\n");
    ret = osal_event_write(&g_event_id, TEST_EVENT);
    if (ret != OSAL_SUCCESS) {
        printf("event write failed\r\n");
    }
    printf("example_task_entry_event event write success\r\n");
    // 清除标志位
    ret = osal_event_clear(&g_event_id, TEST_EVENT);
    if (ret != OSAL_SUCCESS) {
        printf("event clear failed\r\n");
    }
    printf("example_task_event event clear success\r\n");
}

void event_task_entry(void)
{
    uint32_t ret = 0;
    osal_task *example_task1_info, *example_task2_info;
    // 初始化事件
    osal_event_init(&g_event_id);
    // 创建任务期间锁住任务调度
    osal_kthread_lock();
    // 创建任务1
    example_task1_info = osal_kthread_create((osal_kthread_handler)example_event_read, NULL, "example_task1",
                                             TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(example_task1_info, READ_TASK_PRI_EVENT);
    if (ret != OSAL_SUCCESS) {
        printf("example_task1 creat failed\r\n");
    }
    example_task2_info = osal_kthread_create((osal_kthread_handler)example_event_write, NULL, "example_task2",
                                             TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(example_task2_info, WRITE_TASK_PRI_EVENT);
    if (ret != OSAL_SUCCESS) {
        printf("example_task2 creat failed\r\n");
    }
    osal_kthread_unlock();
}

/* Run the EventTask_entry. */
app_run(event_task_entry);