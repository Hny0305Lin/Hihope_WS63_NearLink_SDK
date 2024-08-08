/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides hal uart \n
 *
 * History: \n
 * 2023-02-17, Create file. \n
 */

#include <stdint.h>
#include <stdio.h>
#include "common_def.h"
#include "uart_porting.h"
#include "hal_uart.h"

uintptr_t g_hal_uarts_regs[UART_BUS_MAX_NUM] = { 0 };
hal_uart_funcs_t *g_hal_uarts_funcs[UART_BUS_MAX_NUM] = { 0 };

uintptr_t hal_uart_base_addrs_get(uart_bus_t bus)
{
    return (uintptr_t)uart_porting_base_addr_get(bus);
}

errcode_t hal_uart_regs_init(uart_bus_t bus)
{
    if (hal_uart_base_addrs_get(bus) == 0) {
        return ERRCODE_UART_REG_ADDR_INVALID;
    }
    g_hal_uarts_regs[bus] = hal_uart_base_addrs_get(bus);
    return ERRCODE_SUCC;
}

errcode_t hal_uart_register_funcs(uart_bus_t bus, hal_uart_funcs_t *funcs)
{
    if (bus >= UART_BUS_MAX_NUM || funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_uarts_funcs[bus] = funcs;
    return ERRCODE_SUCC;
}

errcode_t hal_uart_unregister_funcs(uart_bus_t bus)
{
    if (bus >= UART_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_uarts_funcs[bus] = NULL;
    return ERRCODE_SUCC;
}

hal_uart_funcs_t *hal_uart_get_funcs(uart_bus_t bus)
{
    if (bus >= UART_BUS_MAX_NUM) {
        return NULL;
    }
    return g_hal_uarts_funcs[bus];
}

errcode_t hal_uart_set_rx_fifo_int_level(uart_bus_t bus, uart_fifo_rx_int_lvl_t level)
{
    hal_uart_funcs_t *hal_funcs = g_hal_uarts_funcs[bus];
    if (hal_funcs == NULL) {
        return ERRCODE_UART_NOT_IMPLEMENT;
    }
    return hal_funcs->ctrl(bus, UART_CTRL_SET_RX_FIFO_LEVEL, (uintptr_t)(uint32_t)level);
}

errcode_t hal_uart_set_tx_fifo_int_level(uart_bus_t bus, uart_fifo_tx_int_lvl_t level)
{
    hal_uart_funcs_t *hal_funcs = g_hal_uarts_funcs[bus];
    if (hal_funcs == NULL) {
        return ERRCODE_UART_NOT_IMPLEMENT;
    }
    return hal_funcs->ctrl(bus, UART_CTRL_SET_TX_FIFO_LEVEL, (uintptr_t)(uint32_t)level);
}

errcode_t hal_uart_init(uart_bus_t bus, hal_uart_callback_t callback, const hal_uart_pin_config_t *pins,
                        const hal_uart_attr_t *attr, hal_uart_flow_ctrl_t flow_ctrl)
{
    uart_port_register_hal_funcs(bus);
    hal_uart_funcs_t *hal_funcs = g_hal_uarts_funcs[bus];
    if (hal_funcs == NULL || attr == NULL) {
        return ERRCODE_UART_NOT_IMPLEMENT;
    }
    return hal_funcs->init(bus, callback, (hal_uart_pin_config_t *)pins, (hal_uart_attr_t *)attr, flow_ctrl);
}

errcode_t hal_uart_deinit(uart_bus_t bus)
{
    hal_uart_funcs_t *hal_funcs = g_hal_uarts_funcs[bus];
    return hal_funcs->deinit(bus);
}

errcode_t hal_uart_write(uart_bus_t bus, const uint8_t *data, uint16_t len)
{
    hal_uart_funcs_t *hal_funcs = g_hal_uarts_funcs[bus];
    return hal_funcs->write(bus, data, len);
}

int32_t hal_uart_read(uart_bus_t bus, const uint8_t *data, uint16_t len)
{
    hal_uart_funcs_t *hal_funcs = g_hal_uarts_funcs[bus];
    return hal_funcs->read(bus, data, len);
}

errcode_t hal_uart_ctrl(uart_bus_t bus, hal_uart_ctrl_id_t id, uintptr_t param)
{
    hal_uart_funcs_t *hal_funcs = g_hal_uarts_funcs[bus];
    if (hal_funcs == NULL) {
        return ERRCODE_UART_NOT_IMPLEMENT;
    }
    return hal_funcs->ctrl(bus, id, param);
}

#if defined(CONFIG_UART_SUPPORT_DMA)
void hal_uart_set_dma_config(uart_bus_t bus, const hal_uart_extra_attr_t *extra_attr)
{
    hal_uart_funcs_t *hal_funcs = g_hal_uarts_funcs[bus];
    hal_funcs->dma_cfg(bus, (hal_uart_extra_attr_t *)extra_attr);
    return;
}
#endif