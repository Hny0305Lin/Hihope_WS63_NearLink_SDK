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
#include "app_init.h"

#define TASKS_TEST_DURATION_MS        5000
#define TASKS_TEST_TASK_PRIO          24
#define TASKS_TEST_TASK_STACK_SIZE    0x1000

static void *tasks_test_task(const char *arg)
{
    unused(arg);

    while (1) {
        osal_msleep(TASKS_TEST_DURATION_MS);
        osal_printk("Hello BS25, Now you can develop SLE Product!\r\n");
    }

    return NULL;
}

static void tasks_test_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)tasks_test_task, 0, "TasksTask",
                                      TASKS_TEST_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TASKS_TEST_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the tasks_test_entry. */
app_run(tasks_test_entry);