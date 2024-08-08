/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: System adaptation layer interface implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sys_adapter.h"

int HILINK_Restart(void)
{
    return app_call0(APP_CALL_HILINK_RESTART, int);
}

unsigned char HILINK_GetSystemBootReason(void)
{
    return app_call0(APP_CALL_HILINK_GET_SYSTEM_BOOT_REASON, unsigned char);
}