/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Specific UART HAL Driver Implementation.
 *
 * Create:  2023-2-17
 */

#include "hal_uart_v151_regs_def.h"
#include "hal_uart.h"
#ifdef SUPPORT_DFX_PANIC
#include "panic.h"
#endif

#ifdef USE_CMSIS_OS
#ifdef __LITEOS__
#include "los_hwi.h"
#endif
#endif

/**
* IRQ numbers assigned to each UART supported by the core
*/

#if (UART_BUS_MAX_NUMBER > 4)
#error "The specified number of UARTS exceeds the available UART peripherals"
#endif /* (UART_BUS_MAX_NUMBER > 4) */

/*
* Resets the uarts on the protocol and apps cores using the RESET_REG register
* The security core does not have that register, this function does nothing on
* the security core
*/
void hal_uart_specific_reset_uart(uart_bus_t uart)
{
    fifo_ctl_t data_ctl;
#if defined(CONFIG_UART_SUPPORT_LPM)
    uint32_t uart_rx_data;
    uint32_t uart_rx_cnt = g_hal_uart_reg[uart]->rx_fifo_cnt;
    while (uart_rx_cnt != 0) {
        uart_rx_data = g_hal_uart_reg[uart]->data;
        uart_rx_cnt = g_hal_uart_reg[uart]->rx_fifo_cnt;
    }
    unused(uart_rx_data);
#endif
    data_ctl = (fifo_ctl_t)g_hal_uart_reg[uart]->fifo_ctl;
    data_ctl.fifo_ctl.rx_fifo_rst = 1;
    data_ctl.fifo_ctl.tx_fifo_rst = 1;
    g_hal_uart_reg[uart]->fifo_ctl = data_ctl.d32;
}

/*
* set uart dma_mode, 0x0 dma extra diabled, 0x1 dma extra enbale
*/
void hal_uart_set_dmaen(uart_bus_t bus, uint32_t val)
{
    uart_parameter_t uartdmacr;
    uartdmacr.d32 = g_hal_uart_reg[bus]->uart_parameter;
    uartdmacr.uart_parameter.dma_mode = val;
    g_hal_uart_reg[bus]->uart_parameter = uartdmacr.d32;
}