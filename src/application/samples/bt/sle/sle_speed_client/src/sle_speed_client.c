/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: SLE private service register sample of client.
 */
#include "app_init.h"
#include "systick.h"
#include "tcxo.h"
#include "los_memory.h"
#include "securec.h"
#include "test_suite_uart.h"
#include "soc_osal.h"

#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"

#include "sle_speed_client.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID BTH_GLE_SAMPLE_UUID_CLIENT

#define SLE_MTU_SIZE_DEFAULT        512
#define SLE_SEEK_INTERVAL_DEFAULT   100
#define SLE_SEEK_WINDOW_DEFAULT     100
#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16
#define RECV_PKT_CNT 1000
#define SLE_SPEED_HUNDRED   100        /* 100  */
#define SPEED_DEFAULT_CONN_INTERVAL 0x09
#define SPEED_DEFAULT_TIMEOUT_MULTIPLIER 0x1f4
#define SPEED_DEFAULT_SCAN_INTERVAL 400
#define SPEED_DEFAULT_SCAN_WINDOW 20

static int g_recv_pkt_num = 0;
static uint64_t g_count_before_get_us;
static uint64_t g_count_after_get_us;

static sle_announce_seek_callbacks_t g_seek_cbk = {0};
static sle_connection_callbacks_t    g_connect_cbk = {0};
static ssapc_callbacks_t             g_ssapc_cbk = {0};
static sle_addr_t                    g_remote_addr = {0};
static uint16_t                      g_conn_id = 0;
static ssapc_find_service_result_t   g_find_service_result = {0};

void sle_sample_sle_enable_cbk(errcode_t status)
{
    if (status == 0) {
        sle_start_scan();
    }
}

void sle_sample_seek_enable_cbk(errcode_t status)
{
    if (status == 0) {
        return;
    }
}

void sle_sample_seek_disable_cbk(errcode_t status)
{
    if (status == 0) {
        sle_connect_remote_device(&g_remote_addr);
    }
}

void sle_sample_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data != NULL) {
        uint8_t mac[SLE_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
        if (memcmp(seek_result_data->addr.addr, mac, SLE_ADDR_LEN) == 0) {
            (void)memcpy_s(&g_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
            sle_stop_seek();
        }
    }
}

static uint32_t GetFloatInt(float in)
{
    return (uint32_t)(((uint64_t)(in * SLE_SPEED_HUNDRED)) / SLE_SPEED_HUNDRED);
}

static uint32_t GetFloatDec(float in)
{
    return (uint32_t)(((uint64_t)(in * SLE_SPEED_HUNDRED)) % SLE_SPEED_HUNDRED);
}

static void sle_uart_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
                                     errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);

    g_recv_pkt_num++;
    if (g_recv_pkt_num == 1) {
        g_count_before_get_us = uapi_tcxo_get_us();
    } else if (g_recv_pkt_num == RECV_PKT_CNT) {
        g_count_after_get_us = uapi_tcxo_get_us();
        printf("g_count_after_get_us = %llu, g_count_before_get_us = %llu, data_len = %d\r\n",
            g_count_after_get_us, g_count_before_get_us, data->data_len);
        float time = (float)(g_count_after_get_us - g_count_before_get_us) / 1000000.0;  /* 1s = 1000000.0us */
        printf("time = %d.%d s\r\n", GetFloatInt(time), GetFloatDec(time));
        uint16_t len = data->data_len;
        float speed = len * RECV_PKT_CNT * 8 / time;  /* 1B = 8bits */
        printf("speed = %d.%d bps\r\n", GetFloatInt(speed), GetFloatDec(speed));
        g_recv_pkt_num = 0;
    }
}

static void sle_uart_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
                                   errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\n sle_uart_indication_cb sle uart recived data : %s\r\n", data->data);
}

void sle_sample_seek_cbk_register(void)
{
    g_seek_cbk.sle_enable_cb = sle_sample_sle_enable_cbk;
    g_seek_cbk.seek_enable_cb = sle_sample_seek_enable_cbk;
    g_seek_cbk.seek_disable_cb = sle_sample_seek_disable_cbk;
    g_seek_cbk.seek_result_cb = sle_sample_seek_result_info_cbk;
}

