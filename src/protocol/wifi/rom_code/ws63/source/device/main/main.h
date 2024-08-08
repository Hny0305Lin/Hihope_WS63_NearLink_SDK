/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Head file of MAIN element structure and its corresponding enumeration.
 */

#ifndef MAIN_H
#define MAIN_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if_rom.h"
#include "hcc_cfg_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAIN_H

/*****************************************************************************
  10 函数声明
*****************************************************************************/
osal_void cali_closeout_mem_cfg(osal_void);
osal_void send_bsp_ready(osal_void);
osal_void device_main_init(osal_void);
osal_void device_main_exit(osal_void);
osal_s32  device_module_init(osal_void);
osal_s32  dmac_main_init(osal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
