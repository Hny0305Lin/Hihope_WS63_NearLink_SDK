/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: BLE uart sample of client. \n
 *
 * History: \n
 * 2023-04-03, Create file. \n
 */
#include "securec.h"
#include "product.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "uart.h"
#include "bts_le_gap.h"
#include "bts_gatt_client.h"
#include "ble_uart_client_scan.h"
#include "ble_uart_client.h"

#define UUID16_LEN 2
/* Characteristic UUID */
#define BLE_UART_CHARACTER_CLIENT_UUID_TX                  0xEFEF
#define BLE_UART_CLIENT_LOG "[ble uart client]"
#define BLE_UART_CLIENT_ERROR "[ble uart client error]"

/* client id, invalid client id is "0" */
static uint8_t g_uart_client_id = 0;
/* connection id, invalid client id is "0" */
static uint16_t g_uart_conn_id = 0;
/* max transport unit, default is 100 */
static uint16_t g_uart_mtu = 100;
/* characteristic handle */
static uint16_t g_ble_uart_chara_hanle_write_value = 0;

/* uart client app uuid for test */
static bt_uuid_t g_client_app_uuid = { UUID16_LEN, { 0 } };

/* server address for client connect */
static uint8_t g_ble_server_addr_connect[] = { 0x66, 0x55, 0x44, 0x33, 0x22, 0x11 };

static uint8_t g_ble_uart_name_value[] = { 'b', 'l', 'e', '_', 'u', 'a', 'r', 't', '\0' };
static uint8_t g_ble_uart_client_addr[] = { 0x12, 0x22, 0x33, 0x44, 0x55, 0x66 };

static void bts_data_to_uuid_len2(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    out_uuid->uuid_len = UUID16_LEN;
    out_uuid->uuid[0] = (uint8_t)(uuid_data >> 8); /* 8: octet bit num */
    out_uuid->uuid[1] = (uint8_t)(uuid_data);
}

/* ble client discover all service */
errcode_t ble_uart_client_discover_all_service(uint16_t conn_id)
{
    bt_uuid_t service_uuid = { 0 }; /* uuid length is zero, discover all service */
    return gattc_discovery_service(g_uart_client_id, conn_id, &service_uuid);
}

/* ble client write data to server */
errcode_t ble_uart_client_write_cmd(uint8_t *data, uint16_t len, uint16_t handle)
{
    gattc_handle_value_t uart_handle_value = { 0 };
    uart_handle_value.handle = handle;
    uart_handle_value.data_len = len;
    uart_handle_value.data = data;
    osal_printk("%s ble_uart_client_write_cmd len: %d, g_uart_client_id: %x\n",
                BLE_UART_CLIENT_LOG, len, g_uart_client_id);
    for (uint16_t i = 0; i < len; i++) {
        osal_printk("%02x", data[i]);
    }
    osal_printk("\n");
    errcode_t ret = gattc_write_cmd(g_uart_client_id, g_uart_conn_id, &uart_handle_value);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s gattc_write_cmd failed\n", BLE_UART_CLIENT_LOG);
        return ERRCODE_BT_FAIL;
    }
    return ERRCODE_BT_SUCCESS;
}

/* ble client set scan param callback */
void ble_uart_client_set_scan_param_cbk(errcode_t status)
{
    osal_printk("%s set scan param status: %d\n", BLE_UART_CLIENT_LOG, status);
    gap_ble_remove_all_pairs();
    ble_uart_start_scan();
}

/* ble client scan result callback */
void ble_uart_client_scan_result_cbk(gap_scan_result_data_t *scan_result_data)
{
    if (memcmp(g_ble_server_addr_connect, scan_result_data->addr.addr, sizeof(g_ble_server_addr_connect)) == 0) {
        gap_ble_stop_scan();
        osal_printk("\naddr:");
        for (uint8_t i = 0; i < BD_ADDR_LEN; i++) {
            osal_printk(" %02x: ", scan_result_data->addr.addr[i]);
        }
        bd_addr_t bt_uart_client_addr = { 0 };
        bt_uart_client_addr.type = scan_result_data->addr.type;
        if (memcpy_s(bt_uart_client_addr.addr, BD_ADDR_LEN, scan_result_data->addr.addr, BD_ADDR_LEN) != EOK) {
            osal_printk("%s add server app addr memcpy failed\r\n", BLE_UART_CLIENT_ERROR);
            return;
        }
        gap_ble_connect_remote_device(&bt_uart_client_addr);
    }
}

