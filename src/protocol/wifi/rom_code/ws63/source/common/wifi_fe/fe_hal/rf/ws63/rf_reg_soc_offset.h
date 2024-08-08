/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: rf寄存器定义
 * Date: 2022-11-26
*/


#ifndef _RF_REG_SOC_OFFSET_H_
#define _RF_REG_SOC_OFFSET_H_

#include "osal_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RF_WB_CTL_RB_BASE_ADDR           0x44004000

#define RF_WB_CTL_RB_SYS_CTL_ID_REG               (RF_WB_CTL_RB_BASE_ADDR + 0x0)
#define RF_WB_CTL_RB_RF_WL_REG_CLK_CTL_REG        (RF_WB_CTL_RB_BASE_ADDR + 0x10)
#define RF_WB_CTL_RB_RF_WL_REG_SOFT_RSTN_REG      (RF_WB_CTL_RB_BASE_ADDR + 0x14)
#define RF_WB_CTL_RB_RF_WL_REG_CMD_SOFT_RSTN_REG  (RF_WB_CTL_RB_BASE_ADDR + 0x18)
#define RF_WB_CTL_RB_RF_WL_REG_CMD_FIFO_STS_REG   (RF_WB_CTL_RB_BASE_ADDR + 0x1C)
#define RF_WB_CTL_RB_RF_WL_CBB_DIS_REG            (RF_WB_CTL_RB_BASE_ADDR + 0x20)
#define RF_WB_CTL_RB_RF_BT_CBB_DIS_REG            (RF_WB_CTL_RB_BASE_ADDR + 0x24)
#define RF_WB_CTL_RB_ABB_ADC_CTRL_REG             (RF_WB_CTL_RB_BASE_ADDR + 0x30)
#define RF_WB_CTL_RB_ABB_DAC_CTRL_REG             (RF_WB_CTL_RB_BASE_ADDR + 0x34)
#define RF_WB_CTL_RB_ABB_EN_CTRL_REG              (RF_WB_CTL_RB_BASE_ADDR + 0x38)
#define RF_WB_CTL_RB_CFG_WL_ADC_MUX_REG           (RF_WB_CTL_RB_BASE_ADDR + 0x3C)
#define RF_WB_CTL_RB_CFG_RF_MAN_EN_CTRL_REG       (RF_WB_CTL_RB_BASE_ADDR + 0x40)
#define RF_WB_CTL_RB_CFG_TRXEN_CTRL_REG           (RF_WB_CTL_RB_BASE_ADDR + 0x44)
#define RF_WB_CTL_RB_CFG_DCOC_IQ_CTRL_REG         (RF_WB_CTL_RB_BASE_ADDR + 0x48)
#define RF_WB_CTL_RB_CFG_PA_IDX_CTRL_REG          (RF_WB_CTL_RB_BASE_ADDR + 0x4C)
#define RF_WB_CTL_RB_CFG_LPF_CTRL_REG             (RF_WB_CTL_RB_BASE_ADDR + 0x50)
#define RF_WB_CTL_RB_CFG_RADAR_CTRL_REG           (RF_WB_CTL_RB_BASE_ADDR + 0x54)
#define RF_WB_CTL_RB_CFG_PPA_CODE_REG             (RF_WB_CTL_RB_BASE_ADDR + 0x58)
#define RF_WB_CTL_RB_CFG_TEMP_LOCK_CTRL_REG       (RF_WB_CTL_RB_BASE_ADDR + 0x5C)
#define RF_WB_CTL_RB_CFG_TEMP_BANK_SEL_CTRL_REG   (RF_WB_CTL_RB_BASE_ADDR + 0x60)
#define RF_WB_CTL_RB_CFG_RF_DIAG_MUX_REG          (RF_WB_CTL_RB_BASE_ADDR + 0x64)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
