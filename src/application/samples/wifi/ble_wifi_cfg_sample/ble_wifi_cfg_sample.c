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
#include "app_init.h"

#include "ble_wifi_cfg_scan.h"
#include "ble_wifi_cfg_adv.h"
#include "ble_wifi_cfg_server.h"
#include "ble_wifi_cfg_client.h"

#define WIFI_IFNAME_MAX_SIZE 16
#define WIFI_MAX_KEY_LEN 65
#define WIFI_MAX_SSID_LEN 33
#define WIFI_SCAN_AP_LIMIT 64
#define WIFI_MAC_LEN 6
#define WIFI_GET_IP_MAX_TIMES 100
#define WIFI_MAX_CONFIG_INFO_LEN 64
#define WIFI_CONFIG_INFO_SSID_LEN 32
#define WIFI_CONFIG_INFO_KEY_LEN 32
#define BGLE_WIFI_CFG_LOG "[BGLE_WIFI_DEBUG]"
#define WIFI_AP_LIST_MAX_NUM 10
/* 前2字节为上报类型和ap个数 */
#define WIFI_AP_LIST_PREFIX_LEN 2

#define WLAN_REASON_UNSPECIFIED 1
#define WLAN_REASON_PREV_AUTH_NOT_VALID 2
#define WLAN_REASON_DEAUTH_LEAVING 3
#define WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY 4
#define WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA 6
#define WLAN_REASON_MICHAEL_MIC_FAILURE 14
#define WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT 15
#define WLAN_REASON_GROUP_KEY_UPDATE_TIMEOUT 16
#define WLAN_DISCONN_BY_AP_BIT 15

#define WLAN_STATUS_CHALLENGE_FAIL 15

#define MAC_JOIN_RSP_TIMEOUT 5200
#define MAC_AUTH_RSP2_TIMEOUT 5201
#define MAC_AUTH_RSP4_TIMEOUT 5202
#define MAC_ASOC_RSP_TIMEOUT 5203
#define WLAN_DISASOC_MISC_LINKLOSS 5206
#define WIFI_NETWORK_NOT_FOUND_ERROR 5300
#define MAC_STATUS_MAX 7000


enum {
    CONFIG_DEMO_INIT = 0,           /* 0:初始态 */
    CONFIG_DEMO_WIFI_INIT,          /* 1:重新连接设备 待配网 */
    CONFIG_DEMO_WIFI_SCAN_DOING,    /* 2:扫描中 */
    CONFIG_DEMO_WIFI_SCAN_DONE,     /* 3:扫描完成 */
    CONFIG_DEMO_WIFI_CONNECT_DOING, /* 4:WiFi连接中 */
    CONFIG_DEMO_WIFI_CONNECT_DONE,  /* 5:Wifi连接完成 */
    CONFIG_DEMO_WIFI_DHCP_DONE,
} bgwc_state_enum;

enum {
    CFG_TYPE_WIFI_STATE = 1, /* WiFi关联状态 */
    CFG_TYPE_AP_LIST = 2     /* 扫描到的AP列表信息 */
} bgwc_cfg_type;

typedef enum {
    WIFI_ERRCODE_NONE = 0,
    WIFI_ERRCODE_SSID_NOT_FOUND,
    WIFI_ERRCODE_PWD_ERROR,
    WIFI_ERRCODE_DHCP_FAILED,
    WIFI_ERRCODE_BEACON_LOST,
    WIFI_ERRCODE_OTHERS
} bgwc_wifi_errcode;

typedef struct {
    char ssid[WIFI_MAX_SSID_LEN];
    int8_t rssi;
} bgwc_wifi_bss;

td_char g_data[WIFI_MAX_CONFIG_INFO_LEN] = {0};
static td_u8 g_bgwc_state = CONFIG_DEMO_INIT;
uint8_t g_wifi_cfg_info_flag = 0; /* 0:no info 1:save info */
uint8_t g_wifi_list_req_flag = 0; /* 0:recv req 1:no req */
int8_t g_errcode = WIFI_ERRCODE_NONE;

static int8_t get_wifi_errcode(void)
{
    return g_errcode;
}

uint8_t get_wifi_cfg_info_flag(void)
{
    return g_wifi_cfg_info_flag;
}

void set_wifi_cfg_info_flag(uint8_t flag)
{
    g_wifi_cfg_info_flag = flag;
}

uint8_t get_wifi_list_req_flag(void)
{
    return g_wifi_list_req_flag;
}

void set_wifi_list_req_flag(uint8_t flag)
{
    g_wifi_list_req_flag = flag;
}

