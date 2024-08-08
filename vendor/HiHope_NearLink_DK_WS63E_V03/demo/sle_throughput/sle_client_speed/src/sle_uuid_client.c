/**
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 *
 * Description: SLE private service register sample of client.
 */
#include "securec.h"
#include "test_suite_uart.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "soc_osal.h"
#include "sle_ssap_client.h"
#include "../inc/sle_uuid_client.h"
#include "app_init.h"
#include "systick.h"
#include "tcxo.h"
#include "los_memory.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID BTH_GLE_SAMPLE_UUID_CLIENT

#define SLE_MTU_SIZE_DEFAULT        250
#define SLE_SEEK_INTERVAL_DEFAULT   100
#define SLE_SEEK_WINDOW_DEFAULT     100
#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16

int g_rx_buffer_size = 0;
char* g_rev_start_ret = NULL;
char* g_rev_stop_ret = NULL;
uint64_t g_start_time = 0;
uint64_t g_stop_time = 0;

sle_announce_seek_callbacks_t g_seek_cbk = {0};
sle_connection_callbacks_t    g_connect_cbk = {0};
ssapc_callbacks_t             g_ssapc_cbk = {0};
sle_addr_t                    g_remote_addr = {0};
uint16_t                      g_conn_id = 0;
ssapc_find_service_result_t   g_find_service_result = {0};

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
        (void)memcpy_s(&g_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
        sle_stop_seek();
    }
}
int i = 0;
uint64_t count_before_get_us;
uint64_t count_after_get_us;
static char templine[32] = {0};
static void sle_uart_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
                                     errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    i++;
    if (i == 1) {
        count_before_get_us = uapi_tcxo_get_us();
    } else if (i == 1000) {
        count_after_get_us = uapi_tcxo_get_us();
        printf("count_after_get_us = %llu, count_before_get_us = %llu\r\n", count_after_get_us, count_before_get_us);
        printf("count_us = %llu, data[0] = %0X, data[1] = %0x\r\n", count_after_get_us - count_before_get_us, data->data[0], data->data[1]);
        float time = (float)(count_after_get_us - count_before_get_us) / 1000000.000;
        sprintf(templine, ": %f", time);
        printf("speed = %s s\r\n", templine);
        float speed = 235 * 1000 * 8 / time;
        sprintf(templine, ": %f", speed);
        printf("speed = %s bps\r\n", templine);
         LOS_MEM_POOL_STATUS status;
        LOS_MemInfoGet(m_aucSysMem0, &status);
        printf("[SYS INFO] mem: used:%u, free:%u\r\n", status.uwTotalUsedSize, status.uwTotalFreeSize);
        i = 0;
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
    // sle_connection_param_update_t parame = {0};
    // parame.conn_id = conn_id;
    // parame.interval_min = 0xA;
    // parame.interval_max = 0xA; //18.75ms
    // parame.max_latency = 0;
    // parame.supervision_timeout = 0x1f3;
    // sle_update_connect_param(&parame);
}

void sle_sample_update_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    test_suite_uart_sendf("[ssap client] updat state changed conn_id:%d, interval = %02x\n", conn_id, param->interval);
    if (status == 0) {
    }
}

void sle_sample_update_req_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_req_t *param)
{
    test_suite_uart_sendf("[ssap client] updat req state changed conn_id:%d, interval_min = %02x, interval_max = %02x\n", conn_id, param->interval_min, param->interval_max);
    if (status == 0) {
    }
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

void sle_sample_ssapc_cbk_register(ssapc_notification_callback notification_cb, ssapc_notification_callback indication_cb)
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

void sle_connect(void)
{
    sle_default_connect_param_t param = {0};
    param.enable_filter_policy = 0;
    param.gt_negotiate = 0;
    param.initiate_phys = 1;
    param.max_interval = 0xa;
    param.min_interval = 0xa;
    param.scan_interval = 400;
    param.scan_window = 20;
    param.timeout = 0x1fc;
    sle_default_connection_param_set(&param);
}

void sle_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb)
{
    uint8_t local_addr[SLE_ADDR_LEN] = { 0x13, 0x67,0x5c, 0x07, 0x00, 0x51 };//0x04-->0x07
    sle_addr_t local_address;
    local_address.type = 0;
    (void)memcpy_s(local_address.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN);
    sle_sample_seek_cbk_register();
    sle_connect();
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
    sle_client_init(sle_uart_notification_cb, sle_uart_indication_cb);
    return 0;
}

static void sle_speed_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_speed_init, 0, "RadarTask", 0x2000);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, 24);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

// /* Run the blinky_entry. */
app_run(sle_speed_entry);