/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: header for cali init power
 */
#ifndef __FE_INIT_POW_H__
#define __FE_INIT_POW_H__
#include "cali_online_common.h"
#include "hal_common_ops_rom.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 校准模块支持的band */
#define WLAN_CALI_BAND_BUTT                  (WLAN_BAND_2G + 1)
osal_void cali_pow_initialize_tx_power(hal_to_dmac_device_stru *hal_to_dmac_device);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif  // __CALI_INIT_POWER_H__