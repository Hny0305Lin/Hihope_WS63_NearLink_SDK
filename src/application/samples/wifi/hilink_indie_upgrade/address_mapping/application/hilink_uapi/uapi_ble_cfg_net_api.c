  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: ble cfg net api \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */
#include "hilink_call.h"
#include "ble_cfg_net_api.h"

int BLE_CfgNetInit(const BLE_InitPara *para, const BLE_CfgNetCb *callback)
{
    return hilink_call2(HILINK_CALL_BLE_CFG_NET_INIT, int,
        const BLE_InitPara *, para, const BLE_CfgNetCb *, callback);
}

int BLE_CfgNetDeInit(const BLE_GattHandleList *handleList, unsigned int flag)
{
    return hilink_call2(HILINK_CALL_BLE_CFG_NET_DE_INIT, int,
        const BLE_GattHandleList *, handleList, unsigned int, flag);
}

int BLE_CfgNetAdvCtrl(unsigned int advSecond)
{
    return hilink_call1(HILINK_CALL_BLE_CFG_NET_ADV_CTRL, int, unsigned int, advSecond);
}

int BLE_CfgNetAdvUpdate(const BLE_AdvInfo *advInfo)
{
    return hilink_call1(HILINK_CALL_BLE_CFG_NET_ADV_UPDATE, int, const BLE_AdvInfo *, advInfo);
}

int BLE_CfgNetDisConnect(void)
{
    return hilink_call0(HILINK_CALL_BLE_CFG_NET_DIS_CONNECT, int);
}

int BLE_SendCustomData(BLE_DataType dataType, const unsigned char *buff, unsigned int len)
{
    return hilink_call3(HILINK_CALL_BLE_SEND_CUSTOM_DATA, int,
        BLE_DataType, dataType, const unsigned char *, buff, unsigned int, len);
}

int BLE_GetAdvType(void)
{
    return hilink_call0(HILINK_CALL_BLE_GET_ADV_TYPE, int);
}

void BLE_SetAdvType(int type)
{
    hilink_call1(HILINK_CALL_BLE_SET_ADV_TYPE, void, int, type);
}

int BLE_SetAdvNameMpp(const unsigned char *mpp, unsigned int len)
{
    return hilink_call2(HILINK_CALL_BLE_SET_ADV_NAME_MPP, int,
        const unsigned char *, mpp, unsigned int, len);
}

int BLE_NearDiscoveryInit(const BLE_NearDiscoveryCb *cb)
{
    return hilink_call1(HILINK_CALL_BLE_NEAR_DISCOVERY_INIT, int, const BLE_NearDiscoveryCb *, cb);
}

int BLE_NearDiscoveryEnable(unsigned long waitTime)
{
    return hilink_call1(HILINK_CALL_BLE_NEAR_DISCOVERY_ENABLE, int, unsigned long, waitTime);
}

int HILINK_BT_GetTaskStackSize(const char *name, unsigned long *stackSize)
{
    return hilink_call2(HILINK_CALL_HILINK_BT_GET_TASK_STACK_SIZE, int,
        const char *, name, unsigned long *, stackSize);
}

int HILINK_BT_SetTaskStackSize(const char *name, unsigned long stackSize)
{
    return hilink_call2(HILINK_CALL_HILINK_BT_SET_TASK_STACK_SIZE, int,
        const char *, name, unsigned long, stackSize);
}