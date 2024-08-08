/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the ble main, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_bt_api.h"

void HILINK_GetDeviceSn(unsigned int len, char *sn)
{
    app_call2(APP_CALL_HILINK_GET_DEVICE_SN, void, unsigned int, len, char *, sn);
}

int HILINK_GetSubProdId(char *subProdId, int len)
{
    return app_call2(APP_CALL_HILINK_GET_SUB_PROD_ID, int, char *, subProdId, int, len);
}

int HILINK_BT_GetDevSurfacePower(char *power)
{
    return app_call1(APP_CALL_HILINK_BT_GET_DEV_SURFACE_POWER, int, char *, power);
}

HILINK_BT_DevInfo *HILINK_BT_GetDevInfo(void)
{
    return app_call0(APP_CALL_HILINK_BT_GET_DEV_INFO, HILINK_BT_DevInfo *);
}

int HILINK_GetCustomInfo(char *customInfo, unsigned int len)
{
    return app_call2(APP_CALL_HILINK_GET_CUSTOM_INFO, int, char *, customInfo, unsigned int, len);
}

int HILINK_GetManuId(char *manuId, unsigned int len)
{
    return app_call2(APP_CALL_HILINK_GET_MANU_ID, int, char *, manuId, unsigned int, len);
}

int HILINK_BT_GetMacAddr(unsigned char *mac, unsigned int len)
{
    return app_call2(APP_CALL_HILINK_BT_GET_MAC_ADDR, int, unsigned char *, mac, unsigned int, len);
}

int getDeviceVersion(char* *firmwareVer, char* *softwareVer, char* *hardwareVer)
{
    return app_call3(APP_CALL_GET_DEVICE_VERSION, int, char* *, firmwareVer,
        char* *, softwareVer, char* *, hardwareVer);
}