/* ble client connect state change callback */
void ble_uart_client_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
                                        gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    bd_addr_t bt_uart_client_addr = { 0 };
    bt_uart_client_addr.type = addr->type;
    g_uart_conn_id = conn_id;
    if (memcpy_s(bt_uart_client_addr.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN) != EOK) {
        osal_printk("%s add server app addr memcpy failed\r\n", BLE_UART_CLIENT_ERROR);
        return;
    }
    osal_printk("%s connect state change conn_id: %d, status: %d, pair_status:%d, disc_reason %x\n",
                BLE_UART_CLIENT_LOG, conn_id, conn_state, pair_state, disc_reason);

    if (conn_state == GAP_BLE_STATE_CONNECTED  &&  pair_state == GAP_BLE_PAIR_NONE) {
        osal_printk("%s connect change cbk conn_id =%d \n", BLE_UART_CLIENT_LOG, conn_id);
        gattc_exchange_mtu_req(g_uart_client_id, g_uart_conn_id, g_uart_mtu);
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        osal_printk("%s connect change cbk conn disconnected \n", BLE_UART_CLIENT_LOG);
        ble_uart_start_scan();
        return;
    }
}

/* ble client pair result callback */
void ble_uart_client_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    osal_printk("%s pair result conn_id: %d,status: %d \n", BLE_UART_CLIENT_LOG, conn_id, status);
    osal_printk("addr:\n");
    for (uint8_t i = 0; i < BD_ADDR_LEN; i++) {
        osal_printk("%2x", addr->addr[i]);
    }
    osal_printk("\n");
    gattc_exchange_mtu_req(g_uart_client_id, g_uart_conn_id, g_uart_mtu);
}


/* ble client bt stack enable callback */
void ble_uart_client_enable_cbk(errcode_t status)
{
    osal_printk("ble enable: %d\n", status);
    errcode_t ret = 0;
    bd_addr_t ble_addr = { 0 };
    ble_addr.type = BLE_PUBLIC_DEVICE_ADDRESS;
    osal_msleep(500); /* 延时5s，等待SLE初始化完毕 */
    if (memcpy_s(ble_addr.addr, BD_ADDR_LEN, g_ble_uart_client_addr, sizeof(g_ble_uart_client_addr)) != EOK) {
        osal_printk("%s add server app addr memcpy failed\n", BLE_UART_CLIENT_ERROR);
        return;
    }
    ret = gap_ble_set_local_name(g_ble_uart_name_value, sizeof(g_ble_uart_name_value));
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s gap_ble_set_local_name ret = %x\n", BLE_UART_CLIENT_ERROR, ret);
    }
    ret = gap_ble_set_local_addr(&ble_addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s gap_ble_set_local_addr ret = %x\n", BLE_UART_CLIENT_ERROR, ret);
    }
    ret = gattc_register_client(&g_client_app_uuid, &g_uart_client_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s gattc_register_client ret = %x\n", BLE_UART_CLIENT_ERROR, ret);
    }

    ble_uart_set_scan_parameters();
     osal_printk("ble enable end: %d\n", status);
}

/* ble client service discovery callback */
static void ble_uart_client_discover_service_cbk(uint8_t client_id, uint16_t conn_id,
                                                 gattc_discovery_service_result_t *service, errcode_t status)
{
    gattc_discovery_character_param_t param = { 0 };
    osal_printk("%s Discovery service callback client:%d conn_id:%d\n", BLE_UART_CLIENT_LOG, client_id, conn_id);
    osal_printk("%s start handle:%d end handle:%d uuid_len:%d uuid:\n",
                BLE_UART_CLIENT_LOG, service->start_hdl, service->end_hdl, service->uuid.uuid_len);
    for (uint8_t i = 0; i < service->uuid.uuid_len; i++) {
        osal_printk("%02x", service->uuid.uuid[i]);
    }
    osal_printk("\n %s status:%d\n", BLE_UART_CLIENT_LOG, status);
    param.service_handle = service->start_hdl;
    param.uuid.uuid_len = service->uuid.uuid_len; /* uuid length is zero, discover all character */
    if (memcpy_s(param.uuid.uuid, param.uuid.uuid_len, service->uuid.uuid, service->uuid.uuid_len) != 0) {
        osal_printk("%s memcpy error\n", BLE_UART_CLIENT_ERROR);
    }
    gattc_discovery_character(g_uart_client_id, conn_id, &param);
}

/* ble client character discovery callback */
static void ble_uart_client_discover_character_cbk(uint8_t client_id, uint16_t conn_id,
                                                   gattc_discovery_character_result_t *character, errcode_t status)
{
    for (uint8_t i = 0; i < character->uuid.uuid_len; i++) {
        osal_printk("%02x", character->uuid.uuid[i]);
    }
    osal_printk("\n%s discover character declare_handle:%d, value_handle:%d, properties:%2x\n",
                BLE_UART_CLIENT_LOG, character->declare_handle, character->value_handle, character->properties);
    osal_printk("%s client_id:%d, conn_id = %d, status:%d\n", BLE_UART_CLIENT_LOG, client_id, conn_id, status);
    bt_uuid_t write_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UART_CHARACTER_CLIENT_UUID_TX, &write_uuid);
    write_uuid.uuid_len = BT_UUID_MAX_LEN;
    if (memcmp(character->uuid.uuid, write_uuid.uuid, character->uuid.uuid_len) == 0) {
        g_ble_uart_chara_hanle_write_value = character->value_handle;
        osal_printk("%s write declare_handle:%d, value_handle:%d, properties:%2x\n",
                    BLE_UART_CLIENT_LOG, character->declare_handle, character->value_handle, character->properties);
    }
    gattc_discovery_descriptor(g_uart_client_id, conn_id, character->declare_handle);
}

