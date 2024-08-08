/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wifi_p2p.h 的头文件
 */


#ifndef SERVICE_WIFI_P2P_H
#define SERVICE_WIFI_P2P_H
#include "errcode.h"
#include "wifi_p2p_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_service_wifi_p2p Wifi P2P
 * @ingroup  middleware_service_wifi
 * @{
 */


/**
 * @if Eng
 * @brief  Start P2P interface.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  P2P开启。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_enable(void);

/**
 * @if Eng
 * @brief  Close wifi p2p.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  P2P关闭。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_disable(void);

/**
 * @if Eng
 * @brief  Get P2P enable status.
 * @retval 1    P2P is initialized.
 * @retval 0    P2P is not initialized.
 * @else
 * @brief  获取P2P使能状态。
 * @retval 1 P2P已经初始化。
 * @retval 0 P2P未初始化。
 * @endif
 */
int32_t wifi_p2p_is_enabled(void);

/**
 * @if Eng
 * @brief  Trigger P2P device scanning and search.
 * @param  [in]  sec Scan time.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  触发p2p设备扫描搜索。
 * @param  [in]  sec 表示扫描时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_find(int32_t sec);

/**
 * @if Eng
 * @brief  Stopping P2P Device Scanning.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  停止p2p设备扫描。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_stop_find(void);

/**
 * @if Eng
 * @brief  Stopping P2P Device Connecting.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  停止p2p设备连接。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_connect_cancel(void);

/**
 * @if Eng
 * @brief  Setting P2P Device listening time.
 * @param  [in]  period Listen time.
 * @param  [in]  interval Total time.
 * @retval WIFI_SUCCESS        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  停止p2p设备连接。
 * @param  [in]  period 表示监听的时间。
 * @param  [in]  interval 表示一个周期的总时间。
 * @retval WIFI_SUCCESS 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_listen(uint32_t period, uint32_t interval);

/**
 * @if Eng
 * @brief  Obtains the information about the discovered P2P device.
 * @param  [out]  dev_list List of found P2P devices.
 * @param  [out]  dev_num Maximum number of P2P devices with feedback and actual feedback.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  获取搜索到的p2p设备信息。
 * @param  [out]  dev_list 表示搜索到的p2p设备列表。
 * @param  [out]  dev_num  表示最大反馈与实际反馈的p2p设备数目。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_get_peers_info(p2p_device_stru *dev_list, uint32_t *dev_num);

/**
 * @if Eng
 * @brief  P2P active connection.
 * @param  [in]  p2p_config  Network information of the P2P device to be connected.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  p2p主动连接。
 * @param  [in]  p2p_config  待连接的p2p设备网络信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_connect(const p2p_config_stru *p2p_config);

/**
 * @if Eng
 * @brief  Indicates whether the P2P accepts the connection from the peer end.
 * @param  [in]  p2p_config  Network information of the P2P device to be connected.
 * @param  [in]  assoc  The value 0 indicates that the connection is rejected，1 indicates accepts.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  p2p是否接受对端的连接。
 * @param  [in]  p2p_config  表示待连接的p2p设备网络信息。
 * @param  [in]  assoc       1表示接收连接；0表示拒绝连接。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_connect_accept(const p2p_config_stru *p2p_config, int assoc);

/**
 * @if Eng
 * @brief  P2P disconnection.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  p2p断连。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_disconnect(void);

/**
 * @if Eng
 * @brief  The P2P go command is used to obtain information about connected clients.
 * @param  [in]  client_list Gc information list.
 * @param  [in]  client_num Indicates the maximum number of feedback GCs and the actual number of feedback GCs.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  p2p go获取已连接的client的信息。
 * @param  [in]      client_list 表示反馈的gc信息列表。
 * @param  [in]  client_num  指示反馈gc的最大个数,指示实际反馈gc的个数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t  wifi_p2p_go_get_gc_info(p2p_client_info_stru *client_list, uint32_t *client_num);

/**
 * @if Eng
 * @brief  Setting P2P Device Information.
 * @param  [in]  p2p_dev_set_info P2P setting information.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置p2p设备信息。
 * @param  [in]  p2p_dev_set_info  表示p2p设置信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_set_device_config(const p2p_device_config_stru *p2p_dev_set_info);

/**
 * @if Eng
 * @brief  Obtaining P2P Device Information.
 * @param  [in]  p2p_dev_set_info P2P setting information.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  获取p2p设备信息。
 * @param  [in]  p2p_dev_set_info  表示p2p设置信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_get_device_config(p2p_device_config_stru *p2p_dev_set_info);

/**
 * @if Eng
 * @brief  Displays the P2P connection status.
 * @param  [in]  status P2P connection status information to be fed back.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  显示p2p的连接状态信息。
 * @param  [in]  status  表示待反馈的p2p的连接状态信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_p2p_get_connect_info(p2p_status_info_stru *status);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_P2P_H
