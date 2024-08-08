/**
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
 */
#include "securec.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "soc_osal.h"
#include "app_init.h"
#include "common_def.h"
#include "cmsis_os2.h"
#include "debug_print.h"
#include "lwip/nettool/ping.h"
#define SLE_CONN_STATUS_LED_INDICATOR 0
#if SLE_CONN_STATUS_LED_INDICATOR
#include "LED_Indicator.h"

#endif

#define SLE_MTU_SIZE_DEFAULT 300
#define SLE_SEEK_INTERVAL_DEFAULT 100
#define SLE_SEEK_WINDOW_DEFAULT 100
#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16

static sle_announce_seek_callbacks_t g_seek_cbk = {0};
static sle_connection_callbacks_t g_connect_cbk = {0};
static ssapc_callbacks_t g_ssapc_cbk = {0};
static sle_addr_t g_remote_addr = {0};
#if 0
static uint16_t                      g_client_id = 0; /* 此处使用默认的client ID 0 */
#endif
static uint16_t g_conn_id = 0;
static ssapc_find_service_result_t g_find_service_result = {0};

static void example_sle_start_scan(void);

/**
 * @if Eng
 * @brief  LED Control type.
 * @else
 * @brief  LED控制类型。
 * @endif
 */
typedef enum {
    EXAMPLE_CONTORL_LED_EXIT = 0x00,              /*!< @if Eng Exit LED Control Demo
                                                    @else   退出LED控制演示  */
    EXAMPLE_CONTORL_LED_MAINBOARD_LED_ON = 0x01,  /*!< @if Eng Turn on the LED on MainBoard
                                                    @else   打开主板上的LED @endif */
    EXAMPLE_CONTORL_LED_MAINBOARD_LED_OFF = 0x02, /*!< @if Eng Turn off the LED on MainBoard
                                                    @else   关闭主板上的LED @endif */
    EXAMPLE_CONTORL_LED_LEDBOARD_RLED_ON = 0x03,  /*!< @if Eng Turn on the RED LED on LED Board
                                                    @else   打开灯板上的红色LED @endif */
    EXAMPLE_CONTORL_LED_LEDBOARD_RLED_OFF = 0x04, /*!< @if Eng Turn off the RED LED on LED Board
                                                    @else   关闭灯板上的红色LED @endif */
    EXAMPLE_CONTORL_LED_LEDBOARD_YLED_ON = 0x05,  /*!< @if Eng Turn on the YELLOW LED on LED Board
                                                    @else   打开灯板上的黄色LED @endif */
    EXAMPLE_CONTORL_LED_LEDBOARD_YLED_OFF = 0x06, /*!< @if Eng Turn off the YELLOW LED on LED Board
                                                    @else   关闭灯板上的黄色LED @endif */
    EXAMPLE_CONTORL_LED_LEDBOARD_GLED_ON = 0x07,  /*!< @if Eng Turn on the GREEN LED on LED Board
                                                    @else   打开灯板上的绿色LED @endif */
    EXAMPLE_CONTORL_LED_LEDBOARD_GLED_OFF = 0x08, /*!< @if Eng Turn off the GREEN LED on LED Board
                                                    @else   关闭灯板上的绿色LED @endif */
} example_control_led_type_t;

