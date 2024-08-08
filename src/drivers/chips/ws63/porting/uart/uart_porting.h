/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides uart port template \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#ifndef UART_PORT_H
#define UART_PORT_H
#include <stdint.h>
#include "td_type.h"
#include "platform_core.h"
#include "std_def.h"
#include "debug_print.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_uart UART
 * @ingroup  drivers_port
 * @{
 */

#define UART_CLOCK_FRQ 24000000

#define UART_L_BAUDRATE  115200UL
#define UART_BUS_MAX_NUM UART_BUS_MAX_NUMBER

#define UART_SHADOW  YES
#define UART_DLF_SIZE  6
#define UART_FIFO_ENABLED YES

/**
 * @brief  Base address list for all of the IPs.
 */
extern const uintptr_t g_uart_base_addrs[UART_BUS_MAX_NUM];

/**
 * @brief  Register hal funcs objects into hal_uart module.
 */
void uart_port_register_hal_funcs(uart_bus_t bus);

uintptr_t uart_porting_base_addr_get(uart_bus_t bus);
/**
 * @brief  Get the bus clock of specified uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @return The bus clock of specified uart.
 */
uint32_t uart_port_get_clock_value(uart_bus_t bus);

/**
 * @brief  set the bus clock of specified uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @param  [in]  clock The bus clock of specified uart.
 */
void uart_port_set_clock_value(uart_bus_t bus, uint32_t clock);

/**
 * @brief  Config the pinmux of the uarts above.
 */
void uart_port_config_pinmux(uart_bus_t bus);

/**
 * @brief  Register the interrupt of uarts.
 */
void uart_port_register_irq(uart_bus_t bus);

/**
 * @brief  unRegister the interrupt of uarts.
 */
void uart_port_unregister_irq(uart_bus_t bus);

/**
 * @brief  Handler of UART0 IRQ.
 */
void irq_uart0_handler(void);

/**
 * @brief  Handler of UART1 IRQ.
 */
void irq_uart1_handler(void);

/**
 * @brief  Handler of UART2 IRQ.
 */
void irq_uart2_handler(void);

/**
 * @brief  Handler of UART3 IRQ.
 */
void irq_uart3_handler(void);

#if defined(CONFIG_UART_SUPPORT_DMA)
/**
 * @brief  Get the DMA destination handshaking select of uart transfer.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @return The DMA destination handshaking select of uart transfer.
 */
uint8_t uart_port_get_dma_trans_dest_handshaking(uart_bus_t bus);

/**
 * @brief  Get the DMA source handshaking select of uart transfer.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @return The DMA source handshaking select of uart transfer.
 */
uint8_t uart_port_get_dma_trans_src_handshaking(uart_bus_t bus);
#endif  /* CONFIG_UART_SUPPORT_DMA */
void hal_uart_clear_pending(uart_bus_t uart);

/**
 * @brief Force trigger uart interrupt.
 */
void uart_port_set_pending_irq(uart_bus_t uart);

/**
 * @brief  uart lock.
 * @param bus uart bus.
 */
uint32_t uart_porting_lock(uart_bus_t bus);

/**
 * @brief  uart unlock.
 * @param bus uart bus.
 * @param irq_sts irq status.
 */
void uart_porting_unlock(uart_bus_t bus, uint32_t irq_sts);

#ifdef LOG_SUPPORT
/**
 * @brief  hso uart init
 *
 * @param void
 */
void log_uart_port_init(void);
#endif
#ifdef ASIC_SMOKE_TEST
void uart2_init(uint32_t baud_rate);
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif