/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: BLE UART Sample Source. \n
 *
 * History: \n
 * 2023-07-20, Create file. \n
 */
#include "securec.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "uart.h"
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_SERVER)
#define BLE_UART_TASK_STACK_SIZE            0x1200
#include "bts_gatt_server.h"
#include "ble_uart_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT)
#define BLE_UART_TASK_STACK_SIZE            0x1200
#include "bts_gatt_client.h"
#include "ble_uart_client.h"
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT */

#define BLE_UART_TASK_PRIO                  28
#define BLE_UART_BT_STACK_POWER_MS      10000

typedef struct {
    uint8_t *value;
    uint16_t value_len;
} msg_data_t;
unsigned long mouse_msg_queue = 0;
unsigned int msg_rev_size = sizeof(msg_data_t);

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_SERVER)
static void ble_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    osal_printk("ble_uart_read_int_handler server.\r\n");
    unused(error);
    if (ble_uart_get_connection_state() != 0) {
        msg_data_t msg_data = { 0 };
        void* buffer_cpy = osal_vmalloc(length);
        if (memcpy_s(buffer_cpy, length, buffer, length) != EOK) {
            osal_vfree(buffer_cpy);
            return;
        }
        msg_data.value = (uint8_t *)buffer_cpy;
        msg_data.value_len = length;
        osal_msg_queue_write_copy(mouse_msg_queue, (void *)&msg_data, msg_rev_size, 0);
    }
}

static void *ble_uart_server_task(const char *arg)
{
    unused(arg);
    ble_uart_server_init();
    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_BLE_UART_BUS,
                                                   UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                   1, ble_uart_read_int_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Register uart callback fail.");
        return NULL;
    }
    while (1) {
        msg_data_t msg_data = { 0 };
        int msg_ret = osal_msg_queue_read_copy(mouse_msg_queue, &msg_data, &msg_rev_size, OSAL_WAIT_FOREVER);
        if (msg_ret != OSAL_SUCCESS) {
            osal_printk("msg queue read copy fail.");
            if (msg_data.value != NULL) {
                osal_vfree(msg_data.value);
            }
            continue;
        }
        if (msg_data.value != NULL) {
            ble_uart_server_send_input_report(msg_data.value, msg_data.value_len);
            osal_vfree(msg_data.value);
        }
    }
    return NULL;
}
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT)
static void ble_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    osal_printk("ble_uart_read_int_handler client.\r\n");
    unused(error);
    msg_data_t msg_data = { 0 };
    void* buffer_cpy = osal_vmalloc(length);
    if (memcpy_s(buffer_cpy, length, buffer, length) != EOK) {
        osal_vfree(buffer_cpy);
        return;
    }
    msg_data.value = (uint8_t *)buffer_cpy;
    msg_data.value_len = length;
    osal_msg_queue_write_copy(mouse_msg_queue, (void *)&msg_data, msg_rev_size, 0);
}

static void *ble_uart_client_task(const char *arg)
{
    unused(arg);
    osal_printk("ble_uart_client_task entry.");
    ble_uart_client_init();
    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_BLE_UART_BUS,
                                                   UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                   1, ble_uart_read_int_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Register uart callback fail.");
        return NULL;
    }
    while (1) {
        msg_data_t msg_data = { 0 };
        int msg_ret = osal_msg_queue_read_copy(mouse_msg_queue, &msg_data, &msg_rev_size, OSAL_WAIT_FOREVER);
        if (msg_ret != OSAL_SUCCESS) {
            osal_printk("msg queue read copy fail.");
            if (msg_data.value != NULL) {
                osal_vfree(msg_data.value);
            }
            continue;
        }
        if (msg_data.value != NULL) {
            uint16_t write_handle = ble_uart_get_write_vlaue_handle();
            ble_uart_client_write_cmd(msg_data.value, msg_data.value_len, write_handle);
            osal_vfree(msg_data.value);
        }
    }
    return NULL;
}
#endif  /* CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT */

static void ble_uart_entry(void)
{
    // char *arg = NULL;
    int msg_ret = osal_msg_queue_create("task_msg", msg_rev_size, &mouse_msg_queue, 0, msg_rev_size);
    if (msg_ret != OSAL_SUCCESS) {
        osal_printk("msg queue create fail.");
        return;
    }
    osal_task *task_handle = NULL;
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_SERVER)
    // ble_uart_server_task(arg);
    task_handle = osal_kthread_create((osal_kthread_handler)ble_uart_server_task, 0, "BLEUartServerTask",
                                      BLE_UART_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT)
    // ble_uart_client_task(arg);
    task_handle = osal_kthread_create((osal_kthread_handler)ble_uart_client_task, 0, "BLEUartClientTask",
                                      BLE_UART_TASK_STACK_SIZE);
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT */
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, BLE_UART_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the ble_uart_entry. */
app_run(ble_uart_entry);