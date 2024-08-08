/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: wifi sta scan result list adapt handle
 */

#ifndef _WIFI_SCAN_LIST_ADAPT_H_
#define _WIFI_SCAN_LIST_ADAPT_H_

#include "soc_wifi_api.h"
#include "wpa_supplicant_i.h"
#include "utils/common.h"

typedef enum {
    WIFI_APP_SERVICE_TYPE_UPDATE_WPS_INFO = 0,
    WIFI_APP_SERVICE_TYPE_SELECT_BSS,
    WIFI_APP_SERVICE_TYPE_GET_BSS,
    WIFI_APP_SERVICE_TYPE_GET_BSSID,
    WIFI_APP_SERVICE_TYPE_VALID_BSS,
    WIFI_APP_SERVICE_TYPE_BEACON_IE_GET,
    WIFI_APP_SERVICE_TYPE_UPDATE_WPAS_SSID,
    WIFI_APP_SERVICE_TYPE_GET_DRV_SCAN_RESULT,

    WIFI_APP_SERVICE_TYPE_BUTT
} wifi_app_service_type_enum;

typedef struct {
    struct wpa_supplicant *wpa_s;
} wifi_wps_update_ap_info;

typedef struct {
    wifi_app_common_param common_param;
    wifi_wps_update_ap_info private_param;
} wifi_app_wps_update_ap_info_param;

typedef struct {
    const u8 *bssid;
    const u8 *ssid;
    size_t ssid_len;
} wifi_app_valid_bss;

typedef struct {
    wifi_app_common_param common_param;
    wifi_app_valid_bss private_param;
} wifi_app_valid_bss_param;

typedef struct {
    const u8 *bssid;
    const u8 *ssid;
    size_t ssid_len;
    struct wpa_bss *bss_buf;
    unsigned int buf_size;
} wifi_app_bss_get;

typedef struct {
    wifi_app_common_param common_param;
    wifi_app_bss_get private_param;
} wifi_app_bss_get_param;

typedef struct {
    const u8 *bssid;
    struct wpa_bss *bss_buf;
    unsigned int buf_size;
} wifi_app_bssid_get;

typedef struct {
    wifi_app_common_param common_param;
    wifi_app_bssid_get private_param;
} wifi_app_bssid_get_param;

typedef struct {
    struct wpa_supplicant *wpa_s;
    struct wpa_bss *bss_buf;
    unsigned int buf_size;
} wifi_app_beacon_ie_get;

typedef struct {
    wifi_app_common_param common_param;
    wifi_app_beacon_ie_get private_param;
} wifi_app_beacon_ie_get_param;

typedef struct {
    struct wpa_ssid *wpa_s_ssid;
    const u8 *bssid;
} wifi_app_update_wpas_ssid;

typedef struct {
    wifi_app_common_param common_param;
    wifi_app_update_wpas_ssid private_param;
} wifi_app_update_wpas_ssid_param;

typedef struct {
    struct wpa_supplicant *wpa_s;
    struct wpa_ssid *group;
    struct wpa_ssid **selected_ssid;
    struct wpa_bss *bss_buf;
    unsigned int buf_size;
} wifi_app_select_bss;

typedef struct {
    wifi_app_common_param common_param;
    wifi_app_select_bss private_param;
} wifi_app_select_bss_param;

typedef struct {
    ext_wifi_ap_info *buf; /* 应用提供的结构体数组 */
    unsigned int max_size; /* 应用提供的结构体数组元素个数, 不超过64 */
    unsigned int *size; /* 驱动本次回填的实际元素个数, 不超过64 */
    const u8 *configed_bssid;
    const u8 *configed_ssid;
    size_t configed_ssid_len;
} wifi_app_get_drv_scan_result;

typedef struct {
    struct wpa_supplicant *wpa_s;
    char *start;
    char *pos;
    char *end;
    size_t reply_len;
} wifi_app_get_driver_scan_result;

typedef struct {
    wifi_app_common_param common_param;
    wifi_app_get_drv_scan_result private_param;
} wifi_app_get_scan_result_param;

typedef struct {
    unsigned char service_type;
    int (*wifi_app_service_cb)(void *param);
} wifi_app_service_cb_handle;

int wifi_app_service(void *param);

struct wpa_bss *wpa_supplicant_select_bss(struct wpa_supplicant *wpa_s, struct wpa_ssid *group,
    struct wpa_ssid **selected_ssid,  int only_first_ssid);
void os_free_drv_scan_bss(void *ptr);

#ifndef CONFIG_NO_SCAN_PROCESSING
int wpas_select_network_from_last_scan(struct wpa_supplicant *wpa_s, int new_scan, int own_request);
#endif /* CONFIG_NO_SCAN_PROCESSING */

#endif /* _WIFI_SCAN_LIST_ADAPT_H_ */
