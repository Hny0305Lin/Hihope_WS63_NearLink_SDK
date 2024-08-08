/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Header file for common_alg_rf_cali_tx_pwr_pktram.c.
 */

#ifndef __CALI_TX_PWR_H__
#define __CALI_TX_PWR_H__
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "cali_outer_hal_if.h"
#include "cali_base_def.h"
#include "cali_accum_data.h"
#include "fe_rf_dev.h"
#include "power_ppa_ctrl_spec.h"
#include "cali_data_type.h"
#include "fe_hal_phy_if.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_CALI_H

/*****************************************************************************
  2 宏定义
*************************************************************************/
#define HAL_NUM_OF_CUST_TXPOWER_SCALING_TABLE      1024          /* 协议速率查询表最大发射功率参数的个数 */
#define CALI_MAX_SC_INT             (-30000)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 消息头定义
*****************************************************************************/

/*****************************************************************************
  6 消息定义
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

typedef struct {
    osal_u16 start_code;
    osal_u16 end_code;
    osal_u16 code_th;
    osal_u16 code_step;
    osal_s16 min_abs_val;          /* abs(pwr_val - pwr_ref) */
    osal_u16 rsv;
} cali_txpwr_upc_loop_stru;

typedef struct {
    osal_u16 search_max;      /* 模拟迭代最大值 */
    osal_u16 search_min;      /* 模拟迭代最小值 */
    osal_u16 search_normal;   /* 模拟迭代一般值 */
    osal_u16 search_th;       /* 模拟迭代 */
    osal_u16 search_step;     /* 模拟迭代步长 */
    osal_u16 default_upc;     /* upc code 初始值 */
    osal_u16 max_upc;         /* upc code 最大值 */
    osal_u16 min_upc;         /* upc code 最小值 */
    osal_u8 iq_idx;           /* IQ通路 */
    osal_u8 rsv[3];
} cali_txpwr_offline_stru;

typedef struct {
    osal_s32 vdet_offset[CALI_DC_OFFSET_NUM];       /* 无单音时的DC值 */
    osal_s16 p_ref;                /* 目标功率 */
    osal_u8 cur_lvl;               /* 当前校准档位 */
    osal_u8 ppa_cap;               /* 电容值 */
    osal_u16 atx_pwr_cmp;          /* upc code */
    osal_u8 iq_idx;                /* 2G还回通路只有I路有模拟信号 */
    osal_u8 chn_idx;               /* 频点序号 0, 1, 2 */
    cali_txpwr_offline_stru txpwr_ctrl;
#ifdef BOARD_ASIC_WIFI
    cali_txpwr_lvl_cfg_stru lvl_cfg[CALI_TXPWR_GAIN_LVL_NUM];
#endif
} cali_txpwr_param_stru;

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
#ifdef _PRE_WLAN_ATE
osal_void cali_txpwr_get_vdet_val(const hal_device_stru *device, const cali_txpwr_param_stru *txpwr_param,
    cali_accum_para_stru *accum_param, osal_s32 *pdet_val);
#endif
cali_txpwr_offline_stru cali_txpwr_get_param(osal_u8 band);
osal_void cali_txpwr_path(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth, osal_u8 chain);
osal_void cali_txpwr_recover(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth, osal_u8 chain);
osal_void cali_tx_power(const hal_device_stru *device, wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth, osal_u8 chain, osal_u8 chn_idx);
osal_void cali_txpwr_single_freq(const hal_device_stru *device,
    wlan_channel_band_enum_uint8 band,
    cali_bandwidth_enum_uint8 rf_cali_bandwidth,
    cali_txpwr_param_stru *txpwr_param,
    cali_accum_para_stru *accum_param);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of __COMMON_ALG_RF_CALI_TX_PWR_PKTRAM_H__ */
