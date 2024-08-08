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

/* 由于舵机的频率为50HZ，PWM不支持这么小的频率，所以采用GPIO模拟PWM波形 */

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "systick.h"
#include "test_suite_log.h"
#include "watchdog.h"
#include "app_init.h"

#define COUNT 10 // 通过计算舵机转到对应角度需要发送10个左右的波形
#define SG92R_TASK_STACK_SIZE 0x1000
#define SG92R_TASK_PRIO 17
#define BSP_SG92R 2
#define FREQ_TIME 20000

void SetAngle(unsigned int duty)
{
    unsigned int time = FREQ_TIME;

    uapi_gpio_set_val(BSP_SG92R, GPIO_LEVEL_HIGH);
    uapi_systick_delay_us(duty);
    uapi_gpio_set_val(BSP_SG92R, GPIO_LEVEL_LOW);
    uapi_systick_delay_us(time - duty);
}

/* The steering gear is centered
 * 1、依据角度与脉冲的关系，设置高电平时间为1500微秒
 * 2、不断地发送信号，控制舵机居中
 */
void RegressMiddle(void)
{
    unsigned int angle = 1500;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
    }
}

/* Turn 90 degrees to the right of the steering gear
 * 1、依据角度与脉冲的关系，设置高电平时间为500微秒
 * 2、不断地发送信号，控制舵机向右旋转90度
 */
/*  Steering gear turn right */
void EngineTurnRight(void)
{
    unsigned int angle = 500;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
    }
}

/* Turn 90 degrees to the left of the steering gear
 * 1、依据角度与脉冲的关系，设置高电平时间为2500微秒
 * 2、不断地发送信号，控制舵机向左旋转90度
 */
/* Steering gear turn left */
void EngineTurnLeft(void)
{
    unsigned int angle = 2500;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
    }
}

void S92RInit(void)
{
    uapi_pin_set_mode(BSP_SG92R, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(BSP_SG92R, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(BSP_SG92R, GPIO_LEVEL_LOW);
}

void Sg92RTask(void)
{
    unsigned int time = 200;
    S92RInit();

    while (1) {
        uapi_watchdog_kick();
        /* 舵机归中 Steering gear centering */
        RegressMiddle();
        uapi_systick_delay_ms(time);

        /*
         * 舵机左转90度
         * Steering gear turns 90 degrees to the left
         */
        EngineTurnLeft();
        uapi_systick_delay_ms(time);

        /* 舵机归中 Steering gear centering */
        RegressMiddle();
        uapi_systick_delay_ms(time);

        /*
         * 舵机右转90度
         * Steering gear turns right 90 degrees
         */
        EngineTurnRight();
        uapi_systick_delay_ms(time);

        /* 舵机归中 Steering gear centering */
        RegressMiddle();
        uapi_systick_delay_ms(time);
    }
}

void SG92RSampleEntry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)Sg92RTask, NULL, "Sg92RTask", SG92R_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, SG92R_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}
app_run(SG92RSampleEntry);