/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the Time Adaptation Layer Interface. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_time_adapter.h"

int HILINK_GetOsTime(HiLinkTimeval *time)
{
    return app_call1(APP_CALL_HILINK_GET_OS_TIME, int, HiLinkTimeval *, time);
}

int HILINK_GetUtcTime(HiLinkTimeval *time)
{
    return app_call1(APP_CALL_HILINK_GET_UTC_TIME, int, HiLinkTimeval *, time);
}