/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_psd_rom.c.
 */

#ifndef HAL_PSD_ROM_H
#define HAL_PSD_ROM_H

#include "hal_device_rom.h"
#include "hal_phy_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_PSD
#define PSD_DATA_LEN 256
#define PSD_VAL_MIN (-128)

osal_void hh503_irq_psd_det(hal_device_stru *hal_device, osal_u8 data);
osal_s8 *hal_psd_get_buffer(osal_void);
osal_void hal_psd_alloc(osal_void);
osal_void hal_psd_free(osal_void);
osal_void hal_psd_data_clear(osal_void);
/* 钩子函数声明 */
typedef osal_u32 (*hal_psd_det_cb)(osal_s8 *psd_data);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_psd */
