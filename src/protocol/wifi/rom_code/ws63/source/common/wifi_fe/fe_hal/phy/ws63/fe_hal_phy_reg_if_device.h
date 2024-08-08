/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 校准涉及到的phy寄存器操作接口
 * Date: 2022-10-22
 */

#ifndef __FE_HAL_PHY_REG_IF_DEVICE_H__
#define __FE_HAL_PHY_REG_IF_DEVICE_H__

#include "osal_types.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CALI_TXDC_BITS 12
#define CALI_TXDC_OFST_0 BIT_OFFSET_0
#define CALI_TXDC_OFST_12 BIT_OFFSET_12

/* 数字补偿使能定义 */
#define HH503_DIGITAL_COMP_BYPASS 1 /* 数字补偿旁路 */
#define HH503_DIGITAL_COMP_EN 0     /* 数字补偿使能 */

/* 单音信号频率枚举值定义 */
typedef enum {
    HAL_CALI_TONE_FREQ_1P25MHZ = 4, /* 1.25MHz单频 */
    HAL_CALI_TONE_FREQ_2P5MHZ  = 8, /* 2.5MHz单频 */
    HAL_CALI_TONE_FREQ_5MHZ  = 16, /* 5MHz单频 */
    HAL_CALI_TONE_FREQ_7P5MHZ  = 24, /* 7.5MHz单频 */
    HAL_CALI_TONE_FREQ_10MHZ  = 32, /* 10MHz单频 */
    HAL_CALI_TONE_FREQ_12P5MHZ = 40, /* 12.5MHz单频 */
    HAL_CALI_TONE_FREQ_20MHZ  = 64, /* 20MHz单频 */
    HAL_CALI_TONE_FREQ_17P5MHZ = 56, /* 17.5MHz单频 */
    HAL_CALI_TONE_FREQ_22P5MHZ = 72, /* 22.5MHz单频 */
    HAL_CALI_TONE_FREQ_40MHZ  = 128, /* 40MHz单频 */
    HAL_CALI_TONE_FREQ_BUTT
} cali_single_tone_freq_enum;
typedef osal_u8 cali_single_tone_freq_enum_uint8;

/* 单音信号屏蔽选择枚举值定义 */
typedef enum {
    HAL_CALI_TONE_MASK_I_Q, /* 00：输出I路，Q路测试信号给DAC */
    HAL_CALI_TONE_MASK_I,   /* 01：输出I路，屏蔽Q路测试信号给DAC   */
    HAL_CALI_TONE_MASK_Q,   /* 10：输出Q路，屏蔽I路测试信号给DAC  */

    HAL_CALI_TONE_MASK_BUTT /* 11: 无效状态，同00 */
} cali_single_tone_mask_enum;
typedef osal_u8 cali_single_tone_mask_enum_uint8;

