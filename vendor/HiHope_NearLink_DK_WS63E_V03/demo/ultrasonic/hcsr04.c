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

/*
    HCSR04 超声波模块的相关API接口
*/

#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "gpio.h"
#include "systick.h"
#include "i2c.h"
#include "osal_debug.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"
#include "watchdog.h"
#include "app_init.h"

#define DELAY_US20 20
#define DELAY_10MS 10
#define BSP_LED_0 0
#define BSP_LED_1 1
#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_MASTER_ADDR 0x0
#define I2C_SLAVE1_ADDR 0x38
#define I2C_SET_BANDRATE 400000
#define HCSR04_TASK_STACK_SIZE 0x1000
#define HCSR04_TASK_PRIO 17

void app_i2c_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
}

void Hcsr04Init(void)
{

    uapi_pin_set_mode(BSP_LED_0, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(BSP_LED_0, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(BSP_LED_0, GPIO_LEVEL_LOW);
    uapi_pin_set_mode(BSP_LED_1, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(BSP_LED_1, GPIO_DIRECTION_INPUT);
}

float GetDistance(void)
{
    static uint64_t start_time = 0, time = 0;
    float distance = 0.0;
    gpio_level_t value = 0;
    unsigned int flag = 0;
    static char line[32] = {0};
    /*
     * 设置GPIO7输出低电平
     * 给trig发送至少10us-20us的高电平脉冲，以触发传感器测距
     * Set GPIO7 to output direction
     * Send a high level pulse of at least 10us to the trig to trigger the range measurement of the sensor
     */
    uapi_gpio_set_val(BSP_LED_0, GPIO_LEVEL_HIGH);
    uapi_systick_delay_us(DELAY_US20);
    uapi_gpio_set_val(BSP_LED_0, GPIO_LEVEL_LOW);
    /*
     * 计算与障碍物之间的距离
     * Calculate the distance from the obstacle
     */
    while (1) {
        /*
         * 获取GPIO8的输入电平状态
         * Get the input level status of GPIO8
         */
        value = uapi_gpio_get_output_val(BSP_LED_1);
        /*
         * 判断GPIO8的输入电平是否为高电平并且flag为0
         * Judge whether the input level of GPIO8 is high and the flag is 0
         */
        if (value == GPIO_LEVEL_HIGH && flag == 0) {
            /*
             * 获取系统时间
             * get SysTime
             */
            start_time = uapi_systick_get_us();
            flag = 1;
        }
        /*
         * 判断GPIO8的输入电平是否为低电平并且flag为1
         * Judge whether the input level of GPIO8 is low and the flag is 1
         */
        if (value == GPIO_LEVEL_LOW && flag == 1) {
            /*
             * 获取高电平持续时间
             * Get high level duration
             */
            time = uapi_systick_get_us() - start_time;
            break;
        }
    }
    /* 计算距离障碍物距离（340米/秒 转换为 0.034厘米/微秒, 2代表去来，两倍距离） */
    /* Calculate the distance from the obstacle */
    /* (340 m/s is converted to 0.034 cm/microsecond 2 represents going and coming, twice the distance) */
    distance = time * 0.034 / 2;
    sprintf(line, "distance: %.2f", distance);
    ssd1306_SetCursor(0, 0);
    ssd1306_DrawString(line, Font_7x10, White);
    ssd1306_UpdateScreen();
    return distance;
}

void Hcsr04SampleTask(void)
{
    Hcsr04Init();
    printf("Hcsr04SampleTask init\r\n");
    uint32_t baudrate = I2C_SET_BANDRATE;
    uint32_t hscode = I2C_MASTER_ADDR;
    app_i2c_init_pin();
    errcode_t ret = uapi_i2c_master_init(1, baudrate, hscode);
    if (ret != 0) {
        printf("i2c init failed, ret = %0x\r\n", ret);
    }
    ssd1306_Init();
    ssd1306_Fill(Black);
    while (1) {
        GetDistance();
        osal_mdelay(DELAY_10MS);
    }
}

void Hcsr04SampleEntry(void)
{
    uint32_t ret;
    osal_task *taskid;
    // 创建任务调度
    osal_kthread_lock();
    // 创建任务1
    taskid = osal_kthread_create((osal_kthread_handler)Hcsr04SampleTask, NULL, "Hcsr04Task", HCSR04_TASK_STACK_SIZE);
    ret = osal_kthread_set_priority(taskid, HCSR04_TASK_PRIO);
    if (ret != OSAL_SUCCESS) {
        printf("create task1 failed .\n");
    }
    osal_kthread_unlock();
}
app_run(Hcsr04SampleEntry);