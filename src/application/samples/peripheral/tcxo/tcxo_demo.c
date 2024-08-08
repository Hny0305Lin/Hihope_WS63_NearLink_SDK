/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: TCXO Sample Source. \n
 *
 * History: \n
 * 2023-07-17, Create file. \n
 */
#include "tcxo.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define TCXO_DELAY_MS             1000
#define TCXO_DELAY_US             20000
#define TCXO_TASK_DURATION_MS     500

#define TCXO_TASK_PRIO            24
#define TCXO_TASK_STACK_SIZE      0x1000

static void *tcxo_task(const char *arg)
{
    unused(arg);
    uint64_t count_before_get_ms;
    uint64_t count_after_get_ms;
    uint64_t count_before_get_us;
    uint64_t count_after_get_us;

    /* TCXO init. */
    uapi_tcxo_init();

    while (1) {
        osal_msleep(TCXO_TASK_DURATION_MS);
        osal_printk("tcxo delay %dms!\r\n", TCXO_DELAY_MS);
        count_before_get_ms = uapi_tcxo_get_ms();
        uapi_tcxo_delay_ms(TCXO_DELAY_MS);
        count_after_get_ms = uapi_tcxo_get_ms();
        osal_printk("count_after_get_ms = %llu, count_before_get_ms = %llu\r\n", count_after_get_ms,
                    count_before_get_ms);
        osal_printk("count_ms = %llu\r\n", count_after_get_ms - count_before_get_ms);
        if (count_after_get_ms > count_before_get_ms) {
            osal_printk("tcxo get ms work normall.\r\n");
        }

        osal_printk("tcxo delay %dus!\r\n", TCXO_DELAY_US);
        count_before_get_us = uapi_tcxo_get_us();
        uapi_tcxo_delay_us(TCXO_DELAY_US);
        count_after_get_us = uapi_tcxo_get_us();
        osal_printk("count_after_get_us = %llu, count_before_get_us = %llu\r\n", count_after_get_us,
                    count_before_get_us);
        osal_printk("count_us = %llu\r\n", count_after_get_us - count_before_get_us);
        if (count_after_get_us > count_before_get_us) {
            osal_printk("tcxo get us work normall.\r\n");
        }
    }

    return NULL;
}

static void tcxo_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)tcxo_task, 0, "TcxoTask", TCXO_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TCXO_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the tcxo_entry. */
app_run(tcxo_entry);