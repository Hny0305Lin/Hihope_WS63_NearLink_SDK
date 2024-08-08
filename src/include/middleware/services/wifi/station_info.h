/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: station_info.h 的头文件
 */


/**
* @defgroup middleware_service_wifi_station_info WiFi Station Info
* @ingroup middleware_service_wifi
* @{
*/

#ifndef SERVICE_WIFI_STATION_INFO_H
#define SERVICE_WIFI_STATION_INFO_H
#include "wifi_device_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  Returns the information about the STA connected to the AP.
 * @else
 * @brief  返回与AP相连的STA信息。
 * @endif
 */
typedef struct {
    uint8_t mac_addr[WIFI_MAC_LEN];     /*!< @if Eng MAC address.
                                             @else  MAC地址。  @endif */
    int8_t rssi;                        /*!< @if Eng Received signal strength indicator (RSSI).
                                             @else  RSSI。  @endif */
    int8_t rsv;                         /*!< @if Eng Reserved.
                                             @else  保留字段。  @endif */
    uint32_t best_rate;                 /*!< @if Eng Best sending rate (kbps).
                                             @else  softap上一次接收相连的station报文最佳发送速率值。  @endif */
} wifi_sta_info_stru;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_STATION_INFO_H
/** @} */
