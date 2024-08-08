/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: mutex
 *
 * Create: 2021-12-16
 */

#include <los_mux.h>
#include <los_mux_pri.h>
#include "soc_osal.h"
#include "osal_errno.h"
#include "osal_inner.h"

int osal_mutex_init(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    if (GET_MUX_INDEX((unsigned int)(UINTPTR)mutex->mutex) >= (unsigned int)KERNEL_MUX_LIMIT) {
        osal_log("mutex id invalid!\n");
        return OSAL_FAILURE;
    }
    LosMuxCB *mutex_handle = GET_MUX((unsigned int)(UINTPTR)mutex->mutex);
    if (mutex_handle->muxStat == LOS_USED && ((unsigned int)(UINTPTR)mutex->mutex != 0)) {
        osal_log("The mutex has been initialized. mutex id:%d.\n", (unsigned int)(UINTPTR)mutex->mutex);
        return OSAL_FAILURE;
    }

    unsigned int ret = LOS_MuxCreate((unsigned int *)&(mutex->mutex));
    if (ret != LOS_OK) {
        osal_log("LOS_MuxCreate failed! ret = %#x address:%#x.\n", ret, (uint32_t)__builtin_return_address(0));
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

void osal_mutex_destroy(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    unsigned int ret = LOS_MuxDelete((unsigned int)(UINTPTR)mutex->mutex);
    if (ret != LOS_OK) {
        osal_log("LOS_MuxDelete failed! ret = %#x address:%#x.\n", ret, (uint32_t)__builtin_return_address(0));
    }
    mutex->mutex = NULL;
}

int osal_mutex_lock(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    unsigned int ret = LOS_MuxPend((unsigned int)(UINTPTR)mutex->mutex, LOS_WAIT_FOREVER);
    if (ret != LOS_OK) {
        osal_log("LOS_MuxPend failed! ret = %#x address:%#x.\n", ret, (uint32_t)__builtin_return_address(0));
        return OSAL_FAILURE;
    }

    return OSAL_SUCCESS;
}

int osal_mutex_lock_timeout(osal_mutex *mutex, unsigned int timeout)
{
    if (mutex == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    unsigned int tick = (timeout == OSAL_MUTEX_WAIT_FOREVER) ? LOS_WAIT_FOREVER : osal_msecs_to_jiffies(timeout);

    unsigned int ret = LOS_MuxPend((unsigned int)(UINTPTR)mutex->mutex, tick);
    if (ret != LOS_OK) {
        osal_log("LOS_MuxPend failed! ret = %#x address:%#x.\n", ret, (uint32_t)__builtin_return_address(0));
        return OSAL_FAILURE;
    }

    return OSAL_SUCCESS;
}

int osal_mutex_lock_interruptible(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    unsigned int ret = LOS_MuxPend((unsigned int)(UINTPTR)mutex->mutex, LOS_WAIT_FOREVER);
    if (ret == LOS_ERRNO_MUX_PEND_INTERR) {
        osal_log("The mutex is being locked during an interrupt!\n");
        return OSAL_EINTR;
    } else if (ret != LOS_OK) {
        osal_log("LOS_MuxPend failed! ret = %#x address:%#x.\n", ret, (uint32_t)__builtin_return_address(0));
        return OSAL_FAILURE;
    }

    return OSAL_SUCCESS;
}

int osal_mutex_trylock(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("parameter invalid!\n");
        return FALSE;
    }

    unsigned int ret = LOS_MuxPend((unsigned int)(UINTPTR)mutex->mutex, LOS_NO_WAIT);
    if (ret != LOS_OK) {
        return FALSE;
    }

    return TRUE;
}

void osal_mutex_unlock(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }

    unsigned int ret = LOS_MuxPost((unsigned int)(UINTPTR)mutex->mutex);
    if (ret != LOS_OK) {
        osal_log("LOS_MuxPost failed! ret = %#x address:%#x..\n", ret, (uint32_t)__builtin_return_address(0));
    }
}

int osal_mutex_is_locked(osal_mutex *mutex)
{
    osal_unused(mutex);
    return -1;
}
