/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the ble adapter, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include <stdbool.h>
#include "app_call.h"
#include "ohos_bt_gatt.h"
#include "ohos_bt_def.h"
#include "ohos_bt_gatt_server.h"

BdAddr* GetLocalAddress(void)
{
    return app_call0(APP_CALL_GET_LOCAL_ADDRESS, BdAddr*);
}

bool GetLocalName(unsigned char *localName, unsigned char *length)
{
    return app_call2(APP_CALL_GET_LOCAL_NAME, bool, unsigned char *, localName, unsigned char *, length);
}

bool SetLocalName(unsigned char *localName, unsigned char length)
{
    return app_call2(APP_CALL_SET_LOCAL_NAME, bool, unsigned char *, localName, unsigned char, length);
}

bool BluetoothFactoryReset(void)
{
    return app_call0(APP_CALL_BLUETOOTH_FACTORY_RESET, bool);
}

int GetBtScanMode(void)
{
    return app_call0(APP_CALL_GET_BT_SCAN_MODE, int);
}

bool SetBtScanMode(int mode, int duration)
{
    return app_call2(APP_CALL_SET_BT_SCAN_MODE, bool, int, mode, int, duration);
}

int ReadBtMacAddr(unsigned char *mac, unsigned int len)
{
    return app_call2(APP_CALL_READ_BT_MAC_ADDR, int, unsigned char *, mac, unsigned int, len);
}

bool GetPariedDevicesNum(unsigned int *number)
{
    return app_call1(APP_CALL_GET_PARIED_DEVICES_NUM, bool, unsigned int *, number);
}

int GetPairState(void)
{
    return app_call0(APP_CALL_GET_PAIR_STATE, int);
}

bool RemovePair(const BdAddr addr)
{
    return app_call1(APP_CALL_REMOVE_PAIR, bool, const BdAddr, addr);
}

bool RemoveAllPairs(void)
{
    return app_call0(APP_CALL_REMOVE_ALL_PAIRS, bool);
}

bool ReadRemoteRssiValue(const BdAddr *bdAddr, int transport)
{
    return app_call2(APP_CALL_READ_REMOTE_RSSI_VALUE, bool, const BdAddr *, bdAddr, int, transport);
}

bool IsAclConnected(BdAddr *addr)
{
    return app_call1(APP_CALL_IS_ACL_CONNECTED, bool, BdAddr *, addr);
}

bool DisconnectRemoteDevice(BdAddr *addr)
{
    return app_call1(APP_CALL_DISCONNECT_REMOTE_DEVICE, bool, BdAddr *, addr);
}

bool ConnectRemoteDevice(BdAddr *addr)
{
    return app_call1(APP_CALL_CONNECT_REMOTE_DEVICE, bool, BdAddr *, addr);
}

int InitBtStack(void)
{
    return app_call0(APP_CALL_INIT_BT_STACK, int);
}

int EnableBtStack(void)
{
    return app_call0(APP_CALL_ENABLE_BT_STACK, int);
}

int DisableBtStack(void)
{
    return app_call0(APP_CALL_DISABLE_BT_STACK, int);
}

int SetDeviceName(const char *name, unsigned int len)
{
    return app_call2(APP_CALL_SET_DEVICE_NAME, int, const char *, name, unsigned int, len);
}

int BleSetAdvData(int advId, const BleConfigAdvData *data)
{
    return app_call2(APP_CALL_BLE_SET_ADV_DATA, int, int, advId, const BleConfigAdvData *, data);
}

int BleStartAdv(int advId, const BleAdvParams *param)
{
    return app_call2(APP_CALL_BLE_START_ADV, int, int, advId, const BleAdvParams *, param);
}

int BleStopAdv(int advId)
{
    return app_call1(APP_CALL_BLE_STOP_ADV, int, int, advId);
}

int BleUpdateAdv(int advId, const BleAdvParams *param)
{
    return app_call2(APP_CALL_BLE_UPDATE_ADV, int, int, advId, const BleAdvParams *, param);
}

int BleSetSecurityIoCap(BleIoCapMode mode)
{
    return app_call1(APP_CALL_BLE_SET_SECURITY_IO_CAP, int, BleIoCapMode, mode);
}

int BleSetSecurityAuthReq(BleAuthReqMode mode)
{
    return app_call1(APP_CALL_BLE_SET_SECURITY_AUTH_REQ, int, BleAuthReqMode, mode);
}

int BleGattSecurityRsp(BdAddr bdAddr, bool accept)
{
    return app_call2(APP_CALL_BLE_GATT_SECURITY_RSP, int, BdAddr, bdAddr, bool, accept);
}

int BleScanFilterParamSetup(BleAdvScanFilterParam *param)
{
    return app_call1(APP_CALL_BLE_SCAN_FILTER_PARAM_SETUP, int, BleAdvScanFilterParam *, param);
}

