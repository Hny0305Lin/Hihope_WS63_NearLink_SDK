/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: OTA Adaptation Implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include <stdbool.h>
#include "app_call.h"

bool HILINK_OtaAdapterFlashInit(void)
{
    return app_call0(APP_CALL_HILINK_OTA_ADAPTER_FLASH_INIT, bool);
}

unsigned int HILINK_OtaAdapterGetUpdateIndex(void)
{
    return app_call0(APP_CALL_HILINK_OTA_ADAPTER_GET_UPDATE_INDEX, unsigned int);
}

int HILINK_OtaAdapterFlashErase(unsigned int size)
{
    return app_call1(APP_CALL_HILINK_OTA_ADAPTER_FLASH_ERASE, int, unsigned int, size);
}

int HILINK_OtaAdapterFlashWrite(const unsigned char *buf, unsigned int bufLen)
{
    return app_call2(APP_CALL_HILINK_OTA_ADAPTER_FLASH_WRITE, int, const unsigned char *, buf, unsigned int, bufLen);
}

int HILINK_OtaAdapterFlashRead(unsigned int offset, unsigned char *buf, unsigned int bufLen)
{
    return app_call3(APP_CALL_HILINK_OTA_ADAPTER_FLASH_READ, int,
        unsigned int, offset, unsigned char *, buf, unsigned int, bufLen);
}

bool HILINK_OtaAdapterFlashFinish(void)
{
    return app_call0(APP_CALL_HILINK_OTA_ADAPTER_FLASH_FINISH, bool);
}

unsigned int HILINK_OtaAdapterFlashMaxSize(void)
{
    return app_call0(APP_CALL_HILINK_OTA_ADAPTER_FLASH_MAX_SIZE, unsigned int);
}

void HILINK_OtaAdapterRestart(int flag)
{
    app_call1(APP_CALL_HILINK_OTA_ADAPTER_RESTART, void, int, flag);
}

int HILINK_OtaStartProcess(int type)
{
    return app_call1(APP_CALL_HILINK_OTA_START_PROCESS, int, int, type);
}

int HILINK_OtaEndProcess(int status)
{
    return app_call1(APP_CALL_HILINK_OTA_END_PROCESS, int, int, status);
}

int HILINK_GetRebootFlag(void)
{
    return app_call0(APP_CALL_HILINK_GET_REBOOT_FLAG, int);
}
