/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: ble uuid server sample.
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
#include "soc_osal.h"
#include "app_init.h"
#include "ble_server_adv.h"
#include "ble_gatt_server.h"

/* server app uuid for test */
char g_uuid_app_uuid[] = {0x0, 0x0};

/* ble indication att handle */
uint16_t g_indication_characteristic_att_hdl = 0;

/* ble notification att handle */
uint16_t g_notification_characteristic_att_hdl = 0;

/* ble connect handle */
uint16_t g_conn_hdl = 0;

/* ble server id */
uint8_t g_server_id = 0;

#define OCTET_BIT_LEN 8
#define UUID_LEN_2 2

#define sample_at_log_print(fmt, args...) test_suite_uart_sendf(fmt, ##args)

/* 将uint16的uuid数字转化为bt_uuid_t */
void stream_data_to_uuid(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    char uuid[] = {(uint8_t)(uuid_data >> OCTET_BIT_LEN), (uint8_t)uuid_data};
    out_uuid->uuid_len = UUID_LEN_2;
    if (memcpy_s(out_uuid->uuid, out_uuid->uuid_len, uuid, UUID_LEN_2) != EOK) {
        return;
    }
}

errcode_t compare_service_uuid(bt_uuid_t *uuid1, bt_uuid_t *uuid2)
{
    if (uuid1->uuid_len != uuid2->uuid_len) {
        return ERRCODE_BT_FAIL;
    }
    if (memcmp(uuid1->uuid, uuid2->uuid, uuid1->uuid_len) != 0) {
        return ERRCODE_BT_FAIL;
    }
    return ERRCODE_BT_SUCCESS;
}

/* 添加描述符：客户端特性配置 */
static void ble_uuid_server_add_descriptor_ccc(uint32_t server_id, uint32_t srvc_handle)
{
    bt_uuid_t ccc_uuid = {0};
    uint8_t ccc_data_val[] = {0x00, 0x00};

    sample_at_log_print("[uuid server] beginning add descriptors\r\n");
    stream_data_to_uuid(BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION, &ccc_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = ccc_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    descriptor.value_len = sizeof(ccc_data_val);
    descriptor.value = ccc_data_val;
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
    osal_vfree(ccc_uuid.uuid);
}

/* 添加HID服务的所有特征和描述符 */
static void ble_uuid_server_add_characters_and_descriptors(uint32_t server_id, uint32_t srvc_handle)
{
    bt_uuid_t server_uuid = {0};
    uint8_t server_value[] = {0x12, 0x34};
    sample_at_log_print("[uuid server] beginning add characteristic\r\n");
    stream_data_to_uuid(BLE_UUID_UUID_SERVER_REPORT, &server_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = server_uuid;
    character.properties = UUID_SERVER_PROPERTIES;
    character.permissions = 0;
    character.value_len = sizeof(server_value);
    character.value = server_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    ble_uuid_server_add_descriptor_ccc(server_id, srvc_handle);
}

/* 服务添加回调 */
static void ble_uuid_server_service_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    bt_uuid_t service_uuid = {0};
    sample_at_log_print("[uuid server] add service cbk: server: %d, status: %d, srv_handle: %d, uuid_len: %d,uuid:",
        server_id, status, handle, uuid->uuid_len);
    for (int8_t i = 0; i < uuid->uuid_len ; i++) {
        sample_at_log_print("%02x", (uint8_t)uuid->uuid[i]);
    }
    sample_at_log_print("\n");
    stream_data_to_uuid(BLE_UUID_UUID_SERVER_SERVICE, &service_uuid);
    if (compare_service_uuid(uuid, &service_uuid) == ERRCODE_BT_SUCCESS) {
        ble_uuid_server_add_characters_and_descriptors(server_id, handle);
        sample_at_log_print("[uuid server] start service\r\n");
        gatts_start_service(server_id, handle);
    } else {
        sample_at_log_print("[uuid server] unknown service uuid\r\n");
        return;
    }
}

/* 特征添加回调 */
static void  ble_uuid_server_characteristic_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status)
{
    int8_t i = 0;
    sample_at_log_print("[uuid server] add characteristic cbk: server: %d, status: %d, srv_hdl: %d "\
        "char_hdl: %x, char_val_hdl: %x, uuid_len: %d, uuid: ",
        server_id, status, service_handle, result->handle, result->value_handle, uuid->uuid_len);
    for (i = 0; i < uuid->uuid_len ; i++) {
        sample_at_log_print("%02x", (uint8_t)uuid->uuid[i]);
    }
    sample_at_log_print("\n");
    g_notification_characteristic_att_hdl = result->value_handle;
}

