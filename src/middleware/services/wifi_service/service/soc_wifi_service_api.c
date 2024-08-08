/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: soc_wifi_wpa_service_api.c
 */

#include "soc_osal.h"
#include "soc_wifi_api.h"
#include "service_wifi_api.h"
#include "wifi_device.h"
#include "wifi_p2p.h"
#include "soc_wifi_p2p_api.h"
#include "securec.h"
#include "wifi_hotspot.h"
#include "wifi_alg.h"
#include "service_event.h"
#include "lwip/netifapi.h"
#include "lwip/dns.h"
#include "mac_addr.h"
#include "tsensor.h"
#if defined(CONFIG_RST_SUPPORT) || defined(CONFIG_PLAT_DFR_SUPPORT)
#include "plat_service.h"
#ifdef _PRE_WLAN_FEATURE_CENTRALIZE
#ifdef BOARD_FPGA_WIFI
#include "abb_config.h"
#include "mpw0_poweron.h"
#endif // BOARD_FPGA_WIFI
#endif // _PRE_WLAN_FEATURE_CENTRALIZE
#endif // defined(CONFIG_RST_SUPPORT) || defined(CONFIG_PLAT_DFR_SUPPORT)

#define OAL_STATIC

#ifdef BUILD_UT
#define OSAL_STATIC
#else
#define OSAL_STATIC static
#endif

#define service_is_freqs(freqs) (((freqs) == 2412 || (freqs) == 2417 || (freqs) == 2422 || (freqs) == 2427 || \
                                  (freqs) == 2432 || (freqs) == 2437 || (freqs) == 2442 || (freqs) == 2447 || \
                                  (freqs) == 2452 || (freqs) == 2457 || (freqs) == 2462 || (freqs) == 2467 || \
                                  (freqs) == 2472 || (freqs) == 2484) ? (freqs) : 0)
#define service_char_to_upper(ch)   (((ch) >= 'a' && (ch) <= 'z') ? ((ch) - 0x20) : (ch))

#define service_check_hex(ch)     ((((ch) >= '0') && ((ch) <= '9')) || (((ch) >= 'A') && ((ch) <= 'F')) || \
                                  (((ch) >= 'a') && ((ch) <= 'f')) ? 1 : 0)

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WIFI_ACTIVE_VAP_MAX_NUM   2
#define WIFI_MAC_CONTRY_CODE_LEN 3
#define SERVICE_P2P_MAX_FIND_TIME 120
#define SERVICE_P2P_MIN_FIND_TIME 5
#define SERVICE_P2P_MAX_FIND_NUM  32
#define SERVICE_P2P_MAX_GC_NUM  4
#define SERVICE_P2P_DEFAULT_INTENT 7
#define SERVICE_P2P_MAX_INTENT 15
#define SERVICE_P2P_MAX_LISTEN_TIME 65535
#define SERVICE_BASE_FREQS 2412
#define SERVICE_MIN_PRIO 10
#define SERVICE_MIN_STACK 4096
#define SERVICE_FREQS_INTERVAL 5
#define SERVICE_LINKLOSS_PROBEREQ_RATIO_MIN 1
#define SERVICE_LINKLOSS_PROBEREQ_RATIO_MAX 10
#define SERVICE_LINKLOSS_THRESHOLD_MIN 50
#define SERVICE_LINKLOSS_THRESHOLD_MAX 500
#define WIFI_SENDPKT_MIN_LEN 10
#define WIFI_SENDPKT_MAX_LEN 1400
#define SERVICE_MAX_PERIOD_INTERVAL 65535
#define WIFI_CONN_SCAN_INTERVAL 5000
#define WIFI_USER_INPUT_KEY_LEN (WIFI_MAX_KEY_LEN + 2) /* 关联时临时保存用户输入的密码信息,长度 = 64 + \0 + 2个双引号 */

#define service_is_freqs(freqs) (((freqs) == 2412 || (freqs) == 2417 || (freqs) == 2422 || (freqs) == 2427 || \
                                  (freqs) == 2432 || (freqs) == 2437 || (freqs) == 2442 || (freqs) == 2447 || \
                                  (freqs) == 2452 || (freqs) == 2457 || (freqs) == 2462 || (freqs) == 2467 || \
                                  (freqs) == 2472 || (freqs) == 2484) ? (freqs) : 0)

/*****************************************************************************
 全局变量定义
*****************************************************************************/
softap_config_stru        g_softap_config = {0};
softap_config_advance_stru g_softap_advance_config = {100, 2, 86400, 1, 1, WIFI_MODE_11B_G_N_AX};

char                 g_sta_ifname[WIFI_IFNAME_MAX_SIZE + 1];
char                 g_hotspot_ifname[WIFI_IFNAME_MAX_SIZE + 1];
char                 g_p2p_ifname[WIFI_IFNAME_MAX_SIZE + 1];
int                  g_drop_disconnect = 0;
int                  g_sta_enble_flag = 0;
int                  g_softap_enble_flag = 0;
int                  g_p2p_enable_flag = 0;
int                  g_softap_hidden = 1;
dl_list              g_api_cb_node;
int                  g_api_cb_init = 0;
int                  g_p2p_isgo = 0;
#ifdef _PRE_WLAN_FEATURE_CSI
int                  g_csi_enable_flag = 0;
#endif
/* 上一次扫描上报的时间 */
static uint64_t g_sta_last_scan_time_stamp_ms = 0;
/* 这是否是一个由关联触发的扫描 */
OAL_STATIC int g_sta_conn_req_flag = 0;
/* 关联请求信息 */
static wifi_sta_config_stru g_sta_conn_config = {0};
#if defined(LWIP_IPV4) && defined(LWIP_IPV6) && (LWIP_IPV4) && (LWIP_IPV6)
__attribute__((weak)) err_t netifapi_netif_add_ip6_address(struct netif *netif, ip_addr_t *ipaddr);
#endif
static errcode_t wifi_sta_connect_part2(void);
/*****************************************************************************
 函数实现
*****************************************************************************/
static uint32_t service_check_api_cb(uint32_t line_num)
{
    if (g_api_cb_init == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:cd is not init", line_num);
        return ERRCODE_FAIL;
    }
    if (list_empty(&g_api_cb_node) == 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:cb list is null", line_num);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

OSAL_STATIC void send_broadcast_connected_change_for_sta(int state, const wifi_linked_info_stru *connect_para,
    int32_t reason_code)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_connection_changed != NULL) {
            node->service_cb.wifi_event_connection_changed(state, connect_para, reason_code);
        }
    }
}

OSAL_STATIC void send_broadcast_sta_connected_for_ap(const wifi_sta_info_stru *sta_info)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_softap_sta_join != NULL) {
            node->service_cb.wifi_event_softap_sta_join(sta_info);
        }
    }
}

OSAL_STATIC void send_broadcast_sta_leave_for_ap(const wifi_sta_info_stru *sta_info)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_softap_sta_leave != NULL) {
            node->service_cb.wifi_event_softap_sta_leave(sta_info);
        }
    }
}

#ifdef CONFIG_P2P_SUPPORT
static void send_broadcast_sta_leave_for_p2p(const p2p_client_info_stru *gc_info)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_p2p_go_connection_changed != NULL) {
            node->service_cb.wifi_event_p2p_go_connection_changed(WIFI_STATE_NOT_AVALIABLE, gc_info);
        }
    }
}

static void send_broadcast_sta_connected_for_p2p(const p2p_client_info_stru *gc_info)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_p2p_go_connection_changed != NULL) {
            node->service_cb.wifi_event_p2p_go_connection_changed(WIFI_STATE_AVALIABLE, gc_info);
        }
    }
}
#endif /* CONFIG_P2P_SUPPORT */

int32_t wifi_is_need_psk(wifi_security_enum security_type)
{
    if ((security_type != WIFI_SEC_TYPE_OPEN) &&
        (security_type != WIFI_SEC_TYPE_OWE)) {
        return 1;
    }
    return 0;
}

OSAL_STATIC uint64_t wifi_get_timestamp_ms(void)
{
    osal_timeval tv;
    osal_gettimeofday(&tv);
    return (uint64_t)(((int64_t)tv.tv_sec) * 1000000 + (int64_t)tv.tv_usec) >> 10;  /* 秒与微秒的转换需乘1000000 */
}

OSAL_STATIC void send_broadcast_scan_done(int state, const event_wifi_scan_done *wifi_scan_done)
{
    g_sta_last_scan_time_stamp_ms = wifi_get_timestamp_ms();
    /* 如果当前是一个由关联触发的扫描,走完下半段 */
    if (g_sta_conn_req_flag == 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:scan triggered by connect!", __LINE__);
        wifi_sta_connect_part2();
        return;
    }
    if (g_api_cb_init == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:cb is not init", __LINE__);
        return;
    }
    if (list_empty(&g_api_cb_node) == 1 ||
        memcmp(wifi_scan_done->ifname, g_sta_ifname, strlen(wifi_scan_done->ifname)) != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:cb list is null", __LINE__);
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_scan_state_changed != NULL) {
            node->service_cb.wifi_event_scan_state_changed(state, wifi_scan_done->bss_num);
        }
    }
}

#ifdef CONFIG_P2P_SUPPORT
static void send_broadcast_connected_change_for_p2p(int state, const p2p_status_info_stru *p2p_status)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_p2p_gc_connection_changed != NULL) {
            node->service_cb.wifi_event_p2p_gc_connection_changed(state, p2p_status);
        }
    }
}
#endif /* CONFIG_P2P_SUPPORT */

OSAL_STATIC void send_broadcast_ap_state_change_for_ap(int state)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_softap_state_changed != NULL) {
            node->service_cb.wifi_event_softap_state_changed(state);
        }
    }
}

#ifdef CONFIG_P2P_SUPPORT
static void send_broadcast_ap_state_change_for_p2p(int state)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_p2p_go_start != NULL) {
            node->service_cb.wifi_event_p2p_go_start(state);
        }
    }
}

static void send_broadcast_p2p_recive_req(const unsigned char *p2p_dev_addr, char wps_mode)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_p2p_receive_connect != NULL) {
            node->service_cb.wifi_event_p2p_receive_connect(p2p_dev_addr, wps_mode);
        }
    }
}

static void send_broadcast_p2p_go_neg_success(const event_p2p_go_neg_success *go_neg_success)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_p2p_go_neg_result != NULL) {
            node->service_cb.wifi_event_p2p_go_neg_result(WIFI_STATE_AVALIABLE, go_neg_success->role_go);
        }
    }
}

static void send_broadcast_p2p_go_neg_fail(const event_p2p_go_neg_failure *p2p_go_neg_failure)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_p2p_go_neg_result != NULL) {
            node->service_cb.wifi_event_p2p_go_neg_result(WIFI_STATE_NOT_AVALIABLE, WIFI_STATE_NOT_AVALIABLE);
        }
    }
}
#endif /* CONFIG_P2P_SUPPORT */

OSAL_STATIC void send_broadcast_connected(const event_wifi_connected *wifi_connected)
{
    wifi_linked_info_stru connect_para = {0};
#ifdef CONFIG_P2P_SUPPORT
    p2p_status_info_stru p2p_status = {0};
#endif /* CONFIG_P2P_SUPPORT */
    if (memcmp(wifi_connected->ifname, g_sta_ifname, strlen(wifi_connected->ifname)) == 0) {
        wifi_sta_get_ap_info(&connect_para);
        send_broadcast_connected_change_for_sta(WIFI_STATE_AVALIABLE, &connect_para, 0);
#ifdef CONFIG_P2P_SUPPORT
    } else if (memcmp(wifi_connected->ifname, g_p2p_ifname, strlen(wifi_connected->ifname)) == 0) {
        wifi_p2p_get_connect_info(&p2p_status);
        send_broadcast_connected_change_for_p2p(WIFI_STATE_AVALIABLE, &p2p_status);
#endif /* CONFIG_P2P_SUPPORT */
    }
}

/* 转换reason code, 如果是被对端断开的, bit 15置为1 */
OSAL_STATIC int32_t wifi_convert_disconnect_reason_code(const event_wifi_disconnected *wifi_disconnected)
{
    int32_t reason_code = wifi_disconnected->reason_code;

    if ((wifi_disconnected->locally_generated == 1) || (reason_code >= WIFI_MAC_REPORT_DISCONNECT_OFFSET)) {
        return reason_code;
    }
    /* 对端主动断开的, bit 15置为1 */
    reason_code |= (int32_t)(1 << 15);
    return reason_code;
}

OSAL_STATIC void send_broadcast_disconnected(const event_wifi_disconnected *wifi_disconnected)
{
    wifi_linked_info_stru disconnect_para = {0};
#ifdef CONFIG_P2P_SUPPORT
    p2p_status_info_stru p2p_status = {0};
#endif /* CONFIG_P2P_SUPPORT */
    if (g_drop_disconnect == 1) {
        g_drop_disconnect = 0;
        return;
    }

    if (memcmp(wifi_disconnected->ifname, g_sta_ifname, strlen(wifi_disconnected->ifname)) == 0) {
        if (memcpy_s(disconnect_para.bssid, sizeof(disconnect_para.bssid),
            wifi_disconnected->bssid, sizeof(wifi_disconnected->bssid)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return;
        }
        disconnect_para.conn_state = WIFI_DISCONNECTED;
        disconnect_para.wpa_state = wifi_disconnected->wpa_state;
        send_broadcast_connected_change_for_sta(WIFI_STATE_NOT_AVALIABLE, &disconnect_para,
            wifi_convert_disconnect_reason_code(wifi_disconnected));
#ifdef CONFIG_P2P_SUPPORT
    } else if (memcmp(wifi_disconnected->ifname, g_p2p_ifname, strlen(wifi_disconnected->ifname)) == 0) {
        if (memcpy_s(p2p_status.group_bssid, sizeof(p2p_status.group_bssid),
            wifi_disconnected->bssid, sizeof(wifi_disconnected->bssid)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return;
        }
        send_broadcast_connected_change_for_p2p(WIFI_STATE_NOT_AVALIABLE, &p2p_status);
#endif /* CONFIG_P2P_SUPPORT */
    }
}

OSAL_STATIC void send_broadcast_ap_state_change(const char *ifname, int state)
{
    if (memcmp(ifname, g_hotspot_ifname, strlen(ifname)) == 0) {
        g_softap_enble_flag = state;
        send_broadcast_ap_state_change_for_ap(state);
#ifdef CONFIG_P2P_SUPPORT
    } else if (memcmp(ifname, g_p2p_ifname, strlen(ifname)) == 0) {
        g_p2p_isgo = state;
        send_broadcast_ap_state_change_for_p2p(state);
#endif /* CONFIG_P2P_SUPPORT */
    }
}

OSAL_STATIC void send_broadcast_sta_connected(const event_ap_sta_connected *ap_sta_connected)
{
    wifi_sta_info_stru sta_info[WIFI_DEFAULT_MAX_NUM_STA] = {0};
    int sta_num = WIFI_DEFAULT_MAX_NUM_STA;
    int i;
#ifdef CONFIG_P2P_SUPPORT
    p2p_client_info_stru gc_info[SERVICE_P2P_MAX_GC_NUM] = {0};
    int p2p_num = SERVICE_P2P_MAX_GC_NUM;
#endif /* CONFIG_P2P_SUPPORT */
    if (memcmp(ap_sta_connected->ifname, g_hotspot_ifname, strlen(ap_sta_connected->ifname)) == 0) {
        wifi_softap_get_sta_list(sta_info, &sta_num);
        for (i = 0; i < sta_num; i++) {
            if ((memcmp(sta_info[i].mac_addr, ap_sta_connected->addr, sizeof(ap_sta_connected->addr))) == 0) {
                send_broadcast_sta_connected_for_ap(&sta_info[i]);
                return;
            }
        }
#ifdef CONFIG_P2P_SUPPORT
    } else if (memcmp(ap_sta_connected->ifname, g_p2p_ifname, strlen(ap_sta_connected->ifname)) == 0) {
        wifi_p2p_go_get_gc_info(gc_info, &p2p_num);
        for (i = 0; i < p2p_num; i++) {
            if ((memcmp(gc_info[i].gc_bssid, ap_sta_connected->addr, sizeof(ap_sta_connected->addr))) == 0) {
                send_broadcast_sta_connected_for_p2p(&gc_info[i]);
                return;
            }
        }
#endif /* CONFIG_P2P_SUPPORT */
    }
}

OSAL_STATIC void send_broadcast_sta_leave(const event_ap_sta_disconnected *ap_sta_disconnected)
{
    wifi_sta_info_stru sta_info = {0};
#ifdef CONFIG_P2P_SUPPORT
    p2p_client_info_stru gc_info = {0};
#endif /* CONFIG_P2P_SUPPORT */
    if (memcmp(ap_sta_disconnected->ifname, g_hotspot_ifname, strlen(ap_sta_disconnected->ifname)) == 0) {
        if ((memcpy_s(sta_info.mac_addr, sizeof(sta_info.mac_addr), ap_sta_disconnected->addr,
            sizeof(ap_sta_disconnected->addr))) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return;
        }
        send_broadcast_sta_leave_for_ap(&sta_info);
#ifdef CONFIG_P2P_SUPPORT
    } else if (memcmp(ap_sta_disconnected->ifname, g_p2p_ifname, strlen(ap_sta_disconnected->ifname)) == 0) {
        if ((memcpy_s(gc_info.gc_bssid, sizeof(gc_info.gc_bssid), ap_sta_disconnected->addr,
            sizeof(ap_sta_disconnected->addr))) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return;
        }
        send_broadcast_sta_leave_for_p2p(&gc_info);
#endif /* CONFIG_P2P_SUPPORT */
    }
}

#ifdef CONFIG_P2P_SUPPORT
OSAL_STATIC void send_broadcast_p2p_go_neg_req(const event_p2p_go_neg_request *p2p_go_neg_request)
{
    char wps_mode = WIFI_WPS_BUTT;
    if (p2p_go_neg_request->dev_passwd_id == 0x1 || p2p_go_neg_request->dev_passwd_id == 0x5) {
        wps_mode = WIFI_WPS_PIN;
    } else if (p2p_go_neg_request->dev_passwd_id == 0x4) {
        wps_mode = WIFI_WPS_PBC;
    }
    send_broadcast_p2p_recive_req(p2p_go_neg_request->addr, wps_mode);
}
#endif /* CONFIG_P2P_SUPPORT */

#ifdef CONFIG_WPS_SUPPORT
OSAL_STATIC void send_broadcast_wps_result(int state, const char *ifname)
{
    wifi_if_type_enum type = IFTYPE_STA;
    ext_wifi_p2p_status_info p2p_status;
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    if (memcmp(ifname, g_hotspot_ifname, strlen(ifname)) == 0) {
        type = IFTYPE_AP;
    } else if (memcmp(ifname, g_sta_ifname, strlen(ifname)) == 0) {
        type = IFTYPE_STA;
#ifdef CONFIG_P2P_SUPPORT
    } else if (memcmp(ifname, g_p2p_ifname, strlen(ifname)) == 0) {
        uapi_wifi_p2p_status(&p2p_status);
        if ((p2p_status.p2p_state) == EXT_WIFI_P2P_MODE_GC) {
            type = IFTYPE_P2P_CLIENT;
        } else if ((p2p_status.p2p_state) == EXT_WIFI_P2P_MODE_GO) {
            type = IFTYPE_P2P_GO;
        } else if ((p2p_status.p2p_state) == EXT_WIFI_P2P_DEVICE_ONLY) {
            type = IFTYPE_P2P_DEVICE;
        }
#endif
    }
    if ((type == IFTYPE_STA || type == IFTYPE_P2P_CLIENT) && state == WIFI_STATE_AVALIABLE) {
        g_drop_disconnect = 1;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_wps_result != NULL) {
            node->service_cb.wifi_event_wps_result(state, type);
        }
    }
}
#endif

#ifdef CONFIG_P2P_SUPPORT
static void send_broadcast_p2p_invitation_recive(const event_p2p_invite_recieve *p2p_invite_recieve)
{
    if (p2p_invite_recieve->unknow_network != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:network is saved", __LINE__);
        return;
    }
    send_broadcast_p2p_recive_req(p2p_invite_recieve->go_dev_addr, WIFI_WPS_PBC);
}

static void send_broadcast_p2p_invitation_result(const event_p2p_invite_result *p2p_invite_result)
{
    if (service_check_api_cb(__LINE__) == ERRCODE_FAIL) {
        return;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && node->service_cb.wifi_event_p2p_invitation_result != NULL) {
            node->service_cb.wifi_event_p2p_invitation_result(p2p_invite_result->status == 1 ?
                WIFI_STATE_AVALIABLE : WIFI_STATE_NOT_AVALIABLE);
        }
    }
}

