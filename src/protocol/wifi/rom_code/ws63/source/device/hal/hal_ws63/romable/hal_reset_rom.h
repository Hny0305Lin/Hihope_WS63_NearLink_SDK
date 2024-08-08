/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_reset_rom.c.
 */

#ifndef __HAL_RESET_ROM_H__
#define __HAL_RESET_ROM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hh503_phy_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* MAC每个bank所需保存的长度 */
#define PS_MAC_CTRL0_REG_LENGTH         HH503_MAC_CTRL0_OFFEST_BUTT
#define PS_MAC_CTRL1_REG_LENGTH         HH503_MAC_CTRL1_OFFEST_BUTT
#define PS_MAC_CTRL2_REG_LENGTH         HH503_MAC_CTRL2_OFFEST_BUTT

/* wlmac后面2个地址与前面的相隔太远 单独保存 */
#define PS_MAC_WLMAC_CTRL_REG_LENGTH    HH503_MAC_WLMAC_CTRL_OFFEST_BUTT

/* 0x400c0800 ~ 0x400c0b80(HH503_PHY_CFG_INTR_MASK) 长度0x384 */
#define PS_PHY_BANK1_REG_LENGTH         (HH503_PHY_CFG_RX_BUFFER_DELAY_CTL - HH503_PHY_BANK1_BASE + 0x4)
/* 0x400c0c00 ~ 0x400c0fb0(HH503_PHY_PHY_RX_11AX_CTRL0) 长度0x3b4 */
#define PS_PHY_BANK2_REG_LENGTH         (HH503_PHY_PHY_RX_11AX_CTRL0 - HH503_PHY_BANK2_BASE + 0x4)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hal_reset_rom.h */
