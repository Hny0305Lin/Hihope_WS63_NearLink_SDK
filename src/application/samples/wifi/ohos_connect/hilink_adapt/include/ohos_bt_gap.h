/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: ohos 蓝牙gap相关宏与接口定义
 */
#ifndef OHOS_BT_GAP_H
#define OHOS_BT_GAP_H

#include "ohos_bt_def.h"

#define SERVICE_LIMITED_DISCOVERABILITY 0x002000
#define SERVICE_POSITIONING 0x010000
#define SERVICE_NETWORKING 0x020000
#define SERVICE_RENDER 0x040000
#define SERVICE_CAPTURE 0x080000
#define SERVICE_OBJECT_TRANSFER 0x100000
#define SERVICE_AUDIO 0x200000
#define SERVICE_TELEPHONY 0x400000
#define SERVICE_INFORMATION 0x800000

#define OHOS_BD_NAME_LEN 32
typedef struct {
    BdAddr addr;
    unsigned char deviceName[OHOS_BD_NAME_LEN]; /* Name of the device record, must be in UTF-8 */
} PairedDeviceInfo;

/* Bluetooth stack state */
typedef enum {
    OHOS_GAP_STATE_TURNING_ON = 0x0,
    OHOS_GAP_STATE_TURN_ON,
    OHOS_GAP_STATE_TURNING_OFF,
    OHOS_GAP_STATE_TURN_OFF
} BtStackState;

/* Bluetooth ACL connection state */
typedef enum {
    OHOS_GAP_ACL_STATE_CONNECTED,
    OHOS_GAP_ACL_STATE_DISCONNECTED,
    OHOS_GAP_ACL_STATE_LE_CONNECTED,
    OHOS_GAP_ACL_STATE_LE_DISCONNECTED
} GapAclState;

/* bondable mode */
typedef enum {
    OHOS_GAP_BONDABLE_MODE_OFF = 0x00,
    OHOS_GAP_BONDABLE_MODE_ON = 0x01
} GapBtBondableMode;

typedef enum {
    OHOS_GAP_ACCESS_UNKNOWN,
    OHOS_GAP_ACCESS_ALLOWED,
    OHOS_GAP_ACCESS_FORBIDDEN,
} GapBtPermissionType;

typedef enum {
    OHOS_GAP_SCAN_MODE_NONE = 0x00,
    OHOS_GAP_SCAN_MODE_CONNECTABLE,
    OHOS_GAP_SCAN_MODE_GENERAL_DISCOVERABLE,
    OHOS_GAP_SCAN_MODE_LIMITED_DISCOVERABLE,
    OHOS_GAP_SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE,
    OHOS_GAP_SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE
} GapBtScanMode;

typedef enum {
    OHOS_PROFILE_HEADSET = 0x0,
    OHOS_PROFILE_A2DP,
    OHOS_PROFILE_OPP,
    OHOS_PROFILE_HID,
    OHOS_PROFILE_PANU,
    OHOS_PROFILE_NAP,
    OHOS_PROFILE_HFP
} GapBtProfileType;

typedef enum {
    OHOS_GAP_PAIR_NONE = 0x01,
    OHOS_GAP_PAIR_PAIRING = 0x02,
    OHOS_GAP_PAIR_PAIRED = 0x03
} GapBtPairedState;

bool EnableBle(void);

bool DisableBle(void);

bool EnableBt(void);

bool DisableBt(void);

/**
 *  @brief Get local host bluetooth address
 *  @return @c Local host bluetooth address
 */
BdAddr* GetLocalAddress(void);

/**
 *  @brief Get local host bluetooth name
 *  @param localName actual receiving device name
           length - localName length, initail set length to zero, and call this func to set real length
 *  @return Local host bluetooth name
 */
bool GetLocalName(unsigned char *localName, unsigned char *length);

/**
 * @brief Set local device name.
 * @param localName Device name.
          length device name length
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool SetLocalName(unsigned char *localName, unsigned char length);

/**
 * @brief Factory reset bluetooth service.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool BluetoothFactoryReset(void);

/**
 * @brief Set device scan mode.
 * @param mode Scan mode.
 * @param duration Scan time, see details {@link GapBtScanMode}
 * @return special mode
 */
int GetBtScanMode(void);

/**
 * @brief Set device scan mode.
 * @param mode Scan mode, see details {@link GapBtScanMode}.
 * @param duration Scan time.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool SetBtScanMode(int mode, int duration);

/*
 * @brief Get paired devices.
 * @param pairList - 按照maxPairNums申请的设备列表数组
          maxPairNums - 指定需要获取的设备列表最大个数
          realPairNums - 实际的配对设备列表个数
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool GetPariedDevicesNum(unsigned int *number);

bool GetPairedDevicesList(PairedDeviceInfo *devInfo, int *number);

bool GetRemoteDevName(PairedDeviceInfo *devInfo);

/**
 * @brief Get device pair state.
 * @param device Remote device.
 * @return Returns device pair state. see detail {@link GapBtPairedState}
 */
int GetPairState(void);

/**
 * @brief Remove pair.
 * @param BdAddr Remote device address.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool RemovePair(const BdAddr addr);

/**
 * @brief Remove all pairs.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool RemoveAllPairs(void);

/**
 * @brief Read remote device rssi value.
 *
 * @param bdAddr device address.
 * @param transport Transport type, details see {@link BtTransportId}
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool ReadRemoteRssiValue(const BdAddr *bdAddr, int transport);

/**
 * @brief Check if device acl connected.
 * @param addr device address.
 * @return Returns <b>true</b> if device acl connected;
 *         returns <b>false</b> if device does not acl connect.
 */
bool IsAclConnected(BdAddr *addr);

/**
 * @brief disconnect remote device all profile.
 * @param addr device address.
 * @return Returns <b>true</b> if device acl connected;
 *         returns <b>false</b> if device does not acl connect.
 */
bool DisconnectRemoteDevice(BdAddr *addr);

/*
 * @brief connect remote device acl profile.
 * @param: remote device address
 * @return Returns <b>true</b> if device acl connected;
 *         returns <b>false</b> if device does not acl connect.
 */
bool ConnectRemoteDevice(BdAddr *addr);

/**
 * @brief Adapter state change function. 开关蓝牙的callback上报
 * @param transport Transport type when state change.
 *        OHOS_BT_TRANSPORT_BR_EDR : classic;
 *        OHOS_BT_TRANSPORT_LE : ble.
 *        detail see {@link BtTransportId}
 * @param state Change to the new state
 *        details see {@link BtStackState}.
 */
typedef void (*GapStateChangedCallback)(const int transport, const int status);

/**
 * @brief ACL state change callback
 * @param BtGapAclState - details of acl state changed
 * @param reason - clarify detals of REASON CODE from stack - If possible, suggest using the standard HCI error code.
 * @return void
 */
typedef void (*GapAclStateChangedCallback)(const BdAddr *bdAddr, GapAclState state, unsigned int reason);

/**
 * @brief Discovery state changed observer.
 * @param status DISCOVERY_STARTED/DISCOVERYING/DISCOVERY_STOPED
 * @return void
 */
typedef void (*GapDiscoveryStateChangedCallback)(int status);

/**
 * @brief Discovery state changed observer.
 * @param status Device discovery status.
 * @return void
 */
typedef void (*GapDiscoveryResultCallback)(const BdAddr *bdAddr);

/**
 * @brief Pair request observer.
 * @param bdAddr Remote device address.
 * @return void
 */
typedef void (*GapPairRequestedCallback)(const BdAddr *bdAddr);

/**
 * @brief Pair confirmed observer.
 * @param BdAddr Remote device address.
 * @param reqType Pair type.
 * @param number Paired passkey.
 * @return void
 */
typedef void (*GapPairConfirmedCallback)(const BdAddr *bdAddr, int reqType, int number);

/**
 * @brief Scan mode changed observer.
 * @param mode Device scan mode.
 * @return void
 */
typedef void (*GapScanModeChangedCallback)(int mode);

/**
 * @brief Device name changed observer.
 * @param deviceName Device name.
 * @return void
 */
