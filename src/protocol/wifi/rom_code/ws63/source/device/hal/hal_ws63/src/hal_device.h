/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_device.c.
 */

#ifndef __HAL_DEVICE_H__
#define __HAL_DEVICE_H__

/*****************************************************************************
    头文件包含
*****************************************************************************/
#include "hal_common_ops.h"
#include "hal_device_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    宏定义
*****************************************************************************/
#define HAL_DEVICE_2G_BAND_NUM_FOR_LOSS (3) /* 2g 插损的band个数，用于插损相关的计算 */
#define HAL_DEVICE_5G_BAND_NUM_FOR_LOSS WLAN_5G_SUB_BAND_NUM
#define HAL_DEVICE_2G_DELTA_RSSI_NUM (2) /* 20M/40M */
#define HAL_DEVICE_5G_DELTA_RSSI_NUM (4) /* 20M/40M/80M/160M */
#define HAL_2G_CHANNEL_NUM (13)
#define INVALID_SAR_PWR_LIMIT                 (0XFF)   /* 当前SAR功率值 */
#define HAL_SAR_PWR_LIMIT_THRESHOLD            (15)    /* SAR功率阈值，低于阈值表示正在降SAR，高于阈值表示降SAR结束 */

#define WLAN_HAL_OHTER_BSS_ID                   6   /* 其他BSS的广播ID */
#define WLAN_HAL_OTHER_BSS_OTHER_ID             7   /* 来自其他BSS的其他帧(组播、单播) */
/* MAC上报的tbtt中断类别最大值，2个ap的tbtt中断(0-1)+3个sta的tbtt中断(4-6) */
#define WLAN_MAC_REPORT_TBTT_IRQ_MAX            7
/*****************************************************************************
    结构体定义
*****************************************************************************/

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
/* customize rf cfg struct */
typedef struct {
    osal_s8 rf_gain_db_mult4; /* 插损值(单位0.25dB) */
    osal_u8 resv[3];            /* 3 byte保留字段 */
} hal_cfg_custom_gain_db_per_band_stru;
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

/*****************************************************************************
    函数声明
*****************************************************************************/
osal_u8 get_sar_ctrl_params(osal_u8 idx);
osal_u8 *get_sar_ctrl_params_dev(osal_void);
oal_bool_enum_uint8 hal_is_hw_tx_queue_empty(hal_to_dmac_device_stru *hal_device);
osal_u8 hal_device_get_m2s_mask(osal_void);

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
osal_void hal_set_nvram_params_by_delt_pwr(hal_to_dmac_device_stru *hal_device, const osal_u8 *param);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_device.h */
