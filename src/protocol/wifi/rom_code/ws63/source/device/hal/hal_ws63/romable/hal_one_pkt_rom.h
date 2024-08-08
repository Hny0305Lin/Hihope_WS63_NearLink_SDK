/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_irq_rom.c.
 */

#ifndef HAL_ONE_PKT_ROM_H
#define HAL_ONE_PKT_ROM_H

#include "osal_types.h"
#include "hal_device_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void hh503_irq_one_pkt_done_isr(hal_device_stru *hal_device, osal_u8 data);
typedef osal_u32 (*hal_wait_one_packet_done_cb)(osal_u16 timeout);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HAL_ONE_PKT_H__ */