typedef void (*GapLocalDeviceNameChangedCallback)(const unsigned char *deviceName, unsigned char length);

/**
 * @brief Device address changed observer.
 * @param bdAddr Device address.
 * @return void
 */
typedef void (*GapLocalDeviceAddrChangedCallback)(const BdAddr *bdAddr);

/**
 * @brief Pair status changed observer.
 * @param bdAddr Device address.
 * @param status Remote device pair status.
 * @return void
 */
typedef void (*GapPairStatusChangedCallback)(const BdAddr *bdAddr, int status);

/**
 * @brief Remote uuid changed observer.
 * @param bdAddr Device address.
 * @param uuid Remote device uuids.
 * @return void
 */
typedef void (*GapRemoteUuidChangedCallback)(const BdAddr *bdAddr, BtUuid uuid);

/**
 * @brief Remote name changed observer.
 * @param bdAddr Device address.
 * @param deviceName Remote device name.
 * @return void
 */
typedef void (*GapRemoteNameChangedCallback)(const BdAddr *bdAddr,
    const unsigned char *deviceName, unsigned char length);

/**
 * @brief Remote alias changed observer.
 * @param bdAddr Device address.
 * @param alias Remote device alias.
 * @return void
 */
typedef void (*GapRemoteAliasChangedCallback)(const BdAddr *bdAddr, const unsigned char *alias, unsigned char length);

/**
 * @brief Remote cod changed observer.
 * @param bdAddr Device address.
 * @param cod Remote device cod.
 * @return void
 */
typedef void (*GapRemoteCodChangedCallback)(const BdAddr *bdAddr, int cod);

/**
 * @brief Remote battery level changed observer.
 * @param bdAddr Device address.
 * @param cod Remote device battery Level.
 * @return void
 */
typedef void (*GapRemoteBatteryLevelChangedCallback)(const BdAddr *bdAddr, int batteryLevel);

/**
 * @brief Remote rssi event observer.
 * @param bdAddr Device address.
 * @param rssi Remote device rssi.
 * @param status Read status.
 * @return void
 */
typedef void (*GapReadRemoteRssiEventCallback)(const BdAddr *bdAddr, int rssi, int status);

/**
 * @brief Query application whether to accept the connection.
 * @param bdAddr Device address.
 * @param res Application decision, 0 is reject, 1 is accept.
 * @return void
 */
typedef void (*GapIsAcceptConnOnSafeModeCallback)(const BdAddr *bdAddr, bool *res);

/**
 * @brief GAP回调函数接口定义
 */
typedef struct {
    GapStateChangedCallback stateChangeCallback;
    GapAclStateChangedCallback aclStateChangedCallbak;
    GapScanModeChangedCallback scanModeChangedCallback;
    GapPairStatusChangedCallback pairStatusChangedCallback;
    GapLocalDeviceNameChangedCallback deviceNameChangedCallback;
    GapLocalDeviceAddrChangedCallback deviceAddrChangedCallback;
    GapPairRequestedCallback pairRequestedCallback;
    GapPairConfirmedCallback pairConfiremedCallback;
    GapDiscoveryStateChangedCallback discoveryStateChangedCallback;
    GapDiscoveryResultCallback discoveryResultCallback;
    GapRemoteUuidChangedCallback remoteUuidChangedCallback;
    GapRemoteNameChangedCallback remoteNameChangedCallback;
    GapRemoteAliasChangedCallback remoteAliasChangedCallback;
    GapRemoteCodChangedCallback remoteCodChangedCallback;
    GapRemoteBatteryLevelChangedCallback remoteBatteryLevelChangedCallback;
    GapReadRemoteRssiEventCallback readRemoteRssiEventCallback;
    GapIsAcceptConnOnSafeModeCallback isAcceptConnOnSafeModeCallback;
} BtGapCallBacks;

/**
 * @brief 注册GAP回调函数。
 * @param func 回调函数原型，参考{@link BtGapCallBacks}。
 * @return 成功返回{@link OHOS_BT_STATUS_SUCCESS}，失败返回错误码，具体错误码含义参考{@link BtStatus}定义
 */
int GapRegisterCallbacks(BtGapCallBacks *func);

#endif