/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: 温度补偿用到的rf接口头文件
 */

#ifndef __FE_HAL_RF_IF_TEMPERATE_H__
#define __FE_HAL_RF_IF_TEMPERATE_H__
#include "osal_types.h"
#include "wlan_types_base_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
osal_bool fe_hal_rf_need_config_temp_code(osal_u8 old_code, osal_u8 new_code, osal_s16 cur_temp);
osal_u8 fe_hal_rf_get_temp_code_by_temperate(osal_s16 temperature);
osal_void fe_hal_rf_config_temp_code_value(osal_u8 rf_id, wlan_channel_band_enum_uint8 band, osal_u8 temp_code,
    osal_u8 bank_sel, osal_bool is_init);
osal_u32 fe_hal_rf_read_temperature(wlan_channel_band_enum_uint8 band, osal_s16 *cur_temp);
osal_u8 fe_hal_rf_get_rf_temp_code_bank_sel(osal_u8 rf_id, wlan_channel_band_enum_uint8 band);
osal_void fe_hal_rf_set_rf_temp_code_bank_sel(osal_u8 rf_id, wlan_channel_band_enum_uint8 band, osal_u8 bank_sel);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif  // __FE_HAL_RF_IF_TEMPERATE_H__