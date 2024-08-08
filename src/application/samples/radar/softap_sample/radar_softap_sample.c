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
#include "soc_osal.h"
#include "radar_service.h"

#define WIFI_IFNAME_MAX_SIZE             16
#define WIFI_MAX_KEY_LEN                 65
#define WIFI_MAX_SSID_LEN                33
#define WIFI_INIT_WAIT_TIME              500 // 5s
#define WIFI_START_SOFTAP_DELAY          100 // 1s
#define WIFI_IP_1                        192
#define WIFI_IP_2                        168
#define WIFI_IP_3                        130
#define WIFI_IP_4                        1
#define WIFI_IP_5                        255

#define WIFI_CHANNEL_NUM                 12
#define WIFI_SECURITY_TYPE               3

#define WIFI_BECAON_INTERVAL             100
#define WIFI_DTIM_PERIOD                 2
#define WIFI_GROUP_REKEY                 86400
#define WIFI_PROTOCOL_MODE               4
#define WIFI_HIDDEN_SSID_FLAG            1

#define RADAR_STATUS_QUERY_DELAY         1000 // 10s

#define RADAR_DEFAULT_LOOP               8
#define RADAR_DEFAULT_PERIOD             5000
#define RADAR_DEFAULT_DBG_TYPE           1
#define RADAR_DEFAULT_WAVE               2

/*****************************************************************************
  Radar SoftAP+Socket sample用例
*****************************************************************************/
td_s32 radar_start_softap(td_void)
{
    /* SoftAp接口的信息 */
    td_char ssid[WIFI_MAX_SSID_LEN] = "my_softAP";
    td_char pre_shared_key[WIFI_MAX_KEY_LEN] = "123456789";
    softap_config_stru hapd_conf = {0};
    softap_config_advance_stru config = {0};
    td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "ap0"; /* 创建的SoftAp接口名 */
    struct netif *netif_p = TD_NULL;
    ip4_addr_t   st_gw;
    ip4_addr_t   st_ipaddr;
    ip4_addr_t   st_netmask;
    IP4_ADDR(&st_ipaddr, WIFI_IP_1, WIFI_IP_2, WIFI_IP_3, WIFI_IP_4); /* IP地址设置：192.168.43.1 */
    IP4_ADDR(&st_netmask, WIFI_IP_5, WIFI_IP_5, WIFI_IP_5, 0); /* 子网掩码设置：255.255.255.0 */
    IP4_ADDR(&st_gw, WIFI_IP_1, WIFI_IP_2, WIFI_IP_3, WIFI_IP_4); /* 网关地址设置：192.168.43.2 */

    (void)osDelay(WIFI_INIT_WAIT_TIME); /* 500: 延时5s，等待wifi初始化完毕 */
    PRINT("SoftAP try enable.\r\n");

    (td_void)memcpy_s(hapd_conf.ssid, sizeof(hapd_conf.ssid), ssid, sizeof(ssid));
    (td_void)memcpy_s(hapd_conf.pre_shared_key, WIFI_MAX_KEY_LEN, pre_shared_key, WIFI_MAX_KEY_LEN);
    hapd_conf.security_type = WIFI_SECURITY_TYPE; /* 3: 加密方式设置为WPA_WPA2_PSK */
    hapd_conf.channel_num = WIFI_CHANNEL_NUM; /* 12：工作信道设置为12信道 */
    hapd_conf.wifi_psk_type = 0;

    /* 配置SoftAp网络参数 */
    config.beacon_interval = WIFI_BECAON_INTERVAL; /* 100：Beacon周期设置为100ms */
    config.dtim_period = WIFI_DTIM_PERIOD; /* 2：DTIM周期设置为2 */
    config.gi = 0; /* 0：short GI默认关闭 */
    config.group_rekey = WIFI_GROUP_REKEY; /* 86400：组播秘钥更新时间设置为1天 */
    config.protocol_mode = WIFI_PROTOCOL_MODE; /* 4：协议类型设置为802.11b + 802.11g + 802.11n + 802.11ax */
    config.hidden_ssid_flag = WIFI_HIDDEN_SSID_FLAG; /* 1：不隐藏SSID */
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
    PRINT("SoftAp start success.\r\n");
    return 0;
}

static void radar_print_res(radar_result_t *res)
{
    PRINT("[RADAR_SAMPLE] lb:%u, hb:%u, hm:%u\r\n", res->lower_boundary, res->upper_boundary, res->is_human_presence);
}

typedef struct {
    uint8_t times; // 发射次数, 0-无限次
    uint8_t loop; // 单次雷达工作, TRx的波形数量
    uint8_t ant; // Rx天线数量
    uint8_t wave; // 波形选择, 0-320M/40M CTA, 1-160M/20M CW
    uint8_t dbg_type; // 维测方式. 0-不外发维测数据, 1-外发脉压后的数据, 2-外发相干累加后的数据
    uint16_t period; // 雷达工作间隔
} radar_driver_para_t;

static void radar_set_driver_para_weakref(radar_driver_para_t *para) __attribute__ ((weakref("radar_set_driver_para")));

static void radar_init_para(void)
{
    radar_driver_para_t para;
    para.ant = 0;
    para.dbg_type = RADAR_DEFAULT_DBG_TYPE;
    para.loop = RADAR_DEFAULT_LOOP;
    para.period = RADAR_DEFAULT_PERIOD;
    para.times = 0; // 1-使用软件控雷达, 0-使用硬件控雷达
    para.wave = RADAR_DEFAULT_WAVE;
    radar_set_driver_para_weakref(&para);
}

int radar_demo_init(void *param)
{
    PRINT("[RADAR_SAMPLE] radar_demo_init softap!\r\n");

    param = param;
    radar_start_softap();
    radar_init_para();
    uapi_radar_register_result_cb(radar_print_res);
    // 启动雷达
    (void)osDelay(WIFI_START_SOFTAP_DELAY);
    uapi_radar_set_status(RADAR_STATUS_START);

    for (;;) {
        (void)osDelay(RADAR_STATUS_QUERY_DELAY);
        uint8_t sts;
        uapi_radar_get_status(&sts);
        uint16_t time;
        uapi_radar_get_delay_time(&time);
        uint16_t iso;
        uapi_radar_get_isolation(&iso);
    }

    return 0;
}
