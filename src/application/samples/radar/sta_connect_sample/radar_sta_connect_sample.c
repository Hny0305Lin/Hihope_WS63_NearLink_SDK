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
#include "radar_service.h"
#include "gpio.h"
#include "pinctrl.h"

#define WIFI_IFNAME_MAX_SIZE             16
#define WIFI_MAX_SSID_LEN                33
#define WIFI_SCAN_AP_LIMIT               64
#define WIFI_MAC_LEN                     6
#define WIFI_NOT_AVALLIABLE              0
#define WIFI_AVALIABE                    1
#define WIFI_GET_IP_MAX_COUNT            300

#define WIFI_TASK_PRIO                  (osPriority_t)(13)
#define WIFI_TASK_STACK_SIZE            0x1000

#define RADAR_STA_SAMPLE_LOG             "[RADAR_STA_SAMPLE]"
#define RADAR_STATUS_QUERY_DELAY         1000 // 10s
#define RPT_CTRL_DELAY                   30  // 300ms

#define RADAR_DEFAULT_LOOP 8
#define RADAR_DEFAULT_PERIOD 5000
#define RADAR_DEFAULT_DBG_TYPE 0
#define RADAR_DEFAULT_WAVE 2

#define RADAR_API_NO_HUMAN 0
#define RADAR_API_RANGE_CLOSE 50
#define RADAR_API_RANGE_NEAR 100
#define RADAR_API_RANGE_MEDIUM 200
#define RADAR_API_RANGE_FAR 600

#define LOG_UART_BAUD_RATE 9600

#define CTRL_AIR_CONDITIONER_LI

#ifdef CTRL_WATER_HEATER
#define RPT_1M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x02', '\x00', '\x0B', '\x77'                           \
    }
#define RPT_2M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x02', '\x00', '\x2A', '\xA6'                           \
    }
#define RPT_6M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x02', '\x00', '\x2B', '\xA7'                           \
    }
#define RPT_OPEN_CTRL                                                            \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x01', '\x00', '\x01', '\x6C'                           \
    }
#define RPT_CLOSE_CTRL                                                           \
    {                                                                            \
        '\xFF', '\xFF', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x5D', '\x01', '\x00', '\x00', '\x6B'                           \
    }
#define NEED_OPEN_CTRL
#endif

#ifdef CTRL_AIR_CONDITIONER_LI
#define RPT_1M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x36', '\x22', '\x26', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x01', '\x06', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x1E', '\x00',  \
        '\x00', '\x00', '\x9E', '\xC1', '\x0D'                                   \
    }
#define RPT_2M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x34', '\x22', '\x26', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x01', '\x06', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x1E', '\x00',  \
        '\x00', '\x00', '\x9C', '\x7E', '\xFE'                                   \
    }
#define RPT_6M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x30', '\x22', '\x26', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x06', '\x02', '\x00', '\x00',  \
        '\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x1E', '\x00',  \
        '\x00', '\x00', '\x98', '\xC3', '\x56'                                   \
    }
#define RPT_CLOSE_CTRL                                                           \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x37', '\x22', '\x26', '\x00', '\x02', '\x00',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x06', '\x06', '\x00', '\x00',  \
        '\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x1E', '\x00',  \
        '\x00', '\x00', '\xA2', '\xA9', '\xAD'                                   \
    }
#define NEED_HANDSHAKE
#endif

#ifdef CTRL_AIR_CONDITIONER_GUA
#define RPT_1M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x36', '\x01', '\x20', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x01', '\x06', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x00', '\x00', '\x59', '\x77', '\x94'                                   \
    }
#define RPT_2M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x34', '\x06', '\x20', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x01', '\x06', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x00', '\x00', '\x5C', '\xA6', '\xCD'                                   \
    }
#define RPT_6M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x30', '\x06', '\x20', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x06', '\x02', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x00', '\x00', '\x59', '\x5F', '\x6A'                                   \
    }
#define RPT_CLOSE_CTRL                                                           \
    {                                                                            \
        '\xFF', '\xFF', '\x24', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x30', '\x06', '\x20', '\x00', '\x02', '\x00',  \
        '\x00', '\x00', '\x00', '\x00', '\x30', '\x06', '\x02', '\x00', '\x00',  \
        '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x00', '\x00', '\x58', '\x8E', '\x7A'                                   \
    }
#define NEED_HANDSHAKE
#endif

#ifdef CTRL_AIR_CONDITIONER_GUA_SH
#define RPT_1M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x14', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x0B', '\x01', '\x23', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\xE8', '\x7F', '\xA2'                   \
    }
