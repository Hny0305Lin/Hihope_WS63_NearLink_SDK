/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_ampdu_rom.c.
 */

#ifndef HAL_AMPDU_ROM_H
#define HAL_AMPDU_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hal_common_ops_rom.h"
#include "hal_device_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
 功能描述  : 硬聚时计算第一次收到对端BA帧时的bitmap确认情况
*****************************************************************************/
osal_void hal_tx_update_dscr_para_hw(const hal_tx_dscr_stru *tx_dscr, hal_tx_dscr_ctrl_one_param *tx_hw_stat);
/*****************************************************************************
 功能描述  : 获取描述符中BA SSN
*****************************************************************************/
osal_void hal_tx_get_dscr_ba_ssn(hal_tx_dscr_stru *tx_dscr, osal_u16 *ba_ssn);
/*****************************************************************************
 功能描述  : 获取描述符中BA bitmap表
*****************************************************************************/
osal_void hal_tx_get_dscr_ba_bitmap(hal_tx_dscr_stru *tx_dscr, osal_u32 *bitmap);
/*****************************************************************************
 功能描述  : 添加一个表项到BA LUT表
*****************************************************************************/
osal_void hal_add_machw_ba_lut_entry(machw_tx_ba_params_stru tx_ba_pars, const osal_u8 *dst_addr);

/*****************************************************************************
 功能描述  : 添加一个表项到TX BA LUT表
*****************************************************************************/
osal_void hal_add_machw_tx_ba_lut_entry(machw_tx_ba_params_stru tx_ba_pars);

/*****************************************************************************
 功能描述  : 从BA LUT表中删除一个表项
*****************************************************************************/
osal_void hal_remove_machw_ba_lut_entry(osal_u8 lut_index);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* end of HAL_AMPDU_ROM_H */
#endif /* end of HAL_AMPDU_ROM_H */
#endif
