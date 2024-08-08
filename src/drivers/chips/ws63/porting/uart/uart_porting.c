/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides uart port template \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include "uart_porting.h"
#include "hal_uart_v151.h"
#ifdef SUPPORT_HAL_PINCTRL
#include "pinctrl_porting.h"
#include "pinctrl.h"
#endif
#include "uart.h"
#include "platform_core.h"
#include "securec.h"
#include "osal_interrupt.h"
#include "soc_osal.h"
#if defined(CONFIG_UART_SUPPORT_DMA)
#include "dma_porting.h"
#endif

#ifdef CONFIG_UART_SUPPORT_PORTTING_IRQ
#include "interrupt_porting.h"
#endif
#ifdef LOG_SUPPORT
#include "log_oam_msg.h"
#endif

#if !defined(BUILD_NOOSAL)
#if defined(USE_ALIOS)
#include "cmsis_os2.h"
#else
#include "cmsis_os.h"
#endif
#endif

#define UART_BUS_0_BASE_ADDR (UART0_BASE - 4)
#define UART_BUS_1_BASE_ADDR (UART1_BASE - 4)
#define UART_BUS_2_BASE_ADDR (UART2_BASE - 4)
#define GPIO12_PAD_CTRL_GROUP_ADDR 0x4400D030
#define GPIO13_PAD_CTRL_GROUP_ADDR 0x4400D034

#define UART_TRANS_LEN_MAX   128

const uintptr_t g_uart_base_addrs[UART_BUS_MAX_NUM] = {
    (uintptr_t)UART_BUS_0_BASE_ADDR,
    (uintptr_t)UART_BUS_1_BASE_ADDR,
    (uintptr_t)UART_BUS_2_BASE_ADDR
};
#if !defined(BUILD_NOOSAL)
static osal_mutex g_uart_tx_mutex[UART_BUS_MAX_NUMBER] = {0};
const bool g_uart_support_mutex[UART_BUS_MAX_NUMBER] = {
    CONFIG_UART0_SUPPORT_WRITE_MUTEX,
    CONFIG_UART1_SUPPORT_WRITE_MUTEX,
    CONFIG_UART2_SUPPORT_WRITE_MUTEX,
};
#endif
uintptr_t uart_porting_base_addr_get(uart_bus_t bus)
{
    return g_uart_base_addrs[bus];
}

typedef struct uart_interrupt {
    core_irq_t irq_num;
    osal_irq_handler irq_func;
} hal_uart_interrupt_t;

#ifdef CONFIG_UART_SUPPORT_PORTTING_IRQ
static const hal_uart_interrupt_t g_uart_interrupt_lines[UART_BUS_MAX_NUMBER] = {
#if UART_BUS_MAX_NUMBER > 0
    { UART_0_IRQN, (osal_irq_handler)irq_uart0_handler },
#endif
#if UART_BUS_MAX_NUMBER > 1
    { UART_1_IRQN, (osal_irq_handler)irq_uart1_handler },
#endif
#if UART_BUS_MAX_NUMBER > 2
    { UART_2_IRQN, (osal_irq_handler)irq_uart2_handler },
#endif
#if UART_BUS_MAX_NUMBER > 3
    { UART_3_IRQN, (osal_irq_handler)irq_uart3_handler },
#endif
};
#endif

void uart_port_register_hal_funcs(uart_bus_t bus)
{
    hal_uart_register_funcs(bus, hal_uart_v151_funcs_get());
}

#ifdef SUPPORT_CLOCKS_CORE
#include "clocks_core_common.h"
#endif
#include "debug_print.h"

static uint32_t g_uart_clock_value = UART_CLOCK_FRQ;

void uart_port_set_clock_value(uart_bus_t bus, uint32_t clock)
{
    unused(bus);
    g_uart_clock_value = clock;
}

uint32_t uart_port_get_clock_value(uart_bus_t bus)
{
#ifdef SUPPORT_CLOCKS_CORE
    return uart_get_clock_value(bus);
#else
    UNUSED(bus);
    return g_uart_clock_value;
#endif
}