/* 描述符添加回调 */
static void  ble_uuid_server_descriptor_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    int8_t i = 0;
    sample_at_log_print("[uuid server] add descriptor cbk : server: %d, status: %d, srv_hdl: %d, desc_hdl: %x ,"\
        "uuid_len:%d, uuid: ", server_id, status, service_handle, handle, uuid->uuid_len);
    for (i = 0; i < uuid->uuid_len ; i++) {
        sample_at_log_print("%02x", (uint8_t)uuid->uuid[i]);
    }
    sample_at_log_print("\n");
}

/* 开始服务回调 */
static void ble_uuid_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    sample_at_log_print("[uuid server] start service cbk : server: %d status: %d srv_hdl: %d\n",
        server_id, status, handle);
}

static void ble_uuid_server_receive_write_req_cbk(uint8_t server_id, uint16_t conn_id,
    gatts_req_write_cb_t *write_cb_para, errcode_t status)
{
    sample_at_log_print("[uuid server]ReceiveWriteReqCallback--server_id:%d conn_id:%d\n", server_id, conn_id);
    sample_at_log_print("request_id:%d att_handle:%d offset:%d need_rsp:%d need_authorize:%d is_prep:%d\n",
        write_cb_para->request_id, write_cb_para->handle, write_cb_para->offset, write_cb_para->need_rsp,
        write_cb_para->need_authorize, write_cb_para->is_prep);
    sample_at_log_print("data_len:%d data:\n", write_cb_para->length);
    for (uint8_t i = 0; i < write_cb_para->length; i++) {
        sample_at_log_print("%02x ", write_cb_para->value[i]);
    }
    sample_at_log_print("\n");
    sample_at_log_print("status:%d\n", status);
}

static void ble_uuid_server_receive_read_req_cbk(uint8_t server_id, uint16_t conn_id,
    gatts_req_read_cb_t *read_cb_para, errcode_t status)
{
    sample_at_log_print("[uuid server]ReceiveReadReq--server_id:%d conn_id:%d\n", server_id, conn_id);
    sample_at_log_print("request_id:%d att_handle:%d offset:%d need_rsp:%d need_authorize:%d is_long:%d\n",
        read_cb_para->request_id, read_cb_para->handle, read_cb_para->offset, read_cb_para->need_rsp,
        read_cb_para->need_authorize, read_cb_para->is_long);
    sample_at_log_print("status:%d\n", status);
}

static void ble_uuid_server_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    sample_at_log_print("adv enable adv_id: %d, status:%d\n", adv_id, status);
}

static void ble_uuid_server_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    sample_at_log_print("adv disable adv_id: %d, status:%d\n",
        adv_id, status);
}


void ble_uuid_server_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    sample_at_log_print("connect state change conn_id: %d, status: %d, pair_status:%d, addr %x disc_reason %x\n",
        conn_id, conn_state, pair_state, addr[0], disc_reason);
    g_conn_hdl = conn_id;
}

void ble_uuid_server_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    sample_at_log_print("mtu change change server_id: %d, conn_id: %d, mtu_size: %d, status:%d \n",
        server_id, conn_id, mtu_size, status);
}

void ble_uuid_server_enable_cbk(errcode_t status)
{
    sample_at_log_print("enable status: %d\n", status);
}