static errcode_t example_control_serverboard_led_by_write_req(uint8_t client_id,
                                                              uint16_t conn_id,
                                                              example_control_led_type_t led_operation)
{
    if (led_operation == EXAMPLE_CONTORL_LED_EXIT) {
        return ERRCODE_SUCC;
    } else if (led_operation == EXAMPLE_CONTORL_LED_MAINBOARD_LED_ON) {
        uint8_t write_req_data[] = {'L', 'E', 'D', '_', 'O', 'N'};
        uint8_t len = sizeof(write_req_data);
        ssapc_write_param_t param = {0};
        param.handle = g_find_service_result.start_hdl;
        param.type = SSAP_PROPERTY_TYPE_VALUE;
        param.data_len = len;
        param.data = write_req_data;
        ssapc_write_req(client_id, conn_id, &param);
    } else if (led_operation == EXAMPLE_CONTORL_LED_MAINBOARD_LED_OFF) {
        uint8_t write_req_data[] = {'L', 'E', 'D', '_', 'O', 'F', 'F'};
        uint8_t len = sizeof(write_req_data);
        ssapc_write_param_t param = {0};
        param.handle = g_find_service_result.start_hdl;
        param.type = SSAP_PROPERTY_TYPE_VALUE;
        param.data_len = len;
        param.data = write_req_data;
        ssapc_write_req(client_id, conn_id, &param);
    } else if (led_operation == EXAMPLE_CONTORL_LED_LEDBOARD_RLED_ON) {
        uint8_t write_req_data[] = {'R', 'L', 'E', 'D', '_', 'O', 'N'};
        uint8_t len = sizeof(write_req_data);
        ssapc_write_param_t param = {0};
        param.handle = g_find_service_result.start_hdl;
        param.type = SSAP_PROPERTY_TYPE_VALUE;
        param.data_len = len;
        param.data = write_req_data;
        ssapc_write_req(client_id, conn_id, &param);
    } else if (led_operation == EXAMPLE_CONTORL_LED_LEDBOARD_RLED_OFF) {
        uint8_t write_req_data[] = {'R', 'L', 'E', 'D', '_', 'O', 'F', 'F'};
        uint8_t len = sizeof(write_req_data);
        ssapc_write_param_t param = {0};
        param.handle = g_find_service_result.start_hdl;
        param.type = SSAP_PROPERTY_TYPE_VALUE;
        param.data_len = len;
        param.data = write_req_data;
        ssapc_write_req(client_id, conn_id, &param);
    } else if (led_operation == EXAMPLE_CONTORL_LED_LEDBOARD_YLED_ON) {
        uint8_t write_req_data[] = {'Y', 'L', 'E', 'D', '_', 'O', 'N'};
        uint8_t len = sizeof(write_req_data);
        ssapc_write_param_t param = {0};
        param.handle = g_find_service_result.start_hdl;
        param.type = SSAP_PROPERTY_TYPE_VALUE;
        param.data_len = len;
        param.data = write_req_data;
        ssapc_write_req(client_id, conn_id, &param);
    } else if (led_operation == EXAMPLE_CONTORL_LED_LEDBOARD_YLED_OFF) {
        uint8_t write_req_data[] = {'Y', 'L', 'E', 'D', '_', 'O', 'F', 'F'};
        uint8_t len = sizeof(write_req_data);
        ssapc_write_param_t param = {0};
        param.handle = g_find_service_result.start_hdl;
        param.type = SSAP_PROPERTY_TYPE_VALUE;
        param.data_len = len;
        param.data = write_req_data;
        ssapc_write_req(client_id, conn_id, &param);
    } else if (led_operation == EXAMPLE_CONTORL_LED_LEDBOARD_GLED_ON) {
        uint8_t write_req_data[] = {'G', 'L', 'E', 'D', '_', 'O', 'N'};
        uint8_t len = sizeof(write_req_data);
        ssapc_write_param_t param = {0};
        param.handle = g_find_service_result.start_hdl;
        param.type = SSAP_PROPERTY_TYPE_VALUE;
        param.data_len = len;
        param.data = write_req_data;
        ssapc_write_req(client_id, conn_id, &param);
    } else if (led_operation == EXAMPLE_CONTORL_LED_LEDBOARD_GLED_OFF) {
        uint8_t write_req_data[] = {'G', 'L', 'E', 'D', '_', 'O', 'F', 'F'};
        uint8_t len = sizeof(write_req_data);
        ssapc_write_param_t param = {0};
        param.handle = g_find_service_result.start_hdl;
        param.type = SSAP_PROPERTY_TYPE_VALUE;
        param.data_len = len;
        param.data = write_req_data;
        ssapc_write_req(client_id, conn_id, &param);
    }

    return ERRCODE_SUCC;
}

void example_sta_ping_ap(void)
{
    const char *expected_ip = "192.168.63.1";
    if (os_shell_ping(1, &expected_ip) != OS_OK) {
        PRINT("[WiFi STA] os shell ping fail\r\n");
    }
}

#define LED_STATUS_TASK_STACK_SIZE 0x1000
#define LED_STATUS_TASK_PRIO (osPriority_t)(17)

