/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides iot_uart driver source \n
 *
 * History: \n
 * 2023-10-18， Create file. \n
 */
#include "securec.h"
#include "iot_errno.h"
#include "uart.h"
#include "iot_uart.h"
#if defined(CONFIG_UART_SUPPORT_DMA)
#include "dma.h"
#endif

#define IOT_UART_RX_BUFFER_SIZE                1024
#define IOT_UART_SET_WR_TIMEOUT                0
#define IOT_UART_MIN_DATABITS                  5
#define IOT_UART_MAX_DATABITS                  8
#define IOT_UART_LIMIT_STOPBITS                6
#define IOT_UART_MIN_STOPBITS                  1
#define IOT_UART_MAX_STOPBITS                  2

#if defined(CONFIG_UART_SUPPORT_DMA)
#define UART_DMA_WIDTH                         0
#define UART_DMA_BURST_LENGTH                  2
#define UART_DMA_PRIORITY                      0
#define UART_DMA_TX_ENABLE                     1
#define UART_DMA_RX_ENABLE                     1
#endif

uint8_t g_iot_uart_rx_buff[IOT_UART_RX_BUFFER_SIZE] = { 0 };

unsigned int IoTUartInit(unsigned int id, const IotUartAttribute *param)
{
    if (param == NULL || param->dataBits < IOT_UART_MIN_DATABITS || param->dataBits > IOT_UART_MAX_DATABITS ||
        param->stopBits < IOT_UART_MIN_STOPBITS || param->stopBits > IOT_UART_MAX_STOPBITS) {
        return IOT_FAILURE;
    }
    uint8_t databits = 0;
    uint8_t stopbits = 0;
    uart_pin_config_t pin_config = {
        .tx_pin = S_MGPIO0,
        .rx_pin = S_MGPIO1,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    if (param->dataBits >= IOT_UART_LIMIT_STOPBITS) {
        databits = param->dataBits - IOT_UART_MIN_DATABITS;
    }

    if (param->stopBits == IOT_UART_MAX_STOPBITS) {
        stopbits = 1;
    }

    uart_attr_t attr = {
        .baud_rate = param->baudRate,
        .data_bits = databits,
        .stop_bits = stopbits,
        .parity = param->parity
    };
    uart_buffer_config_t uart_buffer_config = {
        .rx_buffer = g_iot_uart_rx_buff,
        .rx_buffer_size = IOT_UART_RX_BUFFER_SIZE
    };

#if defined(CONFIG_UART_SUPPORT_DMA)
    uart_extra_attr_t extra_attr = {
        .tx_dma_enable = UART_DMA_TX_ENABLE,
        .tx_int_threshold = 0,
        .rx_dma_enable = UART_DMA_RX_ENABLE,
        .rx_int_threshold = 0
    };
    uapi_dma_init();
    uapi_dma_open();
    if (uapi_uart_init((uart_bus_t)id, &pin_config, &attr, &extra_attr, &uart_buffer_config) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
#else
    if (uapi_uart_init((uart_bus_t)id, &pin_config, &attr, NULL, &uart_buffer_config) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
#endif
    return IOT_SUCCESS;
}

unsigned int IoTUartDeinit(unsigned int id)
{
    if (uapi_uart_deinit((uart_bus_t)id) != ERRCODE_SUCC) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

int IoTUartWrite(unsigned int id, const unsigned char *data, unsigned int dataLen)
{
    if (data == NULL) {
        return IOT_FAILURE;
    }
#if defined(CONFIG_UART_SUPPORT_DMA)
    uart_write_dma_config_t dma_cfg = {
        .src_width = UART_DMA_WIDTH,
        .dest_width = UART_DMA_WIDTH,
        .burst_length = UART_DMA_BURST_LENGTH,
        .priority = UART_DMA_PRIORITY
    };
    if (uapi_uart_write_by_dma((uart_bus_t)id, (const uint8_t *)data, (uint32_t)dataLen,
        &dma_cfg) != (int32_t)dataLen) {
        return IOT_FAILURE;
    }
#else
    if (uapi_uart_write((uart_bus_t)id, (const uint8_t *)data, (uint32_t)dataLen,
        IOT_UART_SET_WR_TIMEOUT) != (int32_t)dataLen) {
        return IOT_FAILURE;
    }
#endif
    return (int32_t)dataLen;
}

int IoTUartRead(unsigned int id, unsigned char *data, unsigned int dataLen)
{
    if (data == NULL) {
        return IOT_FAILURE;
    }
#if defined(CONFIG_UART_SUPPORT_DMA)
    uart_write_dma_config_t dma_cfg = {
        .src_width = UART_DMA_WIDTH,
        .dest_width = UART_DMA_WIDTH,
        .burst_length = UART_DMA_BURST_LENGTH,
        .priority = UART_DMA_PRIORITY
    };
    if (uapi_uart_read_by_dma((uart_bus_t)id, g_iot_uart_rx_buff, dataLen, &dma_cfg) != (int32_t)dataLen) {
        return IOT_FAILURE;
    }
#else
    if (uapi_uart_read((uart_bus_t)id, g_iot_uart_rx_buff, dataLen, IOT_UART_SET_WR_TIMEOUT) != (int32_t)dataLen) {
        return IOT_FAILURE;
    }
#endif
    if (memcpy_s(data, dataLen, g_iot_uart_rx_buff, dataLen) != EOK) {
        return IOT_FAILURE;
    }
    return (int32_t)dataLen;
}

unsigned int IoTUartSetFlowCtrl(unsigned int id, IotFlowCtrl flowCtrl)
{
    unused(id);
    unused(flowCtrl);
    return IOT_SUCCESS;
}
