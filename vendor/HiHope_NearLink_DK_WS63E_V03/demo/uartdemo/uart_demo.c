/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pinctrl.h"
#include "uart.h"
#include "watchdog.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "app_init.h"

#define UART_BAUDRATE 115200
#define UART_DATA_BITS 3
#define UART_STOP_BITS 1
#define UART_PARITY_BIT 0
#define UART_TRANSFER_SIZE 18
#define CONFIG_UART1_TXD_PIN 15
#define CONFIG_UART1_RXD_PIN 16
#define CONFIG_UART1_PIN_MODE 1
#define CONFIG_UART1_BUS_ID 1
#define CONFIG_UART_INT_WAIT_MS 5

#define UART_TASK_STACK_SIZE 0x1000
#define UART_TASK_DURATION_MS 1000
#define UART_TASK_PRIO 17

static uint8_t g_app_uart_tx_buff[UART_TRANSFER_SIZE] = "hello uart1";
static uint8_t g_app_uart_rx_buff[UART_TRANSFER_SIZE] = {0};

#define CONFIG_UART_POLL_TRANSFER_MODE 1
#if defined(CONFIG_UART_POLL_TRANSFER_MODE)
// 轮询读取uart值
#endif

#if defined(CONFIG_UART_INT_TRANSFER_MODE)
static uint8_t g_app_uart_int_rx_flag = 0;
#endif

static uart_buffer_config_t g_app_uart_buffer_config = {.rx_buffer = g_app_uart_rx_buff,
                                                        .rx_buffer_size = UART_TRANSFER_SIZE};

static void app_uart_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_UART1_TXD_PIN, CONFIG_UART1_PIN_MODE);
    uapi_pin_set_mode(CONFIG_UART1_RXD_PIN, CONFIG_UART1_PIN_MODE);
}

static void app_uart_init_config(void)
{
    uart_attr_t attr = {.baud_rate = UART_BAUDRATE,
                        .data_bits = UART_DATA_BITS,
                        .stop_bits = UART_STOP_BITS,
                        .parity = UART_PARITY_BIT};

    uart_pin_config_t pin_config = {.tx_pin = S_MGPIO0, .rx_pin = S_MGPIO1, .cts_pin = PIN_NONE, .rts_pin = PIN_NONE};
    uapi_uart_deinit(CONFIG_UART1_BUS_ID); // 重点，UART初始化之前需要去初始化，否则会报0x80001044
    int res = uapi_uart_init(CONFIG_UART1_BUS_ID, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
    if (res != 0) {
        printf("uart init failed res = %02x\r\n", res);
    }
}

#if defined(CONFIG_UART_INT_TRANSFER_MODE)
static void app_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    if (buffer == NULL || length == 0) {
        osal_printk("uart%d int mode transfer illegal data!\r\n", CONFIG_UART1_BUS_ID);
        return;
    }

    uint8_t *buff = (uint8_t *)buffer;
    if (memcpy_s(g_app_uart_rx_buff, length, buff, length) != EOK) {
        osal_printk("uart%d int mode data copy fail!\r\n", CONFIG_UART1_BUS_ID);
        return;
    }
    osal_printk("uart write data = %s\r\n", buff);
    g_app_uart_int_rx_flag = 1;
}

static void app_uart_write_int_handler(const void *buffer, uint32_t length, const void *params)
{
    unused(params);
    uint8_t *buff = (void *)buffer;
    for (uint8_t i = 0; i < length; i++) {
        osal_printk("uart%d write data[%d] = %d\r\n", CONFIG_UART1_BUS_ID, i, buff[i]);
    }
}
#endif

void *uart_task(const char *arg)
{
    unused(arg);
    /* UART pinmux. */
    app_uart_init_pin();
    /* UART init config. */
    app_uart_init_config();

#if defined(CONFIG_UART_INT_TRANSFER_MODE)
    osal_printk("uart%d int mode register receive callback start!\r\n", CONFIG_UART1_BUS_ID);
    if (uapi_uart_register_rx_callback(CONFIG_UART1_BUS_ID, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE, 1,
                                       app_uart_read_int_handler) == ERRCODE_SUCC) {
        osal_printk("uart%d int mode register receive callback succ!\r\n", CONFIG_UART1_BUS_ID);
    }
#endif
    while (1) {
        osal_mdelay(UART_TASK_DURATION_MS);
#if defined(CONFIG_UART_POLL_TRANSFER_MODE)
        osal_printk("uart%d poll mode send start!, len = %d\r\n", CONFIG_UART1_BUS_ID, sizeof(g_app_uart_rx_buff));
        (void)uapi_watchdog_kick();
        if (uapi_uart_write(CONFIG_UART1_BUS_ID, g_app_uart_tx_buff, UART_TRANSFER_SIZE, 0) == UART_TRANSFER_SIZE) {
            osal_printk("uart%d poll mode send back succ!\r\n", CONFIG_UART1_BUS_ID);
        }
        if (uapi_uart_read(CONFIG_UART1_BUS_ID, g_app_uart_rx_buff, UART_TRANSFER_SIZE, 0) > 0) {
            osal_printk("uart%d poll mode receive succ!, g_app_uart_rx_buff = %s\r\n", CONFIG_UART1_BUS_ID,
                        g_app_uart_rx_buff);
        }
#endif
#if defined(CONFIG_UART_INT_TRANSFER_MODE)
        while (g_app_uart_int_rx_flag != 1) {
            osDelay(CONFIG_UART_INT_WAIT_MS);
        }
        g_app_uart_int_rx_flag = 0;
        osal_printk("uart%d int mode send back!\r\n", CONFIG_UART1_BUS_ID);
        if (uapi_uart_write_int(CONFIG_UART1_BUS_ID, g_app_uart_tx_buff, UART_TRANSFER_SIZE, 0,
                                app_uart_write_int_handler) == ERRCODE_SUCC) {
            osal_printk("uart%d int mode send back succ!\r\n", CONFIG_UART1_BUS_ID);
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