static int example_get_led_status_and_ping_task(const char *arg)
{
    unused(arg);
    errcode_t ret = ERRCODE_FAIL;
    PRINT("[SLE Client]start get led status and ping task\r\n");

    do {
        (void)osal_msleep(4000); /* 延时4s，等待写请求及后继的操作完成，避免Log混在一起 */
        example_sta_ping_ap();

        (void)osal_msleep(6000); /* 延时6s，等待Ping的结果信息打印完成，避免Log混在一起 */
        ret = ssapc_read_req(0, g_conn_id, g_find_service_result.start_hdl,
                             SSAP_PROPERTY_TYPE_VALUE); /* 此处使用默认的client ID 0 */
        if (ret != ERRCODE_SUCC) {
            PRINT("[SLE Client] send ssapc_read_req to server led status fail ret 0x%x \r\n", ret);
        }
    } while (1);

    return 0;
}

static void example_get_led_status_and_ping_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)example_get_led_status_and_ping_task, 0,
                                      "LedStatusAndPingTask", LED_STATUS_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, LED_STATUS_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

static void example_sle_enable_cbk(errcode_t status)
{
    if (status == ERRCODE_SUCC) {
        example_sle_start_scan();
    }
}

static void example_sle_seek_enable_cbk(errcode_t status)
{
    if (status == ERRCODE_SUCC) {
        return;
    }
}

static void example_sle_seek_disable_cbk(errcode_t status)
{
    if (status == ERRCODE_SUCC) {
        sle_connect_remote_device(&g_remote_addr);
    }
}

static uint8_t g_sle_expected_addr[SLE_ADDR_LEN] = {0x02, 0x01, 0x06, 0x08, 0x06, 0x03};

static void example_sle_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data == NULL) {
        PRINT("[SLE Client] seek result seek_result_data is NULL\r\n");
        return;
    }

    if (memcmp((void *)seek_result_data->addr.addr, (void *)g_sle_expected_addr, SLE_ADDR_LEN) == 0) {
        PRINT("[SLE Client] seek result find expected addr:%02x***%02x%02x\r\n", seek_result_data->addr.addr[0],
              seek_result_data->addr.addr[4], seek_result_data->addr.addr[5]);
        (void)memcpy_s(&g_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
        sle_stop_seek();
    }
}

static void example_sle_seek_cbk_register(void)
{
    g_seek_cbk.sle_enable_cb = example_sle_enable_cbk;
    g_seek_cbk.seek_enable_cb = example_sle_seek_enable_cbk;
    g_seek_cbk.seek_disable_cb = example_sle_seek_disable_cbk;
    g_seek_cbk.seek_result_cb = example_sle_seek_result_info_cbk;
}

static void example_sle_connect_state_changed_cbk(uint16_t conn_id,
                                                  const sle_addr_t *addr,
                                                  sle_acb_state_t conn_state,
                                                  sle_pair_state_t pair_state,
                                                  sle_disc_reason_t disc_reason)
{
    PRINT("[SLE Client] conn state changed conn_id:0x%x, addr:%02x***%02x%02x\r\n", conn_id, addr->addr[0],
          addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    PRINT("[SLE Client] conn state changed disc_reason:0x%x\r\n", disc_reason);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        if (pair_state == SLE_PAIR_NONE) {
            sle_pair_remote_device(&g_remote_addr);
        }
        g_conn_id = conn_id;
    }
}

static void example_sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    PRINT("[SLE Client] pair complete conn_id:0x%x, status:0x%x, addr:%02x***%02x%02x\r\n", conn_id, status,
          addr->addr[0], addr->addr[4], addr->addr[5]); /* 0 4 5: addr index */
    if (status == ERRCODE_SUCC) {
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(1, conn_id, &info); /* 此处没有使用默认的client ID 0 */
    }
}

static void example_sle_connect_cbk_register(void)
{
    g_connect_cbk.connect_state_changed_cb = example_sle_connect_state_changed_cbk;
    g_connect_cbk.pair_complete_cb = example_sle_pair_complete_cbk;
}

static void example_sle_exchange_info_cbk(uint8_t client_id,
                                          uint16_t conn_id,
                                          ssap_exchange_info_t *param,
                                          errcode_t status)
{
    PRINT("[SLE Client] pair complete client id:0x%x, status:0x%x\r\n", client_id, status);
    PRINT("[SLE Client] exchange mtu, mtu size:0x%x, version:0x%x.\r\n", param->mtu_size, param->version);

    if (status == ERRCODE_SUCC) {
        ssapc_find_structure_param_t find_param = {0};
        find_param.type = SSAP_FIND_TYPE_PRIMARY_SERVICE;
        find_param.start_hdl = 1;
        find_param.end_hdl = 0xFFFF;
        ssapc_find_structure(0, conn_id, &find_param); /* 此处使用默认的client ID 0 */
    }
}

