/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
* Description: wifi sta scan result list adapt handle
*/

#include "scan_list_adapt.h"
#include "bss.h"
#include "securec.h"
#include "driver_i.h"
#include "driver_soc.h"
#include "driver_soc_common.h"
#include "wpa_supplicant_i.h"
#include "eap_common/eap_wsc_common.h"
#include "../../wpa_supplicant/wpa_supplicant/config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef wpa_unref_param
#define wpa_unref_param(P)  ((P) = (P))
#endif

#define MAX_BUF_LEN 65535

static struct wpa_scan_res *wifi_app_service_create_bss(wifi_app_common_output_param *param, size_t *size)
{
    struct wpa_bss *bss = (struct wpa_bss *)os_zalloc(sizeof(struct wpa_bss) + param->frame_ie_len);
    if (bss == NULL) {
        wpa_error_log1(MSG_ERROR, "wifi_app_service_create_bss malloc %d failed", param->frame_ie_len);
        goto OUT;
    }
    *size = sizeof(struct wpa_bss) + param->frame_ie_len;

    bss->freq = param->freq;
    bss->caps = param->caps;
    bss->beacon_int = param->beacon_int;
    bss->level = param->level;

    if (memcpy_s(bss->ssid, sizeof(bss->ssid), param->ssid, param->ssid_ie_len) != EOK) {
        wpa_warning_log1(MSG_ERROR, "{wifi_app_service_create_bss::memcpy_s ssid %d fail.}", param->ssid_ie_len);
        goto OUT;
    }
    bss->ssid_len = param->ssid_ie_len;

    if (memcpy_s(bss->bssid, EXT_WIFI_MAC_LEN, param->bssid, EXT_WIFI_MAC_LEN) != EOK) {
        wpa_warning_log0(MSG_ERROR, "{wifi_app_service_create_bss::memcpy_s bssid fail.}");
        goto OUT;
    }

    bss->ie_len = param->frame_ie_len;
    if (memcpy_s(bss->ies, bss->ie_len, param->frame_ie, param->frame_ie_len) != EOK) {
        wpa_warning_log1(MSG_ERROR, "{wifi_app_service_create_bss::memcpy_s ie_len %d fail.}", bss->ie_len);
        goto OUT;
    }

    return bss;
OUT:
    if (bss != NULL) {
        os_free(bss);
    }
    return NULL;
}

static struct wpa_scan_res *wifi_app_service_create_res(wifi_app_common_output_param *param)
{
    struct wpa_scan_res *res = (struct wpa_scan_res *)os_zalloc(sizeof(struct wpa_scan_res) + param->frame_ie_len);
    if (res == NULL) {
        wpa_error_log1(MSG_ERROR, "wifi_app_service_create_res malloc %d failed", param->frame_ie_len);
        goto OUT;
    }

    res->freq = param->freq;
    res->caps = param->caps;
    res->beacon_int = param->beacon_int;
    res->level = param->level;
    res->ie_len = param->frame_ie_len;

    if (memcpy_s(res->bssid, EXT_WIFI_MAC_LEN, param->bssid, EXT_WIFI_MAC_LEN) != EOK) {
        wpa_warning_log0(MSG_ERROR, "{wifi_app_service_create_res::memcpy_s bssid fail.}");
        goto OUT;
    }

    if (memcpy_s(&res[1], res->ie_len, param->frame_ie, param->frame_ie_len) != EOK) {
        wpa_warning_log1(MSG_ERROR, "{wifi_app_service_create_res::memcpy_s ie_len %d fail.}", res->ie_len);
        goto OUT;
    }

    return res;
OUT:
    if (res != NULL) {
        os_free(res);
    }
    return NULL;
}

void os_free_drv_scan_bss(void *ptr)
{
    if (ptr != NULL) {
        free(ptr);
    }
}

void wpa_scan_results_free(struct wpa_scan_results *res)
{
    if (res == NULL) {
        return;
    }
    os_free(res->res);
    os_free(res);
}

#ifdef CONFIG_IEEE80211R
static int wpa_supplicant_ctrl_iface_ft_ds(struct wpa_supplicant *wpa_s, char *addr)
{
    u8 target_ap[ETH_ALEN];
    struct wpa_bss *bss;
    const u8 *mdie;
    int ret;

    if (hwaddr_aton(addr, target_ap)) {
        wpa_printf(MSG_DEBUG, "CTRL_IFACE FT_DS: invalid "
            "address '%s'", addr);
        return -1;
    }

    wpa_printf(MSG_DEBUG, "CTRL_IFACE FT_DS " MACSTR, MAC2STR(target_ap));

    bss = wpa_bss_get_bssid(wpa_s, target_ap);
    if (bss)
        mdie = wpa_bss_get_ie(bss, WLAN_EID_MOBILITY_DOMAIN);
    else
        mdie = NULL;

    ret = wpa_ft_start_over_ds(wpa_s->wpa, target_ap, mdie);
    os_free(bss);

    return ret;
}
#endif /* CONFIG_IEEE80211R */

struct wpa_bss *wpa_supplicant_pick_network(struct wpa_supplicant *wpa_s, struct wpa_ssid **selected_ssid)
{
    struct wpa_bss *selected = NULL;

    if ((wpa_s->conf != NULL) && (wpa_s->conf->ssid != NULL)) {
        selected = wpa_supplicant_select_bss(wpa_s, wpa_s->conf->ssid, selected_ssid, 0);
    }

    return selected;
}

