/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE UART Sample Source. \n
 *
 * History: \n
 * 2023-07-17, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "pinctrl.h"
#include "uart.h"
// #include "pm_clock.h"
#include "sle_low_latency.h"
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER)
#include "securec.h"
#include "sle_uart_server.h"
#include "sle_uart_server_adv.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT)
#define SLE_UART_TASK_STACK_SIZE            0x600
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_uart_client.h"
#endif  /* CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT */

#define SLE_UART_TASK_PRIO                  28
#define SLE_UART_TASK_DURATION_MS           2000
#define SLE_UART_BAUDRATE                   115200
#define SLE_UART_TRANSFER_SIZE              512

static uint8_t g_app_uart_rx_buff[SLE_UART_TRANSFER_SIZE] = { 0 };

static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_app_uart_rx_buff,
    .rx_buffer_size = SLE_UART_TRANSFER_SIZE
};

static void uart_init_pin(void)
{
    if (CONFIG_SLE_UART_BUS == 0) {
        uapi_pin_set_mode(CONFIG_UART_TXD_PIN, PIN_MODE_1);
        uapi_pin_set_mode(CONFIG_UART_RXD_PIN, PIN_MODE_1);       
    }else if (CONFIG_SLE_UART_BUS == 1) {
        uapi_pin_set_mode(CONFIG_UART_TXD_PIN, PIN_MODE_1);
        uapi_pin_set_mode(CONFIG_UART_RXD_PIN, PIN_MODE_1);       
    }
}

static void uart_init_config(void)
{
    uart_attr_t attr = {
        .baud_rate = SLE_UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    uart_pin_config_t pin_config = {
        .tx_pin = CONFIG_UART_TXD_PIN,
        .rx_pin = CONFIG_UART_RXD_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };
    uapi_uart_deinit(CONFIG_SLE_UART_BUS);
    uapi_uart_init(CONFIG_SLE_UART_BUS, &pin_config, &attr, NULL, &g_app_uart_buffer_config);

}

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER)
#define SLE_UART_SERVER_DELAY_COUNT         5

#define SLE_UART_TASK_STACK_SIZE            0x1200
#define SLE_ADV_HANDLE_DEFAULT              1
#define SLE_UART_SERVER_MSG_QUEUE_LEN       5
#define SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE  32
#define SLE_UART_SERVER_QUEUE_DELAY         0xFFFFFFFF
#define SLE_UART_SERVER_BUFF_MAX_SIZE       800
#ifdef CONFIG_SAMPLE_SUPPORT_PERFORMANCE_TYPE
#define SLE_UART_SERVER_SEND_BUFF_MAX_LEN   250
#else
#define SLE_UART_SERVER_SEND_BUFF_MAX_LEN   40
#endif
unsigned long g_sle_uart_server_msgqueue_id;
#define SLE_UART_SERVER_LOG                 "[sle uart server]"
static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
        SLE_UART_SERVER_LOG, server_id, conn_id, read_cb_para->handle, status);
}
static void ssaps_server_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    osal_printk("%s ssaps write request callback cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
        SLE_UART_SERVER_LOG, server_id, conn_id, write_cb_para->handle, status);
    if ((write_cb_para->length > 0) && write_cb_para->value) {
        osal_printk("\n sle uart recived data : %s\r\n", write_cb_para->value);
        uapi_uart_write(CONFIG_SLE_UART_BUS, (uint8_t *)write_cb_para->value, write_cb_para->length, 0);
    }
}

#ifdef CONFIG_SAMPLE_SUPPORT_LOW_LATENCY_TYPE
uint8_t g_buff[SLE_UART_SERVER_BUFF_MAX_SIZE] = {0};
uint16_t g_uart_buff_len = 0;
uint8_t g_buff_data_valid = 0;
uint8_t g_mcs_flag = 0;
#endif
static void sle_uart_server_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    if (sle_uart_client_is_connected()) {
#ifdef CONFIG_SAMPLE_SUPPORT_LOW_LATENCY_TYPE
    g_buff_data_valid = 1;
    g_uart_buff_len = 0;
    (void)memcpy_s(g_buff, SLE_UART_SERVER_SEND_BUFF_MAX_LEN, buffer, length);
    g_uart_buff_len = length;
#else
    sle_uart_server_send_report_by_handle(buffer, length);
#endif
    } else {
        osal_printk("%s sle client is not connected! \r\n", SLE_UART_SERVER_LOG);
    }
}

#ifdef CONFIG_SAMPLE_SUPPORT_LOW_LATENCY_TYPE
uint8_t *sle_uart_low_latency_tx_cbk(uint16_t *len)
{
#ifdef CONFIG_SAMPLE_SUPPORT_PERFORMANCE_TYPE
    if (g_mcs_flag == 0) {
        sle_uart_server_sample_set_mcs(get_connect_id());
    }
    g_uart_buff_len = SLE_UART_SERVER_SEND_BUFF_MAX_LEN;
    g_buff_data_valid = 1;
    g_mcs_flag = 1;
#endif
    if (g_buff_data_valid == 0) {
        return NULL;
    }
    if (g_uart_buff_len == 0) {
        return NULL;
    }
    *len = g_uart_buff_len;
    g_buff_data_valid = 0;
    return g_buff;
}

