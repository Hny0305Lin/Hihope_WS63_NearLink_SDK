/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  UART HAL Driver.
 *
 * Create:  2023-2-17
 */

#include "chip_io.h"
#include "soc_osal.h"
#include "platform_types.h"
#include "hal_uart.h"
#include "hal_uart_v151_regs_def.h"
#include "hal_uart_v151_regs_op.h"
#include "tcxo.h"

#define hal_uart_get_high_8bit(val) ((val) >> 8)
#define hal_uart_get_low_8bit(val) ((val) & 0xff)
#define round_off(val) ((uint32_t)((val) + 1 / 2))

#define HAL_UART_FIFO_DEPTH_SHIFT_2    2
#define HAL_UART_FIFO_DEPTH_MINUS_2    2
#define HAL_UART_FIFO_DEPTH_MULTIPLE   16

#define HAL_UART_BARD_LEFT_SHIFT_2 2
#define HAL_UART_BARD_LEFT_SHIFT_6 6
#define HAL_UART_INIT_DELAY_10US   10

#define IBRD_NEED_BAUD_OFFSET_NUM      4
#define REMAINDER_NEED_BAUD_OFFSET_NUM 4
#define HAL_UART_GET_BAUD_RATE_SHIFT   8

/** Uart FIFO Level */
typedef enum {
    HAL_UART_RX_FIFO_AVAILABLE_LEVEL_EQ_1 = 0,            // 1 character in FIFO
    HAL_UART_TX_FIFO_EMPTY_LEVEL_EQ_0 = 0,                // FIFO Empty
    HAL_UART_RX_FIFO_AVAILABLE_LEVEL_1_4 = 1,             // FIFO 1/4 full
    HAL_UART_TX_FIFO_EMPTY_LEVEL_EQ_2 = 1,                // 2 characters in FIFO
    HAL_UART_RX_FIFO_AVAILABLE_LEVEL_1_2 = 2,       // FIFO 1/2 full
    HAL_UART_TX_FIFO_EMPTY_LEVEL_1_4 = 2,                 // FIFO 1/4 full
    HAL_UART_RX_FIFO_AVAILABLE_LEVEL_LESS_2 = 3,          // FIFO 2 less than full
    HAL_UART_TX_FIFO_EMPTY_LEVEL_1_2 = 3,                 // FIFO 1/2 full
} hal_uart_fifo_int_lvl_t;

/**
 * @brief  Sets the FIFO interrupt leves for receiving and transmitting.
 * @param  uart Uart bus.
 * @param  rx_level Level at which the receive interrupt interrupt will be triggered.
 * @param  tx_level Level at which the transmit interrupt interrupt will be triggered.
 */
void hal_uart_init_fifo(uart_bus_t uart, hal_uart_fifo_int_lvl_t rx_level, hal_uart_fifo_int_lvl_t tx_level);

void hal_uart_set_data_bits(uart_bus_t uart, hal_uart_data_bit_t bits);

/**
 * @brief  UART TX interrupt and DMA request signal trigger by TX fifo level.
 * @param  uart Uart bus.
 * @param  value Programmable the interrupt mode enable or disable
 */
void hal_uart_set_ptim_en(uart_bus_t uart, bool value);

void hal_uart_auto_flow_ctl_en(uart_bus_t bus, hal_uart_auto_flow_ctl_t auto_flow);

/**
 * Base addresses of all UARTS supported by the core
 */
uart_reg_t *g_hal_uart_reg[UART_BUS_MAX_NUMBER];

/* flag to force the execution of the tx interrupt even if the tx condition is not triggered */
#define HAL_UART_FORCE_TX_ISR_FLAG 0x01
#define HAL_UART_FORCE_IDLE_ISR_FLAG 0x02
static uint8_t g_hal_uart_force_isr_flags[UART_BUS_MAX_NUMBER];
static hal_uart_callback_t g_hal_uart_callback[UART_BUS_MAX_NUMBER] = { 0 };

hal_uart_fifo_int_lvl_t g_hal_uart_rx_fifo_level[UART_BUS_MAX_NUMBER];
hal_uart_fifo_int_lvl_t g_hal_uart_tx_fifo_level[UART_BUS_MAX_NUMBER];

