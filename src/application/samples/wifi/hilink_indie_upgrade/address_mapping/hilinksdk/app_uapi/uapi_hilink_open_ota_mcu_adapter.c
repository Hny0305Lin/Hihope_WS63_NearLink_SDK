/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Upgrade and adaptation of the external MCU. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

int HILINK_GetMcuVersion(char *version, unsigned int inLen, unsigned int *outLen)
{
    return app_call3(APP_CALL_HILINK_GET_MCU_VERSION, int,
        char *, version, unsigned int, inLen, unsigned int *, outLen);
}

int HILINK_NotifyOtaStatus(int flag, unsigned int len, unsigned int type)
{
    return app_call3(APP_CALL_HILINK_NOTIFY_OTA_STATUS, int, int, flag, unsigned int, len, unsigned int, type);
}

int HILINK_NotifyOtaData(const unsigned char *data, unsigned int len, unsigned int offset)
{
    return app_call3(APP_CALL_HILINK_NOTIFY_OTA_DATA, int,
        const unsigned char *, data, unsigned int, len, unsigned int, offset);
}