int wpa_supplicant_fast_associate(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_NO_SCAN_PROCESSING
    return -1;
#else /* CONFIG_NO_SCAN_PROCESSING */
    struct os_reltime now;
    os_get_reltime(&now);
    if (os_reltime_expired(&now, &wpa_s->last_scan, wpa_s->conf->scan_res_valid_for_connect)) {
        wpa_printf(MSG_DEBUG, "Fast associate: Old scan results");
        return -1;
    }

    return wpas_select_network_from_last_scan(wpa_s, 0, 1);
#endif /* CONFIG_NO_SCAN_PROCESSING */
}

/**
 * wpa_supplicant_get_scan_results - Get scan results
 * @wpa_s: Pointer to wpa_supplicant data
 * @info: Information about what was scanned or %NULL if not available
 * @new_scan: Whether a new scan was performed
 * Returns: Scan results, %NULL on failure
 *
 * This function request the current scan results from the driver and updates
 * the local BSS list wpa_s->bss. The caller is responsible for freeing the
 * results with wpa_scan_results_free().
 */
struct wpa_scan_results *wpa_supplicant_get_scan_results(struct wpa_supplicant *wpa_s,
    struct scan_info *info, int new_scan)
{
    struct wpa_scan_results *scan_res;
#ifndef EXT_CODE_CROP
    size_t i;
    int (*compar)(const void *, const void *) = wpa_scan_result_compar;
#endif

    scan_res = wpa_drv_get_scan_results2(wpa_s);
    if (scan_res == NULL) {
#ifndef EXT_CODE_CROP
        wpa_dbg(wpa_s, MSG_DEBUG, "Failed to get scan results");
#endif /* EXT_CODE_CROP */
        return NULL;
    }
    if (scan_res->fetch_time.sec == 0) {
        /*
         * Make sure we have a valid timestamp if the driver wrapper
         * does not set this.
         */
        os_get_reltime(&scan_res->fetch_time);
    }
#ifndef EXT_CODE_CROP
    filter_scan_res(wpa_s, scan_res);
#ifndef EXT_SCAN_SIZE_CROP
    for (i = 0; i < scan_res->num; i++) {
        struct wpa_scan_res *scan_res_item = scan_res->res[i];
#ifndef EXT_CODE_CROP
        scan_snr(scan_res_item);
        scan_est_throughput(wpa_s, scan_res_item);
#else
        scan_res_item->snr = scan_res_item->level;
#endif /* EXT_CODE_CROP */
    }
#endif /* EXT_SCAN_SIZE_CROP */
#endif

#ifndef EXT_CODE_CROP
#ifdef CONFIG_WPS
    if (wpas_wps_searching(wpa_s)) {
        wpa_dbg(wpa_s, MSG_DEBUG, "WPS: Order scan results with WPS " "provisioning rules");
        compar = wpa_scan_result_wps_compar;
    }
#endif /* CONFIG_WPS */

    if (scan_res->res) {
        qsort(scan_res->res, scan_res->num, sizeof(struct wpa_scan_res *), compar);
    }
#endif

#ifndef EXT_CODE_CROP
    dump_scan_res(scan_res);
#endif /* EXT_CODE_CROP */
#ifndef EXT_CODE_CROP
    if (wpa_s->ignore_post_flush_scan_res) {
        /* FLUSH command aborted an ongoing scan and these are the
         * results from the aborted scan. Do not process the results to
         * maintain flushed state. */
        wpa_dbg(wpa_s, MSG_DEBUG, "Do not update BSS table based on pending post-FLUSH scan results");
        wpa_s->ignore_post_flush_scan_res = 0;
        return scan_res;
    }
#endif /* EXT_CODE_CROP */
    wpa_bss_update_start(wpa_s);
#ifndef EXT_CODE_CROP
    for (i = 0; i < scan_res->num; i++)
        wpa_bss_update_scan_res(wpa_s, scan_res->res[i], &scan_res->fetch_time);
#endif
    wpa_bss_update_end(wpa_s, info, new_scan);

    return scan_res;
}

#ifdef CONFIG_WPS
static int wpas_wps_update_ap_info_cb(void *params)
{
    struct wpa_scan_res *res = NULL;
    wifi_app_wps_update_ap_info_param *param = (wifi_app_wps_update_ap_info_param *)params;

    res = wifi_app_service_create_res(&(param->common_param.output_para));
    if (res != NULL) {
        wpas_wps_update_ap_info_bss(param->private_param.wpa_s, res);
        os_free(res);
    }

    return 0;
}

void wpas_wps_update_ap_info(struct wpa_supplicant *wpa_s, struct wpa_scan_results *scan_res)
{
    wifi_app_wps_update_ap_info_param params;

    wpa_unref_param(scan_res);

    (void)memset_s(&params, sizeof(params), 0, sizeof(params));

    params.common_param.input_para.length = sizeof(wifi_app_wps_update_ap_info_param);
    params.common_param.input_para.type = WIFI_APP_SERVICE_TYPE_UPDATE_WPS_INFO;
    params.common_param.input_para.wifi_app_service_cb = wifi_app_service;

    params.private_param.wpa_s = wpa_s;

    (void)wpa_drv_handle_app_service(wpa_s, &params);

#ifndef EXT_CODE_CROP
    wpas_wps_dump_ap_info(wpa_s);
#endif /* EXT_CODE_CROP */
}
#endif

