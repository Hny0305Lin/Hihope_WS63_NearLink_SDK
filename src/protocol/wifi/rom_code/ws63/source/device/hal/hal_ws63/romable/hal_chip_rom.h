/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_chip_rom.c.
 */

#ifndef HAL_CHIP_ROM_H
#define HAL_CHIP_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "hal_device_rom.h"
#include "hal_ext_if_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  STRUCT定义
*****************************************************************************/


/* 定义CHIP下专用寄存器 */
typedef struct tag_hal_hal_chip_stru {
    hal_to_dmac_chip_stru hal_chip_base;
    hal_device_stru device;
    hal_cfg_rf_custom_cap_info_stru rf_custom_mgr;
    uintptr_t *_rom; /* ROM化后资源扩展指针 */
} hal_chip_stru;

/*****************************************************************************
   函数声明
*****************************************************************************/
osal_void hal_mac_soft_reset(osal_void);
hal_chip_stru *hal_get_chip_stru(osal_void);
hal_to_dmac_device_stru *hal_chip_get_hal_device(osal_void);
hal_device_stru *hh503_chip_get_device(osal_void);

typedef osal_u32 (*hal_device_init_resv_cb)(hal_to_dmac_device_stru *hal_device_base);
// 原host
osal_u32 hal_chip_init(osal_void);
osal_void hal_chip_init_rf_custom_cap(hal_chip_stru *hal_chip);

typedef osal_u32 (*hal_device_init_resv_cb)(hal_to_dmac_device_stru *hal_device_base);
typedef osal_void (*hal_device_init_ext_cb)(hal_chip_stru *hal_chip);
osal_u32 hal_device_init_resv(hal_to_dmac_device_stru *hal_device_base);
osal_void hal_device_init_ext(hal_chip_stru *hal_chip);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_chip_rom.h */
