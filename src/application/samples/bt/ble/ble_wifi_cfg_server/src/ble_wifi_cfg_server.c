/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: ble wifi config server sample.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "osal_addr.h"
#include "bts_def.h"
#include "securec.h"
#include "errcode.h"
#include "test_suite_uart.h"
#include "bts_def.h"
#include "bts_le_gap.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "ble_wifi_cfg_server.h"

/* server app uuid for test */
char   g_app_uuid[] = {0x0, 0x0};

/* ble service att handle */
uint16_t g_service_hdl;

/* ble chara handle trans wifi cfg info */
uint16_t g_chara_cfg_hdl = 0;

/* ble chara handle trans wifi list req&resp */
uint16_t g_chara_wifi_list_hdl = 0;

/* ble indication att handle */
uint16_t g_ind_crt_att_hdl = 0;

/* ble notification att handle */
uint16_t g_ntf_crt_att_hdl = 0;

uint16_t g_descriptor_report_hdl = 0;
uint16_t g_descriptor_ctrl_hdl = 0;
uint16_t g_descriptor_cfg_hdl = 0;

/* ble connect handle */
uint16_t g_conn_handle = 0;

#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2
#define CCC_UUID 0x2902

/* Service UUID */
#define BLE_WIFI_CFG_SERVICE 0xfd5c
/* Characteristic Control Information UUID */
#define BLE_WIFI_CFG_CONTROL_INFO 0xfd5d
/* Client Characteristic Configuration Wi-Fi Information UUID */
#define BLE_WIFI_CFG_INFORMATION 0xfd5e
/* Client AP Lists Req/Resp Configuration UUID */
#define BLE_WIFI_CFG_REQ_REPORT 0xfd5f

/* Client Characteristic Configuration UUID */
#define BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION 0x2902

/* Server ID */
#define BLE_WIFI_CFG_SERVER_ID 1
/* Wifi Configure Write Characteristic */
#define BLE_WIFI_CFG_CHARACTER_VALUE_1 0x12
#define BLE_WIFI_CFG_CHARACTER_VALUE_2 0x34
#define BLE_WIFI_CFG_CHARACTER_VALUE_INDICATE_0 0X00
#define BLE_WIFI_CFG_CHARACTER_VALUE_INDICATE_2 0X02

#define sample_at_log_print(fmt, args...) test_suite_uart_sendf(fmt, ##args)

extern void set_wifi_cfg_info(uint8_t *info, uint16_t info_len);
extern int bgwc_wifi_list_resp_send(uint16_t handle);

/* 将uint16的uuid数字转化为bt_uuid_t */
void streams_data_to_uuid(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    char uuid[] = {(uint8_t)(uuid_data >> OCTET_BIT_LEN), (uint8_t)uuid_data};
    out_uuid->uuid_len = UUID_LEN_2;
    if (memcpy_s(out_uuid->uuid, out_uuid->uuid_len, uuid, UUID_LEN_2) != EOK) {
        return;
    }
}

errcode_t service_uuid_compare(bt_uuid_t *uuid1, bt_uuid_t *uuid2)
{
    if (uuid1->uuid_len != uuid2->uuid_len) {
        return ERRCODE_BT_FAIL;
    }
    if (memcmp(uuid1->uuid, uuid2->uuid, uuid1->uuid_len) != 0) {
        return ERRCODE_BT_FAIL;
    }
    return ERRCODE_BT_SUCCESS;
}

/* 开始服务回调 */
static void ble_wifi_cfg_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    sample_at_log_print("[wifi_cfg_server] start service cbk : server: %d status: %d srv_hdl: %d\n", server_id, status,
        handle);
}

