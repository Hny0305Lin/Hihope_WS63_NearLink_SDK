/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: MAC/SLP interface implementation.
 */
#ifndef HAL_SLP_ROM_H
#define HAL_SLP_ROM_H
#ifdef _PRE_WLAN_FEATURE_SLP
#include "osal_types.h"

/*****************************************************************************
  1 其他头文件包含
******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  函数声明
******************************************************************************/
osal_void hal_set_slp_tx_gpio(osal_u32 val);
osal_void hal_set_slp_rx_gpio(osal_u32 val);
osal_u32 hal_get_slp_tx_gpio(osal_void);
osal_u32 hal_get_slp_rx_gpio(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
#endif