void sle_uart_low_latency_tx_cbk_register(void)
{
    sle_low_latency_tx_callbacks_t cbk_func = {0};
    cbk_func.low_latency_tx_cb = sle_uart_low_latency_tx_cbk;
    sle_low_latency_tx_register_callbacks(&cbk_func);
}
#endif

static void sle_uart_server_create_msgqueue(void)
{
    if (osal_msg_queue_create("sle_uart_server_msgqueue", SLE_UART_SERVER_MSG_QUEUE_LEN, \
        (unsigned long *)&g_sle_uart_server_msgqueue_id, 0, SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE) != OSAL_SUCCESS) {
        osal_printk("^%s sle_uart_server_create_msgqueue message queue create failed!\n", SLE_UART_SERVER_LOG);
    }
}

static void sle_uart_server_delete_msgqueue(void)
{
    osal_msg_queue_delete(g_sle_uart_server_msgqueue_id);
}

static void sle_uart_server_write_msgqueue(uint8_t *buffer_addr, uint16_t buffer_size)
{
    osal_msg_queue_write_copy(g_sle_uart_server_msgqueue_id, (void *)buffer_addr, \
                              (uint32_t)buffer_size, 0);
}

static int32_t sle_uart_server_receive_msgqueue(uint8_t *buffer_addr, uint32_t *buffer_size)
{
    return osal_msg_queue_read_copy(g_sle_uart_server_msgqueue_id, (void *)buffer_addr, \
                                    buffer_size, SLE_UART_SERVER_QUEUE_DELAY);
}
static void sle_uart_server_rx_buf_init(uint8_t *buffer_addr, uint32_t *buffer_size)
{
    *buffer_size = SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE;
    (void)memset_s(buffer_addr, *buffer_size, 0, *buffer_size);
}

static void *sle_uart_server_task(const char *arg)
{
    unused(arg);
    uint8_t rx_buf[SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE] = {0};
    uint32_t rx_length = SLE_UART_SERVER_MSG_QUEUE_MAX_SIZE;
    uint8_t sle_connect_state[] = "sle_dis_connect";

    sle_uart_server_create_msgqueue();
    sle_uart_server_register_msg(sle_uart_server_write_msgqueue);
    sle_uart_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);


#ifdef CONFIG_SAMPLE_SUPPORT_LOW_LATENCY_TYPE
    sle_uart_low_latency_tx_cbk_register();
#endif
    /* UART pinmux. */
    uart_init_pin();

    /* UART init config. */
    uart_init_config();

    uapi_uart_unregister_rx_callback(CONFIG_SLE_UART_BUS);
    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_SLE_UART_BUS,
                                                   UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                   1, sle_uart_server_read_int_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Register uart callback fail.[%x]\r\n", SLE_UART_SERVER_LOG, ret);
        return NULL;
    }
    while (1) {
        sle_uart_server_rx_buf_init(rx_buf, &rx_length);
        sle_uart_server_receive_msgqueue(rx_buf, &rx_length);
        if (strncmp((const char *)rx_buf, (const char *)sle_connect_state, sizeof(sle_connect_state)) == 0) {
            ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
            if (ret != ERRCODE_SLE_SUCCESS) {
                osal_printk("%s sle_connect_state_changed_cbk,sle_start_announce fail :%02x\r\n",
                    SLE_UART_SERVER_LOG, ret);
            }
        }
        osal_msleep(SLE_UART_TASK_DURATION_MS);
    }
    sle_uart_server_delete_msgqueue();
    return NULL;
}
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT)

void sle_uart_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\n sle uart recived data : %s\r\n", data->data);
    uapi_uart_write(CONFIG_SLE_UART_BUS, (uint8_t *)(data->data), data->data_len, 0);
}

void sle_uart_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\n sle uart recived data : %s\r\n", data->data);
    uapi_uart_write(CONFIG_SLE_UART_BUS, (uint8_t *)(data->data), data->data_len, 0);
}

static void sle_uart_client_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    ssapc_write_param_t *sle_uart_send_param = get_g_sle_uart_send_param();
    uint16_t g_sle_uart_conn_id = get_g_sle_uart_conn_id();
    sle_uart_send_param->data_len = length;
    sle_uart_send_param->data = (uint8_t *)buffer;
    ssapc_write_req(0, g_sle_uart_conn_id, sle_uart_send_param);
}

static void *sle_uart_client_task(const char *arg)
{
    unused(arg);
    /* UART pinmux. */
    uart_init_pin();

    /* UART init config. */
    uart_init_config();

    uapi_uart_unregister_rx_callback(CONFIG_SLE_UART_BUS);
    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_SLE_UART_BUS,
                                                   UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                   1, sle_uart_client_read_int_handler);
    sle_uart_client_init(sle_uart_notification_cb, sle_uart_indication_cb);
    
    if (ret != ERRCODE_SUCC) {
        osal_printk("Register uart callback fail.");
        return NULL;
    }

    return NULL;
}
#endif  /* CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT */

static void sle_uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_uart_server_task, 0, "SLEUartServerTask",
                                      SLE_UART_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_uart_client_task, 0, "SLEUartDongleTask",
                                      SLE_UART_TASK_STACK_SIZE);
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT */
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_UART_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the sle_uart_entry. */
app_run(sle_uart_entry);