#define RPT_2M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x14', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x0A', '\x01', '\x23', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\xE7', '\xBA', '\xF3'                   \
    }
#define RPT_6M_CTRL                                                              \
    {                                                                            \
        '\xFF', '\xFF', '\x14', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x08', '\x06', '\x21', '\x00', '\x02', '\x01',  \
        '\x00', '\x00', '\x00', '\x00', '\xE8', '\x99', '\xF5'                   \
    }
#define RPT_CLOSE_CTRL                                                           \
    {                                                                            \
        '\xFF', '\xFF', '\x14', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x60', '\x01', '\x08', '\x06', '\x21', '\x00', '\x02', '\x00',  \
        '\x00', '\x00', '\x00', '\x00', '\xE7', '\x59', '\xC8'                   \
    }
#define NEED_HANDSHAKE
#endif

#ifdef NEED_HANDSHAKE
#define HANDSHAKE_STEP_ONE                                                       \
    {                                                                            \
        '\xFF', '\xFF', '\x0A', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x61', '\x00', '\x0F', '\x7A'                                           \
    }
#define HANDSHAKE_STEP_TWO                                                       \
    {                                                                            \
        '\xFF', '\xFF', '\x08', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x70', '\xB8', '\x86', '\x41'                                           \
    }
#define HANDSHAKE_STEP_THIRD                                                     \
    {                                                                            \
        '\xFF', '\xFF', '\x0A', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x01', '\x4D', '\x01', '\x99', '\xB3', '\xB4'                           \
    }
#define HANDSHAKE_STEP_FOUR                                                      \
    {                                                                            \
        '\xFF', '\xFF', '\x08', '\x40', '\x00', '\x00', '\x00', '\x00', '\x00',  \
        '\x73', '\xBB', '\x87', '\x01'                                           \
    }
#endif

typedef struct {
    uint8_t times; // 发射次数, 0-无限次
    uint8_t loop; // 单次雷达工作, TRx的波形数量
    uint8_t ant; // Rx天线数量
    uint8_t wave; // 波形选择, 0-320M/40M CTA, 1-160M/20M CW
    uint8_t dbg_type; // 维测方式. 0-不外发维测数据, 1-外发脉压后的数据, 2-外发相干累加后的数据
    uint16_t period; // 雷达工作间隔
} radar_driver_para_t;

static void radar_set_driver_para_weakref(radar_driver_para_t *para) __attribute__ ((weakref("radar_set_driver_para")));
static int radar_socket_server_weakref(void) __attribute__ ((weakref("radar_socket_server")));
static td_void wifi_scan_state_changed(td_s32 state, td_s32 size);
static td_void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code);
static void radar_print_dbg_data_weakref(uint8_t *buf, uint16_t len) __attribute__ ((weakref("radar_print_dbg_data")));

wifi_event_stru wifi_event_cb = {
    .wifi_event_connection_changed      = wifi_connection_changed,
    .wifi_event_scan_state_changed      = wifi_scan_state_changed,
};

enum {
    WIFI_STA_SAMPLE_INIT = 0,       /* 0:初始态 */
    WIFI_STA_SAMPLE_SCANING,        /* 1:扫描中 */
    WIFI_STA_SAMPLE_SCAN_DONE,      /* 2:扫描完成 */
    WIFI_STA_SAMPLE_FOUND_TARGET,   /* 3:匹配到目标AP */
    WIFI_STA_SAMPLE_CONNECTING,     /* 4:连接中 */
    WIFI_STA_SAMPLE_CONNECT_DONE,   /* 5:关联成功 */
    WIFI_STA_SAMPLE_GET_IP,         /* 6:获取IP */
} wifi_state_enum;

static td_u8 g_wifi_state = WIFI_STA_SAMPLE_INIT;

/*****************************************************************************
  STA 扫描事件回调函数
*****************************************************************************/
static td_void wifi_scan_state_changed(td_s32 state, td_s32 size)
{
    UNUSED(state);
    UNUSED(size);
    g_wifi_state = WIFI_STA_SAMPLE_SCAN_DONE;
}

/*****************************************************************************
  STA 关联事件回调函数
*****************************************************************************/
static td_void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(info);
    UNUSED(reason_code);

    if (state == WIFI_NOT_AVALLIABLE) {
        PRINT("%s::Connect fail!. try agin !\r\n", RADAR_STA_SAMPLE_LOG);
        g_wifi_state = WIFI_STA_SAMPLE_INIT;
    } else {
        g_wifi_state = WIFI_STA_SAMPLE_CONNECT_DONE;
    }
}

/*****************************************************************************
  STA 匹配目标AP
*****************************************************************************/
td_s32 example_get_match_network(wifi_sta_config_stru *expected_bss)
{
    td_s32  ret;
    td_u32  num = 64; /* 64:扫描到的Wi-Fi网络数量 */
    td_char expected_ssid[] = "my_softAP";
    td_char key[] = "123456789"; /* 待连接的网络接入密码 */
    td_bool find_ap = TD_FALSE;
    td_u8   bss_index;
    /* 获取扫描结果 */
    td_u32 scan_len = sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
    if (result == TD_NULL) {
        return -1;
    }
    memset_s(result, scan_len, 0, scan_len);
    ret = wifi_sta_get_scan_info(result, &num);
    if (ret != 0) {
        osal_kfree(result);
        return -1;
    }
    /* 筛选扫描到的Wi-Fi网络，选择待连接的网络 */
    for (bss_index = 0; bss_index < num; bss_index ++) {
        if (strlen(expected_ssid) == strlen(result[bss_index].ssid)) {
            if (memcmp(expected_ssid, result[bss_index].ssid, strlen(expected_ssid)) == 0) {
                find_ap = TD_TRUE;
                break;
            }
        }
    }
    /* 未找到待连接AP,可以继续尝试扫描或者退出 */
    if (find_ap == TD_FALSE) {
        osal_kfree(result);
        return -1;
    }
    /* 找到网络后复制网络信息和接入密码 */
    if (memcpy_s(expected_bss->ssid, WIFI_MAX_SSID_LEN, expected_ssid, strlen(expected_ssid)) != 0) {
        osal_kfree(result);
        return -1;
    }
    if (memcpy_s(expected_bss->bssid, WIFI_MAC_LEN, result[bss_index].bssid, WIFI_MAC_LEN) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->security_type = result[bss_index].security_type;
    if (memcpy_s(expected_bss->pre_shared_key, WIFI_MAX_SSID_LEN, key, strlen(key)) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->ip_type = 1; /* 1：IP类型为动态DHCP获取 */
    osal_kfree(result);
    return 0;
}

/*****************************************************************************
  STA 关联状态查询
*****************************************************************************/
td_bool example_check_connect_status(td_void)
{
    td_u8 index;
    wifi_linked_info_stru wifi_status;
    /* 获取网络连接状态，共查询5次，每次间隔500ms */
    for (index = 0; index < 5; index ++) {
        (void)osDelay(50); /* 50: 延时500ms */
        memset_s(&wifi_status, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));
        if (wifi_sta_get_ap_info(&wifi_status) != 0) {
            continue;
        }
        if (wifi_status.conn_state == 1) {
            return 0; /* 连接成功退出循环 */
        }
    }
    return -1;
}

/*****************************************************************************
  STA DHCP状态查询
*****************************************************************************/
td_bool example_check_dhcp_status(struct netif *netif_p, td_u32 *wait_count)
{
    if ((ip_addr_isany(&(netif_p->ip_addr)) == 0) && (*wait_count <= WIFI_GET_IP_MAX_COUNT)) {
        /* DHCP成功 */
        return 0;
    }

    if (*wait_count > WIFI_GET_IP_MAX_COUNT) {
        PRINT("%s::STA DHCP timeout, try again !.\r\n", RADAR_STA_SAMPLE_LOG);
        *wait_count = 0;
        g_wifi_state = WIFI_STA_SAMPLE_INIT;
    }
    return -1;
}

/*****************************************************************************
  RADAR 相关接口
*****************************************************************************/
static void radar_uart_port_init(void)
{
    uart_attr_t uart_line_config = {
        .baud_rate = LOG_UART_BAUD_RATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    (void)uapi_uart_set_attr(LOG_UART_BUS, &uart_line_config);
}
#ifdef RADAR_ONE_GEAR_CTRL
static void radar_send_open_msg(void)
{
    const uint8_t arr[] = { '\xAA', '\x36', '\xFB', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x02', '\x01',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\xFF', '\x00', '\x00', '\x00', '\x05', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\x00', '\x00', '\xFF',
        '\xFF', '\xD0' };
    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }
}

static void radar_send_close_msg(void)
{
    const uint8_t arr[] = { '\xAA', '\x36', '\xFB', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x02', '\x02',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\xFF', '\x00', '\x00', '\x00', '\x05', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\x00', '\x00', '\xFF',
        '\xFF', '\xCF' };
    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }
}
#else
static void radar_1m_rpt(void)
{
    const uint8_t arr[] = RPT_1M_CTRL;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}

static void radar_2m_rpt(void)
{
    const uint8_t arr[] = RPT_2M_CTRL;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}

static void radar_6m_rpt(void)
{
    const uint8_t arr[] = RPT_6M_CTRL;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}

static void radar_close_machine(void)
{
    const uint8_t arr[] = RPT_CLOSE_CTRL;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}

#ifdef NEED_OPEN_CTRL
static void radar_open_machine(void)
{
    const uint8_t arr[] = RPT_OPEN_CTRL;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr, sizeof(arr));
    }
}
#endif
#endif

#ifdef RADAR_ONE_GEAR_CTRL
static void radar_ctrl_proc(radar_result_t *res)
{
    switch (g_radar_led_gear) {
        case RADAR_INSIDE_1M:
            if (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR) {
                radar_set_led_on();
                radar_send_open_msg();
            } else {
                radar_set_led_off();
                radar_send_close_msg();
            }
            break;
        case RADAR_INSIDE_2M:
            if ((res->lower_boundary == RADAR_API_RANGE_NEAR &&
                 res->upper_boundary == RADAR_API_RANGE_MEDIUM) ||
                (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR)) {
                radar_set_led_on();
                radar_send_open_msg();
            } else {
                radar_set_led_off();
                radar_send_close_msg();
            }
            break;
        default:    // 默认6M档位
            if (res->is_human_presence == 1) {
                radar_set_led_on();
                radar_send_open_msg();
            } else {
                radar_set_led_off();
                radar_send_close_msg();
            }
    }
}
#else
static void radar_ctrl_proc(radar_result_t *res)
{
    if (res->is_human_presence == 0) { // 无人
        radar_close_machine();
    } else {
        if (res->lower_boundary == 0 && res->upper_boundary == RADAR_API_RANGE_NEAR) { //1m
#ifdef NEED_OPEN_CTRL
            radar_open_machine();
#endif
            radar_1m_rpt();
        } else if (res->lower_boundary == RADAR_API_RANGE_NEAR && res->upper_boundary == RADAR_API_RANGE_MEDIUM) { // 2m
#ifdef NEED_OPEN_CTRL
            radar_open_machine();
#endif
            radar_2m_rpt();
        } else { // 6m
#ifdef NEED_OPEN_CTRL
            radar_open_machine();
#endif
            radar_6m_rpt();
        }
    }
}
#endif

static void radar_print_res(radar_result_t *res)
{
    PRINT("[RADAR_SAMPLE] lb:%u, hb:%u, hm:%u\r\n", res->lower_boundary, res->upper_boundary, res->is_human_presence);

    radar_ctrl_proc(res);
}

static void radar_socket_init(void)
{
    if (radar_socket_server_weakref != NULL) {
        radar_socket_server_weakref();
    }
}

#ifdef NEED_HANDSHAKE
static void handshake_proc(void)
{
    const uint8_t arr1[] = HANDSHAKE_STEP_ONE;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr1, sizeof(arr1));
    }

    (void)osDelay(RPT_CTRL_DELAY);

    const uint8_t arr2[] = HANDSHAKE_STEP_TWO;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr2, sizeof(arr2));
    }

    (void)osDelay(RPT_CTRL_DELAY);

    const uint8_t arr3[] = HANDSHAKE_STEP_THIRD;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr3, sizeof(arr3));
    }

    (void)osDelay(RPT_CTRL_DELAY);

     const uint8_t arr4[] = HANDSHAKE_STEP_FOUR;

    if (radar_print_dbg_data_weakref != NULL) {
        radar_print_dbg_data_weakref((uint8_t *)arr4, sizeof(arr4));
    }

    (void)osDelay(RPT_CTRL_DELAY);
}
#endif

