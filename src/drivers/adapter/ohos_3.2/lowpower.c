/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides lowpower driver source \n
 *
 * History: \n
 * 2023-10-18， Create file. \n
 */
#include "common_def.h"
#include "iot_errno.h"
#include "lowpower.h"

unsigned int LpcInit(void)
{
    // uapi_pm_init uapi_clocks_init uapi_pm_vote_init已经在系统启动时适配
    return IOT_SUCCESS;
}

unsigned int LpcSetType(LpcType type)
{
    unused(type);
    // 低功耗睡眠模式是各业务投票决定，不能简单的设置睡眠状态
    // 睡眠投票接口：uapi_pm_add_sleep_veto
    // 睡眠去投票接口：uapi_pm_remove_sleep_veto
    return IOT_SUCCESS;
}