static void p2p_recive_pbc_req(void)
{
    int ret;
    service_error_log2(SERVICE_ERROR, "Srv:%d:[%d][1:is_go 0:not_go]", __LINE__, g_p2p_isgo);
    if (g_p2p_isgo == WIFI_STATE_AVALIABLE) {
        ret = uapi_wifi_p2p_stop_find();
        if (ret != EXT_WIFI_OK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:stop p2p find fail", __LINE__);
            return;
        }
        ret = uapi_wifi_p2p_wps_pbc();
        if (ret != EXT_WIFI_OK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:start wps pbc fail", __LINE__);
            return;
        }
    }
}
#endif /* CONFIG_P2P_SUPPORT */

OSAL_STATIC void wpa_event_cb_handle(const ext_wifi_event *event)
{
    if (event == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:input event is null", __LINE__);
        return;
    }
    service_error_log2(SERVICE_ERROR, "Srv:%d:recive event = %d", __LINE__, event->event);
    switch (event->event) {
        case EXT_WIFI_EVT_SCAN_DONE:
            send_broadcast_scan_done(WIFI_STATE_AVALIABLE, &event->info.wifi_scan_done);
            break;
        case EXT_WIFI_EVT_CONNECTED:
            send_broadcast_connected(&event->info.wifi_connected);
            break;
        case EXT_WIFI_EVT_DISCONNECTED:
            send_broadcast_disconnected(&event->info.wifi_disconnected);
            break;
        case EXT_WIFI_EVT_AP_START:
            send_broadcast_ap_state_change(event->info.wifi_ap_start.ifname, WIFI_STATE_AVALIABLE);
            break;
        case EXT_WIFI_EVT_AP_DISABLE:
            send_broadcast_ap_state_change(event->info.wifi_ap_fail.ifname, WIFI_STATE_NOT_AVALIABLE);
            break;
        case EXT_WIFI_EVT_STA_CONNECTED:
            send_broadcast_sta_connected(&event->info.ap_sta_connected);
            break;
        case EXT_WIFI_EVT_STA_DISCONNECTED:
            send_broadcast_sta_leave(&event->info.ap_sta_disconnected);
            break;
#ifdef CONFIG_P2P_SUPPORT
        case EXT_WIFI_EVT_P2P_GO_NEG_REQUEST:
            send_broadcast_p2p_go_neg_req(&event->info.p2p_go_neg_request);
            break;
        case EXT_WIFI_EVT_P2P_GO_NEG_SUCCESS:
            send_broadcast_p2p_go_neg_success(&event->info.p2p_go_neg_success);
            break;
        case EXT_WIFI_EVT_P2P_GO_NEG_FAILURE:
            send_broadcast_p2p_go_neg_fail(&event->info.p2p_go_neg_failure);
            break;
#endif /* CONFIG_P2P_SUPPORT */
#ifdef CONFIG_WPS_SUPPORT
        case EXT_WIFI_EVT_WPS_SUCCESS:
            send_broadcast_wps_result(WIFI_STATE_AVALIABLE, event->info.wifi_wps_success.ifname);
            break;
        case EXT_WIFI_EVT_WPS_TIMEOUT:
            send_broadcast_wps_result(WIFI_STATE_NOT_AVALIABLE, event->info.wifi_wps_timeout.ifname);
            break;
        case EXT_WIFI_EVT_WPS_FAIL:
            send_broadcast_wps_result(WIFI_STATE_NOT_AVALIABLE, event->info.wifi_wps_fail.ifname);
            break;
#endif /* CONFIG_WPS_SUPPORT */
#ifdef CONFIG_P2P_SUPPORT
        case EXT_WIFI_EVT_P2P_INVITATION_RECIEVE:
            send_broadcast_p2p_invitation_recive(&event->info.p2p_invite_recieve);
            break;
        case EXT_WIFI_EVT_P2P_INVITATION_RESULT:
            send_broadcast_p2p_invitation_result(&event->info.p2p_invite_result);
            break;
        case EXT_WIFI_EVT_P2P_PROV_DISC_PBC_REQ:
            p2p_recive_pbc_req();
            break;
#endif /* CONFIG_P2P_SUPPORT */
        default:
            service_error_log1(SERVICE_ERROR, "Srv:%d:event is unknown", __LINE__);
            return;
    }
}

static int register_callback(void)
{
    int ret;
    ret = uapi_wifi_config_callback(0, SERVICE_MIN_PRIO, SERVICE_MIN_STACK);
    if (ret != 0) {
        return ret;
    }
    ret = uapi_wifi_register_event_callback(wpa_event_cb_handle);
    return ret;
}

static service_event_cb *find_wifi_event(const wifi_event_stru *event)
{
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && memcmp(&node->service_cb, event, sizeof(wifi_event_stru)) == 0) {
            return node;
        }
    }
    return NULL;
}

