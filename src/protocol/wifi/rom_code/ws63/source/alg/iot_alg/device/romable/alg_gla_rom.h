/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: header file of alg_gla_rom.c
 */

#ifndef __ALG_GLA_ROM_H__
#define __ALG_GLA_ROM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "alg_main_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
   函数声明
*****************************************************************************/
osal_u32 alg_gla_param_sync(const alg_param_sync_stru *sync);
oal_bool_enum_uint8 alg_get_gla_switch_enable(alg_gla_id_enum_uint32 alg_id,
    alg_gla_switch_type_enum_uint8 gla_switch_type);
osal_u32 alg_gla_config(dmac_vap_stru *dmac_vap, dmac_ioctl_alg_param_stru *alg_param);
oal_bool_enum_uint8 alg_print_single_user_gla_info(alg_gla_id_enum_uint32 alg_id, dmac_user_stru *dmac_user);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
