/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wifi_hotspot.h 的头文件
 */


#ifndef SERVICE_WIFI_HOTSPOT_H
#define SERVICE_WIFI_HOTSPOT_H
#include "wifi_device.h"
#include "errcode.h"
#include "wifi_hotspot_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_service_wifi_hotspot WiFi Hotspot
 * @ingroup  middleware_service_wifi
 * @{
 */


/**
 * @if Eng
 * @brief  Start softap interface.
 * @param  [in]  config SoftAP's configuration.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  开启SoftAP。
 * @param  [in]  config 表示SoftAP配置。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_softap_enable(const softap_config_stru *config);

/**
 * @if Eng
 * @brief  Close softap interface.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  关闭SoftAP。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_softap_disable(void);

/**
 * @if Eng
 * @brief  Get hotspot enable status.
 * @retval 1    softap is initialized.
 * @retval 0    softap is not initialized.
 * @else
 * @brief  获取hotspot使能状态。
 * @retval 1 softap已经使能。
 * @retval 0 softap未使能。
 * @endif
 */
int32_t wifi_is_softap_enabled(void);

/**
 * @if Eng
 * @brief  Extended configuration of SoftAP.
 * @param  [in]  config Config parameters of hotspot.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  开启SoftAP。
 * @param  [in]  config 表示Softap的扩展配置。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_set_softap_config_advance(const softap_config_advance_stru *config);

/**
 * @if Eng
 * @brief  get hotspot config.
 * @param  [out]  result Config of hotspot.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  获取SoftAP配置。
 * @param  [out]  result hotspot的配置信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_get_softap_config(softap_config_stru *result);

/**
 * @if Eng
 * @brief  get hotspot config.
 * @param  [out]  result Advance config of hotspot.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  获取SoftAP扩展配置。
 * @param  [out]  result hotspot的扩展配置信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_get_softap_config_advance(softap_config_advance_stru *result);

/**
 * @if Eng
 * @brief  Get all user's information of softap.
 * @param  [out]  result Station information.
 * @param  [out]  size Station number.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  softap获取已连接的station的信息。
 * @param  [out]  result STA信息。
 * @param  [out]  size   STA个数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_softap_get_sta_list(wifi_sta_info_stru *result, uint32_t *size);

/**
 * @if Eng
 * @brief  Softap deauth user by mac address.
 * @param  [in]  mac    Station mac address.
 * @param  [in]  mac_len Station mac address length, must be 6.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  softap指定断开连接的station网络。
 * @param  [in]  mac    MAC地址。
 * @param  [in]  mac_len MAC地址长度,必须为6。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_softap_deauth_sta(const uint8_t *mac, int32_t mac_len);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_HOTSPOT_H
