/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 * Description: sle uuid server sample.
 */
#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "sle_common.h"
#include "test_suite_uart.h"
#include "sle_errcode.h"
#include "sle_ssap_server.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "../inc/sle_uuid_server.h"
#include "../inc/sle_server_adv.h"
#include "app_init.h"
#include "watchdog.h"
#include "soc_osal.h" 
#include "tcxo.h"
#include "systick.h"
#include "los_memory.h"

#define OCTET_BIT_LEN 8
#define UUID_LEN_2     2
#define BT_INDEX_4     4
#define BT_INDEX_5     5
#define BT_INDEX_0     0

#define encode2byte_little(_ptr, data) \
    do { \
        *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 8); \
        *(uint8_t *)(_ptr) = (uint8_t)(data); \
    } while (0)

/* sle server app uuid for test */
char g_sle_uuid_app_uuid[UUID_LEN_2] = {0x0, 0x0};
/* server notify property uuid for test */
char g_sle_property_value[OCTET_BIT_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
/* sle connect acb handle */
uint16_t g_sle_conn_hdl = 0;
/* sle server handle */
uint8_t g_server_id = 0;
/* sle service handle */
uint16_t g_service_handle = 0;
/* sle ntf property handle */
uint16_t g_property_handle = 0;

uint16_t g_sle_paire_state = 0;

#define sample_at_log_print(fmt, args...) test_suite_uart_sendf(fmt, ##args)

static uint8_t sle_uuid_base[] = { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA, \
    0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static void sle_uuid_set_base(sle_uuid_t *out)
{
    (void)memcpy_s(out->uuid, SLE_UUID_LEN, sle_uuid_base, SLE_UUID_LEN);
    out->len = UUID_LEN_2;
}

static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[14], u2);
}

static void ssaps_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    sample_at_log_print("[uuid server] ssaps read request cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
        server_id, conn_id, read_cb_para->handle, status);
}

static void ssaps_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    sample_at_log_print("[uuid server] ssaps write request cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
        server_id, conn_id, write_cb_para->handle, status);
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id,  ssap_exchange_info_t *mtu_size,
    errcode_t status)
{
    sample_at_log_print("[uuid server] ssaps write request cbk server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n",
        server_id, conn_id, mtu_size->mtu_size, status);
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    sample_at_log_print("[uuid server] start service cbk server_id:%x, handle:%x, status:%x\r\n",
        server_id, handle, status);
}

static void sle_ssaps_register_cbks(void)
{
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = ssaps_read_request_cbk;
    ssaps_cbk.write_request_cb = ssaps_write_request_cbk;
    ssaps_register_callbacks(&ssaps_cbk);
}

errcode_t sle_uuid_server_send_report_by_handle_id(uint8_t *data, uint8_t len,uint16_t connect_id)
{
    ssaps_ntf_ind_t param = {0};
    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = data;
    param.value_len = len;
    ssaps_notify_indicate(g_server_id, connect_id, &param);
    return ERRCODE_SLE_SUCCESS;
}

#define DATA_LEN 236
unsigned char data[DATA_LEN];
extern uint8_t gle_tx_acb_data_num_get(void);
uint64_t count_before_get_us;
uint64_t count_after_get_us;

void send_data_thread_function(void)
{
    sle_set_data_len(g_sle_conn_hdl, 250);
    sle_set_phy_t phy_parm = {
        .tx_format = SLE_RADIO_FRAME_2,
        .rx_format = SLE_RADIO_FRAME_2,
        .tx_phy = 2,
        .rx_phy = 2,
        .tx_pilot_density = SLE_PHY_PILOT_DENSITY_16_TO_1,
        .rx_pilot_density = SLE_PHY_PILOT_DENSITY_16_TO_1,
        .g_feedback = 0,
        .t_feedback = 0,
    };
    sle_set_phy_param(g_sle_conn_hdl, &phy_parm);
    sle_set_mcs(g_sle_conn_hdl, 10);
    int i = 0;
    count_before_get_us = uapi_tcxo_get_us();
    for (int j = 0; j < 20000; j++) {
        while (i < 1000) {
            if (gle_tx_acb_data_num_get() > 0) {
                i++;
                data[0] = (i >> 8) & 0xFF;
                data[1] = i & 0xFF;
                sle_uuid_server_send_report_by_handle_id(data,DATA_LEN, g_sle_conn_hdl);
            }
        }
        i = 0;
        LOS_MEM_POOL_STATUS status;
        LOS_MemInfoGet(m_aucSysMem0, &status);
        printf("[SYS INFO] mem: used:%u, free:%u\r\n", status.uwTotalUsedSize, status.uwTotalFreeSize);
    } 
    count_after_get_us = uapi_tcxo_get_us();
    printf("count_after_get_us = %llu, count_before_get_us = %llu\r\n", count_after_get_us, count_before_get_us);
    printf("count_us = %llu\r\n", count_after_get_us - count_before_get_us);
    printf("t1:send data thread end\n");
}

