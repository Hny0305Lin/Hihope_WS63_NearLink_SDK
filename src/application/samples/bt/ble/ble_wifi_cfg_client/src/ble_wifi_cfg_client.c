/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: BT config wifi Service client module.
 */
#include "securec.h"
#include "osal_list.h"
#include "test_suite_uart.h"
#include "bts_le_gap.h"
#include "bts_gatt_client.h"
#include "ble_wifi_cfg_scan.h"

#define UUID16_LEN 2

#define sample_at_log_print(fmt, args...) test_suite_uart_sendf(fmt, ##args)

#define BLE_WIFI_CFG_CLIENT_LOG "[ble wifi cfg client]"
#define BLE_WIFI_CFG_CLIENT_ERROR "[ble wifi cfg error]"

/* client id, invalid client id is "0" */
uint8_t g_ble_wifi_cfg_client_id = 0;

static uint16_t g_ble_wifi_cfg_conn_id = 0;

/* max transport unit, default is 100 */
static uint16_t g_ble_wifi_cfg_mtu = 100;

/* client app uuid for test */
static bt_uuid_t g_ble_wifi_cfg_client_app = {UUID16_LEN, {0}};


uint8_t ble_wifi_adv_data[31] = {0x02, 0x01, 0x02, 0x13, 0xFF, 0x7D, 0x02, 0x0E, 0x70, 0x80, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x01, 0x06, 0xCA, 0x2D, 0x28, 0xA0, 0x9D, 0xA3, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define HW_ADV_DATA_LEN 0x17

static void ble_wifi_cfg_client_discover_service_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_service_result_t *service, errcode_t status)
{
    gattc_discovery_character_param_t param = {0};
    sample_at_log_print("[GATTClient]Discovery service----client:%d conn_id:%d\n", client_id, conn_id);
    sample_at_log_print("start handle:%d end handle:%d uuid_len:%d\n            uuid:",
        service->start_hdl, service->end_hdl, service->uuid.uuid_len);
    for (uint8_t i = 0; i < service->uuid.uuid_len; i++) {
        sample_at_log_print("%02x", service->uuid.uuid[i]);
    }
    sample_at_log_print("\n            status:%d\n", status);
    param.service_handle = service->start_hdl;
    param.uuid.uuid_len = 0; /* uuid length is zero, discover all character */
    gattc_discovery_character(g_ble_wifi_cfg_client_id, conn_id, &param);
}

static void ble_wifi_cfg_client_discover_character_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_character_result_t *character, errcode_t status)
{
    sample_at_log_print("[GATTClient]Discovery character----client:%d conn_id:%d uuid_len:%d\n            uuid:",
        client_id, conn_id, character->uuid.uuid_len);
    for (uint8_t i = 0; i < character->uuid.uuid_len; i++) {
        sample_at_log_print("%02x", character->uuid.uuid[i]);
    }
    sample_at_log_print("\n            declare handle:%d value handle:%d properties:%x\n", character->declare_handle,
        character->value_handle, character->properties);
    sample_at_log_print("            status:%d\n", status);
    gattc_discovery_descriptor(g_ble_wifi_cfg_client_id, conn_id, character->declare_handle);
}

static void ble_wifi_cfg_client_discover_descriptor_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_descriptor_result_t *descriptor, errcode_t status)
{
    sample_at_log_print("[GATTClient]Discovery descriptor----client:%d conn_id:%d uuid len:%d\n            uuid:",
        client_id, conn_id, descriptor->uuid.uuid_len);
    for (uint8_t i = 0; i < descriptor->uuid.uuid_len; i++) {
        sample_at_log_print("%02x", descriptor->uuid.uuid[i]);
    }
    sample_at_log_print("\n            descriptor handle:%d\n", descriptor->descriptor_hdl);
    sample_at_log_print("            status:%d\n", status);
}

static void ble_wifi_cfg_client_discover_service_compl_cbk(uint8_t client_id, uint16_t conn_id, bt_uuid_t *uuid,
    errcode_t status)
{
    sample_at_log_print(
        "[GATTClient]Discovery service complete----client:%d conn_id:%d uuid len:%d\n            uuid:",
        client_id, conn_id, uuid->uuid_len);
    for (uint8_t i = 0; i < uuid->uuid_len; i++) {
        sample_at_log_print("%02x", uuid->uuid[i]);
    }
    sample_at_log_print("status:%d\n", status);
}

