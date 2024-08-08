/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wifi_hotspot_config.h 的头文件
 */


#ifndef SERVICE_WIFI_HOTSPOT_CONFIG_C_H
#define SERVICE_WIFI_HOTSPOT_CONFIG_C_H

#include "wifi_device_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_service_wifi_hotspot_config WiFi Hotspot Config
 * @ingroup middleware_service_wifi
 * @{
 */

/**
 * @if Eng
 * @brief  Config of hot spot.
 * @else
 * @brief  Hotspot的配置。
 * @endif
 */
typedef struct {
    int8_t ssid[WIFI_MAX_SSID_LEN];          /*!< @if Eng Service set ID (SSID).
                                                  @else   SSID。  @endif */
    int8_t pre_shared_key[WIFI_MAX_KEY_LEN]; /*!< @if Eng Pre Shared Key(PSK).
                                                  @else   预共享秘钥。  @endif  */
    int8_t reserved[2];
    wifi_security_enum security_type;        /*!< @if Eng Security Type.
                                                  @else   安全类型。  @endif */
    int32_t channel_num;                     /*!< @if Eng Channel number.
                                                  @else   信道号。  @endif */
    int32_t wifi_psk_type;                   /*!< @if Eng Type of PSK.
                                                  @else   PSK的类型。  @endif */
} softap_config_stru;


/**
 * @if Eng
 * @brief  Extended configuration of SoftAP.
 * @else
 * @brief  SoftAP的扩展配置。
 * @endif
 */
typedef struct {
    uint32_t beacon_interval;         /*!< @if Eng beacon_interval, range(25ms~1000ms), default(100ms),
                                                   0 means not configured.
                                           @else   beaconInterval, 范围25ms~1000ms, 默认100ms, 0表示未配置。  @endif */
    uint32_t dtim_period;             /*!< @if Eng dtim_period, range(1~30), default(2), 0 means not configured.
                                           @else   dtimPeriod, 范围1~30, 默认2，0表示未配置。  @endif */
    uint32_t group_rekey;             /*!< @if Eng group_rekey, range(30s~86400s), default(86400s),
                                                   0 means not configured.
                                           @else   config->groupRekey, 范围30s~86400s, 默认86400s，0表示未配置。  @endif */
    uint32_t hidden_ssid_flag;        /*!< @if Eng hidden_ssid_flag, not hidden:1, hidden: 2.
                                                   The default value is not hidden, 0 means not configured.
                                           @else   hiddenSsidFlag, 不隐藏：1，隐藏：2。默认配置为不隐藏，0表示未配置。  @endif */
    uint32_t gi;                      /*!< @if Eng gi,The default value is auto_GI.0,
                                                   indicating that this parameter is not configured.
                                                   If a non-zero valid value is configured,
                                                   a valid protocol mode must also be configured.
                                                   This function is not supported currently and
                                                   will be supported in the future.
                                           @else   config->Gi, 默认是auto_GI.0表示未配置。进行非0有效值配置时，
                                                   需要同时配置有效的协议模式。当前暂不支持，后续支持。  @endif */
    protocol_mode_enum protocol_mode; /*!< @if Eng protocol_mode_enum mode.By default, this parameter is configured
                                                   based on the maximum protocol capability of the chip.
                                                   0: not configured.
                                           @else   协议模式，默认按照芯片最大协议能力进行配置，0表示未配置。  @endif */
} softap_config_advance_stru;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_HOTSPOT_CONFIG_C_H
