/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wifi_event.h 的头文件
 */


#ifndef SERVICE_WIFI_EVENT_C_H
#define SERVICE_WIFI_EVENT_C_H

#include "wifi_linked_info.h"
#include "station_info.h"
#include "wifi_p2p_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_service_wifi_evevt WiFi Event
 * @ingroup middleware_service_wifi
 * @{
 */

/**
 * @if Eng
 * @brief  The pointer to a WiFi event callback for station and hotspot connection, disconnection, or scan.
 * @else
 * @brief  指向用于Sta和Hotspot连接、断开连接或扫描的WiFi事件回调的指针。
 * @endif
 */
typedef struct {
    /*!< @if Eng Connection state change.
         @else   连接状态改变。  @endif */
    void (*wifi_event_connection_changed)(int32_t state, const wifi_linked_info_stru *info, int32_t reason_code);
    /*!< @if Eng Scan state change.
         @else   扫描状态改变。  @endif */
    void (*wifi_event_scan_state_changed)(int32_t state, int32_t size);
    /*!< @if Eng Hotspot state change.
         @else   Hotspot状态改变。  @endif */
    void (*wifi_event_softap_state_changed)(int32_t state);
    /*!< @if Eng Station connected.
         @else   Station连接。  @endif */
    void (*wifi_event_softap_sta_join)(const wifi_sta_info_stru *info);
    /*!< @if Eng Station disconnected.
         @else   Station断连。  @endif */
    void (*wifi_event_softap_sta_leave)(const wifi_sta_info_stru *info);
    /*!< @if Eng P2P recive connect requset.
         @else   P2P接收连接请求。  @endif */
    void (*wifi_event_p2p_receive_connect)(const uint8_t *bssid, int8_t wps_method);
    /*!< @if Eng P2P go negotiation result.
         @else   P2P GO协商结果。  @endif */
    void (*wifi_event_p2p_go_neg_result)(int32_t state, int32_t mode);
    /*!< @if Eng P2P go start states change.
         @else   P2P GO建立结果。  @endif */
    void (*wifi_event_p2p_go_start)(int32_t state);
    /*!< @if Eng P2P Invitation Result.
         @else   P2P邀请结果。  @endif */
    void (*wifi_event_p2p_invitation_result)(int32_t state);
    /*!< @if Eng P2P gc connection change.
         @else   P2P Gc关联结果。  @endif */
    void (*wifi_event_p2p_gc_connection_changed)(int32_t state, const p2p_status_info_stru *status);
    /*!< @if Eng P2P go connection change.
         @else   P2P GO关联结果。  @endif */
    void (*wifi_event_p2p_go_connection_changed)(int32_t state, const p2p_client_info_stru *client);
    /*!< @if Eng WPS connect result.
         @else   WPS关联结果。  @endif */
    void (*wifi_event_wps_result)(int32_t state, wifi_if_type_enum wifi_if_type);
} wifi_event_stru;

/**
 * @if Eng
 * @brief  States in WiFi events.
 * @else
 * @brief  WiFi事件状态。
 * @endif
 */
typedef enum {
    WIFI_STATE_NOT_AVALIABLE = 0, /*!< @if Eng Unavailable state.
                                       @else   不可用状态。  @endif */
    WIFI_STATE_AVALIABLE          /*!< @if Eng Available state.
                                       @else   可用状态。  @endif */
} wifi_event_state_enum;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_EVENT_C_H