/*****************************************************************************
 功能描述  : 驱动遍历链表 匹配wpa supplicant传过来的ssid和bssid
*****************************************************************************/
static int wpa_bss_get_cb(void *params)
{
    struct wpa_bss *bss = NULL;
    size_t size = 0;
    wifi_app_bss_get_param *param = (wifi_app_bss_get_param *)params;

    if (os_memcmp(param->common_param.output_para.bssid, param->private_param.bssid, ETH_ALEN) != 0) {
        return 0;
    }

    if ((param->common_param.output_para.ssid_ie_len != param->private_param.ssid_len) ||
        (os_memcmp(param->common_param.output_para.ssid, param->private_param.ssid,
        param->private_param.ssid_len) != 0)) {
        return 0;
    }

    bss = wifi_app_service_create_bss(&(param->common_param.output_para), &size);
    if (bss == NULL) {
        wpa_warning_log1(MSG_ERROR, "{wpa_bss_get_func::create_wpa_bss fail, size %d.}", size);
        return -1;
    }

    if (memcpy_s((unsigned char *)param->private_param.bss_buf, param->private_param.buf_size,
        (unsigned char *)bss, size) != EOK) {
        wpa_warning_log2(MSG_ERROR, "{wpa_bss_get_func::memcpy buf %d %d fail.}", param->private_param.buf_size, size);
        os_free(bss);
        return -1;
    }

    os_free(bss);
    return 1;
}

/**
 * wpa_bss_get - Fetch a BSS table entry based on BSSID and SSID
 * @wpa_s: Pointer to wpa_supplicant data
 * @bssid: BSSID
 * @ssid: SSID
 * @ssid_len: Length of @ssid
 * Returns: Pointer to the BSS entry or %NULL if not found
 */
struct wpa_bss *wpa_bss_get(struct wpa_supplicant *wpa_s, const u8 *bssid, const u8 *ssid, size_t ssid_len)
{
    wifi_app_bss_get_param params = {0};

    unsigned int len = sizeof(struct wpa_bss) + 768;  /* 768 mgmt pkt max length */
    struct wpa_bss *bss = (struct wpa_bss *)os_zalloc(len);
    if (bss == NULL) {
        return NULL;
    }

    (void)memset_s(&params, sizeof(params), 0, sizeof(params));
    params.common_param.input_para.length = sizeof(wifi_app_bss_get_param);
    params.common_param.input_para.type = WIFI_APP_SERVICE_TYPE_GET_BSS;
    params.common_param.input_para.wifi_app_service_cb = wifi_app_service;

    params.private_param.bssid = bssid;
    params.private_param.ssid = ssid;
    params.private_param.ssid_len = ssid_len;
    params.private_param.bss_buf = bss;
    params.private_param.buf_size = len;

    if (wpa_drv_handle_app_service(wpa_s, &params) == 0) {
        return bss;
    }

    os_free(bss);
    return NULL;
}

/*****************************************************************************
 功能描述  : 驱动遍历链表 匹配wpa supplicant传过来的bssid
*****************************************************************************/
static int wpa_bssid_get_cb(void *params)
{
    struct wpa_bss *bss = NULL;
    size_t size = 0;
    wifi_app_bssid_get_param *param = (wifi_app_bssid_get_param *)params;

    if (os_memcmp(param->common_param.output_para.bssid, param->private_param.bssid, ETH_ALEN) != 0) {
        return 0;
    }

    bss = wifi_app_service_create_bss(&(param->common_param.output_para), &size);
    if (bss == NULL) {
        wpa_warning_log1(MSG_ERROR, "{wpa_bssid_get_func::create_wpa_bss fail, size %d.}", size);
        return -1;
    }

    if (memcpy_s((unsigned char *)param->private_param.bss_buf, param->private_param.buf_size,
        (unsigned char *)bss, size) != EOK) {
        wpa_warning_log2(MSG_ERROR, "{wpa_bssid_get_func::memcpy buf %d %d fail.}", param->private_param.buf_size, size);
        os_free(bss);
        return -1;
    }

    os_free(bss);
    return 1;
}

/**
 * wpa_bss_get_bssid - Fetch a BSS table entry based on BSSID
 * @wpa_s: Pointer to wpa_supplicant data
 * @bssid: BSSID
 * Returns: Pointer to the BSS entry or %NULL if not found
 */
struct wpa_bss *wpa_bss_get_bssid(struct wpa_supplicant *wpa_s, const u8 *bssid)
{
    wifi_app_bssid_get_param params = {0};

    unsigned int len = sizeof(struct wpa_bss) + 768;  /* 768 mgmt pkt max length */
    struct wpa_bss *bss = (struct wpa_bss *)os_zalloc(len);
    if (bss == NULL) {
        return NULL;
    }

    (void)memset_s(&params, sizeof(params), 0, sizeof(params));
    params.common_param.input_para.length = sizeof(wifi_app_bssid_get_param);
    params.common_param.input_para.type = WIFI_APP_SERVICE_TYPE_GET_BSSID;
    params.common_param.input_para.wifi_app_service_cb = wifi_app_service;

    params.private_param.bssid = bssid;
    params.private_param.bss_buf = bss;
    params.private_param.buf_size = len;

    if (wpa_drv_handle_app_service(wpa_s, &params) == 0) {
        return bss;
    }

    os_free(bss);
    return NULL;
}

/**
 * wpa_bss_get_bssid_latest - Fetch the latest BSS table entry based on BSSID
 * @wpa_s: Pointer to wpa_supplicant data
 * @bssid: BSSID
 * Returns: Pointer to the BSS entry or %NULL if not found
 *
 * This function is like wpa_bss_get_bssid(), but full BSS table is iterated to
 * find the entry that has the most recent update. This can help in finding the
 * correct entry in cases where the SSID of the AP may have changed recently
 * (e.g., in WPS reconfiguration cases).
 */