/* ble client descriptor discovery callback */
static void ble_uart_client_discover_descriptor_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_descriptor_result_t *descriptor, errcode_t status)
{
    osal_printk("%s Discovery descriptor----client:%d conn_id:%d uuid len:%d, uuid:\n",
                BLE_UART_CLIENT_LOG, client_id, conn_id, descriptor->uuid.uuid_len);
    for (uint8_t i = 0; i < descriptor->uuid.uuid_len; i++) {
        osal_printk("%02x", descriptor->uuid.uuid[i]);
    }
    osal_printk("\n%s descriptor handle:%d, status:%d\n", BLE_UART_CLIENT_LOG, descriptor->descriptor_hdl, status);

    gattc_read_req_by_uuid_param_t paramsss = { 0 };
    paramsss.uuid = descriptor->uuid;
    paramsss.start_hdl = descriptor->descriptor_hdl;
    paramsss.end_hdl = descriptor->descriptor_hdl;
    gattc_read_req_by_uuid(client_id, conn_id, &paramsss);
}

/* ble client compare service uuid */
static void ble_uart_client_discover_service_compl_cbk(uint8_t client_id, uint16_t conn_id, bt_uuid_t *uuid,
                                                       errcode_t status)
{
    osal_printk("%s Discovery service complete----client:%d conn_id:%d uuid len:%d uuid:\n",
                BLE_UART_CLIENT_LOG, client_id, conn_id, uuid->uuid_len);
    for (uint8_t i = 0; i < uuid->uuid_len; i++) {
        osal_printk("%02x", uuid->uuid[i]);
    }
    osal_printk("\n%s status:%d\n", BLE_UART_CLIENT_LOG, status);
}

/* ble client character discovery complete callback */
static void ble_uart_client_discover_character_compl_cbk(uint8_t client_id, uint16_t conn_id,
                                                         gattc_discovery_character_param_t *param, errcode_t status)
{
    osal_printk("%s Discovery character complete----client:%d conn_id:%d uuid len:%d uuid: \n",
                BLE_UART_CLIENT_LOG, client_id, conn_id, param->uuid.uuid_len);
    for (uint8_t i = 0; i < param->uuid.uuid_len; i++) {
        osal_printk("%02x", param->uuid.uuid[i]);
    }
    osal_printk("\n%s service handle:%d status:%d\n", BLE_UART_CLIENT_LOG, param->service_handle, status);
}

/* ble client descriptor discovery complete callback */
static void ble_uart_client_discover_descriptor_compl_cbk(uint8_t client_id, uint16_t conn_id,
                                                          uint16_t character_handle, errcode_t status)
{
    osal_printk("%s Discovery descriptor complete----client:%d conn_id:%d\n", BLE_UART_CLIENT_LOG, client_id, conn_id);
    osal_printk("%s charatcer handle:%d, status:%d\n", BLE_UART_CLIENT_LOG, character_handle, status);
}

/* Callback invoked when receive read response */
static void ble_uart_client_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *read_result,
                                         gatt_status_t status)
{
    osal_printk("%s Read result client:%d conn_id:%d\n", BLE_UART_CLIENT_LOG, client_id, conn_id);
    osal_printk("%s handle:%d data_len:%d\ndata:", BLE_UART_CLIENT_LOG, read_result->handle, read_result->data_len);
    for (uint8_t i = 0; i < read_result->data_len; i++) {
        osal_printk("%02x", read_result->data[i]);
    }
    osal_printk("\n%s status:%d\n", BLE_UART_CLIENT_LOG, status);
}

/* Callback invoked when read complete */
static void ble_uart_client_read_compl_cbk(uint8_t client_id, uint16_t conn_id, gattc_read_req_by_uuid_param_t *param,
                                           errcode_t status)
{
    osal_printk("%s Read by uuid complete----client:%d conn_id:%d\n", BLE_UART_CLIENT_LOG, client_id, conn_id);
    osal_printk("%s start handle:%d end handle:%d uuid len:%d uuid:\n",
                BLE_UART_CLIENT_LOG, param->start_hdl, param->end_hdl, param->uuid.uuid_len);
    for (uint8_t i = 0; i < param->uuid.uuid_len; i++) {
        osal_printk("%02x", param->uuid.uuid[i]);
    }
    osal_printk("\n%s status:%d\n", BLE_UART_CLIENT_LOG, status);
}

