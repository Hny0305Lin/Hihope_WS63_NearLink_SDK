/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file for hal_irq_rom.c.
 */

#ifndef HAL_IRQ_ROM_H
#define HAL_IRQ_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "osal_types.h"
#include "hal_device_rom.h"
#include "hal_mac_err_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  宏定义
*****************************************************************************/
#define RISCV_SYS_VECTOR_CNT   26
#define HH503_MAC_RPT_INTR_RX_TRIG_FRM_MASK    0x40000000
#define HH503_MAC_RPT_INTR_RX_NORM_COMPLETE_MASK    0x1
#define HH503_MAC_RPT_INTR_RX_HIPRI_COMPLETE_MASK    0x4
#define HH503_MAC_RPT_INTR_TX_COMPLETE_MASK    0x2
#define HH503_MAC_RPT_INTR_VAP0_TBTT_MASK    0x100
#define HH503_MAC_RPT_INTR_VAP1_TBTT_MASK    0x80
#define HH503_MAC_RPT_INTR_VAP2_TBTT_MASK    0x40
#define HH503_MAC_RPT_INTR_ERROR_MASK    0x8
#define HH503_MAC_RPT_INTR_CH_STATISTIC_DONE_MASK    0x20000
#define HH503_MAC_RPT_INTR_ONE_PACKET_DONE_MASK    0x4000
#define HH503_MAC_RPT_INTR_CSI_DONE_MASK    0x2000
#define HH503_MAC_RPT_INTR_P2P_CT_WINDOW_END_MASK    0x40000
#define HH503_MAC_RPT_INTR_P2P_NOA_ABSENT_START_MASK    0x80000
#define HH503_MAC_RPT_INTR_P2P_NOA_ABSENT_END_MASK    0x100000
#define HH503_MAC_RPT_INTR_VAP0_WAIT_BCN_TIMEOUT_MASK    0x200000
#define HH503_MAC_RPT_INTR_VAP1_WAIT_BCN_TIMEOUT_MASK    0x400000
#define HH503_MAC_RPT_INTR_VAP2_WAIT_BCN_TIMEOUT_MASK    0x800000
#define HH503_MAC_RPT_INTR_ABORT_START_MASK    0x200
#define HH503_MAC_RPT_INTR_ABORT_DONE_MASK    0x8000
#define HH503_MAC_RPT_INTR_MODE_SELECT_END_MASK    0x10000
#define HH503_MAC_RPT_INTR_ABORT_END_MASK    0x400
#define HH503_MAC_RPT_INTR_FTM_MASK    0x10000000
#define HH503_MAC_RPT_INTR_HE_ROM_UPDATE_MASK    0x80000000
#define HH503_MAC_RPT_INTR_BCN_NO_FRM_MASK    0x20000000
#define HH503_MAC_IS_TRS_OFFSET  8
#define HH503_MAC_IS_TRS_MASK 0x100
#define HH503_MAC_TRIG_AP_TX_POWER_LSB_OFFSET  28
#define HH503_MAC_TRIG_AP_TX_POWER_LSB_MASK 0xF0000000
#define HH503_MAC_TRIG_AP_TX_POWER_MSB_OFFSET  0
#define HH503_MAC_TRIG_AP_TX_POWER_MSB_MASK 0x3
#define HH503_MAC_TRIG_TYPE_OFFSET  0
#define HH503_MAC_TRIG_TYPE_MASK 0xF
#define HH503_MAC_TRIG_UL_BW_OFFSET  18
#define HH503_MAC_TRIG_UL_BW_MASK 0xC0000

#define NUM_HAL_INTERRUPT_WLAN (19 + RISCV_SYS_VECTOR_CNT)
#define NUM_HAL_INTERRUPT_PHY  (18 + RISCV_SYS_VECTOR_CNT)
#define IRQ_FLAG_PRI8 8
#define IRQ_FLAG_PRI11 11
#define HH503_MAC0_IRQ NUM_HAL_INTERRUPT_WLAN
#define HH503_PHY0_IRQ NUM_HAL_INTERRUPT_PHY
/*****************************************************************************
  函数声明
*****************************************************************************/
osal_void hh503_irq_mac_isr_cb(u_interrupt_status *status);
osal_void hh503_clear_mac_error_int_status(const hal_error_state_stru *status);
osal_void hal_set_noa_count(osal_u16 value);
/*****************************************************************************
 ROM回调函数类型定义
*****************************************************************************/
typedef osal_void (*hh503_cb_irq_mac_isr)(u_interrupt_status *status);
typedef osal_void (*hh503_cb_irq_phy_isr)(u_interrupt_status *status);
osal_void hh503_irq_rx_txbf_frame_process(hal_to_dmac_device_stru *hal_device, osal_u32 *base_dscr,
    osal_u8 queue_id);

osal_s32 hh503_irq_isr_all(osal_s32 p1, osal_void *p2);
osal_s32 hh503_irq_phy_all(osal_s32 p1, osal_void *p2);

osal_void hal_mask_interrupt(osal_u32 offset);

typedef osal_u32 (*hal_p2p_noa_absent_end)(hal_device_stru *hal_device, osal_u8 data);
typedef osal_u32 (*hal_p2p_noa_absent_start)(hal_device_stru *hal_device, osal_u8 data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif /* end of hal_irq_rom.h */