struct wpa_bss *wpa_bss_get_bssid_latest(struct wpa_supplicant *wpa_s, const u8 *bssid)
{
    return wpa_bss_get_bssid(wpa_s, bssid);
}

/*****************************************************************************
 功能描述  : 驱动遍历链表 匹配wpa supplicant传过来的ssid 和 bssid, 确认是否存在
*****************************************************************************/
static int wpas_valid_bss_cb(void *params)
{
    wifi_app_valid_bss_param *param = (wifi_app_valid_bss_param *)params;

    if (os_memcmp(param->common_param.output_para.bssid, param->private_param.bssid, ETH_ALEN) != 0) {
        return 0;
    }

    if ((param->common_param.output_para.ssid_ie_len != param->private_param.ssid_len) ||
        (os_memcmp(param->common_param.output_para.ssid, param->private_param.ssid,
        param->private_param.ssid_len) != 0)) {
        return 0;
    }

    return 1;
}

int wpas_valid_bss(struct wpa_supplicant *wpa_s, struct wpa_bss *test_bss)
{
    wifi_app_valid_bss_param params = {0};

    (void)memset_s(&params, sizeof(params), 0, sizeof(params));
    params.common_param.input_para.length = sizeof(wifi_app_valid_bss_param);
    params.common_param.input_para.type = WIFI_APP_SERVICE_TYPE_VALID_BSS;
    params.common_param.input_para.wifi_app_service_cb = wifi_app_service;

    params.private_param.bssid = test_bss->bssid;
    params.private_param.ssid = test_bss->ssid;
    params.private_param.ssid_len = test_bss->ssid_len;

    if (wpa_drv_handle_app_service(wpa_s, &params) == 0) {
        return 1;
    }

    return 0;
}

static struct wpa_bss *wpa_supplicant_select_bss_handle(struct wpa_supplicant *wpa_s, struct wpa_bss *bss,
    struct wpa_ssid *group, struct wpa_ssid **selected_ssid)
{
    struct wpa_ssid *ssid = wpa_scan_res_match(wpa_s, 0, bss, group, 0);
    if (ssid == NULL) {
        return NULL;
    }

    /* selected_ssid != NULL 场景, 提前退出 */
    if (selected_ssid != NULL) {
        *selected_ssid = ssid;
        wpa_dbg(wpa_s, MSG_DEBUG, "select BSS done2");
        return bss;
    }

    if (ssid != wpa_s->current_ssid) {
        return NULL;
    }

    wpa_dbg(wpa_s, MSG_DEBUG, "select BSS done");
    return bss;
}

static int wpas_select_bss_cb(void *params)
{
    struct wpa_bss *bss = NULL;
    struct wpa_bss *bss_tmp = NULL;
    size_t size = 0;
    wifi_app_select_bss_param *param = (wifi_app_select_bss_param *)params;

    bss = wifi_app_service_create_bss(&(param->common_param.output_para), &size);
    if (bss == NULL) {
        wpa_warning_log1(MSG_ERROR, "{wpas_select_bss_func::create_wpa_bss fail, size %d.}", size);
        return 0;
    }
    bss_tmp = wpa_supplicant_select_bss_handle(param->private_param.wpa_s, bss,
        param->private_param.group, param->private_param.selected_ssid);
    if (bss_tmp == NULL) {
        os_free(bss);
        return 0;
    }

    /* selected_ssid == NULL 场景下, 不需要拷贝 */
    if (param->private_param.selected_ssid == NULL) {
        os_free(bss);
        return 1;
    }

    /* 拷贝到上层传下来的内存里带回去, bss_buf 或 buf_size 异常时, 会返回 EOK, 不另外校验 */
    if (memcpy_s((unsigned char *)param->private_param.bss_buf, param->private_param.buf_size,
        (unsigned char *)bss, size) != EOK) {
        wpa_warning_log2(MSG_ERROR, "wpas_select_bss_func:memcpy fail,size %d %d", param->private_param.buf_size, size);
        os_free(bss);
        return -1;
    }

    os_free(bss);
    return 1;
}

struct wpa_bss *wpa_supplicant_select_bss(struct wpa_supplicant *wpa_s,
    struct wpa_ssid *group, struct wpa_ssid **selected_ssid, int only_first_ssid)
{
    wifi_app_select_bss_param params = {0};
    int ret;

    wpa_unref_param(only_first_ssid);

    (void)memset_s(&params, sizeof(params), 0, sizeof(params));
    params.common_param.input_para.length = sizeof(wifi_app_select_bss_param);
    params.common_param.input_para.type = WIFI_APP_SERVICE_TYPE_SELECT_BSS;
    params.common_param.input_para.wifi_app_service_cb = wifi_app_service;

    if (wpa_s->current_ssid) {
        params.private_param.wpa_s = wpa_s;
        params.private_param.group = group;
        params.private_param.selected_ssid = NULL;
        params.private_param.bss_buf = NULL;
        params.private_param.buf_size = 0;

        (void)wpa_drv_handle_app_service(wpa_s, &params);
    }

    unsigned int len = sizeof(struct wpa_bss) + 768; /* 768 mgmt pkt max length */
    struct wpa_bss *bss = (struct wpa_bss *)os_zalloc(len);
    if (bss == NULL) {
        return NULL;
    }

