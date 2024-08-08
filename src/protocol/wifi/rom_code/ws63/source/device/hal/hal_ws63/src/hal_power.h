/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_power.c.
 */

#ifndef __HAL_POWER_H__
#define __HAL_POWER_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_power_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* 0~5的pow档位 */
typedef enum {
    HAL_POW_LEVEL_0 = 0,
    HAL_POW_LEVEL_1,
    HAL_POW_LEVEL_2,
    HAL_POW_LEVEL_3,
    HAL_POW_LEVEL_4,
    HAL_POW_LEVEL_5,
    HAL_POW_LEVEL_BUTT
} hal_pow_lvl_no_enum;

typedef struct {
    osal_u32  dpd_tpc_lv        : 2,
            cfr_index         : 2,
            upc_level         : 1,
            lpf_gain          : 3,
            pa_gain           : 2,
            dac_gain          : 2,
            delta_dbb_scaling : 10,
            dpd_enable        : 1,
            bit_rsv               : 9;
} pow_code_params_stru;

typedef struct {
    wlan_channel_band_enum_uint8 freq_band; /* 2.4G or 5G */
    osal_u8 rate_idx;
    osal_u8 pwr_idx;
    osal_s16 tx_power; /* 发送功率 */
    osal_u8 resv[3];
} hal_tpc_set_stru;

/* 档位分配占比的参数 */
typedef struct {
    osal_u8   dac_idx;
    osal_u8   lpf_idx;
    osal_u8   upc_idx;
    osal_u8   pa_idx;
} hal_pow_distri_ratio_stru;

typedef struct {
    osal_u8   cfr_idx;
    osal_u8   dpd_idx;
} hal_dpd_cfr_distri_ratio_stru;

typedef struct {
    osal_u32  dbb_scaling_2g    : 10;
    osal_u32  dpd_tpc_lv_ch1    : 2;
    osal_u32  dpd_update_en_ch1 : 1;
    osal_u32  dpd_tpc_lv_ch0    : 2;
    osal_u32  dpd_update_en_ch0 : 1;
    osal_u32  cfr_idx           : 2;
    osal_u32  dbb_scaling_5g    : 10;
    osal_u32  resv0             : 4;
    osal_s16   max_power_2g;
    osal_s16   max_power_5g;
} hal_pow_tpc_param_stru;

typedef struct {
    osal_u32  trig_phy_mode;
    osal_u32  trig_data_rate;
} hal_pow_trig_param_stru;

osal_void hh503_pow_set_pow_to_pow_code(hal_to_dmac_device_stru *hal_device, osal_u8 max_pow,
    const mac_channel_stru *channel);
osal_void hh503_pow_sw_initialize_tx_power_ext(hal_to_dmac_device_stru *hal_device);
osal_void hh503_tb_mcs_tx_power_ext(wlan_channel_band_enum_uint8 band);
osal_void hh503_tb_tx_power_init_ext(wlan_channel_band_enum_uint8 band);
osal_void hal_tpc_rate_pow_print_rate_pow_table(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
