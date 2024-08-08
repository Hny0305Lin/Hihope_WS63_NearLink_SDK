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
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "app_init.h"

#define WIFI_SOFTAP_TASK_PRIO 24
#define WIFI_TASK_STACK_SIZE 0x2000
#define WIFI_IFNAME_MAX_SIZE 16

static errcode_t example_softap_function(void)
{
    /* SoftAp SSID */
    char ssid[WIFI_MAX_SSID_LEN] = "H";

    char pre_shared_key[WIFI_MAX_KEY_LEN] = "12345678";
    softap_config_stru hapd_conf = {0};

    char ifname[WIFI_IFNAME_MAX_SIZE] = "ap0"; /* WiFi SoftAP 网络设备名，SDK默认是ap0, 以实际名称为准 */
    struct netif *netif_p = NULL;
    ip4_addr_t st_gw = {0};
    ip4_addr_t st_ipaddr = {0};
    ip4_addr_t st_netmask = {0};
    IP4_ADDR(&st_ipaddr, 192, 168, 63, 1);   /* IP地址设置：192.168.63.1 */
    IP4_ADDR(&st_netmask, 255, 255, 255, 0); /* 子网掩码设置：255.255.255.0 */
    IP4_ADDR(&st_gw, 192, 168, 63, 2);       /* 网关地址设置：192.168.63.2 */

    PRINT("SoftAP try enable.\r\n");

    (void)memcpy_s(hapd_conf.ssid, sizeof(hapd_conf.ssid), ssid, sizeof(ssid));
    (void)memcpy_s(hapd_conf.pre_shared_key, WIFI_MAX_KEY_LEN, pre_shared_key, WIFI_MAX_KEY_LEN);

    hapd_conf.security_type = WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX; /* 个人级的WPA和WPA2混合 */
    hapd_conf.channel_num = 6;                                /* 6：工作信道设置为6信道 */

    /* 启动SoftAp接口 */
    if (wifi_softap_enable(&hapd_conf) != ERRCODE_SUCC) {
        PRINT("softap enable fail.\r\n");
        return ERRCODE_FAIL;
    }

    /* 配置DHCP服务器 */
    netif_p = netif_find(ifname);
    if (netif_p == NULL) {
        PRINT("find netif fail.\r\n", ifname);
        (void)wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    if (netifapi_netif_set_addr(netif_p, &st_ipaddr, &st_netmask, &st_gw) != ERR_OK) {
        PRINT("set addr() fail.\r\n");
        (void)wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    if (netifapi_dhcps_start(netif_p, NULL, 0) != ERR_OK) {
        PRINT("dhcps start() fail.\r\n");
        (void)wifi_softap_disable();
        return ERRCODE_FAIL;
    }
    PRINT("SoftAp start success.\r\n");
    return ERRCODE_SUCC;
}

static int demo_init(const char *arg)
{
    unused(arg);

    (void)osal_msleep(5000); /* 延时5s，等待wifi初始化完毕 */
    /* 启SoftAP功能和DHCP服务 */
    if (example_softap_function() != ERRCODE_SUCC) {
        return -1;
    }
    return 0;
}

static void example_wifi_softap_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)demo_init, 0, "WiFiSoftAPTask", WIFI_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, WIFI_SOFTAP_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the example_wifi_softap_entry. */
app_run(example_wifi_softap_entry);