    (void)memset_s(&params, sizeof(params), 0, sizeof(params));
    params.common_param.input_para.length = sizeof(wifi_app_select_bss_param);
    params.common_param.input_para.type = WIFI_APP_SERVICE_TYPE_SELECT_BSS;
    params.common_param.input_para.wifi_app_service_cb = wifi_app_service;

    params.private_param.wpa_s = wpa_s;
    params.private_param.group = group;
    params.private_param.selected_ssid = selected_ssid;
    params.private_param.bss_buf = bss;
    params.private_param.buf_size = len;

    ret = wpa_drv_handle_app_service(wpa_s, &params);
    if (ret == 0) {
        return bss;
    }

    os_free(bss);
    return NULL;
}

#ifndef CONFIG_NO_WPA
static int wpa_get_beacon_ie_handle(wifi_app_beacon_ie_get_param *param)
{
    struct wpa_ssid *ssid = NULL;
    unsigned int flags = 0;
    struct wpa_supplicant *wpa_s;

    if ((param == NULL) || (param->private_param.wpa_s == NULL)) {
        return -1;
    }

    wpa_s = param->private_param.wpa_s;
    if (os_memcmp(param->common_param.output_para.bssid, wpa_s->bssid, ETH_ALEN) != 0) {
        return -1;
    }

    ssid = wpa_s->current_ssid;
    if (ssid == NULL ||
        ((param->common_param.output_para.ssid_ie_len == ssid->ssid_len &&
        os_memcmp(param->common_param.output_para.ssid, ssid->ssid, ssid->ssid_len) == 0) ||
        ssid->ssid_len == 0)) {
        return 0;
    }

#ifdef CONFIG_OWE
    /* origin bss->flags will always be zero, the func to update flags is owe_trans_ssid, has #ifndef EXT_CODE_CROP */
    if (ssid && (ssid->key_mgmt & WPA_KEY_MGMT_OWE) &&
        (flags & WPA_BSS_OWE_TRANSITION)) {
        return 0;
    }
#endif /* CONFIG_OWE */

    return -1;
}

static int wpa_get_beacon_ie_cb(void *params)
{
    int ret;
    struct wpa_bss *bss = NULL;
    size_t size = 0;
    wifi_app_beacon_ie_get_param *param = (wifi_app_beacon_ie_get_param *)params;

    ret = wpa_get_beacon_ie_handle(param);
    if (ret != 0) {
        return 0;
    }

    bss = wifi_app_service_create_bss(&(param->common_param.output_para), &size);
    if (bss == NULL) {
        wpa_warning_log1(MSG_ERROR, "{wpa_get_beacon_ie_func::create_wpa_bss fail, size %d.}", size);
        return -1;
    }

    /* 拷贝到上层传下来的内存里带回去, bss_buf 或 buf_size 异常时, 会返回 EOK, 不另外校验 */
    if (memcpy_s((unsigned char *)param->private_param.bss_buf, param->private_param.buf_size,
        (unsigned char *)bss, size) != EOK) {
        wpa_warning_log2(MSG_ERROR, "wpa_get_beacon_ie_func:fail,size %d %d", param->private_param.buf_size, size);
        os_free(bss);
        return -1;
    }

    os_free(bss);
    return 1;
}

int wpa_get_beacon_ie(struct wpa_supplicant *wpa_s)
{
    int ret = 0;
    struct wpa_bss *curr = NULL, *bss;
    const u8 *ie;
    wifi_app_beacon_ie_get_param params = {0};

    unsigned int len = sizeof(struct wpa_bss) + 768;  /* 768 mgmt pkt max length */
    bss = (struct wpa_bss *)os_zalloc(len);
    if (bss == NULL) {
        return -1;
    }

    (void)memset_s(&params, sizeof(params), 0, sizeof(params));
    params.common_param.input_para.length = sizeof(wifi_app_beacon_ie_get_param);
    params.common_param.input_para.type = WIFI_APP_SERVICE_TYPE_BEACON_IE_GET;
    params.common_param.input_para.wifi_app_service_cb = wifi_app_service;

    params.private_param.wpa_s = wpa_s;
    params.private_param.bss_buf = bss;
    params.private_param.buf_size = len;

    if (wpa_drv_handle_app_service(wpa_s, &params) == 0) {
        curr = bss;
    }

    if (curr) {
        ie = wpa_bss_get_vendor_ie(curr, WPA_IE_VENDOR_TYPE);
        if (wpa_sm_set_ap_wpa_ie(wpa_s->wpa, ie, ie ? 2 + ie[1] : 0))
            ret = -1;

        ie = wpa_bss_get_ie(curr, WLAN_EID_RSN);
        if (wpa_sm_set_ap_rsn_ie(wpa_s->wpa, ie, ie ? 2 + ie[1] : 0))
            ret = -1;

        ie = wpa_bss_get_ie(curr, WLAN_EID_RSNX);
        if (wpa_sm_set_ap_rsnxe(wpa_s->wpa, ie, ie ? 2 + ie[1] : 0))
            ret = -1;
    } else {
        ret = -1;
    }

    os_free(bss);
    return ret;
}
#endif

