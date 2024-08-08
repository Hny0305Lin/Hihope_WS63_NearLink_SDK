/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: wifi_alg.h 的头文件
 */


#ifndef SERVICE_WIFI_ALG_H
#define SERVICE_WIFI_ALG_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_service_wifi_alg ALG
 * @ingroup  middleware_service_wifi
 * @{
 */

/**
 * @if Eng
 * @brief  fixed rate value.
 * @else
 * @brief  固定速率值。
 * @endif
 */
typedef struct {
    uint32_t rate_value;    /*!< @if Eng fixed rate value.
                                     @else  固定速率值。  @endif */
} alg_param_stru;

/**
 * @if Eng
 * @brief  Set Fixed Rate for TX packets.
 *
 * @param  [in]  auto_rate Auto rate or fixed rate.
 * @param  [in]  alg_param Rate index.
 *
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置TX方向发送报文的速率模式。
 *
 * @param  [in]  auto_rate 表示速率使用固定模式或自动模式。
 * @param  [in]  alg_param 速率等级。
 *
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
errcode_t wifi_set_fixed_tx_rate(unsigned char auto_rate, alg_param_stru *alg_param);

/**
 * @if Eng
 * @brief  Get current best rate for specified user.
 * @param  [in]  mac           Mac address.
 * @param  [in]  mac_len       Mac address len.
 * @param  [in]  tx_best_rate obtained best rate.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  获取用户当前的最优速率。
 * @param  [in]  mac           用户的mac地址。
 * @param  [in]  mac_len       mac地址长度。
 * @param  [in]  tx_best_rate 获取的用户最佳速率。
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
errcode_t wifi_get_negotiated_rate(const uint8_t *mac, int32_t mac_len, uint32_t *tx_best_rate);

/**
 * @if Eng
 * @brief  Set TPC mode.
 * @param  [in]  tpc_value TPC mode to be set: 0-off_mode, 1-promote_mode, 2-auto_mode.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置TPC模式。
 * @param  [in]  tpc_value 设置的TPC模式：0-关闭动态调整，1-功率提升模式，2-自动调整模式。
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
errcode_t wifi_set_tpc_mode(uint32_t tpc_value);

/**
 * @if Eng
 * @brief  Set RTS mode.
 * @param  [in]  mode       RTS mode to be set: 0-auto_mode, 1-threshold_mode, 2-off_mode.
 * @param  [in]  pkt_length The length of the packet sent, used when rts is in the threshold mode.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置RTS模式。
 * @param  [in]  mode       设置的RTS模式：0-自动模式，1-超过配置报文长度发送RTS 未超过不发，2-不发RTS。
 * @param  [in]  pkt_length RTS模式为1时配置的RTS报文长度门限。
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
errcode_t wifi_set_rts_mode(uint8_t mode, uint16_t pkt_length);

/**
 * @if Eng
 * @brief  Set CCA threshold.
 * @param  [in]  mode      Whether to set the CCA threshold for 20M or for 40M, 0-20M, 1-40M.
 * @param  [in]  threshold The value of the CCA threshold to be set.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置CCA门限。
 * @param  [in]  mode      为20M或40M设置CCA门限，0-20M，1-40M。
 * @param  [in]  threshold CCA门限值。
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
errcode_t wifi_set_cca_threshold(uint8_t mode, int8_t threshold);

#ifdef _PRE_WLAN_FEATURE_INTRF_MODE
/**
 * @if Eng
 * @brief  Set the INTRF mode.
 * @param  [in]  ifname Device name.
 * @param  [in]  enable Enable or disable the intrf mode.
 * @param  [in]  flag   Intrf mode config flag.
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  设置抗干扰模式。
 * @param  [in]  ifname 设备名。
 * @param  [in]  enable 是否使能抗干扰模式。
 * @param  [in]  flag   抗干扰措施使能bit配置。
 * @retval ERRCODE_SUCC  成功。
 * @retval Other         失败。
 * @endif
 */
errcode_t wifi_set_intrf_mode(const char *ifname, uint8_t enable, uint16_t flag);
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_ALG_H