#define GPIO_05_SEL   0x4400d014
#define GPIO_06_SEL   0x4400d018
#define GPIO_07_SEL   0x4400d01c
#define GPIO_08_SEL   0x4400d020
#define UART_2_MODE   2
#define GPIO_13_SEL   0x4400d034
#define GPIO_14_SEL   0x4400d038
#define UART1_TXD_SEL 0x4400d03c
#define UART1_RXD_SEL 0x4400d040
#define UART_1_MODE   1
#define UART0_TXD_SEL 0x4400d044
#define UART0_RXD_SEL 0x4400d048
#define UART_0_MODE   1
void uart_port_config_pinmux(uart_bus_t bus)
{
#ifndef BOARD_ASIC
    // uart2需要配管脚复用
    if (bus == UART_BUS_2) {
        uapi_reg_write32(GPIO12_PAD_CTRL_GROUP_ADDR, 0x1);
        uapi_reg_write32(GPIO13_PAD_CTRL_GROUP_ADDR, 0x1);
    }
    return;
#else
    if (bus == UART_BUS_2) {
        writel(GPIO_05_SEL, UART_2_MODE);
        writel(GPIO_06_SEL, UART_2_MODE);
        writel(GPIO_07_SEL, UART_2_MODE);
        writel(GPIO_08_SEL, UART_2_MODE);
    } else if (bus == UART_BUS_1) {
        writel(GPIO_13_SEL, UART_1_MODE);
        writel(GPIO_14_SEL, UART_1_MODE);
        writel(UART1_TXD_SEL, UART_1_MODE);
        writel(UART1_RXD_SEL, UART_1_MODE);
    } else if (bus == UART_BUS_0) {
        writel(UART0_TXD_SEL, UART_0_MODE);
        writel(UART0_RXD_SEL, UART_0_MODE);
    }
#endif
#if !defined(BUILD_NOOSAL)
    if (bus < UART_BUS_MAX_NUMBER && g_uart_support_mutex[bus] == true && g_uart_tx_mutex[bus].mutex == NULL) {
        osal_mutex_init(&g_uart_tx_mutex[bus]);
    }
#endif
}

#ifdef CONFIG_UART_SUPPORT_PORTTING_IRQ
void uart_port_register_irq(uart_bus_t bus)
{
    osal_irq_request(g_uart_interrupt_lines[bus].irq_num, g_uart_interrupt_lines[bus].irq_func, NULL, NULL, NULL);
    osal_irq_set_priority(g_uart_interrupt_lines[bus].irq_num, irq_prio(g_uart_interrupt_lines[bus].irq_num));
    osal_irq_enable(g_uart_interrupt_lines[bus].irq_num);
}

void irq_uart0_handler(void)
{
    hal_uart_irq_handler(UART_BUS_0);
    osal_irq_clear(UART_0_IRQN);
}

void irq_uart1_handler(void)
{
    hal_uart_irq_handler(UART_BUS_1);
    osal_irq_clear(UART_1_IRQN);
}

void irq_uart2_handler(void)
{
    hal_uart_irq_handler(UART_BUS_2);
    osal_irq_clear(UART_2_IRQN);
}

void uart_port_unregister_irq(uart_bus_t bus)
{
    osal_irq_disable(g_uart_interrupt_lines[bus].irq_num);
}

void uart_port_set_pending_irq(uart_bus_t uart)
{
    int_set_pendind_irq(g_uart_interrupt_lines[uart].irq_num);
}
#else
void uart_port_register_irq(uart_bus_t bus)
{
    unused(bus);
}

void uart_port_unregister_irq(uart_bus_t bus)
{
    unused(bus);
}
#endif

#ifdef SW_UART_DEBUG
#define DEBUG_UART_RX_BUFFER_SIZE 1
STATIC uart_bus_t g_sw_debug_uart;
static bool g_sw_debug_uart_enabled = false;
STATIC uint8_t g_uart_rx_buffer[DEBUG_UART_RX_BUFFER_SIZE];

static void uart_rx_callback(const void *buf, uint16_t buf_len, bool remaining);

void uart_rx_callback(const void *buf, uint16_t buf_len, bool remaining)
{
    UNUSED(remaining);
    if (!g_sw_debug_uart_enabled) {
        return;
    }
    uapi_uart_write(SW_DEBUG_UART_BUS, (const void *)buf, buf_len, 0);
}

