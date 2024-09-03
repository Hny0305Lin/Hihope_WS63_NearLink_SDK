/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE GATEWAY Sample Source. \n
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
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_GATEWAY_SERVER)
#include "securec.h"
#include "sle_uart_server.h"
#include "sle_uart_server_adv.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_GATEWAY_CLIENT)
#define SLE_GATEWAY_TASK_STACK_SIZE            0x1000
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_uart_client.h"

#include "lwip/nettool/misc.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "wifi_connect.h"

// 在sock_fd 进行监听，在 new_fd 接收新的链接
int g_sock_fd;

// 服务器的地址信息
struct sockaddr_in g_send_addr;
socklen_t g_addr_length = sizeof(g_send_addr);

#define CONFIG_WIFI_SSID            "test"                              // 要连接的WiFi 热点账号
#define CONFIG_WIFI_PWD             "0987654321"                        // 要连接的WiFi 热点密码
#define CONFIG_SERVER_IP            "192.168.1.175"                     // 要连接的服务器IP
#define CONFIG_SERVER_PORT          8888     

#endif  /* CONFIG_SAMPLE_SUPPORT_SLE_GATEWAY_CLIENT */

#define SLE_GATEWAY_TASK_PRIO                  28
#define SLE_GATEWAY_TASK_DURATION_MS           2000
#define SLE_GATEWAY_BAUDRATE                   115200
#define SLE_GATEWAY_TRANSFER_SIZE              256

static uint8_t g_app_uart_rx_buff[SLE_GATEWAY_TRANSFER_SIZE] = { 0 };

static uart_buffer_config_t g_app_uart_buffer_config = {
    .rx_buffer = g_app_uart_rx_buff,
    .rx_buffer_size = SLE_GATEWAY_TRANSFER_SIZE
};

static void uart_init_config(void)
{
    uart_attr_t attr = {
        .baud_rate = SLE_GATEWAY_BAUDRATE,
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
    uapi_uart_deinit(CONFIG_SLE_GATEWAY_BUS);
    uapi_uart_init(CONFIG_SLE_GATEWAY_BUS, &pin_config, &attr, NULL, &g_app_uart_buffer_config);

}

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_GATEWAY_SERVER)
#define SLE_GATEWAY_SERVER_DELAY_COUNT         5

#define SLE_GATEWAY_TASK_STACK_SIZE            0x1200
#define SLE_ADV_HANDLE_DEFAULT              1
#define SLE_GATEWAY_SERVER_MSG_QUEUE_LEN       5
#define SLE_GATEWAY_SERVER_MSG_QUEUE_MAX_SIZE  32
#define SLE_GATEWAY_SERVER_QUEUE_DELAY         0xFFFFFFFF
#define SLE_GATEWAY_SERVER_BUFF_MAX_SIZE       800
#ifdef CONFIG_SAMPLE_SUPPORT_PERFORMANCE_TYPE
#define SLE_GATEWAY_SERVER_SEND_BUFF_MAX_LEN   250
#else
#define SLE_GATEWAY_SERVER_SEND_BUFF_MAX_LEN   40
#endif
unsigned long g_sle_uart_server_msgqueue_id;
#define SLE_GATEWAY_SERVER_LOG                 "[sle uart server]"
static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
        SLE_GATEWAY_SERVER_LOG, server_id, conn_id, read_cb_para->handle, status);
}
static void ssaps_server_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    osal_printk("%s ssaps write request callback cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
        SLE_GATEWAY_SERVER_LOG, server_id, conn_id, write_cb_para->handle, status);
    if ((write_cb_para->length > 0) && write_cb_para->value) {
        osal_printk("\n sle uart recived data : %s\r\n", write_cb_para->value);
        uapi_uart_write(CONFIG_SLE_GATEWAY_BUS, (uint8_t *)write_cb_para->value, write_cb_para->length, 0);
    }
}


static void sle_uart_server_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    if (sle_uart_client_is_connected()) {
    sle_uart_server_send_report_by_handle(buffer, length);
    } else {
        osal_printk("%s sle client is not connected! \r\n", SLE_GATEWAY_SERVER_LOG);
    }
}


static void sle_uart_server_create_msgqueue(void)
{
    if (osal_msg_queue_create("sle_uart_server_msgqueue", SLE_GATEWAY_SERVER_MSG_QUEUE_LEN, \
        (unsigned long *)&g_sle_uart_server_msgqueue_id, 0, SLE_GATEWAY_SERVER_MSG_QUEUE_MAX_SIZE) != OSAL_SUCCESS) {
        osal_printk("^%s sle_uart_server_create_msgqueue message queue create failed!\n", SLE_GATEWAY_SERVER_LOG);
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
                                    buffer_size, SLE_GATEWAY_SERVER_QUEUE_DELAY);
}
static void sle_uart_server_rx_buf_init(uint8_t *buffer_addr, uint32_t *buffer_size)
{
    *buffer_size = SLE_GATEWAY_SERVER_MSG_QUEUE_MAX_SIZE;
    (void)memset_s(buffer_addr, *buffer_size, 0, *buffer_size);
}

static void *sle_uart_server_task(const char *arg)
{
    unused(arg);
    uint8_t rx_buf[SLE_GATEWAY_SERVER_MSG_QUEUE_MAX_SIZE] = {0};
    uint32_t rx_length = SLE_GATEWAY_SERVER_MSG_QUEUE_MAX_SIZE;
    uint8_t sle_connect_state[] = "sle_dis_connect";

    sle_uart_server_create_msgqueue();
    sle_uart_server_register_msg(sle_uart_server_write_msgqueue);
    sle_uart_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);

    /* UART init config. */
    uart_init_config();

    uapi_uart_unregister_rx_callback(CONFIG_SLE_GATEWAY_BUS);
    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_SLE_GATEWAY_BUS,
                                                   UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                   1, sle_uart_server_read_int_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Register uart callback fail.[%x]\r\n", SLE_GATEWAY_SERVER_LOG, ret);
        return NULL;
    }
    while (1) {
        sle_uart_server_rx_buf_init(rx_buf, &rx_length);
        sle_uart_server_receive_msgqueue(rx_buf, &rx_length);
        if (strncmp((const char *)rx_buf, (const char *)sle_connect_state, sizeof(sle_connect_state)) == 0) {
            ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
            if (ret != ERRCODE_SLE_SUCCESS) {
                osal_printk("%s sle_connect_state_changed_cbk,sle_start_announce fail :%02x\r\n",
                    SLE_GATEWAY_SERVER_LOG, ret);
            }
        }
        osal_msleep(SLE_GATEWAY_TASK_DURATION_MS);
    }
    sle_uart_server_delete_msgqueue();
    return NULL;
}


#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_GATEWAY_CLIENT)

void sle_uart_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\n sle uart recived data : %s\r\n", data->data);
    uapi_uart_write(CONFIG_SLE_GATEWAY_BUS, (uint8_t *)(data->data), data->data_len, 0);
    sendto(g_sock_fd, (uint8_t *)(data->data), data->data_len, 0, (struct sockaddr *)&g_send_addr, g_addr_length);
}

void sle_uart_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\n sle uart recived data : %s\r\n", data->data);
    uapi_uart_write(CONFIG_SLE_GATEWAY_BUS, (uint8_t *)(data->data), data->data_len, 0);
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
    char recvBuf[512];
    /* UART init config. */
    uart_init_config();

    /*  Wifi connect */
    wifi_connect(CONFIG_WIFI_SSID, CONFIG_WIFI_PWD);

    /* create socket */
    printf("create socket start!\r\n");
    if ((g_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("create socket failed!\r\n");
        return 0;
    }
    g_send_addr.sin_family = AF_INET;
    g_send_addr.sin_port = htons(CONFIG_SERVER_PORT);
    g_send_addr.sin_addr.s_addr = inet_addr(CONFIG_SERVER_IP);
    g_addr_length = sizeof(g_send_addr);
    printf("create socket end!\r\n");

    uapi_uart_unregister_rx_callback(CONFIG_SLE_GATEWAY_BUS);
    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_SLE_GATEWAY_BUS,
                                                   UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                   1, sle_uart_client_read_int_handler);
    sle_uart_client_init(sle_uart_notification_cb, sle_uart_indication_cb);
    
    if (ret != ERRCODE_SUCC) {
        osal_printk("Register uart callback fail.");
        return NULL;
    }
    while (1) {
        bzero(recvBuf, sizeof(recvBuf));
        // 接收服务端返回的字符串
        recvfrom(g_sock_fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr *)&g_send_addr, &g_addr_length);
        // printf("%s:%d=>%s\n", inet_ntoa(send_addr.sin_addr), ntohs(send_addr.sin_port), recvBuf);
        ssapc_write_param_t *sle_uart_send_param = get_g_sle_uart_send_param();
        uint16_t g_sle_uart_conn_id = get_g_sle_uart_conn_id();
        sle_uart_send_param->data_len = strlen(recvBuf);
        sle_uart_send_param->data = (uint8_t *)recvBuf;
        ssapc_write_req(0, g_sle_uart_conn_id, sle_uart_send_param);
    }
    return NULL;
}


#endif  /* CONFIG_SAMPLE_SUPPORT_SLE_GATEWAY_CLIENT */

static void sle_uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_GATEWAY_SERVER)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_uart_server_task, 0, "SLEUartServerTask",
                                      SLE_GATEWAY_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_GATEWAY_CLIENT)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_uart_client_task, 0, "SLEUartDongleTask",
                                      SLE_GATEWAY_TASK_STACK_SIZE);
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_GATEWAY_CLIENT */
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_GATEWAY_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the sle_uart_entry. */
app_run(sle_uart_entry);