/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_alg_txbf.c.
 */

#ifndef HAL_ALG_TXBF_ROM_H
#define HAL_ALG_TXBF_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_common_ops_rom.h"
#include "hh503_phy_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
   宏定义
*****************************************************************************/

/*****************************************************************************
  STRUCT定义
*****************************************************************************/

/*****************************************************************************
  函数声明
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_TXBF
#ifdef _PRE_WLAN_SUPPORT_VHT
osal_void hal_irq_rx_txbf_frame_process(hal_to_dmac_device_stru *hal_device, osal_u32 *base_dscr,
    osal_u8 queue_id);
osal_void hal_set_sta_membership_status_63_32(osal_u32 value);
osal_void hal_set_sta_membership_status_31_0(osal_u32 value);
osal_void hal_set_sta_user_p_63_48(osal_u32 value);
osal_void hal_set_sta_user_p_47_32(osal_u32 value);
osal_void hal_set_sta_user_p_31_16(osal_u32 value);
osal_void hal_set_sta_user_p_15_0(osal_u32 value);
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_alg_txbf_rom.h */
