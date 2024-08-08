/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Application core main function for standard \n
 * Author: @CompanyNameTag \n
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
#include "soc_osal.h"
#include "radar_service.h"
#include "gpio.h"
#include "pinctrl.h"
#include "app_init.h"

#define WIFI_IFNAME_MAX_SIZE             16
#define WIFI_MAX_KEY_LEN                 65
#define WIFI_MAX_SSID_LEN                33
#define WIFI_INIT_WAIT_TIME              5000 // 5s
#define WIFI_START_SOFTAP_DELAY          1000 // 1s
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

#define RADAR_API_NO_HUMAN 0
#define RADAR_API_RANGE_CLOSE 50
#define RADAR_API_RANGE_NEAR 100
#define RADAR_API_RANGE_MEDIUM 200
#define RADAR_API_RANGE_FAR 600

#define CONFIG_RED_LED_PIN 7
#define CONFIG_GREEN_LED_PIN 11
#define CONFIG_YELLOW_LED_PIN 10

#define RADAR_TASK_PRIO              24
#define RADAR_TASK_STACK_SIZE        0x1000

static void gpio_set_value(pin_t pin)
{
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT);
}

static void radar_led_init(void)
{
    // 1. 初始化所有GPIO并设置GPIO的类型
    uapi_gpio_init();
    gpio_set_value(CONFIG_RED_LED_PIN);
    gpio_set_value(CONFIG_GREEN_LED_PIN);
    gpio_set_value(CONFIG_YELLOW_LED_PIN);
}

static void radar_switch_green_on_off(bool onoff)
{
    if (onoff) {
        uapi_gpio_set_val(CONFIG_GREEN_LED_PIN, GPIO_LEVEL_LOW);
    } else {
        uapi_gpio_set_val(CONFIG_GREEN_LED_PIN, GPIO_LEVEL_HIGH);
    }
}

static void radar_switch_yellow_on_off(bool onoff)
{
    if (onoff) {
        uapi_gpio_set_val(CONFIG_YELLOW_LED_PIN, GPIO_LEVEL_LOW);
    } else {
        uapi_gpio_set_val(CONFIG_YELLOW_LED_PIN, GPIO_LEVEL_HIGH);
    }
}

static void radar_switch_red_on_off(bool onoff)
{
    if (onoff) {
        uapi_gpio_set_val(CONFIG_RED_LED_PIN, GPIO_LEVEL_LOW);
    } else {
        uapi_gpio_set_val(CONFIG_RED_LED_PIN, GPIO_LEVEL_HIGH);
    }
}

static void radar_ctrl_led(radar_result_t *res)
{
    if (res->is_human_presence) {
        radar_switch_green_on_off(true);
    } else {
        radar_switch_green_on_off(false);
    }
    if (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR) {
        printf("R 1 meters\r\n");
        radar_switch_red_on_off(true);
        radar_switch_yellow_on_off(true);
    } else if ((res->lower_boundary == RADAR_API_RANGE_NEAR && res->upper_boundary == RADAR_API_RANGE_MEDIUM)) {
        printf("R 2 meters\r\n");
        radar_switch_red_on_off(false);
        radar_switch_yellow_on_off(true);
    } else {
        printf("R xxxx meters\r\n");
        radar_switch_red_on_off(false);
        radar_switch_yellow_on_off(false);
    }
}

/*****************************************************************************
  Radar SoftAP+Socket sample用例
*****************************************************************************/
static errcode_t radar_start_softap(void)
{
    /* SoftAp接口的信息 */
    char ssid[WIFI_MAX_SSID_LEN] = "my_softAP";
    char pre_shared_key[WIFI_MAX_KEY_LEN] = "123456789";
    softap_config_stru hapd_conf = {0};
    softap_config_advance_stru config = {0};
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "ap0"; /* 创建的SoftAp接口名 */
    struct netif *netif_p = TD_NULL;
    ip4_addr_t   st_gw;
    ip4_addr_t   st_ipaddr;
    ip4_addr_t   st_netmask;
    IP4_ADDR(&st_ipaddr, WIFI_IP_1, WIFI_IP_2, WIFI_IP_3, WIFI_IP_4); /* IP地址设置：192.168.43.1 */
    IP4_ADDR(&st_netmask, WIFI_IP_5, WIFI_IP_5, WIFI_IP_5, 0); /* 子网掩码设置：255.255.255.0 */
    IP4_ADDR(&st_gw, WIFI_IP_1, WIFI_IP_2, WIFI_IP_3, WIFI_IP_4); /* 网关地址设置：192.168.43.2 */

    osal_msleep(WIFI_INIT_WAIT_TIME);
    printf("SoftAP try enable.\r\n");

    (void)memcpy_s(hapd_conf.ssid, sizeof(hapd_conf.ssid), ssid, sizeof(ssid));
    (void)memcpy_s(hapd_conf.pre_shared_key, WIFI_MAX_KEY_LEN, pre_shared_key, WIFI_MAX_KEY_LEN);
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
        (void)wifi_softap_disable();
        return -1;
    }
    if (netifapi_netif_set_addr(netif_p, &st_ipaddr, &st_netmask, &st_gw) != 0) {
        (void)wifi_softap_disable();
        return -1;
    }
    if (netifapi_dhcps_start(netif_p, NULL, 0) != 0) {
        (void)wifi_softap_disable();
        return -1;
    }
    printf("SoftAp start success.\r\n");
    return 0;
}

static void radar_printf_res(radar_result_t *res)
{
    printf("[RADAR_SAMPLE] lb:%u, hb:%u, hm:%u\r\n", res->lower_boundary, res->upper_boundary, res->is_human_presence);
    radar_ctrl_led(res);
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

static void radar_demo_init(const char *arg)
{
    UNUSED(arg);
    printf("[RADAR_SAMPLE] radar_demo_init softap!\r\n");
    radar_led_init();
    radar_start_softap();
    radar_init_para();
    uapi_radar_register_result_cb(radar_printf_res);
    // 启动雷达
    osal_msleep(WIFI_START_SOFTAP_DELAY);
    uapi_radar_set_status(RADAR_STATUS_START);

    for (;;) {
        uapi_radar_set_delay_time(8); // 设置退出延迟时间为8S,目前最小时间为8
        osal_msleep(RADAR_STATUS_QUERY_DELAY);
        uint8_t sts;
        uapi_radar_get_status(&sts);
        uint16_t time;
        uapi_radar_get_delay_time(&time);
        uint16_t iso;
        uapi_radar_get_isolation(&iso);
    }
}

static void radar_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)radar_demo_init, 0, "RadarTask", RADAR_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, RADAR_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the radar_entry. */
app_run(radar_entry);
