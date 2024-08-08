/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_chip.c.
 */

#ifndef HAL_CHIP_H
#define HAL_CHIP_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_chip_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  宏定义
*****************************************************************************/
#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_CHIP_H

/*****************************************************************************
   函数声明
*****************************************************************************/
osal_u32 hal_chip_init(osal_void);
osal_void hal_chip_exit(osal_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_chip.h */
