/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: WDT Sample Source. \n
 *
 * History: \n
 * 2023-06-29, Create file. \n
 */
#include "pinctrl.h"
#include "watchdog.h"
#include "soc_osal.h"
#include "app_init.h"

#define TIME_OUT                  2
#define WDT_MODE                  1
#define TEST_PARAM_KICK_TIME      10
#define WDT_TASK_DURATION_MS      500

#define WDT_TASK_PRIO             24
#define WDT_TASK_STACK_SIZE       0x1000

static errcode_t watchdog_callback(uintptr_t param)
{
    UNUSED(param);
    osal_printk("watchdog kick timeout!\r\n");
    return ERRCODE_SUCC;
}

static void *watchdog_task(const char *arg)
{
    UNUSED(arg);

    errcode_t ret = uapi_watchdog_init(TIME_OUT);
    if (ret == ERRCODE_INVALID_PARAM) {
        osal_printk("param is error, timeout is %d.\r\n", TIME_OUT);
        return NULL;
    }
    (void)uapi_watchdog_enable((wdt_mode_t)WDT_MODE);
    (void)uapi_register_watchdog_callback(watchdog_callback);
    osal_printk("init watchdog\r\n");
#if defined(CONFIG_WDT_TIMEOUT_SAMPLE)
    while (1) {};
#endif

#if defined(CONFIG_WDT_KICK_SAMPLE)
    while (1) {
        osal_msleep(WDT_TASK_DURATION_MS);
        (void)uapi_watchdog_kick();
        osal_printk("kick success\r\n");
    }
#endif
    (void)uapi_watchdog_deinit();
    return NULL;
}

static void watchdog_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)watchdog_task, 0, "WatchdogTask", WDT_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, WDT_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the watchdog_entry. */
app_run(watchdog_entry);