#ifndef CONFIG_P2P
static int wpas_wps_add_network_update_wpas_ssid_cb(void *params)
{
    struct wpa_ssid *ssid = NULL;
    wifi_app_update_wpas_ssid_param *param = (wifi_app_update_wpas_ssid_param *)params;

    if (os_memcmp(param->private_param.bssid, param->common_param.output_para.bssid, ETH_ALEN) != 0) {
        return 0;
    }

    ssid = param->private_param.wpa_s_ssid;
    os_free(ssid->ssid);
    ssid->ssid = os_memdup(param->common_param.output_para.ssid, param->common_param.output_para.ssid_ie_len);
    if (ssid->ssid == NULL) {
        return -1;
    }
    ssid->ssid_len = param->common_param.output_para.ssid_ie_len;
    wpa_hexdump_ascii(MSG_DEBUG, "WPS: Picked SSID from scan results", ssid->ssid, ssid->ssid_len);

    return 1;
}
#endif /* CONFIG_P2P */

struct wpa_ssid *wpas_wps_add_network(struct wpa_supplicant *wpa_s, int registrar, const u8 *dev_addr, const u8 *bssid)
{
    struct wpa_ssid *ssid;

#ifdef LOS_WPA_PATCH
    struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_priv(wpa_s);
    if (wifi_dev == NULL)
        return NULL;
#endif

    ssid = wpa_config_add_network(wpa_s->conf);
    if (ssid == NULL)
        return NULL;
#ifdef LOS_WPA_PATCH
    // sync ssid index with wifi_dev
    ssid->id = wifi_dev->network_id;
#endif

#ifndef EXT_CODE_CROP
    wpas_notify_network_added(wpa_s, ssid);
#endif /* EXT_CODE_CROP */
    wpa_config_set_network_defaults(ssid);
    ssid->temporary = 1;
    if (wpa_config_set(ssid, "key_mgmt", "WPS", 0) < 0 ||
        wpa_config_set(ssid, "eap", "WSC", 0) < 0 ||
        wpa_config_set(ssid, "identity", registrar ? "\"" WSC_ID_REGISTRAR "\"" : "\"" WSC_ID_ENROLLEE "\"", 0) < 0) {
#ifdef CONFIG_WPS
        wpas_notify_network_removed(wpa_s, ssid);
#endif /* CONFIG_WPS */
        (void)wpa_config_remove_network(wpa_s->conf, ssid->id);
        return NULL;
    }

#ifdef CONFIG_P2P
	if (dev_addr)
        (void)memcpy_s(ssid->go_p2p_dev_addr, sizeof(ssid->go_p2p_dev_addr), dev_addr, ETH_ALEN);
#else
    (void)dev_addr;
#endif /* CONFIG_P2P */

    if (bssid) {
        (void)memcpy_s(ssid->bssid, sizeof(ssid->bssid), bssid, ETH_ALEN);
        ssid->bssid_set = 1;

        /*
         * Note: With P2P, the SSID may change at the time the WPS
         * provisioning is started, so better not filter the AP based
         * on the current SSID in the scan results.
         */
#ifndef CONFIG_P2P
        wifi_app_update_wpas_ssid_param params = {0};

        (void)memset_s(&params, sizeof(params), 0, sizeof(params));
        params.common_param.input_para.length = sizeof(wifi_app_update_wpas_ssid_param);
        params.common_param.input_para.type = WIFI_APP_SERVICE_TYPE_UPDATE_WPAS_SSID;
        params.common_param.input_para.wifi_app_service_cb = wifi_app_service;

        params.private_param.wpa_s_ssid = ssid;
        params.private_param.bssid = bssid;

        (void)wpa_drv_handle_app_service(wpa_s, &params);
#endif /* CONFIG_P2P */
    }

    return ssid;
}

static void drv_soc_get_scan_pairwise(const char *pairwise_str, ext_wifi_pairwise *pairwise)
{
    int idx, num;

    /* 与枚举值ext_wifi_pairwise定义是逆序的，优先匹配混合型 */
    char *cipher_str[] = {"\"CCMP-256\"\"+TKIP\"", "\"CCMP-256\"\"+CCMP\"", "\"CCMP-256\"", "\"CCMP\"\"+TKIP\"",
        "\"TKIP\"", "\"CCMP\""};

    num = sizeof(cipher_str) / sizeof(cipher_str[0]);
    for (idx = 0; idx < num; idx++) {
        if (strstr(pairwise_str, cipher_str[idx]) != NULL) {
            break;
        }
    }
    if ((idx >= num) || (idx >= EXT_WIFI_PAIRWISE_BUTT)) {
        *pairwise = EXT_WIFI_PARIWISE_UNKNOWN;
    } else {
        *pairwise = EXT_WIFI_PAIRWISE_BUTT - idx - 1;
    }
}

void drv_soc_get_scan_crypt_para(struct wpa_scan_res *res, int32 *auth, u8 *ft, ext_wifi_pairwise *pairwise)
{
    struct wpa_ie_data data1 = {0};
    struct wpa_ie_data data2 = {0};
    ext_wifi_pairwise pairwise1 = EXT_WIFI_PARIWISE_UNKNOWN;
    ext_wifi_pairwise pairwise2 = EXT_WIFI_PARIWISE_UNKNOWN;
    char ciphers_buf[128] = {0};    /* buf 128字节 */

    if (res == NULL || auth == NULL || ft == NULL || pairwise == NULL) {
        return;
    }

    *auth = drv_soc_get_scan_auth_type(res, &data1, &data2);

#ifdef CONFIG_IEEE80211R
    if ((data1.key_mgmt & WPA_KEY_MGMT_FT) || (data2.key_mgmt & WPA_KEY_MGMT_FT)) {
        *ft = EXT_TRUE;
    }
#endif
    if (wpa_write_ciphers(ciphers_buf, ciphers_buf + 128, data1.pairwise_cipher, "+") > 0) { /* buf 128字节 */
        drv_soc_get_scan_pairwise(ciphers_buf, &pairwise1);
    }

    memset_s(ciphers_buf, sizeof(ciphers_buf), 0, sizeof(ciphers_buf));
    if (wpa_write_ciphers(ciphers_buf, ciphers_buf + 128, data2.pairwise_cipher, "+") > 0) { /* buf 128字节 */
        drv_soc_get_scan_pairwise(ciphers_buf, &pairwise2);
    }

    *pairwise = (pairwise1 >= pairwise2) ? pairwise1 : pairwise2;

    return;
}

