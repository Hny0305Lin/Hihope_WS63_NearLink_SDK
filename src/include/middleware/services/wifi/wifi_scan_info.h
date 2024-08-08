/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wifi_scan_info.h 的头文件
 */


/**
* @defgroup middleware_service_wifi_scan_info WiFi Scan Info
* @ingroup middleware_service_wifi
* @{
*/

#ifndef SERVICE_WIFI_SCAN_INFO_C_H
#define SERVICE_WIFI_SCAN_INFO_C_H

#include "wifi_device_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  WiFi scan result information.
 * @else
 * @brief  WiFi扫描结果信息。
 * @endif
 */
typedef struct {
    char ssid[WIFI_MAX_SSID_LEN];       /*!< @if Eng Service set ID (SSID).
                                             @else SSID。 @endif */
    uint8_t bssid[WIFI_MAC_LEN];        /*!< @if Eng Basic service set ID (BSSID).
                                             @else BSSID。 @endif */
    int8_t reserved;
    wifi_security_enum security_type;   /*!< @if Eng Security type.For details, see wifi_security_enumType.
                                             @else 安全类型。 @endif */
    int32_t rssi;                       /*!< @if Eng Received signal strength indicator (RSSI).
                                             @else 信号强度。 @endif */
    int32_t band;                       /*!< @if Eng band.
                                             @else 频带。 @endif */
    int32_t channel_num;                /*!< @if Eng channel num.
                                             @else 信道号。 @endif */
} wifi_scan_info_stru;

/**
 * @if Eng
 * @brief  callback function definition of wpa-enterprise param import.
 * @param  [in]  scan_result scan_result.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  定制化扫描回调函数。
 * @param  [in]  scan_result 扫描结果。
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
typedef void (*wifi_scan_no_save_cb)(wifi_scan_info_stru *scan_result);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_SCAN_INFO_C_H
/** @} */
