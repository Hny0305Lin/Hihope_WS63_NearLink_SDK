/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_irq_rom.c.
 */

#ifndef HAL_TBTT_ROM_H
#define HAL_TBTT_ROM_H

#include "osal_types.h"
#include "hal_device_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HAL_TBTT_H

osal_void hh503_irq_tbtt_isr(hal_device_stru *hal_device, osal_u8 data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HAL_TBTT_ROM_H */

