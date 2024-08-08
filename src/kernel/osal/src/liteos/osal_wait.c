/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: wait
 *
 * Create: 2021-12-16
 */

#include <los_event.h>
#include <los_spinlock.h>
#include <los_memory.h>
#include <linux/wait.h>

#include "soc_osal.h"
#include "osal_errno.h"
#include "osal_inner.h"

int osal_wait_init(osal_wait *wait)
{
    if (wait == NULL || wait->wait != NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    wait_queue_head_t *wq =
        (wait_queue_head_t *)LOS_MemAlloc((void *)m_aucSysMem0, sizeof(wait_queue_head_t));
    if (wq == NULL) {
        osal_log("LOS_MemAlloc failed!\n");
        return OSAL_FAILURE;
    }

    unsigned int ret = LOS_EventInit(&wq->stEvent);
    if (ret != LOS_OK) {
        osal_log("LOS_EventInit failed! ret = %#x.\n", ret);
        LOS_MemFree((void*)m_aucSysMem0, (void *)wq);
        return OSAL_FAILURE;
    }
    LOS_SpinInit(&wq->lock);
    LOS_ListInit(&wq->poll_queue);
    wait->wait = wq;
    return OSAL_SUCCESS;
}

int osal_wait_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param)
{
    // not support interruptible wait in liteos
    return osal_wait_uninterruptible(wait, func, param);
}

int osal_wait_uninterruptible(osal_wait *wait, osal_wait_condition_func func, const void *param)
{
    if (wait == NULL || wait->wait == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    wait_queue_head_t *wq = (wait_queue_head_t *)(wait->wait);

    while (func != NULL && func(param) == 0) {
        (VOID)LOS_EventRead(&wq->stEvent, 0x1U, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
    }

    return OSAL_SUCCESS;
}

/**
 * @par Description: same as <wait_event_interruptible_timeout> from liteos
 * This API is used to sleep a process until the condition evaluates to true or a timeout elapses.
 * The condition is checked each time when the waitqueue wait is woken up.
 * @attention
 * The value range of parameter timeout is [0, 0xFFFFFFFF], and 0xFFFFFFFF means waiting forever.</li>
 * @param  wait [IN] the waitqueue to wait on.
 * @param  condition [IN] a condition evaluates to true or false.
 * @param  timeout [IN] the max sleep time unit is ms.
 *
 * @retval #0 return 0 if the condition evaluated to false after the timeout elapsed
 * @retval #1 return 1 if the condition evaluated to true after the timeout elapsed
 * @retval #others return the remaining ticks if the condition evaluated to true before the timeout elapsed
 */
int osal_wait_timeout_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param, unsigned long ms)
{
    if (wait == NULL || wait->wait == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    wait_queue_head_t *wq = (wait_queue_head_t *)(wait->wait);

    unsigned long long ticksnow = LOS_TickCountGet();
    unsigned int timetick = LOS_MS2Tick(ms);
    if (timetick > INT32_MAX) {
        osal_log("ms parameter invalid!\n");
        return OSAL_FAILURE;
    }
    int timeout = (int)timetick;
    int tmp_timeout = timeout;
    int ret = timeout;
    if (func == NULL) {
        return ret; /* default return timeout if condition func NULL */
    }
    if (ms == 0 && func(param)) {
        return 1;
    }
    while (!func(param)) {
        unsigned int event_ret = LOS_EventRead(&wq->stEvent, 0x1U, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, (tmp_timeout));
        if (ms == OSAL_WAIT_FOREVER) {
            if (!func(param)) {
                continue;
            }
            break;
        }

        tmp_timeout = (int)(timeout - (unsigned int)(LOS_TickCountGet() - ticksnow));
        if (tmp_timeout <= 0 || event_ret == LOS_ERRNO_EVENT_READ_TIMEOUT) {
            ret = (func(param) == FALSE) ? FALSE : TRUE;
            break;
        }
        if (func(param) != 0) {
            ret = tmp_timeout;
            break;
        }
    }

    return ret;
}


int osal_wait_timeout_uninterruptible(osal_wait *wait, osal_wait_condition_func func, const void *param,
    unsigned long ms)
{
    return osal_wait_timeout_interruptible(wait, func, param, ms);
}

void osal_wait_wakeup(osal_wait *wait)
{
    if (wait == NULL || wait->wait == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    wait_queue_head_t *wq = (wait_queue_head_t *)(wait->wait);
    (VOID)LOS_EventWrite(&wq->stEvent, 0x1);
}

void osal_wait_wakeup_interruptible(osal_wait *wait)
{
    osal_wait_wakeup(wait);
}

void osal_wait_destroy(osal_wait *wait)
{
    if (wait == NULL || wait->wait == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    wait_queue_head_t *wq = (wait_queue_head_t *)(wait->wait);

    LOS_EventDestroy(&wq->stEvent);
    LOS_ListDelInit(&wq->poll_queue);
    LOS_MemFree((void *)m_aucSysMem0, (void *)wq);
    wait->wait = NULL;
}