void sw_debug_uart_init(uint32_t baud_rate)
{
    uart_pin_config_t uart_pins;
    uart_attr_t uart_line_config;
    uart_buffer_config_t uart_buffer_config;

    g_sw_debug_uart = SW_DEBUG_UART_BUS;
    // TX configuration
    uart_pins.tx_pin = CHIP_FIXED_TX_PIN;
    uart_pins.rts_pin = PIN_NONE;

    // RX configuration
    uart_pins.rx_pin = CHIP_FIXED_RX_PIN;
    uart_pins.cts_pin = PIN_NONE;

    uart_line_config.baud_rate = baud_rate;
    uart_line_config.data_bits = UART_DATA_BIT_8;
    uart_line_config.parity = UART_PARITY_NONE;
    uart_line_config.stop_bits = UART_STOP_BIT_1;

    uart_buffer_config.rx_buffer_size = DEBUG_UART_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_uart_rx_buffer;

    (void)uapi_uart_init(SW_DEBUG_UART_BUS, &uart_pins, &uart_line_config, NULL, &uart_buffer_config);
    uapi_uart_register_rx_callback(SW_DEBUG_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   DEBUG_UART_RX_BUFFER_SIZE, uart_rx_callback);

    g_sw_debug_uart_enabled = true;
}
#ifdef ASIC_SMOKE_TEST
STATIC uart_bus_t g_uart2;
static bool g_uart2_enabled = false;
STATIC uint8_t g_uart2_rx_buffer[DEBUG_UART_RX_BUFFER_SIZE];
#define DEBUG_UART2_RX_BUFFER_SIZE 1
void uart2_rx_callback(const void *buf, uint16_t buf_len, bool remaining)
{
    UNUSED(remaining);
    if (!g_uart2_enabled) {
        return;
    }
    uapi_uart_write(CHIP_FIXED_UART_BUS, (const void *)buf, buf_len, 0);
}
void uart2_init(uint32_t baud_rate)
{
    uart_pin_config_t uart_pins;
    uart_attr_t uart_line_config;
    uart_buffer_config_t uart_buffer_config;

    g_uart2 = CHIP_FIXED_UART_BUS;
    // TX configuration
    uart_pins.tx_pin = CHIP_FIXED_TX_PIN;
    uart_pins.rts_pin = PIN_NONE;

    // RX configuration
    uart_pins.rx_pin = CHIP_FIXED_RX_PIN;
    uart_pins.cts_pin = PIN_NONE;

    uart_line_config.baud_rate = baud_rate;
    uart_line_config.data_bits = UART_DATA_BIT_8;
    uart_line_config.parity = UART_PARITY_NONE;
    uart_line_config.stop_bits = UART_STOP_BIT_1;

    uart_buffer_config.rx_buffer_size = DEBUG_UART2_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_uart2_rx_buffer;

    (void)uapi_uart_init(CHIP_FIXED_UART_BUS, &uart_pins, &uart_line_config, NULL, &uart_buffer_config);
    uapi_uart_register_rx_callback(CHIP_FIXED_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   DEBUG_UART2_RX_BUFFER_SIZE, uart2_rx_callback);

    g_uart2_enabled = true;
}
#endif

#ifdef SW_UART_CHIP_DEFINE
void UartPuts(const char *s, uint32_t len, bool is_lock)
{
    if ((s == NULL) || (strlen(s) == 0)) {
        return;
    }

    UNUSED(is_lock);
#ifdef SW_UART_DEBUG
    uapi_uart_write(SW_DEBUG_UART_BUS, (const void *)s, len, 0);
#elif defined(TEST_SUITE)
    test_suite_uart_send(s);
#elif defined(SW_RTT_DEBUG)
    SEGGER_RTT_Write(0, (const char *)s, len);
#else
    UNUSED(s);
    UNUSED(len);
#endif
}
#endif

static void print_str_inner(const char *fmt, va_list ap)
{
    int32_t len;
    static char str_buf[UART_TRANS_LEN_MAX] = {0};
    char *tmp_buf = NULL;
    uint32_t buflen = UART_TRANS_LEN_MAX;

    tmp_buf = str_buf;
    len = vsnprintf_s(tmp_buf, buflen, buflen - 1, fmt, ap);
    if ((len == -1) && (*tmp_buf == '\0')) {
        /* parameter is illegal or some features in fmt dont support */
        return;
    }
#ifdef CONFIG_PRINTF_BUFFER_DYNAMIC
    const char *errmsgmalloc = "print long str, malloc failed!\n";
    while (len == -1) {
        /* tmp_buf is not enough */
        if (buflen != UART_TRANS_LEN_MAX) {
            osal_kfree(tmp_buf);
        }

        buflen = buflen << 1;
        tmp_buf = (char *)osal_kmalloc(buflen, OSAL_GFP_KERNEL);
        if (tmp_buf == NULL) {
            uapi_uart_write(SW_DEBUG_UART_BUS, (const uint8_t *)errmsgmalloc, (uint32_t)strlen(errmsgmalloc), 0);
            return;
        }
        len = vsnprintf_s(tmp_buf, buflen, buflen - 1, fmt, ap);
        if (*tmp_buf == '\0') {
            /* parameter is illegal or some features in fmt dont support */
            osal_kfree(tmp_buf);
            return;
        }
    }
#endif
    *(tmp_buf + len) = '\0';
    uapi_uart_write(SW_DEBUG_UART_BUS, (const uint8_t *)tmp_buf, len, 0);
#ifdef CONFIG_PRINTF_BUFFER_DYNAMIC
    if (buflen != UART_TRANS_LEN_MAX) {
        osal_kfree(tmp_buf);
    }
#endif
}

