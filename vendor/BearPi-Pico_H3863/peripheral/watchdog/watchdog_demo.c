/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: WDT Sample Source. \n
 *
 * History: \n
 * 2023-06-29, Create file. \n
 */
#include "pinctrl.h"
#include "soc_osal.h"
#include "watchdog.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define WDT_TASK_STACK_SIZE       0x1000
#define WDT_TASK_DURATION_MS      500
#define WDT_TASK_PRIO             (osPriority_t)(17)
#define TIME_OUT                  2
#define WDT_MODE                  1
#define TEST_PARAM_KICK_TIME      10

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
    osThreadAttr_t attr;

    attr.name = "WatchdogTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = WDT_TASK_STACK_SIZE;
    attr.priority = WDT_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)watchdog_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the watchdog_entry. */
app_run(watchdog_entry);