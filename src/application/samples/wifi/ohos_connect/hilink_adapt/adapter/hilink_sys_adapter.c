/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 系统适配层接口实现（此文件为DEMO，需集成方适配修改）
 */

#include "hilink_sys_adapter.h"
#include "hilink_network_adapter.h"

int HILINK_Restart(void)
{
    HILINK_ReconnectWiFi();
    return 0;
}

unsigned char HILINK_GetSystemBootReason(void)
{
    return HILINK_NORMAL_BOOT;
}