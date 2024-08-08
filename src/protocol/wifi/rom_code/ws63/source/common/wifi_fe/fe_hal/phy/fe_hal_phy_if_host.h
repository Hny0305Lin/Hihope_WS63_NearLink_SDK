/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 校准涉及到的phy功能接口
 * Date: 2022-10-19
 */

#ifndef __FE_HAL_PHY_IF_HOST_H__
#define __FE_HAL_PHY_IF_HOST_H__
#include "hal_device.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define CALI_CHANNEL_NUM13_IDX 12   // 从13信道开始做滤波收窄
#define CALI_CHANNEL_NUM11_IDX 10   // 从11信道开始做滤波收窄
typedef struct {
    osal_u32 reg_addr;
    osal_u32 cfg_val;
    osal_u32 recover_val;
} common_cfg_reg_stru;
// 无委认证: 配置滤波系数和flatness补偿系数
osal_void fe_hal_phy_set_cta_coef(hal_device_stru *device, osal_u8 ch_idx,
    wlan_channel_bandwidth_enum_uint8 bandwidth);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif  // __CALI_PHY_IF_H__