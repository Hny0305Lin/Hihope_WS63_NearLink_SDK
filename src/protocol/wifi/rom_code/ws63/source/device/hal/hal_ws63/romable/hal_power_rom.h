/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_power_rom.c.
 */

#ifndef HAL_POWER_ROM_H
#define HAL_POWER_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_soc_rom.h"
#include "hal_mac_rom.h"
#include "hal_phy_rom.h"

#include "oal_ext_if_rom.h"
#include "hal_ext_if_rom.h"
#include "wlan_types_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*************************************************************************/
/* 定制化相关宏 */
#define HAL_NUM_OF_CUST_TXPOWER_SCALING_TABLE      1024          /* 协议速率查询表最大发射功率参数的个数 */
#define HAL_INIT_NVM_MAX_TXPWR_BASE_2P4G           190           /* 基准最大发射功率初始值 0.1dbm */
#define HAL_INIT_NVM_MAX_TXPWR_BASE_5G             170
#define HAL_MAX_TXPOWER_MIN                        130           /* 最大发送功率的最小有效值:130 13.0dbm */
#define HAL_MAX_TXPOWER_MAX                        238           /* 最大发送功率的最大有效值:238 23.8dbm */

#define HH503_PHY_POWER_REF_2G_DEFAULT 0x14E8FC /* POWER REF 2G默认值取mt7对应值 */
#define HH503_PHY_POWER_REF_5G_DEFAULT 0x0ce0f4

/* tpc */
#define HAL_TPC_CH_NUM 32 /* tpc_ch档位总计有32档 */

/* PGA STEP */
#define HAL_PGA_STEP_NUM 64 /* PGA STEP档位总计有64档 */

/* PGA STEP */
#define HAL_POWER_CHANGE_COEF 10 /* POWER转换系数 */

/* 32档位UPC LUT的增益间隔粒度为0.5，扩大10倍用于计算,0.5*ALG_TPC_POW_PRECISION_SHIFT */
#define HAL_POW_LPF_LUT_NUM                 8          /* 筛选使用的LPF档位数目 */
#define HAL_POW_DAC_LUT_NUM                 4          /* 筛选使用的DAC档位数目 */

#define HAL_POW_DIST_5G_TABLE_LEN           13          /* 5G档位增益分配表长度 */
#define HAL_POW_DIST_2G_TABLE_LEN           16          /* 2G档位增益分配表长度 */

#ifdef _PRE_WLAN_ONLINE_DPD
#define HH503_DPD_OFF_CFR_ON               2
#define HH503_DPD_LVL_INVALID              3
#endif
#define HAL_RESP_POWER_REG_NUM              2

#define hal_get_cfr_idx_from_tpc(tpc)           ((osal_u8)(oal_get_bits((tpc), NUM_2_BITS, BIT_OFFSET_10)))
#define hal_get_dpd_tpc_lv_ch0_from_tpc(tpc)    ((osal_u8)(oal_get_bits((tpc), NUM_2_BITS, BIT_OFFSET_8)))
#define hal_get_tpc_ch0_from_tpc(tpc)           ((osal_u8)(oal_get_bits((tpc), NUM_8_BITS, BIT_OFFSET_0)))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef struct {
    osal_u16 tpc_ch0          : 8,
            dpd_tpc_lv_ch0    : 2,
            cfr_idx           : 2,
            bit_rsv           : 4;
} tpc_code_params_stru;

typedef struct {
    osal_s16  max_pow  : 6,
            min_pow  : 6,
            offset   : 4;
} mcs_tx_pow_stru;

/*****************************************************************************
  10 函数声明
*****************************************************************************/

osal_u32 hh503_rf_get_center_freq_chan_num(osal_u8 chan_number, wlan_channel_bandwidth_enum_uint8 bandwidth,
    osal_u8 *center_freq_chn);

osal_void hh503_pow_init_vap_pow_code_ext(hal_to_dmac_device_stru *hal_device,
    hal_vap_pow_info_stru *vap_pow_info, hal_pow_set_type_enum_uint8 type, const mac_channel_stru *channel);
osal_void hal_sync_tpc_config_device(hal_to_dmac_device_stru *hal_device, osal_u8 *data, osal_u16 data_len);
osal_void hh503_pow_set_resp_tpc_code_to_pow_mode(hh503_tx_resp_phy_mode_reg_stru *pow_code,
    const tpc_code_params_stru *tpc_code);
osal_void hh503_set_ack_cts_pow_code(hal_to_dmac_device_stru *hal_device, wlan_channel_band_enum_uint8 band,
    osal_u8 subband_idx, wlan_user_distance_enum_uint8 distance_idx);
/*****************************************************************************
  11 ROM回调函数类型定义
*****************************************************************************/
/* hal */
typedef osal_void (*hh503_get_cali_param_set_tpc_cb)(hal_to_dmac_device_stru *hal_device,
    wlan_channel_band_enum_uint8 band, osal_u8 channel_idx);
typedef osal_void (*hh503_tb_mcs_tx_power_cb)(wlan_channel_band_enum_uint8 band);
typedef osal_void (*hh503_tb_tx_power_init_cb)(wlan_channel_band_enum_uint8 band);

typedef osal_void (*hh503_set_tx_dscr_power_tpc_cb)(osal_u8 rate_idx, wlan_channel_band_enum_uint8 band,
    osal_s16 tx_power, tpc_code_params_stru *tpc_param);
typedef osal_void (*hh503_pow_sw_initialize_tx_power_cb)(hal_to_dmac_device_stru *hal_device);
typedef osal_void (*hh503_pow_initialize_tx_power_cb)(hal_to_dmac_device_stru *hal_device);
typedef osal_void (*hh503_pow_init_vap_pow_code_cb)(hal_to_dmac_device_stru *hal_device,
    hal_vap_pow_info_stru *vap_pow_info, hal_pow_set_type_enum_uint8 type, const mac_channel_stru *channel);
typedef osal_void (*fe_hal_sync_tpc_pow_rate_table_cb)(osal_u8 *data, osal_u16 data_len);
typedef osal_u16 (*fe_tpc_rate_pow_get_rate_tpc_code_cb)(osal_u8 band, osal_u8 protocol_mode,
    osal_u8 mcs, osal_u8 bw, osal_u8 pwr_lvl);
osal_u8 hal_device_get_pow_base_idx(osal_u8 protocol_idx, osal_u8 cap_idx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
