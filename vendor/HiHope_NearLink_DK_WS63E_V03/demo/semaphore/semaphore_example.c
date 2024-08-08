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

#include "common.h"
#include "osal_wait.h"
#include "soc_errno.h"
#include "soc_osal.h"
#include "app_init.h"

#define TASK_PRO_TASK1 21
#define TASK_PRO_TASK2 20
#define SEM_TASK_STACK_SIZE 0x1000
#define SEM_TASK_PRIO 24
#define TASK_STACK_SIZE 0x1000

static osal_semaphore g_sem_id;

void example_sem_task1(void)
{
    uint32_t ret;
    printf("example_sem_task1 try get sem g_usSemID ,timeout 100 ms.\n");
    /*定时阻塞模式申请信号量，定时时间为100ms*/
    ret = osal_sem_down_timeout(&g_sem_id, 100);
    /*申请到信号量*/
    if (OSAL_SUCCESS == ret) {
        osal_sem_up(&g_sem_id);
    } else {
        /*定时时间到，未申请到信号量*/
        printf("example_sem_task1 timeout and try get sem g_usSemID wait forever.\n");
        /*永久阻塞模式申请信号量*/
        ret = osal_sem_down_timeout(&g_sem_id, OSAL_WAIT_FOREVER);
        printf("example_sem_task1 wait_forever and get sem g_usSemID .\n");
        if (OSAL_SUCCESS == ret) {
            osal_sem_up(&g_sem_id);
        }
    }
}
void example_sem_task2(void)
{
    uint32_t ret;
    printf("example_sem_task2 try get sem g_usSemID wait forever.\n");
    /*永久阻塞模式申请信号量*/
    ret = osal_sem_down_timeout(&g_sem_id, OSAL_WAIT_FOREVER);
    if (OSAL_SUCCESS == ret) {
        printf("example_sem_task2 get sem g_usSemID and then delay 200ms .\n");
    }
    /*任务休眠200ms*/
    osal_mdelay(200);
    printf("example_sem_task2 post sem g_usSemID .\n");
    /*释放信号量*/
    osal_sem_up(&g_sem_id);
}

static void example_task_sem(void)
{
    uint32_t ret;
    osal_task *taskid1, *taskid2;
    /*创建信号量*/
    osal_sem_init(&g_sem_id, 0);
    /*锁任务调度*/
    osal_kthread_lock();
    // 创建任务1
    taskid1 = osal_kthread_create((osal_kthread_handler)example_sem_task1, NULL, "example_task1", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid1, TASK_PRO_TASK1);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    // 创建任务2
    taskid2 = osal_kthread_create((osal_kthread_handler)example_sem_task2, NULL, "example_task2", TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid2, TASK_PRO_TASK2);
    if (ret != OSAL_SUCCESS) {
        printf("create task2 failed .\n");
    }
    osal_kthread_unlock();
    osal_sem_up(&g_sem_id);
    /*任务休眠400ms*/
    osal_mdelay(400);
    /*删除信号量*/
    osal_sem_destroy(&g_sem_id);
    /*删除任务*/
    osal_kthread_destroy(taskid1, 0);
    osal_kthread_destroy(taskid2, 0);
}

static void example_task_entry_sem(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)example_task_sem, NULL, "example_task_sem", SEM_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, SEM_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}

/* Run the muxTask_entry. */
app_run(example_task_entry_sem);
