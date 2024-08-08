/*
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
#include "errcode.h"
#include "osal_addr.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "../inc/WiFi_SLE_Coexist_Server_adv.h"

#include "cmsis_os2.h"
#include "debug_print.h"
/* sle device name */
#define NAME_MAX_LENGTH 17
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MIN_DEFAULT 0x64
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MAX_DEFAULT 0x64
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MIN_DEFAULT 0xC8
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MAX_DEFAULT 0xC8
/* 超时时间5000ms，单位10ms */
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT 0x1F4
/* 超时时间4990ms，单位10ms */
#define SLE_CONN_MAX_LATENCY 0x1F3
/* 广播发送功率 */
#define SLE_ADV_TX_POWER 10
/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT 1
/* 最大广播数据长度 */
#define SLE_ADV_DATA_LEN_MAX 251
/* 广播名称 */
static uint8_t sle_local_name[NAME_MAX_LENGTH] = {'W', 'i', 'F', 'i', '_', 'S', 'L', 'E', '_',
                                                  'C', 'o', 'e', 'x', 'i', 's', 't', '\0'};

static uint16_t example_sle_set_adv_local_name(uint8_t *adv_data, uint16_t max_len)
{
    errno_t ret = -1;
    uint8_t index = 0;

    uint8_t *local_name = sle_local_name;
    uint8_t local_name_len = (uint8_t)strlen((char *)local_name);
    for (uint8_t i = 0; i < local_name_len; i++) {
        PRINT("[SLE Adv] local_name[%d] = 0x%02x\r\n", i, local_name[i]);
    }

    adv_data[index++] = local_name_len + 1;
    adv_data[index++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    ret = memcpy_s(&adv_data[index], max_len - index, local_name, local_name_len);
    if (ret != EOK) {
        PRINT("[SLE Adv] memcpy fail\r\n");
        return 0;
    }
    return (uint16_t)index + local_name_len;
}

static uint16_t example_sle_set_adv_data(uint8_t *adv_data)
{
    size_t len = 0;
    uint16_t idx = 0;
    errno_t ret = 0;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_disc_level = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    ret = memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_disc_level, len);
    if (ret != EOK) {
        PRINT("[SLE Adv] adv_disc_level memcpy fail\r\n");
        return 0;
    }
    idx += len;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_access_mode = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_ACCESS_MODE,
        .value = 0,
    };
    ret = memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_access_mode, len);
    if (ret != EOK) {
        PRINT("[SLE Adv] memcpy fail\r\n");
        return 0;
    }
    idx += len;
    return idx;
}

static uint16_t example_sle_set_scan_response_data(uint8_t *scan_rsp_data)
{
    uint16_t idx = 0;
    errno_t ret = -1;
    size_t scan_rsp_data_len = sizeof(struct sle_adv_common_value);

    struct sle_adv_common_value tx_power_level = {
        .length = scan_rsp_data_len - 1,
        .type = SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
        .value = SLE_ADV_TX_POWER,
    };
    ret = memcpy_s(scan_rsp_data, SLE_ADV_DATA_LEN_MAX, &tx_power_level, scan_rsp_data_len);
    if (ret != EOK) {
        PRINT("[SLE Adv] sle scan response data memcpy fail\r\n");
        return 0;
    }
    idx += scan_rsp_data_len;

    /* set local name */
    idx += example_sle_set_adv_local_name(&scan_rsp_data[idx], SLE_ADV_DATA_LEN_MAX - idx);
    return idx;
}

// 0x02 : Demo No. , from 1; 0x01, 0x06, 0x08 : Hoperun Address; 0x06, 0x03 : WS63
static uint8_t g_sle_local_addr[SLE_ADDR_LEN] = {0x02, 0x01, 0x06, 0x08, 0x06, 0x03};

static void example_sle_set_addr(void)
{
    uint8_t *addr = g_sle_local_addr;

    sle_addr_t sle_addr = {0};
    sle_addr.type = 0;
    if (memcpy_s(sle_addr.addr, SLE_ADDR_LEN, addr, SLE_ADDR_LEN) != EOK) {
        PRINT("[SLE Adv] addr memcpy fail \r\n");
    }

    if (sle_set_local_addr(&sle_addr) == ERRCODE_SUCC) {
        PRINT("[SLE Adv] set sle addr SUCC \r\n");
    }
}

static uint8_t g_local_device_name[] = {'W', 'i', 'F', 'i', '_', 'S', 'L', 'E', '_', 'C', 'o', 'e', 'x', 'i', 's', 't'};

static void example_sle_set_name(void)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = sle_set_local_name(g_local_device_name, sizeof(g_local_device_name));
    if (ret != ERRCODE_SUCC) {
        PRINT("[SLE Adv] set local name fail, ret:%x\r\n", ret);
    }
}

static errcode_t example_sle_set_default_announce_param(void)
{
    sle_announce_param_t param = {0};
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_ADV_HANDLE_DEFAULT;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;

    if (memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, g_sle_local_addr, SLE_ADDR_LEN) != EOK) {
        PRINT("[SLE Adv] set sle adv param memcpy addr fail\r\n");
        return ERRCODE_MEMCPY;
    }

    return sle_set_announce_param(param.announce_handle, &param);
}

static errcode_t example_sle_set_default_announce_data(void)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t announce_data_len = 0;
    uint8_t seek_data_len = 0;
    sle_announce_data_t data = {0};
    uint8_t adv_handle = SLE_ADV_HANDLE_DEFAULT;
    uint8_t announce_data[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t seek_rsp_data[SLE_ADV_DATA_LEN_MAX] = {0};

    PRINT("[SLE Adv] set adv data default\r\n");
    announce_data_len = example_sle_set_adv_data(announce_data);
    data.announce_data = announce_data;
    data.announce_data_len = announce_data_len;

    seek_data_len = example_sle_set_scan_response_data(seek_rsp_data);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = seek_data_len;

    ret = sle_set_announce_data(adv_handle, &data);
    if (ret == ERRCODE_SUCC) {
        PRINT("[SLE Adv] set announce data success.");
    } else {
        PRINT("[SLE Adv] set adv param fail.");
    }
    return ERRCODE_SUCC;
}

void example_sle_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    PRINT("[SLE Adv] sle announce enable id:%02x, state:%02x\r\n", announce_id, status);
}

void example_sle_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    PRINT("[SLE Adv] sle announce disable id:%02x, state:%02x\r\n", announce_id, status);
}

void example_sle_announce_terminal_cbk(uint32_t announce_id)
{
    PRINT("[SLE Adv] sle announce terminal id:%02x\r\n", announce_id);
}

void example_sle_enable_cbk(errcode_t status)
{
    PRINT("[SLE Adv] sle enable status:%02x\r\n", status);
}

void example_sle_announce_register_cbks(void)
{
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.announce_enable_cb = example_sle_announce_enable_cbk;
    seek_cbks.announce_disable_cb = example_sle_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = example_sle_announce_terminal_cbk;
    seek_cbks.sle_enable_cb = example_sle_enable_cbk;
    sle_announce_seek_register_callbacks(&seek_cbks);
}

errcode_t example_sle_server_adv_init(void)
{
    PRINT("[SLE Adv] example_sle_server_adv_init in\r\n");
    example_sle_announce_register_cbks();
    example_sle_set_default_announce_param();
    example_sle_set_default_announce_data();

    example_sle_set_addr();
    example_sle_set_name();

    sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    PRINT("[SLE Adv] example_sle_server_adv_init out\r\n");
    return ERRCODE_SUCC;
}
