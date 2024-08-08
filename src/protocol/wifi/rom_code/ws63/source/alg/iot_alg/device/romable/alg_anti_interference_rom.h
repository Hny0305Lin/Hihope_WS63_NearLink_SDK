/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: algorithm antanne interference head
 */

#ifndef ALG_ANTI_INTERFERENCE_ROM_H
#define ALG_ANTI_INTERFERENCE_ROM_H

#ifdef _PRE_WLAN_FEATURE_ANTI_INTERF

/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "alg_main_rom.h"
#include "dmac_alg_if_part_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/******************************************************************************
  4 函数声明
******************************************************************************/
osal_s32 alg_dmac_anti_intf_init(osal_void);
osal_void alg_dmac_anti_intf_exit(osal_void);
osal_u32 alg_dmac_anti_intf_config_param(const dmac_vap_stru *dmac_vap, const dmac_ioctl_alg_param_stru *alg_param);
osal_s32 dmac_get_anti_intf_ar_stat(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 dmac_clear_anti_intf_info_stat(dmac_vap_stru *dmac_vap, frw_msg *msg);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* #ifdef _PRE_WLAN_FEATURE_ANTI_INTERF */
#endif /* end of alg_anti_interference_rom.h */