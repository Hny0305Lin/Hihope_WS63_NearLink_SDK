/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: MAC/OMI interface implementation.
 */
#ifndef HAL_OMI_ROM_H
#define HAL_OMI_ROM_H

#ifdef _PRE_WLAN_FEATURE_OM
/*****************************************************************************
  1 其他头文件包含
******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  10 函数声明
******************************************************************************/
osal_void hh503_irq_rom_info_handle_isr(hal_device_stru *hal_device);
osal_void hh503_irq_he_htc_receive_isr(hal_device_stru *hal_device, osal_u8 data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
#endif

