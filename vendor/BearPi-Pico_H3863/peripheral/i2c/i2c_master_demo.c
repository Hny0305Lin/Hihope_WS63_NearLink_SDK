/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: I2C Sample Source. \n
 *
 * History: \n
 * 2023-05-25, Create file. \n
 */
#include "pinctrl.h"
#include "soc_osal.h"
#include "i2c.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"

#include "ssd1306_fonts.h"
#include "ssd1306.h"

#define I2C_MASTER_ADDR                   0x0
#define I2C_SET_BAUDRATE                  400000
#define I2C_MASTER_PIN_MODE               2

#define I2C_TASK_STACK_SIZE               0x1000
#define I2C_TASK_DURATION_MS              500
#define I2C_TASK_PRIO                     (osPriority_t)(17)

static void app_i2c_init_pin(void)
{
    /* I2C pinmux. */
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, I2C_MASTER_PIN_MODE);       
    uapi_pin_set_pull(CONFIG_I2C_SCL_MASTER_PIN, PIN_PULL_TYPE_UP);
    uapi_pin_set_pull(CONFIG_I2C_SDA_MASTER_PIN, PIN_PULL_TYPE_UP);
}

static void *i2c_master_task(const char *arg)
{
    unused(arg);

    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint8_t hscode = I2C_MASTER_ADDR;

    /* I2C master init config. */

    app_i2c_init_pin();
    uapi_i2c_master_init(CONFIG_I2C_MASTER_BUS_ID, baudrate, hscode);

    ssd1306_Init();
    ssd1306_Fill(Black);

    while (1) {
            osal_msleep(I2C_TASK_DURATION_MS);
            ssd1306_SetCursor(0, 0);
            ssd1306_DrawString("Hello BearPi", Font_7x10, White);
            ssd1306_UpdateScreen();
    }
    return NULL;
}

static void i2c_master_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "I2cMasterTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = I2C_TASK_STACK_SIZE;
    attr.priority = I2C_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)i2c_master_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the i2c_master_entry. */
app_run(i2c_master_entry);