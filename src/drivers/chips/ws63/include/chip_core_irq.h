/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:   Chip core irq >= 0 define.
 *
 * Create:  2021-06-16
 */

#ifndef CHIP_CORE_IRQ_H
#define CHIP_CORE_IRQ_H

#define LOCAL_INTERRUPT0 26
typedef enum core_irq {
/* -------------------  Processor Interrupt Numbers  ------------------------------ */
    TIMER_0_IRQN          = LOCAL_INTERRUPT0 +  0,
    TIMER_1_IRQN          = LOCAL_INTERRUPT0 +  1,
    TIMER_2_IRQN          = LOCAL_INTERRUPT0 +  2,
    RTC_0_IRQN            = LOCAL_INTERRUPT0 +  3,
    RESERVED0_IRQN        = LOCAL_INTERRUPT0 +  4,
    I2C_0_IRQN            = LOCAL_INTERRUPT0 +  5,
    I2C_1_IRQN            = LOCAL_INTERRUPT0 +  6,
    GPIO_0_IRQN           = LOCAL_INTERRUPT0 +  7,
    GPIO_1_IRQN           = LOCAL_INTERRUPT0 +  8,
    GPIO_2_IRQN           = LOCAL_INTERRUPT0 +  9,
    SOFT_INT0_IRQN        = LOCAL_INTERRUPT0 + 10,
    SOFT_INT1_IRQN        = LOCAL_INTERRUPT0 + 11,
    SOFT_INT2_IRQN        = LOCAL_INTERRUPT0 + 12,
    SOFT_INT3_IRQN        = LOCAL_INTERRUPT0 + 13,
    COEX_WL_IRQN          = LOCAL_INTERRUPT0 + 14,
    COEX_BT_INT           = LOCAL_INTERRUPT0 + 15,
    COEX_WIFI_RESUME_IRQN = LOCAL_INTERRUPT0 + 16,
    QSPI0_2CS_IRQN        = LOCAL_INTERRUPT0 + 17,
    WLPHY_IRQN            = LOCAL_INTERRUPT0 + 18,
    WLMAC_IRQN            = LOCAL_INTERRUPT0 + 19,
    BT_BB_BLE_IRQN        = LOCAL_INTERRUPT0 + 20,
    BT_BB_GLE_IRQN        = LOCAL_INTERRUPT0 + 21,
    TSENSOR_IRQN          = LOCAL_INTERRUPT0 + 22,
    PMU_CMU_ERR_IRQN      = LOCAL_INTERRUPT0 + 23,
    DIAG_IRQN             = LOCAL_INTERRUPT0 + 24,
    I2S_IRQN              = LOCAL_INTERRUPT0 + 25,
    QSPI_IRQN             = LOCAL_INTERRUPT0 + 26,
    UART_0_IRQN           = LOCAL_INTERRUPT0 + 27,
    UART_1_IRQN           = LOCAL_INTERRUPT0 + 28,
    UART_2_IRQN           = LOCAL_INTERRUPT0 + 29,
    PWM_ABNOR_IRQN        = LOCAL_INTERRUPT0 + 30,
    PWM_CFG_IRQN          = LOCAL_INTERRUPT0 + 31,
    SFC_IRQN              = LOCAL_INTERRUPT0 + 32,
    DMA_IRQN              = LOCAL_INTERRUPT0 + 33,
    TIMER_ABNOR_IRQN      = LOCAL_INTERRUPT0 + 34,
    I2S_TX_IRQN           = LOCAL_INTERRUPT0 + 35,
    I2S_RX_IRQN           = LOCAL_INTERRUPT0 + 36,
    PKE_REE_IRQN          = LOCAL_INTERRUPT0 + 37,
    SPACC_REE_IRQN        = LOCAL_INTERRUPT0 + 38,
    RKP_REE_IRQN          = LOCAL_INTERRUPT0 + 39,
    KLAD_REE_IRQN         = LOCAL_INTERRUPT0 + 40,
    SLP_UART_RX_WAKE_IRQN = LOCAL_INTERRUPT0 + 41,
    TIMING_GEN_INT        = LOCAL_INTERRUPT0 + 42,
    MAC_MONITOR_IRQN      = LOCAL_INTERRUPT0 + 43,
    MEM_MONITOR_IRQN      = LOCAL_INTERRUPT0 + 44,
    TCM_MONITOR_IRQN      = LOCAL_INTERRUPT0 + 45,
    LSADC_IRQNR           = LOCAL_INTERRUPT0 + 46,
    BUTT_IRQN
} core_irq_t;

#endif