void sle_sample_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    test_suite_uart_sendf("[ssap client] conn state changed conn_id:%d, addr:%02x***%02x%02x\n", conn_id, addr->addr[0],
        addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    test_suite_uart_sendf("[ssap client] conn state changed disc_reason:0x%x\n", disc_reason);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        if (pair_state == SLE_PAIR_NONE) {
            sle_pair_remote_device(&g_remote_addr);
        }
        g_conn_id = conn_id;
    }
}

void sle_sample_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    test_suite_uart_sendf("[ssap client] pair complete conn_id:%d, addr:%02x***%02x%02x\n", conn_id, addr->addr[0],
        addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    if (status == 0) {
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(1, g_conn_id, &info);
    }
}

void sle_sample_update_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    unused(status);
    test_suite_uart_sendf("[ssap client] updat state changed conn_id:%d, interval = %02x\n", conn_id, param->interval);
}

void sle_sample_update_req_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    test_suite_uart_sendf("[ssap client] sle_sample_update_req_cbk interval_min = %02x, interval_max = %02x\n",
        param->interval_min, param->interval_max);
}

void sle_sample_connect_cbk_register(void)
{
    g_connect_cbk.connect_state_changed_cb = sle_sample_connect_state_changed_cbk;
    g_connect_cbk.pair_complete_cb = sle_sample_pair_complete_cbk;
    g_connect_cbk.connect_param_update_req_cb = sle_sample_update_req_cbk;
    g_connect_cbk.connect_param_update_cb = sle_sample_update_cbk;
}

void sle_sample_exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
    errcode_t status)
{
    test_suite_uart_sendf("[ssap client] pair complete client id:%d status:%d\n", client_id, status);
    test_suite_uart_sendf("[ssap client] exchange mtu, mtu size: %d, version: %d.\n",
        param->mtu_size, param->version);

    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PRIMARY_SERVICE;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}

void sle_sample_find_structure_cbk(uint8_t client_id, uint16_t conn_id, ssapc_find_service_result_t *service,
    errcode_t status)
{
    test_suite_uart_sendf("[ssap client] find structure cbk client: %d conn_id:%d status: %d \n",
        client_id, conn_id, status);
    test_suite_uart_sendf("[ssap client] find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n",
        service->start_hdl, service->end_hdl, service->uuid.len);
    if (service->uuid.len == UUID_16BIT_LEN) {
        test_suite_uart_sendf("[ssap client] structure uuid:[0x%02x][0x%02x]\r\n",
            service->uuid.uuid[14], service->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            test_suite_uart_sendf("[ssap client] structure uuid[%d]:[0x%02x]\r\n", idx, service->uuid.uuid[idx]);
        }
    }
    g_find_service_result.start_hdl = service->start_hdl;
    g_find_service_result.end_hdl = service->end_hdl;
    memcpy_s(&g_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
}

void sle_sample_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *structure_result, errcode_t status)
{
    test_suite_uart_sendf("[ssap client] find structure cmp cbk client id:%d status:%d type:%d uuid len:%d \r\n",
        client_id, status, structure_result->type, structure_result->uuid.len);
    if (structure_result->uuid.len == UUID_16BIT_LEN) {
        test_suite_uart_sendf("[ssap client] find structure cmp cbk structure uuid:[0x%02x][0x%02x]\r\n",
            structure_result->uuid.uuid[14], structure_result->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            test_suite_uart_sendf("[ssap client] find structure cmp cbk structure uuid[%d]:[0x%02x]\r\n", idx,
                structure_result->uuid.uuid[idx]);
        }
    }
    uint8_t data[] = {0x11, 0x22, 0x33, 0x44};
    uint8_t len = sizeof(data);
    ssapc_write_param_t param = {0};
    param.handle = g_find_service_result.start_hdl;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data_len = len;
    param.data = data;
    ssapc_write_req(0, conn_id, &param);
}

