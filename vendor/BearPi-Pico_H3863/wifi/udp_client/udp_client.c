/*
 * Copyright (c) 2024 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "lwip/nettool/misc.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "wifi_connect.h"

#define UDP_CLIENT_TASK_PRIO                  (osPriority_t)(13)
#define UDP_CLIENT_TASK_DURATION_MS           2000
#define UDP_CLIENT_TASK_STACK_SIZE            0x1000

#define CONFIG_WIFI_SSID            "test"                              // 要连接的WiFi 热点账号
#define CONFIG_WIFI_PWD             "0987654321"                        // 要连接的WiFi 热点密码
#define CONFIG_SERVER_IP            "192.168.1.175"                     // 要连接的服务器IP
#define CONFIG_SERVER_PORT          8888                                // 要连接的服务器端口

static const char *send_data = "Hello! I'm UDP Client!\r\n";

int udp_client_sample_task(void *param)
{
    param = param;

    // 在sock_fd 进行监听，在 new_fd 接收新的链接
    int sock_fd;

    // 服务器的地址信息
    struct sockaddr_in send_addr;
    socklen_t addr_length = sizeof(send_addr);
    char recvBuf[512];

    // 连接Wifi
    wifi_connect(CONFIG_WIFI_SSID, CONFIG_WIFI_PWD);

    // 创建socket
    printf("create socket start!\r\n");
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("create socket failed!\r\n");
        return 0;
    }
    printf("create socket end!\r\n");
    // 初始化预连接的服务端地址
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(CONFIG_SERVER_PORT);
    send_addr.sin_addr.s_addr = inet_addr(CONFIG_SERVER_IP);
    addr_length = sizeof(send_addr);

    while (1) {
        bzero(recvBuf, sizeof(recvBuf));

        // 发送数据到服务远端
        printf("sendto start!\r\n");
        sendto(sock_fd, send_data, strlen(send_data), 0, (struct sockaddr *)&send_addr, addr_length);
        printf("sendto end!\r\n");
        // 线程休眠一段时间
        osDelay(100);

        // 接收服务端返回的字符串
        recvfrom(sock_fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr *)&send_addr, &addr_length);
        printf("%s:%d=>%s\n", inet_ntoa(send_addr.sin_addr), ntohs(send_addr.sin_port), recvBuf);
    }

    // 关闭这个 socket
    lwip_close(sock_fd);
    return 0;
}
static void udp_client_sample_entry(void)
{
    osThreadAttr_t attr;
    attr.name       = "udp_client_sample_task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = UDP_CLIENT_TASK_STACK_SIZE;
    attr.priority   = UDP_CLIENT_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)udp_client_sample_task, NULL, &attr) == NULL) {
        printf("Create udp_client_sample_task fail.\r\n");
    }
    printf("Create udp_client_sample_task succ.\r\n");
}

/* Run the udp_client_sample_task. */
app_run(udp_client_sample_entry);