/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx res
 * This file should be changed only infrequently and with great care.
 */
#include "dfx_res.h"
#include "osal_inner.h"

#define ENABLE_LOS_CUSTOM_DEBUG 0
#if defined(SOCMN1_PRODUCT_EVB) || defined(SOCMN1_PRODUCT_EVB_K)
#if ENABLE_LOS_CUSTOM_DEBUG
#include "los_custom_debug.h"
#endif
#endif

errcode_t dfx_os_get_resource_status(const osal_os_resource_use_stat_t *os_resource_stat)
{
    if (os_resource_stat == NULL) {
        return ERRCODE_FAIL;
    }
#if ENABLE_LOS_CUSTOM_DEBUG
#if defined(SOCMN1_PRODUCT_EVB) || defined(SOCMN1_PRODUCT_EVB_K)
    os_resource_stat->timer_usage = (uint8_t)LOS_SwtmrUsage();
    os_resource_stat->task_usage = (uint8_t)LOS_TaskUsage(NULL);
    os_resource_stat->sem_usage = (uint8_t)LOS_SemUsage();
    os_resource_stat->queue_usage = (uint8_t)LOS_QueueUsage();
    os_resource_stat->mux_usage = (uint8_t)LOS_MuxUsage();
#endif
#endif
    return ERRCODE_SUCC;
}