/* Callback invoked when receive write response */
static void ble_uart_client_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    osal_printk("%s Write result----client:%d conn_id:%d handle:%d\n", BLE_UART_CLIENT_LOG, client_id, conn_id, handle);
    osal_printk("%s status:%d\n", BLE_UART_CLIENT_LOG, status);
}

/* Callback invoked when change MTU complete */
static void ble_uart_client_mtu_changed_cbk(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    osal_printk("%s Mtu changed----client:%d conn_id:%d, mtu size:%d, status:%d\n",
                BLE_UART_CLIENT_LOG, client_id, conn_id, mtu_size, status);
    ble_uart_client_discover_all_service(conn_id);
}

/* Callback invoked when receive server notification */
static void ble_uart_client_notification_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
                                             errcode_t status)
{
    osal_printk("%s Receive notification----client:%d conn_id:%d\n", BLE_UART_CLIENT_LOG, client_id, conn_id);
    osal_printk("%s handle:%d data_len:%d\ndata:", BLE_UART_CLIENT_LOG, data->handle, data->data_len);
    osal_printk("%s ble_uart_client_notification_cbk %s", BLE_UART_CLIENT_LOG, data->data);
    osal_printk("\n%s status:%d\n", BLE_UART_CLIENT_LOG, status);
    uapi_uart_write(CONFIG_BLE_UART_BUS, (uint8_t *)(data->data), data->data_len, 0);
}

/* Callback invoked when receive server indication */
static void ble_uart_client_indication_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
                                           errcode_t status)
{
    osal_printk("%s Receive indication----client:%d conn_id:%d\n", BLE_UART_CLIENT_LOG, client_id, conn_id);
    osal_printk("%s handle:%d data_len:%d\ndata:", BLE_UART_CLIENT_LOG, data->handle, data->data_len);
    for (uint8_t i = 0; i < data->data_len; i++) {
        osal_printk("%02x", data->data[i]);
    }
    osal_printk("\n%s status:%d\n", BLE_UART_CLIENT_LOG, status);
}

/* register gatt and gap callback */
errcode_t ble_uart_client_callback_register(void)
{
    errcode_t ret = 0;

    gattc_callbacks_t cb = { 0 };
    gap_ble_callbacks_t gap_cb = { 0 };
    gap_cb.ble_enable_cb = ble_uart_client_enable_cbk;
    gap_cb.set_scan_param_cb = ble_uart_client_set_scan_param_cbk;
    gap_cb.scan_result_cb = ble_uart_client_scan_result_cbk;
    gap_cb.conn_state_change_cb = ble_uart_client_connect_change_cbk;
    gap_cb.pair_result_cb = ble_uart_client_pair_result_cb;
    ret |= gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s reg gap cbk failed ret = %d\n", BLE_UART_CLIENT_ERROR, ret);
    }

    cb.discovery_svc_cb = ble_uart_client_discover_service_cbk;
    cb.discovery_svc_cmp_cb = ble_uart_client_discover_service_compl_cbk;
    cb.discovery_chara_cb = ble_uart_client_discover_character_cbk;
    cb.discovery_chara_cmp_cb = ble_uart_client_discover_character_compl_cbk;
    cb.discovery_desc_cb = ble_uart_client_discover_descriptor_cbk;
    cb.discovery_desc_cmp_cb = ble_uart_client_discover_descriptor_compl_cbk;
    cb.read_cb = ble_uart_client_read_cfm_cbk;
    cb.read_cmp_cb = ble_uart_client_read_compl_cbk;
    cb.write_cb = ble_uart_client_write_cfm_cbk;
    cb.mtu_changed_cb = ble_uart_client_mtu_changed_cbk;
    cb.notification_cb = ble_uart_client_notification_cbk;
    cb.indication_cb = ble_uart_client_indication_cbk;
    ret = gattc_register_callbacks(&cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s reg gatt cbk failed ret = %d\n", BLE_UART_CLIENT_ERROR, ret);
    }
#if (CORE_NUMS < 2)
    enable_ble();
#endif
    return ret;
}

/* ble uart client init */
errcode_t ble_uart_client_init(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;

    ret |= ble_uart_client_callback_register();
    osal_printk("[SLE Client] try enable1.\r\n");

    ret |= enable_ble();
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s enable_ble ret = %x\n", BLE_UART_CLIENT_ERROR, ret);
    }
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s init failed ret = %x\n", BLE_UART_CLIENT_ERROR, ret);
    }
    return ret;
}

uint16_t ble_uart_get_write_vlaue_handle(void)
{
    return g_ble_uart_chara_hanle_write_value;
}