static void ble_wifi_cfg_server_receive_write_req_cbk(uint8_t server_id, uint16_t conn_id,
    gatts_req_write_cb_t *write_cb_para, errcode_t status)
{
    sample_at_log_print("[wifi_cfg_server]ReceiveWriteReqCallback--server_id:%d conn_id:%d\n", server_id, conn_id);
    sample_at_log_print("request_id:%d att_handle:%d offset:%d need_rsp:%d need_authorize:%d is_prep:%d\n",
        write_cb_para->request_id, write_cb_para->handle, write_cb_para->offset, write_cb_para->need_rsp,
        write_cb_para->need_authorize, write_cb_para->is_prep);
    sample_at_log_print("data_len:%d data:\n", write_cb_para->length);
    for (uint8_t i = 0; i < write_cb_para->length; i++) {
        sample_at_log_print("%02x ", write_cb_para->value[i]);
    }
    sample_at_log_print("\n");
    sample_at_log_print("status:%d\n", status);

    if (write_cb_para->handle == g_chara_cfg_hdl) {
        set_wifi_cfg_info(write_cb_para->value, write_cb_para->length);
    }
    if (write_cb_para->handle == g_chara_wifi_list_hdl) {
        bgwc_wifi_list_resp_send(write_cb_para->handle);
    }
}

static void ble_wifi_cfg_server_receive_read_req_cbk(uint8_t server_id, uint16_t conn_id,
    gatts_req_read_cb_t *read_cb_para, errcode_t status)
{
    sample_at_log_print("[wifi_cfg_server]ReceiveReadReq--server_id:%d conn_id:%d\n", server_id, conn_id);
    sample_at_log_print("request_id:%d att_handle:%d offset:%d need_rsp:%d need_authorize:%d is_long:%d\n",
        read_cb_para->request_id, read_cb_para->handle, read_cb_para->offset, read_cb_para->need_rsp,
        read_cb_para->need_authorize, read_cb_para->is_long);
    sample_at_log_print("status:%d\n", status);
}

static void ble_wifi_cfg_server_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    sample_at_log_print("adv enable adv_id: %d, status:%d\n", adv_id, status);
}

static void ble_wifi_cfg_server_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    sample_at_log_print("adv disable adv_id: %d, status:%d\n", adv_id, status);
}

void ble_wifi_cfg_server_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    sample_at_log_print("connect state change conn_id: %d, status: %d, pair_status:%d, addr %x disc_reason %x\n",
        conn_id, conn_state, pair_state, addr[0], disc_reason);
    g_conn_handle = conn_id;
}

void ble_wifi_cfg_server_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    sample_at_log_print("mtu change change server_id: %d, conn_id: %d, mtu_size: %d, status:%d \n", server_id, conn_id,
        mtu_size, status);
}

void ble_wifi_cfg_server_enable_cbk(errcode_t status)
{
    sample_at_log_print("enable status: %d\n", status);
}

static errcode_t ble_wifi_cfg_server_register_callbacks(void)
{
    errcode_t ret;
    gap_ble_callbacks_t gap_cb = { 0 };
    gatts_callbacks_t service_cb = { 0 };

    gap_cb.start_adv_cb = ble_wifi_cfg_server_adv_enable_cbk;
    gap_cb.stop_adv_cb = ble_wifi_cfg_server_adv_disable_cbk;
    gap_cb.conn_state_change_cb = ble_wifi_cfg_server_connect_change_cbk;
    gap_cb.ble_enable_cb = ble_wifi_cfg_server_enable_cbk;
    ret = gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        sample_at_log_print("[wifi_cfg_server] reg gap cbk failed\r\n");
        return ERRCODE_BT_FAIL;
    }

    service_cb.start_service_cb = ble_wifi_cfg_server_service_start_cbk;
    service_cb.read_request_cb = ble_wifi_cfg_server_receive_read_req_cbk;
    service_cb.write_request_cb = ble_wifi_cfg_server_receive_write_req_cbk;
    service_cb.mtu_changed_cb = ble_wifi_cfg_server_mtu_changed_cbk;
    ret = gatts_register_callbacks(&service_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        sample_at_log_print("[wifi_cfg_server] reg service cbk failed\r\n");
        return ERRCODE_BT_FAIL;
    }
    return ret;
}