static void ble_wifi_cfg_client_discover_character_compl_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_character_param_t *param, errcode_t status)
{
    sample_at_log_print(
        "[GATTClient]Discovery character complete----client:%d conn_id:%d uuid len:%d\n            uuid:",
        client_id, conn_id, param->uuid.uuid_len);
    for (uint8_t i = 0; i < param->uuid.uuid_len; i++) {
        sample_at_log_print("%02x", param->uuid.uuid[i]);
    }
    sample_at_log_print("\n            service handle:%d\n", param->service_handle);
    sample_at_log_print("            status:%d\n", status);
}

static void ble_wifi_cfg_client_discover_descriptor_compl_cbk(uint8_t client_id, uint16_t conn_id,
    uint16_t character_handle, errcode_t status)
{
    sample_at_log_print("[GATTClient]Discovery descriptor complete----client:%d conn_id:%d\n", client_id, conn_id);
    sample_at_log_print("            charatcer handle:%d\n", character_handle);
    sample_at_log_print("            status:%d\n", status);
}

static void ble_wifi_cfg_client_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *read_result,
    gatt_status_t status)
{
    sample_at_log_print("[GATTClient]Read result----client:%d conn_id:%d\n", client_id, conn_id);
    sample_at_log_print("            handle:%d data_len:%d\ndata:", read_result->handle, read_result->data_len);
    for (uint8_t i = 0; i < read_result->data_len; i++) {
        sample_at_log_print("%02x", read_result->data[i]);
    }
    sample_at_log_print("\n            status:%d\n", status);
}

static void ble_wifi_cfg_client_read_compl_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_read_req_by_uuid_param_t *param, errcode_t status)
{
    sample_at_log_print("[GATTClient]Read by uuid complete----client:%d conn_id:%d\n", client_id, conn_id);
    sample_at_log_print("start handle:%d end handle:%d uuid len:%d\n            uuid:",
        param->start_hdl, param->end_hdl, param->uuid.uuid_len);
    for (uint8_t i = 0; i < param->uuid.uuid_len; i++) {
        sample_at_log_print("%02x", param->uuid.uuid[i]);
    }
    sample_at_log_print("\n            status:%d\n", status);
}

static void ble_wifi_cfg_client_write_cfm_cbk(uint8_t client_id, uint16_t conn_id,
    uint16_t handle, gatt_status_t status)
{
    sample_at_log_print("[GATTClient]Write result----client:%d conn_id:%d handle:%d\n", client_id, conn_id, handle);
    sample_at_log_print("            status:%d\n", status);
}

static void ble_wifi_cfg_client_mtu_changed_cbk(uint8_t client_id, uint16_t conn_id,
    uint16_t mtu_size, errcode_t status)
{
    sample_at_log_print("[GATTClient]Mtu changed----client:%d conn_id:%d mtu size:%d\n", client_id, conn_id,
        mtu_size);
    sample_at_log_print("status:%d\n", status);
}

static void ble_wifi_cfg_client_notification_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status)
{
    sample_at_log_print("[GATTClient]Receive notification----client:%d conn_id:%d\n", client_id, conn_id);
    sample_at_log_print("handle:%d data_len:%d\ndata:", data->handle, data->data_len);
    for (uint8_t i = 0; i < data->data_len; i++) {
        sample_at_log_print("%02x", data->data[i]);
    }
    sample_at_log_print("\n            status:%d\n", status);
}

static void ble_wifi_cfg_client_indication_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status)
{
    sample_at_log_print("[GATTClient]Receive indication----client:%d conn_id:%d\n", client_id, conn_id);
    sample_at_log_print("            handle:%d data_len:%d\ndata:", data->handle, data->data_len);
    for (uint8_t i = 0; i < data->data_len; i++) {
        sample_at_log_print("%02x", data->data[i]);
    }
    sample_at_log_print("\n            status:%d\n", status);
}

/* ble client set scan param callback */
void ble_wifi_cfg_set_scan_param_cbk(errcode_t status)
{
    sample_at_log_print("%s set scan param status: %d\n", 0, status);
    gap_ble_remove_all_pairs(); /* 配网业务无需多连接，因此扫描时需将其他设备断开, 然后扫描配对新设备 */
    ble_wifi_device_start_scan();
}

/* ble client scan result callback */
void ble_wifi_cfg_scan_result_cbk(gap_scan_result_data_t *scan_result_data)
{
    if (memcmp(scan_result_data->adv_data, ble_wifi_adv_data, HW_ADV_DATA_LEN) == 0) {
        gap_ble_stop_scan();
        sample_at_log_print("\naddr:");
        for (uint8_t i = 0; i < BD_ADDR_LEN; i++) {
            sample_at_log_print(" %02x: ", scan_result_data->addr.addr[i]);
        }
        bd_addr_t client_addr = { 0 };
        client_addr.type = scan_result_data->addr.type;
        if (memcpy_s(client_addr.addr, BD_ADDR_LEN, scan_result_data->addr.addr, BD_ADDR_LEN) != EOK) {
            sample_at_log_print("%s add server app addr memcpy failed\r\n", BLE_WIFI_CFG_CLIENT_ERROR);
            return;
        }
        gap_ble_connect_remote_device(&client_addr);
    }
}

