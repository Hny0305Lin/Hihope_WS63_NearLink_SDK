/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: 产线涉及到的hal层接口
 */
#ifndef FE_HAL_EQUIPMENT_H
#define FE_HAL_EQUIPMENT_H
#include "osal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#define FE_EFUSE_GROUP_MAX 3                   // 产测数据有三组
#define FE_EFUSE_POWER_CALI_NUM 7              // power cali参数个数: 第0个为高低功率factor, 第1~6个为curve
#define FE_EFUSE_CALI_BAND_NUM 3               // rssi comp参数个数
#define FE_EFUSE_POWER_CALI_LOCK_BIT_OFFSET 0  // power cali的lock bit偏移值
#define FE_EFUSE_RSSI_LOCK_BIT_OFFSET 1        // rssi comp的lock bit偏移值
typedef enum {
    FE_EFUSE_POWER_CALI_1_ID = 145,
    FE_EFUSE_RSSI_BAND123_1_ID = 159,
    FE_EFUSE_MFG_LOCK_1_ID = 161,
    FE_EFUSE_POWER_CALI_2_ID = 163,
    FE_EFUSE_RSSI_BAND123_2_ID = 177,
    FE_EFUSE_MFG_LOCK_2_ID = 179,
    FE_EFUSE_POWER_CALI_3_ID = 181,
    FE_EFUSE_RSSI_BAND123_3_ID = 195,
    FE_EFUSE_MFG_LOCK_3_ID = 197,
    FE_EFUSE_MAX_ID = 256,
} fe_efuse_byte;

osal_u32 fe_efuse_get_all_power_cali(osal_u8 *value, osal_u8 len);
osal_u32 fe_efuse_get_all_rssi_comp(osal_u8 *value, osal_u8 len);
#endif // _PRE_PLAT_FEATURE_CUSTOMIZE

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif  // FE_HAL_EQUIPMENT_H