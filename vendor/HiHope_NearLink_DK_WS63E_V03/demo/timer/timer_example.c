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
#include "timer.h"
#include "osal_debug.h"
#include "app_init.h"
#include "tcxo.h"
#include "chip_core_irq.h"

#define TIMER_TASK_STACK_SIZE 0x1000
#define TIMER_TASK_PRIO 17 // 任务的优先级，数值越小优先级越高
#define TIMER_TIMERS_NUM 2
#define TIMER_INDEX 1
#define TIMER_PRIO 1
#define TIMER_DELAY_INT 5
#define TIMER1_DELAY_1S 1000000 // 延时1s
#define TIMER2_DELAY_2S 2000000 // 延时2s

timer_handle_t timer_index[TIMER_TIMERS_NUM] = {0};
typedef struct timer_info {
    uint32_t start_time;
    uint32_t end_time;
    uint32_t delay_time;
} timer_info_t;

static timer_info_t g_timers_info[TIMER_TIMERS_NUM] = {{0, 0, TIMER1_DELAY_1S}, {0, 0, TIMER2_DELAY_2S}};

/**
 * @brief Callback for Timer1 triggering
 *
 */
void Timer1Callback(uintptr_t data)
{
    uint32_t timer = (uint32_t)data;
    g_timers_info[timer].end_time = uapi_tcxo_get_ms();
    osal_printk("This is Timer1Callback real time = %dms\r\n", // 单定时器
                (g_timers_info[0].end_time - g_timers_info[0].start_time));
}

/**
 * @brief Callback for Timer2 triggering
 *
 */
void Timer2Callback(uintptr_t data)
{
    uint32_t timer = (uint32_t)data;
    g_timers_info[timer].end_time = uapi_tcxo_get_ms();
    osal_printk("This is Timer2Callback real time = %dms\r\n",
                (g_timers_info[1].end_time - g_timers_info[1].start_time));
    g_timers_info[1].start_time = g_timers_info[1].end_time;
    uapi_timer_start(timer_index[1], g_timers_info[1].delay_time, Timer2Callback, 1); // 周期定时器
}

/**
 * @brief Main Entry of the Timer Example
 *
 */
static void *timer_task(const char *arg)
{
    unused(arg);
    uapi_timer_init();
    uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO);
    // 创建单次软件定时，1s
    uapi_timer_create(TIMER_INDEX, &timer_index[0]);
    g_timers_info[0].start_time = uapi_tcxo_get_ms();
    uapi_timer_start(timer_index[0], g_timers_info[0].delay_time, Timer1Callback, 0); // 0代表定时器参数，用于传递给定时器回调函数
    // 创建周期性软件定时，2s
    uapi_timer_create(TIMER_INDEX, &timer_index[1]);
    g_timers_info[1].start_time = uapi_tcxo_get_ms();
    uapi_timer_start(timer_index[1], g_timers_info[1].delay_time, Timer2Callback, 1); // 1代表定时器参数，用于传递给定时器回调函数
    return NULL;
}

static void timer_entry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)timer_task, NULL, "timer_task",
                                 TIMER_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, TIMER_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task failed .\n");
    }
}
/* Run the timer_entry. */
app_run(timer_entry);