int BleScanFilterAddRemove(BleAdvScanFilterCondition *param)
{
    return app_call1(APP_CALL_BLE_SCAN_FILTER_ADD_REMOVE, int, BleAdvScanFilterCondition *, param);
}

int BleScanFilterClear(int clientId, int filterIndex)
{
    return app_call2(APP_CALL_BLE_SCAN_FILTER_CLEAR, int, int, clientId, int, filterIndex);
}

int BleScanFilterEnable(int clientId, bool enable)
{
    return app_call2(APP_CALL_BLE_SCAN_FILTER_ENABLE, int, int, clientId, bool, enable);
}

int BleSetScanParameters(int clientId, BleScanParams *param)
{
    return app_call2(APP_CALL_BLE_SET_SCAN_PARAMETERS, int, int, clientId, BleScanParams *, param);
}

int BleStartScan(void)
{
    return app_call0(APP_CALL_BLE_START_SCAN, int);
}

int BleStopScan(void)
{
    return app_call0(APP_CALL_BLE_STOP_SCAN, int);
}

int BleGattRegisterCallbacks(BtGattCallbacks *func)
{
    return app_call1(APP_CALL_BLE_GATT_REGISTER_CALLBACKS, int, BtGattCallbacks *, func);
}

int BleStartAdvEx(int *advId, const StartAdvRawData rawData, BleAdvParams advParam)
{
    return app_call3(APP_CALL_BLE_START_ADV_EX, int, int *, advId,
        const StartAdvRawData, rawData, BleAdvParams, advParam);
}

int BleGattsRegister(BtUuid appUuid)
{
    return app_call1(APP_CALL_BLE_GATTS_REGISTER, int, BtUuid, appUuid);
}

int BleGattsUnRegister(int serverId)
{
    return app_call1(APP_CALL_BLE_GATTS_UN_REGISTER, int, int, serverId);
}

int BleGattsDisconnect(int serverId, BdAddr bdAddr, int connId)
{
    return app_call3(APP_CALL_BLE_GATTS_DISCONNECT, int, int, serverId, BdAddr, bdAddr, int, connId);
}

int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number)
{
    return app_call4(APP_CALL_BLE_GATTS_ADD_SERVICE, int, int, serverId, BtUuid, srvcUuid,
        bool, isPrimary, int, number);
}

int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid,
                              int properties, int permissions)
{
    return app_call5(APP_CALL_BLE_GATTS_ADD_CHARACTERISTIC, int, int, serverId, int, srvcHandle,
        BtUuid, characUuid, int, properties, int, permissions);
}

int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions)
{
    return app_call4(APP_CALL_BLE_GATTS_ADD_DESCRIPTOR, int, int, serverId, int, srvcHandle,
        BtUuid, descUuid, int, permissions);
}

int BleGattsStartService(int serverId, int srvcHandle)
{
    return app_call2(APP_CALL_BLE_GATTS_START_SERVICE, int, int, serverId, int, srvcHandle);
}

int BleGattsStopService(int serverId, int srvcHandle)
{
    return app_call2(APP_CALL_BLE_GATTS_STOP_SERVICE, int, int, serverId, int, srvcHandle);
}

int BleGattsDeleteService(int serverId, int srvcHandle)
{
    return app_call2(APP_CALL_BLE_GATTS_DELETE_SERVICE, int, int, serverId, int, srvcHandle);
}

int BleGattsClearServices(int serverId)
{
    return app_call1(APP_CALL_BLE_GATTS_CLEAR_SERVICES, int, int, serverId);
}

int BleGattsSendResponse(int serverId, GattsSendRspParam *param)
{
    return app_call2(APP_CALL_BLE_GATTS_SEND_RESPONSE, int, int, serverId, GattsSendRspParam *, param);
}

int BleGattsSendIndication(int serverId, GattsSendIndParam *param)
{
    return app_call2(APP_CALL_BLE_GATTS_SEND_INDICATION, int, int, serverId, GattsSendIndParam *, param);
}

int BleGattsSetEncryption(BdAddr bdAddr, BleSecAct secAct)
{
    return app_call2(APP_CALL_BLE_GATTS_SET_ENCRYPTION, int, BdAddr, bdAddr, BleSecAct, secAct);
}

int BleGattsRegisterCallbacks(BtGattServerCallbacks *func)
{
    return app_call1(APP_CALL_BLE_GATTS_REGISTER_CALLBACKS, int, BtGattServerCallbacks *, func);
}

int BleGattsStartServiceEx(int *srvcHandle, BleGattService *srvcInfo)
{
    return app_call2(APP_CALL_BLE_GATTS_START_SERVICE_EX, int, int *, srvcHandle,
        BleGattService *, srvcInfo);
}

int BleGattsStopServiceEx(int srvcHandle)
{
    return app_call1(APP_CALL_BLE_GATTS_STOP_SERVICE_EX, int, int, srvcHandle);
}