#ifdef BUILD_APPLICATION_STANDARD
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
#include "log_printf.h"
#define hal_uart_print_err(log_id, fmt, count, args...) \
    BASE_PRINT_ERR(CONNECT(LOG_BCORE_PLT_DRIVER_UART, log_id), fmt, count, ##args)
#define hal_uart_print_info(log_id, fmt, count, args...) \
    BASE_PRINT_INFO(CONNECT(LOG_BCORE_PLT_DRIVER_UART, log_id), fmt, count, ##args)
#endif
#endif
static errcode_t hal_uart_is_tx_fifo_full(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param);
static errcode_t hal_uart_is_tx_fifo_empty(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param);
static errcode_t hal_uart_is_rx_fifo_empty(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param);
static errcode_t hal_uart_is_busy(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param);
#if defined(CONFIG_UART_SUPPORT_DMA)
static errcode_t hal_uart_v151_get_dma_data_addr(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param);
#endif

static void hal_uart_init_reg_base(uart_bus_t uart)
{
    g_hal_uart_reg[uart] = (uart_reg_t *)g_uart_base_addrs[uart];
    g_hal_uart_reg[0] = (uart_reg_t *)g_uart_base_addrs[0];
}

static errcode_t hal_uart_v151_init(uart_bus_t uart, hal_uart_callback_t callback, const hal_uart_pin_config_t *pins,
                                    const hal_uart_attr_t *attr, hal_uart_flow_ctrl_t flow_ctrl)
{
    unused(pins);

    hal_uart_init_reg_base(uart);
    g_hal_uart_force_isr_flags[uart] = 0;

    hal_uart_set_data_bits(uart, attr->data_bits);
    hal_uart_set_stop_bits(uart, attr->stop_bits);
    hal_uart_set_parity(uart, attr->parity);

    // specific priority of irq associated with uart

    hal_uart_sir_mode_en(uart, false);
    hal_uart_tx_pause_en(uart, false);

    hal_uart_set_baud_rate(uart, attr->baud_rate, uart_port_get_clock_value(uart));
    uapi_tcxo_delay_us(HAL_UART_INIT_DELAY_10US);
#ifdef HSO_SUPPORT
    hal_uart_init_fifo(uart, HAL_UART_RX_FIFO_AVAILABLE_LEVEL_1_4, HAL_UART_TX_FIFO_EMPTY_LEVEL_EQ_2);
#else
    hal_uart_init_fifo(uart, HAL_UART_RX_FIFO_AVAILABLE_LEVEL_1_4, HAL_UART_TX_FIFO_EMPTY_LEVEL_EQ_2);
#endif
    g_hal_uart_callback[uart] = callback;
    if (flow_ctrl == UART_FLOW_CTRL_RTS_CTS) {
        hal_uart_auto_flow_ctl_en(uart, HAL_UART_AUTO_FLOW_CTL_ENABLED);
    } else {
        hal_uart_auto_flow_ctl_en(uart, HAL_UART_AUTO_FLOW_CTL_DISABLED);
    }

#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
    hal_uart_set_ptim_en(uart, true);
    hal_uart_rx_en(uart, true);
#endif
    return ERRCODE_SUCC;
}

#if defined(CONFIG_UART_SUPPORT_TX_INT)
static errcode_t hal_uart_en_tx_int(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    hal_uart_enable_interrupt(bus, HAL_UART_INTERRUPT_TX, param);
    return ERRCODE_SUCC;
}
#endif

static errcode_t hal_uart_en_rx_int(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    hal_uart_enable_interrupt(bus, HAL_UART_INTERRUPT_RX, param);
    return ERRCODE_SUCC;
}

static errcode_t hal_uart_en_idle_int(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    hal_uart_enable_interrupt(bus, HAL_UART_INTERRUPT_CHAR_TIMEOUT, param);
    return ERRCODE_SUCC;
}

static errcode_t hal_uart_en_para_err_int(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    hal_uart_enable_interrupt(bus, HAL_UART_INTERRUPT_ERROR, param);
    return ERRCODE_SUCC;
}

static errcode_t hal_uart_v151_ctrl_set_attr(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    hal_uart_attr_t *attr = (hal_uart_attr_t *)param;
    hal_uart_set_data_bits(bus, attr->data_bits);
    hal_uart_set_stop_bits(bus, attr->stop_bits);
    hal_uart_set_parity(bus, attr->parity);
    hal_uart_set_baud_rate(bus, attr->baud_rate, uart_port_get_clock_value(bus));
    return ERRCODE_SUCC;
}

static errcode_t hal_uart_v151_ctrl_get_rxfifo_passnum(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    unused(bus);
    if (param == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t *state = (uint32_t *)param;
    *state = 0xffffffff;
    return ERRCODE_SUCC;
}

static errcode_t hal_uart_v151_ctrl_set_rxfifo_int_level(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    uint32_t level = (uint32_t)param;
    if (level > (uint32_t)HAL_UART_RX_FIFO_AVAILABLE_LEVEL_LESS_2) {
        return ERRCODE_INVALID_PARAM;
    }

    hal_uart_init_fifo(bus, (uint32_t)level, g_hal_uart_tx_fifo_level[bus]);
    return ERRCODE_SUCC;
}

static errcode_t hal_uart_v151_ctrl_set_txfifo_int_level(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    uint32_t level = (uint32_t)param;
    if (level > (uint32_t)HAL_UART_TX_FIFO_EMPTY_LEVEL_1_2) {
        return ERRCODE_INVALID_PARAM;
    }

    hal_uart_init_fifo(bus, g_hal_uart_rx_fifo_level[bus], (uint32_t)level);
    return ERRCODE_SUCC;
}

static errcode_t hal_uart_v151_get_uart_rxfifo_threshold(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    uint32_t rx_fifo_thresh = 0;
    uint32_t *addr = (uint32_t *)param;
    switch (g_hal_uart_rx_fifo_level[bus]) {
        case HAL_UART_RX_FIFO_AVAILABLE_LEVEL_EQ_1:
            rx_fifo_thresh = 1;
            break;
        case HAL_UART_RX_FIFO_AVAILABLE_LEVEL_1_4:
            rx_fifo_thresh = (CONFIG_UART_FIFO_DEPTH >> HAL_UART_FIFO_DEPTH_SHIFT_2);
            break;
        case HAL_UART_RX_FIFO_AVAILABLE_LEVEL_1_2:
            rx_fifo_thresh = CONFIG_UART_FIFO_DEPTH >> 1;
            break;
        default:
            rx_fifo_thresh = CONFIG_UART_FIFO_DEPTH - HAL_UART_FIFO_DEPTH_MINUS_2;
            break;
    }
    *addr = rx_fifo_thresh;
    return ERRCODE_SUCC;
}

static const hal_uart_ctrl_t g_hal_uart_ctrl_func_array[UART_CTRL_MAX + 1] = {
    hal_uart_v151_ctrl_set_attr,           // UART_CTRL_SET_ATTR
    NULL,                                  // UART_CTRL_GET_ATTR
#if defined(CONFIG_UART_SUPPORT_TX_INT)
    hal_uart_en_tx_int,                    // UART_CTRL_EN_TX_INT
#endif
    hal_uart_en_rx_int,                    // UART_CTRL_EN_RX_INT
    hal_uart_en_idle_int,                  // UART_CTRL_EN_IDLE_INT
    hal_uart_en_para_err_int,              // UART_CTRL_EN_PARITY_ERR_INT
    hal_uart_en_para_err_int,              // UART_CTRL_EN_FRAME_ERR_INT
    hal_uart_is_tx_fifo_full,              // UART_CTRL_CHECK_TX_FIFO_FULL
    hal_uart_is_tx_fifo_empty,             // UART_CTRL_CHECK_TX_BUSY
    hal_uart_is_rx_fifo_empty,             // UART_CTRL_CHECK_RX_FIFO_EMPTY
    NULL,                                  // UART_CTRL_FIFO_ENABLE
    hal_uart_v151_ctrl_set_rxfifo_int_level,  // UART_CTRL_SET_RX_FIFO_LEVEL
    hal_uart_v151_ctrl_set_txfifo_int_level,  // UART_CTRL_SET_TX_FIFO_LEVEL
    NULL,                                  // UART_CTRL_GET_REG_ADDRS
    NULL,                                  // UART_CTRL_TX_DMA_PROCESS
    NULL,                                  // UART_CTRL_FLOW_CTRL
    NULL,                                  // UART_CTRL_RESTORE
    hal_uart_is_busy,                      // UART_CTRL_CHECK_UART_BUSY
    hal_uart_v151_ctrl_get_rxfifo_passnum, // UART_CTRL_GET_RX_FIFO_PASSNUM
#if defined(CONFIG_UART_SUPPORT_LPM)
    NULL,                                  // UART_CTRL_SUSPEND
    NULL,                                  // UART_CTRL_RESUME
#endif
#if defined(CONFIG_UART_SUPPORT_DMA)
    hal_uart_v151_get_dma_data_addr,
#endif
    hal_uart_v151_get_uart_rxfifo_threshold, // get uart rx fifo threshold
    NULL,                                    // UART_CTRL_MAX
};

static errcode_t hal_uart_v151_deinit(uart_bus_t uart)
{
#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
    hal_uart_rx_en(uart, false);
#endif

    hal_uart_disable_uart(uart);
    g_hal_uart_callback[uart] = NULL;
    return ERRCODE_SUCC;
}

void hal_uart_set_fifo_en(uart_bus_t uart, hal_uart_fifo_t fifo)
{
    fifo_ctl_t data = (fifo_ctl_t)g_hal_uart_reg[uart]->fifo_ctl;
    data.fifo_ctl.fifo_en = fifo;
    g_hal_uart_reg[uart]->fifo_ctl = data.d32;
}

void hal_uart_init_fifo(uart_bus_t uart, hal_uart_fifo_int_lvl_t rx_level, hal_uart_fifo_int_lvl_t tx_level)
{
    fifo_ctl_t data = (fifo_ctl_t)g_hal_uart_reg[uart]->fifo_ctl;
#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
    data.fifo_ctl.rx_empty_trig_h = CONFIG_UART_RX_EMPTY_TRIG_H;
#endif
    data.fifo_ctl.rx_empty_trig = rx_level;
    data.fifo_ctl.tx_empty_trig = tx_level;
    data.fifo_ctl.fifo_en = 1;
    g_hal_uart_reg[uart]->fifo_ctl = data.d32;
    g_hal_uart_rx_fifo_level[uart] = rx_level;
    g_hal_uart_tx_fifo_level[uart] = tx_level;
}

uint16_t hal_uart_get_fifo_depth(uart_bus_t uart)
{
    return hal_uart_get_cpr_fifo_mode(uart) * HAL_UART_FIFO_DEPTH_MULTIPLE;
}

/*
 * Obtains the RX FIFO trigger level in bytes, as configured by hal_uart_set_fifo_int_levels
 */
uint16_t hal_uart_get_rx_fifo_level(uart_bus_t uart)
{
    uint16_t fifo_depth = hal_uart_get_fifo_depth(uart);
    switch (g_hal_uart_rx_fifo_level[uart]) {
        case HAL_UART_RX_FIFO_AVAILABLE_LEVEL_EQ_1:
            return 1;
        case HAL_UART_RX_FIFO_AVAILABLE_LEVEL_1_4:
            return fifo_depth >> HAL_UART_FIFO_DEPTH_SHIFT_2;
        case HAL_UART_RX_FIFO_AVAILABLE_LEVEL_1_2:
            return fifo_depth >> 1;
        case HAL_UART_RX_FIFO_AVAILABLE_LEVEL_LESS_2:
            return fifo_depth - HAL_UART_FIFO_DEPTH_MINUS_2;
        default:
            break;
    }
    return 0;
}

volatile uint32_t *hal_uart_get_data_register(uart_bus_t uart)
{
    return (volatile uint32_t *)&(g_hal_uart_reg[uart]->data);
}

uint32_t hal_uart_get_error_status(uart_bus_t uart)
{
    return g_hal_uart_reg[uart]->line_status;
}

bool hal_uart_get_error_status_parity(uart_bus_t uart)
{
    line_status_t data = (line_status_t)g_hal_uart_reg[uart]->line_status;
    return data.line_status.parity_err;
}

bool hal_uart_get_error_status_fram(uart_bus_t uart)
{
    line_status_t data = (line_status_t)g_hal_uart_reg[uart]->line_status;
    return data.line_status.frame_err;
}

bool hal_uart_get_error_status_break(uart_bus_t uart)
{
    line_status_t data = (line_status_t)g_hal_uart_reg[uart]->line_status;
    return data.line_status.break_intr;
}

bool hal_uart_get_error_status_overrun(uart_bus_t uart)
{
    line_status_t data = (line_status_t)g_hal_uart_reg[uart]->line_status;
    return data.line_status.overrun_err;
}

static void hal_uart_set_diven(uart_bus_t uart, uint8_t val)
{
    uart_ctl_t data = (uart_ctl_t)g_hal_uart_reg[uart]->uart_ctl;
    data.uart_ctl.div_en = val;
    g_hal_uart_reg[uart]->uart_ctl = data.d32;
}

void hal_uart_set_baud_rate(uart_bus_t uart, uint32_t baud, uint32_t uart_clk)
{
    div_fra_t div_f;
    div_h_t div_h;
    div_l_t div_l;

    if (baud == 0) {
        return;
    }
    uint16_t brd_i = uart_clk / (baud << IBRD_NEED_BAUD_OFFSET_NUM);
    uint32_t remainder = uart_clk % (baud << REMAINDER_NEED_BAUD_OFFSET_NUM);
    uint16_t brd_f = round_off((remainder << UART_DLF_SIZE) / (baud << IBRD_NEED_BAUD_OFFSET_NUM));
    hal_uart_set_diven(uart, 1);
    div_l.div_l.div_l = hal_uart_get_low_8bit(brd_i);
    div_h.div_h.div_h = hal_uart_get_high_8bit(brd_i);
    g_hal_uart_reg[uart]->div_l = div_l.d32;
    g_hal_uart_reg[uart]->div_h = div_h.d32;
    div_f.div_fra.div_fra = brd_f;
    g_hal_uart_reg[uart]->div_fra = div_f.d32;
    hal_uart_set_diven(uart, 0);
}

uint32_t hal_uart_get_baud_rate(uart_bus_t uart, uint32_t uart_clk)
{
    uint32_t brd_i;
    uint32_t brd_f;
    uint32_t baud;

    hal_uart_set_diven(uart, 1);
    brd_i = g_hal_uart_reg[uart]->div_l & (g_hal_uart_reg[uart]->div_h << HAL_UART_GET_BAUD_RATE_SHIFT);
    brd_f = g_hal_uart_reg[uart]->div_fra;
    hal_uart_set_diven(uart, 0);
    /* brd_i is the integer baud divider, brd_f is fractional baud divider
     * According to hal_uart_set_baud_rate, clock could be expressed as ((16 * baud) * brd_i + brd_f * baud / 4)
     * so baud is (clock * 4) / (64 * brd_i  + brd_f)
     */
    baud = (uart_clk << HAL_UART_BARD_LEFT_SHIFT_2) / (brd_i + brd_f / (1 << UART_DLF_SIZE));
    return baud;
}

void hal_uart_set_data_bits(uart_bus_t uart, hal_uart_data_bit_t bits)
{
    uart_ctl_t data = (uart_ctl_t)g_hal_uart_reg[uart]->uart_ctl;
    data.uart_ctl.dlen = bits;
    g_hal_uart_reg[uart]->uart_ctl = data.d32;
}

void hal_uart_set_stop_bits(uart_bus_t uart, hal_uart_stop_bit_t bits)
{
    uart_ctl_t data = (uart_ctl_t)g_hal_uart_reg[uart]->uart_ctl;
    data.uart_ctl.stp = bits;
    g_hal_uart_reg[uart]->uart_ctl = data.d32;
}

void hal_uart_set_parity(uart_bus_t uart, hal_uart_parity_t parity)
{
    switch (parity) {
        case UART_PARITY_NONE:
            hal_uart_check_en(uart, false);
            break;

        case UART_PARITY_EVEN:
            hal_uart_check_en(uart, true);
            hal_uart_parity_en(uart, 1);
            break;

        case UART_PARITY_ODD:
            hal_uart_check_en(uart, true);
            hal_uart_parity_en(uart, 0);
            break;
        default:
            break;
    }
}

void hal_uart_set_ptim_en(uart_bus_t uart, bool value)
{
    intr_en_t data = (intr_en_t)g_hal_uart_reg[uart]->intr_en;
    data.intr_en.ptim_en = value;
    g_hal_uart_reg[uart]->intr_en = data.d32;
}

void hal_uart_auto_flow_ctl_en(uart_bus_t bus, hal_uart_auto_flow_ctl_t auto_flow)
{
    modem_ctl_t data = (modem_ctl_t)g_hal_uart_reg[bus]->modem_ctl;
    data.modem_ctl.afc_en = auto_flow;
    data.modem_ctl.rts = auto_flow;
    g_hal_uart_reg[bus]->modem_ctl = data.d32;
}

void hal_uart_enable_interrupt(uart_bus_t uart, hal_uart_interrupt_t interrupt_type, bool val)
{
    intr_en_t inter_en = (intr_en_t)g_hal_uart_reg[uart]->intr_en;

    // Perform configuration
    switch (interrupt_type) {
#if defined(CONFIG_UART_SUPPORT_TX_INT)
        case HAL_UART_INTERRUPT_TX:
            inter_en.intr_en.tran_em_intr_en = val;
            break;
#endif
        case HAL_UART_INTERRUPT_RX:
        case HAL_UART_INTERRUPT_CHAR_TIMEOUT:
            inter_en.intr_en.rece_data_intr_en = val;
            break;
        case HAL_UART_INTERRUPT_ERROR:
            inter_en.intr_en.rece_line_stat_intr_en = val;
            break;
        case HAL_UART_INTERRUPT_MODEM_STATUS:
            inter_en.intr_en.modem_intr_en = val;
            break;

        case HAL_UART_INTERRUPT_RECEIVER_LINE_STATUS:
            inter_en.intr_en.rece_line_stat_intr_en = val;
            break;

        default:
            break;
    }
    g_hal_uart_reg[uart]->intr_en = inter_en.d32;
}

void hal_uart_clear_interrupt(uart_bus_t uart, hal_uart_interrupt_t interrupt_type)
{
    unused(interrupt_type);
    intr_id_t data = (intr_id_t)g_hal_uart_reg[uart]->intr_id;
    unused(data);
    hal_uart_clear_pending(uart);
}

void hal_uart_disable_uart(uart_bus_t uart)
{
    unused(uart);
}

static errcode_t hal_uart_is_tx_fifo_full(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    bool *tx_fifo_full = (bool *)param;
    fifo_status_t data = (fifo_status_t)g_hal_uart_reg[bus]->fifo_status;
    *tx_fifo_full = data.fifo_status.tx_fifo_full;
    return ERRCODE_SUCC;
}

static errcode_t hal_uart_is_tx_fifo_empty(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    bool *tx_fifo_empty = (bool *)param;
    fifo_status_t data = (fifo_status_t)g_hal_uart_reg[bus]->fifo_status;
    *tx_fifo_empty = data.fifo_status.tx_fifo_empty;
    return ERRCODE_SUCC;
}

static errcode_t hal_uart_is_busy(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    bool *uart_busy = (bool *)param;
    fifo_status_t data = (fifo_status_t)g_hal_uart_reg[bus]->fifo_status;
#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
    *uart_busy = data.fifo_status.uart_busy;
#else
    *uart_busy = !data.fifo_status.tx_fifo_empty;
#endif
    return ERRCODE_SUCC;
}

bool hal_uart_is_rx_fifo_full(uart_bus_t uart)
{
    fifo_status_t data = (fifo_status_t)g_hal_uart_reg[uart]->fifo_status;
    return data.fifo_status.rx_fifo_full;
}

static errcode_t hal_uart_is_rx_fifo_empty(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    bool *rx_fifo_empty = (bool *)param;
    fifo_status_t data = (fifo_status_t)g_hal_uart_reg[bus]->fifo_status;
    *rx_fifo_empty = data.fifo_status.rx_fifo_empty;
    return ERRCODE_SUCC;
}

uint32_t hal_uart_get_fifo_status(uart_bus_t uart)
{
    return g_hal_uart_reg[uart]->fifo_status;
}

void hal_uart_force_tx_isr(uart_bus_t uart)
{
    g_hal_uart_force_isr_flags[uart] |= HAL_UART_FORCE_TX_ISR_FLAG;
    uart_port_set_pending_irq(uart);
}

void hal_uart_force_idle_isr(uart_bus_t uart)
{
    g_hal_uart_force_isr_flags[uart] |= HAL_UART_FORCE_IDLE_ISR_FLAG;
    uart_port_set_pending_irq(uart);
}

void hal_uart_check_en(uart_bus_t uart, bool val)
{
    uart_ctl_t data = (uart_ctl_t)g_hal_uart_reg[uart]->uart_ctl;
    data.uart_ctl.pen = val;
    g_hal_uart_reg[uart]->uart_ctl = data.d32;
}

void hal_uart_parity_en(uart_bus_t uart, hal_uart_parity_t val)
{
    uart_ctl_t data = (uart_ctl_t)(g_hal_uart_reg[uart]->uart_ctl);
    data.uart_ctl.eps = val;
    g_hal_uart_reg[uart]->uart_ctl = data.d32;
}

void hal_uart_sir_mode_en(uart_bus_t uart, bool val)
{
    unused(uart);
    unused(val);
}

void hal_uart_tx_pause_en(uart_bus_t uart, bool val)
{
    halt_tx_t data = (halt_tx_t)g_hal_uart_reg[uart]->halt_tx;
    data.halt_tx.halt_tx = val;
    g_hal_uart_reg[uart]->halt_tx = data.d32;
}

void hal_uart_set_baud_div(uart_bus_t uart, uint8_t val)
{
    baud_ctl_t data = (baud_ctl_t)g_hal_uart_reg[uart]->baud_ctl;
    data.baud_ctl.baud_div = val;
    g_hal_uart_reg[uart]->baud_ctl = data.d32;
}

uint32_t hal_uart_get_baud_div(uart_bus_t uart)
{
    baud_ctl_t data = (baud_ctl_t)g_hal_uart_reg[uart]->baud_ctl;
    return data.baud_ctl.baud_div;
}

#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
/**
 * uart v151 pro need config rx enable to recv data.
 */
void hal_uart_rx_en(uart_bus_t uart, bool val)
{
    receive_ctl_t data = (receive_ctl_t)g_hal_uart_reg[uart]->receive_ctl;
    data.receive_ctl.receive_enable = val;
    g_hal_uart_reg[uart]->receive_ctl = data.d32;
}

/**
 * uart v151 pro use 8x sample rate of baudrate, need close low power clock.
 * uart v151 pro use 16x sample rate of baudrate, can open low power clock.
 */
void hal_uart_set_lp_req_en(uart_bus_t uart, bool val)
{
    lp_ctl_t data = (lp_ctl_t)g_hal_uart_reg[uart]->lp_ctl;
    data.lp_ctl.lp_req_en = val;
    g_hal_uart_reg[uart]->lp_ctl = data.d32;
}
#endif

volatile uint32_t g_test_uart_write = 1;
void hal_uart_irq_handler(uart_bus_t uart)
{
    if (uart >= UART_BUS_MAX_NUM) {
        return;
    }
    // Get the current register values

    volatile intr_id_t uart_int_reg = (intr_id_t)g_hal_uart_reg[uart]->intr_id;
    // Detect the interrupt cause and trigger the right callback
    if (uart_int_reg.intr_id.intr_id == HAL_UART_INTERRUPT_BUSY_DETECT) {
#ifdef BUILD_APPLICATION_STANDARD
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
        hal_uart_print_info(0, "hal_uart_isr err: 0x%x ", ONE_ARG, uart_int_reg);
#endif
#endif
#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
        // read Line Status register to clear this interrupt
        hal_uart_get_error_status(uart);
#endif
        g_hal_uart_callback[uart](uart, UART_EVT_FRAME_ERR_ISR, 0);
    }

    if (uart_int_reg.intr_id.intr_id == HAL_UART_INTERRUPT_RX) {
        g_hal_uart_callback[uart](uart, UART_EVT_RX_ISR, 0);
    }

    if ((uart_int_reg.intr_id.intr_id == HAL_UART_INTERRUPT_NO_INTERRUPT) ||
        (uart_int_reg.intr_id.intr_id == HAL_UART_INTERRUPT_CHAR_TIMEOUT) ||
        (g_hal_uart_force_isr_flags[uart] & HAL_UART_FORCE_IDLE_ISR_FLAG) != 0) {
        g_hal_uart_callback[uart](uart, UART_EVT_IDLE_ISR, 0);
        g_hal_uart_force_isr_flags[uart] &= ~HAL_UART_FORCE_IDLE_ISR_FLAG;
    }
#if defined(CONFIG_UART_SUPPORT_TX_INT)
    if ((uart_int_reg.intr_id.intr_id == HAL_UART_INTERRUPT_TX) ||
        (g_hal_uart_force_isr_flags[uart] & HAL_UART_FORCE_TX_ISR_FLAG) != 0) {
        // TX Interrupt
        g_hal_uart_callback[uart](uart, UART_EVT_TX_ISR, 0);

        g_hal_uart_force_isr_flags[uart] &= ~HAL_UART_FORCE_TX_ISR_FLAG;
    }
#endif
    // ERROR INTERRUPT
    if (uart_int_reg.intr_id.intr_id == HAL_UART_INTERRUPT_RECEIVER_LINE_STATUS) {
        // read Line Status register to clear this interrupt
        hal_uart_get_error_status(uart);
        g_hal_uart_callback[uart](uart, UART_EVT_OVERRUN_ERR_ISR, 0);
    }
}

static errcode_t hal_uart_v151_write(uart_bus_t bus, const uint8_t *data, uint16_t len)
{
    uint8_t *data_buffer = (uint8_t *)data;
    uint16_t length = len;
    while (length > 0) {
        fifo_status_t fifo_st = (fifo_status_t)g_hal_uart_reg[bus]->fifo_status;
        if (fifo_st.fifo_status.tx_fifo_full == 0) {
            g_hal_uart_reg[bus]->data = *data_buffer++;
            length--;
        }
    }

    return ERRCODE_SUCC;
}

static int32_t hal_uart_v151_read(uart_bus_t bus, const uint8_t *data, uint16_t len)
{
    int32_t i = 0;
    uint8_t *data_buff = (uint8_t *)data;

    for (i = 0; i < len; i++) {
        fifo_status_t fifo_status = (fifo_status_t)g_hal_uart_reg[bus]->fifo_status;
        if (fifo_status.fifo_status.rx_fifo_empty != 0) {
            break;
        }
        data_buff[i] = (uint8_t)(g_hal_uart_reg[bus]->data);
    }
    return i;
}

static errcode_t hal_uart_v151_ctrl(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    if (g_hal_uart_ctrl_func_array[id] != NULL) {
        return g_hal_uart_ctrl_func_array[id](bus, id, param);
    }
    return ERRCODE_UART_NOT_IMPLEMENT;
}

#if defined(CONFIG_UART_SUPPORT_DMA)
static void hal_uart_v151_dma_config(uart_bus_t bus, const hal_uart_extra_attr_t *extra_attr)
{
    hal_uart_set_dmaen(bus, 0);
    hal_uart_enable_interrupt(bus, HAL_UART_INTERRUPT_RECEIVER_LINE_STATUS, 1);
    if (extra_attr->rx_dma_enable) {
        hal_uart_set_dmaen(bus, 1);
        hal_uart_set_fifo_en(bus, HAL_UART_FIFO_ENABLED);
        hal_uart_init_fifo(bus, extra_attr->rx_int_threshold, extra_attr->tx_int_threshold);
        hal_uart_enable_interrupt(bus, HAL_UART_INTERRUPT_RX, 0);
        hal_uart_enable_interrupt(bus, HAL_UART_INTERRUPT_CHAR_TIMEOUT, 0);
    }
    if (extra_attr->tx_dma_enable) {
        hal_uart_set_dmaen(bus, 1);
        hal_uart_set_fifo_en(bus, HAL_UART_FIFO_ENABLED);
        hal_uart_init_fifo(bus, extra_attr->rx_int_threshold, extra_attr->tx_int_threshold);
        hal_uart_enable_interrupt(bus, HAL_UART_INTERRUPT_TX, 0);
    }
}

/* only support for dma testsuit */
void hal_uart_uartifls_set_rxiflsel(uart_bus_t bus, uint32_t val)
{
    unused(val);
    hal_uart_extra_attr_t extra_attr = {
        .tx_dma_enable = 1,
        .tx_int_threshold = HAL_UART_TX_FIFO_EMPTY_LEVEL_EQ_0,
        .rx_dma_enable = 1,
        .rx_int_threshold = HAL_UART_RX_FIFO_AVAILABLE_LEVEL_1_4
    };
    hal_uart_v151_dma_config(bus, &extra_attr);
}

/* only support for dma testsuit */
void hal_uart_int_set(uart_bus_t bus, uint32_t reg, uint32_t int_id,  uint32_t val)
{
    unused(bus);
    unused(reg);
    unused(int_id);
    unused(val);
}

static errcode_t hal_uart_v151_get_dma_data_addr(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    uint32_t *addr = (uint32_t *)param;
    *addr = (uint32_t)(uintptr_t)(&(g_hal_uart_reg[bus]->data));
    return ERRCODE_SUCC;
}

#endif

static hal_uart_funcs_t g_hal_uart_v51_funcs = {
    .init = hal_uart_v151_init,
    .deinit = hal_uart_v151_deinit,
    .write = hal_uart_v151_write,
    .read = hal_uart_v151_read,
    .ctrl = hal_uart_v151_ctrl,
#if defined(CONFIG_UART_SUPPORT_DMA)
    .dma_cfg = hal_uart_v151_dma_config,
#endif
};

hal_uart_funcs_t *hal_uart_v151_funcs_get(void)
{
    return &g_hal_uart_v51_funcs;
}
