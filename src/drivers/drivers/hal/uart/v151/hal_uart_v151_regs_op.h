/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  UART HAL Header.
 *
 * Create:  2023-02-17
 */

#ifndef HAL_UART_V151_REGS_OP_H
#define HAL_UART_V151_REGS_OP_H

#include "platform_core.h"
#include "hal_uart_v151_regs_def.h"
#include "hal_uart.h"

/** @defgroup drivers_hal_uart_v151_regs_op UART V151 Regs Operation
  * @ingroup  connectivity_drivers_hal
  * @{
  */
/**
 * @brief  Bitmasks used to configure which UART features to enable in
 */
#define HAL_UART_ENABLE_FLAG_NONE 0
#define HAL_UART_ENABLE_FLAG_RX   (1U << 0)
#define HAL_UART_ENABLE_FLAG_TX   (1U << 1)
#define HAL_UART_ENABLE_FLAG_CTS  (1U << 2)
#define HAL_UART_ENABLE_FLAG_RTS  (1U << 3)

extern uart_reg_t* g_hal_uart_reg[UART_BUS_MAX_NUMBER];

/** FIFO Status */
typedef enum {
    HAL_UART_FIFO_DISABLED,
    HAL_UART_FIFO_ENABLED,
} hal_uart_fifo_t;

/** Programmable THRE Interrupt Mode */
typedef enum {
    HAL_UART_PTIM_MODE_DISABLED,
    HAL_UART_PTIM_MODE_ENABLED,
} hal_uart_ptim_mode_t;

/** Auto Flow Control Status */
typedef enum {
    HAL_UART_AUTO_FLOW_CTL_DISABLED,
    HAL_UART_AUTO_FLOW_CTL_ENABLED,
} hal_uart_auto_flow_ctl_t;

/** Sample baud rate */
typedef enum {
    HAL_UART_SAMPLE_BAUD_RATE_EIGHT = 0x7,
    HAL_UART_SAMPLE_BAUD_RATE_SIXTEEN = 0xf,
} hal_uart_sample_baud_rate_t;

/** Low power request enable */
typedef enum {
    HAL_UART_LOW_POWER_REQ_DISABLED,
    HAL_UART_LOW_POWER_REQ_ENABLED,
} hal_uart_low_power_req_t;

// uart dma config
typedef enum {
    HAL_UART_DMA_DISABLE = 0,    // diable tx and rx
    HAL_UART_DMA_RX_EN = 1,      // enable rx
    HAL_UART_DMA_TX_EN = 2,      // enable tx
    HAL_UART_DMA_RX_DISABLE = 3, // diable rx
    HAL_UART_DMA_TX_DISABLE = 4, // diable tx
} hal_uart_dma_config_t;


/** Uart interrupt types */
typedef enum {
    HAL_UART_INTERRUPT_MODEM_STATUS = 0x0,
    HAL_UART_INTERRUPT_NO_INTERRUPT = 0x1,
    HAL_UART_INTERRUPT_TX = 0x2,
    HAL_UART_INTERRUPT_RX = 0x4,
    HAL_UART_INTERRUPT_RECEIVER_LINE_STATUS = 0x6,
    HAL_UART_INTERRUPT_BUSY_DETECT = 0x7,
    HAL_UART_INTERRUPT_CHAR_TIMEOUT = 0xC,
    HAL_UART_INTERRUPT_ERROR,
    HAL_UART_INTERRUPT_IDLE
} hal_uart_interrupt_t;

/* Uart stop bits modes available */
typedef enum {
    UART_STOP_BITS_1 = 0, /* !< 1 Stop bit */
    UART_STOP_BITS_2 = 1, /* !< 2 Stop bits */
} uart_stop_bits_t;

typedef enum {
    UART_ERR_INFO_UARTLCR_H = 0,
    UART_ERR_INFO_UARTCR = 1,
    UART_ERR_INFO_UARTIBRD = 2,
    UART_ERR_INFO_UARTFBRD = 3,
    UART_ERR_INFO_UARTDMACR = 4,
    UART_ERR_INFO_UARTRSR = 5,
    UART_ERR_INFO_UARTRIS = 6,
    UART_ERR_INFO_INT_FLAG = 7,
    UART_ERROR_MAX_INFO_NUM /* The number of uart frame or parity error information */
} uart_err_info_index;

#if defined(CONFIG_UART_SUPPORT_DMA)
#define UART_INT_RTI HAL_UART_INTERRUPT_RX
#define UART_INT_RXI HAL_UART_INTERRUPT_CHAR_TIMEOUT
#define UARTIMSC 0
void hal_uart_uartifls_set_rxiflsel(uart_bus_t bus, uint32_t val);
void hal_uart_int_set(uart_bus_t bus, uint32_t reg, uint32_t int_id, uint32_t val);
#endif

