/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Tasks Sample Source. \n
 *
 * History: \n
 * 2023-04-03, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define TASKS_TEST_TASK_STACK_SIZE    0x1000
#define TASKS_TEST_TASK_PRIO          (osPriority_t)(17)
#define TASKS_TEST_DURATION_MS        5000

static void *tasks_test_task(const char *arg)
{
    unused(arg);

    while (1) {
        osal_msleep(TASKS_TEST_DURATION_MS);
        osal_printk("Hello BearPi!\r\n");
    }

    return NULL;
}

static void tasks_test_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "TasksTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASKS_TEST_TASK_STACK_SIZE;
    attr.priority = TASKS_TEST_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)tasks_test_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the tasks_test_entry. */
app_run(tasks_test_entry);