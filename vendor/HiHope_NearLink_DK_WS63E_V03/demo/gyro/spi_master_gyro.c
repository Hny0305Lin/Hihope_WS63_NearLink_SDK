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

#include "math.h"
#include "pinctrl.h"
#include "spi.h"
#include "gpio.h"
#include "i2c.h"
#include "watchdog.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "app_init.h"
#include "ssd1306.h"
#include "spi_master_gyro.h"

#define SPI_SLAVE_NUM 1
#define SPI_BUS_CLK 32000000
#define SPI_FREQUENCY 2
#define SPI_CLK_POLARITY 1
#define SPI_CLK_PHASE 1
#define SPI_FRAME_FORMAT 0
#define SPI_FRAME_FORMAT_STANDARD 0
#define SPI_FRAME_SIZE_8 0x1f
#define SPI_TMOD 0
#define SPI_WAIT_CYCLES 0x10
#define CONFIG_SPI_DI_MASTER_PIN 11
#define CONFIG_SPI_DO_MASTER_PIN 9
#define CONFIG_SPI_CLK_MASTER_PIN 7
#define CONFIG_SPI_CS_MASTER_PIN 8
#define CONFIG_SPI_MASTER_PIN_MODE 3
#define CONFIG_SPI_TRANSFER_LEN 2
#define CONFIG_SPI_MASTER_BUS_ID 0
#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_SET_BANDRATE 400000
#define I2C_MASTER_ADDR 0x0

#define SPI_TASK_STACK_SIZE 0x1000
#define SPI_TASK_DURATION_MS 300
#define SPI_TASK_PRIO 17

#define gyroKp (20.0f)     // 比例增益支配率收敛到加速度计/磁强计
#define gyroKi (0.0004f)   // 积分增益支配率的陀螺仪偏见的衔接
#define gyroHalfT (0.005f) // 采样周期的一半
#define PAI 3.14
#define DEGREES 180

float q0 = 1, q1 = 0, q2 = 0, q3 = 0;        // 四元数的元素，代表估计方向
float exInt = 0, eyInt = 0, ezInt = 0;       // 按比例缩小积分误差
float g_gyro_yaw, g_gyro_pitch, g_gyro_roll; // 偏航角，俯仰角，翻滚角
static float yaw_conv = 0.0f;

static void app_spi_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_SPI_DI_MASTER_PIN, CONFIG_SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_DO_MASTER_PIN, CONFIG_SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_CLK_MASTER_PIN, CONFIG_SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_CS_MASTER_PIN, 0);
    uapi_gpio_set_dir(CONFIG_SPI_CS_MASTER_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_SPI_CS_MASTER_PIN, GPIO_LEVEL_HIGH);
}

static void app_i2c_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
}

static uint8_t gyro_spi_write_read(uint8_t addr, uint8_t writedata, uint8_t *rx_data)
{
    uint32_t ret = 0;
    uint8_t tx_data[CONFIG_SPI_TRANSFER_LEN] = {addr, writedata};
    spi_xfer_data_t data = {
        .tx_buff = tx_data,
        .tx_bytes = CONFIG_SPI_TRANSFER_LEN,
        .rx_buff = rx_data,
        .rx_bytes = CONFIG_SPI_TRANSFER_LEN,
    };
    uapi_gpio_set_val(CONFIG_SPI_CS_MASTER_PIN, GPIO_LEVEL_LOW);

    ret = uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &data, 0xFFFFFFFF);
    if (ret != 0) {
        printf("spi = %0x master send failed\r\n", ret);
        return ret;
    }
    ret = uapi_spi_master_read(CONFIG_SPI_MASTER_BUS_ID, &data, 0xFFFFFFFF);
    if (ret != 0) {
        printf("spi = %0x master read failed\r\n", ret);
        return ret;
    }
    for (int i = 0; i < 2; i++) {
        printf("%0x, readbuff[%d] = %0x\r\n", tx_data[0], i, data.rx_buff[i]);
    }
    uapi_gpio_set_val(CONFIG_SPI_CS_MASTER_PIN, GPIO_LEVEL_HIGH);
    return data.rx_buff[1];
}

