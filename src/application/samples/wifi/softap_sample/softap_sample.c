/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Application core main function for standard \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"

#define WIFI_IFNAME_MAX_SIZE            16
#define WIFI_MAX_KEY_LEN                65
#define WIFI_MAX_SSID_LEN               33
#define WIFI_SOFTAP_SAMPLE_LOG          "[WIFI_SOFTAP_SAMPLE]"

#define WIFI_TASK_PRIO                  (osPriority_t)(13)
#define WIFI_TASK_DURATION_MS           2000
#define WIFI_TASK_STACK_SIZE            0x1000
/*****************************************************************************
  SoftAP sample用例
*****************************************************************************/
td_s32 example_softap_function(td_void)
{
    /* SoftAp接口的信息 */
    td_char ssid[WIFI_MAX_SSID_LEN] = "my_softAP";
    td_char pre_shared_key[WIFI_MAX_KEY_LEN] = "my_password";
    softap_config_stru hapd_conf = {0};
    softap_config_advance_stru config = {0};
    td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "ap0"; /* 创建的SoftAp接口名 */
    struct netif *netif_p = TD_NULL;
    ip4_addr_t   st_gw;
    ip4_addr_t   st_ipaddr;
    ip4_addr_t   st_netmask;
    IP4_ADDR(&st_ipaddr, 192, 168, 43, 1); /* IP地址设置：192.168.43.1 */
    IP4_ADDR(&st_netmask, 255, 255, 255, 0); /* 子网掩码设置：255.255.255.0 */
    IP4_ADDR(&st_gw, 192, 168, 43, 2); /* 网关地址设置：192.168.43.2 */

    /* 配置SoftAp基本参数 */
    (td_void)memcpy_s(hapd_conf.ssid, sizeof(hapd_conf.ssid), ssid, sizeof(ssid));
    (td_void)memcpy_s(hapd_conf.pre_shared_key, WIFI_MAX_KEY_LEN, pre_shared_key, WIFI_MAX_KEY_LEN);
    hapd_conf.security_type = 3; /* 3: 加密方式设置为WPA_WPA2_PSK */
    hapd_conf.channel_num = 13; /* 13：工作信道设置为13信道 */
    hapd_conf.wifi_psk_type = 0;

    /* 配置SoftAp网络参数 */
    config.beacon_interval = 100; /* 100：Beacon周期设置为100ms */
    config.dtim_period = 2; /* 2：DTIM周期设置为2 */
    config.gi = 0; /* 0：short GI默认关闭 */
    config.group_rekey = 86400; /* 86400：组播秘钥更新时间设置为1天 */
    config.protocol_mode = 4; /* 4：协议类型设置为802.11b + 802.11g + 802.11n + 802.11ax */
    config.hidden_ssid_flag = 1; /* 1：不隐藏SSID */
    if (wifi_set_softap_config_advance(&config) != 0) {
        return -1;
    }
    /* 启动SoftAp接口 */
    if (wifi_softap_enable(&hapd_conf) != 0) {
        return -1;
    }
    /* 配置DHCP服务器 */
    netif_p = netif_find(ifname);
    if (netif_p == TD_NULL) {
        (td_void)wifi_softap_disable();
        return -1;
    }
    if (netifapi_netif_set_addr(netif_p, &st_ipaddr, &st_netmask, &st_gw) != 0) {
        (td_void)wifi_softap_disable();
        return -1;
    }
    if (netifapi_dhcps_start(netif_p, NULL, 0) != 0) {
        (td_void)wifi_softap_disable();
        return -1;
    }
    PRINT("%s::SoftAp start success.\r\n", WIFI_SOFTAP_SAMPLE_LOG);
    return 0;
}

int softap_sample_init(void *param)
{
    param = param;

    /* 等待wifi初始化完成 */
    while (wifi_is_wifi_inited() == 0) {
        (void)osDelay(10); /* 1: 等待100ms后判断状态 */
    }
    PRINT("%s::wifi init succ.\r\n", WIFI_SOFTAP_SAMPLE_LOG);

    if (example_softap_function() != 0) {
        PRINT("%s::example_softap_function fail.\r\n", WIFI_SOFTAP_SAMPLE_LOG);
        return -1;
    }
    return 0;
}

static void softap_sample_entry(void)
{
    osThreadAttr_t attr;
    attr.name       = "softap_sample_task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = WIFI_TASK_STACK_SIZE;
    attr.priority   = WIFI_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)softap_sample_init, NULL, &attr) == NULL) {
        PRINT("%s::Create softap_sample_task fail.\r\n", WIFI_SOFTAP_SAMPLE_LOG);
    }
    PRINT("%s::Create softap_sample_task succ.\r\n", WIFI_SOFTAP_SAMPLE_LOG);
}

/* Run the sta_sample_task. */
app_run(softap_sample_entry);