static void radar_function(void)
{
    radar_socket_init();
    uapi_radar_register_result_cb(radar_print_res);
    (void)osDelay(100);
#ifdef NEED_HANDSHAKE
    handshake_proc();
#endif

    for (;;) {
        (void)osDelay(RADAR_STATUS_QUERY_DELAY);
        uint8_t sts;
        uapi_radar_get_status(&sts);
        uint16_t time;
        uapi_radar_get_delay_time(&time);
        uint16_t iso;
        uapi_radar_get_isolation(&iso);
    }
}

td_s32 example_sta_function(td_void)
{
    td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0"; /* 创建的STA接口名 */
    wifi_sta_config_stru expected_bss = {0}; /* 连接请求信息 */
    struct netif *netif_p = TD_NULL;
    td_u32 wait_count = 0;

    /* 创建STA接口 */
    if (wifi_sta_enable() != 0) {
        return -1;
    }
    PRINT("%s::STA enable succ.\r\n", RADAR_STA_SAMPLE_LOG);

    do {
        (void)osDelay(1); /* 1: 等待10ms后判断状态 */
        if (g_wifi_state == WIFI_STA_SAMPLE_INIT) {
            g_wifi_state = WIFI_STA_SAMPLE_SCANING;
            /* 启动STA扫描 */
            if (wifi_sta_scan() != 0) {
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                continue;
            }
        } else if (g_wifi_state == WIFI_STA_SAMPLE_SCAN_DONE) {
            /* 获取待连接的网络 */
            if (example_get_match_network(&expected_bss) != 0) {
                PRINT("%s::Do not find AP, try again !\r\n", RADAR_STA_SAMPLE_LOG);
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                continue;
            }
            g_wifi_state = WIFI_STA_SAMPLE_FOUND_TARGET;
        } else if (g_wifi_state == WIFI_STA_SAMPLE_FOUND_TARGET) {
            g_wifi_state = WIFI_STA_SAMPLE_CONNECTING;
            /* 启动连接 */
            if (wifi_sta_connect(&expected_bss) != 0) {
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                continue;
            }
        } else if (g_wifi_state == WIFI_STA_SAMPLE_CONNECT_DONE) {
            g_wifi_state = WIFI_STA_SAMPLE_GET_IP;
            netif_p = netifapi_netif_find(ifname);
            if (netif_p == TD_NULL || netifapi_dhcp_start(netif_p) != 0) {
                PRINT("%s::find netif or start DHCP fail, try again !\r\n", RADAR_STA_SAMPLE_LOG);
                g_wifi_state = WIFI_STA_SAMPLE_INIT;
                continue;
            }
        } else if (g_wifi_state == WIFI_STA_SAMPLE_GET_IP) {
            if (example_check_dhcp_status(netif_p, &wait_count) == 0) {
                break;
            }
            wait_count++;
        }
    } while (1);

    radar_function();

    return 0;
}

int sta_sample_init(void *param)
{
    param = param;

    /* 注册事件回调 */
    if (wifi_register_event_cb(&wifi_event_cb) != 0) {
        PRINT("%s::wifi_event_cb register fail.\r\n", RADAR_STA_SAMPLE_LOG);
        return -1;
    }

    radar_uart_port_init();

    /* 等待wifi初始化完成 */
    while (wifi_is_wifi_inited() == 0) {
        (void)osDelay(10); /* 1: 等待100ms后判断状态 */
    }

    if (example_sta_function() != 0) {
        PRINT("%s::example_sta_function fail.\r\n", RADAR_STA_SAMPLE_LOG);
        return -1;
    }
    return 0;
}

static void sta_sample_entry(void)
{
    osThreadAttr_t attr;
    attr.name       = "radar_sample";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = WIFI_TASK_STACK_SIZE;
    attr.priority   = WIFI_TASK_PRIO;
    (void)osThreadNew((osThreadFunc_t)sta_sample_init, NULL, &attr);
}

/* Run the sta_sample_task. */
app_run(sta_sample_entry);