void set_wifi_cfg_info(uint8_t *info, uint16_t info_len)
{
    set_wifi_cfg_info_flag(1);
    (void)memcpy_s(g_data, WIFI_MAX_CONFIG_INFO_LEN, info, info_len);
}

int bgwc_wifi_list_resp_send(uint16_t handle)
{
    set_wifi_list_req_flag(1);
    uint8_t result = 0x01;
    errcode_t ret = ble_wifi_cfg_server_send_report_by_handle(handle, (const uint8_t *)&result, sizeof(uint8_t));
    if (ret != ERRCODE_BT_SUCCESS) {
        PRINT("bgwc_wifi_list_resp_send fail, ret:%x.\n", ret);
    }
    return ret;
}

td_s32 example_get_match_network(wifi_sta_config_stru *expected_bss)
{
    td_s32 ret;
    td_u32 num = 64; /* 最大扫描到网络数量64 */
    td_char expected_ssid[WIFI_CONFIG_INFO_SSID_LEN] = {0};
    td_char key[WIFI_CONFIG_INFO_KEY_LEN] = {0}; /* 待连接的网络接入密码 */
    td_bool find_ap = TD_FALSE;
    td_u8 bss_index;
    /* 获取扫描结果 */
    td_u32 scan_len = sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
    if (result == NULL) {
        return -1;
    }
    memset_s(result, scan_len, 0, scan_len);
    ret = wifi_sta_get_scan_info(result, &num);
    if (ret != 0) {
        osal_kfree(result);
        return -1;
    }

    memcpy_s(expected_ssid, WIFI_CONFIG_INFO_SSID_LEN, g_data, WIFI_CONFIG_INFO_SSID_LEN);
    memcpy_s(key, WIFI_CONFIG_INFO_SSID_LEN, g_data + WIFI_CONFIG_INFO_SSID_LEN, WIFI_CONFIG_INFO_KEY_LEN);

    PRINT("%s expected_ssid :%s, key:%s\r\n", BGLE_WIFI_CFG_LOG, expected_ssid, key);

    /* 筛选扫描到的Wi-Fi网络，选择待连接的网络 */
    for (bss_index = 0; bss_index < num; bss_index++) {
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
    expected_bss->ip_type = 1;
    osal_kfree(result);
    return 0;
}

td_bool example_check_connect_status(td_void)
{
    td_u8 index;
    wifi_linked_info_stru wifi_status;
    /* 获取网络连接状态，共查询5次 */
    for (index = 0; index < 5; index++) {
        (td_void)osDelay(50); /* 每次间隔50 tick */
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

static td_void bgwc_get_ap_list_info(wifi_scan_info_stru *scan_ret, uint32_t real_ap_number, uint8_t *report_data)
{
    bgwc_wifi_bss bss = { 0 };
    uint8_t *bss_data = NULL;
    uint32_t count = 0;

    report_data[0] = CFG_TYPE_AP_LIST;
    bss_data = report_data + WIFI_AP_LIST_PREFIX_LEN;
    for (uint32_t idx = 0; idx < real_ap_number; idx++) {
        if (strlen(scan_ret[idx].ssid) == 0) {
            continue;
        }
        if (memcpy_s(bss.ssid, sizeof(bss.ssid), scan_ret[idx].ssid, sizeof(scan_ret[idx].ssid) - 1) != EOK) {
            return;
        }
        bss.rssi = (int8_t)scan_ret[idx].rssi;
        if (memcpy_s(bss_data, sizeof(bgwc_wifi_bss), &bss, sizeof(bss)) != EOK) {
            return;
        }
        PRINT("%d:ssid[%s]\trssi[%d] \t\r\n", count, bss.ssid, bss.rssi);
        bss_data += sizeof(bgwc_wifi_bss);
        count++;
        /* report max num: 10 */
        if (count >= WIFI_AP_LIST_MAX_NUM) {
            break;
        }
    }
    report_data[1] = (uint8_t)count;
}

static td_void bgwc_scan_state_changed(td_s32 state, td_s32 size)
{
    wifi_scan_info_stru *scan_ret = NULL;
    uint32_t real_ap_number = (uint32_t)size;
    uint8_t *report_data = NULL;
    uint32_t max_len;
    PRINT("%s bgwc_scan_state_changed enter.\n", BGLE_WIFI_CFG_LOG);
    UNUSED(state);

    g_bgwc_state = CONFIG_DEMO_WIFI_SCAN_DONE;
    if ((get_wifi_list_req_flag() == 0) || (size <= 0)) {
        return;
    }

    scan_ret = (wifi_scan_info_stru *)malloc(sizeof(wifi_scan_info_stru) * real_ap_number);
    if (scan_ret == NULL) {
        return;
    }
    memset_s(scan_ret, sizeof(wifi_scan_info_stru) * real_ap_number, 0, sizeof(wifi_scan_info_stru) * real_ap_number);
    if (wifi_sta_get_scan_info(scan_ret, &real_ap_number) != ERRCODE_SUCC) {
        free(scan_ret);
        return;
    }

    max_len = sizeof(bgwc_wifi_bss) * WIFI_AP_LIST_MAX_NUM + WIFI_AP_LIST_PREFIX_LEN;
    report_data = (uint8_t *)malloc(max_len);
    if (report_data == NULL) {
        free(scan_ret);
        return;
    }
    memset_s(report_data, max_len, 0, max_len);
    bgwc_get_ap_list_info(scan_ret, real_ap_number, report_data);
    ble_wifi_cfg_server_send_report_by_uuid((const uint8_t *)report_data,
        sizeof(bgwc_wifi_bss) * report_data[1] + WIFI_AP_LIST_PREFIX_LEN); /* 真实写入数据的长度 */
    /* 恢复初始值 */
    set_wifi_list_req_flag(0);
    free(scan_ret);
    free(report_data);
    return;
}

static void bgwc_wifi_reason_code(td_s32 reason_code, int8_t *err_code)
{
    int disconn_by_ap = (reason_code >> WLAN_DISCONN_BY_AP_BIT) & 1;

    reason_code = reason_code & ~(1 << WLAN_DISCONN_BY_AP_BIT);
    /* 密码错误由对端AP校验并返回错误码 */
    if (disconn_by_ap == 1) {
        switch (reason_code) {
            case 0:
                *err_code = WIFI_ERRCODE_NONE;
                break;
            case WLAN_REASON_PREV_AUTH_NOT_VALID:
            case WLAN_REASON_MICHAEL_MIC_FAILURE:
            case WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT:
            case WLAN_REASON_GROUP_KEY_UPDATE_TIMEOUT:
                *err_code = WIFI_ERRCODE_PWD_ERROR;
                break;
            default:
                *err_code = WIFI_ERRCODE_OTHERS;
                break;
        }
        return;
    }
    /* 自身主动断开场景, 错误码转换 */
    if (reason_code >= MAC_STATUS_MAX) {
        reason_code -= MAC_STATUS_MAX;
    }

    if (reason_code == WLAN_DISASOC_MISC_LINKLOSS) {
        *err_code = WIFI_ERRCODE_BEACON_LOST;
        return;
    } else if (reason_code == WIFI_NETWORK_NOT_FOUND_ERROR) {
        *err_code = WIFI_ERRCODE_SSID_NOT_FOUND;
        return;
    }
    switch (reason_code) {
        case WLAN_STATUS_CHALLENGE_FAIL:
            *err_code = WIFI_ERRCODE_PWD_ERROR;
            break;
        default:
            *err_code = WIFI_ERRCODE_OTHERS;
            break;
    }
    return;
}

static td_void bgwc_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(state);
    PRINT("%s bgwc_connection_changed enter.\n", BGLE_WIFI_CFG_LOG);

    g_errcode = WIFI_ERRCODE_NONE;
    if (info->conn_state == WIFI_DISCONNECTED) {
        bgwc_wifi_reason_code(reason_code, &g_errcode);
    }
    g_bgwc_state = CONFIG_DEMO_WIFI_CONNECT_DONE;
    return;
}

static td_void bgwc_softap_state_changed(td_s32 state)
{
    UNUSED(state);
    PRINT("%s bgwc_softap_state_changed enter.\n", BGLE_WIFI_CFG_LOG);
    return;
}

wifi_event_stru ble_wifi_cfg_event_cb = {
    .wifi_event_scan_state_changed = bgwc_scan_state_changed,
    .wifi_event_connection_changed = bgwc_connection_changed,
    .wifi_event_softap_state_changed = bgwc_softap_state_changed
};

int bgwc_wifi_connect(void)
{
    wifi_sta_config_stru expected_bss = { 0 }; /* 连接请求信息 */
    /* 获取待连接的网络 */
    if (example_get_match_network(&expected_bss) != 0) {
        PRINT("Do not find AP, try again !\r\n");
        g_errcode = WIFI_ERRCODE_SSID_NOT_FOUND;
        return -1;
    }

    /* 启动连接 */
    if (wifi_sta_connect(&expected_bss) != 0) {
        PRINT("STA connect fail.\r\n");
        g_errcode = WIFI_ERRCODE_OTHERS;
        return -1;
    }
    return 0;
}

static void bgwc_ble_start(void)
{
    errcode_t ret = ERRCODE_SUCC;
    /* 调用BLE起广播接口与注册回调接口 */
    ret |= ble_wifi_cfg_server_init();
    PRINT("%s Ble Init State:%d.\r\n", BGLE_WIFI_CFG_LOG, ret);
    ret |= ble_wifi_cfg_start_adv();
    PRINT("%s Ble Adv State:%d.\r\n", BGLE_WIFI_CFG_LOG, ret);
}

static int bgwc_wifi_start(void)
{
    g_bgwc_state = CONFIG_DEMO_WIFI_INIT;

    if (wifi_sta_enable() != 0) {
        PRINT("%s sta enbale fail !\r\n", BGLE_WIFI_CFG_LOG);
        return -1;
    }

    if (wifi_register_event_cb(&ble_wifi_cfg_event_cb) != 0) {
        PRINT("%s wifi_register_event_cb fail.\r\n", BGLE_WIFI_CFG_LOG);
        return -1;
    }
    return 0;
}

static void *ble_wifi_cfg_example_task(const char *arg)
{
    td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0"; /* 创建的STA接口名 */
    struct netif *netif_p = NULL;
    uint8_t result[WIFI_AP_LIST_PREFIX_LEN] = {CFG_TYPE_WIFI_STATE, 0};
    UNUSED(arg);
    (td_void)osDelay(200); /* 初始化等待200 tick */

    bgwc_ble_start();
    bgwc_wifi_start();

    while (1) {
        (td_void)osDelay(10); /* 等待10 tick */
        /* 兼容主动和被动配网方案 */
        if (get_wifi_cfg_info_flag() || get_wifi_list_req_flag()) {
            PRINT("wifi cfg flag:%d, wifi list flag:%d.\n", get_wifi_cfg_info_flag(), get_wifi_list_req_flag());
            /* 启动STA扫描 */
            if (wifi_sta_scan() != 0) {
                printf("wifi_sta_scan fail.\n");
                g_bgwc_state = CONFIG_DEMO_WIFI_INIT;
            } else {
                break;
            }
        }
    }

    while (1) {
        (td_void)osDelay(10); /* 等待10 tick */
        /* 检测是否下发配置信息 */
        if (get_wifi_cfg_info_flag() && (g_bgwc_state == CONFIG_DEMO_WIFI_SCAN_DONE)) {
            if (bgwc_wifi_connect() == 0) {
                g_bgwc_state = CONFIG_DEMO_WIFI_CONNECT_DOING;
            } else {
                printf("bgwc_wifi_connect fail.\n");
                g_bgwc_state = CONFIG_DEMO_WIFI_INIT;
                break;
            }
        }
        /* 完成WiFi连接 尝试DHCP */
        if (g_bgwc_state == CONFIG_DEMO_WIFI_CONNECT_DONE) {
            break;
        }
    }
    result[1] = get_wifi_errcode();
    if (result[1] != WIFI_ERRCODE_NONE) {
        PRINT("STA ASSOC Fail.\r\n");
        goto EXIT;
    }
    result[1] = WIFI_ERRCODE_DHCP_FAILED;
    PRINT("STA DHCP start.\r\n");
    /* DHCP获取IP地址 */
    netif_p = netifapi_netif_find(ifname);
    if (netif_p == NULL) {
        PRINT("not find %s.\r\n", ifname);
        goto EXIT;
    }
    if (netifapi_dhcp_start(netif_p) != 0) {
        PRINT("STA DHCP Fail.\r\n");
        goto EXIT;
    }

    for (td_char i = 0; i < WIFI_GET_IP_MAX_TIMES; i++) {
        (td_void)osDelay(10); /* 等待10 tick */
        if (ip_addr_isany(&(netif_p->ip_addr)) == 0) {
            PRINT("STA DHCP Succ.\r\n");
            result[1] = WIFI_ERRCODE_NONE;
            break;
        }
    }
EXIT:
    PRINT("result code:%d.\r\n", result[1]);
    ble_wifi_cfg_server_send_report_by_uuid((const uint8_t *)result, sizeof(result));
    return NULL;
}

#define BGWC_TASK_PRIO (osPriority_t)(13)
#define BGWC_TASK_STACK_SIZE 0x1000

static void bgle_wifi_cfg_entry(void)
{
    osal_kthread_lock();
    osal_task *g_wifi_cfg_task = osal_kthread_create((osal_kthread_handler)ble_wifi_cfg_example_task, 0,
        "bgle_wifi_cfg_task", BGWC_TASK_STACK_SIZE);
    if (g_wifi_cfg_task != NULL) {
        osal_kthread_set_priority(g_wifi_cfg_task, BGWC_TASK_PRIO);
        osal_kfree(g_wifi_cfg_task);
    }
    osal_kthread_unlock();
}

/* Run the ble_wifi_cfg_entry. */
app_run(bgle_wifi_cfg_entry);