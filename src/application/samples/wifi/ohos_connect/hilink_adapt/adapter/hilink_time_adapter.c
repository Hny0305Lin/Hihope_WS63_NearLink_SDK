/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 时间适配层接口cmsis2实现源文件（此文件为DEMO，需集成方适配修改）
 */

#include "hilink_time_adapter.h"
#include "cmsis_os2.h"
#include "hilink_sal_defines.h"

#ifndef MS_PER_SECOND
#define MS_PER_SECOND   1000
#endif
#ifndef US_PER_MS
#define US_PER_MS       1000
#endif

int HILINK_GetOsTime(HiLinkTimeval *time)
{
    if (time == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    if (osKernelGetTickFreq() == 0) {
        HILINK_SAL_CRIT("invalid tick freq\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    uint64_t ms = ((uint64_t)osKernelGetTickCount() * MS_PER_SECOND / osKernelGetTickFreq());
    time->sec = ms / MS_PER_SECOND;
    time->usec = ms % MS_PER_SECOND * US_PER_MS;

    return HILINK_SAL_OK;
}

int HILINK_GetUtcTime(HiLinkTimeval *time)
{
    (void)time;
    return HILINK_SAL_NOT_SUPPORT;
}