static void wifi_cfg_add_character_control_point(uint8_t server_id, uint16_t srvc_handle)
{
    gatts_add_chara_info_t character = { 0 };
    gatts_add_character_result_t add_character_result = { 0 };
    uint8_t character_value[] = {BLE_WIFI_CFG_CHARACTER_VALUE_1, BLE_WIFI_CFG_CHARACTER_VALUE_2};
    uint8_t ccc_value_indicate[UUID_LEN_2] = {BLE_WIFI_CFG_CHARACTER_VALUE_INDICATE_2,
                                              BLE_WIFI_CFG_CHARACTER_VALUE_INDICATE_0};
    gatts_add_desc_info_t ccc = { { UUID_LEN_2, { 0 } },
                                  GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE,
                                  UUID_LEN_2,
                                  NULL };
    streams_data_to_uuid(BLE_WIFI_CFG_CONTROL_INFO, &character.chara_uuid);
    character.properties = BLE_WIFI_CFG_PROPERTIES;
    character.permissions = 0;
    character.value_len = sizeof(character_value);
    character.value = character_value;
    gatts_add_characteristic_sync(server_id, srvc_handle, &character, &add_character_result);

    ccc.desc_uuid.uuid[0] = BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION >> 8; /* 8: octet bit num */
    ccc.desc_uuid.uuid[1] = (uint8_t)BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION;
    ccc.value = ccc_value_indicate;
    gatts_add_descriptor_sync(server_id, srvc_handle, &ccc, &g_descriptor_ctrl_hdl);
}

static void wifi_cfg_add_character_cfg_info(uint8_t server_id, uint16_t srvc_handle)
{
    gatts_add_chara_info_t character = { 0 };
    gatts_add_character_result_t add_character_result = { 0 };
    uint8_t character_value[] = {BLE_WIFI_CFG_CHARACTER_VALUE_1, BLE_WIFI_CFG_CHARACTER_VALUE_2};
    uint8_t ccc_value_indicate[UUID_LEN_2] = {BLE_WIFI_CFG_CHARACTER_VALUE_INDICATE_2,
                                              BLE_WIFI_CFG_CHARACTER_VALUE_INDICATE_0};
    gatts_add_desc_info_t ccc = { { UUID_LEN_2, { 0 } },
                                  GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE,
                                  UUID_LEN_2,
                                  NULL };

    streams_data_to_uuid(BLE_WIFI_CFG_INFORMATION, &character.chara_uuid);
    character.properties = UUID_SERVER_IND_PROPERTIES;
    character.permissions = 0;
    character.value_len = sizeof(character_value);
    character.value = character_value;
    gatts_add_characteristic_sync(server_id, srvc_handle, &character, &add_character_result);
    g_chara_cfg_hdl = add_character_result.value_handle;

    ccc.desc_uuid.uuid[0] = BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION >> 8; /* 8: octet bit num */
    ccc.desc_uuid.uuid[1] = (uint8_t)BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION;
    ccc.value = ccc_value_indicate;
    gatts_add_descriptor_sync(server_id, srvc_handle, &ccc, &g_descriptor_cfg_hdl);
}

static void wifi_cfg_add_character_req_report(uint8_t server_id, uint16_t srvc_handle)
{
    gatts_add_chara_info_t character = { 0 };
    gatts_add_character_result_t add_character_result = { 0 };
    uint8_t character_value[] = {BLE_WIFI_CFG_CHARACTER_VALUE_1, BLE_WIFI_CFG_CHARACTER_VALUE_2};
    uint8_t ccc_value_indicate[UUID_LEN_2] = {BLE_WIFI_CFG_CHARACTER_VALUE_INDICATE_2,
                                              BLE_WIFI_CFG_CHARACTER_VALUE_INDICATE_0};
    gatts_add_desc_info_t ccc = { { UUID_LEN_2, { 0 } },
                                  GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE,
                                  UUID_LEN_2,
                                  NULL };
    streams_data_to_uuid(BLE_WIFI_CFG_REQ_REPORT, &character.chara_uuid);
    character.properties = BLE_WIFI_CFG_PROPERTIES;
    character.permissions = 0;
    character.value_len = sizeof(character_value);
    character.value = character_value;
    gatts_add_characteristic_sync(server_id, srvc_handle, &character, &add_character_result);
    g_chara_wifi_list_hdl = add_character_result.value_handle;

    ccc.desc_uuid.uuid[0] = BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION >> 8; /* 8: octet bit num */
    ccc.desc_uuid.uuid[1] = (uint8_t)BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION;
    ccc.value = ccc_value_indicate;
    gatts_add_descriptor_sync(server_id, srvc_handle, &ccc, &g_descriptor_report_hdl);
}

