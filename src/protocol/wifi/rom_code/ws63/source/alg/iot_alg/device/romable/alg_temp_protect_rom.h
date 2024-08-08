/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: algorithm temp protect rom
 */

#ifndef ALG_TEMP_PROTECT_ROM_H
#define ALG_TEMP_PROTECT_ROM_H

/*****************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "dmac_ext_if_rom.h"
#include "frw_msg_rom.h"
#include "osal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_TEMP_PROTECT
/*****************************************************************************
  10 函数声明
******************************************************************************/
osal_u32 alg_temp_init(osal_void);
osal_u32 alg_temp_exit(osal_void);
osal_s32 dmac_alg_temp_protect_state_switch(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_s32 dmac_alg_temp_protect_pa_off_switch(dmac_vap_stru *dmac_vap, frw_msg *msg);
osal_bool alg_temp_protect_is_over_state(osal_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