static int uapi_wifi_sta_scan_results_cb(void *params)
{
    u32 max_element;
    int auth = -1;
    u8 ft = 0;
    ext_wifi_pairwise pairwise = EXT_WIFI_PARIWISE_UNKNOWN;
    u8 index;
    struct wpa_scan_res *res = NULL;
    ext_wifi_ap_info *ap_list = NULL;
    wifi_app_get_scan_result_param *param = (wifi_app_get_scan_result_param *)params;
    u32 count = *(param->private_param.size); /* 实际回填的ap下标 */

    max_element = param->private_param.max_size;
    index = param->common_param.output_para.index;

    if ((index >= max_element) || (count >= max_element)) {
        wpa_warning_log3(MSG_DEBUG, "uapi_wifi_sta_scan_results_cb, idx %d count %d max %d", index, count, max_element);
        return 1; /* 结束遍历 */
    }

    ap_list = &(((ext_wifi_ap_info *)(param->private_param.buf))[count]);
    ap_list->rssi = param->common_param.output_para.level;
    ap_list->channel = param->common_param.output_para.channel;

    if (memcpy_s(ap_list->ssid, sizeof(ap_list->ssid),
        param->common_param.output_para.ssid, param->common_param.output_para.ssid_ie_len) != EOK) {
        wpa_warning_log0(MSG_ERROR, "uapi_wifi_sta_scan_results_cb memcpy_s ssid error.");
        return -1;
    }
    ap_list->ssid[sizeof(ap_list->ssid) - 1] = '\0';

    if (memcpy_s(ap_list->bssid, sizeof(ap_list->bssid),
        param->common_param.output_para.bssid, EXT_WIFI_MAC_LEN) != EOK) {
        wpa_warning_log0(MSG_ERROR, "uapi_wifi_sta_scan_results_cb memcpy_s bssid error.");
        return -1;
    }

    if (wifi_scan_result_filter_parse(ap_list) == 0) {
        (void)memset_s(ap_list, sizeof(*ap_list), 0, sizeof(*ap_list));
        return 0;
    }

    res = wifi_app_service_create_res(&(param->common_param.output_para));
    if (res != NULL) {
        drv_soc_get_scan_crypt_para(res, &auth, &ft, &pairwise);
        os_free(res);
    }

    ap_list->auth = auth;
    ap_list->ft_flag = ft;
    ap_list->pairwise = pairwise;

    /* 个数 加1 */
    (*(param->private_param.size))++;

    return 0;
}

int uapi_wifi_sta_scan_results(ext_wifi_ap_info *ap_list, unsigned int *ap_num)
{
    wifi_app_get_scan_result_param *params = NULL;

    if ((ap_list == NULL) || (ap_num == NULL) || (*ap_num > WIFI_SCAN_AP_LIMIT)) {
        return -1;
    }

    if (wifi_dev_get(EXT_WIFI_IFTYPE_STATION) == NULL) {
        return -1;
    }

    /* 额外申请4字节 供size变量使用 */
    params = (wifi_app_get_scan_result_param *)os_zalloc(sizeof(wifi_app_get_scan_result_param) + 4);
    if (params == NULL) {
        return -1;
    }

    memset_s(ap_list, sizeof(ext_wifi_ap_info) * (*ap_num), 0, sizeof(ext_wifi_ap_info) * (*ap_num));

    params->common_param.input_para.length = sizeof(wifi_app_get_scan_result_param);
    params->common_param.input_para.type = WIFI_APP_SERVICE_TYPE_GET_DRV_SCAN_RESULT;
    params->common_param.input_para.wifi_app_service_cb = wifi_app_service;

    params->private_param.buf = ap_list;
    params->private_param.max_size = *ap_num;
    params->private_param.size = params + 1; /* 额外申请4字节 供size变量使用, 记录实际回填的ap个数 */
    params->private_param.configed_bssid = NULL;
    params->private_param.configed_ssid = NULL;
    params->private_param.configed_ssid_len = 0;

    /* 可能跟wpa supplicant中同时调用该函数, frw队列中应该会阻塞等待前一个消息处理完成 */
    (void)uapi_wifi_app_service("wlan0", params);

    /* 驱动回填 */
    *ap_num = *(params->private_param.size);

    (void)memset_s(&g_scan_record, sizeof(g_scan_record), 0, sizeof(g_scan_record));
    os_free(params);
    return 0;
}

int uapi_wifi_sta_scan_results_clear(void)
{
    int ret = 0;

    ret = uapi_wifi_scan_results_clear();
    if (ret != 0) {
        wpa_warning_log0(MSG_ERROR, "uapi_wifi_sta_scan_results_clear:: clear scan results fail.");
        return -1;
    }

    return 0;
}

