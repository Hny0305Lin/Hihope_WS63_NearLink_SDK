/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink BLE file实现源文件（此文件为DEMO，需集成方适配修改）
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "osal_list.h"
#include "securec.h"
#include "test_suite_uart.h"
#include "bts_le_gap.h"
#include "bts_gatt_client.h"
#include "bts_gatt_server.h"
#include "cmsis_os2.h"

#include "ohos_bt_gatt.h"
#include "ohos_bt_def.h"
#include "ohos_bt_gatt_server.h"

static uint8_t g_chara_val[] = {0x11, 0x22, 0x33, 0x44};
static uint8_t g_desc_val[]  = {0x55, 0x66, 0x77, 0x88};
#define UUID16_LEN 2
#define UUID32_LEN 4
#define UUID128_LEN 16
#define BLE_MAX_SERVICES_NUMS 16
#define BLE_HILINK_SERVER_LOG "[BLE_HILINK_SERVER]"
#define BLE_ADV_HANDLE_DEFAULT 1
#define INVALID_SERVER_ID 0
#define EXT_ADV_OR_SCAN_RSP_DATA_LEN 251
#define MAX_READ_REQ_LEN 200

static BtGattCallbacks *g_ble_gap_cb = NULL;
static BtGattServerCallbacks *g_ble_gatts_cb = NULL;

static uint16_t g_services_handle[BLE_MAX_SERVICES_NUMS] = {0};
static uint16_t g_server_request_id = 0;
static uint16_t g_srvc_handle = 0;
static uint16_t g_cb_chara_handle = 0;
static uint16_t g_cb_desc_handle = 0;
static uint16_t g_indicate_handle = 17;
static uint8_t g_adv_status = 0;
static uint8_t g_io_cap_mode = 0;
static uint8_t g_sc_mode = 0;
static uint8_t g_gatt_write_flag = 0;   /* 0:write 1:read */
static uint8_t g_service_flag = 0;      /* 0:enable 1:disable start service */
static uint8_t g_server_id = INVALID_SERVER_ID;  /* gatt server ID */


typedef struct {
    int conn_id;
    int attr_handle;  /* The handle of the attribute to be read */
    BleGattServiceRead read;
    BleGattServiceWrite write;
    BleGattServiceIndicate indicate;
} hilink_ble_gatt_func;

hilink_ble_gatt_func g_charas_func[10] = {{0}};     /* 设置最大Service数量10 */
static uint8_t g_chara_cnt = 0;

static void reverse_uuid(uint8_t *input, char *output, int len)
{
    for (int i = 0; i < len; i++) {
        output[i] = input[len - i - 1];
    }
}

BleGattServiceRead get_chara_read_func(int conn_id, int attr_handle)
{
    for (int i = 0; i < g_chara_cnt; i++) {
        if ((g_charas_func[i].attr_handle == attr_handle)) {
            return g_charas_func[i].read;
        }
    }
    printf("get_chara_read_func Not Found! \n");
    return NULL;
}

BleGattServiceWrite get_chara_write_func(int conn_id, int attr_handle)
{
    for (int i = 0; i < g_chara_cnt; i++) {
        if (g_charas_func[i].attr_handle == attr_handle) {
            return g_charas_func[i].write;
        }
    }
    printf("get_chara_write_func Not Found! \n");
    return NULL;
}

BleGattServiceIndicate get_chara_ind_func(int conn_id, int attr_handle)
{
    for (int i = 0; i < g_chara_cnt; i++) {
        if (g_charas_func[i].attr_handle == attr_handle) {
            return g_charas_func[i].write;
        }
    }
    printf("get_chara_ind_func Not Found! \n");
    return NULL;
}

static uint32_t perm_bt_to_bluez(uint32_t permissions)
{
    uint32_t perm = 0;
    if (permissions & OHOS_GATT_PERMISSION_READ) {
        perm |= GATT_ATTRIBUTE_PERMISSION_READ;
    }
    if (permissions & OHOS_GATT_PERMISSION_READ_ENCRYPTED) {
        perm |= (GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_ENCRYPTION_NEED);
    }
    if (permissions & OHOS_GATT_PERMISSION_READ_ENCRYPTED_MITM) {
        perm |= (GATT_ATTRIBUTE_PERMISSION_READ |
            GATT_ATTRIBUTE_PERMISSION_ENCRYPTION_NEED | GATT_ATTRIBUTE_PERMISSION_MITM_NEED);
    }
    if (permissions & OHOS_GATT_PERMISSION_WRITE) {
        perm |= GATT_ATTRIBUTE_PERMISSION_WRITE;
    }
    if (permissions & OHOS_GATT_PERMISSION_WRITE_ENCRYPTED) {
        perm |= (GATT_ATTRIBUTE_PERMISSION_WRITE | GATT_ATTRIBUTE_PERMISSION_ENCRYPTION_NEED);
    }
    if (permissions & OHOS_GATT_PERMISSION_WRITE_ENCRYPTED_MITM) {
        perm |= (GATT_ATTRIBUTE_PERMISSION_WRITE |
            GATT_ATTRIBUTE_PERMISSION_ENCRYPTION_NEED | GATT_ATTRIBUTE_PERMISSION_MITM_NEED);
    }
    printf("convert %04x to %04x.\n", permissions, perm);
    return perm;
}

bool EnableBle(void)
{
    return 0;
}

bool DisableBle(void)
{
    return 0;
}

bool EnableBt(void)
{
    return 0;
}

bool DisableBt(void)
{
    return 0;
}

/**
 *  @brief Get local host bluetooth address
 *  @return @c Local host bluetooth address
 */
BdAddr* GetLocalAddress(void)
{
    printf("%s GetLocalAddress enter.\n", BLE_HILINK_SERVER_LOG);
    return NULL;
}

/**
 *  @brief Get local host bluetooth name
 *  @param localName actual receiving device name
           length - localName length, initail set length to zero, and call this func to set real length
 *  @return Local host bluetooth name
 */
