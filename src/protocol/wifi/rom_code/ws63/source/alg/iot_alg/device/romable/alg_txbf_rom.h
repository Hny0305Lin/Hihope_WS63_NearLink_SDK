/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: algorithm txbf
 */

#ifndef ALG_TXBF_ROM_H
#define ALG_TXBF_ROM_H

/*****************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "dmac_tx_bss_comm_rom.h"
#include "wlan_spec_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_TXBF
/*****************************************************************************
  10 函数声明
******************************************************************************/
osal_void alg_txbf_init(osal_void);
typedef osal_u32 (*alg_txbf_init_cb)(void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
