/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_alg_txbf.c.
 */

#ifndef __HAL_ALG_RTS_H__
#define __HAL_ALG_RTS_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_common_ops.h"
#include "hh503_phy_reg.h"
#include "wlan_spec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
   宏定义
*****************************************************************************/

/*****************************************************************************
  STRUCT定义
*****************************************************************************/

/*****************************************************************************
  函数声明
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_RTS
osal_void hal_set_rts_rate_init(osal_void);
osal_void hal_set_rts_fail_tx_psdu_en(oal_bool_enum_uint8 cfg_rts_fail_tx_psdu_en);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_alg_rts.h */