bool GetLocalName(unsigned char *localName, unsigned char *length)
{
    printf("%s GetLocalName enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/**
 * @brief Set local device name.
 * @param localName Device name.
          length device name length
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool SetLocalName(unsigned char *localName, unsigned char length)
{
    printf("%s SetLocalName enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/**
 * @brief Factory reset bluetooth service.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool BluetoothFactoryReset(void)
{
    return 0;
}

/**
 * @brief Set device scan mode.
 * @param mode Scan mode.
 * @param duration Scan time, see details {@link GapBtScanMode}
 * @return special mode
 */
int GetBtScanMode(void)
{
    return 0;
}

/**
 * @brief Set device scan mode.
 * @param mode Scan mode, see details {@link GapBtScanMode}.
 * @param duration Scan time.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool SetBtScanMode(int mode, int duration)
{
    return 0;
}

/*
 * @brief read bt mac address
 * @param[in] <mac> mac addr
 * @param[in] <len> addr length
 * @return 0-success, other-fail
 */
int ReadBtMacAddr(unsigned char *mac, unsigned int len)
{
    printf("%s ReadBtMacAddr enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Get paired devices.
 * @param pairList - 按照maxPairNums申请的设备列表数组
          maxPairNums - 指定需要获取的设备列表最大个数
          realPairNums - 实际的配对设备列表个数
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool GetPariedDevicesNum(unsigned int *number)
{
    printf("%s GetPariedDevicesNum enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/**
 * @brief Get device pair state.
 * @param device Remote device.
 * @return Returns device pair state. see detail {@link GapBtPairedState}
 */
int GetPairState(void)
{
    printf("%s GetPairState enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/**
 * @brief Remove pair.
 * @param BdAddr Remote device address.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool RemovePair(const BdAddr addr)
{
    printf("%s RemovePair enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/**
 * @brief Remove all pairs.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool RemoveAllPairs(void)
{
    printf("%s RemoveAllPairs enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/**
 * @brief Read remote device rssi value.
 *
 * @param bdAddr device address.
 * @param transport Transport type, details see {@link BtTransportId}
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool ReadRemoteRssiValue(const BdAddr *bdAddr, int transport)
{
    printf("%s ReadRemoteRssiValue enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/**
 * @brief Check if device acl connected.
 * @param addr device address.
 * @return Returns <b>true</b> if device acl connected;
 *         returns <b>false</b> if device does not acl connect.
 */
bool IsAclConnected(BdAddr *addr)
{
    printf("%s IsAclConnected enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/**
 * @brief disconnect remote device all profile.
 * @param addr device address.
 * @return Returns <b>true</b> if device acl connected;
 *         returns <b>false</b> if device does not acl connect.
 */
bool DisconnectRemoteDevice(BdAddr *addr)
{
    printf("%s DisconnectRemoteDevice enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief connect remote device acl profile.
 * @param: remote device address
 * @return Returns <b>true</b> if device acl connected;
 *         returns <b>false</b> if device does not acl connect.
 */
bool ConnectRemoteDevice(BdAddr *addr)
{
    printf("%s ConnectRemoteDevice enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Initialize the Bluetooth protocol stack
 * @param[in] void
 * @return 0-success, other-fail
 */
int InitBtStack(void)
{
    printf("%s InitBtStack enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Bluetooth protocol stack enable
 * @param[in] void
 * @return 0-success, other-fail
 */
int EnableBtStack(void)
{
    printf("%s EnableBtStack enter.\n", BLE_HILINK_SERVER_LOG);
    errcode_t ret = ERRCODE_BT_SUCCESS;
    ret |= enable_ble();
    if (ret != ERRCODE_BT_SUCCESS) {
        printf("%s EnableBtStack fail, ret:%d.\n", BLE_HILINK_SERVER_LOG, ret);
        return -1;
    }
    return 0;
}

/*
 * @brief Bluetooth protocol stack disable
 * @param[in] void
 * @return 0-success, other-fail
 */
int DisableBtStack(void)
{
    printf("%s DisableBtStack enter.\n", BLE_HILINK_SERVER_LOG);
    errcode_t ret = ERRCODE_BT_SUCCESS;
    ret |= disable_ble();
    if (ret != ERRCODE_BT_SUCCESS) {
        printf("%s DisableBtStack fail, ret:%d.\n", BLE_HILINK_SERVER_LOG, ret);
        return -1;
    }
    return 0;
}

/*
 * @brief set this device's name for friendly
 * @param[in] <name> device name
 * @param[in] <len> length
 * @return 0-success, other-fail
 */
int SetDeviceName(const char *name, unsigned int len)
{
    printf("%s SetDeviceName enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief set advertising data
 * @param[in] <advId> specified by upper layer
 * @param[in] <data> adv data or scan response
 * @return 0-success, other-fail
 */
int BleSetAdvData(int advId, const BleConfigAdvData *data)
{
    printf("%s BleSetAdvData enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief start ble advertising
 * @param[in] <advId> specified by upper layer
 * @param[in] <param> ble advertising param list
 * @return 0-success, other-fail
 */
int BleStartAdv(int advId, const BleAdvParams *param)
{
    printf("%s BleStartAdv enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief stop ble advertising
 * @param[in] <advId> specified by upper layer
 * @return 0-success, other-fail
 */
int BleStopAdv(int advId)
{
    printf("%s BleStopAdv enter.\n", BLE_HILINK_SERVER_LOG);

    errcode_t ret = gap_ble_stop_adv(BLE_ADV_HANDLE_DEFAULT);
    if (ret != ERRCODE_BT_SUCCESS) {
        printf("%s BleStopAdv gap_ble_stop_adv error.\n", BLE_HILINK_SERVER_LOG);
        return ERRCODE_BT_FAIL;
    }

    return 0;
}

/*
 * @Update the parameters as per spec, user manual specified values and restart multi ADV
 * @param[in] <advId> specified by upper layer
 * @param[in] <param> ble advertising param list
 * @return 0-success, other-fail
 */
int BleUpdateAdv(int advId, const BleAdvParams *param)
{
    printf("%s BleUpdateAdv enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief set security IO capability
 * @param[in] <mode> BleIoCapMode
 * @return 0-success, other-fail
 */
int BleSetSecurityIoCap(BleIoCapMode mode)
{
    printf("%s BleSetSecurityIoCap enter, io_mod:%d.\n", BLE_HILINK_SERVER_LOG, mode);
    g_io_cap_mode = mode;
    return 0;
}

/*
 * @brief set security authority
 * @param[in] <mode> BleAuthReqMode
 * @return 0-success, other-fail
 */
int BleSetSecurityAuthReq(BleAuthReqMode mode)
{
    printf("%s BleSetSecurityAuthReq enter sc_mode:%d.\n", BLE_HILINK_SERVER_LOG, mode);
    g_sc_mode = mode;
    return 0;
}

/*
 * @brief The device accept or reject the connection initiator.
 * @param[in] <bdAddr> initiator's address
 * @param[in] <accept> 0-reject, 1-accept
 * @return 0-success, other-fail
 */
int BleGattSecurityRsp(BdAddr bdAddr, bool accept)
{
    printf("%s BleGattSecurityRsp enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Setup scan filter params
 * @param[in] <param> BleAdvScanFilterParam
 * @return 0-success, other-fail
 */
int BleScanFilterParamSetup(BleAdvScanFilterParam *param)
{
    printf("%s BleScanFilterParamSetup enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Configure a scan filter condition
 * @param[in] <param> BleAdvScanFilterCondition
 * @return 0-success, other-fail
 */
int BleScanFilterAddRemove(BleAdvScanFilterCondition *param)
{
    printf("%s BleScanFilterAddRemove enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Clear all scan filter conditions for specific filter index
 * @param[in] <clientId> client Id
 * @param[in] <filterIndex> filter index
 * @return 0-success, other-fail
 */
int BleScanFilterClear(int clientId, int filterIndex)
{
    printf("%s BleScanFilterClear enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Enable / disable scan filter feature
 * @param[in] <clientId> client Id
 * @param[in] <enable> 0-disable, 1-enable
 * @return 0-success, other-fail
 */
int BleScanFilterEnable(int clientId, bool enable)
{
    printf("%s BleScanFilterEnable enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Set BLE scan parameters
 * @param[in] <clientId> client Id
 * @param[in] <param> BleScanParams, include scanInterval,scanWindow and so on.
 * @return 0-success, other-fail
 */
int BleSetScanParameters(int clientId, BleScanParams *param)
{
    printf("%s BleSetScanParameters enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Start Ble scan
 * @return 0-success, other-fail
 */
int BleStartScan(void)
{
    printf("%s BleStartScan enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Stop Ble scan
 * @return 0-success, other-fail
 */
int BleStopScan(void)
{
    printf("%s BleStopScan enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

static void ble_enable_cb_adapt(errcode_t status)
{
    printf("%s ble_enable_cb_adapt status: %d.\n", BLE_HILINK_SERVER_LOG, status);
    return;
}

static void ble_disable_cb_adapt(errcode_t status)
{
    printf("%s ble_disenable_cb_adapt status: %d.\n", BLE_HILINK_SERVER_LOG, status);
    return;
}

static void set_adv_data_cb_adapt(uint8_t adv_id, errcode_t status)
{
    printf("%s set_adv_data_cb_adapt status: %d.\n", BLE_HILINK_SERVER_LOG, status);
    return;
}

static void set_adv_param_cb_adapt(uint8_t adv_id, errcode_t status)
{
    printf("%s set_adv_param_cb_adapt status: %d.\n", BLE_HILINK_SERVER_LOG, status);
    return;
}

static void set_scan_param_cb_adapt(errcode_t status)
{
    printf("%s set_scan_param_cb_adapt status: %d.\n", BLE_HILINK_SERVER_LOG, status);
    return;
}

static void start_adv_cb_adapt(uint8_t adv_id, adv_status_t status)
{
    printf("%s start_adv_cb_adapt enter adv_id:%d, status: %d.\n", BLE_HILINK_SERVER_LOG, adv_id, status);
    g_ble_gap_cb->advEnableCb(adv_id, 0);
    return;
}

static void stop_adv_cb_adapt(uint8_t adv_id, adv_status_t status)
{
    printf("%s stop_adv_cb_adapt enter status: %d.\n", BLE_HILINK_SERVER_LOG, status);
    g_ble_gap_cb->advDisableCb(adv_id, 0);
    return;
}

static void scan_result_cb_adapt(gap_scan_result_data_t *scan_result_data)
{
    printf("%s scan_result_cb_adapt enter.\n", BLE_HILINK_SERVER_LOG);
    return;
}

static void conn_state_change_cb_adapt(uint16_t conn_id, bd_addr_t *addr,
    gap_ble_conn_state_t conn_state, gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    printf("%s connect state change conn_id: %d, status: %d, pair_status:%d, disc_reason %x\n",
        BLE_HILINK_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);

    BdAddr bd_addr = {0};
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        printf("%s conn_state_change_cb_adapt conn succ.\n", BLE_HILINK_SERVER_LOG);
        (void)memcpy_s(bd_addr.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN);
        if (g_ble_gatts_cb->connectServerCb) {
            g_ble_gatts_cb->connectServerCb(conn_id, g_server_id, &bd_addr);
        }
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        printf("%s conn_state_change_cb_adapt disconnect.\n", BLE_HILINK_SERVER_LOG);
        errcode_t ret = gap_ble_start_adv(BLE_ADV_HANDLE_DEFAULT);
        if (ret != ERRCODE_BT_SUCCESS) {
            printf("%s conn_state_change_cb_adapt gap_ble_start_adv error.\n", BLE_HILINK_SERVER_LOG);
            return;
        }
        g_adv_status = 1;
    }
}

gap_ble_callbacks_t g_gap_callback = {
    .ble_enable_cb          = ble_enable_cb_adapt,
    .ble_disable_cb         = ble_disable_cb_adapt,
    .set_adv_data_cb        = set_adv_data_cb_adapt,
    .set_adv_param_cb       = set_adv_param_cb_adapt,
    .set_scan_param_cb      = set_scan_param_cb_adapt,
    .start_adv_cb           = start_adv_cb_adapt,
    .stop_adv_cb            = stop_adv_cb_adapt,
    .scan_result_cb         = scan_result_cb_adapt,
    .conn_state_change_cb   = conn_state_change_cb_adapt
};

/*
 * @brief Callback invoked for gatt common function
 * @param[in] <BtGattCallbacks> Callback funcs
 * @return 0-success, other-fail
 */
int BleGattRegisterCallbacks(BtGattCallbacks *func)
{
    printf("%s BleGattRegisterCallbacks enter.\n", BLE_HILINK_SERVER_LOG);
    errcode_t ret = ERRCODE_BT_FAIL;
    if (func != NULL) {
        g_ble_gap_cb = func;
        ret = gap_ble_register_callbacks(&g_gap_callback);
        if (ret != ERRCODE_BT_SUCCESS) {
            printf("%s gap_ble_register_callbacks fail.\n", BLE_HILINK_SERVER_LOG);
            return ERRCODE_BT_FAIL;
        }
    }

    return 0;
}

/*
 * @brief Start advertising include set adv data.
 * This API will not described in the development manual, only for Hilink.
 * @return 0-success, other-fail
 */
int BleStartAdvEx(int *advId, const StartAdvRawData rawData, BleAdvParams advParam)
{
    printf("%s BleStartAdvEx enter.\n", BLE_HILINK_SERVER_LOG);
    errcode_t ret = ERRCODE_BT_FAIL;
    gap_ble_adv_params_t cfg_adv_params = {0};

    if (g_adv_status == 0) {
        *advId = BLE_ADV_HANDLE_DEFAULT;
        cfg_adv_params.min_interval         = advParam.minInterval;
        cfg_adv_params.max_interval         = advParam.maxInterval;
        cfg_adv_params.duration             = advParam.duration;
        cfg_adv_params.channel_map          = advParam.channelMap;   /* 广播通道选择bitMap, 可参考BleAdvChannelMap */
        cfg_adv_params.adv_type             = advParam.advType;
        cfg_adv_params.adv_filter_policy    = advParam.advFilterPolicy;
        cfg_adv_params.peer_addr.type       = advParam.peerAddrType;
        cfg_adv_params.own_addr.type        = advParam.ownAddrType;
        cfg_adv_params.tx_power             = advParam.txPower;
        (void)memcpy_s(&cfg_adv_params.peer_addr.addr, BD_ADDR_LEN, advParam.peerAddr.addr, BD_ADDR_LEN);
        (void)memset_s(&cfg_adv_params.own_addr.addr, BD_ADDR_LEN, 0, BD_ADDR_LEN);
        ret = gap_ble_set_adv_param(BLE_ADV_HANDLE_DEFAULT, &cfg_adv_params);
        if (ret != ERRCODE_BT_SUCCESS) {
            printf("%s BleStartAdvEx gap_ble_set_adv_param error.\n", BLE_HILINK_SERVER_LOG);
            return -1;
        }

        gap_ble_config_adv_data_t cfg_adv_data = {0};
        cfg_adv_data.adv_length         = rawData.advDataLen;
        cfg_adv_data.adv_data           = rawData.advData;        /* set adv data */
        cfg_adv_data.scan_rsp_length    = rawData.rspDataLen;
        cfg_adv_data.scan_rsp_data      = rawData.rspData;      /* set scan response data */
        ret = gap_ble_set_adv_data(BLE_ADV_HANDLE_DEFAULT, &cfg_adv_data);
        if (ret != ERRCODE_BT_SUCCESS) {
            printf("%s BleStartAdvEx gap_ble_set_adv_data error.\n", BLE_HILINK_SERVER_LOG);
            return -1;
        }

        ret = gap_ble_start_adv(BLE_ADV_HANDLE_DEFAULT);
        if (ret != ERRCODE_BT_SUCCESS) {
            printf("%s BleStartAdvEx gap_ble_start_adv error.\n", BLE_HILINK_SERVER_LOG);
            return -1;
        }
        g_adv_status = 1;
    } else {
        printf("%s BleStartAdvEx already adv.\n", BLE_HILINK_SERVER_LOG);
        return -1;
    }
    return 0;
}

/*
 * @brief gatt server application register, callback return serverId
 * @param[in] <appUuid> specified by upper layer
 * @return 0-success, other-fail
 */
int BleGattsRegister(BtUuid appUuid)
{
    printf("%s BleGattsRegister enter.\n", BLE_HILINK_SERVER_LOG);
    bt_uuid_t app_uuid = {0};
    app_uuid.uuid_len = appUuid.uuidLen;
    if (memcpy_s(app_uuid.uuid, app_uuid.uuid_len, appUuid.uuid, appUuid.uuidLen) != EOK) {
        return ERRCODE_BT_FAIL;
    }
    gatts_register_server(&app_uuid, &g_server_id);
    return ERRCODE_BT_SUCCESS;
}

/*
 * @brief gatt server deregister
 * @param[in] <clientId> server interface Id
 * @return 0-success, other-fail
 */
int BleGattsUnRegister(int serverId)
{
    printf("%s BleGattsUnRegister enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Cancel connection with remote device
 * @param[in] <serverId> server interface id
 * @param[in] <bdAddr>   remote address
 * @param[in] <connId>   connection index.
 * @return 0-success, other-fail
 */
int BleGattsDisconnect(int serverId, BdAddr bdAddr, int connId)
{
    printf("%s BleGattsDisconnect enter.\n", BLE_HILINK_SERVER_LOG);
    bd_addr_t bd_addr = { 0 };
    bd_addr.type = BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS;
    memcpy_s(bd_addr.addr, BD_ADDR_LEN, bdAddr.addr, BD_ADDR_LEN);
    return gap_ble_disconnect_remote_device(&bd_addr);
}

/*
 * @brief add service
 * @param[in] <serverId>  server interface id
 * @param[in] <srvcUuid>  service uuid and uuid length
 * @param[in] <isPrimary> is primary or secondary service.
 * @param[in] <number>    service characther attribute number.
 * @return 0-success, other-fail
 */
int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number)
{
    printf("%s BleGattsAddService enter.\n", BLE_HILINK_SERVER_LOG);
    errcode_t ret = 0;
    bt_uuid_t service_uuid = {0};
    uint16_t service_handle = 0;
    service_uuid.uuid_len = srvcUuid.uuidLen;
    (void)memcpy_s(service_uuid.uuid, srvcUuid.uuidLen, (uint8_t *)srvcUuid.uuid, srvcUuid.uuidLen);

    while (1) {
        osDelay(10);  /* 等待10 tick */
        /* APP 调用StartService为异步接口 无法保证Add Service时 前一个Service已完成Start */
        if (g_service_flag == 0) {
            ret = gatts_add_service_sync(g_server_id, &service_uuid, 1, &service_handle);
            g_service_flag = 1;
            if (ret != ERRCODE_BT_SUCCESS) {
                printf("%s BleGattsStartServiceEx Add Service Fail, ret:%x !!!\n", BLE_HILINK_SERVER_LOG, ret);
            }
            break;
        }
    }
    g_srvc_handle = service_handle;
    printf("%s BleGattsAddService end, srvcHandle:%d.\n", BLE_HILINK_SERVER_LOG, g_srvc_handle);

    return ret;
}

/*
 * @brief add characteristic
 * @param[in] <serverId>    server interface id
 * @param[in] <srvcHandle>  service handle
 * @param[in] <characUuid>  characteristic uuid and uuid length
 * @param[in] <properties>  e.g. (OHOS_GATT_CHARACTER_PROPERTY_BIT_BROADCAST | OHOS_GATT_CHARACTER_PROPERTY_BIT_READ)
 * @param[in] <permissions> e.g. (OHOS_GATT_PERMISSION_READ | OHOS_GATT_PERMISSION_WRITE)
 * @return 0-success, other-fail
 */
int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid,
                              int properties, int permissions)
{
    printf("%s BleGattsAddCharacteristic enter, srvcHandle:%d.\n", BLE_HILINK_SERVER_LOG, srvcHandle);
    bt_uuid_t chara_uuid = {0};
    gatts_add_chara_info_t chara_info = {0};
    gatts_add_character_result_t chara_result = {0};
    chara_uuid.uuid_len = characUuid.uuidLen;
    (void)memcpy_s(chara_uuid.uuid, characUuid.uuidLen, (uint8_t *)characUuid.uuid, characUuid.uuidLen);

    chara_info.chara_uuid   = chara_uuid;
    chara_info.properties   = properties;
    chara_info.permissions  = perm_bt_to_bluez(permissions);
    chara_info.value_len    = sizeof(g_chara_val);
    chara_info.value        = g_chara_val;
    int ret = gatts_add_characteristic_sync(g_server_id, srvcHandle, &chara_info, &chara_result);
    printf("%s BleGattsAddCharacteristic ret:%d handle:%d, value_handle:%d.\n",
        BLE_HILINK_SERVER_LOG, ret, chara_result.handle, chara_result.value_handle);

    g_cb_chara_handle = chara_result.value_handle;
    return ret;
}

/*
 * @brief add descriptor
 * @param[in] <serverId>    server interface id
 * @param[in] <srvcHandle>  service handle
 * @param[in] <descUuid>    descriptor uuid and uuid length
 * @param[in] <permissions> e.g. (OHOS_GATT_PERMISSION_READ | OHOS_GATT_PERMISSION_WRITE)
 * @return 0-success, other-fail
 */
int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions)
{
    printf("%s BleGattsAddDescriptor enter.\n", BLE_HILINK_SERVER_LOG);
    bt_uuid_t desc_uuid = {0};
    gatts_add_desc_info_t descriptor = {0};
    uint16_t desc_handle = 0;

    desc_uuid.uuid_len = descUuid.uuidLen;
    (void)memcpy_s(desc_uuid.uuid, descUuid.uuidLen, (uint8_t *)descUuid.uuid, descUuid.uuidLen);

    descriptor.desc_uuid    = desc_uuid;
    descriptor.permissions  = perm_bt_to_bluez(permissions);
    descriptor.value_len    = sizeof(g_desc_val);
    descriptor.value        = g_desc_val;

    int ret = gatts_add_descriptor_sync(g_server_id, srvcHandle, &descriptor, &desc_handle);
    g_cb_desc_handle = desc_handle;
    printf("%s BleGattsAddDescriptor ret:%d desc_handle:%d.\n", BLE_HILINK_SERVER_LOG, ret, desc_handle);
    return ret;
}

/*
 * @brief start service
 * @param[in] <serverId>    server interface id
 * @param[in] <srvcHandle>  service handle
 * @return 0-success, other-fail
 */
int BleGattsStartService(int serverId, int srvcHandle)
{
    printf("%s BleGattsStartService enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief start service
 * @param[in] <serverId>    server interface id
 * @param[in] <srvcHandle>  service handle
 * @return 0-success, other-fail
 */
int BleGattsStopService(int serverId, int srvcHandle)
{
    printf("%s BleGattsStopService enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief remove a service from the list of provided services
 * @param[in] <serverId>   server interface id
 * @param[in] <srvcHandle>  service handle
 * @return 0-success, other-fail
 */
int BleGattsDeleteService(int serverId, int srvcHandle)
{
    printf("%s BleGattsDeleteService enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief remove all services from the list of provided services
 * @param[in] <serverId>   server interface id
 * @return 0-success, other-fail
 */
int BleGattsClearServices(int serverId)
{
    printf("%s BleGattsClearServices enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}

/*
 * @brief Send a response to a read or write request to a remote device.
 * @param[in] <serverId> server interface id
 * @param[in] <GattsSendRspParam> response param
 * @return 0-success, other-fail
 */
int BleGattsSendResponse(int serverId, GattsSendRspParam *param)
{
    printf("%s BleGattsSendResponse enter, handle:%d.\n", BLE_HILINK_SERVER_LOG, param->attrHandle);
    int ret = 0;
    gatts_send_rsp_t rsp_param = {0};
    rsp_param.request_id = g_server_request_id;
    rsp_param.status = 0;
    rsp_param.offset = 0;
    rsp_param.value_len = param->valueLen;
    rsp_param.value = (uint8_t *)param->value;

    if (g_gatt_write_flag) {
        ret = gatts_send_response(g_server_id, param->connectId, &rsp_param);
        printf("%s BleGattsSendResponse send write resp, ret:%x.\n", BLE_HILINK_SERVER_LOG, ret);
    } else {
        ret = gatts_send_response(g_server_id, param->connectId, &rsp_param);
        printf("%s BleGattsSendResponse send read resp, ret:%x.\n", BLE_HILINK_SERVER_LOG, ret);
    }

    return ret;
}

/*
 * @brief Send a notification or indication that a local characteristic has been updated
 * @param[in] <serverId> server interface id
 * @param[in] <GattsSendIndParam> indication param
 * @return 0-success, other-fail
 */
int BleGattsSendIndication(int serverId, GattsSendIndParam *param)
{
    printf("%s BleGattsSendIndication enter, handle:%d.\n", BLE_HILINK_SERVER_LOG, param->attrHandle);

    int ret = 0;
    gatts_ntf_ind_t ntf_param = {0};
    ntf_param.attr_handle   = g_indicate_handle;
    ntf_param.value_len     = param->valueLen;
    ntf_param.value         = (uint8_t *)param->value;
    ret = gatts_notify_indicate(g_server_id, param->connectId, &ntf_param);
    if (ret != 0) {
        printf("%s gatts_notify_indicate fail, ret:%x.\n", BLE_HILINK_SERVER_LOG, ret);
    }

    int yet = ret ? 0 : -1;
    if (g_ble_gatts_cb->indicationSentCb != NULL) {
        printf("%s indicationSentCb form Hilink.\n", BLE_HILINK_SERVER_LOG);
        g_ble_gatts_cb->indicationSentCb(param->connectId, yet);
    }

    BleGattServiceIndicate indicate_func = get_chara_ind_func(param->connectId, g_indicate_handle);
    if (indicate_func != NULL) {
        ret = indicate_func(ntf_param.value, ntf_param.value_len);
        if (ret != 0) {
            printf("indicateFunc fail %d.\n", ret);
        }
    }
    return ret;
}

/*
 * @brief Set the encryption level of the data transmission channel during connection
 * @param[in] <bdAddr> remote address
 * @param[in] <secAct> BleSecAct
 * @return 0-success, other-fail
 */
int BleGattsSetEncryption(BdAddr bdAddr, BleSecAct secAct)
{
    printf("%s BleGattsSetEncryption enter, secAct:%d.\n", BLE_HILINK_SERVER_LOG, secAct);

    gap_ble_sec_params_t sec_params = {0};
    sec_params.bondable = 1;
    sec_params.io_capability = g_io_cap_mode;
    sec_params.sc_enable = 0;
    sec_params.sc_mode = secAct;
    int ret = gap_ble_set_sec_param(&sec_params);
    if (ret != 0) {
        printf("%s gap_ble_set_sec_param fail, ret:%d.\n", BLE_HILINK_SERVER_LOG, ret);
    }
    return 0;
}

void add_service_cb_adapt(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    printf("%s add_service_cb server_id:%d, handle:%d, status:%d.\n", BLE_HILINK_SERVER_LOG, server_id, handle, status);
    return;
}

void add_characteristic_cb_adapt(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status)
{
    printf("%s add_characteristic_cb server: %d srvc_hdl: %d char_hdl: %d char_val_hdl: %d uuid_len: %d \n",
        BLE_HILINK_SERVER_LOG, server_id, service_handle, result->handle, result->value_handle, uuid->uuid_len);
    return;
}

void add_descriptor_cb_adapt(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    printf("%s add_descriptor_cb server: %d srv_hdl: %d desc_hdl: %d uuid_len:%d.\n",
        BLE_HILINK_SERVER_LOG, server_id, service_handle, handle, uuid->uuid_len);
    return;
}

void start_service_cb_adapt(uint8_t server_id, uint16_t handle, errcode_t status)
{
    printf("%s start_service_cb server: %d srv_hdl: %d status: %d\n", BLE_HILINK_SERVER_LOG, server_id, handle, status);

    g_service_flag = 0;
    if (g_ble_gatts_cb->serviceStartCb) {
        g_ble_gatts_cb->serviceStartCb(status, server_id, handle);
    }
    return;
}

void stop_service_cb_adapt(uint8_t server_id, uint16_t handle, errcode_t status)
{
    printf("%s stop_service_cb server: %d srv_hdl: %d status: %d\n",
        BLE_HILINK_SERVER_LOG, server_id, handle, status);
    if (g_ble_gatts_cb->serviceStopCb) {
        g_ble_gatts_cb->serviceStopCb(status, server_id, handle);
    }
    return;
}

void delete_service_cb_adapt(uint8_t server_id, errcode_t status)
{
    printf("%s stop_service_cb server: %d srv_hdl: %d status: %d\n",
        BLE_HILINK_SERVER_LOG, server_id, status);
    return;
}

void read_request_cb_adapt(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    printf("%s read_request_cb_adapt--server_id:%d conn_id:%d status:%d\n",
        BLE_HILINK_SERVER_LOG, server_id, conn_id, status);

    char buff[MAX_READ_REQ_LEN] = {0};
    unsigned int length = MAX_READ_REQ_LEN;
    int ret = 0;

    BleGattServiceRead read_func = get_chara_read_func(conn_id, read_cb_para->handle);
    if (read_func != NULL) {
        ret = read_func((uint8_t *)buff, &length);
        if (ret != ERRCODE_BT_SUCCESS) {
            printf("read_func fail %d.\n", ret);
            length = 0;
        }
    }

    GattsSendRspParam rsp = {0};
    rsp.connectId       = conn_id;
    rsp.status          = OHOS_GATT_SUCCESS;
    rsp.attrHandle      = read_cb_para->handle;

    if (length > read_cb_para->offset) {
        rsp.valueLen    = length - read_cb_para->offset;
        rsp.value       = buff   + read_cb_para->offset;
    } else {
        rsp.valueLen    = 0;
        rsp.value       = buff;
    }

    BleGattsSendResponse(g_server_id, &rsp);

    g_gatt_write_flag = 0;
    g_server_request_id = read_cb_para->request_id;
}

void write_request_cb_adapt(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    printf("%s write_request_cb_adapt--request_id:%d att_handle:%d data_len:%d\n",
        BLE_HILINK_SERVER_LOG, write_cb_para->request_id, write_cb_para->handle, write_cb_para->length);

    BleGattServiceWrite write_func = get_chara_write_func(conn_id, write_cb_para->handle);
    if (write_func != NULL) {
        int ret = write_func(write_cb_para->value, write_cb_para->length);
        if (ret != 0) {
            printf("write_func fail %d.\n", ret);
        }
    }

    g_server_request_id = write_cb_para->request_id;
    g_gatt_write_flag = 1;

    if (write_cb_para->is_prep) {
        GattsSendRspParam rsp = {0};
        char one_byte_rsp = 0;
        rsp.connectId   = conn_id;
        rsp.status      = OHOS_GATT_SUCCESS;
        rsp.attrHandle  = write_cb_para->handle;
        rsp.valueLen    = sizeof(one_byte_rsp);
        rsp.value       = &one_byte_rsp;

        int ret = BleGattsSendResponse(g_server_id, &rsp);
        if (ret != ERRCODE_BT_SUCCESS) {
            printf("BleGattsSendResponse fail %d.\n", ret);
        }
    }
}

void mtu_changed_cb_adapt(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    printf("%s mtu_changed_cb server_id:%d conn_id:%d mtu_size: %d status: %d\n",
        BLE_HILINK_SERVER_LOG, server_id, conn_id, mtu_size, status);

    g_ble_gatts_cb->mtuChangeCb(conn_id, mtu_size);
}

gatts_callbacks_t g_gatt_callback = {
    .add_service_cb          = add_service_cb_adapt,
    .add_characteristic_cb   = add_characteristic_cb_adapt,
    .add_descriptor_cb       = add_descriptor_cb_adapt,
    .start_service_cb        = start_service_cb_adapt,
    .stop_service_cb         = stop_service_cb_adapt,
    .delete_service_cb       = delete_service_cb_adapt,
    .read_request_cb         = read_request_cb_adapt,
    .write_request_cb        = write_request_cb_adapt,
    .mtu_changed_cb          = mtu_changed_cb_adapt
};

/*
 * @brief Callback invoked for gatt server function
 * @param[in] <BtGattServerCallbacks> Callback funcs
 * @return 0-success, other-fail
 */
int BleGattsRegisterCallbacks(BtGattServerCallbacks *func)
{
    printf("%s BleGattsRegisterCallbacks enter.\n", BLE_HILINK_SERVER_LOG);
    errcode_t ret = ERRCODE_BT_FAIL;
    if (func != NULL) {
        g_ble_gatts_cb = func;
        ret = gatts_register_callbacks(&g_gatt_callback);
        if (ret != ERRCODE_BT_SUCCESS) {
            printf("%s gatts_register_callbacks fail, ret:%d.\n", BLE_HILINK_SERVER_LOG, ret);
            return ERRCODE_BT_FAIL;
        }
    }

    return 0;
}

static errcode_t ble_uuid_server_init(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    bt_uuid_t app_uuid = {0};
    char uuid[] = {0x12, 0x34};
    app_uuid.uuid_len = sizeof(uuid);
    ret |= memcpy_s(app_uuid.uuid, app_uuid.uuid_len, uuid, sizeof(uuid));
    if (ret != 0) {
        return -1;
    }
    ret |= gatts_register_server(&app_uuid, &g_server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        printf("%s gatts_register_server fail.\n", BLE_HILINK_SERVER_LOG);
        return -1;
    }
    return ret;
}

/*
 * @brief Start sevice include add service/characteristic/Descriptor option.
 * This API will not described in the development manual, only for Hilink.
 * @return 0-success, other-fail
 */


static uint8_t g_hilink_group_cnt = 0;
#define HILINK_SERVICE_NUM 3
#define HILINK_CHARA_NUM 4
static char g_hilink_group_uuid[HILINK_SERVICE_NUM + HILINK_CHARA_NUM][OHOS_BLE_UUID_MAX_LEN] = {
    {0x15, 0xF1, 0xE4, 0x00, 0xA2, 0x77, 0x43, 0xFC, 0xA4, 0x84, 0xDD, 0x39, 0xEF, 0x8A, 0x91, 0x00},
    {0x15, 0xF1, 0xE4, 0x01, 0xA2, 0x77, 0x43, 0xFC, 0xA4, 0x84, 0xDD, 0x39, 0xEF, 0x8A, 0x91, 0x00},
    {0x15, 0xF1, 0xE5, 0x00, 0xA2, 0x77, 0x43, 0xFC, 0xA4, 0x84, 0xDD, 0x39, 0xEF, 0x8A, 0x91, 0x00},
    {0x15, 0xF1, 0xE5, 0x01, 0xA2, 0x77, 0x43, 0xFC, 0xA4, 0x84, 0xDD, 0x39, 0xEF, 0x8A, 0x91, 0x00},
    {0x15, 0xF1, 0xE6, 0x00, 0xA2, 0x77, 0x43, 0xFC, 0xA4, 0x84, 0xDD, 0x39, 0xEF, 0x8A, 0x91, 0x00},
    {0x15, 0xF1, 0xE6, 0x02, 0xA2, 0x77, 0x43, 0xFC, 0xA4, 0x84, 0xDD, 0x39, 0xEF, 0x8A, 0x91, 0x00},
    {0x15, 0xF1, 0xE6, 0x01, 0xA2, 0x77, 0x43, 0xFC, 0xA4, 0x84, 0xDD, 0x39, 0xEF, 0x8A, 0x91, 0x00}
};
static char g_hilink_cccd_uuid[UUID16_LEN] = {0x29, 0x02};

static void hilnk_group_add(void)
{
    g_hilink_group_cnt++;
}

static void convert_uuid(uint8_t *uuid_input, UuidType type, BtUuid *uuid_output)
{
    uint8_t temp_uuid[OHOS_BLE_UUID_MAX_LEN] = {0};
    int ret = 0;

    switch (type) {
        case OHOS_UUID_TYPE_16_BIT:
            uuid_output->uuidLen = UUID16_LEN;
            break;
        case OHOS_UUID_TYPE_32_BIT:
            uuid_output->uuidLen = UUID32_LEN;
            break;
        case OHOS_UUID_TYPE_128_BIT:
            uuid_output->uuidLen = UUID128_LEN;
            break;
        default:
            uuid_output->uuidLen = 0;
            break;
    }

    uuid_output->uuid = (char *)uuid_input;
    ret = memcpy_s(temp_uuid, OHOS_BLE_UUID_MAX_LEN, g_hilink_group_uuid[g_hilink_group_cnt], OHOS_BLE_UUID_MAX_LEN);
    if (ret != 0) {
        printf("%s convert_uuid memcpy_s fail.\n", BLE_HILINK_SERVER_LOG);
        return;
    }
    reverse_uuid(temp_uuid, uuid_output->uuid, uuid_output->uuidLen);
    return;
}

static void set_chara_func(BleGattAttr *attr, uint8_t is_indicate)
{
    g_charas_func[g_chara_cnt].conn_id = 0;
    if (is_indicate == 0) {
        if (attr->attrType == OHOS_BLE_ATTRIB_TYPE_CHAR) {
            g_charas_func[g_chara_cnt].attr_handle = g_cb_chara_handle;
        } else {
            g_charas_func[g_chara_cnt].attr_handle = g_cb_desc_handle;
        }
        g_charas_func[g_chara_cnt].read       = attr->func.read;
        g_charas_func[g_chara_cnt].write      = attr->func.write;
        g_charas_func[g_chara_cnt].indicate   = attr->func.indicate;
    } else {
        g_charas_func[g_chara_cnt].attr_handle = g_cb_desc_handle;
    }
    g_chara_cnt++;
}

int BleGattsStartServiceEx(int *srvcHandle, BleGattService *srvcInfo)
{
    printf("%s BleGattsStartServiceEx enter srvHandle:%d.\n", BLE_HILINK_SERVER_LOG, *srvcHandle);
    errcode_t ret = ERRCODE_BT_SUCCESS;
    uint8_t is_indicate = 0;
    BtUuid ble_uuid = {0};
    uint16_t service_handle = 0;

    if (g_server_id == INVALID_SERVER_ID) {
        if (ble_uuid_server_init() != ERRCODE_BT_SUCCESS) {
            printf("%s gatts_register_server fail.\n", BLE_HILINK_SERVER_LOG);
            return -1;
        }
    }

    for (unsigned int i = 0; i < srvcInfo->attrNum; i++) {
        BleGattAttr *attr = &(srvcInfo->attrList[i]);
        convert_uuid(attr->uuid, attr->uuidType, &ble_uuid);

        switch (attr->attrType) {
            case OHOS_BLE_ATTRIB_TYPE_SERVICE:
                ret = BleGattsAddService(g_server_id, ble_uuid, 1, srvcInfo->attrNum);
                if (ret != ERRCODE_BT_SUCCESS) {
                    printf("%s BleGattsAddService failed, ret:0x%x\r\n", BLE_HILINK_SERVER_LOG, ret);
                }
                hilnk_group_add();
                break;
            case OHOS_BLE_ATTRIB_TYPE_CHAR:
                ret = BleGattsAddCharacteristic(g_server_id, g_srvc_handle,
                    ble_uuid, attr->properties, attr->permission);
                if (ret != ERRCODE_BT_SUCCESS) {
                    printf("%s BleGattsAddCharacteristic failed, ret:0x%x\r\n", BLE_HILINK_SERVER_LOG, ret);
                }
                hilnk_group_add();
                break;
            case OHOS_BLE_ATTRIB_TYPE_CHAR_VALUE:
                break;
            case OHOS_BLE_ATTRIB_TYPE_CHAR_CLIENT_CONFIG:
                break;
            case OHOS_BLE_ATTRIB_TYPE_CHAR_USER_DESCR:
                ret = BleGattsAddDescriptor(g_server_id, g_srvc_handle, ble_uuid, attr->permission);
                if (ret != ERRCODE_BT_SUCCESS) {
                    printf("%s BleGattsAddDescriptor failed:%x.\n", BLE_HILINK_SERVER_LOG, ret);
                }
                hilnk_group_add();
                break;
            default:
                printf("Unknown\n");
        }

        if ((attr->attrType == OHOS_BLE_ATTRIB_TYPE_CHAR_USER_DESCR) || (attr->attrType == OHOS_BLE_ATTRIB_TYPE_CHAR)) {
            set_chara_func(attr, 0);
        }

        if ((attr->properties & OHOS_GATT_CHARACTER_PROPERTY_BIT_INDICATE) ||
            (attr->properties & OHOS_GATT_CHARACTER_PROPERTY_BIT_NOTIFY)) {
            is_indicate = 1;
            g_indicate_handle = g_cb_chara_handle;
        }
    }

    if (is_indicate) {
        ble_uuid.uuid = g_hilink_cccd_uuid;
        ble_uuid.uuidLen = sizeof(g_hilink_cccd_uuid);
        ret = BleGattsAddDescriptor(g_server_id, g_srvc_handle,
            ble_uuid, OHOS_GATT_PERMISSION_READ | OHOS_GATT_PERMISSION_WRITE);
        if (ret == ERRCODE_BT_SUCCESS) {
            set_chara_func(NULL, is_indicate);
        }
    }

    if (g_srvc_handle != 0) {
        ret = gatts_start_service(g_server_id, g_srvc_handle);
        *srvcHandle = g_srvc_handle;
    }

    return 0;
}

/*
 * @brief Stop service.
 * This API will not described in the development manual, only for Hilink.
 * @return 0-success, other-fail
 */
int BleGattsStopServiceEx(int srvcHandle)
{
    printf("%s BleGattsStopServiceEx enter.\n", BLE_HILINK_SERVER_LOG);
    return 0;
}
