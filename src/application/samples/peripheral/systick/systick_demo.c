/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SYSTICK Sample Source. \n
 *
 * History: \n
 * 2023-07-17, Create file. \n
 */
#include "pinctrl.h"
#include "systick.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#define SYSTICK_DELAY_S                2
#define SYSTICK_DELAY_MS               1000
#define SYSTICK_DELAY_US               20000
#define SYSTICK_TASK_DURATION_MS       500

#define SYSTICK_TASK_PRIO              24
#define SYSTICK_TASK_STACK_SIZE        0x1000

static void *systick_task(const char *arg)
{
    unused(arg);
    uint64_t count_before_get_s;
    uint64_t count_after_get_s;
    uint64_t count_before_get_ms;
    uint64_t count_after_get_ms;
    uint64_t count_before_get_us;
    uint64_t count_after_get_us;

    /* SYSTICK init. */
    uapi_systick_init();

    while (1) {
        osal_msleep(SYSTICK_TASK_DURATION_MS);
        osal_printk("systick delay %ds!\r\n", SYSTICK_DELAY_S);
        count_before_get_s = uapi_systick_get_s();
        uapi_systick_delay_s(SYSTICK_DELAY_S);
        count_after_get_s = uapi_systick_get_s();
        osal_printk("count_after_get_s = %llu, count_before_get_s = %llu\r\n", count_after_get_s, count_before_get_s);
        osal_printk("count_s = %llu\r\n", count_after_get_s - count_before_get_s);
        if (count_after_get_s > count_before_get_s) {
            osal_printk("systick get s work normall.\r\n");
        }

        osal_printk("systick delay %dms!\r\n", SYSTICK_DELAY_MS);
        count_before_get_ms = uapi_systick_get_ms();
        uapi_systick_delay_ms(SYSTICK_DELAY_MS);
        count_after_get_ms = uapi_systick_get_ms();
        osal_printk("count_after_get_ms = %llu, count_before_get_ms = %llu\r\n", count_after_get_ms,
                    count_before_get_ms);
        osal_printk("count_ms = %llu\r\n", count_after_get_ms - count_before_get_ms);
        if (count_after_get_ms > count_before_get_ms) {
            osal_printk("systick get ms work normall.\r\n");
        }

        osal_printk("systick delay %dus!\r\n", SYSTICK_DELAY_US);
        count_before_get_us = uapi_systick_get_us();
        uapi_systick_delay_us(SYSTICK_DELAY_US);
        count_after_get_us = uapi_systick_get_us();
        osal_printk("count_after_get_us = %llu, count_before_get_us = %llu\r\n", count_after_get_us,
                    count_before_get_us);
        osal_printk("count_us = %llu\r\n", count_after_get_us - count_before_get_us);
        if (count_after_get_us > count_before_get_us) {
            osal_printk("systick get us work normall.\r\n");
        }
    }

    return NULL;
}

static void systick_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)systick_task, 0, "SystickTask", SYSTICK_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SYSTICK_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the systick_entry. */
app_run(systick_entry);