/*****************************************************************************
* Name         : drv_soc_get_scan_results
* Description  : get scan results
* Input param  : void *priv
* Return value : struct wpa_scan_results *
*****************************************************************************/
struct wpa_scan_results *drv_soc_get_scan_results(void *priv)
{
    struct wpa_scan_results *results = NULL;
    ext_driver_data_stru *drv = priv;

    if (priv == NULL)
        return NULL;

    results = (struct wpa_scan_results *)os_zalloc(sizeof(struct wpa_scan_results));
    if (results == NULL)
        return NULL;

    results->num = drv->scan_ap_num;
    if (results->num == 0) {
        os_free(results);
        return NULL;
    }

    drv->scan_ap_num = 0;
    wpa_warning_log1(MSG_DEBUG, "Received scan results (%u BSSes)", (uint32)results->num);
    return results;
}

static void drv_soc_fill_scan_result(struct wpa_scan_res *res, ext_scan_result_stru *scan_result)
{
    res->flags = (uint32)scan_result->flags;
    res->freq = scan_result->freq;
    res->caps = (uint16)scan_result->caps;
    res->level = scan_result->level;
    res->age = scan_result->age;
    res->ie_len = scan_result->ie_len;
#ifndef EXT_SCAN_SIZE_CROP
    res->beacon_int = (uint16)scan_result->beacon_int;
    res->qual = scan_result->qual;
    res->beacon_ie_len = scan_result->beacon_ie_len;
#endif /* EXT_SCAN_SIZE_CROP */
}

void drv_soc_driver_event_scan_result_process(ext_driver_data_stru *drv, char *data_ptr)
{
    ext_scan_result_stru *scan_result = (ext_scan_result_stru *)data_ptr;
    struct wpa_scan_res *res = NULL;
    errno_t rc;
    ext_wifi_scan_result_stru scan_result_raw = {0};
    if ((scan_result->ie_len > MAX_BUF_LEN) || (scan_result->beacon_ie_len > MAX_BUF_LEN) ||
        (scan_result->variable == NULL))
        goto OUT;
#ifndef EXT_SCAN_SIZE_CROP
    res = (struct wpa_scan_res *)os_zalloc(sizeof(struct wpa_scan_res) + scan_result->ie_len +
        scan_result->beacon_ie_len);
#else
    res = (struct wpa_scan_res *)os_zalloc(sizeof(struct wpa_scan_res) + scan_result->ie_len);
#endif /* EXT_SCAN_SIZE_CROP */
    if (res == NULL)
        goto OUT;

    drv_soc_fill_scan_result(res, scan_result);
    if (memcpy_s(res->bssid, ETH_ADDR_LEN, scan_result->bssid, ETH_ADDR_LEN) != EOK)
        goto OUT;
#ifndef EXT_SCAN_SIZE_CROP
    rc = memcpy_s(&res[1], scan_result->ie_len + scan_result->beacon_ie_len,
        scan_result->variable, scan_result->ie_len + scan_result->beacon_ie_len);
#else
    rc = memcpy_s(&res[1], scan_result->ie_len, scan_result->variable, scan_result->ie_len);
#endif /* EXT_SCAN_SIZE_CROP */
    if (rc != EOK) {
        goto OUT;
    }

    if (drv->scan_ap_num >= SCAN_AP_LIMIT) {
        wpa_error_log0(MSG_ERROR, "drv_soc_driver_event_scan_result_process: drv->scan_ap_num >= SCAN_AP_LIMIT");
        goto OUT;
    }

    drv->scan_ap_num++;
    if (g_raw_scan_cb != NULL && drv_soc_raw_scan_params_convert(res, &scan_result_raw) == EXT_SUCC) {
        g_raw_scan_cb(&scan_result_raw);
    }
    OUT:
    if (res != NULL)
        os_free(res);
    os_free(scan_result->variable);
    scan_result->variable = NULL;
}

int wifi_app_service(void *param)
{
    wifi_app_common_param *para_tmp = NULL;
    int ret = -1;
    unsigned int i;
    wifi_app_service_cb_handle cb_table[] = {
#ifdef CONFIG_WPS
        {WIFI_APP_SERVICE_TYPE_UPDATE_WPS_INFO, wpas_wps_update_ap_info_cb},
#endif
        {WIFI_APP_SERVICE_TYPE_SELECT_BSS, wpas_select_bss_cb},
        {WIFI_APP_SERVICE_TYPE_GET_BSS, wpa_bss_get_cb},
        {WIFI_APP_SERVICE_TYPE_GET_BSSID, wpa_bssid_get_cb},
        {WIFI_APP_SERVICE_TYPE_VALID_BSS, wpas_valid_bss_cb},
        {WIFI_APP_SERVICE_TYPE_BEACON_IE_GET, wpa_get_beacon_ie_cb},
        {WIFI_APP_SERVICE_TYPE_UPDATE_WPAS_SSID, wpas_wps_add_network_update_wpas_ssid_cb},
        {WIFI_APP_SERVICE_TYPE_GET_DRV_SCAN_RESULT, uapi_wifi_sta_scan_results_cb},
    };

    if (param == NULL) {
        return -1;
    }

    para_tmp = (wifi_app_common_param *)param;

    for (i = 0; i < sizeof(cb_table) / sizeof(cb_table[0]); i++) {
        if (cb_table[i].service_type == para_tmp->input_para.type) {
            ret = cb_table[i].wifi_app_service_cb(param);
            break;
        }
    }

    /* ret可能取值 0(继续循环), -1(失败), 1(成功) */
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif
