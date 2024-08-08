/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_rf.c.
 */

#ifndef __HAL_RF_H__
#define __HAL_RF_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_ext_if.h"
#include "hal_device.h"
#include "fe_rf_customize.h"
#include "fe_hal_rf_reg_if.h"
#include "fe_rf_customize_rx_insert_loss.h"
#include "fe_rf_dev.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_RF_H
/*****************************************************************************
  2 共用宏定义
*****************************************************************************/
#define HH503_SOC_BUCK_RO         0x400032B4

/* wifi校准buf长度 */
#define RF_CALI_DATA_BUF_LEN              (0x6cd8)
#define RF_SINGLE_CHAN_CALI_DATA_BUF_LEN  (RF_CALI_DATA_BUF_LEN >> 1)
#define HH503_TEMPERATURE_THRESH         50

#ifdef _PRE_WLAN_03_MPW_RF
#define HH503_TOP_TST_SW2                      30
#endif /* end of _PRE_WLAN_03_MPW_RF */

#ifdef _PRE_WLAN_03_MPW_RF
#define HH503_PLL_LD_REG34           34
#endif

#define HH503_RF_TCXO_25MHZ          1
#define HH503_RF_TCXO_40MHZ          2
#define HH503_RF_TCXO_384MHZ         3

#define HH503_RF_LPF_GAIN_DEFAULT_VAL    0x4688
#define HH503_RF_DAC_GAIN_DEFAULT_VAL    0x24

/* PLL使能默认值配置 */
#ifdef _PRE_WLAN_03_MPW_RF
#define HH503_WL_PLL1_RFLDO789_ENABLE      0xFF00
#endif

#define HH503_RF_2G_CHANNEL_NUM        13
#define HH503_RF_5G_CHANNEL_NUM        7

#define HH503_RF_FREQ_2_CHANNEL_NUM  14  /* 2g支持信道数目 */
#define HH503_RF_FREQ_5_CHANNEL_NUM  29  /* 5g支持信道数目 */

#define HH503_CALI_CHN_INDEX_2422        0
#define HH503_CALI_CHN_INDEX_2447        1
#define HH503_CALI_CHN_INDEX_2472        2

#define CHANNEL_NUM5_IDX          4
#define CHANNEL_NUM10_IDX          9

#define HH503_CALI_CHN_INDEX_4950        0
#define HH503_CALI_CHN_INDEX_5210        1
#define HH503_CALI_CHN_INDEX_5290        2
#define HH503_CALI_CHN_INDEX_5530        3
#define HH503_CALI_CHN_INDEX_5610        4
#define HH503_CALI_CHN_INDEX_5690        5
#define HH503_CALI_CHN_INDEX_5775        6

#define HH503_RF_TEMP_STS_RSV                  0x7

#define HH503_RF_TEMP_INVALID                   (-100)   /* 温度非法值 */

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
/* RF定制化PA偏置寄存器个数 */
#define HH503_TX2G_PA_GATE_VCTL_REG_NUM        (9)
#define HH503_TX2G_PA_VRECT_GATE_THIN_REG_NUM  (32)
#define HH503_TX2G_REG_280_BAND2_3_NUM         (2)
#define HH503_TX2G_PA_GATE_REG_NUM    (HH503_TX2G_PA_GATE_VCTL_REG_NUM + HH503_TX2G_PA_VRECT_GATE_THIN_REG_NUM + \
    HH503_TX2G_REG_280_BAND2_3_NUM)
#endif

/* RF发送和接收带宽枚举 */
typedef enum {
    HH503_RF_BAND_CTRL_10M,
    HH503_RF_BAND_CTRL_20M,
    HH503_RF_BAND_CTRL_40M,
    HH503_RF_BAND_CTRL_80M,

    HH503_RF_BAND_CTRL_BUTT
} hh503_rf_bandwidth_enum;
typedef osal_u8 hh503_rf_bandwidth_enum_uint8;

typedef enum {
    RFLDO_EN_OFF,
    RF_EN_OFF,  /* 全关,包括RF_LDO */
    RF_OFF_BUTT
} rf_off_flag_enum;
typedef osal_u8 rf_off_flag_enum_uint8;

/* 校准信道序号结构体 */
typedef struct {
    osal_u8   norm2_dpa_idx_5g20m[HH503_RF_FREQ_5_CHANNEL_NUM];
    osal_u8   norm2_dpa_idx_2g[HH503_RF_FREQ_2_CHANNEL_NUM];
    osal_u8   other_dpa_idx_2g;
    osal_u8   other_dpa_idx_5g;
} hh503_cali_chn_idx_stru;

typedef struct {
    hal_rf_trx_type_enum           trx_type;            /* 发送链路类型 */
    hal_rf_trx_type_enum           trx_type_hist;       /* 历史发送链路类型 */
    osal_bool                        trx_type_change;     /* 发送链路类型标志 */
    osal_u8                          rf_enabled     : 1,  /* rf使能标志 */
                                   ldo_2g_wf0_on  : 1,  /* 2.4g WF0 ldo打开标志 */
                                   ldo_2g_wf1_on  : 1,  /* 2.4g WF0 ldo打开标志 */
                                   ldo_5g_wf0_on  : 1,  /* 5g ldo打开标志 */
                                   ldo_5g_wf1_on  : 1,  /* 5g ldo打开标志 */
                                   resv           : 3;  /* 5:保留位 */
    hal_rf_chn_param               chn_para;            /* 工作信道参数 */
} hal_rf;

/*****************************************************************************
  RF函数声明
*****************************************************************************/
/*****************************************************************************
  ABB相关接口
*****************************************************************************/
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
typedef struct {
    /* 2g */
    hal_cfg_custom_gain_db_per_band_stru   gain_db_2g[HAL_DEVICE_2G_BAND_NUM_FOR_LOSS];
#ifdef _PRE_WLAN_SUPPORT_5G
    /* 5g */
    hal_cfg_custom_gain_db_per_band_stru   gain_db_5g[HAL_DEVICE_5G_BAND_NUM_FOR_LOSS];
#endif
} hh503_cfg_custom_gain_db_rf_stru;

typedef struct {
    osal_s8   cfg_delta_pwr_ref_rssi_2g[HAL_DEVICE_2G_DELTA_RSSI_NUM];
#ifdef _PRE_WLAN_SUPPORT_5G
    osal_s8   cfg_delta_pwr_ref_rssi_5g[HAL_DEVICE_5G_DELTA_RSSI_NUM];
#endif
} hh503_cfg_custom_delta_pwr_ref_stru;

/* 03 rf相关的定制化参数结构体 */
typedef struct {
    /* INI */
    hh503_cfg_custom_gain_db_rf_stru       rf_gain_db_rf[WLAN_RF_CHANNEL_NUMS];           /* 2.4g 5g插损 */
    hh503_cfg_custom_delta_pwr_ref_stru    delta_pwr_ref_cfg[WLAN_RF_CHANNEL_NUMS];      /* 2.4g 5g delta_rssi值 */
    osal_u16                              aus_dyn_cali_dscr_interval[WLAN_BAND_BUTT];   /* 动态校准开关2.4g 5g */
    osal_u8                               far_dist_pow_gain_switch;                  /* 超远距离功率增益开关 */
    osal_u8                               band_5g_enable;                            /* 软件配置mac dev是否支持5g */
    osal_u8                               tone_amp_grade;                            /* 单音幅度档位 */
    osal_u8                               far_dist_dsss_scale_promote_switch;  /* 超远距11b 1m 2m dbb scale提升使能开关 */

    /* BIT3:CHA1_5G BIT2:CHA0_5G BIT1:CHA1_2G BIT0:CHA0_2G */
    osal_u8                               chn_radio_cap;
    osal_u8                               rsv[3];

    /* cca ed thr */
    hal_cfg_custom_cca_stru                 cfg_cus_cca;

    /* 过温/恢复门限 */
    osal_u16                              over_temp_protect_thread;
    osal_u16                              recovery_temp_protect_thread;
} hh503_rf_custom_stru;

typedef struct {
    osal_s32                  power;
    osal_u16                  mode_ctrl;
    osal_u16                  mode_gain;
    osal_u16                  pa_gain;
    osal_u16                  abb_gain;
    osal_u32                  scaling;
} hh503_adjust_tx_power_stru;

typedef struct {
    osal_s32 power; /* 命令行中的功率 */
    osal_u8 tpc_code; /* 功率对应的tpc code, 用于设置wifi aware action帧功率 */
    osal_u8 resrv[3];
} hh503_adjust_tx_power_level_stru;

typedef struct {
    osal_u8 index; /* 插损值的索引值 */
    osal_u8 channel_num_min; /* 插损值信道最小区间 */
    osal_u8 channel_num_max; /* 插损值信道最大区间 */
    osal_u8 reseve;
} hh503_band_num_for_loss;

/* 插损值索引与信道范围的对应关系 */
static const hh503_band_num_for_loss g_band_num_for_loss[] = {
    {0, 1, 4, 0}, {1, 6, 11, 0}, {2, 13, 14, 0},
    {0, 184, 196, 0}, {1, 36, 48, 0}, {2, 52, 64, 0},
    {3, 100, 112, 0}, {4, 116, 128, 0}, {5, 132, 140, 0},
    {6, 149, 165, 0}
};
#endif // #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE

osal_void hh503_set_rf_bw(wlan_bw_cap_enum_uint8 bw);

/* 回调函数实现 */
osal_void hh503_rf_set_trx_type_ext(hal_rf_trx_type_enum trx_type);
osal_void hh503_dpd_cfr_set_11b_ext(const hal_to_dmac_device_stru *hal_device, osal_u8 is_11b);
osal_void hh503_cali_send_func_ext(hal_to_dmac_device_stru *hal_device, osal_u8* cali_data, osal_u16 frame_len,
    osal_u16 remain);
osal_u32 hh503_config_custom_rf_ext(const osal_u8 *param);
osal_void hh503_config_rssi_for_loss_ext(osal_u8 channel_num, osal_s8 *rssi);
osal_void hh503_set_rf_custom_reg_ext(const hal_to_dmac_device_stru *hal_device);
osal_void hh503_initialize_rf_sys_ext(hal_to_dmac_device_stru *hal_device);
osal_void hh503_pow_set_rf_regctl_enable_ext(const hal_to_dmac_device_stru *hal_device,
    oal_bool_enum_uint8 rf_linectl);
osal_u8 hh503_get_tpc_code_ext(osal_void);
osal_void hh503_adjust_tx_power_level_ext(osal_u8 ch, osal_s8 power);

/*****************************************************************************
  11 ROM回调函数类型定义
*****************************************************************************/
/* hal */
typedef osal_u8 (*hh503_get_rx_chain_cb)(osal_void);
typedef osal_void (*hh503_set_rf_bw_cb)(wlan_bw_cap_enum_uint8 bw);

#ifdef _PRE_WLAN_FEATURE_TEMP_PROTECT
typedef osal_void (*hh503_read_max_temperature_cb)(osal_s16 *ps_temperature);
#endif
typedef osal_void (*hh503_rf_set_trx_type_cb)(hal_rf_trx_type_enum trx_type);
typedef osal_void (*hh503_dpd_cfr_set_11b_cb)(hal_to_dmac_device_stru *hal_device, osal_u8 is_11b);
typedef osal_void (*hh503_cali_send_func_cb)(hal_to_dmac_device_stru *hal_device, osal_u8* cali_data,
    osal_u16 frame_len, osal_u16 remain);
typedef osal_u32 (*hh503_config_custom_rf_cb)(const osal_u8 *param);
typedef osal_void (*hh503_config_rssi_for_loss_cb)(osal_u8 channel_num, osal_s8 *rssi);
typedef osal_void (*hh503_set_rf_custom_reg_cb)(hal_to_dmac_device_stru *hal_device);

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
typedef osal_u32 (*hh503_config_custom_dts_cali_cb)(const osal_u8 *param);
#endif
typedef osal_void (*hh503_initialize_rf_sys_cb)(hal_to_dmac_device_stru *hal_device);
typedef osal_void (*hh503_adjust_tx_power_cb)(osal_u8 ch, osal_s8 power);
typedef osal_void (*hh503_restore_tx_power_cb)(osal_u8 ch);
typedef osal_u8 (*hh503_rf_get_subband_idx_cb)(wlan_channel_band_enum_uint8 band, osal_u8 channel_idx);
typedef osal_void (*hh503_pow_set_rf_regctl_enable_cb)(hal_to_dmac_device_stru *hal_device,
    oal_bool_enum_uint8 rf_linectl);
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
osal_void hal_rf_get_rx_ppdu_info(osal_u8 flag);
#endif
osal_void hal_radar_sensing_switch_abb_lo_tx_top_test_reg12(osal_bool is_on);
osal_void hal_set_cal_tone(osal_u32 tone_cfg);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_rf.h */
