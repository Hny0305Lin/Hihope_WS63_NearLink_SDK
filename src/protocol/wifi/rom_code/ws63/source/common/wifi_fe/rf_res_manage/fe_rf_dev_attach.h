/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Header file for hal_rf_dev.c.
 */

#ifndef HAL_RF_DEV_H
#define HAL_RF_DEV_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if_rom.h"
#include "fe_rf_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void fe_rf_dev_set_ops_ext(hal_rf_ops_cfg_uint8 cfg);
osal_u32 fe_rf_dev_init(osal_u8 rf_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
