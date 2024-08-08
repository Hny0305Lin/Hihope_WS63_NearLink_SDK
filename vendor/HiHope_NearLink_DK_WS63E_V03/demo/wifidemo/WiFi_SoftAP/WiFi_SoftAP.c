/**
# Copyright (C) 2024 HiHope Open Source Organization .
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "app_init.h"
#define WIFI_CONN_STATUS_OLED_DISPLAY 0  /* OLED屏显示STA连接和断开情况 */
#define WIFI_CONN_STATUS_LED_INDICATOR 0 /* LED等体现是否有STA连接 */

#if WIFI_CONN_STATUS_OLED_DISPLAY
#include "OLED_Indicator.h"

#endif

#if WIFI_CONN_STATUS_LED_INDICATOR
#include "LED_Indicator.h"

#endif

#if WIFI_CONN_STATUS_LED_INDICATOR || WIFI_CONN_STATUS_OLED_DISPLAY
/* SDK中有2个osal_adapt.h */
extern int osal_adapt_atomic_read(osal_atomic *atomic);
extern void osal_adapt_atomic_inc(osal_atomic *atomic);
extern void osal_adapt_atomic_dec(osal_atomic *atomic);

#endif

#define WIFI_IFNAME_MAX_SIZE 16

/*****************************************************************************
  WiFi_SoftAP sample用例
*****************************************************************************/
static errcode_t example_softap_function(void)
{
    /* SoftAp SSID */
    char ssid[WIFI_MAX_SSID_LEN] = "NearLink_DK63_WiFi_SoftAP_demo";

    char pre_shared_key[WIFI_MAX_KEY_LEN] = "my_password";
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

#if WIFI_CONN_STATUS_LED_INDICATOR || WIFI_CONN_STATUS_OLED_DISPLAY
static osal_atomic g_sta_num = {0};

static void example_wifi_event_softap_sta_join(const wifi_sta_info_stru *info)
{
    PRINT("SoftAp addr %02x:**:**:**:%02x:%02x jion\r\n", info->mac_addr[0], info->mac_addr[4], info->mac_addr[5]);
    osal_adapt_atomic_inc(&g_sta_num);

/* OLED屏上显示 */
#if WIFI_CONN_STATUS_OLED_DISPLAY
    example_oled_disaplay_printf("%02x***%02x%02x jion\r\n", info->mac_addr[0], info->mac_addr[4], info->mac_addr[5]);
#endif
}

static void example_wifi_event_softap_sta_leave(const wifi_sta_info_stru *info)
{
    PRINT("SoftAp addr %02x:**:**:**:%02x:%02x leave\r\n", info->mac_addr[0], info->mac_addr[4], info->mac_addr[5]);
    osal_adapt_atomic_dec(&g_sta_num);

/* OLED屏上显示 */
#if WIFI_CONN_STATUS_OLED_DISPLAY
    example_oled_disaplay_printf("%02x***%02x%02x leave\r\n", info->mac_addr[0], info->mac_addr[4], info->mac_addr[5]);
#endif
}

static errcode_t example_wifi_event_callback_register(void)
{
    errcode_t ret = ERRCODE_FAIL;
    wifi_event_stru event_cb = {0};

    event_cb.wifi_event_softap_sta_join = example_wifi_event_softap_sta_join;
    event_cb.wifi_event_softap_sta_leave = example_wifi_event_softap_sta_leave;

    ret = wifi_register_event_cb(&event_cb);
    if (ret != ERRCODE_SUCC) {
        PRINT("SoftAp reg event cb failed! ret = 0x%x\r\n", ret);
        return ret;
    }

    return ret;
}

#endif

static int example_wifi_softap_task(const char *arg)
{
#if WIFI_CONN_STATUS_LED_INDICATOR || WIFI_CONN_STATUS_OLED_DISPLAY
    errcode_t ret = ERRCODE_FAIL;
#endif

#if WIFI_CONN_STATUS_LED_INDICATOR
    uint32_t sta_num = 0;
    uint32_t last_sta_num = 0;
    bool first_indicate = true;
#endif

    unused(arg);

    (void)osal_msleep(5000); /* 延时5s，等待wifi初始化完毕 */

#if WIFI_CONN_STATUS_LED_INDICATOR || WIFI_CONN_STATUS_OLED_DISPLAY
    /* 注册回调 */
    ret = example_wifi_event_callback_register();
    if (ret != ERRCODE_SUCC) {
        return -1;
    }

/* 初始化OLED屏 */
#if WIFI_CONN_STATUS_OLED_DISPLAY
    example_oled_init();
#endif

#endif

    /* 启SoftAP功能和DHCP服务 */
    if (example_softap_function() != ERRCODE_SUCC) {
        return -1;
    }

#if WIFI_CONN_STATUS_LED_INDICATOR
    do {
        (void)osal_msleep(1000); /* 延时1s，每个1s判断一次是否有STA连接 */

        sta_num = osal_adapt_atomic_read(&g_sta_num);
		/* 连接到本SoftAP的STA数量为0，且之前连接到本SoftAP的STA数量不为0。或者，连接到本SoftAP的STA数量为0，且是第一次操作指示灯 */
        if (sta_num == 0 && (last_sta_num != 0 || first_indicate)) {
            example_wifi_connect_indicator_exit();
            example_wifi_disconnect_indicator_entry();
            first_indicate = false;
        } else if (sta_num != 0 && (last_sta_num == 0 || first_indicate)) {
            example_wifi_disconnect_indicator_exit();
            example_wifi_connect_indicator_entry();
            first_indicate = false;
        }
        last_sta_num = sta_num;
    } while (1);
#endif

    return 0;
}

#define WIFI_SOFTAP_TASK_PRIO 24
#define WIFI_SOFTAP_TASK_STACK_SIZE 0x2000

static void example_wifi_softap_entry(void)
{
    osal_task *task_handle = NULL;

    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)example_wifi_softap_task, 0, "WiFiSoftAPTask",
                                      WIFI_SOFTAP_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, WIFI_SOFTAP_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the example_wifi_softap_entry. */
app_run(example_wifi_softap_entry);