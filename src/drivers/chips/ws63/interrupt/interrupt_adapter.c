/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  Interrupt DRIVER
 *
 * Create: 2021-06-16
 */
#include "stdint.h"
#include "chip_core_irq.h"

uint8_t g_auc_int_pri[BUTT_IRQN] = {
    0, // USER_SOFTWARE_INT_IRQN
    0, // SUPERVISOR_SOFTWARE_INT_IRQN
    0, // RESERVED_INT2_IRQN
    1, // MACHINE_SOFTWARE_INT_IRQN
    0, // USER_TIMER_INT_IRQN
    0, // SUPERVISOR_TIMER_INT_IRQN
    0, // RESERVED_INT6_IRQN
    0, // MACHINE_TIMER_INT_IRQN
    0, // USER_EXTERNAL_INT_IRQN
    0, // SUPERVISOR_EXTERNAL_INT_IRQN
    0, // RESERVED_INT10_IRQN
    1, // MACHINE_EXTERNAL_INT_IRQN
    7, // NON_MASKABLE_INT_IRQN
    0, // RESERVED_INT13_IRQN
    0, // RESERVED_INT14_IRQN
    0, // RESERVED_INT15_IRQN
    0, // RESERVED_INT16_IRQN
    0, // RESERVED_INT17_IRQN
    0, // RESERVED_INT18_IRQN
    0, // RESERVED_INT19_IRQN
    0, // RESERVED_INT20_IRQN
    0, // RESERVED_INT21_IRQN
    0, // RESERVED_INT22_IRQN
    0, // RESERVED_INT23_IRQN
    0, // RESERVED_INT24_IRQN
    0, // RESERVED_INT25_IRQN

    1, // TIMER_0_IRQN
    1, // TIMER_1_IRQN
    1, // TIMER_2_IRQN
    1, // RTC_0_IRQN
    0, // RESERVED0_IRQN
    1, // I2C_0_IRQN
    1, // I2C_1_IRQN
    1, // GPIO_0_IRQN
    1, // GPIO_1_IRQN
    1, // GPIO_2_IRQN
    0, // SOFT_INT0_IRQN
    0, // SOFT_INT1_IRQN
    0, // SOFT_INT2_IRQN
    0, // SOFT_INT3_IRQN
    0, // COEX_WL_IRQN
    0, // COEX_BT_IRQN
    0, // COEX_WIFI_RESUME_IRQN
    1, // SPI_IRQN
    0, // WLPHY_IRQN
    0, // WLMAC_IRQN
    0, // BLE_IRQN
    0, // GLE_IRQN
    1, // TSENSOR_IRQN
    1, // PMU_CMU_ERR_IRQN
    0, // DIAG_IRQN
    1, // I2S_IRQN
    1, // QSPI_IRQN
    1, // UART0_IRQN
    1, // UART1_IRQN
    1, // UART2_IRQN
    1, // PWM_ABNOR_IRQN
    1, // PWM_CFG_IRQN
    1, // SFC_IRQN
    1, // DMA_IRQN
    1, // TIMER_ABNOR_IRQN
    1, // I2S_TX_IRQN
    1, // I2S_RX_IRQN
    1, // PKE_REE_IRQN
    1, // SPACC_REE_IRQN
    1, // RKP_REE_IRQN
    1, // KLAD_REE_IRQN
    1, // GLP_UART_RX_WAKE_IRQN
    1, // TIMING_GEN_IRQN
    1, // MAC_MONITOR_IRQN
    1, // MEM_MONITOR_IRQN
    1, // TCM_MONITOR_IRQN
    1, // LSADC_IRQNR
};