static void example_sle_find_structure_cbk(uint8_t client_id,
                                           uint16_t conn_id,
                                           ssapc_find_service_result_t *service,
                                           errcode_t status)
{
    PRINT("[SLE Client] find structure cbk client:0x%x conn_id:0x%x status:0x%x \r\n", client_id, conn_id, status);
    PRINT("[SLE Client] find structure start_hdl:[0x%04x], end_hdl:[0x%04x], uuid len:0x%x\r\n", service->start_hdl,
          service->end_hdl, service->uuid.len);
    if (service->uuid.len == UUID_16BIT_LEN) {
        PRINT("[SLE Client] structure uuid:[0x%02x][0x%02x]\r\n", service->uuid.uuid[14],
              service->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            PRINT("[SLE Client] structure uuid[0x%x]:[0x%02x]\r\n", idx, service->uuid.uuid[idx]);
        }
    }

    if (status == ERRCODE_SUCC) {
        g_find_service_result.start_hdl = service->start_hdl;
        g_find_service_result.end_hdl = service->end_hdl;
        memcpy_s(&g_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
    }
}

static void example_sle_find_structure_cmp_cbk(uint8_t client_id,
                                               uint16_t conn_id,
                                               ssapc_find_structure_result_t *structure_result,
                                               errcode_t status)
{
    PRINT("[SLE Client] find structure cmp cbk client id:0x%x conn_id:0x%x status:0x%x type:%d uuid len:0x%x \r\n",
          client_id, conn_id, status, structure_result->type, structure_result->uuid.len);
    if (structure_result->uuid.len == UUID_16BIT_LEN) {
        PRINT("[SLE Client] find structure cmp cbk structure uuid:[0x%02x][0x%02x]\r\n",
              structure_result->uuid.uuid[14], structure_result->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            PRINT("[SLE Client] find structure cmp cbk structure uuid[0x%x]:[0x%02x]\r\n", idx,
                  structure_result->uuid.uuid[idx]);
        }
    }

    if (status == ERRCODE_SUCC) {
        example_get_led_status_and_ping_entry();
    }
}

static void example_sle_find_property_cbk(uint8_t client_id,
                                          uint16_t conn_id,
                                          ssapc_find_property_result_t *property,
                                          errcode_t status)
{
    PRINT(
        "[SLE Client] find property cbk, client id:0x%x, conn id:0x%x, operate ind:0x%x, handle:0x%x"
        "descriptors count:0x%x status:0x%x.\r\n",
        client_id, conn_id, property->operate_indication, property->handle, property->descriptors_count, status);
    for (uint16_t idx = 0; idx < property->descriptors_count; idx++) {
        PRINT("[SLE Client] find property cbk, descriptors type [0x%x]: 0x%02x.\r\n", idx,
              property->descriptors_type[idx]);
    }
    if (property->uuid.len == UUID_16BIT_LEN) {
        PRINT("[SLE Client] find property cbk, uuid: 0x%02x 0x%02x.\r\n", property->uuid.uuid[14],
              property->uuid.uuid[15]); /* 14 15: uuid index */
    } else if (property->uuid.len == UUID_128BIT_LEN) {
        for (uint16_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            PRINT("[SLE Client] find property cbk, uuid [0x%x]: 0x%02x.\r\n", idx, property->uuid.uuid[idx]);
        }
    }
}

static void example_sle_write_cfm_cbk(uint8_t client_id,
                                      uint16_t conn_id,
                                      ssapc_write_result_t *write_result,
                                      errcode_t status)
{
    PRINT("[SLE Client] write cfm cbk client id:0x%x conn_id:0x%x status:0x%x.\r\n", client_id, conn_id, status);
    PRINT("[SLE Client] write cfm cbk handle:0x%x, type:0x%x\r\n", write_result->handle, write_result->type);
}

static void example_sle_read_cfm_cbk(uint8_t client_id,
                                     uint16_t conn_id,
                                     ssapc_handle_value_t *read_data,
                                     errcode_t status)
{
    PRINT("[SLE Client] read cfm cbk client id:0x%x conn id:0x%x status:0x%x\r\n", client_id, conn_id, status);
    PRINT("[SLE Client] read cfm cbk handle:0x%x, type:0x%x, len:0x%x\r\n", read_data->handle, read_data->type,
          read_data->data_len);
    for (uint16_t idx = 0; idx < read_data->data_len; idx++) {
        PRINT("[SLE Client] read cfm cbk[0x%x] 0x%02x\r\n", idx, read_data->data[idx]);
    }

    if (status == ERRCODE_SUCC) {
        if (read_data->data_len == strlen("LED_ON") && read_data->data[0] == 'L' && read_data->data[1] == 'E' &&
            read_data->data[2] == 'D' && read_data->data[3] == '_' && read_data->data[4] == 'O' &&
            read_data->data[5] == 'N') {
            example_control_serverboard_led_by_write_req(client_id, conn_id, EXAMPLE_CONTORL_LED_MAINBOARD_LED_OFF);
        } else {
            example_control_serverboard_led_by_write_req(client_id, conn_id, EXAMPLE_CONTORL_LED_MAINBOARD_LED_ON);
        }
    }
}

static void example_sle_ssapc_cbk_register(void)
{
    g_ssapc_cbk.exchange_info_cb = example_sle_exchange_info_cbk;
    g_ssapc_cbk.find_structure_cb = example_sle_find_structure_cbk;
    g_ssapc_cbk.find_structure_cmp_cb = example_sle_find_structure_cmp_cbk;
    g_ssapc_cbk.ssapc_find_property_cbk = example_sle_find_property_cbk;
    g_ssapc_cbk.write_cfm_cb = example_sle_write_cfm_cbk;
    g_ssapc_cbk.read_cfm_cb = example_sle_read_cfm_cbk;
}

static void example_sle_start_scan(void)
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

static int example_wifi_sle_coexist_client_task(const char *arg)
{
#if SLE_CONN_STATUS_LED_INDICATOR
    errcode_t ret = ERRCODE_FAIL;
    uint16_t size = 0;
    uint16_t last_size = 0;
    bool first_indicate = true;
#endif

    unused(arg);

    (void)osal_msleep(5000); /* 延时5s，等待SLE初始化完毕 */

    PRINT("[SLE Client] try enable.\r\n");

    example_sle_seek_cbk_register();
    example_sle_connect_cbk_register();
    example_sle_ssapc_cbk_register();
    if (sle_announce_seek_register_callbacks(&g_seek_cbk) != ERRCODE_SUCC) {
        PRINT("[SLE Client] sle announce seek register callbacks fail !\r\n");
        return -1;
    }

    if (sle_connection_register_callbacks(&g_connect_cbk) != ERRCODE_SUCC) {
        PRINT("[SLE Client] sle connection register callbacks fail !\r\n");
        return -1;
    }

    if (ssapc_register_callbacks(&g_ssapc_cbk) != ERRCODE_SUCC) {
        PRINT("[SLE Client] sle ssapc register callbacks !\r\n");
        return -1;
    }

    if (enable_sle() != ERRCODE_SUCC) {
        PRINT("[SLE Client] sle enbale fail !\r\n");
        return -1;
    }

#if SLE_CONN_STATUS_LED_INDICATOR
    size = 0;
    last_size = 0;

    do {
        (void)osal_msleep(1000); /* 延时1s，每个1s判断一次是否有SLE设备连接 */

        ret = sle_get_paired_devices_num(&size);
        if (ret != ERRCODE_SUCC) {
            PRINT("[SLE Server] get paired devices num fail\r\n");
            continue;
        }

        if (size == 0 && (last_size != 0 || first_indicate)) { /* 配对的对端设备数量为0，且之前配对的对端设备数量不为0。或者，配对的对端设备数量为0，且是第一次操作指示灯
                                                                */
            example_sle_connect_indicator_exit();
            example_sle_disconnect_indicator_entry();
            first_indicate = false;
        } else if (size != 0 && (last_size == 0 || first_indicate)) {
            example_sle_disconnect_indicator_exit();
            example_sle_connect_indicator_entry();
            first_indicate = false;
        }
        last_size = size;
    } while (1);
#endif

    return 0;
}

#define WIFI_SLE_COEXIST_CLI_TASK_PRIO 24
#define WIFI_SLE_COEXIST_CLI_STACK_SIZE 0x2000

static void example_wifi_sle_coexist_client_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)example_wifi_sle_coexist_client_task, 0,
                                      "WiFiSLECoexistClientTask", WIFI_SLE_COEXIST_CLI_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, WIFI_SLE_COEXIST_CLI_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the example_wifi_sle_coexist_client_entry. */
app_run(example_wifi_sle_coexist_client_entry);