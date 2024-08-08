/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT PAN module.
 */

/**
 * @defgroup bluetooth_bts_pan PAN API
 * @ingroup  bluetooth
 * @{
 */
#ifndef BTS_PAN_H
#define BTS_PAN_H

#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of pan net state.
 * @else
 * @brief Pan 网络状态
 * @endif
 */
typedef enum {
    PAN_NET_STATE_OFF,                      /*!< @if Eng Offline. @else 离线 @endif */
    PAN_NET_STATE_ON,                       /*!< @if Eng Online. @else 在线 @endif */
} pan_net_state_t;

/**
 * @if Eng
 * @brief Open the pan service.
 * @par Description:
 *            Open the pan service.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief 打开pan服务
 * @par 说明:
 *           打开pan服务
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   成功.
 * @retval Other                失败. 参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li pan_service.h
 */
int pan_service_open(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Close the pan service.
 * @par Description:
 *            Close the pan service.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief 关闭PAN服务
 * @par 说明:
 *           关闭PAN服务
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval #ERRCODE_BT_SUCCESS   成功.
 * @retval Other                失败. 参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li pan_service.h
 */
int pan_service_close(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Send the ethernet.
 * @par Description:
 *            Send the ethernet.
 * @param  [in]  ip_packet A poniter of the ethernet data.
 * @param  [in]  ip_length The length of the ethernet data.
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief 发送网络数据包
 * @par 说明:
 *           发送网络数据包
 * @param  [in]  ip_packet 网络数据包指针
 * @param  [in]  ip_length 网络数据包长度
 * @retval #ERRCODE_BT_SUCCESS   成功.
 * @retval Other                失败. 参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li pan_service.h
 */
int pan_service_write_data(const unsigned char *ip_packet, const unsigned short ip_length);

/**
 * @if Eng
 * @brief Get the net state.
 * @par Description:
 *            Get the net state.
 * @retval #PAN_NET_STATE_OFF   offline
 * @retval #PAN_NET_STATE_ON    online
 * @else
 * @brief  获取网络连接状态
 * @par 说明:
 *           获取网络连接状态
 * @retval #PAN_NET_STATE_OFF   离线
 * @retval #PAN_NET_STATE_ON    在线
 * @endif
 * @par Dependency:
 *            @li pan_service.h
 */
unsigned char pan_service_net_state_get(void);

/**
 * @if Eng
 * @brief The call back of net state change.
 * @par Description:
 *           The call back of net state change.
 * @param  [in]  net_state    See @ref pan_net_state_t
 * @retval No return value. See @ref void
 * @else
 * @brief  pan 网络连接状态通知
 * @par 说明:
 *           注册该回调函数之后，pan服务网络连接状态反馈上层应用。
 * @param  [in]  net_state    参考 @ref pan_net_state_t
 * @retval 无返回值。参考 @ref void
 * @endif
 * @par Dependency:
 *            @li pan_service.h
 */
typedef void (*pan_net_state_callback)(unsigned char net_state);

/**
 * @if Eng
 * @brief The call back of ethernet.
 * @par Description:The call back of ethernet.
 * @param  [in]  packet  a pointer to net data.
 * @param  [in]  packet_length  the length of net data.
 * @retval No return value. See @ref void
 * @else
 * @brief  读取网络数据包通知
 * @par 说明:注册该回调函数之后，读取网络数据包结果反馈上层应用。
 * @param  [in]  packet  指针，指向网络数据包所在内存地址.
 * @param  [in]  packet_length  网络数据包长度.
 * @retval 无返回值。参考 @ref void
 * @endif
 * @par Dependency:
 *            @li pan_service.h
 */
typedef void (*pan_rpt_data_callback)(unsigned char *packet, unsigned short packet_length);

/**
 * @if Eng
 * @brief Struct of pan callback function.
 * @else
 * @brief pan回调接口定义。
 * @endif
 */
typedef struct {
    pan_net_state_callback net_state_cb;
    pan_rpt_data_callback rpt_data_cb;
} pan_callbacks_t;

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of framework.
 * @par Description:
 *           Use this funtion to register callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref pan_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  注册上层应用的回调。
 * @par 说明:
 *           注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref pan_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li pan_service.h
 */
int pan_register_callbacks(pan_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref pan_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册上层应用的回调。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref pan_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
int pan_deregister_callbacks(void);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif /* end of bts_pan.h */