static errcode_t ble_uuid_server_register_callbacks(void)
{
    errcode_t ret;
    gap_ble_callbacks_t gap_cb = {0};
    gatts_callbacks_t service_cb = {0};

    gap_cb.start_adv_cb = ble_uuid_server_adv_enable_cbk;
    gap_cb.stop_adv_cb = ble_uuid_server_adv_disable_cbk;
    gap_cb.conn_state_change_cb = ble_uuid_server_connect_change_cbk;
    gap_cb.ble_enable_cb = ble_uuid_server_enable_cbk;
    ret = gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        sample_at_log_print("[uuid server] reg gap cbk failed\r\n");
        return ERRCODE_BT_FAIL;
    }

    service_cb.add_service_cb = ble_uuid_server_service_add_cbk;
    service_cb.add_characteristic_cb = ble_uuid_server_characteristic_add_cbk;
    service_cb.add_descriptor_cb = ble_uuid_server_descriptor_add_cbk;
    service_cb.start_service_cb = ble_uuid_server_service_start_cbk;
    service_cb.read_request_cb = ble_uuid_server_receive_read_req_cbk;
    service_cb.write_request_cb = ble_uuid_server_receive_write_req_cbk;
    service_cb.mtu_changed_cb = ble_uuid_server_mtu_changed_cbk;
    ret = gatts_register_callbacks(&service_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        sample_at_log_print("[uuid server] reg service cbk failed\r\n");
        return ERRCODE_BT_FAIL;
    }
    return ret;
}

uint8_t ble_uuid_add_service(void)
{
    sample_at_log_print("[uuid server] ble uuid add service in\r\n");
    bt_uuid_t service_uuid = {0};
    stream_data_to_uuid(BLE_UUID_UUID_SERVER_SERVICE, &service_uuid);
    gatts_add_service(g_server_id, &service_uuid, true);
    sample_at_log_print("[uuid server] ble uuid add service out\r\n");
    return ERRCODE_BT_SUCCESS;
}

/* 初始化uuid server service */
errcode_t ble_uuid_server_init(void)
{
    enable_ble();
    ble_uuid_server_register_callbacks();
    bt_uuid_t app_uuid = {0};
    app_uuid.uuid_len = sizeof(g_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.uuid_len, g_uuid_app_uuid, sizeof(g_uuid_app_uuid)) != EOK) {
        return ERRCODE_BT_FAIL;
    }
    gatts_register_server(&app_uuid, &g_server_id);
    ble_uuid_add_service();
    sample_at_log_print("[uuid server] init ok\r\n");
    ble_start_adv();
    return ERRCODE_BT_SUCCESS;
}

/* device通过uuid向host发送数据：report */
errcode_t ble_uuid_server_send_report_by_uuid(const uint8_t *data, uint8_t len)
{
    gatts_ntf_ind_by_uuid_t param = {0};
    uint16_t conn_id = g_conn_hdl;
    param.start_handle = 0;
    param.end_handle = 0xffff;
    stream_data_to_uuid(BLE_UUID_UUID_SERVER_REPORT, &param.chara_uuid);
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
    gatts_notify_indicate_by_uuid(g_server_id, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}

/* device通过handle向host发送数据：report */
errcode_t ble_uuid_server_send_report_by_handle(uint16_t attr_handle, const uint8_t *data, uint8_t len)
{
    gatts_ntf_ind_t param = {0};
    uint16_t conn_id = g_conn_hdl;

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
    gatts_notify_indicate(g_server_id, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}

#define BLE_UUID_SERVER_TASK_PRIO 24
#define BLE_UUID_SERVER_STACK_SIZE 0x2000

static void ble_uuid_server_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle= osal_kthread_create((osal_kthread_handler)ble_uuid_server_init, 0, "ble_uuid_server",
        BLE_UUID_SERVER_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_UUID_SERVER_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the app entry. */
app_run(ble_uuid_server_entry);