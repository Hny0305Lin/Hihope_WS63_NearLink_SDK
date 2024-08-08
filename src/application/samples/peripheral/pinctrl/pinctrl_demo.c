/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: PINCTRL Sample Source. \n
 *
 * History: \n
 * 2023-07-27, Create file. \n
 */
#include "pinctrl.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define PINCTRL_PIN_MODE           5
#define PINCTRL_PIN_DS             3
#define PINCTRL_PIN_PULL           2

#define PINCTRL_TASK_PRIO          24
#define PINCTRL_TASK_STACK_SIZE    0x1000

static void *pinctrl_task(const char *arg)
{
    unused(arg);
    pin_t pin = CONFIG_PINCTRL_USE_PIN;
    pin_mode_t mode;
    pin_drive_strength_t ds;
    pin_pull_t pull;

    /* PINCTRL init. */
    uapi_pin_init();

    osal_printk("start get pin<%d> mode!\r\n", pin);
    mode = uapi_pin_get_mode(pin);
    osal_printk("the mode of pin<%d> is %d.\r\n", pin, mode);
    mode = PINCTRL_PIN_MODE;
    osal_printk("start set pin<%d> mode<%d>!\r\n", pin, mode);
    if (uapi_pin_set_mode(pin, mode) == ERRCODE_SUCC && uapi_pin_get_mode(pin) == mode) {
        osal_printk("set pin<%d> mode<%d> succ.\r\n", pin, mode);
    }

    osal_printk("\r\n");
    osal_printk("start get pin<%d> driver-strength!\r\n", pin);
    ds = uapi_pin_get_ds(pin);
    osal_printk("The driver-strength of pin<%d> is %d.\r\n", pin, ds);
    ds = PINCTRL_PIN_DS;
    osal_printk("start set pin<%d> driver-strength<%d>!\r\n", pin, ds);
    if (uapi_pin_set_ds(pin, ds) == ERRCODE_SUCC && uapi_pin_get_ds(pin) == ds) {
        osal_printk("set pin<%d> driver-strength<%d> succ.\r\n", pin, ds);
    }

    osal_printk("\r\n");
    osal_printk("start get pin<%d> pull/down status!\r\n", pin);
    pull = uapi_pin_get_pull(pin);
    osal_printk("The pull/down status of pin<%d> is %d.\r\n", pin, pull);
    pull = PINCTRL_PIN_PULL;
    osal_printk("start set pin<%d> pull/down status<%d>!\r\n", pin, pull);
    if (uapi_pin_set_pull(pin, pull) == ERRCODE_SUCC && uapi_pin_get_pull(pin) == pull) {
        osal_printk("set pin<%d> pull/down status<%d> succ.\r\n", pin, pull);
    }

    /* PINCTRL deinit. */
    uapi_pin_deinit();

    return NULL;
}

static void pinctrl_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)pinctrl_task, 0, "PinctrlTask", PINCTRL_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, PINCTRL_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the pinctrl_entry. */
app_run(pinctrl_entry);