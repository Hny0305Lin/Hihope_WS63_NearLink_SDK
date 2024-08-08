/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: osal adapt
 *
 * Create: 2023-01-11
 */
#ifndef __OSAL_ADAPT_H__
#define __OSAL_ADAPT_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include "td_type.h"
#include "osal_types.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/************************ osal_atomic ************************/
extern int osal_adapt_atomic_init(osal_atomic *atomic);
extern void osal_adapt_atomic_destroy(osal_atomic *atomic);
extern int osal_adapt_atomic_read(osal_atomic *atomic);
extern void osal_adapt_atomic_set(osal_atomic *atomic, int val);
extern int osal_adapt_atomic_inc_return(osal_atomic *atomic);
extern int osal_adapt_atomic_dec_return(osal_atomic *atomic);
extern void osal_adapt_atomic_inc(osal_atomic *atomic);
extern void osal_adapt_atomic_dec(osal_atomic *atomic);
extern void osal_adapt_atomic_add(osal_atomic *atomic, int val);

/************************ osal_timer ************************/
extern int osal_adapt_timer_init(osal_timer *timer, void *func, unsigned long data, unsigned int interval);
extern unsigned int osal_adapt_jiffies_to_msecs(const unsigned int n);
extern int osal_adapt_timer_destroy(osal_timer *timer);
extern unsigned long long osal_adapt_get_jiffies(void);
extern int osal_adapt_timer_mod(osal_timer *timer, unsigned int interval);

/************************ osal_task ************************/
extern osal_task *osal_adapt_kthread_create(osal_kthread_handler thread, void *data,
    const char *name, unsigned int stack_size);
extern void osal_adapt_kthread_lock(void);
extern void osal_adapt_kthread_unlock(void);
extern long osal_adapt_get_current_tid(void);
extern int osal_adapt_kthread_should_stop(void);
extern void osal_adapt_kthread_destroy(osal_task *task, unsigned int stop_flag);
extern int osal_adapt_kthread_set_priority(osal_task *task, unsigned int priority);
extern int osal_adapt_workqueue_init(osal_workqueue *work, osal_workqueue_handler handler);
extern void osal_adapt_workqueue_destroy(osal_workqueue *work);

/************************ osal_intertupt ************************/
extern unsigned int osal_adapt_irq_lock(void);
extern void osal_adapt_irq_restore(unsigned int irq_status);

/************************ osal_wait ************************/
extern void osal_adapt_wait_destroy(osal_wait *wait);
extern void osal_adapt_wait_wakeup(osal_wait *wait);

/************************ osal_event ************************/
extern int osal_adapt_event_init(osal_event *event_obj);
extern int osal_adapt_event_write(osal_event *event_obj, unsigned int mask);
extern int osal_adapt_event_read(osal_event *event_obj, unsigned int mask, unsigned int timeout_ms, unsigned int mode);
extern int osal_adapt_event_clear(osal_event *event_obj, unsigned int mask);
extern int osal_adapt_event_destroy(osal_event *event_obj);

/************************ osal_string ************************/
extern int osal_adapt_strncmp(const char *str1, const char *str2, unsigned long size);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif