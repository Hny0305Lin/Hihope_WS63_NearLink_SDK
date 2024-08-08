/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: BLE config wifi client scan
*/

#include "errcode.h"
#include "bts_def.h"
#include "bts_le_gap.h"
#include "ble_wifi_cfg_scan.h"

static uint16_t scan_interval = 0x48;
static uint16_t scan_window = 0x48;
static uint8_t  scan_type = 0x00;
static uint8_t  scan_phy = 0x01;
static uint8_t  scan_filter_policy = 0x00;

errcode_t ble_wifi_set_scan_parameters(void)
{
    gap_ble_scan_params_t ble_device_scan_params = { 0 };
    ble_device_scan_params.scan_interval = scan_interval;
    ble_device_scan_params.scan_window = scan_window;
    ble_device_scan_params.scan_type = scan_type;
    ble_device_scan_params.scan_phy = scan_phy;
    ble_device_scan_params.scan_filter_policy = scan_filter_policy;
    return gap_ble_set_scan_parameters(&ble_device_scan_params);
}

errcode_t ble_wifi_device_start_scan(void)
{
    return gap_ble_start_scan();
}