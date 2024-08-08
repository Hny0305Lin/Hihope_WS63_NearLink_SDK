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

#define MSESSAGE_TASK_SEND_PRIO 25
#define MSESSAGE_TASK_RECV_PRIO 26
#define MSESSAGE_TASK_PRIO 24
#define MSESSAGE_TASK_STACK_SIZE 0x1000
#define MSG_QUEUE_SIZE 18 // 每数据最大18字节
#define MSG_MAX_LEN 18    // 可存储18条数据

static unsigned long g_msg_queue;
uint8_t abuf[] = "test is message x";

/* 任务1发送数据 */
void example_send_task(void)
{
    printf("send task start\r\n");
    uint32_t i = 0, ret = 0;
    uint32_t uwlen = sizeof(abuf);
    while (i < 5) {
        abuf[uwlen - 2] = '0' + i;
        i++;
        /*将abuf里的数据写入队列*/
        ret = osal_msg_queue_write_copy(g_msg_queue, abuf, sizeof(abuf), OSAL_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS) {
            printf("send message failure,error:%#x\n", ret);
        }
        osal_msleep(5);
    }
}

/* 任务2接收数据 */
void example_recv_task(void)
{
    printf("recv task start\r\n");
    uint8_t msg[2024];
    uint32_t ret = 0;
    // 设置缓存去大小及存放数据读取的消息大小
    uint32_t msg_rev_size = 2024;
    while (1) {
        /* 读取队列里的数据存入msg里 */
        ret = osal_msg_queue_read_copy(g_msg_queue, msg, &msg_rev_size, OSAL_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS) {
            printf("recv message failure,error:%#x\n", ret);
            break;
        }
        printf("recv message:%s\r\n", (char *)msg);
        osal_msleep(5);
    }
    /* 删除队列。需根据具体情况删除，大多数情况下无需删除队列，且在有任务占用等情况下删除队
       列会导致失败。以下代码仅供API展示 */
    // osal_msg_queue_delete(g_msg_queue);
    // printf("delete the queue success!\n");
}

void example_task_msg(void)
{
    uint32_t ret;
    osal_task *taskid1, *taskid2;
    ret = osal_msg_queue_create("name", MSG_QUEUE_SIZE, &g_msg_queue, 0, MSG_MAX_LEN);
    if (ret != OSAL_SUCCESS) {
        printf("create queue failure!,error:%x\n", ret);
    }
    printf("create the queue success! queue_id = %d\n", g_msg_queue);
    // 创建任务1
    taskid1 = osal_kthread_create((osal_kthread_handler)example_send_task, NULL, "task1", MSESSAGE_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid1, MSESSAGE_TASK_SEND_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    // 创建任务2
    taskid2 = osal_kthread_create((osal_kthread_handler)example_recv_task, NULL, "task2", MSESSAGE_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid2, MSESSAGE_TASK_RECV_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task2 failed .\n");
    }
}

static void example_task_entry_message(void)
{
    uint32_t ret;
    osal_task *taskid;
    osal_kthread_lock();
    taskid = osal_kthread_create((osal_kthread_handler)example_task_msg, NULL, "message", MSESSAGE_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, MSESSAGE_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}

/* Run the muxTask_entry. */
app_run(example_task_entry_message);