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
#include "osal_wait.h"
#include "app_init.h"

#define TASK_PRI_TASK1 21
#define TASK_PRI_TASK2 20
#define MUX_TASK_STACK_SIZE 0x1000
#define MUX_TASK_PRIO 17
#define TASK_STACK_SIZE 0x1000

static osal_mutex g_mux_id;

void example_mutex_task1(void)
{
    uint32_t ret;
    printf("task1 try to get mutex,wait 10 ms.\n");
    ret = osal_mutex_lock_timeout(&g_mux_id, 10);
    if (ret == OSAL_SUCCESS) {
        printf("task1 get mutex g_mux_id.\n");
        osal_mutex_unlock(&g_mux_id);
    } else {
        printf("task1 timeout and try to get mutex, wait forever.\n");
        ret = osal_mutex_lock_timeout(&g_mux_id, OSAL_WAIT_FOREVER);
        if (ret == OSAL_SUCCESS) {
            printf("task1 wait forever,get mutex g_mux_id.\n");
            osal_mutex_unlock(&g_mux_id);
        }
    }
}

void example_mutex_task2(void)
{
    printf("task2 try to get mutex, wait forever.\n");
    osal_mutex_lock_timeout(&g_mux_id, OSAL_WAIT_FOREVER);
    printf("task2 get mutex g_mux_id and suspend 100 ms.\n");
    osal_msleep(100);
    printf("task2 resumed and post the g_mux_id\n");
    osal_mutex_unlock(&g_mux_id);
}

void example_task_mux(void)
{
    uint32_t ret;
    osal_task *taskid1, *taskid2;
    osal_mutex_init(&g_mux_id);
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid1 = osal_kthread_create((osal_kthread_handler)example_mutex_task1, NULL, "example_task1", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid1, TASK_PRI_TASK1);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    // 创建任务2
    taskid2 = osal_kthread_create((osal_kthread_handler)example_mutex_task2, NULL, "example_task2", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid2, TASK_PRI_TASK2);
    if (ret != OSAL_SUCCESS) {
        printf("create task2 failed .\n");
    }
    osal_kthread_unlock();
    // 延时3s回收资源
    osal_msleep(3000);
    osal_mutex_destroy(&g_mux_id);
}

static void example_task_entry_mux(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)example_task_mux, 0, "task_mux", MUX_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, MUX_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the muxTask_entry. */
app_run(example_task_entry_mux);