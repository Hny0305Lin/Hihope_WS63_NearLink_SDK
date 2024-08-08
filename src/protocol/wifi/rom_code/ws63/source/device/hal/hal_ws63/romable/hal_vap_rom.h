/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_vap_rom.c.
 */

#ifndef HAL_VAP_ROM_H
#define HAL_VAP_ROM_H

#include "hal_common_ops_rom.h"

osal_void hal_add_vap(hal_to_dmac_device_stru *hal_device_base, wlan_vap_mode_enum_uint8 vap_mode,
    osal_u8 hal_vap_id, hal_to_dmac_vap_stru **hal_vap);
osal_void hal_del_vap(hal_to_dmac_device_stru *hal_device, osal_u8 vap_id);

osal_void hal_get_hal_vap(const hal_to_dmac_device_stru *hal_device_base, osal_u8 vap_id,
    hal_to_dmac_vap_stru **hal_vap);

#ifdef _PRE_WLAN_FEATURE_CHBA
osal_void hal_vap_set_chba_timer(const hal_to_dmac_vap_stru *hal_vap, const twt_reg_param_stru *twt_param,
    osal_u8 count);
#endif

#endif /* end of hal_vap_rom.h */
