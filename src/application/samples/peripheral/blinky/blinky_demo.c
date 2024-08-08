/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Blinky Sample Source. \n
 *
 * History: \n
 * 2023-04-03, Create file. \n
 */
#include "boards.h"
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"

#define BLINKY_DURATION_MS        500

#define BLINKY_TASK_PRIO          24
#define BLINKY_TASK_STACK_SIZE    0x1000

static int blinky_task(const char *arg)
{
    unused(arg);

    uapi_pin_set_mode(BSP_LED_0, HAL_PIO_FUNC_GPIO);

    uapi_gpio_set_dir(BSP_LED_0, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(BSP_LED_0, GPIO_LEVEL_LOW);

    while (1) {
        osal_msleep(BLINKY_DURATION_MS);
        uapi_gpio_toggle(BSP_LED_0);
        osal_printk("Blinky working.\r\n");
    }

    return 0;
}

static void blinky_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)blinky_task, 0, "BlinkyTask", BLINKY_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLINKY_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the blinky_entry. */
app_run(blinky_entry);