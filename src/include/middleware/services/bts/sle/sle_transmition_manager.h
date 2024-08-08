/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: SLE Trans Manager module.
 */

/**
 * @defgroup sle_trans_manager trans manager API
 * @ingroup  SLE
 * @{
 */

#ifndef SLE_TM_SIGNAL_H
#define SLE_TM_SIGNAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of sle link qos state.
 * @else
 * @brief 星闪链路忙闲状态。
 * @endif
 */
typedef enum {
    SLE_QOS_IDLE       = 0x00,    /*!< @if Eng Link state of idle
                                       @else   空闲状态 @endif */
    SLE_QOS_FLOWCTRL   = 0x01,    /*!< @if Eng Link state of flowctrl
                                       @else   流控状态 @endif */
    SLE_QOS_BUSY       = 0x02,    /*!< @if Eng Link state of busy
                                       @else   繁忙状态 @endif */
} sle_link_qos_state_t;

/**
 * @if Eng
 * @brief  Callback invoked when trans data busy.
 * @par Callback invoked when trans data busy.
 * @param [in] conn_id       connection ID.
 * @param [in] link_state    link state.
 * @par Depends:
 * @see  sle_transmission_callbacks_t
 * @else
 * @brief  发送数据繁忙回调函数。
 * @par Description: 发送数据繁忙回调函数。
 * @param [in] conn_id       发送数据繁忙回调信息.
 * @param [in] link_state    链路状态。
 * @par 依赖：
 * @see  sle_transmission_callbacks_t
 * @endif
 */
typedef void (*sle_trans_data_busy_callback)(uint16_t conn_id, sle_link_qos_state_t link_state);

/**
 * @if Eng
 * @brief Struct of SLE transmission manager callback function.
 * @else
 * @brief SLE传输管理回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_trans_data_busy_callback send_data_cb;             /*!< @if Eng Trans data busy callback.
                                                                @else   传输数据繁忙回调函数。 @endif */
} sle_transmission_callbacks_t;


/**
 * @brief  连接管理查询能力比特位参数
 */
typedef struct {
    uint32_t relay_capability : 1;         /*!< @if Eng relay capability
                                                @else   中继能力 @endif */
    uint32_t trans_mode : 1;               /*!< @if Eng trans mode
                                                @else   传输模式 @endif */
    uint32_t measurement_capability : 1;   /*!< @if Eng measurement capability
                                                @else   测量能力 @endif */
    uint32_t access_slb : 1;               /*!< @if Eng access slb
                                                @else   slb接入 @endif */
    uint32_t access_sle : 1;               /*!< @if Eng access sle
                                                @else   sle接入 @endif */
    uint32_t mtu : 1;                      /*!< @if Eng max mtu
                                                @else   最大支持mtu @endif */
    uint32_t mps : 1;                      /*!< @if Eng max mps
                                                @else   最大支持mps @endif */
    uint32_t reverse : 25;                 /*!< @if Eng reverse
                                                @else   保留比特位 @endif */
} sle_transmission_signal_capability_bit_t;

/**
 * @if Eng
 * @brief  Send signal capability request.
 * @par Description: Send signal capability request.
 * @param  [in]  conn_id   Connection id.
 * @param  [in]  param     Capability info.
 * @retval error code
 * @par Depends:
 * @li  sle_common.h
 * @else
 * @brief  发送连接管理能力查询请求。
 * @par Description: 发送连接管理能力查询请求。
 * @param  [in]  conn_id   连接 ID。
 * @param  [in]  param     查询的能力信息。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li  sle_common.h
 * @endif
 */
errcode_t sle_transmission_signal_capability_req(uint16_t conn_id, sle_transmission_signal_capability_bit_t* param);

/**
 * @if Eng
 * @brief  Register SLE transmission manager callbacks.
 * @par Description: Register SLE transmission manager callbacks.
 * @param  [in]  func Callback function.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册SLE传输管理回调函数。
 * @par Description: 注册SLE传输管理回调函数。
 * @param  [in]  func 回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t sle_transmission_register_callbacks(sle_transmission_callbacks_t *func);

#ifdef __cplusplus
}
#endif
#endif /* SLE_TM_SIGNAL */
/**
 * @}
 */