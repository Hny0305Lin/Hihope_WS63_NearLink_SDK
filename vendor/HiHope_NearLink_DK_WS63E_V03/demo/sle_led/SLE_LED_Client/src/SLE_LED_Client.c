/*
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
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
#include "../inc/SLE_LED_Client.h"
#include "soc_osal.h"
#include "app_init.h"
#include "common_def.h"

#include "debug_print.h"
#include "pinctrl.h"
#include "gpio.h"

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

static void example_turn_onoff_led(pin_t pin, gpio_level_t level)
{
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(pin, level);
}

static void example_led_notification_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data)
{

    if (data->data_len == strlen("RLED_ON") && data->data[0] == 'R' && data->data[1] == 'L' && data->data[2] == 'E' &&
        data->data[3] == 'D' && data->data[4] == '_' && data->data[5] == 'O' && data->data[6] == 'N') {
        example_turn_onoff_led(GPIO_07, GPIO_LEVEL_HIGH);
    }

    if (data->data_len == strlen("RLED_OFF") && data->data[0] == 'R' && data->data[1] == 'L' && data->data[2] == 'E' &&
        data->data[3] == 'D' && data->data[4] == '_' && data->data[5] == 'O' && data->data[6] == 'F' &&
        data->data[7] == 'F') {
        example_turn_onoff_led(GPIO_07, GPIO_LEVEL_LOW);
    }

    if (data->data_len == strlen("YLED_ON") && data->data[0] == 'Y' && data->data[1] == 'L' && data->data[2] == 'E' &&
        data->data[3] == 'D' && data->data[4] == '_' && data->data[5] == 'O' && data->data[6] == 'N') {
        example_turn_onoff_led(GPIO_10, GPIO_LEVEL_HIGH);
    }

    if (data->data_len == strlen("YLED_OFF") && data->data[0] == 'Y' && data->data[1] == 'L' && data->data[2] == 'E' &&
        data->data[3] == 'D' && data->data[4] == '_' && data->data[5] == 'O' && data->data[6] == 'F' &&
        data->data[7] == 'F') {
        example_turn_onoff_led(GPIO_10, GPIO_LEVEL_LOW);
    }

    if (data->data_len == strlen("GLED_ON") && data->data[0] == 'G' && data->data[1] == 'L' && data->data[2] == 'E' &&
        data->data[3] == 'D' && data->data[4] == '_' && data->data[5] == 'O' && data->data[6] == 'N') {
        example_turn_onoff_led(GPIO_11, GPIO_LEVEL_HIGH);
    }

    if (data->data_len == strlen("GLED_OFF") && data->data[0] == 'G' && data->data[1] == 'L' && data->data[2] == 'E' &&
        data->data[3] == 'D' && data->data[4] == '_' && data->data[5] == 'O' && data->data[6] == 'F' &&
        data->data[7] == 'F') {
        example_turn_onoff_led(GPIO_11, GPIO_LEVEL_LOW);
    }

    /* 将本端(Cient)的LED等状态, 通过写请求发送给对端(Server) */
    ssapc_write_param_t param = {0};
    param.handle = g_find_service_result.start_hdl;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data_len = data->data_len;

    param.data = osal_vmalloc(param.data_len);
    if (param.data == NULL) {
        PRINT("[SLE Client] write req mem fail\r\n");
        return;
    }

    if (memcpy_s(param.data, param.data_len, data, data->data_len) != EOK) {
        PRINT("[SLE Client] write req memcpy fail\r\n");
        osal_vfree(param.data);
        return;
    }

    if (ssapc_write_req(client_id, conn_id, &param) != ERRCODE_SUCC) {
        PRINT("[SLE Client] write req fail\r\n");
        osal_vfree(param.data);
        return;
    }

    osal_vfree(param.data);
    return;
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

static uint8_t g_sle_expected_addr[SLE_ADDR_LEN] = {0x04, 0x01, 0x06, 0x08, 0x06, 0x03};

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
}

static void example_sle_notification_cbk(uint8_t client_id,
                                         uint16_t conn_id,
                                         ssapc_handle_value_t *data,
                                         errcode_t status)
{
    PRINT("[SLE Client] notification cbk client id:0x%x conn id:0x%x status:0x%x\r\n", client_id, conn_id, status);

    PRINT("[SLE Client] notification cbk handle:0x%x, type:0x%x data len:0x%x \r\n", data->handle, data->type,
          data->data_len);

    for (uint16_t idx = 0; idx < data->data_len; idx++) {
        PRINT("[SLE Client] notification cbk[0x%x] 0x%02x\r\n", idx, data->data[idx]);
    }

    if (status == ERRCODE_SUCC) {
        example_led_notification_cbk(client_id, conn_id, data);
    }
}

static void example_sle_indication_cbk(uint8_t client_id,
                                       uint16_t conn_id,
                                       ssapc_handle_value_t *data,
                                       errcode_t status)
{
    PRINT("[SLE Client] indication cbk client id:0x%x conn id:0x%x status:0x%x\r\n", client_id, conn_id, status);

    PRINT("[SLE Client] indication cbk handle:0x%x, type:0x%x data len:0x%x \r\n", data->handle, data->type,
          data->data_len);

    for (uint16_t idx = 0; idx < data->data_len; idx++) {
        PRINT("[SLE Client] indication cbk[0x%x] 0x%02x\r\n", idx, data->data[idx]);
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

    g_ssapc_cbk.notification_cb = example_sle_notification_cbk;
    g_ssapc_cbk.indication_cb = example_sle_indication_cbk;
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

static int example_sle_led_client_task(const char *arg)
{
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
   
    return 0;
}

#define SLE_LED_CLI_TASK_PRIO 24
#define SLE_LED_CLI_STACK_SIZE 0x2000

static void example_sle_led_client_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)example_sle_led_client_task, 0, "SLELedClientTask",
                                      SLE_LED_CLI_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_LED_CLI_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the example_sle_led_client_entry. */
app_run(example_sle_led_client_entry);