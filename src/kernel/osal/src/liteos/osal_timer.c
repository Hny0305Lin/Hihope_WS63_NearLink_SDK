/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: timer
 *
 * Create: 2021-12-16
 */

#include <los_swtmr.h>
#include <los_memory.h>
#include <los_config.h>
#include <time.h>
#include <limits.h>
#include <linux/hrtimer.h>
#include "soc_osal.h"
#include "osal_errno.h"
#include "osal_inner.h"

#define US_TO_NSEC  1000
#define MS_TO_USEC  1000

#ifdef LOSCFG_COMPAT_LINUX_HRTIMER
typedef enum hrtimer_restart (*hrtimer_function)(struct hrtimer *timer);

int osal_hrtimer_create(osal_hrtimer *phrtimer)
{
    int ret;
    if (phrtimer == NULL || phrtimer->timer != NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    if (phrtimer->interval > (ULONG_MAX / MS_TO_USEC)) {
        osal_log("interval is too large!\n");
        return OSAL_FAILURE;
    }
    union ktime stime;
    stime.tv.sec = 0;
    stime.tv.usec = (phrtimer->interval) * MS_TO_USEC;

    phrtimer->timer = (struct hrtimer *)LOS_MemAlloc((void*)m_aucSysMem0, sizeof(struct hrtimer));
    if (phrtimer->timer == NULL) {
        osal_log("LOS_MemAlloc failed!\n");
        return OSAL_FAILURE;
    }
    ret = hrtimer_create((struct hrtimer *)phrtimer->timer, stime, (hrtimer_function)phrtimer->handler);
    if (ret != 0) {
        osal_log("hrtimer_create failed!\n");
        LOS_MemFree((void*)m_aucSysMem0, phrtimer->timer);
        phrtimer->timer = NULL;
        return OSAL_FAILURE;
    }
    return ret;
}

int osal_hrtimer_start(osal_hrtimer *phrtimer)
{
    if (phrtimer == NULL || phrtimer->timer == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    if (phrtimer->interval > (ULONG_MAX / MS_TO_USEC)) {
        osal_log("interval is too large!\n");
        return OSAL_FAILURE;
    }
    union ktime stime;
    stime.tv.sec = 0;
    stime.tv.usec = (phrtimer->interval) * MS_TO_USEC;
    return hrtimer_start((struct hrtimer *)phrtimer->timer, stime, HRTIMER_MODE_REL);
}

int osal_hrtimer_destroy(osal_hrtimer *phrtimer)
{
    if ((phrtimer == NULL) || (phrtimer->timer == NULL)) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    hrtimer_cancel((struct hrtimer *)phrtimer->timer);
    LOS_MemFree((void*)m_aucSysMem0, phrtimer->timer);
    phrtimer->timer = NULL;
    return OSAL_SUCCESS;
}
#endif

unsigned long osal_timer_get_private_data(const void *sys_data)
{
    return (unsigned long)(UINTPTR)sys_data;
}

int osal_timer_init(osal_timer *timer)
{
    int ret;
    if (timer == NULL || timer->handler == NULL || timer->timer != NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    if (LOS_MS2Tick(timer->interval) <= 0) {
        osal_log("interval:%d invalid!\n", timer->interval);
        return OSAL_FAILURE;
    }
    // osal_timer.data will be used by osal_timer.handler(osal_timer.data);
    ret = LOS_SwtmrCreate(LOS_MS2Tick(timer->interval), LOS_SWTMR_MODE_NO_SELFDELETE,
        (SWTMR_PROC_FUNC)timer->handler, (unsigned short *)(&(timer->timer)), timer->data);
    if (ret != LOS_OK) {
        osal_log("LOS_SwtmrCreate failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

/*
 * timer will restart after modify.
 */
int osal_timer_mod(osal_timer *timer, unsigned int interval)
{
    unsigned int ret;

    if (timer == NULL || timer->handler == NULL || LOS_MS2Tick(interval) <= 0) {
        osal_log("interval=%d parameter invalid!\n", interval);
        return OSAL_FAILURE;
    }
    ret = LOS_SwtmrDelete((unsigned short)(UINTPTR)(timer->timer));
    if (ret != LOS_OK) {
        osal_log("LOS_SwtmrDelete failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    timer->interval = interval;
    // osal_timer.data will be used by osal_timer.handler(osal_timer.data);
    ret = LOS_SwtmrCreate(LOS_MS2Tick(timer->interval), LOS_SWTMR_MODE_NO_SELFDELETE,
        (SWTMR_PROC_FUNC)timer->handler, (unsigned short *)(&(timer->timer)), timer->data);
    if (ret != LOS_OK) {
        osal_log("LOS_SwtmrCreate failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    ret = LOS_SwtmrStart((unsigned short)(UINTPTR)(timer->timer));
    if (ret != LOS_OK) {
        osal_log("LOS_SwtmrStart failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

/*
 * if the timer is timing, this function will restart the timer
 */
int osal_timer_start(osal_timer *timer)
{
    if (timer == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    int ret = LOS_SwtmrStart((unsigned short)(UINTPTR)(timer->timer));
    if (ret != LOS_OK) {
        osal_log("LOS_SwtmrStart failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

int osal_timer_stop(osal_timer *timer)
{
    if (timer == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    int ret = LOS_SwtmrStop((unsigned short)(UINTPTR)(timer->timer));
    if (ret == LOS_ERRNO_SWTMR_NOT_STARTED) {
        return OSAL_SUCCESS; // stop success, timer is already stoped, return 0;
    } else if (ret == LOS_OK) {
        return 1; // stop success, timer is pengding, return 1;
    } else {
        osal_log("LOS_SwtmrStop failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
}

int osal_timer_destroy(osal_timer *timer)
{
    if (timer == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    int ret = LOS_SwtmrDelete((unsigned short)(UINTPTR)(timer->timer));
    if (ret != LOS_OK) {
        osal_log("LOS_SwtmrDelete failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

unsigned long long osal_sched_clock(void)
{
    return LOS_CurrNanosec();
}

void osal_gettimeofday(osal_timeval *tv)
{
    if (tv == NULL) {
        return;
    }

    struct timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0) {
        tv->tv_usec = (unsigned int)(tp.tv_nsec / US_TO_NSEC);
        tv->tv_sec = tp.tv_sec;
    } else {
        osal_log("clock_gettime failed!\n");
    }
}

unsigned long long osal_get_jiffies(void)
{
    return LOS_TickCountGet();
}

unsigned long osal_msecs_to_jiffies(const unsigned int m)
{
    return (unsigned long)LOS_MS2Tick(m);
}

unsigned int osal_jiffies_to_msecs(const unsigned int n)
{
    return LOS_Tick2MS(n);
}

unsigned int osal_get_cycle_per_tick(void)
{
    return LOS_CyclePerTickGet();
}