void print_str(const char *str, ...)
{
    va_list args;
    if ((str == NULL) || (strlen(str) == 0)) {
        return;
    }
    va_start(args, str);
    print_str_inner(str, args);
    va_end(args);
}

#endif

#ifdef LOG_SUPPORT
/** UART Settings. Define these in the C file to avoid pulling in the UART header in the header file. */
#define LOG_UART_RX_MAX_BUFFER_SIZE 16
static uint8_t g_uart_log_rx_buffer[LOG_UART_RX_MAX_BUFFER_SIZE];
void log_uart_port_init(void)
{
    uart_pin_config_t log_uart_pins = {
        .tx_pin = CODELOADER_UART_TX_PIN,
        .rx_pin = CODELOADER_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    uart_attr_t uart_line_config = {
        .baud_rate = CONFIG_LOG_UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };
    uart_buffer_config_t uart_buffer_config;

    uart_buffer_config.rx_buffer_size = LOG_UART_RX_MAX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_uart_log_rx_buffer;

    (void)uapi_uart_init(LOG_UART_BUS, &log_uart_pins, &uart_line_config, NULL, &uart_buffer_config);

#if SYS_DEBUG_MODE_ENABLE == YES
    uapi_uart_register_rx_callback(LOG_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   LOG_UART_RX_MAX_BUFFER_SIZE, log_uart_rx_callback);
#endif
}
#endif

#ifdef CONFIG_UART_SUPPORT_PORTTING_IRQ
void hal_uart_clear_pending(uart_bus_t uart)
{
    switch (uart) {
#if UART_BUS_MAX_NUMBER > 0
        case UART_BUS_0:
            osal_irq_clear(UART_0_IRQN);
            break;
#endif
#if UART_BUS_MAX_NUMBER > 1
        case UART_BUS_1:
            osal_irq_clear(UART_1_IRQN);
            break;
#endif
#if UART_BUS_MAX_NUMBER > 2
        case UART_BUS_2:
            osal_irq_clear(UART_2_IRQN);
            break;
#endif
#if UART_BUS_MAX_NUMBER > 3
        case UART_BUS_3:
            int_clear_pending_irq(UART_3_IRQN);
            break;
#endif
        default:
            break;
    }
}
#endif

#if defined(CONFIG_UART_SUPPORT_DMA)
uint8_t uart_port_get_dma_trans_dest_handshaking(uart_bus_t bus)
{
    switch (bus) {
        case UART_BUS_0:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_L_TX;
        case UART_BUS_1:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_H0_TX;
        case UART_BUS_2:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_H1_TX;
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
    return 0;
}

uint8_t uart_port_get_dma_trans_src_handshaking(uart_bus_t bus)
{
    switch (bus) {
        case UART_BUS_0:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_L_TX;
        case UART_BUS_1:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_H0_RX;
        case UART_BUS_2:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_H1_RX;
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
    return 0;
}
#endif  /* CONFIG_UART_SUPPORT_DMA */

uint32_t uart_porting_lock(uart_bus_t bus)
{
#if !defined(BUILD_NOOSAL)
    if ((g_uart_support_mutex[bus] == true) && (osKernelGetState() == osKernelRunning) &&
        (osal_in_interrupt() == 0)) {
        return (uint32_t)osal_mutex_lock_timeout(&g_uart_tx_mutex[bus], OSAL_MUTEX_WAIT_FOREVER);
    } else {
        return 0;
    }
#else
    unused(bus);
    return osal_irq_lock();
#endif
}

void uart_porting_unlock(uart_bus_t bus, uint32_t irq_sts)
{
#if !defined(BUILD_NOOSAL)
    unused(irq_sts);
    if ((g_uart_support_mutex[bus] == true) && (osKernelGetState() == osKernelRunning) &&
        (osal_in_interrupt() == 0)) {
        osal_mutex_unlock(&g_uart_tx_mutex[bus]);
    }
#else
    unused(bus);
    osal_irq_restore(irq_sts);
#endif
}