static errcode_t sle_uuid_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_at_log_print("[uuid server] sle uuid add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = {0x01, 0x0};

    property.permissions = SLE_UUID_TEST_PROPERTIES;
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    property.value = osal_vmalloc(sizeof(g_sle_property_value));
    if (property.value == NULL) {
        sample_at_log_print("[uuid server] sle property mem fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value,
        sizeof(g_sle_property_value)) != EOK) {
        osal_vfree(property.value);
        sample_at_log_print("[uuid server] sle property mem cpy fail\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property,  &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_at_log_print("[uuid server] sle uuid add property fail, ret:%x\r\n", ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_UUID_TEST_DESCRIPTOR;
    descriptor.value = osal_vmalloc(sizeof(ntf_value));
    if (descriptor.value == NULL) {
        sample_at_log_print("[uuid server] sle descriptor mem fail\r\n");
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(descriptor.value, sizeof(ntf_value), ntf_value, sizeof(ntf_value)) != EOK) {
        sample_at_log_print("[uuid server] sle descriptor mem cpy fail\r\n");
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_at_log_print("[uuid server] sle uuid add descriptor fail, ret:%x\r\n", ret);
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    osal_vfree(descriptor.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = {0};

    sample_at_log_print("[uuid server] sle uuid add service in\r\n");
    app_uuid.len = sizeof(g_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_uuid_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }

    if (sle_uuid_server_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    sample_at_log_print("[uuid server] sle uuid add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
        g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_at_log_print("[uuid server] sle uuid add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    sample_at_log_print("[uuid server] sle uuid add service out\r\n");
    return ERRCODE_SLE_SUCCESS;
}

static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    sample_at_log_print("[uuid server] connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
        disc_reason:0x%x\r\n", conn_id, conn_state, pair_state, disc_reason);
    sample_at_log_print("[uuid server] connect state changed addr:%02x:**:**:**:%02x:%02x\r\n",
        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    g_sle_conn_hdl = conn_id;
    sle_connection_param_update_t parame = {0};
    parame.conn_id = conn_id;
    parame.interval_min = 0xA;
    parame.interval_max = 0xA; //18.75ms
    parame.max_latency = 0;
    parame.supervision_timeout = 0x1f4;
    if (conn_state ==  SLE_ACB_STATE_CONNECTED) {
        sle_update_connect_param(&parame);
    }
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    sample_at_log_print("[uuid server] pair complete conn_id:%02x, status:%x\r\n",
        conn_id, status);
    sample_at_log_print("[uuid server] pair complete addr:%02x:**:**:**:%02x:%02x\r\n",
        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    sle_pair_state_t pair_state;
    sle_get_pair_state(addr, &pair_state);
    printf("pair_state:0x%x\r\n", pair_state);
    if (pair_state == SLE_PAIR_PAIRED) {
        osal_task *task_handle = NULL;
        osal_kthread_lock();
        task_handle = osal_kthread_create((osal_kthread_handler)send_data_thread_function, 0, "RadarTask", 0x2000);
        osal_kthread_set_priority(task_handle, 25);
        if (task_handle != NULL) {
            osal_kfree(task_handle);
        }
        osal_kthread_unlock();
    }
    printf("kthread success\r\n");
}

void sle_sample_update_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_evt_t *param)
{
    test_suite_uart_sendf("[ssap server] updat state changed conn_id:%d, interval = %02x\n", conn_id, param->interval);
    if (status == 0) {
    }
}

void sle_sample_update_req_cbk(uint16_t conn_id, errcode_t status, const sle_connection_param_update_req_t *param)
{
    test_suite_uart_sendf("[ssap server] updat req state changed conn_id:%d, interval_min = %02x, interval_max = %02x\n", conn_id, param->interval_min, param->interval_max);
    if (status == 0) {
    }
}

void sle_sample_rssi_cbk(uint16_t conn_id, int8_t rssi, errcode_t status)
{
    test_suite_uart_sendf("[ssap server] conn_id:%d, rssi = %c, status = %x\n", conn_id, rssi, status);
}

static void sle_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    conn_cbks.connect_param_update_req_cb = sle_sample_update_req_cbk;
    conn_cbks.connect_param_update_cb = sle_sample_update_cbk;
    conn_cbks.read_rssi_cb = sle_sample_rssi_cbk;
    sle_connection_register_callbacks(&conn_cbks);
}

void sle_ssaps_set_info(void)
{
    ssap_exchange_info_t info = {0};
    info.mtu_size = 250;
    info.version = 1;
    ssaps_set_info(g_server_id, &info);
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

/* 初始化uuid server */
errcode_t sle_uuid_server_init(void)
{
    uapi_watchdog_disable();
    enable_sle();
    sle_ssaps_set_info();
    printf("sle enable\r\n");
    sle_conn_register_cbks();
    sle_ssaps_register_cbks();
    sle_uuid_server_add();
    sle_uuid_server_adv_init();
    sle_connect();
    sample_at_log_print("[uuid server] init ok\r\n");
    return ERRCODE_SLE_SUCCESS;
}

int sle_speed_init(void)
{
    for(int i=0;i<DATA_LEN;i++) {
        data[i] = 'A';
        data[DATA_LEN-1]='\0';
    }
    sle_uuid_server_init();
    return 0;
}

static void sle_speed_entry(void)
{
    osal_task *task_handle1 = NULL;
    osal_kthread_lock();
    task_handle1= osal_kthread_create((osal_kthread_handler)sle_speed_init, 0, "speed", 0x4000);
    if (task_handle1 != NULL) {
        osal_kthread_set_priority(task_handle1, 24);
        osal_kfree(task_handle1);
    }
    osal_kthread_unlock();
}

// /* Run the blinky_entry. */
app_run(sle_speed_entry);

