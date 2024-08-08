/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: header file of alg_gla.c
 */

#ifndef __ALG_GLA_H__
#define __ALG_GLA_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "alg_transplant.h"

#include "hal_device.h"
#include "msg_alg_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
   函数声明
*****************************************************************************/
osal_u32 alg_gla_para_sync_fill(alg_param_sync_stru *data);
oal_bool_enum_uint8 alg_host_get_gla_switch_enable(alg_gla_id_enum_uint32 alg_id,
    alg_gla_switch_type_enum_uint8 gla_switch_type);
osal_u32 alg_set_gal_switch_enable(alg_gla_id_enum_uint32 alg_id, alg_gla_switch_type_enum_uint8 gla_switch_type);
osal_u32 alg_set_gal_switch_disable(alg_gla_id_enum_uint32 alg_id, alg_gla_switch_type_enum_uint8 gla_switch_type);
osal_void alg_hmac_gla_init_usual_switch(osal_void);
osal_void alg_hmac_gla_init_optional_switch(osal_void);
osal_void alg_hmac_gla_init(osal_void);
osal_void alg_hmac_gla_exit(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
