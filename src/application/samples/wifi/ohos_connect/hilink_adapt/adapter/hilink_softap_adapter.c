/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: SoftAP适配实现 （此文件为DEMO，需集成方适配修改）
 */
#include "hilink_softap_adapter.h"

#include <stddef.h>
#include "securec.h"
#include "wifi_device.h"
#include "wifi_hotspot.h"
#include "hilink_sal_defines.h"
#include "lwip/netifapi.h"
#include "lwip/nettool/ifconfig.h"
#include "osal_task.h"

typedef struct {
    const char *ifname;
    const char *ip_str;
    const char *netmask;
    const char *netmask_value;
    const char *gateway;
    const char *gateway_value;
} ifcfg_args_t;

static void CheckWifiStateBeforeStartSoftap(void)
{
    /* 套餐2存在hilink重入，重入后起softap需要保证sta与ap已关闭 */
    if (wifi_is_sta_enabled() == 1) {
        HILINK_SAL_NOTICE("sta active, try stop!\r\n");
        if (wifi_sta_disable() != ERRCODE_SUCC) {
            HILINK_SAL_WARN("stop sta fail\r\n");
        }
    }

    if (wifi_is_softap_enabled() == 1) {
        HILINK_SAL_NOTICE("ap active, try stop!\r\n");
        if (wifi_softap_disable() != ERRCODE_SUCC) {
            HILINK_SAL_WARN("stop ap fail\r\n");
        }
    }
}

static int softap_start_dhcps(void)
{
    int ret;
    struct netif *netif_p = netifapi_netif_find("ap0");
    if (netif_p == NULL) {
        HILINK_SAL_WARN("netif_p is null\r\n");
        return HILINK_SAL_NOK;
    }
    if (ip_addr_isany_val(netif_p->ip_addr)) {
        HILINK_SAL_WARN("Please set ip address for dhcp server\r\n");
        return HILINK_SAL_NOK;
    }

    ret = netifapi_dhcps_start(netif_p, NULL, 0);
    if (ret == 0) {
        HILINK_SAL_WARN("softap dhcps set OK\r\n");
        return HILINK_SAL_OK;
    }

    return HILINK_SAL_NOK;
}

static int softap_config_static_ip(void)
{
    int ret = 0;
    ifcfg_args_t ifconfig_param;
    memset_s(&ifconfig_param, sizeof(ifconfig_param), 0, sizeof(ifconfig_param));

    ifconfig_param.ifname = "ap0";
    ifconfig_param.ip_str = "192.168.3.1";
    ifconfig_param.netmask = "netmask";
    ifconfig_param.netmask_value = "255.255.255.0";
    ifconfig_param.gateway = "gateway";
    ifconfig_param.gateway_value = "192.168.3.1";

    lwip_ifconfig(6, &ifconfig_param.ifname); // 6: ifconfig param number

    return HILINK_SAL_OK;
}

int HILINK_StartSoftAp(const char *ssid, unsigned int ssidLen)
{
    if ((ssid == NULL) || (ssidLen == 0)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    CheckWifiStateBeforeStartSoftap();

    softap_config_stru config;
    (void)memset_s(&config, sizeof(softap_config_stru), 0, sizeof(softap_config_stru));

    if (strcpy_s((char *)config.ssid, sizeof(config.ssid), ssid) != EOK) {
        HILINK_SAL_WARN("strcpy error\r\n");
        return HILINK_SAL_STRCPY_ERR;
    }

    config.security_type = WIFI_SEC_TYPE_OPEN;
    config.channel_num = 6; /* 6: channel num */

    if (wifi_softap_enable(&config) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("enable hotspot fail\r\n");
        return HILINK_SAL_SET_SOFTAP_ERR;
    }

    if (softap_config_static_ip() != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("set softap ip failed\r\n");
        return HILINK_SAL_SET_SOFTAP_ERR;
    }
    osal_msleep(4000); /* 4000：配置静态ip之后要睡眠4秒之后再开启dhcps */

    // dhcps
    if (softap_start_dhcps() != 0) {
        HILINK_SAL_ERROR("set softap dhcps failed\r\n");
        return HILINK_SAL_SET_SOFTAP_ERR;
    }

    return HILINK_SAL_OK;
}

int HILINK_StopSoftAp(void)
{
    if (wifi_is_softap_enabled() == 1) {
        if (wifi_softap_disable() != ERRCODE_SUCC) {
            HILINK_SAL_ERROR("Stop softAp disable hotspot fail.\r\n");
            return HILINK_SAL_SET_SOFTAP_ERR;
        }
    }
    return HILINK_SAL_OK;
}