errcode_t wifi_cfg_server_service(void)
{
    errcode_t ret;
    sample_at_log_print("[wifi_cfg_server] ble uuid add service in\r\n");

    /* Add Wifi Configure Service */
    bt_uuid_t service_uuid = { 0 };
    streams_data_to_uuid(BLE_WIFI_CFG_SERVICE, &service_uuid);
    ret = gatts_add_service_sync(BLE_WIFI_CFG_SERVER_ID, &service_uuid, true, &g_service_hdl);
    if (ret != ERRCODE_BT_SUCCESS) {
        sample_at_log_print("[wifi_cfg_server_service] reg service failed, ret=%x\r\n", ret);
        return ret;
    }

    /* Configure Trans Control Info Characteristic */
    wifi_cfg_add_character_control_point(BLE_WIFI_CFG_SERVER_ID, g_service_hdl);
    /* Configure Trans Wi-Fi Info Characteristic */
    wifi_cfg_add_character_cfg_info(BLE_WIFI_CFG_SERVER_ID, g_service_hdl);
    /* Configure Trans Wi-Fi Req/Resp info Characteristic */
    wifi_cfg_add_character_req_report(BLE_WIFI_CFG_SERVER_ID, g_service_hdl);

    gatts_start_service(BLE_WIFI_CFG_SERVER_ID, g_service_hdl);
    sample_at_log_print("[wifi_cfg_server] ble uuid add service out\r\n");
    return ERRCODE_BT_SUCCESS;
}

errcode_t ble_wifi_gatts_register_server(void)
{
    uint8_t server_handle;
    bt_uuid_t app_uuid = { 0 };
    app_uuid.uuid_len = sizeof(g_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.uuid_len, g_app_uuid, sizeof(g_app_uuid)) != EOK) {
        return ERRCODE_BT_FAIL;
    }

    sample_at_log_print("[wifi_cfg_server] gatts_register_server begin\r\n");
    return gatts_register_server(&app_uuid, &server_handle);
}

/* 初始化ble wifi cfg server service */
errcode_t ble_wifi_cfg_server_init(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    ret |= enable_ble();
    ret |= ble_wifi_cfg_server_register_callbacks();
    ret |= ble_wifi_gatts_register_server();
    ret |= wifi_cfg_server_service();
    if (ret != ERRCODE_BT_SUCCESS) {
        sample_at_log_print("[wifi_cfg_server] init fail.\r\n");
    } else {
        sample_at_log_print("[wifi_cfg_server] init ok\r\n");
    }
    return ret;
}

/* device通过uuid向host发送数据：report */
errcode_t ble_wifi_cfg_server_send_report_by_uuid(const uint8_t *data, uint32_t len)
{
    gatts_ntf_ind_by_uuid_t param = { 0 };
    uint16_t conn_id = g_conn_handle;
    param.start_handle = 0;
    param.end_handle = 0xffff;
    streams_data_to_uuid(BLE_WIFI_CFG_CONTROL_INFO, &param.chara_uuid);
    param.value_len = len;
    param.value = osal_vmalloc(len);

    if (param.value == NULL) {
        sample_at_log_print("[hid][ERROR]send report new fail\r\n");
        return ERRCODE_BT_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        sample_at_log_print("[hid][ERROR]send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_BT_FAIL;
    }
    gatts_notify_indicate_by_uuid(BLE_WIFI_CFG_SERVER_ID, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}

/* device通过handle向host发送数据：report */
errcode_t ble_wifi_cfg_server_send_report_by_handle(uint16_t attr_handle, const uint8_t *data, uint8_t len)
{
    gatts_ntf_ind_t param = { 0 };
    uint16_t conn_id = g_conn_handle;

    param.attr_handle = attr_handle;
    param.value = osal_vmalloc(len);
    param.value_len = len;

    if (param.value == NULL) {
        sample_at_log_print("[hid][ERROR]send report new fail\r\n");
        return ERRCODE_BT_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        sample_at_log_print("[hid][ERROR]send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_BT_FAIL;
    }
    gatts_notify_indicate(BLE_WIFI_CFG_SERVER_ID, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}