/* ble client connect state change callback */
void ble_wifi_cfg_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    bd_addr_t client_addr = { 0 };
    client_addr.type = addr->type;
    g_ble_wifi_cfg_conn_id = conn_id;
    if (memcpy_s(client_addr.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN) != EOK) {
        sample_at_log_print("%s add server app addr memcpy failed\r\n", BLE_WIFI_CFG_CLIENT_ERROR);
        return;
    }
    sample_at_log_print("%s connect state change conn_id: %d, status: %d, pair_status:%d, disc_reason %x\n",
                0, conn_id, conn_state, pair_state, disc_reason);

    if (conn_state == GAP_BLE_STATE_CONNECTED  &&  pair_state == GAP_BLE_PAIR_NONE) {
        sample_at_log_print("%s connect change cbk conn_id =%d \n", 0, conn_id);
        gattc_exchange_mtu_req(g_ble_wifi_cfg_client_id, g_ble_wifi_cfg_conn_id, g_ble_wifi_cfg_mtu);
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        sample_at_log_print("%s connect change cbk conn disconnected \n", 0);
        return;
    }
}

/* ble client pair result callback */
void ble_wifi_cfg_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    sample_at_log_print("%s pair result conn_id: %d,status: %d \n", 0, conn_id, status);
    sample_at_log_print("addr:\n");
    for (uint8_t i = 0; i < BD_ADDR_LEN; i++) {
        sample_at_log_print("%2x", addr->addr[i]);
    }
    sample_at_log_print("\n");
    gattc_exchange_mtu_req(g_ble_wifi_cfg_client_id, g_ble_wifi_cfg_conn_id,
        g_ble_wifi_cfg_mtu);
}


errcode_t ble_wifi_cfg_client_callback_register(void)
{
    errcode_t ret = ERRCODE_BT_UNHANDLED;
    gap_ble_callbacks_t gap_cb = { 0 };
    gattc_callbacks_t cb = {0};

    gap_cb.set_scan_param_cb = ble_wifi_cfg_set_scan_param_cbk;
    gap_cb.scan_result_cb = ble_wifi_cfg_scan_result_cbk;
    gap_cb.conn_state_change_cb = ble_wifi_cfg_connect_change_cbk;
    gap_cb.pair_result_cb = ble_wifi_cfg_pair_result_cb;
    ret = gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        sample_at_log_print("%s reg gap cbk failed ret = %d\n", 0, ret);
    }

    cb.discovery_svc_cb = ble_wifi_cfg_client_discover_service_cbk;
    cb.discovery_svc_cmp_cb = ble_wifi_cfg_client_discover_service_compl_cbk;
    cb.discovery_chara_cb = ble_wifi_cfg_client_discover_character_cbk;
    cb.discovery_chara_cmp_cb = ble_wifi_cfg_client_discover_character_compl_cbk;
    cb.discovery_desc_cb = ble_wifi_cfg_client_discover_descriptor_cbk;
    cb.discovery_desc_cmp_cb = ble_wifi_cfg_client_discover_descriptor_compl_cbk;
    cb.read_cb = ble_wifi_cfg_client_read_cfm_cbk;
    cb.read_cmp_cb = ble_wifi_cfg_client_read_compl_cbk;
    cb.write_cb = ble_wifi_cfg_client_write_cfm_cbk;
    cb.mtu_changed_cb = ble_wifi_cfg_client_mtu_changed_cbk;
    cb.notification_cb = ble_wifi_cfg_client_notification_cbk;
    cb.indication_cb = ble_wifi_cfg_client_indication_cbk;
    ret = gattc_register_callbacks(&cb);

    return ret;
}

errcode_t ble_wifi_cfg_client_init(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    ret |= enable_ble();
    ret |= ble_wifi_cfg_client_callback_register();
    ret |= gattc_register_client(&g_ble_wifi_cfg_client_app, &g_ble_wifi_cfg_client_id);
    return ret;
}

errcode_t ble_wifi_cfg_client_discover_all_service(uint16_t conn_id)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    bt_uuid_t service_uuid = {0}; /* uuid length is zero, discover all service */
    ret |= gattc_discovery_service(g_ble_wifi_cfg_client_id, conn_id, &service_uuid);
    return ret;
}

