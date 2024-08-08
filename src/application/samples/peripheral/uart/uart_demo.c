/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: UART Sample Source. \n
 *
 * History: \n
 * 2023-06-29, Create file. \n
 */
#include "pinctrl.h"
#include "uart.h"
#include "watchdog.h"
#include "soc_osal.h"
#include "app_init.h"

#define UART_BAUDRATE                      115200
#define UART_DATA_BITS                     3
#define UART_STOP_BITS                     1
#define UART_PARITY_BIT                    0
#define UART_TRANSFER_SIZE                 16
#define CONFIG_UART_INT_WAIT_MS            5
#define UART_TASK_DURATION_MS              500

#if defined(CONFIG_UART_DMA_TRANSFER_MODE)
#define UART_TX_DMA_ENABLE                 1
#define UART_TX_INT_THRESHOLD              0
#define UART_RX_DMA_ENABLE                 1
#define UART_RX_INT_THRESHOLD              0
#define UART_DMA_WIDTH                     0
#define UART_DMA_BURST_LENGTH              2
#define UART_DMA_PRIORITY                  0
#endif

#define UART_TASK_PRIO                     24
#define UART_TASK_STACK_SIZE               0x1000

static uint8_t g_app_uart_rx_buff[UART_TRANSFER_SIZE] = { 0 };
#if defined(CONFIG_UART_INT_TRANSFER_MODE)
static uint8_t g_app_uart_int_rx_flag = 0;
#endif
static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_app_uart_rx_buff,
    .rx_buffer_size = UART_TRANSFER_SIZE
};

#if defined(CONFIG_UART_DMA_TRANSFER_MODE)
uart_write_dma_config_t g_app_dma_cfg = {
    .src_width = UART_DMA_WIDTH,
    .dest_width = UART_DMA_WIDTH,
    .burst_length = UART_DMA_BURST_LENGTH,
    .priority = UART_DMA_PRIORITY
};
#endif

static void app_uart_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_UART0_TXD_PIN, CONFIG_UART0_PIN_MODE);
    uapi_pin_set_mode(CONFIG_UART0_RXD_PIN, CONFIG_UART0_PIN_MODE);
}

static void app_uart_init_config(void)
{
    uart_attr_t attr = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = UART_DATA_BITS,
        .stop_bits = UART_STOP_BITS,
        .parity = UART_PARITY_BIT
    };

    uart_pin_config_t pin_config = {
        .tx_pin = S_MGPIO0,
        .rx_pin = S_MGPIO1,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

#if defined(CONFIG_UART_DMA_TRANSFER_MODE)
    uart_extra_attr_t extra_attr = {
        .tx_dma_enable = UART_TX_DMA_ENABLE,
        .tx_int_threshold = UART_TX_INT_THRESHOLD,
        .rx_dma_enable = UART_RX_DMA_ENABLE,
        .rx_int_threshold = UART_RX_INT_THRESHOLD
    };

    uapi_uart_init(CONFIG_UART0_BUS_ID, &pin_config, &attr, &extra_attr, &g_app_uart_buffer_config);
#else
    uapi_uart_init(CONFIG_UART0_BUS_ID, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
#endif
}

#if defined(CONFIG_UART_INT_TRANSFER_MODE)
static void app_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    if (buffer == NULL || length == 0) {
        osal_printk("uart%d int mode transfer illegal data!\r\n", CONFIG_UART0_BUS_ID);
        return;
    }

    uint8_t *buff = (uint8_t *)buffer;
    if (memcpy_s(g_app_uart_rx_buff, length, buff, length) != EOK) {
        osal_printk("uart%d int mode data copy fail!\r\n", CONFIG_UART0_BUS_ID);
        return;
    }
    g_app_uart_int_rx_flag = 1;
}

static void app_uart_write_int_handler(const void *buffer, uint32_t length, const void *params)
{
    unused(params);
    uint8_t *buff = (void *)buffer;
    for (uint8_t i = 0; i < length; i++) {
        osal_printk("uart%d write data[%d] = %d\r\n", CONFIG_UART0_BUS_ID, i, buff[i]);
    }
}
#endif

static void *uart_task(const char *arg)
{
    unused(arg);
    /* UART pinmux. */
    app_uart_init_pin();

    /* UART init config. */
    app_uart_init_config();

#if defined(CONFIG_UART_INT_TRANSFER_MODE)
    osal_printk("uart%d int mode register receive callback start!\r\n", CONFIG_UART0_BUS_ID);
    if (uapi_uart_register_rx_callback(CONFIG_UART0_BUS_ID, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                       1, app_uart_read_int_handler) == ERRCODE_SUCC) {
        osal_printk("uart%d int mode register receive callback succ!\r\n", CONFIG_UART0_BUS_ID);
    }
#endif

    while (1) {
        osal_msleep(UART_TASK_DURATION_MS);
#if defined(CONFIG_UART_POLL_TRANSFER_MODE)
        osal_printk("uart%d poll mode receive start!\r\n", CONFIG_UART0_BUS_ID);
        (void)uapi_watchdog_kick();
        if (uapi_uart_read(CONFIG_UART0_BUS_ID, g_app_uart_rx_buff, UART_TRANSFER_SIZE, 0) == UART_TRANSFER_SIZE) {
            osal_printk("uart%d poll mode receive succ!\r\n", CONFIG_UART0_BUS_ID);
        }
        osal_printk("uart%d poll mode send back!\r\n", CONFIG_UART0_BUS_ID);
        if (uapi_uart_write(CONFIG_UART0_BUS_ID, g_app_uart_rx_buff, UART_TRANSFER_SIZE, 0) == UART_TRANSFER_SIZE) {
            osal_printk("uart%d poll mode send back succ!\r\n", CONFIG_UART0_BUS_ID);
        }
#endif
#if defined(CONFIG_UART_INT_TRANSFER_MODE)
        while (g_app_uart_int_rx_flag != 1) {
            osal_msleep(CONFIG_UART_INT_WAIT_MS);
        }
        g_app_uart_int_rx_flag = 0;
        osal_printk("uart%d int mode send back!\r\n", CONFIG_UART0_BUS_ID);
        if (uapi_uart_write_int(CONFIG_UART0_BUS_ID, g_app_uart_rx_buff, UART_TRANSFER_SIZE, 0,
                                app_uart_write_int_handler) == ERRCODE_SUCC) {
            osal_printk("uart%d int mode send back succ!\r\n", CONFIG_UART0_BUS_ID);
        }
#endif
#if defined(CONFIG_UART_DMA_TRANSFER_MODE)
        osal_printk("uart%d dma mode receive start!\r\n", CONFIG_UART0_BUS_ID);
        if (uapi_uart_read_by_dma(CONFIG_UART0_BUS_ID, g_app_uart_rx_buff, UART_TRANSFER_SIZE,
                                  &g_app_dma_cfg) == UART_TRANSFER_SIZE) {
            osal_printk("uart%d dma mode receive succ!\r\n", CONFIG_UART0_BUS_ID);
        }
        osal_printk("uart%d dma mode send back!\r\n", CONFIG_UART0_BUS_ID);
        if (uapi_uart_write_by_dma(CONFIG_UART0_BUS_ID, g_app_uart_rx_buff, UART_TRANSFER_SIZE,
                                   &g_app_dma_cfg) == UART_TRANSFER_SIZE) {
            osal_printk("uart%d dma mode send back succ!\r\n", CONFIG_UART0_BUS_ID);
        }
#endif
    }

    return NULL;
}

static void uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)uart_task, 0, "UartTask", UART_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, UART_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the uart_entry. */
app_run(uart_entry);