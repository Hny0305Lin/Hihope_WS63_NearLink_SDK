/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of HAL ANT SEL ROM external public interfaces .
 */

#ifndef HAL_ANT_SEL_ROM_H
#define HAL_ANT_SEL_ROM_H

#include "osal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

osal_void hal_set_ant_sel_mac_tx_prot(osal_u8 ant_sel);
osal_void hal_set_ant_sel_mac_tx_bar(osal_u8 ant_sel);
osal_void hal_set_ant_sel_mac_tx_cf_end(osal_u8 ant_sel);
osal_void hal_set_ant_sel_mac_tx_beacon(osal_u8 ant_sel);
osal_void hal_set_ant_sel_mac_tx_coex(osal_u8 ant_sel);
osal_void hal_set_ant_sel_mac_tx_one_pkt(osal_u8 ant_sel);
osal_void hal_set_ant_sel_mac_rx(osal_u8 ant_sel);
osal_void hal_set_ant_sel_phy_enable(osal_u8 phy_enable);
osal_void hal_set_ant_sel_phy_cfg_man(osal_u8 cfg_man);
osal_void hal_set_ant_sel_phy_cfg_value(osal_u8 ant_sel);
osal_u32 hal_get_ant_sel_phy_enable(osal_void);
osal_u32 hal_get_ant_sel_phy_cfg_man(osal_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_ant_sel_rom.h */