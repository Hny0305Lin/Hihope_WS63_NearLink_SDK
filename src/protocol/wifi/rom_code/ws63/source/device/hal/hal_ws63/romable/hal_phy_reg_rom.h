/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_phy_reg_rom.c.
 */

#ifndef __HAL_PHY_REG_ROM_H__
#define __HAL_PHY_REG_ROM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "osal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_DAQ
/*****************************************************************************
   宏定义
*****************************************************************************/

/*****************************************************************************
  STRUCT定义
*****************************************************************************/

/*****************************************************************************
  函数声明
*****************************************************************************/
void hal_set_sample_cfg_2_cfg_debug_sample_by_len(osal_u32 cfg_debug_sample_by_len);
void hal_set_sample_cfg_0_cfg_debug_sample_end_mode(osal_u32 cfg_debug_sample_end_mode);
void hal_set_sample_cfg_0_cfg_debug_sample_start_mode(osal_u32 cfg_debug_sample_start_mode);
void hal_set_sample_cfg_2_cfg_with_sample_delay_en(osal_u32 cfg_with_sample_delay_en);
void hal_set_sample_cfg_2_cfg_debug_sample_delay(osal_u32 cfg_debug_sample_delay);
void hal_set_sample_cfg_2_cfg_with_mac_tsf_en(osal_u32 cfg_with_mac_tsf_en);
void hal_set_sample_cfg_2_cfg_with_mac_info_en(osal_u32 cfg_with_mac_info_en);
void hal_set_sample_cfg_0_cfg_debug_sample_en(osal_u32 cfg_debug_sample_en);
void hal_set_wlbb_clr_sample_clear_sample_done(osal_u32 clear_sample_done);
osal_void hal_set_sample_cfg_0_cfg_phy_data_sample_cg_bps(osal_u32 cfg_phy_data_sample_cg_bps);
osal_void hal_set_sample_cfg_0_cfg_phy_data_sample_clk_en(osal_u32 cfg_phy_data_sample_clk_en);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_phy_reg_rom.h */