/**
 * @brief  Sets FIFO enabled or not.
 * @param  uart Uart bus.
 * @param  fifo FIFO state.
 * @return None.
 */
void hal_uart_set_fifo_en(uart_bus_t uart, hal_uart_fifo_t fifo);

/**
 * @brief  Obtains the RX FIFO trigger level in bytes, as configured by hal_uart_init_fifo
 * @param  uart Uart bus.
 * @return The currently configured FIFO trigger level
 */
uint16_t hal_uart_get_rx_fifo_level(uart_bus_t uart);

/**
 * @brief  Returns a pointer to the data register to write or read from.
 * @param  uart Uart bus.
 * @return data pointer.
 */
volatile uint32_t *hal_uart_get_data_register(uart_bus_t uart);

/**
 * @brief  Returns a pointer to the error register to read from.
 * @return error register value.
 */
uint32_t hal_uart_get_error_status(uart_bus_t uart);

/**
 * @brief  Sets the baud rate.
 * @param  uart Uart bus.
 * @param  baud Baud rate.
 * @param  uart_clk Uart clock.
 * @return error code
 */
void hal_uart_set_baud_rate(uart_bus_t uart, uint32_t baud, uint32_t uart_clk);

/**
 * @brief  Reads the baud rate back from the registers
 * @param  uart The Uart bus to read from
 * @param  uart_clk Uart clock.
 * @return The current baud rate
 */
uint32_t hal_uart_get_baud_rate(uart_bus_t uart, uint32_t uart_clk);

/**
 * @brief  Sets the data bits to use in the uart.
 * @param  uart Uart bus.
 * @param  bits Data bits.
 */
void hal_uart_set_data_bits(uart_bus_t uart, hal_uart_data_bit_t bits);

/**
 * @brief  Sets the data bits to use in the uart.
 * @param  uart Uart bus.
 * @param  bits Data bits.
 */
void hal_uart_set_stop_bits(uart_bus_t uart, hal_uart_stop_bit_t bits);

/**
 * @brief  Sets the parity bits to use in the uart.
 * @param  uart Uart bus.
 * @param  parity Parity bits.
 */
void hal_uart_set_parity(uart_bus_t uart, hal_uart_parity_t parity);

/**
 * @brief  Disables the uart.
 * @param  uart Uart bus.
 */
void hal_uart_disable_uart(uart_bus_t uart);

/**
 * @brief  Enables the uart interrupt type specified
 * @param  uart Uart bus.
 * @param  interrupt_type Uart interrupt type
 * @param  val Enable or disable
 */
void hal_uart_enable_interrupt(uart_bus_t uart, hal_uart_interrupt_t interrupt_type, bool val);

/**
 * @brief  Disables the uart interrupt type specified
 * @param  uart
 * @param  interrupt_type
 */
void hal_uart_disable_interrupt(uart_bus_t uart, hal_uart_interrupt_t interrupt_type);

/**
 * @brief  Clears the uart interrupt type specified.
 * @param  uart Uart bus.
 * @param  interrupt_type Uart interrupt type.
 */
void hal_uart_clear_interrupt(uart_bus_t uart, hal_uart_interrupt_t interrupt_type);

/**
 * @brief  Returns the raw fifo status of the uart.
 * @param  uart
 * @return the raw fifo status of the uart.
 */
uint32_t hal_uart_get_fifo_status(uart_bus_t uart);

/**
 * @brief  Forces the pending excecution of the tx isr in an interrupt context.
 * @param  uart Uart bus
 */
void hal_uart_force_tx_isr(uart_bus_t uart);

/**
 * @brief  Forces the pending excecution of the idle isr in an interrupt context.
 * @param  uart Uart bus
 */
void hal_uart_force_idle_isr(uart_bus_t uart);

/**
 * @brief  Switch the TRX of the specified UART, uart_l default value PIN50: rx; PIN51: tx.
 * uart_h default value PIN36: tx; PIN37: rx. uart_m default value PIN30: tx; PIN31: rx.
 * @param  uart The uart to switch.
 * @param  switch_trx false: the default uart trx status; true: reverse the uart trx status.
 * @return The result of switched. true: switch the trx; false: switch to default value.
 */
bool hal_uart_switch_trx(uart_bus_t uart, bool switch_trx);

/**
 * @brief  Generic ISR for UARTs called from the specific handler.
 * @param  uart Uart that triggered the interrupt
 */
void hal_uart_isr(uart_bus_t uart);