errcode_t wifi_register_event_cb(const wifi_event_stru *event)
{
    if (event == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:input is inlegal", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (g_api_cb_init == 0) {
        list_init(&g_api_cb_node);
        g_api_cb_init = 1;
    }
    if (find_wifi_event(event) != NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:this cb is already register", __LINE__);
        return ERRCODE_SUCC;
    }
    service_event_cb *node = (service_event_cb *)malloc(sizeof(service_event_cb));
    if (node == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:malloc fail", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
    if (memcpy_s(&node->service_cb, sizeof(wifi_event_stru), event, sizeof(wifi_event_stru)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        free(node);
        return ERROR_WIFI_UNKNOWN;
    }

    list_tail_insert(&g_api_cb_node, &node->node);
    return ERRCODE_SUCC;
}

errcode_t wifi_unregister_event_cb(const wifi_event_stru *event)
{
    if (event == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:input is inlegal", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }
    if (g_api_cb_init == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:cb is not init", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
    service_event_cb *node = NULL;
    dl_list_for_each_entry(node, &g_api_cb_node, service_event_cb, node) {
        if (node != NULL && memcmp(&node->service_cb, event, sizeof(wifi_event_stru)) == 0) {
            list_delete_node(&node->node);
            free(node);
            return ERRCODE_SUCC;
        }
    }
    service_error_log1(SERVICE_ERROR, "Srv:%d:no such cb", __LINE__);
    return ERROR_WIFI_INVALID_ARGS;
}

OSAL_STATIC wifi_return_code service_addr_precheck(const unsigned char *addr)
{
    // 对0、1、2、3、4、5每一字节进行非0判断
    if ((addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]) == 0) {
        return ERROR_WIFI_INVALID_ARGS;
    }
    // 对0、1、2、3、4、5每一字节进行非FF判断
    if ((addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xFF) {
        return ERROR_WIFI_INVALID_ARGS;
    }

    return ERRCODE_SUCC;
}

static wifi_return_code service_wifi_and_sta_check(void)
{
    if (wifi_is_sta_enabled() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta is not enabled", __LINE__);
        return ERROR_WIFI_IFACE_INVALID;
    }
    return ERRCODE_SUCC;
}

static wifi_return_code service_wifi_and_ap_check(void)
{
    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERROR_WIFI_NOT_STARTED;
    }

    if (g_softap_enble_flag != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:ap is not enabled", __LINE__);
        return ERROR_WIFI_IFACE_INVALID;
    }
    return ERRCODE_SUCC;
}

static wifi_return_code service_check_wifi_device_config(const wifi_sta_config_stru *config)
{
    if (config == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: config is NULL", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (config->ip_type >= UNKNOWN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: ip_type is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (((strlen(config->ssid) == 0) || (strlen(config->ssid) > EXT_WIFI_MAX_SSID_LEN)) &&
        (service_addr_precheck(config->bssid) != ERRCODE_SUCC)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: ssid and bssid are invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (config->security_type == WIFI_SEC_TYPE_INVALID ||
#ifndef CONFIG_OWE
        config->security_type == WIFI_SEC_TYPE_OWE ||
#endif /* CONFIG_OWE */
        config->security_type == WIFI_SEC_TYPE_WAPI_CERT) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: securitytype is not supported", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if ((wifi_is_need_psk(config->security_type) == 1) && (strlen(config->pre_shared_key) == 0 ||
        strlen(config->pre_shared_key) > EXT_WIFI_MAX_KEY_LEN)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: securitytype is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (config->wifi_psk_type != 0 && config->wifi_psk_type != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:wifi_psk_type is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    return ERRCODE_SUCC;
}

static wifi_return_code service_hex_to_asic(const unsigned char* hex, unsigned char* asic, int hex_len)
{
    int j = 0;
    int check_hex1, check_hex2;
    unsigned char high_byte, low_byte;

    for (int i = 0; i < hex_len; i += 2) {   // 2个字节合为一个asic码
        check_hex1 = service_check_hex(hex[i]);
        check_hex2 = service_check_hex(hex[i + 1]);
        if (check_hex1 != 1 || check_hex2 != 1) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:hex is invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
        high_byte = service_char_to_upper(hex[i]);
        low_byte = service_char_to_upper(hex[i + 1]);

        if (high_byte > 0x39) {
            high_byte -= 0x37;           // 这里判断的是将A-F转换为0XA到0XF ,查看ASCII 表，可以看到从字符转换到hex之间的 差值
        } else {
            high_byte -= 0x30;           // 这里是将0-9转换为0x0-0x9  同样查看ASCII表，可以看到从字符准换到hex之间的差值。
        }

        if (low_byte > 0x39) {
            low_byte -= 0x37;
        } else {
            low_byte -= 0x30;
        }

        asic[j] = (high_byte << 4) | low_byte;  // 右移4位
        j++;
    }

    asic[hex_len / 2] = '\0';               // asic最后的长度为hex_len/2
    return ERRCODE_SUCC;
}

static wifi_return_code service_check_wep_key(const char *pre_shared_key, int8_t wifi_psk_type, char *wep_key)
{
    uint32_t key_len = strlen(pre_shared_key);
    int32_t  ret;
    if (wifi_psk_type == 0) {
        // WEP的5byte和13byte固定是ascii码模式
        if (key_len != 5 && key_len != 13) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: wep key invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
    } else if (wifi_psk_type == 1) {
        // 10byte和26byte固定是HEX模式
        if (key_len != 10 && key_len != 26) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: wep key invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
        ret = service_hex_to_asic(pre_shared_key, wep_key, (int32_t)key_len);
        if (ret != ERRCODE_SUCC) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:service_hex_to_asic failed", __LINE__);
            return ret;
        }
    }
    return ERRCODE_SUCC;
}

static wifi_return_code service_check_wpa_key(const char *pre_shared_key, int8_t wifi_psk_type)
{
    unsigned long  key_len = strlen(pre_shared_key);
    if (wifi_psk_type == 0) {
        // wpa、wpa2、wpa/wpa2、wpa2/wpa3个人级认证,8~63byte默认是ASCII
        if (key_len < 8 || key_len > 63) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: wpa key invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
    } else if (wifi_psk_type == 1) {
        // wpa、wpa2、wpa/wpa2、wpa2/wpa3个人级认证, 64byte默认是HEX
        if (key_len != 64) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: wpa key invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
    }
    return ERRCODE_SUCC;
}

static wifi_return_code service_check_sae_key(const char *pre_shared_key, int8_t wifi_psk_type)
{
    unsigned long  key_len = strlen(pre_shared_key);
    if (wifi_psk_type == 0) {
        // SAE个人级认证：8~64byte默认是ASCII
        if (key_len < 8 || key_len > 64) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: sae key invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
    } else if (wifi_psk_type == 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa3 does not support hex", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }
    return ERRCODE_SUCC;
}

static wifi_return_code service_check_wapi_key(const char *pre_shared_key, int8_t wifi_psk_type)
{
    unsigned long  key_len = strlen(pre_shared_key);
    if (wifi_psk_type == 0) {
        if (key_len < 8 || key_len > 64) {  /* wapi个人级认证：ASCII默认长度为8-64byte */
            service_error_log1(SERVICE_ERROR, "Srv:%d: wapi key invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
    } else if (wifi_psk_type == 1) {
        if (key_len < 8 || key_len > 32) {  /* wapi个人级认证：HEX默认长度为8-32byte */
            service_error_log1(SERVICE_ERROR, "Srv:%d: wapi key invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
    }
    return ERRCODE_SUCC;
}

static wifi_return_code service_check_pre_shared_key(const char *pre_shared_key, int security_type,
    int8_t wifi_psk_type, char *wep_key)
{
    int ret;

    if (security_type == WIFI_SEC_TYPE_WEP || security_type == WIFI_SEC_TYPE_WEP_OPEN) {
        ret = service_check_wep_key(pre_shared_key, wifi_psk_type, wep_key);
        if (ret != ERRCODE_SUCC) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:wep key check failed", __LINE__);
            return ret;
        }
    }

    if (security_type == WIFI_SEC_TYPE_WPA2PSK || security_type == WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX ||
        security_type == WIFI_SEC_TYPE_WPAPSK) {
        ret = service_check_wpa_key(pre_shared_key, wifi_psk_type);
        if (ret != ERRCODE_SUCC) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:wpa key check failed", __LINE__);
            return ret;
        }
    }

    if (security_type == WIFI_SEC_TYPE_SAE || security_type == WIFI_SEC_TYPE_WPA3_WPA2_PSK_MIX) {
        ret = service_check_sae_key(pre_shared_key, wifi_psk_type);
        if (ret != ERRCODE_SUCC) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:sae key check failed", __LINE__);
            return ret;
        }
    }

    if (security_type == WIFI_SEC_TYPE_WAPI_PSK) {
        ret = service_check_wapi_key(pre_shared_key, wifi_psk_type);
        if (ret != ERRCODE_SUCC) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:wapi key check failed", __LINE__);
            return ret;
        }
    }
    return ERRCODE_SUCC;
}

static wifi_return_code service_set_ipv4(const wifi_sta_config_stru *config)
{
    ip4_addr_t ipaddr = {0};
    ip4_addr_t netmask = {0};
    ip4_addr_t gw = {0};
    struct netif *netif = netifapi_netif_find_by_name(g_sta_ifname);
    if (netif == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:find_by_name failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    if (config->static_ip.ip_address == 0 || config->static_ip.netmask == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:ipv4 is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if ((memcpy_s(&ipaddr, sizeof(ip4_addr_t), &config->static_ip.ip_address, sizeof(unsigned int))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    if ((memcpy_s(&netmask, sizeof(ip4_addr_t), &config->static_ip.netmask, sizeof(unsigned int))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    if ((memcpy_s(&gw, sizeof(ip4_addr_t), &config->static_ip.gateway, sizeof(unsigned int))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    if (netifapi_netif_set_addr(netif, &ipaddr, &netmask, &gw) != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_addr failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
    return ERRCODE_SUCC;
}

static wifi_return_code service_set_ipv6(const wifi_sta_config_stru *config)
{
#if defined(LWIP_IPV4) && defined(LWIP_IPV6) && (LWIP_IPV4) && (LWIP_IPV6)
    ip_addr_t ipaddr = {0};

    if (strlen(config->static_ipv6.ipv6_address) == 0 ||
        strlen(config->static_ipv6.ipv6_address) > WIFI_IPV6_ADDR_LEN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:ipv6 is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    struct netif *netif = netifapi_netif_find_by_name(g_sta_ifname);
    if (netif == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:find_by_name failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    if ((memcpy_s(ipaddr.u_addr.ip6.addr, sizeof(ipaddr.u_addr.ip6.addr),
        config->static_ipv6.ipv6_address, WIFI_IPV6_ADDR_LEN)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    ipaddr.type = IPADDR_TYPE_V6;
    if (netifapi_netif_add_ip6_address(netif, &ipaddr) != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_addr failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
#endif
    return ERRCODE_SUCC;
}

static wifi_return_code service_set_ipv4_dns(const wifi_sta_config_stru *config)
{
#if defined(LWIP_IPV4) && defined(LWIP_IPV6) && (LWIP_IPV4) && (LWIP_IPV6)
    unsigned char   nums;
    if (config->static_ip.dns_servers[0] != 0 && config->static_ip.dns_servers[1] != 0) {
        nums = WIFI_MAX_DNS_NUM;
    } else if (config->static_ip.dns_servers[0] != 0) {
        nums = 1;
    } else {
        return ERRCODE_SUCC;
    }

    if (nums == 1) {
        ip_addr_t dnsserver;
        dnsserver.type = IPADDR_TYPE_V4;

        if ((memcpy_s(&dnsserver.u_addr.ip4, sizeof(ip4_addr_t),
            config->static_ip.dns_servers, sizeof(unsigned int))) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }

        if (lwip_dns_setserver(nums, &dnsserver) != 0) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:dns_setserver failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    } else {
        ip_addr_t dnsserver[WIFI_MAX_DNS_NUM];
        dnsserver[0].type = IPADDR_TYPE_V4;
        dnsserver[1].type = IPADDR_TYPE_V4;

        if ((memcpy_s(&dnsserver[0].u_addr.ip4, sizeof(ip4_addr_t),
            config->static_ip.dns_servers, sizeof(unsigned int))) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }

        if ((memcpy_s(&dnsserver[1].u_addr.ip4, sizeof(ip4_addr_t),
            &config->static_ip.dns_servers[1], sizeof(unsigned int))) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }

        if (lwip_dns_setserver(nums, dnsserver) != 0) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:dns_setserver_v4 failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    }
#endif
    return ERRCODE_SUCC;
}

static wifi_return_code service_set_ipv6_dns(const wifi_sta_config_stru *config)
{
#if defined(LWIP_IPV4) && defined(LWIP_IPV6) && (LWIP_IPV4) && (LWIP_IPV6)
    unsigned char   nums;
    if (strlen(config->static_ipv6.ipv6_dns_servers[0]) != 0 && strlen(config->static_ipv6.ipv6_dns_servers[1]) != 0) {
        nums = WIFI_MAX_DNS_NUM;
    } else if (strlen(config->static_ipv6.ipv6_dns_servers[0]) != 0) {
        nums = 1;
    } else {
        return ERRCODE_SUCC;
    }

    if (nums == 1) {
        ip_addr_t dnsserver;
        dnsserver.type = IPADDR_TYPE_V6;

        if ((memcpy_s(&dnsserver.u_addr.ip6, sizeof(ip6_addr_t),
            config->static_ipv6.ipv6_dns_servers[0], WIFI_IPV6_DNS_LEN)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }

        if (lwip_dns_setserver(nums, &dnsserver) != 0) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:dns_setserver failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    } else {
        ip_addr_t dnsserver[WIFI_MAX_DNS_NUM];
        dnsserver[0].type = IPADDR_TYPE_V6;
        dnsserver[1].type = IPADDR_TYPE_V6;

        if ((memcpy_s(&dnsserver[0].u_addr.ip6, sizeof(ip6_addr_t),
            config->static_ipv6.ipv6_dns_servers[0], WIFI_IPV6_DNS_LEN)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }

        if ((memcpy_s(&dnsserver[1].u_addr.ip6, sizeof(ip6_addr_t),
            config->static_ipv6.ipv6_dns_servers[1], WIFI_IPV6_DNS_LEN)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }

        if (lwip_dns_setserver(nums, dnsserver) != 0) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:dns_setserver_v6 failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    }
#endif
    return ERRCODE_SUCC;
}

static wifi_return_code service_set_ip(const wifi_sta_config_stru *config)
{
    int ret1, ret2;
    if (config->ip_type == DHCP) {
        return ERRCODE_SUCC;
    }

    ret1 = service_set_ipv4(config);
    ret2 = service_set_ipv6(config);
    if (ret1 != ERRCODE_SUCC && ret2 != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:IPv4 and IPv6 settings failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    ret1 = service_set_ipv4_dns(config);
    if (ret1 != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_set_ipv4_dns failed", __LINE__);
        return ret1;
    }

    ret2 = service_set_ipv6_dns(config);
    if (ret2 != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_set_ipv6_dns failed", __LINE__);
        return ret2;
    }

    return ERRCODE_SUCC;
}

static wifi_return_code service_set_assoc_config(const wifi_sta_config_stru *config, ext_wifi_assoc_request *req,
    const char *wep_key)
{
    uint32_t len;
    if (strlen(config->ssid) != 0 && strlen(config->ssid) <= EXT_WIFI_MAX_SSID_LEN) {
        if ((memcpy_s(req->ssid, EXT_WIFI_MAX_SSID_LEN + 1, config->ssid, strlen(config->ssid) + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    }

    if (service_addr_precheck(config->bssid) == ERRCODE_SUCC) {
        if ((memcpy_s(req->bssid, WIFI_MAC_LEN, config->bssid, WIFI_MAC_LEN)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    }

    req->auth = config->security_type;
    if (config->security_type == WIFI_SEC_TYPE_WEP && config->wifi_psk_type == 1) {
        req->key[0] = '\"';
        len = strlen(wep_key);
        if ((memcpy_s(req->key + 1, EXT_WIFI_MAX_KEY_LEN, wep_key, len + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
        req->key[len + 1] = '\"';
        req->key[len + 2] = '\0';  // 在len + 2置字符串结束符
    } else if (config->security_type == WIFI_SEC_TYPE_WEP && config->wifi_psk_type == 0) {
        req->key[0] = '\"';
        len = strlen(config->pre_shared_key);
        if ((memcpy_s(req->key + 1, EXT_WIFI_MAX_KEY_LEN, config->pre_shared_key, len + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
        req->key[len + 1] = '\"';
        req->key[len + 2] = '\0'; // 在len + 2置字符串结束符
    } else if (wifi_is_need_psk(config->security_type) == 1) {
        if ((memcpy_s(req->key, EXT_WIFI_MAX_KEY_LEN + 1,
            config->pre_shared_key, strlen(config->pre_shared_key) + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    }

    if (config->wifi_psk_type == 1) {
        req->hex_flag = 1;
    } else {
        req->hex_flag = 0;
    }
    if (config->security_type == WIFI_SEC_TYPE_OWE) {
        req->pairwise = EXT_WIFI_PAIRWISE_AES;
    }

    return ERRCODE_SUCC;
}

static wifi_return_code service_check_hotspot_config_advance(const softap_config_advance_stru *config)
{
    // beacon_interval的范围为25到1000
    if (config->beacon_interval < 25 || config->beacon_interval > 1000) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config->beacon is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    // dtim的范围为1到30
    if (config->dtim_period < 1 || config->dtim_period > 30) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config->dtim_period is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    // group_rekey的范围为30到86400
    if (config->group_rekey < 30 || config->group_rekey > 86400) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config->group_rekey is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    // 1不隐藏，2隐藏
    if (config->hidden_ssid_flag < 1 || config->hidden_ssid_flag > 2) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config->hidden_ssid_flag is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (config->protocol_mode != WIFI_MODE_11B && config->protocol_mode != WIFI_MODE_11B_G &&
        config->protocol_mode != WIFI_MODE_11B_G_N && config->protocol_mode != WIFI_MODE_11B_G_N_AX) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config->protocol_mode is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    g_softap_advance_config.beacon_interval = config->beacon_interval;
    g_softap_advance_config.dtim_period = config->dtim_period;
    g_softap_advance_config.gi = config->gi;
    g_softap_advance_config.group_rekey = config->group_rekey;
    g_softap_advance_config.hidden_ssid_flag = config->hidden_ssid_flag;
    g_softap_advance_config.protocol_mode = config->protocol_mode;
    return ERRCODE_SUCC;
}

static errcode_t service_ie_frame_check(wifi_if_type_enum iftype, const unsigned char frame_type_bitmap,
                                        ext_wifi_iftype *iftype_tmp)
{
    if (iftype == IFTYPE_STA) {
        if (frame_type_bitmap != 2) {       /* sta只会在probe request中插入IE，即frame_type_bitmap 为2 */
            service_error_log1(SERVICE_ERROR, "Srv:%d:frame_type_bitmap is invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
        *iftype_tmp = EXT_WIFI_IFTYPE_STATION;
        return ERRCODE_SUCC;
    }

    // softap只会在beacon和probe response中插入IE
    if (iftype == IFTYPE_AP) {
        if ((frame_type_bitmap & 0xFA) != 0) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:frame_type_bitmap is invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
        *iftype_tmp = EXT_WIFI_IFTYPE_AP;
        return ERRCODE_SUCC;
    }

    // p2p go只会在beacon和probe response中插入IE
    if (iftype == IFTYPE_P2P_GO) {
        if ((frame_type_bitmap & 0xFA) != 0) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:frame_type_bitmap is invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
        *iftype_tmp = EXT_WIFI_IFTYPE_P2P_GO;
        return ERRCODE_SUCC;
    }

    // p2p device可以在probe request和probe response中均插入IE
    if (iftype == IFTYPE_P2P_DEVICE) {
        if ((frame_type_bitmap & 0xF9) != 0) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:frame_type_bitmap is invalid", __LINE__);
            return ERROR_WIFI_INVALID_ARGS;
        }
        *iftype_tmp = EXT_WIFI_IFTYPE_P2P_DEVICE;
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}

static wifi_return_code service_set_softap_protocol(protocol_mode_enum protocol_mode)
{
    if (uapi_wifi_softap_set_protocol_mode(protocol_mode) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_protocol_mod failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
    return ERRCODE_SUCC;
}


#ifdef CONFIG_WPS_SUPPORT
static wifi_return_code service_pbc_pin_connect(wifi_wps_config  *wps_config)
{
    unsigned int   spin_len;
    int ret = 0;
    if (wps_config->wps_method == WIFI_WPS_PBC) {
        if (strlen(wps_config->bssid) == 0) {
            ret =  uapi_wifi_sta_wps_pbc(NULL, 0);
        } else {
            ret =  uapi_wifi_sta_wps_pbc(wps_config->bssid, WIFI_MAC_LEN);
        }
    } else if (wps_config->wps_method == WIFI_WPS_PIN) {
        spin_len = strlen(wps_config->wps_pin);
        if (spin_len != (WIFI_WPS_PIN_MAX_LEN_NUM - 1)) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: wps_config pin_len is null", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }

        for (int i = 0; i < spin_len; i++) {
            if ((wps_config->wps_pin[i] < '0') || (wps_config->wps_pin[i] > '9')) {
                service_error_log1(SERVICE_ERROR, "Srv:%d: wps_config.wpsPin is invalid", __LINE__);
                return ERROR_WIFI_UNKNOWN;
            }
        }

        if (strlen(wps_config->bssid) == 0) {
            ret = uapi_wifi_sta_wps_pin(wps_config->wps_pin, spin_len, NULL, 0);
        } else {
            ret = uapi_wifi_sta_wps_pin(wps_config->wps_pin, spin_len, wps_config->bssid, WIFI_MAC_LEN);
        }
    }

    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
}

static unsigned char get_wps_mode(unsigned short config_methods)
{
    unsigned char wps_mode = 0;
    if ((config_methods & WPS_PUSHBUTTON) != 0) {
        wps_mode |= 0x1;
    }
    if ((config_methods & WPS_LABEL) != 0) {
        wps_mode |= 0x2;
    }
    if ((config_methods & WPS_DISPLAY) != 0) {
        wps_mode |= 0x4;
    }
    if ((config_methods & WPS_KEYPAD) != 0) {
        wps_mode |= 0x8;
    }
    if (wps_mode == 0) {
        wps_mode = 0x10;
    }
    return wps_mode;
}
#endif /* CONFIG_WPS_SUPPORT */

errcode_t wifi_init(void)
{
    uint8_t  vap_res_num = WIFI_ACTIVE_VAP_MAX_NUM;
    uint8_t user_res_num = WIFI_DEFAULT_MAX_NUM_STA;

    if (uapi_wifi_get_init_status() == 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi has been initialized", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_init(vap_res_num, user_res_num) != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:wifi_init failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if (wifi_sta_set_pmf_mode(WIFI_MGMT_FRAME_PROTECTION_OPTIONAL) != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:SetWiFiPmf failed", __LINE__);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t wifi_deinit(void)
{
    if (uapi_wifi_get_init_status() == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi has been uninstalled", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_deinit() != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:wifi_deinit failed", __LINE__);
        return ERRCODE_FAIL;
    } else {
        memset_s(g_sta_ifname, WIFI_IFNAME_MAX_SIZE + 1, 0, WIFI_IFNAME_MAX_SIZE + 1);
        memset_s(g_hotspot_ifname, WIFI_IFNAME_MAX_SIZE + 1, 0, WIFI_IFNAME_MAX_SIZE + 1);
        memset_s(g_p2p_ifname, WIFI_IFNAME_MAX_SIZE + 1, 0, WIFI_IFNAME_MAX_SIZE + 1);
        g_sta_enble_flag = 0;
        g_p2p_enable_flag = 0;
        g_softap_enble_flag = 0;
        g_softap_hidden = 1;
        return ERRCODE_SUCC;
    }
}

int32_t wifi_is_wifi_inited(void)
{
    int32_t result = uapi_wifi_get_init_status();
    return result;
}

wifi_return_code get_ifname(wifi_device_type_enum type, unsigned int *name_bufsize, char *ifname)
{
    if (ifname == NULL || type >= WIFI_TYPE_BUTT || name_bufsize == NULL || *name_bufsize > WIFI_IFNAME_MAX_SIZE + 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:param is NULL", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERROR_WIFI_NOT_STARTED;
    }

    if (type == WIFI_TYPE_STA) {
        if (g_sta_enble_flag != 1) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:sta is not enabled", __LINE__);
            return ERROR_WIFI_IFACE_INVALID;
        }

        if ((memcpy_s(ifname, *name_bufsize, g_sta_ifname, strlen(g_sta_ifname) + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    } else if (type == WIFI_TYPE_HOTSPOT) {
        if (g_softap_enble_flag != 1) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:softap is not enabled", __LINE__);
            return ERROR_WIFI_IFACE_INVALID;
        }

        if ((memcpy_s(ifname, *name_bufsize, g_hotspot_ifname, strlen(g_hotspot_ifname) + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    } else if (type == WIFI_TYPE_P2P) {
        if (g_p2p_enable_flag != 1) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:p2p is not enabled", __LINE__);
            return ERROR_WIFI_IFACE_INVALID;
        }

        if ((memcpy_s(ifname, *name_bufsize, g_p2p_ifname, strlen(g_p2p_ifname) + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERROR_WIFI_UNKNOWN;
        }
    }

    *name_bufsize = (unsigned int)strlen(ifname) + 1;

    return ERRCODE_SUCC;
}

errcode_t wifi_sta_enable(void)
{
    if (wifi_is_sta_enabled() == 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta has been enabled", __LINE__);
        return ERRCODE_FAIL;
    }

    int len = WIFI_IFNAME_MAX_SIZE + 1;
    if (register_callback() != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: register callback fail", __LINE__);
        return ERRCODE_FAIL;
    }
    if (uapi_wifi_sta_start(g_sta_ifname, &len) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta_start failed", __LINE__);
        return ERRCODE_FAIL;
    } else {
        g_sta_enble_flag = 1;
        return ERRCODE_SUCC;
    }
}

errcode_t wifi_sta_disable(void)
{
    if (wifi_is_sta_enabled() == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta has been disabled", __LINE__);
        return ERRCODE_FAIL;
    }

    /* 关闭STA时，自动关闭DHCP */
    struct netif *netif = netifapi_netif_find_by_name(g_sta_ifname);
    if (netifapi_dhcp_stop(netif) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:DHCP stop failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_sta_stop() != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta_stop failed", __LINE__);
        return ERRCODE_FAIL;
    } else {
        memset_s(g_sta_ifname, WIFI_IFNAME_MAX_SIZE + 1, 0, WIFI_IFNAME_MAX_SIZE + 1);
        g_sta_enble_flag = 0;
        g_sta_last_scan_time_stamp_ms = 0;
        return ERRCODE_SUCC;
    }
}

int32_t wifi_is_sta_enabled(void)
{
    if (uapi_wifi_get_init_status() != 1) {
        return 0;
    }

    if (g_sta_enble_flag == 0) {
        return 0;
    } else {
        return 1;
    }
}

wifi_dev_t *wifi_get_dev(wifi_iftype_t iftype)
{
    return (wifi_dev_t *)wifi_dev_get(iftype);
}

errcode_t wifi_sta_set_protocol_mode(protocol_mode_enum mode)
{
    if (uapi_wifi_sta_set_protocol_mode(mode) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_protocol_mod failed", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

protocol_mode_enum wifi_sta_get_protocol_mode(void)
{
    return uapi_wifi_sta_get_protocol_mode();
}

errcode_t wifi_sta_set_scan_policy(wifi_if_type_enum iftype, wifi_scan_strategy_stru *scan_strategy)
{
    int ret;
    ext_wifi_scan_param_stru scan_param = { 0 };

    if (iftype >= IFTYPES_BUTT || scan_strategy == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERRCODE_NOT_SUPPORT;
    }

    scan_param.scan_cnt = scan_strategy->scan_cnt;   /* 扫描次数，取值范围0，1～10。默认值为2 */
    scan_param.scan_time = scan_strategy->scan_time; /* 扫描驻留时间，取值范围0，20～120。默认值为20ms。 */
    scan_param.single_probe_send_times = scan_strategy->single_probe_send_times; /* 单个probe发送次数, 即host重发次数 */

    if (iftype == IFTYPE_STA && g_sta_enble_flag == 1) {
        ret = uapi_wifi_set_scan_param(g_sta_ifname, &scan_param);
    } else if (iftype == IFTYPE_AP && g_softap_enble_flag == 1) {
        ret = uapi_wifi_set_scan_param(g_hotspot_ifname, &scan_param);
    } else if (g_p2p_enable_flag == 1) {
        ret = uapi_wifi_set_scan_param(g_p2p_ifname, &scan_param);
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:vap is not enabled or iftype is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%dSetWifiScanStrategy failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_sta_scan_stop(void)
{
    return uapi_wifi_force_scan_complete();
}

errcode_t wifi_ap_scan(void)
{
    uint32_t ret = service_wifi_and_ap_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_ap_check failed", __LINE__);
        return ret;
    }

    int32_t res = uapi_wifi_ap_scan();
    if (res == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_raw_scan(wifi_scan_params_stru *scan_param, wifi_scan_no_save_cb cb)
{
    ext_wifi_scan_params sp = {0};

    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }

    if (scan_param == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: raw scan_param is null", __LINE__);
        return ERRCODE_FAIL;
    }

    if (scan_param->scan_type == WIFI_BASIC_SCAN) {
        sp.scan_type = EXT_WIFI_BASIC_SCAN;
    } else if (scan_param->scan_type == WIFI_CHANNEL_SCAN) {
        sp.chan_list[0] = (uint8_t)scan_param->channel_num;
        if (sp.chan_list[0] == 0 || sp.chan_list[0] > 14) {  // 2.4g信道最大14
            service_error_log1(SERVICE_ERROR, "Srv:%d: raw scan_param->freqs is invalid", __LINE__);
            return ERRCODE_FAIL;
        }
        sp.scan_type = EXT_WIFI_CHANNEL_SCAN;
        sp.chan_num = 1;
    } else if (scan_param->scan_type == WIFI_SSID_SCAN || scan_param->scan_type == WIFI_SSID_PREFIX_SCAN) {
        uint32_t len = strlen(scan_param->ssid);
        if (scan_param->ssid_len == 0 || scan_param->ssid_len > EXT_WIFI_MAX_SSID_LEN || scan_param->ssid_len != len) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: scan ssid is invalid", __LINE__);
            return ERRCODE_FAIL;
        }

        sp.scan_type = (scan_param->scan_type == WIFI_SSID_SCAN) ? EXT_WIFI_SSID_SCAN : EXT_WIFI_SSID_PREFIX_SCAN;
        if ((memcpy_s(sp.ssid, EXT_WIFI_MAX_SSID_LEN + 1, (const void *)scan_param->ssid,
            (size_t)scan_param->ssid_len + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            return ERRCODE_FAIL;
        }

        sp.ssid_len = (uint8_t)scan_param->ssid_len;
    } else if (scan_param->scan_type == WIFI_BSSID_SCAN) {
        if (service_addr_precheck(scan_param->bssid) != ERRCODE_SUCC) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: raw bssid is invalid", __LINE__);
            return ERRCODE_FAIL;
        }

        sp.scan_type = EXT_WIFI_BSSID_SCAN;
        memcpy_s(sp.bssid, EXT_WIFI_MAC_LEN, scan_param->bssid, EXT_WIFI_MAC_LEN);
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: scan_param->scan_type is not supported", __LINE__);
        return ERRCODE_FAIL;
    }

    return uapi_wifi_sta_raw_scan(&sp, (ext_wifi_scan_no_save_cb)cb) == EXT_WIFI_OK ? ERRCODE_SUCC : ERRCODE_FAIL;
}

errcode_t wifi_sta_scan(void)
{
    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }

    int32_t res = uapi_wifi_sta_scan();
    if (res == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_sta_scan_advance(const wifi_scan_params_stru *scan_param)
{
    ext_wifi_scan_params sp = {0};

    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }

    if (scan_param == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:adv scan_param is null", __LINE__);
        return ERRCODE_FAIL;
    }

    if (scan_param->scan_type == WIFI_CHANNEL_SCAN) {
        sp.chan_list[0] = (uint8_t)scan_param->channel_num;
        if (sp.chan_list[0] == 0 || sp.chan_list[0] > 14) {  // 2.4g信道最大14
            service_error_log1(SERVICE_ERROR, "Srv:%d: scan_param->freqs is invalid", __LINE__);
            return ERRCODE_FAIL;
        }
        sp.scan_type = EXT_WIFI_CHANNEL_SCAN;
        sp.chan_num = 1;
    } else if (scan_param->scan_type == WIFI_SSID_SCAN || scan_param->scan_type == WIFI_SSID_PREFIX_SCAN) {
        uint32_t len = strlen(scan_param->ssid);
        if (scan_param->ssid_len == 0 || scan_param->ssid_len > EXT_WIFI_MAX_SSID_LEN || scan_param->ssid_len != len) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: ssid is invalid", __LINE__);
            return ERRCODE_FAIL;
        }

        sp.scan_type = (scan_param->scan_type == WIFI_SSID_SCAN) ? EXT_WIFI_SSID_SCAN : EXT_WIFI_SSID_PREFIX_SCAN;
        if ((memcpy_s(sp.ssid, EXT_WIFI_MAX_SSID_LEN + 1, (const void *)scan_param->ssid,
            (size_t)scan_param->ssid_len + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:ssid memcpy_s failed", __LINE__);
            return ERRCODE_FAIL;
        }

        sp.ssid_len = (uint8_t)scan_param->ssid_len;
    } else if (scan_param->scan_type == WIFI_BSSID_SCAN) {
        if (service_addr_precheck(scan_param->bssid) != ERRCODE_SUCC) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: bssid is invalid", __LINE__);
            return ERRCODE_FAIL;
        }

        sp.scan_type = EXT_WIFI_BSSID_SCAN;
        memcpy_s(sp.bssid, EXT_WIFI_MAC_LEN, scan_param->bssid, EXT_WIFI_MAC_LEN);
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: scan_param->scan_type is not supported", __LINE__);
        return ERRCODE_FAIL;
    }

    return uapi_wifi_sta_advance_scan(&sp) == EXT_WIFI_OK ? ERRCODE_SUCC : ERRCODE_FAIL;
}

errcode_t wifi_sta_get_scan_info(wifi_scan_info_stru *result, uint32_t *size)
{
    ext_wifi_ap_info *ap_list = NULL;

    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }

    // size 最大为64，超过64底层接口截取按64计算
    if (result == NULL || size == NULL || *size == 0 || *size > WIFI_SCAN_AP_LIMIT) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: size is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    memset_s(result, sizeof(wifi_scan_info_stru) * (*size), 0, sizeof(wifi_scan_info_stru) * (*size));

    ap_list = (ext_wifi_ap_info *)malloc(sizeof(ext_wifi_ap_info) * (*size));
    if (ap_list == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%dGetScanInfoList malloc err.", __LINE__);
        return ERRCODE_FAIL;
    }

    int32_t res = uapi_wifi_sta_scan_results(ap_list, size);
    if (res != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta_scan_results failed", __LINE__);
        free(ap_list);
        return ERRCODE_FAIL;
    }

    service_error_log2(SERVICE_ERROR, "Srv:%d:sta_scan_results cnt %d", __LINE__, *size);
    for (uint32_t i = 0; i < *size; i++) {
        uint32_t ssid_len = strlen(ap_list[i].ssid);
        if ((memcpy_s(result[i].ssid, WIFI_MAX_SSID_LEN, ap_list[i].ssid, ssid_len + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            free(ap_list);
            return ERRCODE_FAIL;
        }

        if ((memcpy_s(result[i].bssid, WIFI_MAC_LEN, ap_list[i].bssid, WIFI_MAC_LEN)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            free(ap_list);
            return ERRCODE_FAIL;
        }

        result[i].security_type = ap_list[i].auth;
        // 扫描结果中的rssi需要除以100才能获得实际的rssi
        result[i].rssi = ap_list[i].rssi / 100;
        result[i].band = 1;                    // 92只支持2.4g

        result[i].channel_num = (int32_t)ap_list[i].channel;
    }
    free(ap_list);
    return ERRCODE_SUCC;
}

errcode_t wifi_sta_scan_result_clear(void)
{
    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }
    return uapi_wifi_sta_scan_results_clear() != EXT_WIFI_OK ? ERRCODE_FAIL : ERRCODE_SUCC;
}

errcode_t wifi_set_channel(wifi_if_type_enum iftype, int32_t channel)
{
    const char *ifname = NULL;

    if (iftype >= IFTYPES_BUTT) {
        service_error_log0(SERVICE_ERROR, "wifi_set_channel::para is invalid");
        return ERRCODE_FAIL;
    }
    if (iftype == IFTYPE_STA && g_sta_enble_flag == 1) {
        ifname = g_sta_ifname;
    } else if (iftype == IFTYPE_AP && g_softap_enble_flag == 1) {
        ifname = g_hotspot_ifname;
    } else if (iftype >= IFTYPE_P2P_CLIENT && iftype <= IFTYPE_P2P_DEVICE && g_p2p_enable_flag == 1) {
        ifname = g_p2p_ifname;
    } else {
        service_error_log1(SERVICE_ERROR, "wifi_set_channel::vap is not enabled or iftype[%d] is invalid", iftype);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_set_channel(ifname, (uint8_t)strlen(ifname), channel) != 0) {
        service_error_log1(SERVICE_ERROR, "wifi_set_channel::uapi_wifi_set_channel[%d] failed", channel);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t wifi_get_channel(wifi_if_type_enum iftype, int32_t *channel)
{
    const char *ifname = NULL;

    if (iftype >= IFTYPES_BUTT) {
        service_error_log0(SERVICE_ERROR, "wifi_get_channel::para is invalid");
        return ERRCODE_FAIL;
    }
    if (iftype == IFTYPE_STA && g_sta_enble_flag == 1) {
        ifname = g_sta_ifname;
    } else if (iftype == IFTYPE_AP && g_softap_enble_flag == 1) {
        ifname = g_hotspot_ifname;
    } else if (iftype >= IFTYPE_P2P_CLIENT && iftype <= IFTYPE_P2P_DEVICE && g_p2p_enable_flag == 1) {
        ifname = g_p2p_ifname;
    } else {
        service_error_log1(SERVICE_ERROR, "wifi_get_channel::vap is not enabled or iftype[%d] is invalid", iftype);
        return ERRCODE_FAIL;
    }

    *channel = uapi_wifi_get_channel(ifname, (uint8_t)strlen(ifname));
    return ERRCODE_SUCC;
}

#ifdef CONFIG_WNM
errcode_t wifi_sta_wnm_bss_query(int32_t reason_code, int32_t candidate_list)
{
    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:wifi_sta_wnm_bss_query failed", __LINE__);
        return ret;
    }
    if (uapi_wifi_wnm_bss_query(reason_code, candidate_list) != EXT_WIFI_OK) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif

/* 根据认证方式, 设置pkt_type ;WEP 通过长度判断:5/13 为ascii 10/26为hex */
OSAL_STATIC int32_t wifi_conn_filter_sta_key(wifi_sta_config_stru *assoc_req)
{
    uint32_t key_len = strlen(assoc_req->pre_shared_key);

    if (assoc_req->security_type == WIFI_SEC_TYPE_WPA2PSK ||
        assoc_req->security_type == WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX ||
        assoc_req->security_type == WIFI_SEC_TYPE_WPAPSK) {
        if (assoc_req->wifi_psk_type == 0 && key_len == 64) {   /* 64位密钥长度不能设置psk_type为0 */
            service_error_log1(SERVICE_ERROR, "Srv:%d:WPA/WPA2/WPA-WPA2 psk_type invalid.", __LINE__);
            return ERRCODE_FAIL;
        }
        /* 8-63位密钥长度不能设置psk_type为1,64位为HEX类型密钥 */
        if (assoc_req->wifi_psk_type == 1 && key_len >= 8 && key_len < 64) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:WPA/WPA2/WPA-WPA2 psk_type invalid.", __LINE__);
            return ERRCODE_FAIL;
        }
    }

    if (assoc_req->security_type == WIFI_SEC_TYPE_WEP) {
        if (assoc_req->wifi_psk_type == 0 && (key_len == 10 || key_len == 26)) {    /* WEP密钥长度为10/26表示HEX类型 */
            service_error_log1(SERVICE_ERROR, "Srv:%d:WEP psk_type invalid.", __LINE__);
            return ERRCODE_FAIL;
        }
        if (assoc_req->wifi_psk_type == 1 && (key_len == 5 || key_len == 13)) {     /* WEP密钥长度为5/13表示ASCII类型 */
            service_error_log1(SERVICE_ERROR, "Srv:%d:WEP psk_type invalid.", __LINE__);
            return ERRCODE_FAIL;
        }
    }

    return ERRCODE_SUCC;
}

/* 检查关联请求信息与扫描结果是否匹配不匹配返回0, 匹配返回1 */
OSAL_STATIC int32_t wifi_conn_scan_result_cmp(wifi_sta_config_stru *conn_config, ext_wifi_ap_info *ap_list)
{
    /* 如果SSID/BSSID都没设置,直接err */
    if (strlen(conn_config->ssid) == 0 && service_addr_precheck(conn_config->bssid) != ERRCODE_SUCC) {
        return 0;
    }

    /* 如果同时设置了SSID与BSSID,两者必须都匹配 */
    if (strlen(conn_config->ssid) > 0 && service_addr_precheck(conn_config->bssid) == ERRCODE_SUCC) {
        if (strcmp(conn_config->ssid, ap_list->ssid) == 0 &&
            memcmp(conn_config->bssid, ap_list->bssid, WIFI_MAC_LEN) == 0) {
            return 1;
        }
        return 0;
    }
    /* 只设置了SSID OR BSSID, 单项匹配即可 */
    if (strlen(conn_config->ssid) == 0) {
        return (memcmp(conn_config->bssid, ap_list->bssid, WIFI_MAC_LEN) == 0);
    }
    return (strcmp(conn_config->ssid, ap_list->ssid) == 0);
}

OSAL_STATIC void wifi_sta_connect_fill_security_type(ext_wifi_assoc_request *req)
{
    uint32_t num = WIFI_SCAN_AP_LIMIT;
    int32_t rssi = -128; /* rssi最小值 */
    uint8_t ft_flag = 0;
    wifi_security_enum security_type = WIFI_SEC_TYPE_INVALID;
    ext_wifi_ap_info *ap_list = NULL;
    ext_wifi_pairwise pairwise = EXT_WIFI_PARIWISE_UNKNOWN;
    int32_t ret;

    ap_list = (ext_wifi_ap_info *)malloc(sizeof(ext_wifi_ap_info) * num);
    if (ap_list == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%dwifi_sta_connect_fill_security_type malloc err.", __LINE__);
        return;
    }

    ret = uapi_wifi_sta_scan_results(ap_list, &num);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta_scan_results failed", __LINE__);
        free(ap_list);
        return;
    }

    /* 根据SSID匹配认证方式 */
    for (int32_t i = 0; (i < num) && (i < WIFI_SCAN_AP_LIMIT); i++) {
        if ((wifi_conn_scan_result_cmp(&g_sta_conn_config, &ap_list[i]) == 0) ||
            (g_sta_conn_config.channel != 0 && ap_list[i].channel != g_sta_conn_config.channel)) {
            continue;
        }
        /* 多个匹配情况,取RSSI大者, 扫描结果中的rssi需要除以100才能获得实际的rssi */
        if (ap_list[i].rssi / 100 > rssi) {
            security_type = ap_list[i].auth;
            pairwise = ap_list[i].pairwise;
            rssi = ap_list[i].rssi / 100; /* 除以100获得实际的rssi */
            ft_flag = ap_list[i].ft_flag;
            if (g_sta_conn_config.channel != 0 && memcpy_s(g_sta_conn_config.bssid, sizeof(g_sta_conn_config.bssid),
                ap_list[i].bssid, sizeof(ap_list[i].bssid)) != EOK) {
                free(ap_list);
                return;
            }
            service_error_log4(SERVICE_ERROR, "Srv:find ssid[%s] auth type[%d] pairwise[%d] ft_flag[%d]",
                g_sta_conn_config.ssid, security_type, pairwise, ft_flag);
        }
    }

    free(ap_list);

    g_sta_conn_config.security_type = security_type;
    req->pairwise = pairwise;
    req->ft_flag = ft_flag;
}

/* 校验关联参数中的密码与加密类型是否匹配 */
OAL_STATIC osal_u8 wifi_sta_pre_check_key(wifi_sta_config_stru *sta_conn_config)
{
    if (sta_conn_config == OSAL_NULL) {
        return OSAL_FALSE;
    }
    /* 仅在strengthen_verify为1时才进行校验 */
    if (sta_conn_config->strengthen_verify == 0) {
        return OSAL_TRUE;
    }
    /* 带密码关联OPEN场景 */
    if (sta_conn_config->security_type == WIFI_SEC_TYPE_OPEN && strlen(sta_conn_config->pre_shared_key) != 0) {
        return OSAL_FALSE;
    }
    /* 无密码关联加密场景 */
    if (sta_conn_config->security_type > WIFI_SEC_TYPE_OPEN && strlen(sta_conn_config->pre_shared_key) == 0) {
        return OSAL_FALSE;
    }
    return OSAL_TRUE;
}

/* 关联下半段 */
OAL_STATIC errcode_t wifi_sta_connect_part2(void)
{
    ext_wifi_assoc_request req = {0};
    event_wifi_disconnected wifi_disconnected = {0};
    char wep_key[WIFI_MAX_KEY_LEN];
    (void)memset_s(wep_key, WIFI_MAX_KEY_LEN, 0, WIFI_MAX_KEY_LEN);

    g_sta_conn_req_flag = 0;
    /* 根据扫描结果查询认证方式 */
    wifi_sta_connect_fill_security_type(&req);

    if (g_sta_conn_config.security_type == WIFI_SEC_TYPE_INVALID || wifi_sta_pre_check_key(&g_sta_conn_config) == OSAL_FALSE) {
        /* 未扫到对应AP的情况下,上报错误码 */
        wifi_disconnected.reason_code = WIFI_NETWORK_NOT_FOUND_ERROR;
        wifi_disconnected.locally_generated = 1;
        if (memcpy_s(wifi_disconnected.ifname, sizeof(wifi_disconnected.ifname),
            g_sta_ifname, sizeof(g_sta_ifname)) != EOK) {
            return ERRCODE_FAIL;
        }
        send_broadcast_disconnected(&wifi_disconnected);
        service_error_log1(SERVICE_ERROR, "Srv:%d: not found suitable SSID and auth type", __LINE__);
        return ERRCODE_FAIL;
    }
    /* 如果找到了, 走完关联下半段 */
    if (wifi_is_need_psk(g_sta_conn_config.security_type) == 1) {
        wifi_conn_filter_sta_key(&g_sta_conn_config);
    }

    uint32_t ret = service_check_pre_shared_key(g_sta_conn_config.pre_shared_key, g_sta_conn_config.security_type,
        g_sta_conn_config.wifi_psk_type, wep_key);
    if (ret != ERRCODE_SUCC) {
        /* 密码校验错误,上报错误码 */
        wifi_disconnected.reason_code = WLAN_REASON_MIC_FAILURE;
        wifi_disconnected.locally_generated = 1;
        if (memcpy_s(wifi_disconnected.ifname, sizeof(wifi_disconnected.ifname),
            g_sta_ifname, sizeof(g_sta_ifname)) != EOK) {
            return ERRCODE_FAIL;
        }
        send_broadcast_disconnected(&wifi_disconnected);
        service_error_log1(SERVICE_ERROR, "Srv:%d: config->pre_shared_key is invalid", __LINE__);
        return ret;
    }

    ret = service_set_assoc_config(&g_sta_conn_config, &req, wep_key);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: service_set_assoc_config failed", __LINE__);
        return ret;
    }

    if (uapi_wifi_sta_connect(&req) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:failed", __LINE__);
        return ERRCODE_FAIL;
    } else {
        return ERRCODE_SUCC;
    }
}

static void wifi_sta_connect_fill_scan_param(const wifi_sta_config_stru *config, wifi_scan_params_stru *scan_param)
{
    (void)memset_s(scan_param, sizeof(wifi_scan_params_stru), 0, sizeof(wifi_scan_params_stru));

    /* 设置SSID */
    if (strlen(config->ssid) > 0) {
        service_error_log0(SERVICE_ERROR, "wifi_sta_connect_fill_scan_param:: Specifying an SSID for scanning");
        scan_param->scan_type = WIFI_SSID_SCAN;
        (void)memcpy_s(scan_param->ssid, WIFI_MAX_SSID_LEN, config->ssid, WIFI_MAX_SSID_LEN);
        scan_param->ssid_len = (int8_t)strlen(config->ssid);
    }
}

/* 关联上半段, 如果5s内没扫描过,触发扫描,在扫描结果上报时走完下半段 */
OSAL_STATIC errcode_t wifi_sta_connect_part1(const wifi_sta_config_stru *config)
{
    uint64_t current_time_stamp_ms = wifi_get_timestamp_ms();
    wifi_scan_params_stru scan_param = {0};

    /* 保存请求关联的信息, 认证方式置为无效,由扫描结果获取 */
    if (memcpy_s(&g_sta_conn_config, sizeof(wifi_sta_config_stru), config, sizeof(wifi_sta_config_stru)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: memcpy_s fail", __LINE__);
        return ERRCODE_FAIL;
    }
    g_sta_conn_config.security_type = WIFI_SEC_TYPE_INVALID;

    /* 如果当前正在扫描，在扫描完成回调执行关联的下半段 */
    if (uapi_wifi_get_scan_flag() != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: Trigger association after the scanning is complete", __LINE__);
        g_sta_conn_req_flag = 1;
        return ERRCODE_SUCC;
    }

    /* 5s内没扫描, 且当前没有扫描, 则主动触发扫描,在扫描结束后执行关联的下半段 */
    if ((current_time_stamp_ms - g_sta_last_scan_time_stamp_ms >= WIFI_CONN_SCAN_INTERVAL) ||
        (g_sta_last_scan_time_stamp_ms == 0)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: last scantime > 5s, trigger scan", __LINE__);
        g_sta_conn_req_flag = 1;
        wifi_sta_connect_fill_scan_param(config, &scan_param);
        return wifi_sta_scan_advance(&scan_param);
    } else {
        /* 如果5s内扫描过,直接走关联下半段 */
        return wifi_sta_connect_part2();
    }

    return ERRCODE_SUCC;
}

errcode_t wifi_sta_connect(const wifi_sta_config_stru *config)
{
    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }

    ret = service_check_wifi_device_config(config);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_check_wifi_device_config failed", __LINE__);
        return ret;
    }

    ret = service_set_ip(config);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: ip is invalid", __LINE__);
        return ret;
    }

    return wifi_sta_connect_part1(config);
}

errcode_t wifi_sta_disconnect(void)
{
    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }
    /* 下发DISCONN时, 将关联请求置为false, 终止关联动作 */
    g_sta_conn_req_flag = 0;
    int32_t res = uapi_wifi_sta_disconnect();
    if (res == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_sta_get_ap_info(wifi_linked_info_stru *result)
{
    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }

    if (result == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: result is NULL", __LINE__);
        return ERRCODE_FAIL;
    }

    memset_s(result, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));

    ext_wifi_status connect_status = {0};
    int32_t res = uapi_wifi_sta_get_connect_info(&connect_status);
    if (res != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:get_connect_info failed", __LINE__);
        return ERRCODE_FAIL;
    }

    result->conn_state = connect_status.status;

    if ((memcpy_s(result->ssid, WIFI_MAX_SSID_LEN, connect_status.ssid, strlen(connect_status.ssid) + 1)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if ((memcpy_s(result->bssid, WIFI_MAC_LEN, connect_status.bssid, WIFI_MAC_LEN)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }

    result->channel_num = (int32_t)connect_status.channel;

    result->rssi = uapi_wifi_sta_get_ap_rssi();
    result->snr = uapi_wifi_sta_get_ap_snr();

    return ERRCODE_SUCC;
}

errcode_t wifi_sta_set_reconnect_policy(int32_t enable, uint32_t seconds,
    uint32_t period, uint32_t max_try_count)
{
    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }
    int32_t res = uapi_wifi_sta_set_reconnect_policy(enable, seconds, period, max_try_count);
    if (res == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_sta_set_pmf_mode(wifi_pmf_option_enum pmf)
{
    int32_t ret = uapi_wifi_set_pmf(pmf);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set pmf failed", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

wifi_pmf_option_enum wifi_sta_get_pmf_mode(void)
{
    return uapi_wifi_get_pmf();
}

int16_t wifi_sta_get_connect_status_code(void)
{
    return uapi_wifi_get_mac_status_code();
}

errcode_t wifi_set_mgmt_frame_rx_cb(wifi_rx_mgmt_cb data_cb, uint8_t mode)
{
    if (uapi_wifi_set_mgmt_report(data_cb, mode) != 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_set_promis_mode(wifi_if_type_enum iftype,
    int32_t enable, const wifi_ptype_filter_stru *filter)
{
    const char *ifname = NULL;

    if (iftype >= IFTYPES_BUTT) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERRCODE_FAIL;
    }
    if (iftype == IFTYPE_STA && g_sta_enble_flag == 1) {
        ifname = g_sta_ifname;
    } else if (iftype == IFTYPE_AP && g_softap_enble_flag == 1) {
        ifname = g_hotspot_ifname;
    } else if (iftype >= IFTYPE_P2P_CLIENT && iftype <= IFTYPE_P2P_DEVICE && g_p2p_enable_flag == 1) {
        ifname = g_p2p_ifname;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:vap is not enabled or iftype is invalid", __LINE__);
        return ERRCODE_FAIL;
    }
    if (uapi_wifi_promis_enable(ifname, enable, (const ext_wifi_ptype_filter_stru *)filter) != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:uapi_wifi_promis_enable failed", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_SDP
static errcode_t wifi_set_sdp_mode_config(wifi_if_type_enum iftype, char **ifname, uint32_t line_num)
{
    if (iftype >= IFTYPES_BUTT) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:iftype is invalid", line_num);
        return ERRCODE_FAIL;
    }
    if (iftype == IFTYPE_STA && g_sta_enble_flag == 1) {
        *ifname = g_sta_ifname;
    } else if (iftype == IFTYPE_AP && g_softap_enble_flag == 1) {
        *ifname = g_hotspot_ifname;
    } else if (iftype >= IFTYPE_P2P_CLIENT && iftype <= IFTYPE_P2P_DEVICE && g_p2p_enable_flag == 1) {
        *ifname = g_p2p_ifname;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:vap is not enabled or iftype is invalid", line_num);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_set_sdp_mode(wifi_if_type_enum iftype, int32_t enable, int32_t ratio)
{
    char *ifname = NULL;

    if (wifi_set_sdp_mode_config(iftype, &ifname, __LINE__) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_sdp_enable(ifname, enable, ratio) != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:uapi_wifi_promis_enable failed", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_set_sdp_subscribe(wifi_if_type_enum iftype, char *sdp_subscribe, int32_t local_handle)
{
    char *ifname = NULL;

    if (wifi_set_sdp_mode_config(iftype, &ifname, __LINE__) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_sdp_subscribe(ifname, sdp_subscribe, local_handle) != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:uapi_wifi_promis_enable failed", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif
errcode_t wifi_set_promis_rx_pkt_cb(wifi_promis_cb data_cb)
{
    if (uapi_wifi_promis_set_rx_callback(data_cb) != 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

#ifdef CONFIG_WPS_SUPPORT
wifi_return_code wps_connect(const wifi_wps_config *wps_config)
{
    wifi_wps_config  *wps_config_tmp = NULL;

    int ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }
    if (wps_config == NULL || (wps_config->wps_method >= WIFI_WPS_BUTT)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:wps_config.wps_method is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    wps_config_tmp = (wifi_wps_config *)malloc(sizeof(wifi_wps_config));
    if (wps_config_tmp == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:wifi_wps_config malloc err.", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    if (memcpy_s(wps_config_tmp, sizeof(wifi_wps_config), wps_config, sizeof(wifi_wps_config)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        free(wps_config_tmp);
        return ERROR_WIFI_INVALID_ARGS;
    }
    ret = service_pbc_pin_connect(wps_config_tmp);

    free(wps_config_tmp);
    if (ret == ERRCODE_SUCC) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
}
#endif /* CONFIG_WPS_SUPPORT */

errcode_t wifi_sta_fast_connect(const wifi_fast_connect_stru *fast_request)
{
    ext_wifi_fast_assoc_request  fast_request_temp = {0};
    int8_t wep_key[WIFI_MAX_KEY_LEN];

    if (fast_request->psk_flag != WIFI_WPA_PSK_NOT_USE) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:psk_flag is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    uint32_t ret = service_wifi_and_sta_check();
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed", __LINE__);
        return ret;
    }

    ret = service_check_wifi_device_config(&fast_request->config);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_check_wifi_device_config failed", __LINE__);
        return ret;
    }

    ret = service_set_ip(&fast_request->config);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: ip is invalid", __LINE__);
        return ret;
    }

    ret = service_check_pre_shared_key(fast_request->config.pre_shared_key, fast_request->config.security_type,
                                       fast_request->config.wifi_psk_type, wep_key);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: config->pre_shared_key is invalid", __LINE__);
        return ret;
    }

    ret = service_set_assoc_config(&fast_request->config, &fast_request_temp.req, wep_key);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: service_set_assoc_config failed", __LINE__);
        return ret;
    }

    // 2.4g信道最大14
    if (fast_request->channel_num > 14 || fast_request->channel_num == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: 5g is not supported", __LINE__);
        return ERRCODE_FAIL;
    } else {
        fast_request_temp.channel = fast_request->channel_num;
    }

    fast_request_temp.psk_flag = 0;

    if (uapi_wifi_sta_fast_connect(&fast_request_temp) == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta_fast_connect failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

#if defined(_PRE_WLAN_FEATURE_WOW_OFFLOAD) || defined(CONFIG_WOW_OFFLOAD)
errcode_t wifi_set_wow_pattern(int32_t type, uint8_t index, int8_t *pattern)
{
    int32_t ret, len;
    ret = service_wifi_and_sta_check();
    if (ret != WIFI_SUCCESS) {
        service_error_log2(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check fail ret[%d]", __LINE__, ret);
        return ERRCODE_FAIL;
    }

    len = (int)strlen(pattern);
    // pattern的大小为1至64, index范围为0至3, 2奇偶校验
    if ((len > 64) || (index > 3) || (len % 2 != 0) || (type < WOW_PATTERN_ADD || type >= WOW_PATTERN_BUTT)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:parameter is invalid.", __LINE__);
        return ERRCODE_INVALID_PARAM;
    }

    ret = uapi_wifi_set_wow_pattern((td_u8)type, index, pattern);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_wow_pattern failed.", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_set_wow_sleep_mode(uint8_t en)
{
    int32_t ret;

    ret = service_wifi_and_sta_check();
    if (ret != WIFI_SUCCESS) {
        service_error_log2(SERVICE_ERROR, "Srv:%d:sta_check failed ret[%d]", __LINE__, ret);
        return ERRCODE_FAIL;
    }

    if (en != 0 && en != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:en is invalid.", __LINE__);
        return ERRCODE_INVALID_PARAM;
    }

    ret = uapi_wifi_set_wow_switch(en);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_wow_switch failed.", __LINE__);
        return ERRCODE_FAIL;
    }
}
#endif

errcode_t wifi_send_custom_pkt(const wifi_if_type_enum iftype, const uint8_t *data, uint32_t len)
{
    char *ifname = NULL;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_NOT_SUPPORT;
    }

    /* 待发送帧的内容长度为24到1400 */
    if (data == NULL || len < WIFI_SENDPKT_MIN_LEN || len > WIFI_SENDPKT_MAX_LEN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:parameter is invalid.", __LINE__);
        return ERRCODE_INVALID_PARAM;
    }

    if (iftype >= IFTYPES_BUTT) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERRCODE_INVALID_PARAM;
    }
    if (iftype == IFTYPE_STA && g_sta_enble_flag == 1) {
        ifname = g_sta_ifname;
    } else if (iftype == IFTYPE_AP && g_softap_enble_flag == 1) {
        ifname = g_hotspot_ifname;
    } else if (((iftype == IFTYPE_P2P_CLIENT) || (iftype == IFTYPE_P2P_GO) || (iftype == IFTYPE_P2P_DEVICE)) &&
        g_p2p_enable_flag == 1) {
        ifname = g_p2p_ifname;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:vap is not enabled or iftype is invalid", __LINE__);
        return ERRCODE_INVALID_PARAM;
    }

    if (uapi_wifi_send_custom_pkt(ifname, data, len) == 0) {
        return ERRCODE_SUCC;
    }
    service_error_log1(SERVICE_ERROR, "Srv:%d:uapi_wifi_send_custom_pkt failed.", __LINE__);
    return ERRCODE_FAIL;
}

#ifdef _PRE_WLAN_FEATURE_CSI
errcode_t wifi_set_csi_config(const int8_t *ifname, const csi_config_stru *config)
{
    if (config == NULL || (strcmp(ifname, g_sta_ifname) != 0 &&
            strcmp(ifname, g_hotspot_ifname) != 0 && strcmp(ifname, g_p2p_ifname) != 0)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:param is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    if (wifi_is_wifi_inited() == 0) {
        return ERROR_WIFI_NOT_STARTED;
    }
    uapi_wifi_csi_set_config(ifname, (const ext_csi_config *)config);
    return ERRCODE_SUCC;
}

errcode_t wifi_register_csi_report_cb(wifi_csi_data_cb data_cb)
{
    uapi_csi_register_data_report_cb(data_cb);
    return ERRCODE_SUCC;
}
errcode_t wifi_csi_start(void)
{
    int32_t ret;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }

    if (g_csi_enable_flag == 0) {
        ret = uapi_wifi_csi_start();
    } else {
        return ERRCODE_FAIL;
    }
    if (ret == EXT_WIFI_OK) {
        g_csi_enable_flag = 1;
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: start csi failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_csi_stop(void)
{
    int32_t ret;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_csi_stop();
    if (ret == EXT_WIFI_OK) {
        g_csi_enable_flag = 0;
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: stop csi failed", __LINE__);
        return ERRCODE_FAIL;
    }
}
#endif

errcode_t wifi_set_softap_config_advance(const softap_config_advance_stru *config)
{
    if (config == NULL || (config->gi != 0 && config->gi != 1)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    if (g_softap_enble_flag == 1 || uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:Softap is enabled or Wi-Fi is not initialized.", __LINE__);
        return ERRCODE_FAIL;
    }

    uint32_t ret = service_check_hotspot_config_advance(config);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config is invalid", __LINE__);
        return ret;
    }

    if (uapi_wifi_softap_set_beacon_period((int32_t)config->beacon_interval) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_beacon_period failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_softap_set_dtim_period((int32_t)config->dtim_period) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_dtim_period failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_softap_set_group_rekey((int32_t)config->group_rekey) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_group_rekey failed", __LINE__);
        return ERRCODE_FAIL;
    }

    /* 1 不隐藏 2 隐藏 */
    if (config->hidden_ssid_flag < 1 || config->hidden_ssid_flag > 2) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_hidden_ssid_flag failed", __LINE__);
        return ERRCODE_FAIL;
    }
    g_softap_hidden = (int32_t)config->hidden_ssid_flag;

    if (uapi_wifi_softap_set_shortgi((int32_t)config->gi) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_shortgi failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if (config->protocol_mode != 0) {
        ret = service_set_softap_protocol(config->protocol_mode);
        if (ret != ERRCODE_SUCC) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:set_protocol_mod failed", __LINE__);
            return ret;
        }
    }

    return ERRCODE_SUCC;
}
static errcode_t wifi_check_softap_config(const softap_config_stru *config)
{
    if (config == NULL || (config->wifi_psk_type != 0 && config->wifi_psk_type != 1)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_FAIL;
    }

    if (strlen(config->ssid) == 0 || strlen(config->ssid) > EXT_WIFI_MAX_SSID_LEN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:ssid is invalid", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static errcode_t wifi_set_softap_config(const softap_config_stru *config)
{
    ext_wifi_softap_config  conf = {0};
    char wep_key[WIFI_MAX_KEY_LEN];

    if ((memcpy_s(g_softap_config.ssid, WIFI_MAX_SSID_LEN, config->ssid, strlen(config->ssid) + 1)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if (wifi_is_need_psk(config->security_type) == 1 && config->security_type != WIFI_SEC_TYPE_WPA2PSK &&
        config->security_type != WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX && config->security_type != WIFI_SEC_TYPE_SAE &&
        config->security_type != WIFI_SEC_TYPE_WEP && config->security_type != WIFI_SEC_TYPE_WEP_OPEN &&
#ifdef CONFIG_OWE
        config->security_type != WIFI_SEC_TYPE_OWE &&
#endif /* CONFIG_OWE */
        config->security_type != WIFI_SEC_TYPE_WPA3_WPA2_PSK_MIX) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:security_type does not support", __LINE__);
        return ERRCODE_FAIL;
    }

    uint32_t ret = service_check_pre_shared_key(config->pre_shared_key, config->security_type,
        (int8_t)config->wifi_psk_type, wep_key);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: config->pre_shared_key is invalid", __LINE__);
        return ret;
    }

    /* WEP: 密钥赋值 */
    if ((config->security_type == WIFI_SEC_TYPE_WEP || config->security_type == WIFI_SEC_TYPE_WEP_OPEN) &&
        config->wifi_psk_type == 1) {
        if ((memcpy_s(g_softap_config.pre_shared_key, WIFI_MAX_KEY_LEN, wep_key, strlen(wep_key) + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:hex key memcpy_s failed", __LINE__);
            return ERRCODE_FAIL;
        }
    } else {
        if ((memcpy_s(g_softap_config.pre_shared_key, WIFI_MAX_KEY_LEN,
            config->pre_shared_key, strlen(config->pre_shared_key) + 1)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:ascii key memcpy_s failed", __LINE__);
            return ERRCODE_FAIL;
        }
    }

    g_softap_config.security_type = config->security_type;
    g_softap_config.wifi_psk_type = config->wifi_psk_type;

    // 2.4g信道号最大14
    if (config->channel_num < 0 || config->channel_num > 14) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:channel_num is invalid", __LINE__);
        return ERRCODE_FAIL;
    }
    g_softap_config.channel_num = config->channel_num;

    return ERRCODE_SUCC;
}

static errcode_t wifi_get_enable_status(void)
{
    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not init", __LINE__);
        return ERRCODE_FAIL;
    }

    if (g_softap_enble_flag == 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:SoftAP has been enabled", __LINE__);
        return ERRCODE_FAIL;
    }

    if (g_p2p_enable_flag == 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:P2P has been enabled", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_softap_enable(const softap_config_stru *config)
{
    ext_wifi_softap_config conf = {0};

    if (wifi_check_softap_config(config) != ERRCODE_SUCC || wifi_set_softap_config(config) != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config set failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if (wifi_get_enable_status() != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if ((memcpy_s(conf.ssid, EXT_WIFI_MAX_SSID_LEN + 1, g_softap_config.ssid, strlen(g_softap_config.ssid) + 1)) !=
        EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if ((memcpy_s(conf.key, EXT_WIFI_AP_KEY_LEN + 1,
        g_softap_config.pre_shared_key, strlen(g_softap_config.pre_shared_key) + 1)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }

    conf.authmode = g_softap_config.security_type;
    conf.channel_num = (uint8_t)g_softap_config.channel_num;

    conf.ssid_hidden = (g_softap_hidden == 1) ? 0 : 1;
    conf.pairwise = EXT_WIFI_PAIRWISE_TKIP_AES_MIX;
    if (g_softap_config.security_type ==  WIFI_SEC_TYPE_SAE ||
#ifdef CONFIG_OWE
        g_softap_config.security_type == WIFI_SEC_TYPE_OWE ||
#endif /* CONFIG_OWE */
        g_softap_config.security_type == WIFI_SEC_TYPE_WPA3_WPA2_PSK_MIX) {
        conf.pairwise = EXT_WIFI_PAIRWISE_AES;
    }

    int32_t len = WIFI_IFNAME_MAX_SIZE + 1;
    if (register_callback() != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: register callback fail", __LINE__);
        return ERRCODE_FAIL;
    }
    if (uapi_wifi_softap_start(&conf, g_hotspot_ifname, &len) == EXT_WIFI_OK) {
        g_softap_enble_flag = 1;
        return ERRCODE_SUCC;
    }
    service_error_log1(SERVICE_ERROR, "Srv:%d:softap_start failed", __LINE__);
    return ERRCODE_FAIL;
}

errcode_t wifi_softap_disable(void)
{
    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_FAIL;
    }

    if (g_softap_enble_flag == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:SoftAP has been disabled", __LINE__);
        return ERRCODE_FAIL;
    }

#if defined(LWIP_DHCPS) && LWIP_DHCPS
    /* 关闭SoftAP时，自动关闭DHCPS */
    struct netif *netif = netifapi_netif_find_by_name(g_hotspot_ifname);
    if (netifapi_dhcps_stop(netif) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:DHCPS stop failed", __LINE__);
        return ERRCODE_FAIL;
    }
#endif

    if (uapi_wifi_softap_stop() == EXT_WIFI_OK) {
        g_softap_enble_flag = 0;
        memset_s(g_hotspot_ifname, WIFI_IFNAME_MAX_SIZE + 1, 0, WIFI_IFNAME_MAX_SIZE + 1);
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:softap_stop failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_softap_get_sta_list(wifi_sta_info_stru *result, uint32_t *size)
{
    ext_wifi_ap_sta_info *sta_list;
    uint32_t sta_num = WIFI_DEFAULT_MAX_NUM_STA;

    if (result == NULL || size == NULL || (*size) == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    sta_list = (ext_wifi_ap_sta_info *)malloc(sizeof(ext_wifi_ap_sta_info) * sta_num);
    if (sta_list == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: alloc fail", __LINE__);
        return ERRCODE_FAIL;
    }

    memset_s(sta_list, sizeof(ext_wifi_ap_sta_info) * sta_num, 0, sizeof(ext_wifi_ap_sta_info) * sta_num);

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        free(sta_list);
        return ERRCODE_FAIL;
    }

    if (g_softap_enble_flag == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:SoftAP has been disabled", __LINE__);
        free(sta_list);
        return ERRCODE_FAIL;
    }
    /* 读出ap下挂sta数量及mac信息, 注意此接口入参的sta_num是sta数量的最大值 */
    int ret = uapi_wifi_softap_get_connected_sta(sta_list, &sta_num);

    if (sta_num < *size) {
        service_error_log2(SERVICE_ERROR, "Srv:%d:sta_num[%d]!!", __LINE__, sta_num);
        *size = sta_num;
    }

    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:get_connected_sta failed", __LINE__);
        free(sta_list);
        return ERRCODE_FAIL;
    }

    for (int i = 0; i < *size; i++) {
        if ((memcpy_s(result[i].mac_addr, WIFI_MAC_LEN, sta_list[i].mac, WIFI_MAC_LEN)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            free(sta_list);
            return ERRCODE_FAIL;
        }
        ret = uapi_wifi_ap_get_sta_info(result[i].mac_addr, &result[i].rssi, &result[i].best_rate);
        if (ret != EXT_WIFI_OK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:get_connected_sta info failed", __LINE__);
            free(sta_list);
            return ERRCODE_FAIL;
        }
    }
    free(sta_list);
    return ERRCODE_SUCC;
}

int32_t wifi_is_softap_enabled(void)
{
    if (uapi_wifi_get_init_status() != 1) {
        return 0;
    }

    if (g_softap_enble_flag == 0) {
        return 0;
    } else {
        return 1;
    }
}

errcode_t wifi_get_softap_config(softap_config_stru *result)
{
    if ((memcpy_s(result, sizeof(softap_config_stru), &g_softap_config, sizeof(softap_config_stru))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_get_softap_config_advance(softap_config_advance_stru *result)
{
    if ((memcpy_s(result, sizeof(softap_config_advance_stru),
        &g_softap_advance_config, sizeof(softap_config_advance_stru))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_softap_deauth_sta(const uint8_t *mac, int32_t mac_len)
{
    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_FAIL;
    }

    if (g_softap_enble_flag != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:SoftAP does not enable", __LINE__);
        return ERRCODE_FAIL;
    }

    if (mac == NULL || mac_len != WIFI_MAC_LEN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    int ret = uapi_wifi_softap_deauth_sta(mac, WIFI_MAC_LEN);
    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_set_app_ie(wifi_if_type_enum iftype, ie_index_enmu ie_index, uint8_t frame_type_bitmap,
                          const uint8_t *ie, uint16_t ie_len)
{
    uint32_t ret;
    ext_wifi_iftype iftype_tmp;
    if (iftype >= IFTYPES_BUTT || ie_index >= IE_BUTT || ie_len > EXT_WIFI_USR_IE_MAX_SIZE || frame_type_bitmap == 0 ||
        iftype == IFTYPE_P2P_CLIENT || ie == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_FAIL;
    }

    // bit0表示在beacon帧中插入；bit1表示在probe request帧中插入；bit2表示在probe response帧中插入；bit3~7 保留
    if ((frame_type_bitmap & 0xF8) > 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:frame_type_bitmap is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    ret = service_ie_frame_check(iftype, frame_type_bitmap, &iftype_tmp);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_ie_frame_check failed", __LINE__);
        return ERRCODE_FAIL;
    }

    // ie的索引1,2;数据段的长度为总长度减2
    if ((ie[0] != 0xdd) || (ie[1] != (ie_len - 2))) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:Ie setting error", __LINE__);
        return ERRCODE_FAIL;
    }

    int32_t res = uapi_wifi_add_usr_app_ie(iftype_tmp, ie_index, frame_type_bitmap, ie, ie_len);
    if (res == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_del_app_ie(wifi_if_type_enum iftype, ie_index_enmu ie_index, uint8_t frame_type_bitmap)
{
    uint32_t ret;
    ext_wifi_iftype iftype_tmp;
    if (iftype >= IFTYPES_BUTT || ie_index >= IE_BUTT ||  frame_type_bitmap == 0 || iftype == IFTYPE_P2P_CLIENT) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_FAIL;
    }

    ret = service_ie_frame_check(iftype, frame_type_bitmap, &iftype_tmp);
    if (ret != ERRCODE_SUCC) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:service_ie_frame_check failed", __LINE__);
        return ERRCODE_FAIL;
    }

    return uapi_wifi_delete_usr_app_ie(iftype_tmp, ie_index, frame_type_bitmap) != EXT_WIFI_OK ?
        ERRCODE_FAIL : ERRCODE_SUCC;
}

errcode_t wifi_set_country_code(const int8_t* country_code, uint8_t len)
{
    int ret;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_FAIL;
    }

    if (country_code == NULL || len != WIFI_MAC_CONTRY_CODE_LEN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    ret = uapi_wifi_set_country(country_code, len);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_country failed", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_get_country_code(int8_t *country_code, uint8_t *len)
{
    int ret;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_FAIL;
    }

    if (country_code == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:countryCode is NULL", __LINE__);
        return ERRCODE_FAIL;
    }

    ret = uapi_wifi_get_country(country_code, len);
    if (ret != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:get_country failed", __LINE__);
        return ERRCODE_FAIL;
    }
    service_error_log2(SERVICE_ERROR, "Srv:GetCountryCode[%s][%d]", country_code, *len);
    return ERRCODE_SUCC;
}

OSAL_STATIC int32_t service_freqs_to_chan(int32_t freqs)
{
    int32_t chan;
    int32_t is_freqs = service_is_freqs(freqs);
    if (is_freqs == 0) {
        return 0;
    } else if (is_freqs == 2484) {                                 // 14信道中心频率2484
        chan = 14;                                                 // 14信道
    } else {
        chan = ((freqs - SERVICE_BASE_FREQS) / SERVICE_FREQS_INTERVAL) + 1;
    }
    return chan;
}

#ifdef CONFIG_P2P_SUPPORT
static void p2p_find_peer_by_bssid(ext_wifi_p2p_peer_info *peer,
    const unsigned char *bssid, unsigned long bssid_len, int peer_num, int *peer_id)
{
    int peer_idx;
    for (peer_idx = 0; peer_idx < peer_num; peer_idx++) {
        if (memcmp(bssid, peer[peer_idx].device_addr, bssid_len) == 0) {
            *peer_id = peer_idx;
            service_error_log2(SERVICE_ERROR, "Srv:%d:group_cap %d", __LINE__, peer[peer_idx].group_capab);
            break;
        }
    }
    if (peer_idx == *peer_id) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:can not find same peer", __LINE__);
    }
    *peer_id = peer_idx;
}


errcode_t wifi_p2p_enable(void)
{
    int ret;
    int len = WIFI_IFNAME_MAX_SIZE + 1;

    if (wifi_is_wifi_inited() == 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wifi not init", __LINE__);
        return ERRCODE_FAIL;
    }

    if (g_p2p_enable_flag == 0) {
        if (g_softap_enble_flag == 1) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: SoftAP has been enabled", __LINE__);
            return ERRCODE_FAIL;
        }
        if (register_callback() != 0) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: register callback fail", __LINE__);
            return ERRCODE_FAIL;
        }
        ret = uapi_wifi_p2p_start(g_p2p_ifname, &len);
    } else {
        return ERRCODE_FAIL;
    }

    if (ret == EXT_WIFI_OK) {
        ret = uapi_wifi_set_peer_connect_choose(EXT_WIFI_P2P_PEER_CONNECT_MANUAL);
        if (ret != EXT_WIFI_OK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: set manual connect fail", __LINE__);
            uapi_wifi_p2p_stop();
            return ERRCODE_FAIL;
        }
        g_p2p_enable_flag = 1;
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa enablep2p fail", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_p2p_disable(void)
{
    int ret;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }

    /* 关闭P2P时，根据GC模式下自动关闭DHCP、GO模式下自动关闭DHCPS、不管p2p_device情况 */
    struct netif *netif = netifapi_netif_find_by_name(g_p2p_ifname);
    if (g_p2p_isgo && netifapi_dhcps_stop(netif) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:DHCPS stop failed", __LINE__);
        return ERRCODE_FAIL;
    } else if (!g_p2p_isgo && netifapi_dhcp_stop(netif) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:DHCP stop failed", __LINE__);
        return ERRCODE_FAIL;
    }

    if (g_p2p_enable_flag == 1) {
        ret = uapi_wifi_p2p_stop();
    } else {
        return ERRCODE_FAIL;
    }
    if (ret == EXT_WIFI_OK) {
        if (memset_s(g_p2p_ifname, sizeof(g_p2p_ifname), 0, sizeof(g_p2p_ifname)) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: memset_s failed", __LINE__);
            return ERRCODE_FAIL;
        }
        g_p2p_enable_flag = 0;
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa disablep2p fail", __LINE__);
        return ERRCODE_FAIL;
    }
}

int32_t wifi_p2p_is_enabled(void)
{
    return g_p2p_enable_flag;
}

errcode_t wifi_p2p_find(int32_t sec)
{
    int ret;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }
    if (sec == 0) {
        ret = uapi_wifi_p2p_find(SERVICE_P2P_MAX_FIND_TIME);
    } else if (sec >= SERVICE_P2P_MIN_FIND_TIME && sec <= SERVICE_P2P_MAX_FIND_TIME) {
        ret = uapi_wifi_p2p_find(sec);
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: input arg is inlegal", __LINE__);
        return ERRCODE_FAIL;
    }

    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa findp2p fail", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_p2p_stop_find(void)
{
    int ret;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_p2p_stop_find();
    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa stop p2p find fail", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_p2p_connect_cancel(void)
{
    int ret;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_p2p_cancel();
    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa cancel p2p connect fail", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_p2p_listen(uint32_t period, uint32_t interval)
{
    int ret;
    if (wifi_is_wifi_inited() == 0) {
        return ERROR_WIFI_NOT_STARTED;
    }
    if (g_p2p_enable_flag == 0) {
        return ERROR_WIFI_IFACE_INVALID;
    }
    if (period > SERVICE_MAX_PERIOD_INTERVAL || interval > SERVICE_MAX_PERIOD_INTERVAL || period > interval ||
        (period == 0 && interval > 0) || (period > 0 && interval == 0) || period < 0 || interval < 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:period or interval is error", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    ret = uapi_wifi_p2p_listen(period, interval);
    if (ret == EXT_WIFI_OK) {
        return WIFI_SUCCESS;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p listen fail", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
}

errcode_t wifi_p2p_get_peers_info(p2p_device_stru *dev_list, uint32_t *dev_num)
{
    int ret;
    int peer_num;
    unsigned int num = 0;
    ext_wifi_p2p_peer_info *peers_res = NULL;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }
    if (dev_list == NULL || dev_num == NULL || *dev_num > SERVICE_P2P_MAX_FIND_NUM) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: input is invalid", __LINE__);
        return ERRCODE_FAIL;
    }
    peers_res = (ext_wifi_p2p_peer_info *)malloc(sizeof(ext_wifi_p2p_peer_info) * (*dev_num));
    if (peers_res == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: alloc fail", __LINE__);
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_p2p_peers(peers_res, dev_num);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa stop p2p find fail", __LINE__);
        free(peers_res);
        return ERRCODE_FAIL;
    }
    for (peer_num = 0; peer_num < *dev_num; peer_num++) {
        /* 将没有config_method的屏蔽，不进行显示 */
        if ((get_wps_mode(peers_res[peer_num].config_methods) & 0xF) != 0) {
            if ((memcpy_s(dev_list[num].name, sizeof(dev_list[num].name), peers_res[peer_num].device_name,
                sizeof(peers_res[peer_num].device_name))) != EOK) {
                service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
                free(peers_res);
                return ERRCODE_FAIL;
            }
            if ((memcpy_s(dev_list[num].bssid, sizeof(dev_list[num].bssid), peers_res[peer_num].device_addr,
                sizeof(peers_res[peer_num].device_addr))) != EOK) {
                service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
                free(peers_res);
                return ERRCODE_FAIL;
            }
            dev_list[num].wps_method = get_wps_mode(peers_res[peer_num].config_methods);
            dev_list[num].is_go = peers_res[peer_num].group_capab & 0x1;
            dev_list[num].enable_add_group = !(peers_res[peer_num].group_capab & 0x4);
            num++;
        }
    }
    *dev_num = num;
    free(peers_res);
    return ERRCODE_SUCC;
}

static int wifi_p2p_connect_etc(const char *bssid, int bssid_len, int join_group, int persistent, int go_intent)
{
    ext_wifi_p2p_connect connect;
    ext_wifi_p2p_user_conf user_config = {0};

    if ((memcpy_s(connect.peer_addr, sizeof(connect.peer_addr), bssid, bssid_len)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    connect.join_group = join_group;
    connect.persistent = persistent;
    connect.go_intent = go_intent;

    if (uapi_wifi_p2p_get_user_config(&user_config) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p set user conf fail", __LINE__);
        return ERRCODE_FAIL;
    }

    connect.wps_method = user_config.wps_method;
    /* auto connect, put pin to 0 */
    if ((memset_s(connect.pin, sizeof(connect.pin), 0, sizeof(connect.pin))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memset_s pin failed", __LINE__);
        return ERRCODE_FAIL;
    }
    service_error_log4(SERVICE_ERROR, "Srv:%d wifi_p2p_connect_etc join = %d wps_method=%d go_intent = %d",
        __LINE__, connect.join_group, connect.wps_method, connect.go_intent);
    return uapi_wifi_p2p_connect(connect);
}

static uint32_t wifi_p2p_conn_check_config(const p2p_config_stru *p2p_config, uint32_t line_num)
{
    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }

    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }

    if (p2p_config == NULL || p2p_config->go_intent > SERVICE_P2P_MAX_INTENT ||
        p2p_config->go_intent < 0 || p2p_config->persistent > 1 || p2p_config->persistent < 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: input is invalid", line_num);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_p2p_connect(const p2p_config_stru *p2p_config)
{
    int32_t ret;
    int32_t peer_idx = 0;
    ext_wifi_p2p_status_info status;
    int32_t peer_num = SERVICE_P2P_MAX_FIND_NUM;
    ext_wifi_p2p_peer_info peer[SERVICE_P2P_MAX_FIND_NUM];

    if (wifi_p2p_conn_check_config(p2p_config, __LINE__) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    ret = uapi_wifi_p2p_status(&status);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa get p2p status fail", __LINE__);
        return ERRCODE_FAIL;
    }
    if (status.mode == EXT_WIFI_P2P_MODE_GC || status.mode == EXT_WIFI_P2P_MODE_GO) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:p2p will invite", __LINE__);
        ret = uapi_wifi_p2p_invite(-1, p2p_config->bssid, sizeof(p2p_config->bssid));
    } else if (status.mode == EXT_WIFI_P2P_DEVICE_ONLY) {
        uapi_wifi_p2p_peers(peer, &peer_num);
        p2p_find_peer_by_bssid(peer, p2p_config->bssid, WIFI_MAC_LEN, peer_num, &peer_idx);
        if ((peer_idx >= peer_num) || ((peer[peer_idx].group_capab & 0x1) == 0)) {
            ret = wifi_p2p_connect_etc(p2p_config->bssid, WIFI_MAC_LEN, 0, p2p_config->persistent,
                p2p_config->go_intent);
            if (ret != EXT_WIFI_OK) {
                service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p connect fail", __LINE__);
                return ERRCODE_FAIL;
            } else {
                return ERRCODE_SUCC;
            }
        }
        if (((peer[peer_idx].group_capab & 0x1) == 0x1) && ((peer[peer_idx].group_capab & 0x4) == 0x4)) {
            service_error_log1(SERVICE_ERROR, "Srv:%d: can not join this group group limit", __LINE__);
            return ERRCODE_FAIL;
        }
        ret = wifi_p2p_connect_etc(p2p_config->bssid, WIFI_MAC_LEN, 1, p2p_config->persistent, p2p_config->go_intent);
    }
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p connect fail", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static int32_t wifi_p2p_wps_method_check(const p2p_config_stru *p2p_config)
{
    ext_wifi_p2p_user_conf user_config = {0};
    if (uapi_wifi_p2p_get_user_config(&user_config) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p get user conf fail", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    /* 如果没有pincode，表示使用pbc的方式连接，如果当前设置过，并且设置的不是pbc的方式，直接返回失败 */
    if ((p2p_config->wps_method == WPS_PBC) && (user_config.wps_method == WPS_PIN_DISPLAY)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wps method not match", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    /* 如果有pincode，表示使用pin的方式连接，如果当前设置过，并且设置的不是pbc的方式，直接返回失败 */
    if ((p2p_config->wps_method == WPS_PIN_DISPLAY) && (user_config.wps_method == WPS_PBC)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wps method not match", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    return EXT_WIFI_OK;
}

errcode_t wifi_p2p_connect_accept(const p2p_config_stru *p2p_config, int32_t assoc)
{
    ext_wifi_p2p_status_info status;
    ext_wifi_p2p_connect connect_param = {0};
    ext_wifi_p2p_peer_info peer[SERVICE_P2P_MAX_FIND_NUM];
    int32_t peer_num = SERVICE_P2P_MAX_FIND_NUM;
    int32_t peer_idx = 0;

    if (wifi_p2p_conn_check_config(p2p_config, __LINE__) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    /* 对wps_method进行校验 */
    if (wifi_p2p_wps_method_check(p2p_config) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wifi_p2p_wps_method_check error", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_p2p_status(&status) != EXT_WIFI_OK || status.mode != EXT_WIFI_P2P_DEVICE_ONLY) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p status error", __LINE__);
        return ERRCODE_FAIL;
    }
    if ((memcpy_s(connect_param.peer_addr, sizeof(connect_param.peer_addr), p2p_config->bssid,
        sizeof(p2p_config->bssid))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:addr memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    if ((memcpy_s(connect_param.pin, sizeof(connect_param.pin), p2p_config->pin, sizeof(p2p_config->pin))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:pin memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    connect_param.persistent = p2p_config->persistent;
    connect_param.wps_method = p2p_config->wps_method;
    connect_param.go_intent = p2p_config->go_intent;
    uapi_wifi_p2p_peers(peer, &peer_num);
    p2p_find_peer_by_bssid(peer, p2p_config->bssid, WIFI_MAC_LEN, peer_num, &peer_idx);
    if ((peer_idx >= peer_num) || ((peer[peer_idx].group_capab & 0x1) == 0)) {
        connect_param.join_group = 0;
        return uapi_wifi_p2p_user_accept(connect_param, assoc) != EXT_WIFI_OK ? ERRCODE_FAIL : ERRCODE_SUCC;
    }
    connect_param.join_group = 1;
    return uapi_wifi_p2p_user_accept(connect_param, assoc) != EXT_WIFI_OK ? ERRCODE_FAIL : ERRCODE_SUCC;
}

errcode_t wifi_p2p_disconnect(void)
{
    int32_t ret;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_p2p_cancel();
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p cancel connect fail", __LINE__);
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_p2p_remove_group();
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p remove group fail", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_p2p_get_connect_info(p2p_status_info_stru *status)
{
    int32_t ret;
    ext_wifi_p2p_status_info p2p_status;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }
    if (status == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: input is invalid", __LINE__);
        return ERRCODE_FAIL;
    }
    if (memset_s(status, sizeof(p2p_status_info_stru), 0, sizeof(p2p_status_info_stru)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: memset_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_p2p_status(&p2p_status);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p get status fail", __LINE__);
        return ERRCODE_FAIL;
    }
    status->mode = p2p_status.mode;
    if (status->mode == P2P_MODE_DEVICE_ONLY) {
        return ERRCODE_SUCC;
    } else {
        status->wpa_state = p2p_status.p2p_state;
    }
    status->operation_channel = service_freqs_to_chan(p2p_status.op_freq);

    if ((memcpy_s(status->group_ssid, sizeof(status->group_ssid), p2p_status.ssid, sizeof(p2p_status.ssid))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    if ((memcpy_s(status->group_bssid, sizeof(status->group_bssid),
        p2p_status.bssid, sizeof(p2p_status.bssid))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_p2p_go_get_gc_info(p2p_client_info_stru *client_list, uint32_t *client_num)
{
    int32_t ret;
    int32_t gc_idx;
    ext_wifi_p2p_client_info *connected_client = NULL;

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }
    if (client_list == NULL || client_num == NULL || *client_num > SERVICE_P2P_MAX_GC_NUM) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: input is invalid", __LINE__);
        return ERRCODE_FAIL;
    }
    connected_client = (ext_wifi_p2p_client_info *)malloc(sizeof(ext_wifi_p2p_client_info) * (*client_num));
    if (connected_client == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: alloc fail", __LINE__);
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_p2p_get_connected_client(connected_client, client_num);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: p2p get gc fail", __LINE__);
        goto SERVICE_P2P_GET_GC_FAIL;
    }
    for (gc_idx = 0; gc_idx < *client_num; gc_idx++) {
        if ((memcpy_s(client_list[gc_idx].gc_bssid, sizeof(client_list[gc_idx].gc_bssid), connected_client[gc_idx].mac,
            sizeof(connected_client[gc_idx].mac))) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            goto SERVICE_P2P_GET_GC_FAIL;
        }
        if ((memcpy_s(client_list[gc_idx].gc_device_bssid, sizeof(client_list[gc_idx].gc_device_bssid),
            connected_client[gc_idx].dev_addr, sizeof(connected_client[gc_idx].dev_addr))) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            goto SERVICE_P2P_GET_GC_FAIL;
        }
        if ((memcpy_s(client_list[gc_idx].gc_device_name, sizeof(client_list[gc_idx].gc_device_name),
            connected_client[gc_idx].device_name, sizeof(connected_client[gc_idx].device_name))) != EOK) {
            service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
            goto SERVICE_P2P_GET_GC_FAIL;
        }
    }
    free(connected_client);
    return ERRCODE_SUCC;
SERVICE_P2P_GET_GC_FAIL:
    if (connected_client != NULL) {
        free(connected_client);
    }
    return ERRCODE_FAIL;
}

errcode_t wifi_p2p_set_device_config(const p2p_device_config_stru *p2p_dev_set_info)
{
    int32_t ret;
    ext_wifi_p2p_user_conf user_config = {0};

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }
    if (p2p_dev_set_info == NULL ||
        (p2p_dev_set_info->wps_method != WPS_PBC && p2p_dev_set_info->wps_method != WPS_PIN_DISPLAY)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: input is invalid", __LINE__);
        return ERRCODE_FAIL;
    }
    user_config.go_intent = SERVICE_P2P_DEFAULT_INTENT;
    user_config.oper_channel = p2p_dev_set_info->oper_channel;
    user_config.listen_channel = p2p_dev_set_info->listen_channel;
    user_config.wps_method = p2p_dev_set_info->wps_method;
    if ((memcpy_s(user_config.device_name, sizeof(user_config.device_name),
        p2p_dev_set_info->dev_name, sizeof(p2p_dev_set_info->dev_name))) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_p2p_user_config(user_config);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p set user conf fail", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_p2p_get_device_config(p2p_device_config_stru *p2p_dev_set_info)
{
    ext_wifi_p2p_user_conf user_config = {0};

    if (wifi_is_wifi_inited() == 0) {
        return ERRCODE_FAIL;
    }
    if (g_p2p_enable_flag == 0) {
        return ERRCODE_FAIL;
    }

    if (p2p_dev_set_info == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: input is invalid", __LINE__);
        return ERRCODE_FAIL;
    }

    if (uapi_wifi_p2p_get_user_config(&user_config) != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: wpa p2p get user conf fail", __LINE__);
        return ERRCODE_FAIL;
    }

    if ((memcpy_s(p2p_dev_set_info->dev_name, WPS_DEV_NAME_MAX_LEN + 1,
        &user_config.device_name, WPS_DEV_NAME_MAX_LEN + 1)) != EOK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:memcpy_s failed", __LINE__);
        return ERRCODE_FAIL;
    }
    p2p_dev_set_info->listen_channel = user_config.listen_channel;
    p2p_dev_set_info->oper_channel = user_config.oper_channel;
    p2p_dev_set_info->wps_method = user_config.wps_method;

    return ERRCODE_SUCC;
}
#endif /* end of CONFIG_P2P_SUPPORT */

errcode_t wifi_set_pkt_retry_policy(uint8_t type, uint8_t limit)
{
    int32_t ret;

    if (type >= EXT_WIFI_RETRY_FRAME_BUTT) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:parameter is invalid.", __LINE__);
        return ERRCODE_FAIL;
    }
    ret = uapi_wifi_set_pkt_retry_policy(type, limit);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: set retry params fail", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_reset_mac_phy(void)
{
    int32_t ret;

    ret = uapi_wifi_mac_phy_reset();
    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:reset_mac_phy failed.", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_sta_set_pm(uint8_t ps_switch)
{
    int32_t ret;

    ret = service_wifi_and_sta_check();
    if (ret != WIFI_SUCCESS) {
        service_error_log2(SERVICE_ERROR, "Srv:%d:service_wifi_and_sta_check failed ret[%d]", __LINE__, ret);
        return ERRCODE_FAIL;
    }

    ret = uapi_wifi_set_pm_switch(ps_switch, 0);
    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_ps_switch failed.", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_sta_set_pm_param(uint8_t pm_timeout, uint8_t pm_timer_cnt, uint8_t bcn_timeout,
    uint8_t mcast_timeout, uint16_t sleep_time)
{
    int32_t ret;

    ret = service_wifi_and_sta_check();
    if (ret != WIFI_SUCCESS) {
        service_error_log2(SERVICE_ERROR, "Srv:%d:sta check failed ret[%d]", __LINE__, ret);
        return ERRCODE_FAIL;
    }

    ret = uapi_wifi_sta_set_pm_param(pm_timeout, pm_timer_cnt, bcn_timeout, mcast_timeout, sleep_time);
    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_ps_param failed.", __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t wifi_set_psd_mode(ext_psd_option_param *psd_option)
{
    int32_t ret;
    /* cycle 取值 100 ~ 1000 单位ms dur 取值 1 ~ 65535(u16不用重复校验) 单位min */
    if ((psd_option->cycle < 100 || psd_option->cycle > 1000 || psd_option->duration < 1) &&
        (psd_option->enable != 0)) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: param invalid", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }

    ret = uapi_wifi_set_psd_enable((psd_option_param *)psd_option);
    if (ret != WIFI_SUCCESS) {
        service_error_log2(SERVICE_ERROR, "Srv:%d: fail ret [%d]", __LINE__, ret);
        return ERROR_WIFI_UNKNOWN;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_set_psd_cb(wifi_psd_cb data_cb)
{
    int32_t ret;

    ret = uapi_wifi_set_psd_cb(data_cb);
    if (ret != WIFI_SUCCESS) {
        service_error_log2(SERVICE_ERROR, "Srv:%d: fail ret [%d]", __LINE__, ret);
        return ERROR_WIFI_UNKNOWN;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_set_rts_mode(uint8_t mode, uint16_t pkt_length)
{
    uint32_t ret;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERROR_WIFI_NOT_STARTED;
    }

    ret = uapi_wifi_set_rts_threshold(mode, pkt_length);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_rts_thres failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
}

errcode_t wifi_set_cca_threshold(uint8_t mode, int8_t threshold)
{
    uint32_t ret;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERROR_WIFI_NOT_STARTED;
    }

    ret = uapi_wifi_set_cca_threshold(mode, threshold);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_cca failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
}
errcode_t wifi_set_tpc_mode(uint32_t tpc_value)
{
    uint32_t ret;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERROR_WIFI_NOT_STARTED;
    }

    ret = uapi_wifi_set_tpc(tpc_value);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_tpc failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
}
errcode_t wifi_set_fixed_tx_rate(unsigned char auto_rate, alg_param_stru *alg_param)
{
    uint32_t ret;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERROR_WIFI_NOT_STARTED;
    }

    if (alg_param == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    ret = uapi_wifi_set_phy_txrate(auto_rate, (ext_alg_param_stru *)alg_param);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_txrate failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
}

#ifdef _PRE_WLAN_FEATURE_INTRF_MODE
errcode_t wifi_set_intrf_mode(const char *ifname, uint8_t enable, uint16_t flag)
{
    uint32_t ret;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERROR_WIFI_NOT_STARTED;
    }

    if (ifname == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    ret = uapi_wifi_enable_intrf_mode(ifname, enable, flag);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_intrfmode failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
}
#endif
errcode_t wifi_get_negotiated_rate(const uint8_t *mac, int32_t mac_len, uint32_t *tx_best_rate)
{
    int32_t ret;
    int8_t addr_txt[EXT_WIFI_TXT_ADDR_LEN + 2] = { 0 };

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERROR_WIFI_NOT_STARTED;
    }

    if (mac == NULL) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:para is invalid", __LINE__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    /* 从第2个开始 */
    ret = snprintf_s(addr_txt + 1, sizeof(addr_txt) - 1, sizeof(addr_txt) - 2, MACSTR, mac2str(mac));
    if (ret < 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d snprintf_s faild", __LINE__);
        return EXT_WIFI_FAIL;
    }
    ret = uapi_wifi_get_tx_params(addr_txt, sizeof(addr_txt), tx_best_rate);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:GetWifiNegotiateSpeed failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
}

errcode_t wifi_set_linkloss_config(linkloss_paras_stru *linkloss_paras)
{
    int32_t ret;
    uint8_t proberequest_ratio = 0;
    uint16_t linkloss_threshold = 0;
    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_FAIL;
    }
    proberequest_ratio = linkloss_paras->send_probe_request_ratio;
    linkloss_threshold = linkloss_paras->linkloss_threshold;
    if (proberequest_ratio < SERVICE_LINKLOSS_PROBEREQ_RATIO_MIN ||
        proberequest_ratio > SERVICE_LINKLOSS_PROBEREQ_RATIO_MAX ||
        linkloss_threshold < SERVICE_LINKLOSS_THRESHOLD_MIN || linkloss_threshold > SERVICE_LINKLOSS_THRESHOLD_MAX) {
        service_error_log3(SERVICE_ERROR,
            "Srv:%d:paraments out of rangle Ratio[%d] threshold[%d]", __LINE__,
            proberequest_ratio, linkloss_threshold);
    }
    ret = uapi_wifi_set_linkloss_paras(proberequest_ratio, linkloss_threshold);
    if (ret == EXT_WIFI_OK) {
        return ERRCODE_SUCC;
    } else {
        service_error_log1(SERVICE_ERROR, "Srv:%d:set_ps_switch failed.", __LINE__);
        return ERRCODE_FAIL;
    }
}

#ifdef CONFIG_RST_SUPPORT
errcode_t plat_set_pm_mode(int32_t pm_switch)
{
    int ret;
    ret = plat_pm_func_enable_switch(pm_switch);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log2(SERVICE_ERROR, "Srv:%d:set pm mode fail, ret=[%d].", __LINE__, ret);
        return ERRCODE_FAIL;
    }
}

errcode_t plat_reset_board(void)
{
    uint32_t ret;
    ret = wifi_deinit(); // 去初始化wifi
    if (ret != EXT_WIFI_OK) {
        service_error_log2(SERVICE_ERROR, "Srv:%d:WifiUninit fail, ret=[%d].", __LINE__, ret);
        return ERRCODE_FAIL;
    }
    ret = pm_board_power_reset(); // device复位
    if (ret != EXT_WIFI_OK) {
        service_error_log2(SERVICE_ERROR, "Srv:%d:pm_board_power_reset fail, ret=[%d].", __LINE__, ret);
        return ERRCODE_FAIL;
    }
    ret = plat_reset_reinit_etc(); // 重新初始化平台
    if (ret != EXT_WIFI_OK) {
        service_error_log2(SERVICE_ERROR, "Srv:%d:plat_reset_reinit_etc fail, ret=[%d].", __LINE__, ret);
        return ERRCODE_FAIL;
    }
#ifdef _PRE_WLAN_FEATURE_CENTRALIZE
#ifdef BOARD_FPGA_WIFI
    // 共核编译的直接在host初始化abb 其他情况在device初始化中完成
    hh503_abb5_init_pre();
    mpw0_poweron();
#endif
#endif
    ret = wifi_init(); // 初始化wifi
    if (ret != EXT_WIFI_OK) {
        service_error_log2(SERVICE_ERROR, "Srv:%d:WifiInit fail, ret=[%d].", __LINE__, ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif

static errcode_t plat_get_temperature(int8_t *temperature)
{
    if (temperature == NULL) {
        return ERROR_WIFI_INVALID_ARGS;
    }
    if (uapi_tsensor_get_current_temp(temperature) != WIFI_SUCCESS) {
        return ERROR_WIFI_UNKNOWN;
    }
    return WIFI_SUCCESS;
}

errcode_t wifi_set_base_mac_addr(const int8_t *mac_addr, uint8_t mac_len)
{
    uint32_t ret;
    if (mac_addr == NULL || mac_len != WIFI_MAC_LEN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:parameter is invalid.", __LINE__);
        return ERRCODE_INVALID_PARAM;
    }
    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERRCODE_FAIL;
    }

    if (g_softap_enble_flag == 1 || g_sta_enble_flag == 1 || g_p2p_enable_flag == 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta、p2p or softap are enabled", __LINE__);
        return ERRCODE_FAIL;
    }
    ret = set_dev_addr(mac_addr, mac_len, 0);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: fail", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_get_base_mac_addr(int8_t *mac_addr, uint8_t mac_len)
{
    uint32_t ret;
    if (mac_addr == NULL || mac_len != WIFI_MAC_LEN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:parameter is invalid.", __LINE__);
        return ERRCODE_INVALID_PARAM;
    }
    ret = get_dev_addr(mac_addr, mac_len, 2); /* 2表示NL80211_IFTYPE_STATION */
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: fail", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

#define NL80211_IFTYPE_AP 3
errcode_t wifi_softap_set_mac_addr(const int8_t *mac_addr, uint8_t mac_len)
{
    uint32_t ret;
    if (mac_addr == NULL || mac_len != WIFI_MAC_LEN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:parameter is invalid.", __LINE__);
        return ERRCODE_INVALID_PARAM;
    }
    if (g_softap_enble_flag == 1 || g_sta_enble_flag == 1 || g_p2p_enable_flag == 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:sta、p2p or softap are enabled", __LINE__);
        return ERRCODE_FAIL;
    }
    ret = set_dev_addr(mac_addr, mac_len, NL80211_IFTYPE_AP);
    if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: fail", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_softap_get_mac_addr(int8_t *mac_addr, uint8_t mac_len)
{
    uint32_t ret;
    if (mac_addr == NULL || mac_len != WIFI_MAC_LEN) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:parameter is invalid.", __LINE__);
        return ERRCODE_INVALID_PARAM;
    }
    ret = get_dev_addr(mac_addr, mac_len, NL80211_IFTYPE_AP);
        if (ret != EXT_WIFI_OK) {
        service_error_log1(SERVICE_ERROR, "Srv:%d: fail", __LINE__);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_set_mac_derivation_ptr(wifi_mac_derivation_ptr ptr)
{
    set_mac_derivation_ptr((mac_derivation_ptr)ptr);
    return WIFI_SUCCESS;
}

#ifdef CONFIG_PLAT_DFR_SUPPORT
errcode_t plat_register_driver_panic_cb(wifi_driver_event_cb event_cb)
{
    int ret;
    ret = wifi_driver_event_callback_register(event_cb);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log2(SERVICE_ERROR, "Srv:%d: fail, ret=[%d].", ret, __LINE__);
        return ERRCODE_FAIL;
    }
}

errcode_t plat_set_gpio_mode(int32_t id, int32_t sw)
{
    int ret;
    gpio_param_t gpio;

    gpio.id = id;
    gpio.val = sw;

    ret = uapi_set_gpio_val((int8_t *)&gpio);
    if (ret == 0) {
        return ERRCODE_SUCC;
    } else {
        service_error_log2(SERVICE_ERROR, "Srv:%d: fail, ret=[%d].", __LINE__, ret);
        return ERRCODE_FAIL;
    }
}
#endif

errcode_t wifi_set_low_current_boot_mode(uint8_t flag)
{
    int ret;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:WiFi is not initialized", __LINE__);
        return ERROR_WIFI_NOT_STARTED;
    }

    ret = uapi_set_low_current_boot_mode(flag);
    if (ret != 0) {
        service_error_log1(SERVICE_ERROR, "Srv:%d:config failed", __LINE__);
        return ERROR_WIFI_UNKNOWN;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_brctl_setbr(const char *oper)
{
    if (oper == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    if (uapi_wifi_set_brctl(oper, NULL) != 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_brctl_setif(const char *oper, const char *if_name)
{
    if (oper == NULL || if_name == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    if (uapi_wifi_set_brctl(oper, if_name) != 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_brctl_show(const char *oper)
{
    if (oper == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    if (uapi_wifi_set_brctl(oper, NULL) != 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t wifi_set_tx_pwr_offset(const int8_t *ifname, int16_t offset)
{
    uint32_t ret;

    if (uapi_wifi_get_init_status() != 1) {
        service_error_log0(SERVICE_ERROR, "Service::SetTxPwrOffset:WiFi is not initialized");
        return ERROR_WIFI_NOT_STARTED;
    }

    if (ifname == NULL) {
        service_error_log0(SERVICE_ERROR, "Service::SetTxPwrOffset:parameter is invalid.");
        return ERROR_WIFI_INVALID_ARGS;
    }

    ret = uapi_wifi_set_tx_pwr_offset(ifname, offset);
    if (ret != 0) {
        service_error_log0(SERVICE_ERROR, "Service::SetTxPwrOffset:config failed");
        return ERROR_WIFI_UNKNOWN;
    }
    return ERRCODE_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