void IMU_YAW_CAL(float gyroZ)
{
    int ret = 0;
    static char Pitchline[32] = {0};
    static char Rollline[32] = {0};
    static char Yawline[32] = {0};
    static float dt = 0.03; // 0.03代表300ms读取陀螺仪数据
    static float yaw = 0.0f, temp = 0.0f;
// 除去零偏
#if 0
    static int a = 0;
    a++;
    if (hi_get_seconds() <= 5) { // 5s
        printf("---------times-----------:%d\n", a);
    }
#endif
    if (fabs(gyroZ) < 0.04) { // 0.04标准值
        temp = 0;
    } else {
        temp = gyroZ * dt;
    }
    yaw += temp;
    yaw_conv = yaw * 57.32; // 57.32 初始值
    // 360°一个循环
    if (fabs(yaw_conv) > 360.0f) {
        if ((yaw_conv) < 0) {
            yaw_conv += 360.0f;
        } else {
            yaw_conv -= 360.0f;
        }
    }
    ssd1306_SetCursor(0, 15); // 0为横坐标，15为纵坐标
    ret = sprintf(Pitchline, "Pitch: %.2f", g_gyro_pitch);
    if (ret < 0) {
        printf("Pitch failed\r\n");
    }
    ssd1306_DrawString(Pitchline, Font_7x10, White);
    ssd1306_SetCursor(0, 30); // 0为横坐标，30为纵坐标
    ret = sprintf(Rollline, "roll: %.2f", g_gyro_roll);
    if (ret < 0) {
        printf("roll failed\r\n");
    }
    ssd1306_DrawString(Rollline, Font_7x10, White);
    ssd1306_SetCursor(0, 0); // 0为横坐标，0为纵坐标
    ret = sprintf(Yawline, "roll: %.2f", yaw_conv);
    if (ret < 0) {
        printf("yaw failed\r\n");
    }
    ssd1306_DrawString(Yawline, Font_7x10, White);
    ssd1306_UpdateScreen();
}

void GetRoll(float atan2x, float atan2y)
{
    float atan2_x = atan2x;
    float atan2_y = atan2y;
    if (atan2_x > 0) {
        g_gyro_roll = atan(atan2_y / atan2_x) * DEGREES / PAI;
    } else if (atan2_x < 0 && atan2_y >= 0) {
        g_gyro_roll = atan(atan2_y / atan2_x) * DEGREES / PAI + DEGREES;
    } else if (atan2_x < 0 && atan2_y < 0) {
        g_gyro_roll = atan(atan2_y / atan2_x) * DEGREES / PAI - DEGREES;
    } else if (atan2_y > 0 && atan2_x == 0) {
        g_gyro_roll = 90; // 90°
    } else if (atan2_y < 0 && atan2_x == 0) {
        g_gyro_roll = -90; // -90°
    } else {
        printf("undefined\n");
    }
}

void GetPitch(float atan2x, float atan2y)
{
    float atan2_x = atan2x;
    float atan2_y_pitch = atan2y;
    if (atan2_x > 0) {
        g_gyro_pitch = atan(atan2_y_pitch / atan2_x) * DEGREES / PAI;
    } else if (atan2_x < 0 && atan2_y_pitch >= 0) {
        g_gyro_pitch = atan(atan2_y_pitch / atan2_x) * DEGREES / PAI + DEGREES;
    } else if (atan2_x < 0 && atan2_y_pitch < 0) {
        g_gyro_pitch = atan(atan2_y_pitch / atan2_x) * DEGREES / PAI - DEGREES;
    } else if (atan2_y_pitch > 0 && atan2_x == 0) {
        g_gyro_pitch = 90; // 90°
    } else if (atan2_y_pitch < 0 && atan2_x == 0) {
        g_gyro_pitch = -90; // -90°
    } else {
        printf("undefined\n");
    }
}

