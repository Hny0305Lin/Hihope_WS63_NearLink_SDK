/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: adv config for ble uuid server.
 */
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "osal_addr.h"
#include "securec.h"
#include "errcode.h"
#include "test_suite_uart.h"
#include "bts_def.h"
#include "bts_le_gap.h"
#include "ble_wifi_cfg_adv.h"

#define NAME_MAX_LENGTH 15
#define EXT_ADV_OR_SCAN_RSP_DATA_LEN 251
#define HW_ADV_DATA_LEN 0x17

/* Ble Adv data length */
#define BLE_GENERAL_BYTE_1 1
/* Ble name adv name type */
#define BLE_ADV_LOCAL_NAME_DATA_TYPE 0x09
/* Ble name adv tx power type */
#define BLE_ADV_TX_POWER_LEVEL       0x0A
/* Ble name adv tx power response type */
#define BLE_SCAN_RSP_TX_POWER_LEVEL_LEN 0x03

#define sample_at_log_print(fmt, args...) test_suite_uart_sendf(fmt, ##args)

uint8_t g_device_name[ NAME_MAX_LENGTH] = { 'b', 'l', 'e', '_', 'w', 'i', 'f', 'i', '_', 'c',
    'o', 'n', 'f', 'i', 'g' };

uint8_t g_adv_data[31] = {0x02, 0x01, 0x02, 0x13, 0xFF, 0x7D, 0x02, 0x0E, 0x70, 0x80, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x01, 0x06, 0xCA, 0x2D, 0x28, 0xA0, 0x9D, 0xA3, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static uint16_t ble_set_scan_response_data(uint8_t *scan_rsp_data, uint8_t scan_rsp_data_max_len)
{
    uint8_t idx = 0;
    errno_t n_ret;

    if (scan_rsp_data == NULL) {
        return 0;
    }
    if (scan_rsp_data_max_len == 0) {
        return 0;
    }

    /* tx power level */
    ble_tx_power_level_st tx_power_level = {
        .length = BLE_SCAN_RSP_TX_POWER_LEVEL_LEN - BLE_GENERAL_BYTE_1,
        .adv_data_type = BLE_ADV_TX_POWER_LEVEL,
        .tx_power_value = 0,
    };

    n_ret = memcpy_s(scan_rsp_data, scan_rsp_data_max_len, &tx_power_level, sizeof(ble_tx_power_level_st));
    if (n_ret != EOK) {
        return 0;
    }
    idx += BLE_SCAN_RSP_TX_POWER_LEVEL_LEN;

    /* set local name */
    scan_rsp_data[idx++] = sizeof(g_device_name) + BLE_GENERAL_BYTE_1;
    scan_rsp_data[idx++] = BLE_ADV_LOCAL_NAME_DATA_TYPE;
    if ((idx + sizeof(g_device_name)) > scan_rsp_data_max_len) {
        return 0;
    }
    n_ret = memcpy_s(&scan_rsp_data[idx], scan_rsp_data_max_len - idx, g_device_name, sizeof(g_device_name));
    if (n_ret != EOK) {
        return 0;
    }
    idx += sizeof(g_device_name);
    return idx;
}

uint8_t ble_wifi_cfg_set_adv_data(void)
{
    errcode_t n_ret = 0;
    uint16_t scan_rsp_data_len;
    uint8_t set_scan_rsp_data[EXT_ADV_OR_SCAN_RSP_DATA_LEN] = { 0 };
    gap_ble_config_adv_data_t cfg_adv_data;

    /* set scan response data */
    scan_rsp_data_len = ble_set_scan_response_data(set_scan_rsp_data, EXT_ADV_OR_SCAN_RSP_DATA_LEN);
    if ((scan_rsp_data_len > EXT_ADV_OR_SCAN_RSP_DATA_LEN) || (scan_rsp_data_len == 0)) {
        return 0;
    }
    cfg_adv_data.adv_data = g_adv_data;
    cfg_adv_data.adv_length = HW_ADV_DATA_LEN;

    cfg_adv_data.scan_rsp_data = set_scan_rsp_data;
    cfg_adv_data.scan_rsp_length = scan_rsp_data_len;

    sample_at_log_print("[uplus][debug] uplus_ble_gap_adv_data_set adv_length=%x, adv_data=",
        cfg_adv_data.adv_length);
    for (int i = 0; i < cfg_adv_data.adv_length; i++) {
        sample_at_log_print(" %02x", cfg_adv_data.adv_data[i]);
    }
    sample_at_log_print("\n[uplus][debug] uplus_ble_gap_adv_data_set scan_rsp_length=%x, scan_rsp_data=",
        cfg_adv_data.scan_rsp_length);
    for (int i = 0; i < cfg_adv_data.scan_rsp_length; i++) {
        sample_at_log_print(" %02x", cfg_adv_data.scan_rsp_data[i]);
    }
    sample_at_log_print("\n");
    n_ret =  gap_ble_set_adv_data(BTH_GAP_BLE_ADV_HANDLE_DEFAULT, &cfg_adv_data);
    if (n_ret != 0) {
        return 0;
    }
    return 0;
}

uint8_t ble_wifi_cfg_start_adv(void)
{
    errcode_t n_ret = 0;
    gap_ble_adv_params_t adv_para = {0};

    ble_wifi_cfg_set_adv_data();
    adv_para.min_interval = BLE_ADV_MIN_INTERVAL;
    adv_para.max_interval = BLE_ADV_MAX_INTERVAL;
    adv_para.duration = BTH_GAP_BLE_ADV_FOREVER_DURATION;
    adv_para.peer_addr.type = BLE_PUBLIC_DEVICE_ADDRESS;
    /* 广播通道选择bitMap, 可参考BleAdvChannelMap */
    adv_para.channel_map = BLE_ADV_CHANNEL_MAP_CH_DEFAULT;
    adv_para.adv_type = BLE_ADV_TYPE_CONNECTABLE_UNDIRECTED;
    adv_para.adv_filter_policy = BLE_ADV_FILTER_POLICY_SCAN_ANY_CONNECT_ANY;
    (void)memset_s(&adv_para.peer_addr.addr, BD_ADDR_LEN, 0, BD_ADDR_LEN);
    n_ret = gap_ble_set_adv_param(BTH_GAP_BLE_ADV_HANDLE_DEFAULT, &adv_para);
    if (n_ret != 0) {
        return 0;
    }
    n_ret = gap_ble_start_adv(BTH_GAP_BLE_ADV_HANDLE_DEFAULT);
    if (n_ret != 0) {
        return 0;
    }
    return 0;
}