void sle_sample_find_property_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status)
{
    test_suite_uart_sendf("[ssap client] find property cbk, client id: %d, conn id: %d, operate ind: %d, "
        "descriptors count: %d status:%d.\n", client_id, conn_id, property->operate_indication,
        property->descriptors_count, status);
    for (uint16_t idx = 0; idx < property->descriptors_count; idx++) {
        test_suite_uart_sendf("[ssap client] find property cbk, descriptors type [%d]: 0x%02x.\n",
            idx, property->descriptors_type[idx]);
    }
    if (property->uuid.len == UUID_16BIT_LEN) {
        test_suite_uart_sendf("[ssap client] find property cbk, uuid: %02x %02x.\n",
            property->uuid.uuid[14], property->uuid.uuid[15]); /* 14 15: uuid index */
    } else if (property->uuid.len == UUID_128BIT_LEN) {
        for (uint16_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            test_suite_uart_sendf("[ssap client] find property cbk, uuid [%d]: %02x.\n",
                idx, property->uuid.uuid[idx]);
        }
    }
}

void sle_sample_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_write_result_t *write_result,
    errcode_t status)
{
    test_suite_uart_sendf("[ssap client] write cfm cbk, client id: %d status:%d.\n", client_id, status);
    ssapc_read_req(0, conn_id, write_result->handle, write_result->type);
}

void sle_sample_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *read_data,
    errcode_t status)
{
    test_suite_uart_sendf("[ssap client] read cfm cbk client id: %d conn id: %d status: %d\n",
        client_id, conn_id, status);
    test_suite_uart_sendf("[ssap client] read cfm cbk handle: %d, type: %d , len: %d\n",
        read_data->handle, read_data->type, read_data->data_len);
    for (uint16_t idx = 0; idx < read_data->data_len; idx++) {
        test_suite_uart_sendf("[ssap client] read cfm cbk[%d] 0x%02x\r\n", idx, read_data->data[idx]);
    }
}

void sle_sample_ssapc_cbk_register(ssapc_notification_callback notification_cb,
    ssapc_notification_callback indication_cb)
{
    g_ssapc_cbk.exchange_info_cb = sle_sample_exchange_info_cbk;
    g_ssapc_cbk.find_structure_cb = sle_sample_find_structure_cbk;
    g_ssapc_cbk.find_structure_cmp_cb = sle_sample_find_structure_cmp_cbk;
    g_ssapc_cbk.ssapc_find_property_cbk = sle_sample_find_property_cbk;
    g_ssapc_cbk.write_cfm_cb = sle_sample_write_cfm_cbk;
    g_ssapc_cbk.read_cfm_cb = sle_sample_read_cfm_cbk;
    g_ssapc_cbk.notification_cb = notification_cb;
    g_ssapc_cbk.indication_cb = indication_cb;
}

void sle_speed_connect_param_init(void)
{
    sle_default_connect_param_t param = {0};
    param.enable_filter_policy = 0;
    param.gt_negotiate = 0;
    param.initiate_phys = 1;
    param.max_interval = SPEED_DEFAULT_CONN_INTERVAL;
    param.min_interval = SPEED_DEFAULT_CONN_INTERVAL;
    param.scan_interval = SPEED_DEFAULT_SCAN_INTERVAL;
    param.scan_window = SPEED_DEFAULT_SCAN_WINDOW;
    param.timeout = SPEED_DEFAULT_TIMEOUT_MULTIPLIER;
    sle_default_connection_param_set(&param);
}

void sle_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb)
{
    uint8_t local_addr[SLE_ADDR_LEN] = {0x13, 0x67, 0x5c, 0x07, 0x00, 0x51};
    sle_addr_t local_address;
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    sle_sample_seek_cbk_register();
    sle_speed_connect_param_init();
    sle_sample_connect_cbk_register();
    sle_sample_ssapc_cbk_register(notification_cb, indication_cb);
    sle_announce_seek_register_callbacks(&g_seek_cbk);
    sle_connection_register_callbacks(&g_connect_cbk);
    ssapc_register_callbacks(&g_ssapc_cbk);
    enable_sle();
    sle_set_local_addr(&local_address);
}

void sle_start_scan()
{
    sle_seek_param_t param = {0};
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 0;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
}

int sle_speed_init(void)
{
    osal_msleep(1000);  /* sleep 1000ms */
    sle_client_init(sle_uart_notification_cb, sle_uart_indication_cb);
    return 0;
}

#define SLE_SPEED_TASK_PRIO 26
#define SLE_SPEED_STACK_SIZE 0x2000
static void sle_speed_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_speed_init, 0, "RadarTask", SLE_SPEED_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_SPEED_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the blinky_entry. */
app_run(sle_speed_entry);