void IMU_Attitude_cal(float gcx, float gcy, float gcz, float acx, float acy, float acz)
{
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    float atan2_x, atan2_y;
    float atan2_y_pitch;
    float ax = acx, ay = acy, az = acz;
    float gx = gcx, gy = gcy, gz = gcz;

    // 把采集到的三轴加速度转化为单位向量，即向量除以模
    norm = (float)sqrt((float)(ax * ax + ay * ay + az * az));
    if (norm == 0) {
        printf("209 norm = 0,failed\n");
    }
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    // 把四元素换算成方向余弦中的第三行的三个元素
    // vx、vy、vz其实就是上一次的欧拉角(四元数)机体参考坐标系换算出来的重力的单位向量
    vx = 2 * (q1 * q3 - q0 * q2); // 2计算系数
    vy = 2 * (q0 * q1 + q2 * q3); // 2计算系数
    vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

    // 对向量叉乘，求出姿态误差
    // ex、ey、ez为三轴误差元素
    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);

    // 叉乘向量仍旧是机体坐标系上的，而陀螺仪积分误差也是机体坐标系
    // 而且叉积的大小与陀螺仪误差成正比，正好拿来纠正陀螺
    exInt = exInt + ex * gyroKi;
    eyInt = eyInt + ey * gyroKi;
    ezInt = ezInt + ez * gyroKi;

    // 调整后的陀螺仪测量
    gx = gx + gyroKp * ex + exInt;
    gy = gy + gyroKp * ey + eyInt;
    gz = gz + gyroKp * ez + ezInt;

    // 使用一阶龙格库塔解四元数微分方程
    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * gyroHalfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * gyroHalfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * gyroHalfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * gyroHalfT;

    // 四元数归一化
    norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    if (norm == 0) {
        printf("247 norm = 0,failed\n");
    }
    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;

    // 计算姿态角，本文Roll为横滚角，Pitch为俯仰角
    atan2_x = -2 * q1 * q1 - 2 * q2 * q2 + 1; // 2 计算参数
    atan2_y = 2 * q2 * q3 + 2 * q0 * q1;      // 2 计算参数
    GetRoll(atan2_x, atan2_y);
    // 俯仰角
    atan2_y_pitch = -2 * q1 * q3 + 2 * q0 * q2; // 2 计算参数
    GetPitch(atan2_x, atan2_y_pitch);
}

void Lsm_Get_RawAcc(void)
{
    unsigned char buf[12] = {0};
    short acc_x = 0, acc_y = 0, acc_z = 0;
    float acc_x_conv = 0, acc_y_conv = 0, acc_z_conv = 0;
    short ang_rate_x = 0, ang_rate_y = 0, ang_rate_z = 0;
    float ang_rate_x_conv = 0, ang_rate_y_conv = 0, ang_rate_z_conv = 0;
    unsigned char read_buff[2] = {0};
    if ((gyro_spi_write_read(LSM6DSL_STATUS_REG | SPI_READ, 0xff, read_buff) & 0x01) != 0) {
        buf[0] = gyro_spi_write_read(LSM6DSL_OUTX_H_XL | SPI_READ, 0xff, read_buff);
        buf[1] = gyro_spi_write_read(LSM6DSL_OUTX_L_XL | SPI_READ, 0xff, read_buff);
        buf[2] = gyro_spi_write_read(LSM6DSL_OUTY_H_XL | SPI_READ, 0xff, read_buff);
        buf[3] = gyro_spi_write_read(LSM6DSL_OUTY_L_XL | SPI_READ, 0xff, read_buff);
        buf[4] = gyro_spi_write_read(LSM6DSL_OUTZ_H_XL | SPI_READ, 0xff, read_buff);
        buf[5] = gyro_spi_write_read(LSM6DSL_OUTZ_L_XL | SPI_READ, 0xff, read_buff);

        buf[6] = gyro_spi_write_read(LSM6DSL_OUTX_H_G | SPI_READ, 0xff, read_buff);
        buf[7] = gyro_spi_write_read(LSM6DSL_OUTX_L_G | SPI_READ, 0xff, read_buff);
        buf[8] = gyro_spi_write_read(LSM6DSL_OUTY_H_G | SPI_READ, 0xff, read_buff);
        buf[9] = gyro_spi_write_read(LSM6DSL_OUTY_L_G | SPI_READ, 0xff, read_buff);
        buf[10] = gyro_spi_write_read(LSM6DSL_OUTZ_H_G | SPI_READ, 0xff, read_buff);
        buf[11] = gyro_spi_write_read(LSM6DSL_OUTZ_L_G | SPI_READ, 0xff, read_buff);

        ang_rate_x = (buf[6] << 8) | buf[7];   // 将buff6 右移8位在与上buff 7
        ang_rate_y = (buf[8] << 8) | buf[9];   // 将buff8 右移8位在与上buff 9
        ang_rate_z = (buf[10] << 8) | buf[11]; // 将buff10 右移8位在与上buff 11

        acc_x = (buf[0] << 8) | buf[1]; // 将buff0 右移8位在与上buff 1
        acc_y = (buf[2] << 8) | buf[3]; // 将buff2 右移8位在与上buff 3
        acc_z = (buf[4] << 8) | buf[5]; // 将buff4 右移8位在与上buff 5

        ang_rate_x_conv = PAI / 180.0 * ang_rate_x / 14.29; // 180.0代表度数 14.29量程
        ang_rate_y_conv = PAI / 180.0 * ang_rate_y / 14.29; // 180.0代表度数 14.29量程
        ang_rate_z_conv = PAI / 180.0 * ang_rate_z / 14.29; // 180.0代表度数 14.29量程

        acc_x_conv = acc_x / 4098.36; // 4098.36量程
        acc_y_conv = acc_y / 4098.36; // 4098.36量程
        acc_z_conv = acc_z / 4098.36; // 4098.36量程
        IMU_Attitude_cal(ang_rate_x_conv, ang_rate_y_conv, ang_rate_z_conv, acc_x_conv, acc_y_conv, acc_z_conv);
        IMU_YAW_CAL(ang_rate_z_conv);
    }
}

