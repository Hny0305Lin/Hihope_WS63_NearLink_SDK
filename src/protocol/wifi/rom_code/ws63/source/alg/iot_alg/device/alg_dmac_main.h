/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description:  Entry, initialization, and external interface definition of the algorithm module, depending on the DMAC
 */

#ifndef ALG_DMAC_MAIN_H
#define ALG_DMAC_MAIN_H

/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "alg_main_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void alg_dmac_internal_hook_init(osal_void);
osal_void alg_dmac_internal_hook_exit(osal_void);
#ifdef _PRE_WLAN_FEATURE_INTF_DET
osal_s32 alg_intf_det_start(dmac_vap_stru *dmac_vap, frw_msg *msg);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of alg_dmac_main.h */
