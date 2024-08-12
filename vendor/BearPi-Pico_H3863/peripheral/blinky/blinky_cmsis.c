/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Blinky Sample Source. \n
 *
 * History: \n
 * 2023-04-03, Create file. \n
 */

#include "pinctrl.h"
#include "soc_osal.h"
#include "gpio.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define BLINKY_TASK_STACK_SIZE    0x1000
#define BLINKY_TASK_PRIO          (osPriority_t)(17)

static void *blinky_task(const char *arg)
{
    unused(arg);
    uapi_pin_set_mode(CONFIG_BLINKY_PIN, PIN_MODE_0);
    uapi_gpio_set_dir(CONFIG_BLINKY_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_BLINKY_PIN, GPIO_LEVEL_LOW);

    while (1) {
        osal_msleep(CONFIG_BLINKY_DURATION_MS);
        uapi_gpio_toggle(CONFIG_BLINKY_PIN);
    }

    return NULL;
}

static void blinky_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "BlinkyTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = BLINKY_TASK_STACK_SIZE;
    attr.priority = BLINKY_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)blinky_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the blinky_entry. */
app_run(blinky_entry);