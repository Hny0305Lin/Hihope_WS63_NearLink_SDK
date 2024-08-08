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

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "stdlib.h"
#include "uart.h"
#include "lwip/nettool/misc.h"
#include "soc_osal.h"
#include "app_init.h"

#define WIFI_STA_TASK_PRIO 24
#define WIFI_STA_TASK_STACK_SIZE 0x2000
#define WIFI_IFNAME_MAX_SIZE 16
#define WIFI_SCAN_AP_LIMIT 64
#define WIFI_CONN_STATUS_MAX_GET_TIMES 5 /* 启动连接之后，判断是否连接成功的最大尝试次数 */
#define DHCP_BOUND_STATUS_MAX_GET_TIMES 20 /* 启动DHCP Client端功能之后，判断是否绑定成功的最大尝试次数 */
#define WIFI_STA_IP_MAX_GET_TIMES 5 /* 判断是否获取到IP的最大尝试次数 */

/*****************************************************************************
  STA 扫描-关联 sample用例
*****************************************************************************/
static errcode_t example_get_match_network(const char *expected_ssid,
                                           const char *key,
                                           wifi_sta_config_stru *expected_bss)
{
    uint32_t num = WIFI_SCAN_AP_LIMIT; /* 64:扫描到的Wi-Fi网络数量 */
    uint32_t bss_index = 0;

    /* 获取扫描结果 */
    uint32_t scan_len = sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
    if (result == NULL) {
        return ERRCODE_MALLOC;
    }

    memset_s(result, scan_len, 0, scan_len);
    if (wifi_sta_get_scan_info(result, &num) != ERRCODE_SUCC) {
        osal_kfree(result);
        return ERRCODE_FAIL;
    }

    /* 筛选扫描到的Wi-Fi网络，选择待连接的网络 */
    for (bss_index = 0; bss_index < num; bss_index++) {
        if (strlen(expected_ssid) == strlen(result[bss_index].ssid)) {
            if (memcmp(expected_ssid, result[bss_index].ssid, strlen(expected_ssid)) == 0) {
                break;
            }
        }
    }

    /* 未找到待连接AP,可以继续尝试扫描或者退出 */
    if (bss_index >= num) {
        osal_kfree(result);
        return ERRCODE_FAIL;
    }
    /* 找到网络后复制网络信息和接入密码 */
    if (memcpy_s(expected_bss->ssid, WIFI_MAX_SSID_LEN, result[bss_index].ssid, WIFI_MAX_SSID_LEN) != EOK) {
        osal_kfree(result);
        return ERRCODE_MEMCPY;
    }
    if (memcpy_s(expected_bss->bssid, WIFI_MAC_LEN, result[bss_index].bssid, WIFI_MAC_LEN) != EOK) {
        osal_kfree(result);
        return ERRCODE_MEMCPY;
    }
    expected_bss->security_type = result[bss_index].security_type;
    if (memcpy_s(expected_bss->pre_shared_key, WIFI_MAX_KEY_LEN, key, strlen(key)) != EOK) {
        osal_kfree(result);
        return ERRCODE_MEMCPY;
    }
    expected_bss->ip_type = DHCP; /* IP类型为动态DHCP获取 */
    osal_kfree(result);
    return ERRCODE_SUCC;
}

static errcode_t example_sta_function(void)
{
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0"; /* WiFi STA 网络设备名，SDK默认是wlan0, 以实际名称为准 */
    wifi_sta_config_stru expected_bss = {0};         /* 连接请求信息 */
    const char expected_ssid[] = "H";
    const char key[] = "12345678"; /* 待连接的网络接入密码 */
    struct netif *netif_p = NULL;
    wifi_linked_info_stru wifi_status;
    uint8_t index = 0;

    (void)osal_msleep(5000); /* 延时5s，等待wifi初始化完毕 */
    printf("STA try enable.\r\n");
    /* 创建STA */
    if (wifi_sta_enable() != ERRCODE_SUCC) {
        printf("sta enbale fail !\r\n");
        return ERRCODE_FAIL;
    }

    do {
        printf("Start Scan !\r\n");
        (void)osal_msleep(1000); /* 每次触发扫描至少间隔1s */
        /* 启动STA扫描 */
        if (wifi_sta_scan() != ERRCODE_SUCC) {
            printf("STA scan fail, try again !\r\n");
            continue;
        }

        (void)osal_msleep(3000); /* 延时3s, 等待扫描完成 */

        /* 获取待连接的网络 */
        if (example_get_match_network(expected_ssid, key, &expected_bss) != ERRCODE_SUCC) {
            printf("Can not find AP, try again !\r\n");
            continue;
        }

        printf("STA try connect.\r\n");
        /* 启动连接 */
        if (wifi_sta_connect(&expected_bss) != ERRCODE_SUCC) {
            continue;
        }

        /* 检查网络是否连接成功 */
        for (index = 0; index < WIFI_CONN_STATUS_MAX_GET_TIMES; index++) {
            (void)osal_msleep(500); /* 延时500ms */
            memset_s(&wifi_status, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));
            if (wifi_sta_get_ap_info(&wifi_status) != ERRCODE_SUCC) {
                continue;
            }
            if (wifi_status.conn_state == WIFI_CONNECTED) {
                break;
            }
        }

        if (wifi_status.conn_state == WIFI_CONNECTED) {
            break; /* 连接成功退出循环 */
        }
    } while (1);

    printf("STA DHCP start.\r\n");
    /* DHCP获取IP地址 */
    netif_p = netifapi_netif_find(ifname);
    if (netif_p == NULL) {
        return ERRCODE_FAIL;
    }

    if (netifapi_dhcp_start(netif_p) != ERR_OK) {
        printf("STA DHCP Fail.\r\n");
        return ERRCODE_FAIL;
    }

    for (uint8_t i = 0; i < DHCP_BOUND_STATUS_MAX_GET_TIMES; i++) {
        (void)osal_msleep(500); /* 延时500ms */
        if (netifapi_dhcp_is_bound(netif_p) == ERR_OK) {
            printf("STA DHCP bound success.\r\n");
            break;
        }
    }

    for (uint8_t i = 0; i < WIFI_STA_IP_MAX_GET_TIMES; i++) {
        (void)osal_msleep(10); /* 延时10ms */
        if (netif_p->ip_addr.u_addr.ip4.addr != 0) {
            printf("STA IP %u.%u.%u.%u\r\n", (netif_p->ip_addr.u_addr.ip4.addr & 0x000000ff),
                   (netif_p->ip_addr.u_addr.ip4.addr & 0x0000ff00) >> 8,
                   (netif_p->ip_addr.u_addr.ip4.addr & 0x00ff0000) >> 16,
                   (netif_p->ip_addr.u_addr.ip4.addr & 0xff000000) >> 24);
            netifapi_netif_common(netif_p, dhcp_clients_info_show, NULL);
            if (netifapi_dhcp_start(netif_p) != 0) {
                printf("STA DHCP Fail.\r\n");
                return ERRCODE_FAIL;
            }

            /* 连接成功 */
            printf("STA connect success.\r\n");
            return ERRCODE_SUCC;
        }
    }

    printf("STA connect fail.\r\n");
    return ERRCODE_FAIL;
}

int demo_init(void *param)
{
    param = param;
    example_sta_function();
    return 0;
}

static void wifistaentry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)demo_init, 0, "WiFiStaTask", WIFI_STA_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, WIFI_STA_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the example_wifi_sta_entry. */
app_run(wifistaentry);