/* 单音信号幅度档位枚举值定义 */
typedef enum {
    /* 暂时只支持前7个幅度档位 */
    HAL_CALI_TONE_AMP_NEG21DBFS = 23, /* 0: -21dbfs */
    HAL_CALI_TONE_AMP_NEG18DBFS = 32, /* 1: -18dbfs */
    HAL_CALI_TONE_AMP_NEG15DBFS = 46, /* 2: -15dbfs */
    HAL_CALI_TONE_AMP_NEG12DBFS = 64, /* 3: -12dbfs */
    HAL_CALI_TONE_AMP_NEG9DBFS = 91,  /* 4: -9dbfs */
    HAL_CALI_TONE_AMP_NEG6DBFS = 128, /* 5: -6dbfs */
    HAL_CALI_TONE_AMP_NEG3DBFS = 181, /* 6: -3dbfs */
    HAL_CALI_TONE_AMP_0DBFS = 255,    /* 7: 0dbfs */
    HAL_CALI_TONE_AMP_BUTT
} cali_single_tone_amp_enum;
typedef osal_u8 cali_single_tone_amp_enum_uint8;
typedef enum {
    FE_HAL_PHY_AL_TX_BW_20,            // BW20
    FE_HAL_PHY_AL_TX_BW_SU_ER_106,     // SU ER 106 tone
    FE_HAL_PHY_AL_TX_BW_40 = 4,        // BW40
    FE_HAL_PHY_AL_TX_BW_20_DUP_40 = 5, // BW_20 – Duplicate Legacy in 40MHz
    FE_HAL_PHY_AL_TX_BW_80 = 8,        // BW80
    FE_HAL_PHY_AL_TX_BW_20_DUP_80 = 9  // BW_20 – Duplicate Legacy in 80MHz
} fe_hal_phy_al_tx_bw_enum;
typedef osal_u8 fe_hal_phy_al_tx_bw_enum_uint8;
// pa code
osal_u32 hal_get_rf_pa_code_lut0_cfg_rf_pa_code_lut0(osal_void);
osal_u32 hal_get_rf_pa_code_lut0_cfg_rf_pa_code_lut1(osal_void);
osal_u32 hal_get_rf_pa_code_lut0_cfg_rf_pa_code_lut2(osal_void);
osal_u32 hal_get_rf_pa_code_lut0_cfg_rf_pa_code_lut3(osal_void);
osal_u32 hal_get_rf_pa_code_lut1_cfg_rf_pa_code_lut4(osal_void);
osal_u32 hal_get_rf_pa_code_lut1_cfg_rf_pa_code_lut5(osal_void);
osal_u32 hal_get_rf_pa_code_lut1_cfg_rf_pa_code_lut6(osal_void);
osal_u32 hal_get_rf_pa_code_lut1_cfg_rf_pa_code_lut7(osal_void);
osal_u32 hal_get_rf_pa_code_lut2_cfg_rf_pa_code_lut8(osal_void);
osal_u32 hal_get_rf_pa_code_lut2_cfg_rf_pa_code_lut9(osal_void);
osal_u32 hal_get_rf_pa_code_lut2_cfg_rf_pa_code_lut10(osal_void);
// 二级映射
osal_void hal_set_rf_ppa_code_lut0_cfg_rf_ppa_code_lut0(osal_u32 cfg_rf_ppa_code_lut0);
osal_u32 hal_get_rf_ppa_code_lut0_cfg_rf_ppa_code_lut0(osal_void);
osal_void hal_set_rf_ppa_code_lut0_cfg_rf_ppa_code_lut1(osal_u32 cfg_rf_ppa_code_lut1);
osal_u32 hal_get_rf_ppa_code_lut0_cfg_rf_ppa_code_lut1(osal_void);
osal_void hal_set_rf_ppa_code_lut1_cfg_rf_ppa_code_lut2(osal_u32 cfg_rf_ppa_code_lut2);
osal_u32 hal_get_rf_ppa_code_lut1_cfg_rf_ppa_code_lut2(osal_void);
osal_void hal_set_rf_ppa_code_lut1_cfg_rf_ppa_code_lut3(osal_u32 cfg_rf_ppa_code_lut3);
osal_u32 hal_get_rf_ppa_code_lut1_cfg_rf_ppa_code_lut3(osal_void);
osal_void hal_set_rf_ppa_code_lut2_cfg_rf_ppa_code_lut4(osal_u32 cfg_rf_ppa_code_lut4);
osal_u32 hal_get_rf_ppa_code_lut2_cfg_rf_ppa_code_lut4(osal_void);
osal_void hal_set_rf_ppa_code_lut2_cfg_rf_ppa_code_lut5(osal_u32 cfg_rf_ppa_code_lut5);
osal_u32 hal_get_rf_ppa_code_lut2_cfg_rf_ppa_code_lut5(osal_void);
osal_void hal_set_rf_ppa_code_lut3_cfg_rf_ppa_code_lut6(osal_u32 cfg_rf_ppa_code_lut6);
osal_u32 hal_get_rf_ppa_code_lut3_cfg_rf_ppa_code_lut6(osal_void);
osal_void hal_set_rf_ppa_code_lut3_cfg_rf_ppa_code_lut7(osal_u32 cfg_rf_ppa_code_lut7);
osal_u32 hal_get_rf_ppa_code_lut3_cfg_rf_ppa_code_lut7(osal_void);
osal_void hal_set_rf_ppa_code_lut4_cfg_rf_ppa_code_lut8(osal_u32 cfg_rf_ppa_code_lut8);
osal_u32 hal_get_rf_ppa_code_lut4_cfg_rf_ppa_code_lut8(osal_void);
osal_void hal_set_rf_ppa_code_lut4_cfg_rf_ppa_code_lut9(osal_u32 cfg_rf_ppa_code_lut9);
osal_u32 hal_get_rf_ppa_code_lut4_cfg_rf_ppa_code_lut9(osal_void);
osal_void hal_set_rf_ppa_code_lut5_cfg_rf_ppa_code_lut10(osal_u32 cfg_rf_ppa_code_lut10);
osal_u32 hal_get_rf_ppa_code_lut5_cfg_rf_ppa_code_lut10(osal_void);
osal_void hal_set_cfg_cont_tx_gap_cfg_cont_tx_gap(osal_u32 cont_tx_gap);
osal_u32 hal_get_adc_fs_cfg_adc_fs(osal_void);
osal_u32 hal_get_cal_fft_sa_ctrl1_cfg_cali_iq_fft_size(osal_void);
osal_void hal_set_wcbb_clk_div_3_wp0_sync_data_path_div_num(osal_u32 wp0_sync_data_path_div_num);
// trx iq界限值
osal_void hal_set_wcbb_clk_div_2_wp0_tx_dfe_1dom_div_num(osal_u32 wp0_tx_dfe_1dom_div_num);
osal_void hal_set_cal_txiq_comp_thres_cfg_tpc_group_thres3_ch0(osal_u32 cfg_tpc_group_thres3_ch0);
osal_void hal_set_cal_txiq_comp_thres_cfg_tpc_group_thres2_ch0(osal_u32 cfg_tpc_group_thres2_ch0);
osal_void hal_set_cal_txiq_comp_thres_cfg_tpc_group_thres1_ch0(osal_u32 cfg_tpc_group_thres1_ch0);
osal_void hal_set_rxiq_vga_set_th_cfg_rxiq_mux_mode(osal_u32 cfg_rxiq_mux_mode);
osal_void hal_set_rxiq_vga_set_th_cfg_rxiq_lna_set_th1_ch0(osal_u32 cfg_rxiq_lna_set_th1_ch0);
osal_void hal_set_rxiq_vga_set_th_cfg_rxiq_vga_set_th1_ch0(osal_u32 cfg_rxiq_vga_set_th1_ch0);
// rx dc界限值
osal_void hal_set_rf_line_ctrl_reg_cfg_rxdc_dcoc_vcomp_vga_code_thr(osal_u32 cfg_rxdc_dcoc_vcomp_vga_code_thr);
// TPC ram 读写
osal_u32 hal_get_tpc_ram_access_reg3_rpt_tpc_table_rd_data(osal_void);
osal_void hal_set_wcbb_clk_sel_wp0_fft_clksel_man(osal_u32 wp0_fft_clksel_man);
// dbb_scale寄存器控的配置
osal_void hal_set_digital_scale2_cfg_tx_digital_scale_sel(osal_u32 cfg_tx_digital_scale_sel);
osal_void hal_set_digital_scale2_cfg_tx_digital_scale_val(osal_u32 cfg_tx_digital_scale_val);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif  // __CALI_PHY_REG_IF_H__