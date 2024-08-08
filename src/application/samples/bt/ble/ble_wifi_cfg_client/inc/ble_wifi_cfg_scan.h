/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: BLE config wifi client scan.
 */
#ifndef BLE_WIFI_CFG_SCAN_H
#define BLE_WIFI_CFG_SCAN_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

errcode_t ble_wifi_device_start_scan(void);

errcode_t ble_wifi_set_scan_parameters(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif  /* __cplusplus */
#endif  /* __cplusplus */
#endif