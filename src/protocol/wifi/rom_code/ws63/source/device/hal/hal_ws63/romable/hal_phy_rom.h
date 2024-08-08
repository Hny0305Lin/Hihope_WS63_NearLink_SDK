/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_phy_rom.c.
 */

#ifndef HAL_PHY_ROM_H
#define HAL_PHY_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_common_ops_rom.h"
#include "hal_ext_if_rom.h"
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
typedef enum {
    PHY_DAC_160M = 0,
    PHY_DAC_320M = 1,
    PHY_DAC_640M = 2,
    PHY_DAC_960M = 3,

    PHY_DAC_BUTT
} hh503_phy_dac_enum;

typedef enum {
    PHY_DAC_80M = 0,
    PHY_ADC_160M = 1,
    PHY_ADC_320M = 2,
    PHY_ADC_640M = 3,

    PHY_ADC_BUTT
} hh503_phy_adc_enum;

typedef enum {
    PHY_DAC_160M_NUM = 160,
    PHY_DAC_320M_NUM = 320,
    PHY_DAC_640M_NUM = 640,
    PHY_DAC_960M_NUM = 960,

    PHY_DAC_NUM_BUTT
} hh503_phy_dac_num_enum;

typedef enum {
    PHY_ADC_80M_NUM = 80,
    PHY_ADC_160M_NUM = 160,
    PHY_ADC_320M_NUM = 320,
    PHY_ADC_640M_NUM = 640,

    PHY_ADC_NUM_BUTT
} hh503_phy_adc_num_enum;

/* 逻辑通道序号 */
typedef enum {
    PHY_CH_0 = 0,
    PHY_CH_1 = 1,
    PHY_CH_2 = 2,
    PHY_CH_3 = 3,
    PHY_CH_BUTT,
} phy_ch_enum;
typedef osal_u8 phy_ch_enum_unit8;

/* 物理通道序号 */
typedef enum {
    RF_CH_0 = 0,
    RF_CH_1 = 1,
    RF_CH_2 = 2,
    RF_CH_3 = 3,
    RF_CH_BUTT,
} rf_ch_enum;
typedef osal_u8 rf_ch_enum_unit8;
/*****************************************************************************
  带宽枚举对应phy寄存器的值
*****************************************************************************/
typedef struct {
    wlan_channel_bandwidth_enum_uint8 en_bandwidth; /* 带宽枚举 */
    osal_u8 bandwidth;                             /* 带宽，0->20M 1->40M 2->80M */
    osal_s8 sec20_offset;                           /* 从20偏移 */
    osal_s8 sec40_offset;                           /* 从40偏移 */
    osal_s8 sec80_offset;                           /* 从80偏移 */
} hh503_phy_bandwidth_stru;

/*****************************************************************************
  函数声明
*****************************************************************************/
osal_void hh503_set_phy_channel_num(osal_u8 channel_num);
osal_void hh503_set_phy_channel_freq_factor(osal_u32 freq_factor);
osal_void hh503_set_phy_primary_channel(hal_to_dmac_device_stru *hal_device,
    wlan_channel_band_enum_uint8 band);
osal_void hh503_device_enable_ftm(osal_void);
osal_void hh503_device_disable_ftm(osal_void);
#ifdef _PRE_WLAN_FEATURE_DFS
    osal_void hh503_enable_radar_det_cb(osal_u8 enable);
#endif

#ifdef _PRE_WLAN_FEATURE_INTRF_MODE
osal_void hal_phy_do_sgl_tone_notch_coef(osal_u32 notch_filter_coef1, osal_u32 notch_filter_coef2,
    osal_u32 sgl_tone_0_2_car_and_en, osal_u32 sgl_tone_3_car);
osal_void hal_phy_do_sgl_tone_notch_weight(osal_u32 sgl_tone_0_7_weight, osal_u8 notch_ch);
#endif
/*****************************************************************************
功能描述  : 读取PHY的信道测量结果寄存器
*****************************************************************************/
osal_void hal_get_ch_measurement_result_ram(hal_ch_statics_irq_event_stru *ch_statics);
osal_void hal_get_ch_measurement_result(hal_ch_statics_irq_event_stru *ch_statics);

typedef osal_void (*hh503_initialize_phy_ext_cb)(osal_void);
typedef osal_void (*hh503_update_phy_by_bandwith_cb)(wlan_channel_bandwidth_enum_uint8 bandwidth);
typedef osal_void (*hh503_radar_config_reg_cb)(hal_dfs_radar_type_enum_uint8 radar_type);
typedef osal_void (*hh503_get_single_psd_sample_cb)(osal_u16 index, osal_char *psd_val);
typedef osal_void (*hh503_set_psd_en_cb)(osal_u32 reg_value);
typedef osal_void (*hal_set_phy_filter_cb)(hal_device_stru *hal_device, wlan_channel_band_enum_uint8 band);
typedef osal_void (*hh503_rf_regctl_enable_set_regs_ext_cb)(hal_to_dmac_device_stru *hal_device,
    wlan_channel_band_enum_uint8 freq_band, osal_u8 cur_ch_num, wlan_channel_bandwidth_enum_uint8 bandwidth);
typedef osal_void (*hh503_set_machw_phy_adc_freq_ext_cb)(hal_to_dmac_device_stru *hal_device,
    wlan_channel_bandwidth_enum_uint8 bandwidth);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_phy_rom.h */