/**
 * @brief  Clear the pending status of the corresponding uart
 * @param  uart The uart needs to be clear pending.
 */
void hal_uart_clear_pending(uart_bus_t uart);

/**
 * @brief  Enable/disable uart parity check.
 * @param  uart The uart needs to be enable/disable uart parity check.
 * @param  val True means enable uart parity check, false means disable.
 */
void hal_uart_check_en(uart_bus_t uart, bool val);

/**
 * @brief  Select uart odd/even parity check.
 * @param  uart The uart needs to be enable/disable uart parity check.
 * @param  val Selected  parity check mods.
 */
void hal_uart_parity_en(uart_bus_t uart, hal_uart_parity_t val);

/**
 * @brief  Enable/disable uart sir mode.
 * @param  uart The uart needs to be enable/disable uart sir mode.
 * @param  val True means enable uart sir mode, false means disable.
 */
void hal_uart_sir_mode_en(uart_bus_t uart, bool val);

/**
 * @brief  Enable/disable uart tx pause.
 * @param  uart The uart needs to be enable/disable uart tx pause.
 * @param  val True means enable uart tx pause, false means disable.
 */
void hal_uart_tx_pause_en(uart_bus_t uart, bool val);

/**
 * @brief  Sets the sample baud rate to use in the uart.
 * @param  uart Uart bus.
 * @param  val Sample baud rate.
 */
void hal_uart_set_baud_div(uart_bus_t uart, uint8_t val);

/**
 * @brief  Returns the sample baud rate of the uart.
 * @param  uart
 * @return the sample baud rate of the uart.
 */
uint32_t hal_uart_get_baud_div(uart_bus_t uart);

#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
/**
 * @brief  Enable/disable uart rx.
 * @param  uart The uart needs to be enable/disable uart rx.
 * @param  val True means enable uart rx, false means disable.
 */
void hal_uart_rx_en(uart_bus_t uart, bool val);

/**
 * @brief  Enable/disable uart low power request.
 * @param  uart The uart needs to be enable/disable uart low power request.
 * @param  val True means enable uart low power request, false means disable.
 */
void hal_uart_set_lp_req_en(uart_bus_t uart, bool val);
#endif

/**
 * @brief  Get the error status
 * @param  uart The uart needs to get err status.
 * @return Error status
 */
uint32_t hal_uart_get_error_status(uart_bus_t uart);

/**
 * @brief  Get the parity error status
 * @param  uart The uart needs to get err status.
 * @return True if there are parity error, otherwise false
 */
bool hal_uart_get_error_status_parity(uart_bus_t uart);

/**
 * @brief  Get the frame error status
 * @param  uart The uart needs to get err status.
 * @return True if there are frame error, otherwise false
 */
bool hal_uart_get_error_status_fram(uart_bus_t uart);

/**
 * @brief  Get the break error status
 * @param  uart The uart needs to get err status.
 * @return True if there are break error, otherwise false
 */
bool hal_uart_get_error_status_break(uart_bus_t uart);

/**
 * @brief  Get the overrun error status
 * @param  uart The uart needs to get err status.
 * @return True if there are overrun error, otherwise false
 */
bool hal_uart_get_error_status_overrun(uart_bus_t uart);

/**
 * @brief  set uart dma_mode
 * @param bus The uart needs to set dma
 * @param val 0x0 dma extra diabled, 0x1 dma extra enbale
 */
void hal_uart_set_dmaen(uart_bus_t bus, uint32_t val);

static inline bool hal_uart_get_cpr_shadow(uart_bus_t uart)
{
    return ((uart_parameter_t)g_hal_uart_reg[uart]->uart_parameter).uart_parameter.shadow;
}

static inline uint16_t hal_uart_get_cpr_fifo_mode(uart_bus_t uart)
{
    return((uart_parameter_t)g_hal_uart_reg[uart]->uart_parameter).uart_parameter.fifo_depth;
}

static inline uint16_t hal_uart_get_cpr_afce_mode(uart_bus_t uart)
{
    return((uart_parameter_t)g_hal_uart_reg[uart]->uart_parameter).uart_parameter.afce_mode;
}

static inline uint16_t hal_uart_get_cpr_apb_data_width(uart_bus_t uart)
{
    return((uart_parameter_t)g_hal_uart_reg[uart]->uart_parameter).uart_parameter.apb_data_width;
}

static inline uint16_t hal_uart_get_cpr_dma_extra(uart_bus_t uart)
{
    return((uart_parameter_t)g_hal_uart_reg[uart]->uart_parameter).uart_parameter.dma_mode;
}

/**
  * @}
  */
#endif
