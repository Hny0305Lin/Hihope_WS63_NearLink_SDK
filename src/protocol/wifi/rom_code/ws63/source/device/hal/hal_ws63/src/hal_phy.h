/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_phy.c.
 */

#ifndef HAL_PHY_H
#define HAL_PHY_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_phy_rom.h"
#include "cali_data.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_PHY_H
osal_void hal_initialize_phy(osal_void);
osal_void hh503_phy_set_cfg_pa_mode_code(osal_void);

#define HH503_PHY_TPC_BYPASS_OFFSET        13
#define HH503_PHY_TPC_BYPASS_MASK          0x2000

typedef struct {
    osal_u32 reg_addr;
    osal_u32 adc_80m_val;
    osal_u32 adc_160m_val;
} hh503_phy_adc_reg_stru;

osal_void hal_phy_set_bank1_clk(osal_u8 clk_en);
osal_void hal_read_radar_sensing_exception(osal_void);
osal_void hal_radar_sensing_switch_linectrl(osal_bool is_on);
osal_void hal_radar_sensing_on_linectrl(osal_void);
osal_void hal_radar_sensing_off_linectrl(osal_void);
osal_void hal_radar_sensing_set_tx_dc_val(osal_u16 tx_dc_comp_i, osal_u16 tx_dc_comp_q);
osal_void hal_radar_sensing_set_rx_dc_val(osal_u16 rx_dcoc_i, osal_u16 rx_dcoc_q, osal_u16 rx_dc_comp_i,
    osal_u16 rx_dc_comp_q);
osal_void hal_radar_sensing_set_tx_iq_val(cali_complex_stru *ls_fir);
osal_void hal_radar_sensing_set_rx_iq_val(cali_complex_stru *ls_fir);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_phy.h */
