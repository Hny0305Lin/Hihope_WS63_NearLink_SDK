/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: general purpose register header file.
 */

#ifndef __HAL_GP_REG_H__
#define __HAL_GP_REG_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "hal_gp_reg_rom.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*****************************************************************************
  3 函数声明
*****************************************************************************/
osal_void hal_gp_set_btcoex_wifi_status(osal_u32 val);
osal_u32 hal_gp_get_btcoex_wifi_status(osal_void);
osal_u32 hal_gp_get_btcoex_bt_status(osal_void);
#endif /* end of hal_gp_reg.h */