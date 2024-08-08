/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wifi_device.h 的头文件
 */


#ifndef SERVICE_WIFI_DEVICE_C_H
#define SERVICE_WIFI_DEVICE_C_H

#include "wifi_event.h"
#include "station_info.h"
#include "wifi_scan_info.h"
#include "errcode.h"
#include "wifi_linked_info.h"
#include "wifi_device_config.h"
#include "plat_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_service_wifi_device WiFi Device
 * @ingroup  middleware_service_wifi
 * @{
 */

/**
 * @if Eng
 * @brief  Wifi initialize.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Error code.
 * @else
 * @brief  Wifi初始化。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_init(void);

/**
 * @if Eng
 * @brief  Wifi de-initialize.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Error code.
 * @else
 * @brief  Wifi去初始化。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_deinit(void);

/**
 * @if Eng
 * @brief  Get wifi initialize status.
 * @retval 1 Wifi is initialized.
 * @retval 0 Wifi is not initialized.
 * @else
 * @brief  获取WiFi初始化状态。
 * @retval 1 Wifi已经初始化。
 * @retval 0 Wifi没有初始化。
 * @endif
 */
int32_t wifi_is_wifi_inited(void);

/**
 * @if Eng
 * @brief  Start WiFi station.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  开启STA。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_enable(void);

/**
 * @if Eng
 * @brief  Close wifi station.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  关闭STA。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_disable(void);

/**
 * @if Eng
 * @brief  Get sta enable status.
 * @retval 1 sta is initialized.
 * @retval 0 sta is not initialized.
 * @else
 * @brief  获取sta使能状态。
 * @retval 1 sta已经使能。
 * @retval 0 sta未使能。
 * @endif
 */
int32_t wifi_is_sta_enabled(void);

/**
 * @if Eng
 * @brief  Get dev status.
 * @retval NULL dev is not initialized, otherwise dev is initialized.
 * @else
 * @brief  获取dev使能状态。
 * @retval NULL dev未使能，否则dev使能。
 * @endif
 */
wifi_dev_t *wifi_get_dev(wifi_iftype_t iftype);

/**
 * @if Eng
 * @brief  Set Sta protocol mode.
 * @param  [in]  mode Protocol_mode_enum.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置Sta的协议模式。
 * @param  [in]  mode 协议模式。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_set_protocol_mode(protocol_mode_enum mode);

/**
 * @if Eng
 * @brief  Get Sta protocol mode.
 * @retval protocol_mode_enum Sta Protocol mode.
 * @else
 * @brief  获取Sta的协议模式。
 * @retval protocol_mode_enum Sta协议模式。
 * @endif
 */
protocol_mode_enum wifi_sta_get_protocol_mode(void);

/**
 * @if Eng
 * @brief  Start sta basic scanning in all channels.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  station进行全信道基础扫描。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_scan(void);

/**
 * @if Eng
 * @brief  Start station scanning with specified parameter.
 * @param  [in]  scan_param Parameters of scan.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  station执行带特定参数的扫描。
 * @param  [in]  scan_param 扫描网络参数设置。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_scan_advance(const wifi_scan_params_stru *scan_param);

/**
 * @if Eng
 * @brief  Set sta scan policy.
 * @param  [in]  iftype Interface type.
 * @param  [in]  scan_strategy scan_strategy parament set.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  station设置扫描参数。
 * @param  [in]  iftype 接口类型。
 * @param  [in]  scan_strategy 扫描策略参数配置。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_set_scan_policy(wifi_if_type_enum iftype, wifi_scan_strategy_stru *scan_strategy);

/**
 * @if Eng
 * @brief  Start station scanning with specified parameter.
 * @param  [in]  scan_param Parameters of scan.
 * @param  [in]  cb Callback of scan result.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  station执行带特定参数的原始扫描（不经过wpa，回调由用户指定）。
 * @param  [in]  scan_param 扫描网络参数设置。
 * @param  [in]  cb 扫描完成函数回调，入参为ext_wifi_scan_result_stru。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_raw_scan(wifi_scan_params_stru *scan_param, wifi_scan_no_save_cb cb);

/**
 * @if Eng
 * @brief  Stop sta scan.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  强制停止Sta全信道扫描。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_scan_stop(void);

/**
 * @if Eng
 * @brief  Get sta scan results.
 * @param  [out]  result Scan result.
 * @param  [out]  size Number of scan result.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  获取station扫描结果。
 * @param  [out]  result 扫描的结果。
 * @param  [out]  size 扫描到的网络数目。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_get_scan_info(wifi_scan_info_stru *result, uint32_t *size);

/**
 * @if Eng
 * @brief  clear sta scan results.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  清空station扫描结果。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_scan_result_clear(void);

/**
 * @if Eng
 * @brief  wifi set channel.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  切换信道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_set_channel(wifi_if_type_enum iftype, int32_t channel);

/**
 * @if Eng
 * @brief  wifi get channel.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  获取信道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_get_channel(wifi_if_type_enum iftype, int32_t *channel);

#ifdef CONFIG_WNM
/**
 * @if Eng
 * @brief  send bss query
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  发送bss query报文。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_wnm_bss_query(int32_t reason_code, int32_t candidate_list);
#endif

/**
 * @if Eng
 * @brief  sta start connect.
 * @param  [in]  config Connect parameters of network.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  station进行连接网络。
 * @param  [in]  config 连接网络参数设置。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_connect(const wifi_sta_config_stru *config);

/**
 * @if Eng
 * @brief  Disconnect from network.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  station断开相连的网络。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_disconnect(void);

/**
 * @if Eng
 * @brief  Get status of sta.
 * @param  [out]  result Connection status.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  获取station连接的网络状态。
 * @param  [out]  result 连接状态。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。
 * @endif
 */
errcode_t wifi_sta_get_ap_info(wifi_linked_info_stru *result);

/**
 * @if Eng
 * @brief  Set reconnect policy.
 * @param  [in]  enable Enable reconnect.
 * @param  [in]  seconds Reconnect timeout in seconds for once, range:[2-65535].
 * @param  [in]  period Reconnect period in seconds, range:[1-65535].
 * @param  [in]  max_try_count Max reconnect try count number，range:[1-65535].
 * @retval EXT_WIFI_OK        Execute successfully.
 * @retval EXT_WIFI_FAIL      Execute failed.
 * @else
 * @brief  station设置重新连接网络机制。
 * @param  [in]  enable 使能重连网络参数。
 * @param  [in]  seconds 单次重连超时时间，取值[2-65535]。
 * @param  [in]  period 重连间隔周期，取值[1-65535]。
 * @param  [in]  max_try_count 最大重连次数，取值[1-65535]。
 * @retval EXT_WIFI_OK   成功。
 * @retval EXT_WIFI_FAIL 失败。
 * @endif
 */
errcode_t wifi_sta_set_reconnect_policy(int32_t enable, uint32_t seconds,
    uint32_t period, uint32_t max_try_count);

/**
 * @if Eng
 * @brief  Config pmf settings of sta.
 * @retval EXT_WIFI_OK        Execute successfully.
 * @retval EXT_WIFI_FAIL      Execute failed.
 * @else
 * @brief  配置station的pmf。
 * @retval EXT_WIFI_OK   成功。
 * @retval EXT_WIFI_FAIL 失败。
 * @endif
 */
errcode_t wifi_sta_set_pmf_mode(wifi_pmf_option_enum pmf);

/**
 * @if Eng
 * @brief  Get pmf settings of sta.
 * @retval pmf enum value                       Execute successfully.
 * @retval WIFI_MGMT_FRAME_PROTECTION_BUTT      Execute failed.
 * @else
 * @brief  获取station的pmf设置。
 * @retval pmf enum value                  成功。
 * @retval WIFI_MGMT_FRAME_PROTECTION_BUTT 失败。
 * @endif
 */
wifi_pmf_option_enum wifi_sta_get_pmf_mode(void);

/**
 * @if Eng
 * @brief  Get Status Codes for Authentication and Association Frames.
 * @retval Status Codes value   Execute successfully.
 * @retval -1                   Execute failed.
 * @else
 * @brief  查询Authentication与Association帧status code。
 * @retval Status Codes value   成功。
 * @retval -1                   失败。
 * @endif
 */
int16_t wifi_sta_get_connect_status_code(void);

/**
 * @if Eng
 * @brief  Register receive callback for management frame.
 * @param  [in]  data_cb Callback function pointer.
 * @param  [in]  mode Mode of receiving management frame.
 * @retval EXT_WIFI_OK        Execute successfully.
 * @retval EXT_WIFI_FAIL      Execute failed.
 * @else
 * @brief  注册管理帧收包回调函数。
 * @param  [in]  data_cb 管理帧上报回调函数。
 * @param  [in]  mode 上报管理帧模式。
 * @retval EXT_WIFI_OK        成功。
 * @retval EXT_WIFI_FAIL      失败。
 * @endif
 */
errcode_t wifi_set_mgmt_frame_rx_cb(wifi_rx_mgmt_cb data_cb, uint8_t mode);

/**
 * @if Eng
 * @brief  Set monitor mode.
 * @param  [in]  iftype Interface type.
 * @param  [in]  enable Enable(1) or disable(0).
 * @param  [in]  filter Filtered frame type enum.
 * @retval EXT_WIFI_OK        Execute successfully.
 * @retval EXT_WIFI_FAIL      Execute failed.
 * @else
 * @brief  设置混杂模式。
 * @param  [in]  iftype 接口类型。
 * @param  [in]  enable 开启/关闭。
 * @param  [in]  filter 过滤列表。
 * @retval EXT_WIFI_OK   成功。
 * @retval EXT_WIFI_FAIL 失败。
 * @endif
 */
errcode_t wifi_set_promis_mode(wifi_if_type_enum iftype, int32_t enable, const wifi_ptype_filter_stru *filter);

/**
 * @if Eng
 * @brief  Register receive callback in monitor mode.
 * @param  [in]  data_cb Callback function pointer.
 * @retval EXT_WIFI_OK        Execute successfully.
 * @retval EXT_WIFI_FAIL      Execute failed.
 * @else
 * @brief  注册混杂模式的收包回调函数。
 * @param  [in]  data_cb 混杂模式回调函数。
 * @retval EXT_WIFI_OK        成功。
 * @retval EXT_WIFI_FAIL      失败。
 * @endif
 */
errcode_t wifi_set_promis_rx_pkt_cb(wifi_promis_cb data_cb);

/**
 * @if Eng
 * @brief  Start fast connect.
 * @param  [in]  fast_request  Fast connect parameters.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  station进行快速连接网络。
 * @param  [in]  fast_request  快速连接网络参数。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_sta_fast_connect(const wifi_fast_connect_stru *fast_request);

/**
 * @if Eng
 * @brief  Event processing interface for app registration.
 * @param  [in]  event Callback function registered by the app.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Error code.
 * @else
 * @brief  APP注册的事件处理接口。
 * @param  [in]  event APP注册的回调函数。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_register_event_cb(const wifi_event_stru *event);

/**
 * @if Eng
 * @brief  Event processing interface for app unregistration.
 * @param  [in]  event Callback function to be canceled.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Error code.
 * @else
 * @brief  APP去注册的事件处理接口。
 * @param  [in]  event 待撤销的回调函数。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_unregister_event_cb(const wifi_event_stru *event);

/**
 * @if Eng
 * @brief  Add user IEs to management frame.
 * @param  [in]  iftype Interface type.
 * @param  [in]  ie_index IE index, four indexes are available. That is,
 *                        a maximum of four user-defined IEs can be inserted or deleted in a single management frame.
 * @param  [in]  frame_type_bitmap Types of frames that can be inserted into the IE,
 *                                 bit0 is beacon, bit1 is probe request, bit2 is probe response.
 * @param  [in]  ie User IE value.
 * @param  [in]  ie_len User IE length.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  在管理帧中添加用户IE字段。
 * @param  [in]  iftype 接口类型。
 * @param  [in]  ie_index ie索引,有四个索引可供选择,即在单个管理帧中最多支持4个客户自定义IE的插入或删除。
 * @param  [in]  frame_type_bitmap 可插入IE的帧类型，bit0代表beacon,bit1代表probe request,bit2代表probe response。
 * @param  [in]  ie 用户IE字段内容。
 * @param  [in]  ie_len 用户IE字段内容长度。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_set_app_ie(wifi_if_type_enum iftype, ie_index_enmu ie_index, uint8_t frame_type_bitmap,
    const uint8_t *ie, uint16_t ie_len);

/**
 * @if Eng
 * @brief  Delete user IEs to management frame.
 * @param  [in]  iftype Interface type.
 * @param  [in]  ie_index IE index, four indexes are available. That is,
 *                        a maximum of four user-defined IEs can be inserted or deleted in a single management frame.
 * @param  [in]  frame_type_bitmap Types of frames that can be inserted into the IE,
 *                                 bit0 is beacon, bit1 is probe request, bit2 is probe response.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  在管理帧中删除用户IE字段。
 * @param  [in]  iftype 接口类型。
 * @param  [in]  ie_index ie索引,有四个索引可供选择,即在单个管理帧中最多支持4个客户自定义IE的插入或删除。
 * @param  [in]  frame_type_bitmap 可插入IE的帧类型，bit0代表beacon,bit1代表probe request,bit2代表probe response。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_del_app_ie(wifi_if_type_enum iftype, ie_index_enmu ie_index, uint8_t frame_type_bitmap);

#ifdef _PRE_WLAN_FEATURE_WOW_OFFLOAD
/**
 * @if Eng
 * @brief  Set wow pattern mode.
 * @param  [in]  type wow_pattern_type, Wow pattern.
 * @param  [in]  index Location index.
 * @param  [in]  pattern Hexadecimal TCP/UDP pattern data.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置WoW Pattern模式。
 * @param  [in]  type Wow Pattern模式。
 * @param  [in]  index 位置索引。
 * @param  [in]  pattern 16进制的TCP/UDP pattern数据。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_set_wow_pattern(int32_t type, uint8_t index, int8_t *pattern);

/**
 * @if Eng
 * @brief  WoW sleep enable configuration 1)The wow sleep can be enabled only in STA2.
 *         2)The value 0 indicates that the WoW sleep is disabled,
 *         and the value 1 indicates that the WoW sleep is enabled.
 * @param  [in]  en Enables or disables the function.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  WoW休眠使能配置 1)仅在STA下才能wow休眠使能 2)0代表WoW休眠去使能，1代表WoW休眠使能。
 * @param  [in]  en 使能和去使能该功能。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_set_wow_sleep_mode(uint8_t en);
#endif

/**
 * @if Eng
 * @brief  Start CSI report.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  开启CSI上报。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_csi_start(void);

/**
 * @if Eng
 * @brief  Close CSI report.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  关闭CSI上报。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_csi_stop(void);

/**
 * @if Eng
 * @brief  Config CSI.
 * @param  [in]  ifname Interface which enable CSI, wlan0 or ap0.
 * @param  [in]  config CSI's configuration.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  配置CSI。
 * @param  [in]  ifname 使能CSI，wlan0，ap的接口。
 * @param  [in]  config CSI配置。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_set_csi_config(const int8_t *ifname, const csi_config_stru *config);

/**
 * @if Eng
 * @brief  Register report callback of CSI.
 * @param  [in]  data_cb Callback pointer.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Error code.
 * @else
 * @brief  注册CSI数据上报回调函数。
 * @param  [in]  data_cb 回调函数。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_register_csi_report_cb(wifi_csi_data_cb data_cb);

/**
 * @if Eng
 * @brief  User-defined packets are sent. The packets must be encapsulated according to the 802.11 protocol format.
 * @param  [in]  iftype Interface type.
 * @param  [in]  data Content of the frame to be sent.
 * @param  [in]  len Length of the frame to be sent.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  发送用户定制报文，报文须按照802.11协议格式封装。
 * @param  [in]  iftype 接口类型。
 * @param  [in]  data 待发送帧的内容。
 * @param  [in]  len 待发送的帧长度。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_send_custom_pkt(const wifi_if_type_enum iftype, const uint8_t *data, uint32_t len);

/**
 * @if Eng
 * @brief  Set maximum software retransmission count for data and mgmt packets.
 * @param  [in]  type Frame type.
 * @param  [in]  limit Max software retry count to set.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置数据帧和管理帧的最大软件重传次数。
 * @param  [in]  type 帧类型。
 * @param  [in]  limit 需要设置的软件最大重传次数。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_set_pkt_retry_policy(uint8_t type, uint8_t limit);

/**
 * @if Eng
 * @brief  Reset the MAC and PHY interfaces to solve the problem of MAC and PHY suspension..
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  复位MAC和PHY接口，解决MAC，PHY挂死问题。
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_reset_mac_phy(void);

/**
 * @if Eng
 * @brief  Set Linkloss paras.
 * @param  [in]  linkloss_paras Set linkloss parameters.
 * @retval EXT_ERR_SUCCESS        Execute successfully.
 * @retval Other                  Error code.
 * @else
 * @brief  设置Linkloss的参数。
 * @param  [in]  linkloss_paras 设置linkloss相关参数。
 * @retval EXT_ERR_SUCCESS        成功。
 * @retval Other                  失败。
 * @endif
 */
errcode_t wifi_set_linkloss_config(linkloss_paras_stru *linkloss_paras);

/**
 * @if Eng
 * @brief  Set base mac addr.
 * @param  [in]  mac_addr mac addr ptr.
 * @param  [in]  mac_len mac len.
 * @retval EXT_ERR_SUCCESS        Execute successfully.
 * @retval Other                  Error code.
 * @else
 * @brief  设置基础mac地址。
 * @param  [in]  mac_addr mac地址指针.
 * @param  [in]  mac_len mac地址长度.
 * @retval EXT_ERR_SUCCESS        成功。
 * @retval Other                  失败。
 * @endif
 */
errcode_t wifi_set_base_mac_addr(const int8_t *mac_addr, uint8_t mac_len);

/**
 * @if Eng
 * @brief  Get base mac addr.
 * @param  [in]  mac_addr mac addr ptr.
 * @param  [in]  mac_len mac len.
 * @retval EXT_ERR_SUCCESS        Execute successfully.
 * @retval Other                  Error code.
 * @else
 * @brief  获取基础mac地址。
 * @param  [in]  mac_addr mac地址指针.
 * @param  [in]  mac_len mac地址长度.
 * @retval EXT_ERR_SUCCESS        成功。
 * @retval Other                  失败。
 * @endif
 */
errcode_t wifi_get_base_mac_addr(int8_t *mac_addr, uint8_t mac_len);

/**
 * @if Eng
 * @brief  Set softAP mac addr.
 * @param  [in]  mac_addr mac addr ptr.
 * @param  [in]  mac_len mac len.
 * @retval EXT_ERR_SUCCESS        Execute successfully.
 * @retval Other                  Error code.
 * @else
 * @brief  设置softap mac地址。
 * @param  [in]  mac_addr mac地址指针.
 * @param  [in]  mac_len mac地址长度.
 * @retval EXT_ERR_SUCCESS        成功。
 * @retval Other                  失败。
 * @endif
 */
errcode_t wifi_softap_set_mac_addr(const int8_t *mac_addr, uint8_t mac_len);

/**
 * @if Eng
 * @brief  Get softAP mac addr.
 * @param  [in]  mac_addr mac addr ptr.
 * @param  [in]  mac_len mac len.
 * @retval EXT_ERR_SUCCESS        Execute successfully.
 * @retval Other                  Error code.
 * @else
 * @brief  获取softap mac地址。
 * @param  [in]  mac_addr mac地址指针.
 * @param  [in]  mac_len mac地址长度.
 * @retval EXT_ERR_SUCCESS        成功。
 * @retval Other                  失败。
 * @endif
 */
errcode_t wifi_softap_get_mac_addr(int8_t *mac_addr, uint8_t mac_len);

/**
 * @if Eng
 * @brief  define mac derivation ptr.
 * @param  [in]  origin_mac input mac addr ptr.
 * @param  [in]  num input mac len.
 * @param  [in]  type derivation type,2:sta,3:softAP,7~10:p2p..
 * @param  [in]  output_mac outputput mac addr ptr.
 * @param  [in]  out_put_num output mac len.
 * @retval EXT_ERR_SUCCESS        Execute successfully.
 * @retval Other                  Error code.
 * @else
 * @brief  mac派生方法指针定义。
 * @param  [in]  origin_mac 输入mac地址。
 * @param  [in]  num 输入mac地址长度。
 * @param  [in]  type 派生类型 type,2:sta,3:softAP,7~10:p2p。
 * @param  [in]  output_mac 输出mac地址。
 * @param  [in]  out_put_num 输出mac地址长度。
 * @retval EXT_ERR_SUCCESS        成功。
 * @retval Other                  失败。
 * @endif
 */
typedef unsigned int(*wifi_mac_derivation_ptr)(unsigned char *origin_mac, unsigned char num, unsigned char type,
    unsigned char *output_mac, unsigned char out_put_num);

/**
 * @if Eng
 * @brief  Set mac derivation method callback.
 * @param  [in]  ptr callback ptr.
 * @retval EXT_ERR_SUCCESS        Execute successfully.
 * @retval Other                  Error code.
 * @else
 * @brief  设置mac派生策略。
 * @param  [in]  ptr 派生方法指针.
 * @retval EXT_ERR_SUCCESS        成功。
 * @retval Other                  失败。
 * @endif
 */
errcode_t wifi_set_mac_derivation_ptr(wifi_mac_derivation_ptr ptr);

/**
 * @if Eng
 * @brief  Set the low start-up current mode.
 * @param  [in]  flag Mode setting.
 * @retval EXT_ERR_SUCCESS     Execute successfully.
 * @retval Other               Error code.
 * @else
 * @brief  设置低启动电流模式。
 * @param  [in]  flag 模式设置。
 * @retval EXT_ERR_SUCCESS     成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_set_low_current_boot_mode(uint8_t flag);

/**
 * @if Eng
 * @brief  Get country code.
 * @param  [in]  country_code Country_code.
 * @param  [in]  len Mac addr len.
 * @retval WIFI_SUCCESS        Execute successfully.
 * @retval Other               Error code.
 * @else
 * @brief  获取国家码。
 * @param  [in]  country_code 国家码。
 * @param  [in]  len 国家码数组长度。
 * @retval WIFI_SUCCESS        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_get_country_code(int8_t *country_code, uint8_t *len);

/**
 * @if Eng
 * @brief  Set country code.
 * @param  [in]  country_code Country_code.
 * @param  [in]  len Mac addr len.
 * @retval WIFI_SUCCESS        Execute successfully.
 * @retval Other               Error code.
 * @else
 * @brief  设置国家码。
 * @param  [in]  country_code 国家码。
 * @param  [in]  len 国家码数组长度。
 * @retval WIFI_SUCCESS        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_set_country_code(const int8_t* country_code, uint8_t len);

/**
 * @if Eng
 * @brief  Set pm mode.
 * @param  [in]  ps_switch ps_switch.
 * @retval WIFI_SUCCESS        Execute successfully.
 * @retval Other               Error code.
 * @else
 * @brief  设置低功耗模式。
 * @param  [in]  ps_switch 低功耗模式。
 * @retval WIFI_SUCCESS        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_sta_set_pm(uint8_t ps_switch);

/**
 * @if Eng
 * @brief  Set sdp mode.
 * @param  [in]  iftype Iftype.
 * @param  [in]  enable Enable.
 * @param  [in]  ratio Ratio.
 * @retval WIFI_SUCCESS        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置sdp模式。
 * @param  [in]  iftype 接口类型.
 * @param  [in]  enable 使能开关.
 * @param  [in]  ratio 比例.
 * @retval WIFI_SUCCESS        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_set_sdp_mode(wifi_if_type_enum iftype, int32_t enable, int32_t ratio);

/**
 * @if Eng
 * @brief  Set sdp subscribe.
 * @param  [in]  iftype Iftype.
 * @param  [in]  sdp_subscribe Sdp_subscribe.
 * @param  [in]  local_handle Local_handle.
 * @retval WIFI_SUCCESS        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置sdp subscribe。
 * @param  [in]  iftype 接口类型.
 * @param  [in]  sdp_subscribe sdp订阅.
 * @param  [in]  local_handle 当前句柄.
 * @retval WIFI_SUCCESS        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t wifi_set_sdp_subscribe(wifi_if_type_enum iftype, char *sdp_subscribe, int32_t local_handle);

/**
* @if Eng
* @brief  WPS pbc connection.
* @param  [in]  bssid The bssid parameter can be set to a MAC address or null.
* @retval ERRCODE_SUCC        Execute successfully.
* @retval Other               Execute failed.
* @else
* @brief  wps pbc连接。
* @param  [in]  bssid bssid可以指定mac或者填NULL。
* @retval ERRCODE_SUCC  成功。
* @retval Other         失败。
* @endif
*/
errcode_t wifi_sta_wps_pbc(uint8_t *bssid);

/**
* @if Eng
* @brief  WPS_PIN connection.
* @param  [in]  pin The PIN code in WPS is only a decimal number and contains 8 bytes.
* @param  [in]  bssid The bssid parameter can be set to a MAC address or null.
* @retval ERRCODE_SUCC        Execute successfully.
* @retval Other               Execute failed.
* @else
* @brief  wps_pin 连接。
* @param  [in]  pin WPS中pin码仅限十进制，长度为8 Bytes。
* @param  [in]  bssid bssid可以指定mac或者填NULL。
* @retval ERRCODE_SUCC  成功。
* @retval Other         失败。
* @endif
*/
errcode_t wifi_sta_wps_pin(int8_t *pin, uint8_t *bssid);

/**
* @if Eng
* @brief  WPS_PIN connection.
* @param  [in]  pin Pin code to be obtained.
* @param  [in]  len Length of the PIN code.
* @retval ERRCODE_SUCC        Execute successfully.
* @retval Other               Execute failed.
* @else
* @brief  wps_pin 连接。
* @param  [in]  pin 待获取pin码。
* @param  [in]  len pin码的长度。
* @retval ERRCODE_SUCC  成功。
* @retval Other         失败。
* @endif
*/
errcode_t wifi_sta_get_wps_pin(int8_t* pin, uint32_t len);

/**
* @if Eng
* @brief  Set PSD mode.
* @param  [in]  psd_option Type psd option parameters.
* @retval ERRCODE_SUCC        Execute successfully.
* @retval Other               Execute failed.
* @else
* @brief  设置PSD模式。
* @param  [in]  psd_option 表示psd参数。
* @retval ERRCODE_SUCC  成功。
* @retval Other         失败。
* @endif
*/
errcode_t wifi_set_psd_mode(ext_psd_option_param *psd_option);

/**
* @if Eng
* @brief  Set PSD recallback interface.
* @param  [in]  data_cb Callback function pointer.
* @retval ERRCODE_SUCC        Execute successfully.
* @retval Other               Execute failed.
* @else
* @brief  设置PSD回调接口。
* @param  [in]  data_cb psd回调函数。
* @retval ERRCODE_SUCC  成功。
* @retval Other         失败。
* @endif
*/
errcode_t wifi_set_psd_cb(wifi_psd_cb data_cb);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_DEVICE_C_H