void Lsm6d3s_Init(void)
{
    uint8_t read_buff[CONFIG_SPI_TRANSFER_LEN] = {0};
    gyro_spi_write_read(LSM6DSL_CTRL3_C, 0x34, read_buff);
    gyro_spi_write_read(LSM6DSL_CTRL2_G, 0X4C, read_buff);
    gyro_spi_write_read(LSM6DSL_CTRL10_C, 0x38, read_buff);
    gyro_spi_write_read(LSM6DSL_CTRL1_XL, 0x4F, read_buff);
    gyro_spi_write_read(LSM6DSL_TAP_CFG, 0x10, read_buff);
    gyro_spi_write_read(LSM6DSL_WAKE_UP_DUR, 0x00, read_buff);
    gyro_spi_write_read(LSM6DSL_WAKE_UP_THS, 0x02, read_buff);
    gyro_spi_write_read(LSM6DSL_TAP_THS_6D, 0x40, read_buff);
    gyro_spi_write_read(LSM6DSL_CTRL8_XL, 0x01, read_buff);
}

static void app_spi_master_init_config(void)
{
    spi_attr_t config = {0};
    spi_extra_attr_t ext_config = {0};

    config.is_slave = false;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = SPI_BUS_CLK;
    config.freq_mhz = SPI_FREQUENCY;
    config.clk_polarity = SPI_CLK_POLARITY;
    config.clk_phase = SPI_CLK_PHASE;
    config.frame_format = SPI_FRAME_FORMAT;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    // config.frame_size = SPI_FRAME_SIZE_8; // 80001338代表发送数据的格式不对，需要修改配置参数
    config.frame_size = HAL_SPI_FRAME_SIZE_8;
    config.tmod = SPI_TMOD;
    config.sste = 1;

    ext_config.qspi_param.wait_cycles = SPI_WAIT_CYCLES;
    int ret = uapi_spi_init(CONFIG_SPI_MASTER_BUS_ID, &config, &ext_config);
    if (ret != 0) {
        printf("spi init fail %0x\r\n", ret);
    }
}

static void app_i2c_master_init_config(void)
{
    uint32_t baudrate = I2C_SET_BANDRATE;
    uint32_t hscode = I2C_MASTER_ADDR;
    errcode_t ret = uapi_i2c_master_init(1, baudrate, hscode);
    if (ret != 0) {
        printf("i2c init failed, ret = %0x\r\n", ret);
    }
}

void *spi_gyro_task(void)
{
    uapi_watchdog_disable();
    app_i2c_init_pin();
    app_spi_init_pin();
    app_i2c_master_init_config();
    app_spi_master_init_config();
    ssd1306_Init();
    ssd1306_Fill(Black);
    uint8_t read_buff[CONFIG_SPI_TRANSFER_LEN] = {0};
    while (gyro_spi_write_read(LSM6DSL_WHO_AM_I | SPI_READ, 0xff, read_buff) != 0x6a) {
        osal_mdelay(SPI_TASK_DURATION_MS); // 等待IMU上电复位成功
    }
    Lsm6d3s_Init();
    while (1) {
        osal_mdelay(1);
        Lsm_Get_RawAcc();
    }
    return 0;
}

static void spi_master_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)spi_gyro_task, 0, "spi_gyro_task", SPI_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SPI_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the spi_master_entry. */
app_run(spi_master_entry);