/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: wifi APIs
 */

#include "utils/common.h"
#include "wifi_api.h"
#include "utils/eloop.h"
#include "wpa_supplicant_i.h"
#include "wpa_cli_rtos.h"
#include "wpa_supplicant_if.h"
#include "common/ieee802_11_common.h"
#include "config_ssid.h"
#include "src/crypto/sha1.h"
#include "driver_soc.h"
#include "driver_soc_ioctl.h"
#include "soc_osal.h"
#ifdef CONFIG_WPS
#include "wps/wps.h"
#endif
#ifdef LOS_CONFIG_MESH
#include "soc_mesh.h"
#endif /* LOS_CONFIG_MESH */
#include "securec.h"
#ifdef LOS_CONFIG_WPA_ENTERPRISE
#include "tls.h"
#endif /* LOS_CONFIG_WPA_ENTERPRISE */

#define WPA_TASK_STACK_SIZE          0x1800UL
#define WPA_CB_STACK_SIZE            0x800UL
#define WPA_TASK_PRIO_NUM            4
#define WPS_PIN_LENGTH 8
#define WPA_USE_IE_LEN_SUBSCRIPT     1
#define WPA_USE_IE_INIT_BYTE_NUM     2
#define WPA_DEFAULT_IDENTITY         "\"xxx\""
#define WPA_DEFAULT_PRIV_KEY         "\"wifi\""
#define WPA_IDENTITY_EXTRA_LEN       3

#define WPA_KEY_MGMT_WPA_PSK         "WPA-PSK"
#define WPA_KEY_MGMT_WPA_PSK_PMF     "WPA-PSK WPA-PSK-SHA256"
#define WPA_KEY_MGMT_WPA3_PSK        "SAE"
#define WPA_KEY_MGMT_WPA3_PSK_MIX    "SAE WPA-PSK WPA-PSK-SHA256"
#define WPA_KEY_MGMT_WPA2            "WPA-EAP"
#define WPA_KEY_MGMT_WPA3            "WPA-EAP-SHA256"
#define WPA_KEY_MGMT_WPA3_WPA2_MIX   "WPA-EAP WPA-EAP-SHA256"
#define WPA_GROUP_MGMT_AES_128_CMAC  "AES-128-CMAC"
#define WPA_KEY_MGMT_WPA_FT_PSK         "WPA-PSK FT-PSK"
#define WPA_KEY_MGMT_WPA_FT_PSK_PMF     "WPA-PSK WPA-PSK-SHA256 FT-PSK"
#define WPA_KEY_MGMT_WPA3_FT_PSK        "SAE FT-SAE"
#define WPA_KEY_MGMT_WPA3_FT_PSK_MIX    "SAE WPA-PSK WPA-PSK-SHA256 FT-SAE FT-PSK"
#define WPA2_PROTO  "WPA2"

static bool g_wpa_event_inited_flag;
unsigned int g_wpataskid;
struct ext_wifi_dev *g_wifi_dev[WPA_MAX_WIFI_DEV_NUM] = { NULL };
struct wifi_ap_opt_set g_ap_opt_set                    = { 0 };
struct wifi_sta_opt_set g_sta_opt_set                  = { 0 };
struct wifi_reconnect_set g_reconnect_set              = { 0 };
struct ext_scan_record g_scan_record                  = { 0 };
char *g_scan_result_buf                                = NULL;
size_t g_result_len                                    = 0;
int g_mesh_sta_flag                                    = 0;
int g_fast_connect_flag                                = 0;
int g_fast_connect_scan_flag                           = 0;
int g_connecting_flag                                  = 0;
int g_usr_scanning_flag                                = 0;
int g_mesh_flag                                        = 0;
int g_scan_flag                                        = WPA_FLAG_OFF;
static unsigned int g_lock_flag                        = WPA_FLAG_OFF;
static int g_ft_flag                                   = WPA_FLAG_ON;
static int g_assoc_auth                                = EXT_WIFI_SEC_TYPE_INVALID;
unsigned char g_quick_conn_psk[EXT_WIFI_STA_PSK_LEN]    = { 0 };
unsigned int g_quick_conn_psk_flag                     = WPA_FLAG_OFF;
ext_wifi_status *g_sta_status                           = NULL;
ext_wifi_ap_sta_info *g_ap_sta_info                     = NULL;
unsigned int g_sta_num                                 = 0;
wpa_rm_network g_wpa_rm_network                        = 0;
char g_csi_ifname[WIFI_IFNAME_MAX_SIZE]               = { 0 };

/* call back configuration */
uapi_wifi_event_cb g_wpa_event_cb                        = 0;
static unsigned int g_wpa_event_running                  = 0;
static unsigned int g_wpa_event_taskid                   = 0;
static unsigned char g_direct_cb                         = 0; /* 0:create new task call cb, 1:direct call cb */
static unsigned char g_cb_task_prio                      = 20; /* callback task priority */
static unsigned short g_cb_stack_size                    = 0x800; /* callback task stack size 2k */

ext_wifi_scan_no_save_cb g_raw_scan_cb = NULL;

#ifdef LOS_CONFIG_WPA_ENTERPRISE
#define EXT_WIFI_IDENTITY_LEN 64
#define ENTERPRISE_CERT_MAX_LEN 2048
#define ENTERPRISE_WFA_CERT_MAX_LEN  3072
static char g_eap_identity[EXT_WIFI_IDENTITY_LEN + WPA_IDENTITY_EXTRA_LEN] = WPA_DEFAULT_IDENTITY;
static ext_wifi_eap_method g_eap_method = EXT_WIFI_EAP_METHOD_TLS;
static const char* g_eap_method_name[(int)EXT_WIFI_EAP_METHOD_BUTT] = {
    [(int)EXT_WIFI_EAP_METHOD_TLS] = "TLS"
};
static unsigned char *g_wfa_ca_cert = NULL;
static unsigned char *g_wfa_cli_cert = NULL;
static unsigned char *g_wfa_cli_key = NULL;
static char *g_wfa_ent_identity = NULL;
extern unsigned char __attribute__((weak)) g_ent_ca_cert[ENTERPRISE_CERT_MAX_LEN];
extern unsigned char __attribute__((weak)) g_ent_cli_cert[ENTERPRISE_CERT_MAX_LEN];
extern unsigned char __attribute__((weak)) g_ent_cli_key[ENTERPRISE_CERT_MAX_LEN];
extern char __attribute__((weak)) g_ent_identity[EXT_WIFI_IDENTITY_LEN];
#endif /* LOS_CONFIG_WPA_ENTERPRISE */

void wpa_event_task_free(void)
{
#ifdef __LITEOS__
    // 防止LiteOS任务无法回收，触发一次任务回收
    LOS_TaskResRecycle();
#endif
    if (g_wpa_event_taskid != 0) {
        osal_kfree((void *)g_wpa_event_taskid);
        g_wpa_event_taskid = 0; /* 防止异常return后，重复释放 */
    }
}

static int lock_flag_opr(int (*cb)(void))
{
    int ret;
    unsigned int int_save;

    if (cb == NULL) {
        return EXT_WIFI_FAIL;
    }

    os_intlock(&int_save);
    ret = cb();
    os_intrestore(int_save);

    return ret;
}

static int try_set_lock_flag_internal(void)
{
    int ret = EXT_WIFI_OK;

    if (g_lock_flag == WPA_FLAG_ON)
        ret = EXT_WIFI_FAIL;
    else
        g_lock_flag = WPA_FLAG_ON;

    return ret;
}

/* forward declaration */
int try_set_lock_flag(void)
{
    return lock_flag_opr(try_set_lock_flag_internal);
}

static int is_lock_flag_off_internal(void)
{
    int ret = EXT_WIFI_OK;

    if (g_lock_flag == WPA_FLAG_ON)
        ret = EXT_WIFI_FAIL;

    return ret;
}

int is_lock_flag_off(void)
{
    return lock_flag_opr(is_lock_flag_off_internal);
}

static int clr_lock_flag_internal(void)
{
    g_lock_flag = WPA_FLAG_OFF;
    return EXT_WIFI_OK;
}

void clr_lock_flag(void)
{
    (void)lock_flag_opr(clr_lock_flag_internal);
}

int chan_to_freq(unsigned char chan)
{
    if (chan == 0)
        return chan;
    if ((chan < 1) || (chan > 14)) { /* 1: channel 1; 14: channel 14 */
        wpa_error_log1(MSG_ERROR, "warning : bad channel number: %u \n", (unsigned int)chan);
        return EXT_WIFI_FAIL;
    }
    if (chan == 14) /* 14: channel 14 */
        return 2414 + 5 * chan; /* 2414: frequency, 5: the number of channel  */
    return 2407 + 5 * chan; /* 2407: frequency, 5: the number of channel */
}

int addr_precheck(const unsigned char *addr)
{
    if ((addr == NULL) || is_zero_ether_addr(addr) || is_broadcast_ether_addr(addr)) {
        wpa_error_log0(MSG_ERROR, "bad addr");
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

static int sta_precheck(void)
{
    unsigned int ret = (unsigned int)(los_count_wifi_dev_in_use() >= WPA_DOUBLE_IFACE_WIFI_DEV_NUM);
    unsigned int int_save;
    os_intlock(&int_save);

    for (int i = 0; i < WPA_MAX_WIFI_DEV_NUM; i++) {
        if (g_wifi_dev[i] != NULL)
            ret |= (unsigned int)((g_wifi_dev[i]->iftype == EXT_WIFI_IFTYPE_STATION));
    }
    os_intrestore(int_save);

    if (ret)
        return EXT_WIFI_FAIL;
    return EXT_WIFI_OK;
}

static int wifi_sta_enterprise_check(ext_wifi_auth_mode mode)
{
    if ((mode == EXT_WIFI_SECURITY_WPA) || (mode == EXT_WIFI_SECURITY_WPA2) || (mode == EXT_WIFI_SECURITY_WPA3) ||
        (mode == EXT_WIFI_SECURITY_WPA3_WPA2_MIX))
        return EXT_WIFI_OK;
    return EXT_WIFI_FAIL;
}

static int is_sta_on(void)
{
    if (los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_STATION) == NULL)
        return EXT_WIFI_FAIL;

    return EXT_WIFI_OK;
}

int is_ap_mesh_or_p2p_on(void)
{
    if ((los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_AP) != NULL) ||
        (los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_MESH_POINT) != NULL) ||
        (los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_P2P_CLIENT) != NULL) ||
        (los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_P2P_GO) != NULL) ||
        (los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_P2P_DEVICE) != NULL))
        return EXT_WIFI_OK;

    return EXT_WIFI_FAIL;
}


int los_count_wifi_dev_in_use(void)
{
    int i;
    int count = 0;
    unsigned int int_save;

    os_intlock(&int_save);
    for (i = 0; i < WPA_MAX_WIFI_DEV_NUM; i++) {
        if (g_wifi_dev[i] != NULL)
            count++;
    }
    os_intrestore(int_save);

    return count;
}

struct ext_wifi_dev * los_get_wifi_dev_by_name(const char *ifname)
{
    int i;
    if (ifname == NULL)
        return NULL;

    unsigned int int_save;
    os_task_lock(&int_save);
    for (i = 0; i < WPA_MAX_WIFI_DEV_NUM; i++) {
        if ((g_wifi_dev[i] != NULL) && (strcmp(g_wifi_dev[i]->ifname, ifname) == 0)) {
            g_wifi_dev[i]->network_id = i;
            os_task_unlock(int_save);
            return g_wifi_dev[i];
        }
    }
    os_task_unlock(int_save);

    return NULL;
}

struct ext_wifi_dev * los_get_wifi_dev_by_iftype(ext_wifi_iftype iftype)
{
    int i;
    struct ext_wifi_dev *dev = NULL;

    if (iftype >= EXT_WIFI_IFTYPES_BUTT)
        return NULL;

    unsigned int int_save;
    os_task_lock(&int_save);
    for (i = 0; i < WPA_MAX_WIFI_DEV_NUM; i++) {
        if ((g_wifi_dev[i] != NULL) && (g_wifi_dev[i]->iftype == iftype)) {
            dev = g_wifi_dev[i];
            break;
        }
    }
    os_task_unlock(int_save);

    return dev;
}

struct ext_wifi_dev * los_get_wifi_dev_by_priv(const void *ctx)
{
    int i;
    struct ext_wifi_dev *dev = NULL;

    if (ctx == NULL)
        return NULL;

    unsigned int int_save;
    os_task_lock(&int_save);
    for (i = 0; i < WPA_MAX_WIFI_DEV_NUM; i++) {
        if ((g_wifi_dev[i] != NULL) && (g_wifi_dev[i]->priv == ctx)) {
            dev = g_wifi_dev[i];
            break;
        }
    }
    os_task_unlock(int_save);

    return dev;
}

struct ext_wifi_dev * wpa_get_other_existed_wpa_wifi_dev(const void *priv)
{
    int i;
    struct ext_wifi_dev *dev = NULL;

    unsigned int int_save;
    os_task_lock(&int_save);
    for (i = 0; i < WPA_MAX_WIFI_DEV_NUM; i++) {
        if ((g_wifi_dev[i] != NULL) && (g_wifi_dev[i]->priv != priv) &&
            ((g_wifi_dev[i]->iftype == EXT_WIFI_IFTYPE_STATION) ||
             (g_wifi_dev[i]->iftype == EXT_WIFI_IFTYPE_MESH_POINT) ||
             ((g_wifi_dev[i]->iftype >= EXT_WIFI_IFTYPE_P2P_CLIENT) &&
             (g_wifi_dev[i]->iftype <= EXT_WIFI_IFTYPE_P2P_DEVICE)))) {
            dev = g_wifi_dev[i];
            break;
        }
    }
    os_task_unlock(int_save);

    return dev;
}

struct ext_wifi_dev * wifi_dev_get(ext_wifi_iftype iftype)
{
    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi_dev_get: wifi dev start or stop is running.");
        return NULL;
    }

    struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_iftype(iftype);
    if (wifi_dev == NULL) {
        wpa_error_log1(MSG_ERROR, "wifi_dev_get: get iftype = %u dev failed.", (unsigned int)iftype);
        return NULL;
    }

    return wifi_dev;
}

void los_free_wifi_dev(struct ext_wifi_dev *wifi_dev)
{
    if (wifi_dev == NULL)
        return;

    wpa_error_log1(MSG_ERROR, "los_free_wifi_dev enter, iftype = %u", (unsigned int)wifi_dev->iftype);
    unsigned int int_save;
    os_task_lock(&int_save);

    for (int i = 0; i < WPA_MAX_WIFI_DEV_NUM; i++) {
        if (g_wifi_dev[i] == wifi_dev) {
            g_wifi_dev[i] = NULL;
            break;
        }
    }
    (void)memset_s(wifi_dev, sizeof(struct ext_wifi_dev), 0, sizeof(struct ext_wifi_dev));
    os_free(wifi_dev);
    os_task_unlock(int_save);
}

struct ext_wifi_dev * wifi_dev_creat(ext_wifi_iftype iftype, protocol_mode_enum mode)
{
    int ret;
    struct ext_wifi_dev *wifi_dev = NULL;
    wifi_dev = (struct ext_wifi_dev *)os_zalloc(sizeof(struct ext_wifi_dev));
    if (wifi_dev == NULL) {
        wpa_error_log0(MSG_ERROR, "wifi_dev malloc err.");
        return NULL;
    }
    wifi_dev->ifname_len = WIFI_IFNAME_MAX_SIZE;

    ret = wal_init_drv_wlan_netdev(iftype, mode, wifi_dev->ifname, &(wifi_dev->ifname_len));
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_dev_creat: wal_init_drv_wlan_netdev failed. \n\n\r!");
        os_free(wifi_dev);
        return NULL;
    }
    if ((wifi_dev->ifname_len <= 0) ||
        (wifi_dev->ifname_len > WIFI_IFNAME_MAX_SIZE) ||
        (wifi_dev->ifname_len != (int)strnlen(wifi_dev->ifname, WIFI_IFNAME_MAX_SIZE + 1))) {
        wpa_error_log0(MSG_ERROR, " wifi_dev_creat : Invalid iface name. \n\n\r");
        goto WIFI_DEV_CREAT_ERROR;
    }
    wifi_dev->iftype = iftype;
    wpa_warning_buf(MSG_INFO, "wifi_dev_creat: ifname:%s", wifi_dev->ifname, strlen(wifi_dev->ifname));
    wpa_warning_log2(MSG_INFO, "wifi_dev_creat: len:%d, iftype: %u\n", wifi_dev->ifname_len,
                     (unsigned int)wifi_dev->iftype);
    return wifi_dev;

WIFI_DEV_CREAT_ERROR:
    (void)wal_deinit_drv_wlan_netdev(wifi_dev->ifname);
    os_free(wifi_dev);
    return NULL;
}

int los_set_wifi_dev(struct ext_wifi_dev *wifi_dev)
{
    unsigned int int_save;

    if (wifi_dev == NULL)
        return EXT_WIFI_FAIL;

    os_intlock(&int_save);
    for (int i = 0; i < WPA_MAX_WIFI_DEV_NUM; i++) {
        if (g_wifi_dev[i] == NULL) {
            g_wifi_dev[i] = wifi_dev;
            g_wifi_dev[i]->network_id = i;
            os_intrestore(int_save);

            return EXT_WIFI_OK;
        }
    }
    os_intrestore(int_save);

    return EXT_WIFI_FAIL;
}

int los_wpa_ssid_config_set(struct wpa_ssid *ssid, const char *name, const char *value)
{
    errno_t rc;
    u8 *ssid_txt = NULL;
    if ((ssid == NULL) || (name == NULL) || (value == NULL) || (strlen(value) > EXT_WIFI_MAX_SSID_LEN))
        return EXT_WIFI_FAIL;
    if (((ssid->ssid) != NULL) && (strlen((char *)(ssid->ssid)) == strlen(value)) &&
        (os_memcmp(ssid->ssid, value, strlen(value)) == 0)) {
        return EXT_WIFI_FAIL;
    }
    str_clear_free((char *)(ssid->ssid));
    ssid_txt = os_zalloc(EXT_WIFI_MAX_SSID_LEN + 1);
    if (ssid_txt == NULL)
        return EXT_WIFI_FAIL;
    rc = memcpy_s(ssid_txt, EXT_WIFI_MAX_SSID_LEN + 1, value, strlen(value));
    if (rc != EOK) {
        os_free(ssid_txt);
        wpa_error_log1(MSG_ERROR, "los_wpa_ssid_config_set memcpy_s failed(%d).", rc);
        return EXT_WIFI_FAIL;
    }
    ssid->ssid = ssid_txt;
    ssid->ssid_len = strlen((char *)ssid_txt);
    return EXT_WIFI_OK;
}

static int wifi_sta_psk_init(char *param, unsigned int param_len,
                             unsigned int key_len, const struct wpa_assoc_request *assoc)
{
    int ret;
    unsigned int max_key_len = WPA_MAX_KEY_LEN;
    if ((param == NULL) || (param_len == 0) || (key_len == 0) || (assoc == NULL))
        return EXT_WIFI_FAIL;
#ifdef CONFIG_WAPI
    /* WAPI-PSK supports a maximum of 64-byte keys. */
    if (assoc->auth == EXT_WIFI_SECURITY_WAPI_PSK) {
        max_key_len = WAPI_MAX_KEY_LEN;
    }
#endif
#ifdef CONFIG_DRIVER_SOC
#ifdef CONFIG_WAPI
    if (assoc->auth == EXT_WIFI_SECURITY_WAPI_PSK) {
        if ((key_len < WPA_MIN_KEY_LEN) || (key_len > max_key_len))
            return EXT_WIFI_FAIL;
        if (assoc->hex_flag) {
            if (key_len & 0x01)
                return EXT_WIFI_FAIL;
            if (memcpy_s(param, param_len, assoc->key, key_len) != EOK)
                return EXT_WIFI_FAIL;
        } else {
            ret = snprintf_s(param, param_len, param_len - 1, "\"%s\"", assoc->key);
            if (ret < 0)
                return EXT_WIFI_FAIL;
        }
    } else {
#endif /* CONFIG_WAPI */
        if (key_len < max_key_len || assoc->auth == EXT_WIFI_SECURITY_SAE ||
            assoc->auth == EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX) {
            ret = snprintf_s(param, param_len, param_len - 1, "\"%s\"", assoc->key);
            if (ret < 0)
                return EXT_WIFI_FAIL;
        } else {
            if (memcpy_s(param, param_len, assoc->key, key_len) != EOK)
                return EXT_WIFI_FAIL;
        }
#ifdef CONFIG_WAPI
    }
#endif /* CONFIG_WAPI */
#else
    ret  = snprintf_s(param, param_len, param_len - 1, "\"%s\"", assoc->key);
    if (ret < 0)
        return EXT_WIFI_FAIL;
#endif
    return EXT_WIFI_OK;
}

#ifdef CONFIG_WPA3
static int wifi_sta_wpa3_network_set(struct wpa_supplicant *wpa_s, const char *id,
                                     const struct wpa_assoc_request *assoc)
{
    const char *key_mgmt = NULL;

    if (assoc->auth == EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX) {
        key_mgmt = assoc->ft_flag == 0 ? WPA_KEY_MGMT_WPA3_PSK_MIX : WPA_KEY_MGMT_WPA3_FT_PSK_MIX;
        if ((wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "key_mgmt", key_mgmt,
            strlen(key_mgmt)) != EXT_WIFI_OK) ||
            (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "ieee80211w", "1", strlen("1")) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
    } else if (assoc->auth == EXT_WIFI_SECURITY_SAE) {
        key_mgmt = assoc->ft_flag == 0 ? WPA_KEY_MGMT_WPA3_PSK : WPA_KEY_MGMT_WPA3_FT_PSK;
        if ((wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "key_mgmt", key_mgmt,
            strlen(key_mgmt)) != EXT_WIFI_OK) ||
            (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "ieee80211w", "2", strlen("2")) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}
#endif /* CONFIG_WPA3 */

static int wifi_sta_set_pairwise(struct wpa_supplicant *wpa_s, const char *id,
                                 const struct wpa_assoc_request *assoc)
{
    if (assoc->wpa_pairwise == EXT_WIFI_PAIRWISE_TKIP) {
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "pairwise", "CCMP TKIP", strlen("CCMP TKIP")) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
    } else if (assoc->wpa_pairwise == EXT_WIFI_PAIRWISE_AES) {
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "pairwise", "CCMP TKIP", strlen("CCMP TKIP")) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
    } else if (assoc->wpa_pairwise == EXT_WIFI_PAIRWISE_TKIP_AES_MIX) {
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "pairwise", "CCMP TKIP", strlen("CCMP TKIP")) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
    } else if (assoc->wpa_pairwise == EXT_WIFI_PAIRWISE_CCMP256) {
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "pairwise", "CCMP-256", strlen("CCMP-256")) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "group", "CCMP-256", strlen("CCMP-256")) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
    } else if (assoc->wpa_pairwise == EXT_WIFI_PAIRWISE_CCMP256_CCMP) {
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "pairwise", "CCMP-256 CCMP", strlen("CCMP-256 CCMP")) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "group", "CCMP-256 CCMP", strlen("CCMP-256 CCMP")) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
    } else if (assoc->wpa_pairwise == EXT_WIFI_PAIRWISE_CCMP256_TKIP) {
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "pairwise", "CCMP-256 TKIP", strlen("CCMP-256 TKIP")) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "group", "CCMP-256 TKIP", strlen("CCMP-256 TKIP")) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

#ifdef LOS_CONFIG_WPA_ENTERPRISE
static int wifi_sta_set_key_mgmt_ent(struct wpa_supplicant *wpa_s, const char *id,
                                     const struct wpa_assoc_request *assoc)
{
    int ret = EXT_WIFI_OK;
    if ((wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "eap", g_eap_method_name[g_eap_method],
        strlen(g_eap_method_name[g_eap_method])) != EXT_WIFI_OK) ||
        (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "private_key", WPA_DEFAULT_PRIV_KEY,
            strlen(WPA_DEFAULT_PRIV_KEY)) != EXT_WIFI_OK))
        return EXT_WIFI_FAIL;

    if (assoc->auth == EXT_WIFI_SECURITY_WPA3_WPA2_MIX) {
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "key_mgmt", WPA_KEY_MGMT_WPA3_WPA2_MIX,
            strlen(WPA_KEY_MGMT_WPA3_WPA2_MIX) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "group_mgmt", WPA_GROUP_MGMT_AES_128_CMAC,
            strlen(WPA_GROUP_MGMT_AES_128_CMAC) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "proto", WPA2_PROTO,
            strlen(WPA_GROUP_MGMT_AES_128_CMAC) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
        ret = wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "ieee80211w", "1", strlen("1"));
    } else if (assoc->auth == EXT_WIFI_SECURITY_WPA3) {
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "key_mgmt", WPA_KEY_MGMT_WPA3,
            strlen(WPA_KEY_MGMT_WPA3) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "group_mgmt", WPA_GROUP_MGMT_AES_128_CMAC,
            strlen(WPA_GROUP_MGMT_AES_128_CMAC) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "proto", WPA2_PROTO,
            strlen(WPA_GROUP_MGMT_AES_128_CMAC) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
        ret = wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "ieee80211w", "2", strlen("2"));
    } else {
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "key_mgmt", WPA_KEY_MGMT_WPA2,
            strlen(WPA_KEY_MGMT_WPA2) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "proto", WPA2_PROTO,
            strlen(WPA_GROUP_MGMT_AES_128_CMAC) != EXT_WIFI_OK))
            return EXT_WIFI_FAIL;
    }
    return ret;
}

static int wifi_sta_set_pairwise_ent(struct wpa_supplicant *wpa_s, const char *id,
                                     const struct wpa_assoc_request *assoc)
{
    (void)assoc;
    if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "group", "CCMP TKIP", strlen("CCMP TKIP")) !=
        EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
    return wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "pairwise", "CCMP TKIP", strlen("CCMP TKIP"));
}
#endif /* LOS_CONFIG_WPA_ENTERPRISE */

static int wifi_sta_set_key_mgmt_wpa(struct wpa_supplicant *wpa_s, const char *id,
    const struct wpa_assoc_request *assoc)
{
    const char *key_mgmt = NULL;

    if (g_sta_opt_set.pmf >= WIFI_MGMT_FRAME_PROTECTION_OPTIONAL) {
        key_mgmt = assoc->ft_flag == 0 ? WPA_KEY_MGMT_WPA_PSK_PMF :
            (g_ft_flag == WPA_FLAG_OFF ? WPA_KEY_MGMT_WPA_PSK_PMF : WPA_KEY_MGMT_WPA_FT_PSK_PMF);
    } else {
        if (assoc->wpa_pairwise >= EXT_WIFI_PAIRWISE_CCMP256) {
            key_mgmt = assoc->ft_flag == 0 ? WPA_KEY_MGMT_WPA_PSK_PMF :
                (g_ft_flag == WPA_FLAG_OFF ? WPA_KEY_MGMT_WPA_PSK_PMF : WPA_KEY_MGMT_WPA_FT_PSK_PMF);
        } else {
            key_mgmt = assoc->ft_flag == 0 ? WPA_KEY_MGMT_WPA_PSK :
                (g_ft_flag == WPA_FLAG_OFF ? WPA_KEY_MGMT_WPA_PSK : WPA_KEY_MGMT_WPA_FT_PSK);
        }
    }

    if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "key_mgmt", key_mgmt,
        strlen(key_mgmt)) != EXT_WIFI_OK) {
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

static int wifi_sta_set_key_mgmt(struct wpa_supplicant *wpa_s, const char *id,
                                 const struct wpa_assoc_request *assoc)
{
    switch (assoc->auth) {
        case EXT_WIFI_SECURITY_WEP:
            if (wpa_cli_configure_wep(wpa_s, id, assoc) != EXT_WIFI_OK)
                return EXT_WIFI_FAIL;
            return EXT_WIFI_OK;
        case EXT_WIFI_SECURITY_WPAPSK:
        case EXT_WIFI_SECURITY_WPA2PSK:
            /* fall-through */
        case EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX:
            if (wifi_sta_set_key_mgmt_wpa(wpa_s, id, assoc) != EXT_WIFI_OK) {
                return EXT_WIFI_FAIL;
            }
            break;
#ifdef CONFIG_WPA3
        case EXT_WIFI_SECURITY_SAE:
        case EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX:
            if (wifi_sta_wpa3_network_set(wpa_s, id, assoc) != EXT_WIFI_OK)
                return EXT_WIFI_FAIL;
            break;
#endif /* CONFIG_WPA3 */
#ifdef LOS_CONFIG_WPA_ENTERPRISE
        case EXT_WIFI_SECURITY_WPA:
        case EXT_WIFI_SECURITY_WPA2:
        case EXT_WIFI_SECURITY_WPA3:
        case EXT_WIFI_SECURITY_WPA3_WPA2_MIX:
            if (wifi_sta_set_key_mgmt_ent(wpa_s, id, assoc) != EXT_WIFI_OK)
                return EXT_WIFI_FAIL;
            break;
#endif /* LOS_CONFIG_WPA_ENTERPRISE */
#ifdef CONFIG_WAPI
        case EXT_WIFI_SECURITY_WAPI_PSK:
            if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "key_mgmt", "WAPI-PSK",
                strlen("WAPI-PSK")) != EXT_WIFI_OK)
                return EXT_WIFI_FAIL;
            break;
#endif
        default:
            return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

static int wifi_sta_psk_separate_set(struct wpa_supplicant *wpa_s, const struct wpa_assoc_request *assoc,
                                     const char *id, bool is_fast_connnect)
{
    /* add \"\" (length 4) for param */
    char param[WPA_MAX_SSID_KEY_INPUT_LEN + 4 + 1] = { 0 };
    /* if fast connect and use psk,not check key */
    if ((is_fast_connnect != false) && (assoc->psk_flag != EXT_WIFI_WPA_PSK_NOT_USE)) {
        if ((snprintf_s(param, sizeof(param), sizeof(param) - 1, "%u", (unsigned int)assoc->psk_flag) < 0) ||
            (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "set_psk", param, sizeof(param)) != EXT_WIFI_OK)) {
            wpa_error_log0(MSG_ERROR, " only psk set fail .\n");
            return EXT_WIFI_FAIL;
        }
    } else {
        if (wifi_sta_psk_init(param, sizeof(param), os_strlen(assoc->key), assoc) != EXT_WIFI_OK)
            return EXT_WIFI_FAIL;
#ifdef CONFIG_WPA3
        if (((assoc->auth == EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX) || (assoc->auth == EXT_WIFI_SECURITY_SAE)) &&
            (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "sae_password", param, sizeof(param))
                != EXT_WIFI_OK)) {
            (void)memset_s(param, sizeof(param), 0, sizeof(param));
            return EXT_WIFI_FAIL;
        }
#endif /* CONFIG_WPA3 */

        if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "psk", param, sizeof(param)) != EXT_WIFI_OK) {
            (void)memset_s(param, sizeof(param), 0, sizeof(param));
            return EXT_WIFI_FAIL;
        }
    }
    (void)memset_s(param, sizeof(param), 0, sizeof(param));
    return EXT_WIFI_OK;
}

#ifdef CONFIG_OWE
static int wifi_sta_owe_network_set(struct wpa_supplicant *wpa_s, const char *id,
    const struct wpa_assoc_request *assoc)
{
    if ((wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN,
        "key_mgmt", "OWE", strlen("OWE")) != EXT_WIFI_OK) ||
        (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "ieee80211w", "2", strlen("2")) != EXT_WIFI_OK) ||
        (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "scan_ssid", "1", strlen("1")) != EXT_WIFI_OK)) {
        wpa_error_log0(MSG_ERROR, " key_mgmt set owe fail .\n");
        return EXT_WIFI_FAIL;
    }
    return wifi_sta_set_pairwise(wpa_s, id, assoc);
}
#endif /* CONFIG_OWE */

static int wifi_sta_set_network_psk(struct wpa_supplicant *wpa_s, const char *id,
                                    const struct wpa_assoc_request *assoc, bool is_fast_connnect)
{
    if (assoc->auth == EXT_WIFI_SECURITY_OPEN) {
        if ((wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN,
                                 "auth_alg", "OPEN", strlen("OPEN")) != EXT_WIFI_OK) ||
            (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN,
                                 "key_mgmt", "NONE", strlen("NONE")) != EXT_WIFI_OK)) {
            wpa_error_log0(MSG_ERROR, " key_mgmt set fail .\n");
            return EXT_WIFI_FAIL;
        }
        return EXT_WIFI_OK;
    }
#ifdef CONFIG_OWE
    if (assoc->auth == EXT_WIFI_SECURITY_OWE) {
        return wifi_sta_owe_network_set(wpa_s, id, assoc);
    }
#endif /* CONFIG_OWE */

    /* set security type */
    if (wifi_sta_set_key_mgmt(wpa_s, id, assoc) != EXT_WIFI_OK)
        return EXT_WIFI_FAIL;

    if (wifi_sta_psk_separate_set(wpa_s, assoc, id, is_fast_connnect) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, " psk set fail .\n");
        return EXT_WIFI_FAIL;
    }

    return wifi_sta_set_pairwise(wpa_s, id, assoc);
}

#ifdef LOS_CONFIG_WPA_ENTERPRISE
int uapi_wifi_put_ca_cert(const unsigned char *ca_cert, unsigned int len)
{
    if (g_wfa_ca_cert == NULL) {
        g_wfa_ca_cert = (char *)os_zalloc(sizeof(char) * ENTERPRISE_WFA_CERT_MAX_LEN);

        if (g_wfa_ca_cert == NULL) {
            return EXT_WIFI_FAIL;
        }
    }

    if (ca_cert != NULL) {
        (osal_void)memset_s(g_wfa_ca_cert, ENTERPRISE_WFA_CERT_MAX_LEN, 0, ENTERPRISE_WFA_CERT_MAX_LEN);

        if (memcpy_s(g_wfa_ca_cert, ENTERPRISE_WFA_CERT_MAX_LEN, ca_cert, len + 1) != EOK) {
            wpa_error_log1(MSG_ERROR, " uapi_wifi_put_ca_cert memcpy failed, len:%d .\n", len);
            return EXT_WIFI_FAIL;
        }
    }

    return EXT_WIFI_OK;
}

int uapi_wifi_put_cli_cert(const unsigned char *cli_cert, unsigned int len)
{
    if (g_wfa_cli_cert == NULL) {
        g_wfa_cli_cert = (char *)os_zalloc(sizeof(char) * ENTERPRISE_WFA_CERT_MAX_LEN);

        if (g_wfa_cli_cert == NULL) {
            return EXT_WIFI_FAIL;
        }
    }

    if (cli_cert != NULL) {
        (osal_void)memset_s(g_wfa_cli_cert, ENTERPRISE_WFA_CERT_MAX_LEN, 0, ENTERPRISE_WFA_CERT_MAX_LEN);

        if (memcpy_s(g_wfa_cli_cert, ENTERPRISE_WFA_CERT_MAX_LEN, cli_cert, len + 1) != EOK) {
            wpa_error_log1(MSG_ERROR, " uapi_wifi_put_cli_cert memcpy failed, len:%d .\n", len);
            return EXT_WIFI_FAIL;
        }
    }

    return EXT_WIFI_OK;
}

int uapi_wifi_put_cli_key(const unsigned char *cli_key, unsigned int len)
{
    if (g_wfa_cli_key == NULL) {
        g_wfa_cli_key = (char *)os_zalloc(sizeof(char) * ENTERPRISE_WFA_CERT_MAX_LEN);

        if (g_wfa_cli_key == NULL) {
            return EXT_WIFI_FAIL;
        }
    }

    if (cli_key != NULL) {
        (osal_void)memset_s(g_wfa_cli_key, ENTERPRISE_WFA_CERT_MAX_LEN, 0, ENTERPRISE_WFA_CERT_MAX_LEN);

        if (memcpy_s(g_wfa_cli_key, ENTERPRISE_WFA_CERT_MAX_LEN, cli_key, len) != EOK) {
            wpa_error_log1(MSG_ERROR, " uapi_wifi_put_cli_key memcpy failed, len:%d .\n", len);
            return EXT_WIFI_FAIL;
        }
    }

    return EXT_WIFI_OK;
}

int uapi_wifi_put_identity(const unsigned char *identity, unsigned int len)
{
    if (g_wfa_ent_identity == NULL) {
        g_wfa_ent_identity = (char *)os_zalloc(sizeof(char) * EXT_WIFI_IDENTITY_LEN);

        if (g_wfa_ent_identity == NULL) {
            return EXT_WIFI_FAIL;
        }
    }

    if (identity != NULL) {
        (osal_void)memset_s(g_wfa_ent_identity, EXT_WIFI_IDENTITY_LEN, 0, EXT_WIFI_IDENTITY_LEN);

        if (memcpy_s(g_wfa_ent_identity, EXT_WIFI_IDENTITY_LEN, identity, len) != EOK) {
            wpa_error_log1(MSG_ERROR, " uapi_wifi_put_identity memcpy failed, len:%d .\n", len);
            return EXT_WIFI_FAIL;
        }
    }

    return EXT_WIFI_OK;
}
int uapi_wifi_ca_cert_import_cb(const unsigned char **buf, unsigned int *len)
{
    if (g_wfa_ca_cert != NULL) {
        *len = strlen(g_wfa_ca_cert) + 1;
        *buf = g_wfa_ca_cert;
    } else if (g_ent_ca_cert != NULL) {
        *len = strlen(g_ent_ca_cert) + 1;
        *buf = g_ent_ca_cert;
    }

    return EXT_WIFI_OK;
}

int uapi_wifi_cli_cert_import_cb(const unsigned char **buf, unsigned int *len)
{
    if (g_wfa_cli_cert != NULL) {
        *len = strlen(g_wfa_cli_cert) + 1;
        *buf = g_wfa_cli_cert;
    } else if (g_ent_cli_cert != NULL) {
        *len = strlen(g_ent_cli_cert) + 1;
        *buf = g_ent_cli_cert;
    }

    return EXT_WIFI_OK;
}

int uapi_wifi_cli_key_import_cb(const unsigned char **buf, unsigned int *len)
{
    if (g_wfa_cli_key != NULL) {
        *len = strlen(g_wfa_cli_key) + 1;
        *buf = g_wfa_cli_key;
    } else if (g_ent_cli_key != NULL) {
        *len = strlen(g_ent_cli_key) + 1;
        *buf = g_ent_cli_key;
    }

    return EXT_WIFI_OK;
}

static int wifi_sta_set_network_ent(struct wpa_supplicant *wpa_s, const char *id,
                                    const struct wpa_assoc_request *assoc)
{
    ext_wifi_ent_import_callback ent_cb;

    uapi_wifi_sta_ent_set_eap_method(EXT_WIFI_EAP_METHOD_TLS);
    if (g_wfa_ent_identity != NULL) {
        if (uapi_wifi_sta_ent_set_identity(g_wfa_ent_identity, strlen(g_wfa_ent_identity))) {
            wpa_error_log0(MSG_ERROR, "wifi_sta_set_network_ent:set identity error.");
            return EXT_WIFI_FAIL;
        }
    } else if (uapi_wifi_sta_ent_set_identity(g_ent_identity, strlen(g_ent_identity))) {
        wpa_error_log0(MSG_ERROR, "wifi_sta_set_network_ent:set identity error.");
        return EXT_WIFI_FAIL;
    }

    ent_cb.ca_cert_import = uapi_wifi_ca_cert_import_cb;
    ent_cb.cli_cert_import = uapi_wifi_cli_cert_import_cb;
    ent_cb.cli_key_import = uapi_wifi_cli_key_import_cb;
    uapi_wifi_sta_ent_set_import_callback(&ent_cb);

    if ((wifi_sta_set_key_mgmt(wpa_s, id, assoc) != EXT_WIFI_OK) ||
        (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "identity",
                             g_eap_identity, strlen(g_eap_identity)) != EXT_WIFI_OK))
        return EXT_WIFI_FAIL;

    return wifi_sta_set_pairwise_ent(wpa_s, id, assoc);
}
#endif /* LOS_CONFIG_WPA_ENTERPRISE */

int los_freq_to_channel(int freq, unsigned int *channel)
{
    unsigned char tmp_channel = 0;
    if (channel == NULL)
        return EXT_WIFI_FAIL;

    if (ieee80211_freq_to_chan(freq, &tmp_channel) == NUM_HOSTAPD_MODES)
        return EXT_WIFI_FAIL;

    *channel = tmp_channel;
    return EXT_WIFI_OK;
}

static int wifi_scan_ssid_set(const struct wpa_scan_params *params, char *buf, int bufflen)
{
    int ret;
    if (params->ssid_len < 0) {
        wpa_error_log0(MSG_ERROR, " ssid scan len error .\n");
        return EXT_WIFI_FAIL;
    }
    if (params->ssid_len == 0)
        return EXT_WIFI_OK;

    ret = snprintf_s(buf, (size_t)bufflen, (size_t)(bufflen - 1), "ssid %s", params->ssid);
    if (ret < 0)
        return EXT_WIFI_FAIL;

    ret = snprintf_s(g_scan_record.ssid, sizeof(g_scan_record.ssid), sizeof(params->ssid) - 1, "%s", params->ssid);
    if (ret < 0)
        return EXT_WIFI_FAIL;

    return EXT_WIFI_OK;
}

static int wifi_scan_param_handle(const struct wpa_scan_params *params, char *addr_txt, int addr_len,
                                  char *freq_buff, int freq_len)
{
    errno_t rc;
    int freq;
    int ret;
    int i;
    int sum = 0;
    char freq_str[WPA_MAX_FREQ_LEN] = { 0 };
    if ((params == NULL) || (addr_txt == NULL) || (freq_buff == NULL) || (addr_len <= 0) || (freq_len <= 0)) {
        wpa_error_log0(MSG_ERROR, "wpa_pre_prarms_handle: input params is NULL\n");
        return EXT_WIFI_FAIL;
    }
    rc = memset_s(&g_scan_record, sizeof(g_scan_record), 0, sizeof(g_scan_record));
    if (rc != EOK) {
        wpa_error_log0(MSG_ERROR, "wpa_pre_prarms_handle memset_s failed");
        return EXT_WIFI_FAIL;
    }
    if (params->flag == EXT_BSSID_SCAN) {
        ret = snprintf_s(addr_txt, (size_t)addr_len, (size_t)(addr_len - 1), MACSTR, MAC2STR(params->bssid));
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "wpa_pre_prarms_handle snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
    }
    g_scan_record.flag = params->flag;
    if (params->flag == EXT_PREFIX_SSID_SCAN)
        g_ssid_prefix_flag = WPA_FLAG_ON;
    // freq scan
    for (i = 0; (i < params->chan_num) && (i < WIFI_24G_CHANNEL_NUMS); i++) {
        freq = chan_to_freq((unsigned char)params->chan_list[i]);
        if (freq <= 0) {
            wpa_error_log0(MSG_ERROR, "wpa_pre_prarms_handle invalid .\n");
            return EXT_WIFI_FAIL;
        }

        if (i == 0) {
            ret = snprintf_s(freq_str, WPA_MAX_FREQ_LEN, WPA_MAX_FREQ_LEN, "freq=%d", freq);
        } else {
            ret = snprintf_s(freq_str, WPA_MAX_FREQ_LEN, WPA_MIN_FREQ_LEN, ",%d", freq);
        }
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "wifi_scan_param_handle snprintf_s faild");
            return EXT_WIFI_FAIL;
        }

        ret = snprintf_s(freq_buff + sum, (size_t)(freq_len - sum), (size_t)(strlen(freq_str) + 1), "%s", freq_str);
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "wpa_pre_prarms_handle snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
        sum += ret;
        if (g_scan_record.chan_num < WIFI_24G_CHANNEL_NUMS)
            g_scan_record.chan_list[g_scan_record.chan_num++] = params->chan_list[i];
    }
    return EXT_WIFI_OK;
}

static int wifi_scan_buffer_process(const char *freq_buff, const char *ssid_buff,
                                    const char *bssid_buff, ext_wifi_iftype iftype)
{
    int sum;
    int ret                            = EXT_WIFI_OK;
    unsigned int ret_val               = EXT_WIFI_OK;
    char buf[WPA_EXTERNED_SSID_LEN]    = { 0 };
    struct wpa_supplicant *wpa_s       = NULL;
    struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_iftype(iftype);
    if ((wifi_dev == NULL) || ((wifi_dev)->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "wpa_buffer_handle: get wifi dev failed");
        return EXT_WIFI_FAIL;
    }
    wpa_s = (struct wpa_supplicant *)(wifi_dev->priv);
    if (freq_buff[0] != '\0') {
        ret = snprintf_s(buf, sizeof(buf), strlen(freq_buff) + 1, "%s ", freq_buff);
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "wpa_buffer_handle: freq_buff snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
    }
    sum = ret;
    if (bssid_buff[0] != '\0') {
        ret = snprintf_s((char *)(buf + sum), (size_t)(sizeof(buf) - (size_t)sum),
                         (size_t)(strlen(bssid_buff) + 1), "%s ", bssid_buff);
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "wpa_buffer_handle: bssid_buff snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
    }
    sum += ret;
    if (ssid_buff[0] != '\0' && g_ssid_prefix_flag != WPA_FLAG_ON) {
        ret = snprintf_s((char *)(buf + sum), (size_t)(sizeof(buf) - (size_t)sum),
                         (size_t)(strlen(ssid_buff) + 1), "%s", ssid_buff);
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "wpa_buffer_handle: ssid_buff snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
    }
    sum += ret;
    g_usr_scanning_flag = WPA_FLAG_ON;
    (void)os_event_clear(g_wpa_event, ~(unsigned int)WPA_EVENT_SCAN_OK);
    if (wpa_cli_scan(wpa_s, buf, sum) == EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_SCAN_OK");
        (void)os_event_read(g_wpa_event, WPA_EVENT_SCAN_OK, &ret_val,
                            WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_EVENT_DELAY_5S);
    }
    g_usr_scanning_flag = WPA_FLAG_OFF;
    g_ssid_prefix_flag = WPA_FLAG_OFF;
    if (ret_val != WPA_EVENT_SCAN_OK) {
        wpa_error_log1(MSG_ERROR, "os_event_read WPA_EVENT_SCAN_OK failed ret_val = %x", ret_val);
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

static int wifi_wpa_scan(const struct wpa_scan_params *params, ext_wifi_iftype iftype)
{
    int ret;
    errno_t rc;
    char freq_buff[WPA_MIN_FREQ_LEN * WIFI_24G_CHANNEL_NUMS] = { 0 };
    char ssid_buff[WPA_MAX_SSID_LEN * 2]    = { 0 };
    char bssid_buff[WPA_CMD_BSSID_LEN]      = { 0 };
    char addr_txt[EXT_WIFI_TXT_ADDR_LEN + 1] = { 0 };

    wpa_error_log0(MSG_ERROR, " ---> ### los_wpa_scan enter. \n\n\r");
    if (iftype == EXT_WIFI_IFTYPE_AP)
        return EXT_WIFI_FAIL;

    if (wifi_scan_param_handle(params, addr_txt, EXT_WIFI_TXT_ADDR_LEN + 1, freq_buff,
        (int)sizeof(freq_buff)) != EXT_WIFI_OK)
        return EXT_WIFI_FAIL;

    ret = wifi_scan_ssid_set(params, ssid_buff, (int)sizeof(ssid_buff));
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, " los_wpa_scan: los_wifi_wpa_ssid_scan_set failed");
        return EXT_WIFI_FAIL;
    }
    if (params->flag == EXT_BSSID_SCAN) {
        ret = snprintf_s(bssid_buff, WPA_CMD_BSSID_LEN, WPA_CMD_BSSID_LEN - 1, "bssid=%s", addr_txt);
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "los_wpa_scan snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
        rc = memcpy_s(g_scan_record.bssid, ETH_ALEN, params->bssid, ETH_ALEN);
        if (rc != EOK)
            return EXT_WIFI_FAIL;
    }
    if (((strlen(freq_buff) + 1) + (strlen(bssid_buff) + 1) + (strlen(ssid_buff) + 1)) > WPA_EXTERNED_SSID_LEN) {
        wpa_error_log0(MSG_ERROR, " ssid_buff is too long.\n");
        return EXT_WIFI_FAIL;
    }
    return wifi_scan_buffer_process(freq_buff, ssid_buff, bssid_buff, iftype);
}

static int wifi_scan_params_parse_channel_scan(const ext_wifi_scan_params *sp, struct wpa_scan_params *scan_params)
{
    unsigned int i;
    unsigned int j;
    unsigned int is_found;

    if ((sp->chan_num > WIFI_24G_CHANNEL_NUMS)) {
        wpa_error_log0(MSG_ERROR, "Invalid channel_num!");
        return EXT_WIFI_FAIL;
    }
    for (i = 0; i < sp->chan_num; i++) {
        if (wifi_channel_check(sp->chan_list[i]) == EXT_WIFI_FAIL) {
            wpa_error_log0(MSG_ERROR, "Invalid channel!");
            return EXT_WIFI_FAIL;
        }
        is_found = 0;
        for (j = 0; j < scan_params->chan_num; j++) {
            if (scan_params->chan_list[j] == sp->chan_list[i]) {
                is_found = 1;
                break;
            }
        }
        if (is_found)
            continue;
        scan_params->chan_list[scan_params->chan_num++] = sp->chan_list[i];
    }
    scan_params->flag = EXT_CHANNEL_SCAN;
    return EXT_WIFI_OK;
}

static int wifi_scan_params_parse_ssid_scan(const ext_wifi_scan_params *sp, struct wpa_scan_params *scan_params)
{
    unsigned int len = (unsigned int)strnlen(sp->ssid, EXT_WIFI_MAX_SSID_LEN + 1);
    if ((sp->ssid_len == 0) || (len != sp->ssid_len) || (len > EXT_WIFI_MAX_SSID_LEN) ||
        (sp->ssid_len > EXT_WIFI_MAX_SSID_LEN)) {
        wpa_error_log0(MSG_ERROR, "invalid scan ssid parameter");
        return EXT_WIFI_FAIL;
    }
    scan_params->ssid_len = sp->ssid_len;
    if ((memcpy_s(scan_params->ssid, sizeof(scan_params->ssid), sp->ssid, sp->ssid_len + 1)) != EOK) {
        wpa_error_log0(MSG_ERROR, "EXT_WIFI_SSID_PREFIX_SCAN wpa_scan_params_parse memcpy_s");
        return EXT_WIFI_FAIL;
    }
    scan_params->flag = (sp->scan_type == EXT_WIFI_SSID_SCAN) ? EXT_SSID_SCAN : EXT_PREFIX_SSID_SCAN;
    return EXT_WIFI_OK;
}

static int wifi_scan_params_parse_bssid_scan(const ext_wifi_scan_params *sp, struct wpa_scan_params *scan_params)
{
    if (addr_precheck(sp->bssid) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "invalid scan bssid parameter");
        return EXT_WIFI_FAIL;
    }
    scan_params->flag = EXT_BSSID_SCAN;
    if ((memcpy_s(scan_params->bssid, ETH_ALEN, sp->bssid, ETH_ALEN)) != EOK) {
        wpa_error_log0(MSG_ERROR, "EXT_WIFI_BSSID_SCAN wpa_scan_params_parse memcpy_s");
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

static int wifi_scan_params_parse(const ext_wifi_scan_params *sp, struct wpa_scan_params *scan_params)
{
    int ret = EXT_WIFI_FAIL;

    if ((sp == NULL) || (sp->scan_type == EXT_WIFI_BASIC_SCAN)) {
        scan_params->flag = EXT_SCAN;
        wpa_warning_log0(MSG_ERROR, "uapi_wifi_sta_scan: basic scan!");
        return EXT_WIFI_OK;
    }
    switch (sp->scan_type) {
        case EXT_WIFI_CHANNEL_SCAN:
            ret = wifi_scan_params_parse_channel_scan(sp, scan_params);
            break;
        case EXT_WIFI_SSID_SCAN:
            /* fall-through */
        case EXT_WIFI_SSID_PREFIX_SCAN:
            ret = wifi_scan_params_parse_ssid_scan(sp, scan_params);
            break;
        case EXT_WIFI_BSSID_SCAN:
            ret = wifi_scan_params_parse_bssid_scan(sp, scan_params);
            break;
        default:
            wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_scan: Invalid scan_type!");
            return EXT_WIFI_FAIL;
    }
    return ret;
}

int wifi_scan(ext_wifi_iftype iftype, bool is_mesh, const ext_wifi_scan_params *sp)
{
    int ret;
    struct wpa_scan_params scan_params = { 0 };
    wpa_error_log0(MSG_ERROR, " ---> ### wpa_scan enter. \n\n\r");

    if (is_mesh == true) {
        if ((iftype == EXT_WIFI_IFTYPE_STATION) && (g_mesh_sta_flag == WPA_FLAG_OFF)) {
            wpa_error_log0(MSG_ERROR, "los_wpa_scan: mesh sta scan while g_mesh_sta_flag is off");
            return EXT_WIFI_FAIL;
        }

        if ((iftype == EXT_WIFI_IFTYPE_MESH_POINT) && (g_mesh_sta_flag != WPA_FLAG_OFF)) {
            wpa_error_log0(MSG_ERROR, "los_wpa_scan: mesh scan while g_mesh_sta_flag is on");
            return EXT_WIFI_FAIL;
        }
    } else if (iftype == EXT_WIFI_IFTYPE_STATION) {
            if (g_mesh_sta_flag != WPA_FLAG_OFF) {
                wpa_error_log0(MSG_ERROR, "los_wpa_scan: sta scan while g_mesh_sta_flag is on.");
                return EXT_WIFI_FAIL;
            }
    } else {
        wpa_error_log0(MSG_ERROR, "los_wpa_scan: Invalid scan cmd.");
        return EXT_WIFI_FAIL;
    }

    if ((wifi_dev_get(iftype) == NULL)) {
        wpa_error_log0(MSG_ERROR, "los_wpa_scan: wifi dev get fail.");
        return EXT_WIFI_FAIL;
    }
    if (wifi_scan_params_parse(sp, &scan_params) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "los_wpa_scan: scan_params_parse fail.");
        return EXT_WIFI_FAIL;
    }

    g_scan_flag = WPA_FLAG_ON;
    ret = wifi_wpa_scan(&scan_params, iftype);
    if (ret != EXT_WIFI_OK) {
        g_scan_flag = WPA_FLAG_OFF;
        wpa_error_log0(MSG_ERROR, "los_wpa_scan fail.");
        return EXT_WIFI_FAIL;
    }

    wpa_error_log1(MSG_ERROR, "los_wpa_scan: scan_params.flag: %u", (unsigned int)scan_params.flag);
    return EXT_WIFI_OK;
}

static void wifi_free_scan_param(ext_scan_stru *scan_params)
{
    if (scan_params == NULL)
        return;

    if (scan_params->ssids != NULL) {
        os_free(scan_params->ssids);
        scan_params->ssids = NULL;
    }

    if (scan_params->bssid != NULL) {
        os_free(scan_params->bssid);
        scan_params->bssid = NULL;
    }

    if (scan_params->freqs != NULL) {
        os_free(scan_params->freqs);
        scan_params->freqs = NULL;
    }

    os_free(scan_params);
    scan_params = NULL;
}

int uapi_wifi_ap_scan(void)
{
    errno_t rc;
    unsigned int ret = EXT_WIFI_OK;
    ext_scan_stru *scan_params = NULL;
    struct wpa_supplicant *wpa_s       = NULL;
    struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_AP);

    if ((wifi_dev == NULL) || (wifi_dev->ifname == NULL)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_ap_scan: get wifi dev failed");
        return EXT_WIFI_FAIL;
    }

    scan_params = (ext_scan_stru *)os_zalloc(sizeof(ext_scan_stru));
    if (scan_params == NULL) {
        return EXT_WIFI_FAIL;
    }
    scan_params->num_ssids = 1;
    scan_params->ssids = (ext_driver_scan_ssid_stru *)os_zalloc(sizeof(ext_driver_scan_ssid_stru));
    if (scan_params->ssids == NULL) {
        ret = EXT_WIFI_FAIL;
        goto EXIT;
    }

    g_scan_flag = WPA_FLAG_ON;
    rc = memset_s(&g_scan_record, sizeof(g_scan_record), 0, sizeof(g_scan_record));
    if (rc != EOK) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_ap_scan memset_s failed");
        ret = EXT_WIFI_FAIL;
        goto EXIT;
    }
    g_scan_record.flag = EXT_SCAN;
    g_usr_scanning_flag = WPA_FLAG_ON;

    ret = drv_soc_ioctl_scan(wifi_dev->ifname, scan_params);
    ret = (ret != EXT_SUCC) ? EXT_WIFI_FAIL : EXT_WIFI_OK;
    g_usr_scanning_flag = WPA_FLAG_OFF;
    g_ssid_prefix_flag = WPA_FLAG_OFF;
EXIT:
    wifi_free_scan_param(scan_params);
    return ret;
}

static int wifi_encap_raw_scan_param(ext_wifi_scan_params *sp, ext_scan_stru *scan_params)
{
    errno_t rc;

    if (sp->scan_type == EXT_WIFI_BASIC_SCAN) {
        g_scan_record.flag = EXT_SCAN;
        return EXT_WIFI_OK;
    } else if (sp->scan_type == EXT_WIFI_CHANNEL_SCAN) {
        g_scan_record.flag = EXT_CHANNEL_SCAN;
        scan_params->freqs = (int32 *)os_zalloc((sizeof(int32)));
        if (scan_params->freqs == NULL) {
            return EXT_WIFI_FAIL;
        }
        if (g_scan_record.chan_num < WIFI_24G_CHANNEL_NUMS) {
            g_scan_record.chan_list[g_scan_record.chan_num++] = sp->chan_list[0];
        }
        scan_params->freqs[0] = chan_to_freq((unsigned char)sp->chan_list[0]);
        scan_params->num_freqs = sp->chan_num;
    } else if (sp->scan_type == EXT_WIFI_SSID_SCAN || sp->scan_type == EXT_WIFI_SSID_PREFIX_SCAN) {
        g_scan_record.flag = (sp->scan_type == EXT_WIFI_SSID_SCAN) ? EXT_SSID_SCAN : EXT_PREFIX_SSID_SCAN;
        g_ssid_prefix_flag = WPA_FLAG_ON;
        rc = memcpy_s(scan_params->ssids[0].ssid, MAX_SSID_LEN, sp->ssid, sp->ssid_len + 1);
        if (rc != EOK) {
            return EXT_WIFI_FAIL;
        }
        rc = memcpy_s(g_scan_record.ssid, sizeof(g_scan_record.ssid), sp->ssid, sp->ssid_len +1);
        if (rc != EOK)
            return EXT_WIFI_FAIL;
    } else if (sp->scan_type == EXT_WIFI_BSSID_SCAN) {
        g_scan_record.flag = EXT_BSSID_SCAN;
        scan_params->bssid = (uint8 *)os_zalloc(ETH_ALEN);
        if (scan_params->bssid == NULL)
            return EXT_WIFI_FAIL;
        rc = memcpy_s(scan_params->bssid, ETH_ALEN, sp->bssid, ETH_ALEN);
        if (rc != EOK) {
            return EXT_WIFI_FAIL;
        }
        rc = memcpy_s(g_scan_record.bssid, ETH_ALEN, scan_params->bssid, ETH_ALEN);
        if (rc != EOK)
            return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

int uapi_wifi_sta_raw_scan(ext_wifi_scan_params *sp, ext_wifi_scan_no_save_cb cb)
{
    errno_t rc;
    unsigned int ret = EXT_WIFI_OK;
    ext_scan_stru *scan_params = NULL;
    struct wpa_supplicant *wpa_s       = NULL;
    struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_STATION);

    if ((wifi_dev == NULL) || (wifi_dev->ifname == NULL)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_raw_scan: get wifi dev failed");
        return EXT_WIFI_FAIL;
    }

    scan_params = (ext_scan_stru *)os_zalloc(sizeof(ext_scan_stru));
    if (scan_params == NULL) {
        return EXT_WIFI_FAIL;
    }
    scan_params->num_ssids = 1;
    scan_params->ssids = (ext_driver_scan_ssid_stru *)os_zalloc(sizeof(ext_driver_scan_ssid_stru));
    if (scan_params->ssids == NULL) {
        ret = EXT_WIFI_FAIL;
        goto EXIT;
    }

    rc = memset_s(&g_scan_record, sizeof(g_scan_record), 0, sizeof(g_scan_record));
    if (rc != EOK) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_raw_scan memset_s failed");
        ret = EXT_WIFI_FAIL;
        goto EXIT;
    }

    if (wifi_encap_raw_scan_param(sp, scan_params) != EXT_WIFI_OK) {
        ret = EXT_WIFI_FAIL;
        goto EXIT;
    }

    g_raw_scan_cb = cb;
    g_scan_flag = WPA_FLAG_ON;
    g_usr_scanning_flag = WPA_FLAG_ON;
    ret = drv_soc_ioctl_scan(wifi_dev->ifname, scan_params);
    ret = (ret != EXT_SUCC) ? EXT_WIFI_FAIL : EXT_WIFI_OK;
    g_usr_scanning_flag = WPA_FLAG_OFF;
    g_ssid_prefix_flag = WPA_FLAG_OFF;
EXIT:
    wifi_free_scan_param(scan_params);
    return ret;
}

int uapi_wifi_get_scan_flag(void)
{
    return g_scan_flag;
}

int uapi_wifi_sta_scan(void)
{
    return wifi_scan(EXT_WIFI_IFTYPE_STATION, false, NULL);
}

int uapi_wifi_sta_advance_scan(ext_wifi_scan_params *sp)
{
    return wifi_scan(EXT_WIFI_IFTYPE_STATION, false, sp);
}

int wifi_scan_result(ext_wifi_iftype iftype)
{
    struct ext_wifi_dev *wifi_dev = wifi_dev_get(iftype);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL) ||
        (wifi_dev->iftype == EXT_WIFI_IFTYPE_AP) ||
        (wifi_dev->iftype >= EXT_WIFI_IFTYPE_P2P_CLIENT)) {
        wpa_error_log0(MSG_ERROR, "wifi_scan_result: get wifi dev failed\n");
        return EXT_WIFI_FAIL;
    }

    (void)os_event_clear(g_wpa_event, ~(unsigned int)WPA_EVENT_GET_SCAN_RESULT_FLAG);
    if (wpa_cli_scan_results((struct wpa_supplicant *)(wifi_dev->priv)) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_scan_result: wpa_cli_scan_results failed.");
        return EXT_WIFI_FAIL;
    }

    wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_GET_SCAN_RESULT_FLAG");
    unsigned int ret_val;
    (void)os_event_read(g_wpa_event, WPA_EVENT_GET_SCAN_RESULT_FLAG, &ret_val,
                        WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_EVENT_DELAY_5S);
    if (ret_val == WPA_EVENT_GET_SCAN_RESULT_OK) {
        if (g_scan_result_buf == NULL)
            return EXT_WIFI_FAIL;
    } else {
        wpa_error_log1(MSG_ERROR, "os_event_read WPA_EVENT_GET_SCAN_RESULT_FLAG error ret_val =%x", ret_val);
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

int wifi_scan_result_bssid_parse(char **starttmp, void *buf, size_t *reply_len)
{
    char *endtmp = NULL;
    char addr_txt[EXT_WIFI_TXT_ADDR_LEN + 1] = { 0 };
    ext_wifi_ap_info *ap_list = (ext_wifi_ap_info *)buf;
    if ((starttmp == NULL) || (*starttmp == NULL) || (buf == NULL) || (reply_len == NULL))
        return EXT_WIFI_FAIL;

    endtmp = strchr(*starttmp, '\t');
    if (endtmp == NULL)
        return EXT_WIFI_FAIL;

    *endtmp = '\0';
    if (strncpy_s(addr_txt, sizeof(addr_txt), *starttmp, EXT_WIFI_TXT_ADDR_LEN) != EOK)
        return EXT_WIFI_FAIL;

    if (hwaddr_aton(addr_txt, ap_list->bssid))
        return EXT_WIFI_FAIL;

    *reply_len -= (size_t)(endtmp - (*starttmp) + 1);
    *starttmp = ++endtmp;
    return EXT_WIFI_OK;
}

int wifi_scan_result_freq_parse(char **starttmp, void *buf, size_t *reply_len)
{
    char *endtmp = NULL;
    ext_wifi_ap_info *ap_list = (ext_wifi_ap_info *)buf;
    if ((starttmp == NULL) || (*starttmp == NULL) || (buf == NULL) || (reply_len == NULL))
        return EXT_WIFI_FAIL;

    endtmp = strchr(*starttmp, '\t');
    if (endtmp == NULL)
        return EXT_WIFI_FAIL;

    *endtmp = '\0';
    if (los_freq_to_channel(atoi(*starttmp), &(ap_list->channel)) != EXT_WIFI_OK)
        ap_list->channel = 0;

    *reply_len -= (size_t)(endtmp - (*starttmp) + 1);
    *starttmp = ++endtmp;
    return EXT_WIFI_OK;
}

int wifi_scan_result_rssi_parse(char **starttmp, void *buf, size_t *reply_len)
{
    char *endtmp = NULL;
    ext_wifi_ap_info *ap_list = (ext_wifi_ap_info *)buf;
    if ((starttmp == NULL) || (*starttmp == NULL) || (buf == NULL) || (reply_len == NULL))
        return EXT_WIFI_FAIL;

    endtmp = strchr(*starttmp, '\t');
    if (endtmp == NULL)
        return EXT_WIFI_FAIL;

    *endtmp = '\0';
    if (**starttmp == '-') {
        ap_list->rssi = -atoi(++(*starttmp));
        *reply_len -= (size_t)(endtmp - (*starttmp) + 1 + 1);
        *starttmp = ++endtmp;
    } else {
        ap_list->rssi = atoi(*starttmp);
        *reply_len -= (size_t)(endtmp - (*starttmp) + 1);
        *starttmp = ++endtmp;
    }
    return EXT_WIFI_OK;
}

static void wifi_scan_result_parse_pairwise(const char *starttmp, ext_wifi_ap_info *ap_list)
{
    /* 与枚举值ext_wifi_pairwise定义是逆序的，优先匹配混合型 */
    char *cipher_str[] = {"\"CCMP-256\"\"+TKIP\"", "\"CCMP-256\"\"+CCMP\"", "\"CCMP-256\"", "\"CCMP\"\"+TKIP\"",
        "\"TKIP\"", "\"CCMP\""};
    int idx, num;

    num = sizeof(cipher_str) / sizeof(char *);
    for (idx= 0; idx < num; idx++) {
        if (strstr(starttmp, cipher_str[idx]) != NULL) {
            break;
        }
    }
    if ((idx >= num) || (idx >= EXT_WIFI_PAIRWISE_BUTT)) {
        ap_list->pairwise = EXT_WIFI_PARIWISE_UNKNOWN;
    } else {
        ap_list->pairwise = EXT_WIFI_PAIRWISE_BUTT - idx - 1;
    }
}

void wifi_scan_result_base_flag_parse(const char *starttmp, void *buf)
{
    ext_wifi_ap_info *ap_list = (ext_wifi_ap_info *)buf;

    if ((starttmp == NULL) || (buf == NULL))
        return;

    char *str_open      = strstr(starttmp, "OPEN");
    char *str_sae       = strstr(starttmp, "SAE");
    char *str_wpa_psk   = strstr(starttmp, "WPA-PSK");
    char *str_wpa2_psk  = strstr(starttmp, "WPA2-PSK");
    char *str_wpa       = strstr(starttmp, "WPA-EAP");
    char *str_wpa2      = strstr(starttmp, "WPA2-EAP");
#ifdef CONFIG_IEEE80211R
    char *str_ft        = strstr(starttmp, "FT");
#endif
    char *str_wep       = strstr(starttmp, "WEP");
    char *str_wps       = strstr(starttmp, "WPS");
    char *str_wmm       = strstr(starttmp, "WMM");
    char *str_ext_mesh  = strstr(starttmp, "EXT_MESH");
    char *str_rsn_psk   = strstr(starttmp, "RSN-PSK");
    char *str_wapi_psk  = strstr(starttmp, "WAPI-PSK");
    char *str_eap_sha256 = strstr(starttmp, "EAP-SHA256");
    char *str_eap_suiteb_192 = strstr(starttmp, "EAP-SUITE-B-192");
    char *str_owe = strstr(starttmp, "WPA2-OWE");

    if (str_sae != NULL) {
        if (str_wpa2_psk != NULL)
            ap_list->auth = EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX;
        else
            ap_list->auth = EXT_WIFI_SECURITY_SAE;
    } else if ((str_wpa_psk != NULL) && ((str_wpa2_psk != NULL) || (str_rsn_psk != NULL))) {
        ap_list->auth = EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX;
    } else if (str_wpa_psk != NULL) {
        ap_list->auth = EXT_WIFI_SECURITY_WPAPSK;
    } else if ((str_wpa2_psk != NULL) || (str_rsn_psk != NULL)) {
        ap_list->auth = EXT_WIFI_SECURITY_WPA2PSK;
    } else if (str_wpa != NULL) {
        ap_list->auth = EXT_WIFI_SECURITY_WPA;
    } else if (str_wpa2 != NULL) {
        if ((str_eap_sha256 != NULL) && (str_eap_suiteb_192 != NULL))
            ap_list->auth = EXT_WIFI_SECURITY_WPA3_WPA2_MIX;
        else if (str_eap_sha256 != NULL)
            ap_list->auth = EXT_WIFI_SECURITY_WPA3;
        else if (str_eap_suiteb_192 != NULL)
            ap_list->auth = EXT_WIFI_SECURITY_WPA3_192BIT;
        else
            ap_list->auth = EXT_WIFI_SECURITY_WPA2;
    } else if (str_wep != NULL) {
        ap_list->auth = EXT_WIFI_SECURITY_WEP;
    } else if (str_open != NULL) {
        ap_list->auth = EXT_WIFI_SECURITY_OPEN;
    } else if (str_owe != NULL) {
        ap_list->auth = EXT_WIFI_SECURITY_OWE;
    } else if (str_wapi_psk != NULL) {
        ap_list->auth = EXT_WIFI_SECURITY_WAPI_PSK;
    } else {
        ap_list->auth = EXT_WIFI_SECURITY_UNKNOWN;
    }
    if (str_wps != NULL)
        ap_list->wps_flag = WPA_FLAG_ON;

    if (str_wmm != NULL)
        ap_list->wmm = WPA_FLAG_ON;

    if (str_ext_mesh != NULL)
        ap_list->ext_mesh_flag = WPA_FLAG_ON;

#ifdef CONFIG_IEEE80211R
    if (str_ft != NULL)
        ap_list->ft_flag = WPA_FLAG_ON;
#endif
    wifi_scan_result_parse_pairwise(starttmp, ap_list);
}

int wifi_scan_result_filter_parse(const void *buf)
{
    const ext_wifi_ap_info *ap_list = (const ext_wifi_ap_info *)buf;
    unsigned char ssid_flag;
    unsigned char chl_flag = 0;
    unsigned char prefix_ssid_flag;
    unsigned char bssid_flag;
    unsigned char i;
    prefix_ssid_flag = (unsigned char)(os_strncmp(g_scan_record.ssid, ap_list->ssid, strlen(g_scan_record.ssid)) == 0);

    for (i = 0; i < g_scan_record.chan_num; i++) {
        if (g_scan_record.chan_list[i] == ap_list->channel) {
            chl_flag = 1;
            break;
        }
    }

    ssid_flag = (unsigned char)(os_strcmp(g_scan_record.ssid, ap_list->ssid) == 0);
    bssid_flag = (unsigned char)(os_memcmp(g_scan_record.bssid, ap_list->bssid, sizeof(ap_list->bssid)) == 0);
    return (g_scan_record.flag == EXT_SCAN_UNSPECIFIED) ||
           (g_scan_record.flag == EXT_SCAN) ||
           ((g_scan_record.flag == EXT_PREFIX_SSID_SCAN) && prefix_ssid_flag) ||
           ((g_scan_record.flag == EXT_CHANNEL_SCAN) && chl_flag) ||
           ((g_scan_record.flag == EXT_BSSID_SCAN) && bssid_flag) ||
           ((g_scan_record.flag == EXT_SSID_SCAN) && ssid_flag);
}

int wifi_scan_result_ssid_parse(char **starttmp, void *buf, size_t *reply_len)
{
    char *endtmp = NULL;
    ext_wifi_ap_info *ap_list = (ext_wifi_ap_info *)buf;

    if ((starttmp == NULL) || (*starttmp == NULL) || (buf == NULL) || (reply_len == NULL))
        return EXT_WIFI_FAIL;

    endtmp = strchr(*starttmp, '\n');
    if (endtmp == NULL)
        return EXT_WIFI_FAIL;

    *endtmp = '\0';
    if (strncpy_s(ap_list->ssid, (size_t)sizeof(ap_list->ssid), *starttmp, (size_t)(endtmp - *starttmp)) != (int)EOK) {
        ap_list->ssid[(sizeof(ap_list->ssid)) - 1] = '\0';
        *reply_len -= (size_t)(endtmp - (*starttmp) + 1);
        *starttmp = ++endtmp;
        return EXT_WIFI_FAIL;
    }
    ap_list->ssid[(sizeof(ap_list->ssid)) - 1] = '\0';
    *reply_len -= (size_t)(endtmp - (*starttmp) + 1);
    *starttmp = ++endtmp;
    return wifi_scan_result_filter_parse(ap_list) ? EXT_WIFI_OK : EXT_WIFI_FAIL;
}

/*************************************************************************************
 buf content format:
 bssid / frequency / signal level / flags / ssid
 xx:xx:xx:xx:xx:xx 2462 -21 [WPA2-PSK-CCMP][ESS] ssid-test
 xx:xx:xx:xx:xx:xx 2437 -35 [WPA2-PSK-CCMP][ESS] ssid
 xx:xx:xx:xx:xx:xx 2412 -37 [WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP-preauth][ESS] abc
 ***************************************************************************************/
static int wifi_scan_results_parse(ext_wifi_ap_info *ap_list, unsigned int *ap_num)
{
    char *starttmp = NULL;
    char *endtmp   = NULL;
    int count = 0;
    size_t reply_len = g_result_len;
    char *buf = g_scan_result_buf;
    int ret = EXT_WIFI_FAIL;
    int max_element = (int)WPA_MIN(*ap_num, WIFI_SCAN_AP_LIMIT);

    if (buf == NULL)
        goto EXIT;
    starttmp = strchr(buf, '\n');
    if (starttmp == NULL)
        goto EXIT;

    *starttmp++ = '\0';
    reply_len -= (size_t)(starttmp - buf);
    while (count < max_element && reply_len > 0) {
        if (wifi_scan_result_bssid_parse(&starttmp, &ap_list[count], &reply_len) == EXT_WIFI_FAIL) {
            ret = EXT_WIFI_OK; // set ok if it is the end of the string
            goto EXIT;
        }
        if ((wifi_scan_result_freq_parse(&starttmp, &ap_list[count], &reply_len) == EXT_WIFI_FAIL) ||
            (wifi_scan_result_rssi_parse(&starttmp, &ap_list[count], &reply_len) == EXT_WIFI_FAIL))
            goto EXIT;
        endtmp = strchr(starttmp, '\t');
        if (endtmp == NULL)
            goto EXIT;

        *endtmp = '\0';
        wifi_scan_result_base_flag_parse(starttmp, &ap_list[count]);

        reply_len -= (size_t)((endtmp - starttmp) + 1);
        starttmp = ++endtmp;
        if (wifi_scan_result_ssid_parse(&starttmp, &ap_list[count], &reply_len) == EXT_WIFI_OK)
            count++;
        else
            (void)memset_s(&ap_list[count], sizeof(*ap_list), 0, sizeof(*ap_list));
    }
    ret = EXT_WIFI_OK;
EXIT:
    (void)memset_s(&g_scan_record, sizeof(g_scan_record), 0, sizeof(g_scan_record));
    *ap_num = (unsigned int)count;
    g_scan_result_buf = NULL;
    (void)os_event_write(g_wpa_event, WPA_EVENT_SCAN_RESULT_FREE_OK);
    return ret;
}

__attribute__((weak)) int uapi_wifi_sta_scan_results(ext_wifi_ap_info *ap_list, unsigned int *ap_num)
{
    if ((ap_list == NULL) || (ap_num == NULL) || (g_mesh_sta_flag == WPA_FLAG_ON))
        return EXT_WIFI_FAIL;

    if (wifi_dev_get(EXT_WIFI_IFTYPE_STATION) == NULL)
        return EXT_WIFI_FAIL;

    if (memset_s(ap_list, sizeof(ext_wifi_ap_info) * (*ap_num), 0, sizeof(ext_wifi_ap_info) * (*ap_num)) != EOK)
        return EXT_WIFI_FAIL;
    if (wifi_scan_result(EXT_WIFI_IFTYPE_STATION) == EXT_WIFI_OK)
        return wifi_scan_results_parse(ap_list, ap_num);
    else {
        (void)memset_s(&g_scan_record, sizeof(g_scan_record), 0, sizeof(g_scan_record));
        *ap_num = 0;
        (void)os_event_write(g_wpa_event, WPA_EVENT_SCAN_RESULT_FREE_OK);
        return EXT_WIFI_FAIL;
    }
}

int wpa_cli_scan_results_clear(struct wpa_supplicant *wpa_s)
{
    char *cmd[] = {"BSS_FLUSH "};

    return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wifi_scan_result_clear(ext_wifi_iftype iftype)
{
    struct ext_wifi_dev *wifi_dev = wifi_dev_get(iftype);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL) ||
        (wifi_dev->iftype == EXT_WIFI_IFTYPE_AP) ||
        (wifi_dev->iftype >= EXT_WIFI_IFTYPE_P2P_CLIENT)) {
        wpa_error_log0(MSG_ERROR, "wifi_scan_result_clear: get wifi dev failed\n");
        return EXT_WIFI_FAIL;
    }

    (void)os_event_clear(g_wpa_event, ~(unsigned int)WPA_EVENT_SCAN_RESULT_CLEAR_OK);
    if (wpa_cli_scan_results_clear((struct wpa_supplicant *)(wifi_dev->priv)) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_scan_result_clear: wpa_cli_scan_results failed.");
        return EXT_WIFI_FAIL;
    }

    wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_SCAN_RESULT_CLEAR_OK");
    unsigned int ret_val;
    (void)os_event_read(g_wpa_event, WPA_EVENT_SCAN_RESULT_CLEAR_OK, &ret_val,
                        WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_EVENT_DELAY_5S);

    return EXT_WIFI_OK;
}

__attribute__((weak)) int uapi_wifi_sta_scan_results_clear(void)
{
    if (g_mesh_sta_flag == WPA_FLAG_ON)
        return EXT_WIFI_FAIL;

    if (wifi_dev_get(EXT_WIFI_IFTYPE_STATION) == NULL)
        return EXT_WIFI_FAIL;

    return wifi_scan_result_clear(EXT_WIFI_IFTYPE_STATION);
}

#ifdef CONFIG_WNM
int uapi_wifi_wnm_bss_query(int reason_code, int candidate_list)
{
    struct ext_wifi_dev *wifi_dev = wifi_dev_get(EXT_WIFI_IFTYPE_STATION);
    char buf[WPA_CMD_BUF_SIZE] = { 0 };
    int ret = 0;
    int len = 0;

    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL) ||
        (wifi_dev->iftype == EXT_WIFI_IFTYPE_AP) ||
        (wifi_dev->iftype >= EXT_WIFI_IFTYPE_P2P_CLIENT)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_wnm_bss_query: get wifi dev failed\n");
        return EXT_WIFI_FAIL;
    }

    if (candidate_list != 0) {
        ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%d list", reason_code);
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "uapi_wifi_wnm_bss_query:: snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
    } else {
        ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%d", reason_code);
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "uapi_wifi_wnm_bss_query:: snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
    }
    len = ret;

    printf("uapi_wifi_wnm_bss_query::send_bss_query[%s], len[%d], buf_len[%d]\r\n", buf, len, strlen(buf));
    (void)os_event_clear(g_wpa_event, ~(unsigned int)WPA_EVENT_STA_WNM_BSS_QUERY);
    if (wpa_cli_wnm_bss_query((struct wpa_supplicant *)(wifi_dev->priv), buf, len) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_wnm_bss_query: wpa_cli_cmd_wnm_bss_query failed.");
        return EXT_WIFI_FAIL;
    }

    wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_STA_WNM_BSS_QUERY");
    unsigned int ret_val;
    (void)os_event_read(g_wpa_event, WPA_EVENT_STA_WNM_BSS_QUERY, &ret_val,
                        WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_EVENT_DELAY_5S);

    return EXT_WIFI_OK;
}
#endif

int uapi_wifi_csi_check_param(const ext_csi_config *config)
{
    /* 0 RA 1 TA */
    if (config->cfg_match_ta_ra_sel != 0 && config->cfg_match_ta_ra_sel != 1) {
        wpa_error_log0(MSG_ERROR, "{uapi_wifi_csi_check_param::get addr_filter_type err!}");
        return EXT_WIFI_FAIL;
    }

    if (addr_precheck(config->mac_addr) != EXT_WIFI_OK) {
        wpa_error_log1(MSG_ERROR, "{uapi_wifi_csi_check_param::get mac err, maclen:%d!}",
            strlen(config->mac_addr));
        return EXT_WIFI_FAIL;
    }

    /* user frame type support, range:0~7, bit0 control, bit1 manament, bit2 data */
    if (config->frame_type_filter_bitmap > 7) {
        wpa_error_log0(MSG_ERROR, "{uapi_wifi_csi_check_param::get frame_type err!}");
        return EXT_WIFI_FAIL;
    }

    /* sub_type_filter switch, 0 close, 1 open */
    if (config->sub_type_filter_enable != 0 && config->sub_type_filter_enable != 1) {
        wpa_error_log0(MSG_ERROR, "{uapi_wifi_csi_check_param::get frame sub type enable err!}");
        return EXT_WIFI_FAIL;
    }

    /* range 0 ~ 15 */
    if (config->sub_type_filter > 15) {
        wpa_error_log0(MSG_ERROR, "{uapi_wifi_csi_check_param::get frame sub type filter err!}");
        return EXT_WIFI_FAIL;
    }

    /* range 0 ~ 63 */
    if (config->ppdu_filter_bitmap > 63) {
        wpa_error_log0(MSG_ERROR, "{uapi_wifi_csi_check_param::get frame ppdu_filter_bitmap err!}");
        return EXT_WIFI_FAIL;
    }

    /* range 0 ~ 4095 */
    if (config->period > 4095) {
        wpa_error_log0(MSG_ERROR, "{uapi_wifi_csi_check_param::get period err!}");
        return EXT_WIFI_FAIL;
    }

    return EXT_WIFI_OK;
}

int uapi_wifi_csi_set_config(const char *ifname, const ext_csi_config *config)
{
    int ret;
    /* range 0 ~ 3 */
    if (config->user_idx > 3) {
        wpa_error_log0(MSG_ERROR, "{uapi_wifi_csi_set_config::get user_idx err!}");
        return EXT_WIFI_FAIL;
    }

    if (config->enable != 0 && config->enable != 1) {
        wpa_error_log0(MSG_ERROR, "{uapi_wifi_csi_set_config::get enable err!}");
        return EXT_WIFI_FAIL;
    }

    /* only when enable is 1, check other param */
    if (config->enable == 1) {
        ret = uapi_wifi_csi_check_param(config);
        if (ret != EXT_WIFI_OK) {
            wpa_error_log0(MSG_ERROR, "{uapi_wifi_csi_set_config::csi params check failed!}");
            return EXT_WIFI_FAIL;
        }
    }

    if (memcpy_s(g_csi_ifname, WIFI_IFNAME_MAX_SIZE, ifname, WIFI_IFNAME_MAX_SIZE) != EOK) {
        (void)memset_s(g_csi_ifname, WIFI_IFNAME_MAX_SIZE, 0, WIFI_IFNAME_MAX_SIZE);
        return EXT_WIFI_FAIL;
    }

    if (drv_soc_set_csi_config(ifname, config) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_set_usr_app_ie_internal: set csi config failed.");
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

void uapi_csi_register_data_report_cb(wifi_csi_data_cb data_cb)
{
    wal_csi_register_data_report_cb(data_cb);
}
int uapi_wifi_csi_start(void)
{
    int ret;
    char *ifname = NULL;
    int switch_flag = WIFI_CSI_SWITCH_ON;

    ifname = g_csi_ifname;
    ret = drv_soc_csi_switch(ifname, &switch_flag);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_csi_start: start csi failed.");
    }
    return ret;
}

int uapi_wifi_csi_stop(void)
{
    int ret;
    char *ifname = NULL;
    int switch_flag = WIFI_CSI_SWITCH_OFF;

    ifname = g_csi_ifname;
    ret = drv_soc_csi_switch(ifname, &switch_flag);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_csi_start: stop csi failed.");
    }
    (void)memset_s(g_csi_ifname, WIFI_IFNAME_MAX_SIZE, 0, WIFI_IFNAME_MAX_SIZE);
    return ret;
}

int uapi_wifi_psk_calc(ext_wifi_sta_psk_config psk_config, unsigned char *get_psk, unsigned int psk_len)
{
    unsigned char psk_calc[EXT_WIFI_STA_PSK_LEN] = { 0 };
    struct ext_wifi_dev *wifi_dev = NULL;

    if ((get_psk == NULL) || (psk_len != EXT_WIFI_STA_PSK_LEN))
        return EXT_WIFI_FAIL;

    if ((strnlen((char *)psk_config.ssid, EXT_WIFI_MAX_SSID_LEN + 1) > EXT_WIFI_MAX_SSID_LEN) ||
        (strlen((char *)psk_config.ssid) < 1)) {
        wpa_error_log0(MSG_ERROR, "invalid ssid length.");
        return EXT_WIFI_FAIL;
    }
    if ((strnlen(psk_config.key, EXT_WIFI_AP_KEY_LEN + 1) > EXT_WIFI_AP_KEY_LEN) ||
        (strlen(psk_config.key) < 1)) {
        wpa_error_log0(MSG_ERROR, "invalid key length.");
        return EXT_WIFI_FAIL;
    }

    wifi_dev = wifi_dev_get(EXT_WIFI_IFTYPE_STATION);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_psk_calc: get wifi dev failed\n");
        return EXT_WIFI_FAIL;
    }
    if (memset_s(get_psk, psk_len, 0, psk_len) != EOK) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_psk_calc: memset_s failed\n");
        return EXT_WIFI_FAIL;
    }
    if (pbkdf2_sha1(psk_config.key, psk_config.ssid, strlen((char *)psk_config.ssid),
        WPA_STA_ITERA, psk_calc, WPA_STA_PMK_LEN) != EXT_WIFI_OK) {
        return EXT_WIFI_FAIL;
    }
    if (memcpy_s(get_psk, psk_len, psk_calc, EXT_WIFI_STA_PSK_LEN) != EOK) {
        (void)memset_s(psk_calc, sizeof(psk_calc), 0, sizeof(psk_calc));
        (void)memset_s(get_psk, psk_len, 0, psk_len);
        return EXT_WIFI_FAIL;
    }
    (void)memset_s(psk_calc, sizeof(psk_calc), 0, sizeof(psk_calc));
    return EXT_WIFI_OK;
}

int uapi_wifi_psk_calc_and_store(ext_wifi_sta_psk_config psk_config)
{
    if (uapi_wifi_psk_calc(psk_config, g_quick_conn_psk, EXT_WIFI_STA_PSK_LEN) != EXT_WIFI_OK)
        return EXT_WIFI_FAIL;

    return EXT_WIFI_OK;
}

static int los_wifi_connect_prepare(const struct wpa_assoc_request *assoc,
                                    struct wpa_supplicant *wpa_s,
                                    const char *network_id_txt,
                                    bool is_fast_connnect)
{
    char addr_txt[EXT_WIFI_TXT_ADDR_LEN + 1] = { 0 };
    char fre_buff[WPA_CMD_MIN_SIZE] = { 0 };
    int ret;

    if ((wpa_cli_disconnect(wpa_s) != EXT_WIFI_OK) ||
        (wpa_cli_remove_network(wpa_s, network_id_txt, WPA_NETWORK_ID_TXT_LEN) != EXT_WIFI_OK) ||
        (wpa_cli_add_network(wpa_s) != EXT_WIFI_OK)) {
        wpa_error_log0(MSG_ERROR, "los_wifi_connect_prepare re-add network faild.\n");
        return EXT_WIFI_FAIL;
    }

    if (assoc->scan_type & (unsigned char)SOC_WPA_BIT_SCAN_BSSID) { // set bssid
        if (snprintf_s(addr_txt, sizeof(addr_txt), sizeof(addr_txt) - 1, MACSTR, MAC2STR(assoc->bssid)) < 0) {
            wpa_error_log0(MSG_ERROR, "los_wifi_connect_prepare addr_txt snprintf_s faild.\n");
            return EXT_WIFI_FAIL;
        }

        if (wpa_cli_set_network(wpa_s, network_id_txt, WPA_NETWORK_ID_TXT_LEN, "bssid", addr_txt,
            sizeof(addr_txt)) != EXT_WIFI_OK) {
            wpa_error_log0(MSG_ERROR, "wpa_cli_set_network bssid faild.\n");
            return EXT_WIFI_FAIL;
        }
    }

    if (assoc->scan_type & (unsigned char)SOC_WPA_BIT_SCAN_SSID) { // set ssid
        if (strlen(assoc->ssid) > WPA_MAX_SSID_LEN) {
            wpa_error_log0(MSG_ERROR, "los_wifi_connect_prepare: ssid len error.\n");
            return EXT_WIFI_FAIL;
        }
        if (wpa_cli_set_network(wpa_s, network_id_txt, WPA_NETWORK_ID_TXT_LEN, "ssid", assoc->ssid,
            sizeof(assoc->ssid)) != EXT_WIFI_OK) {
            wpa_error_log0(MSG_ERROR, "wpa_cli_set_network ssid faild.\n");
            return EXT_WIFI_FAIL;
        }
    }

    if (is_fast_connnect) { // set frequence
        if (snprintf_s(fre_buff, sizeof(fre_buff), sizeof(fre_buff) - 1, "%d",
            chan_to_freq(assoc->channel)) < 0) {
            wpa_error_log0(MSG_ERROR, "los_wifi_connect_prepare: fre_buff snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
        if (wpa_cli_set_network(wpa_s, network_id_txt, WPA_NETWORK_ID_TXT_LEN, "frequency", fre_buff,
            sizeof(fre_buff)) != EXT_WIFI_OK) {
            wpa_error_log0(MSG_ERROR, "wpa_cli_set_network frequency faild.\n");
            return EXT_WIFI_FAIL;
        }
    }
    // set sta mode
    if (wpa_cli_set_network(wpa_s, network_id_txt, WPA_NETWORK_ID_TXT_LEN, "mode", "0", strlen("0")) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wpa_cli_set_network mode faild.\n");
        return EXT_WIFI_FAIL;
    }

    // set security type
#ifdef LOS_CONFIG_WPA_ENTERPRISE
    if (wifi_sta_enterprise_check(assoc->auth) == EXT_WIFI_OK)
        ret = wifi_sta_set_network_ent(wpa_s, network_id_txt, assoc);
    else
#endif /* LOS_CONFIG_WPA_ENTERPRISE */
        ret = wifi_sta_set_network_psk(wpa_s, network_id_txt, assoc, is_fast_connnect);

    if (ret != EXT_WIFI_OK) {
        (void)wpa_cli_remove_network(wpa_s, network_id_txt, WPA_NETWORK_ID_TXT_LEN);
        return EXT_WIFI_FAIL;
    }

    return EXT_WIFI_OK;
}

static int los_wifi_connect(struct wpa_assoc_request *assoc, bool is_fast_connnect)
{
    struct ext_wifi_dev *wifi_dev = NULL;
    char network_id_txt[WPA_NETWORK_ID_TXT_LEN + 1] = { 0 };
    int ret;
    unsigned int ret_val;

    wpa_error_log0(MSG_ERROR, " ---> ### los_wifi_connect enter. \n\n\r");

    wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_STATION);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "los_wifi_connect: get wifi dev failed\n");
        return EXT_WIFI_FAIL;
    }
    if (assoc->scan_type == (unsigned char)SOC_WPA_BIT_SCAN_UNKNOW) {
        wpa_error_log0(MSG_ERROR, "los_wifi_connect: please set ssid or bssid\n");
        return EXT_WIFI_FAIL;
    }
    ret = snprintf_s(network_id_txt, sizeof(network_id_txt), sizeof(network_id_txt) - 1, "%d", wifi_dev->network_id);
    if (ret < 0) {
        wpa_error_log0(MSG_ERROR, "los_wifi_connect: network_id_txt snprintf_s failed");
        return EXT_WIFI_FAIL;
    }
    if (g_assoc_auth != EXT_WIFI_SEC_TYPE_INVALID) {
        assoc->auth = g_assoc_auth;
    }
    if (los_wifi_connect_prepare(assoc, (struct wpa_supplicant *)(wifi_dev->priv), network_id_txt, is_fast_connnect)
        < 0)
        return EXT_WIFI_FAIL;

    g_fast_connect_flag = is_fast_connnect ? WPA_FLAG_ON : WPA_FLAG_OFF;
    g_connecting_flag = WPA_FLAG_ON;
    (void)os_event_clear(g_wpa_event, ~(unsigned int)WPA_EVENT_QUICK_CONNECT_FLAG);

    if (wpa_cli_select_network((struct wpa_supplicant *)(wifi_dev->priv),
                               network_id_txt, WPA_NETWORK_ID_TXT_LEN) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "los_wifi_connect: wpa_cli_select_network failed");
        return EXT_WIFI_FAIL;
    }
    if (is_fast_connnect) {
        wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_QUICK_CONNECT_FLAG");
        (void)os_event_read(g_wpa_event, WPA_EVENT_QUICK_CONNECT_FLAG, &ret_val,
                            WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_EVENT_DELAY_5S);
        g_fast_connect_flag = WPA_FLAG_OFF;
        g_connecting_flag = WPA_FLAG_OFF;
        if (ret_val != WPA_EVENT_QUICK_CONNECT_OK) {
            wpa_error_log1(MSG_ERROR, "os_event_read WPA_EVENT_QUICK_CONNECT_FLAG failed ret_val = %x", ret_val);
            return EXT_WIFI_FAIL;
        }
    }

    return EXT_WIFI_OK;
}

static int wifi_sta_connect_param_check(const ext_wifi_assoc_request *req)
{
    if (req == NULL) {
        wpa_error_log0(MSG_ERROR, "input param is NULL.");
        return EXT_WIFI_FAIL;
    }
    if ((strnlen(req->ssid, EXT_WIFI_MAX_SSID_LEN + 1) > EXT_WIFI_MAX_SSID_LEN) ||
        (strnlen(req->key, EXT_WIFI_MAX_KEY_LEN + 1) > EXT_WIFI_MAX_KEY_LEN)) {
        wpa_error_log0(MSG_ERROR, "invalid ssid/key.");
        return EXT_WIFI_FAIL;
    }
    if ((req->auth >= EXT_WIFI_SECURITY_UNKNOWN)) {
        wpa_error_log0(MSG_ERROR, "invalid auth.");
        return EXT_WIFI_FAIL;
    }
    if ((wifi_is_need_psk(req->auth) == 1) && (os_strlen(req->key) > WPA_MAX_KEY_LEN)) {
        wpa_error_log0(MSG_ERROR, "invalid key length.");
        return EXT_WIFI_FAIL;
    }
    if (is_broadcast_ether_addr(req->bssid)) {
        wpa_error_log0(MSG_ERROR, "invalid bssid.");
        return EXT_WIFI_FAIL;
    }
    if (req->pairwise >= EXT_WIFI_PAIRWISE_BUTT) {
        wpa_error_log0(MSG_ERROR, "invalid pairwise.");
        return EXT_WIFI_FAIL;
    }
    if (((req->auth == EXT_WIFI_SECURITY_SAE) ||
        (req->auth == EXT_WIFI_SECURITY_OWE) ||
        (req->auth == EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX)) &&
        (req->pairwise == EXT_WIFI_PAIRWISE_TKIP)) {
        wpa_error_log0(MSG_ERROR, "pairwise and auth do not match.");
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

static int wifi_key_has_ctrl_char(const char *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (data[i] < 32 || data[i] == 127) { /* less than 32 or equal 127 is ctrl char */
            return EXT_WIFI_FAIL;
        }
    }
    return EXT_WIFI_OK;
}

static int wifi_key_check(const ext_wifi_assoc_request *req,
                          const ext_wifi_fast_assoc_request *fast_request,
                          struct wpa_assoc_request *assoc,
                          bool is_fast_connnect)
{
    size_t keysize = strnlen(req->key, EXT_WIFI_MAX_KEY_LEN + 1);

    if (req->auth == EXT_WIFI_SECURITY_WAPI_PSK) {
        if ((keysize > WPA_MAX_KEY_LEN) || (keysize < WPA_MIN_KEY_LEN)) {
            wpa_error_log0(MSG_ERROR, "Invalid wapi size");
            return EXT_WIFI_FAIL;
        }
        if (req->hex_flag) {
            if ((keysize & 0x01) || (is_hex_string(req->key, keysize) == EXT_WIFI_FAIL)) {
                wpa_error_log0(MSG_ERROR, "Invalid wapi hex character");
                return EXT_WIFI_FAIL;
            }
        }
    } else {
        /* When key size is 64, we think this is psk set in WPA. Psk is hexadecimal. */
        if ((keysize == WPA_MAX_KEY_LEN) && (req->auth != EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX) &&
            (req->auth != EXT_WIFI_SECURITY_SAE) && (is_hex_string(req->key, WPA_MAX_KEY_LEN) == EXT_WIFI_FAIL)) {
            wpa_error_log0(MSG_ERROR, "Invalid hex character");
            return EXT_WIFI_FAIL;
        }
    }

    /*
     * when the connection is sta_connect,the len of key should not be zero.
     * when the connection is sta_fast_connect and the psk is not used, the len of key should not be zero
     */
    if ((is_fast_connnect == false) || (fast_request->psk_flag == EXT_WIFI_WPA_PSK_NOT_USE)) {
        if (keysize == 0) {
            wpa_error_log0(MSG_ERROR, "key length is zero.");
            return EXT_WIFI_FAIL;
        }
        if ((req->auth == EXT_WIFI_SECURITY_WPAPSK) || (req->auth == EXT_WIFI_SECURITY_WPA2PSK) ||
            (req->auth == EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX)) {
            if (wifi_key_has_ctrl_char(req->key, keysize) != EXT_WIFI_OK) {
                wpa_error_log0(MSG_ERROR, "Invalid character");
                return EXT_WIFI_FAIL;
            }
        }
        if (memcpy_s(assoc->key, sizeof(assoc->key), req->key, keysize + 1) != EOK)
            return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

static int wifi_sta_connect(const ext_wifi_assoc_request *req,
                            const ext_wifi_fast_assoc_request *fast_request,
                            bool is_fast_connnect)
{
    int ret;
    struct wpa_assoc_request assoc;

    ret = wifi_sta_connect_param_check(req);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "los_los_wifi_connect_precheck failed.");
        return EXT_WIFI_FAIL;
    }
    assoc.scan_type = SOC_WPA_BIT_SCAN_UNKNOW;
    if ((strlen(req->ssid) > 0) && (strlen(req->ssid) <= EXT_WIFI_MAX_SSID_LEN)) {
        assoc.scan_type |= (unsigned char)SOC_WPA_BIT_SCAN_SSID;
        if (memcpy_s(assoc.ssid, sizeof(assoc.ssid), req->ssid, strlen(req->ssid) + 1) != EOK)
            return EXT_WIFI_FAIL;
    }
    if (!is_zero_ether_addr(req->bssid))  {
        assoc.scan_type |= (unsigned char)SOC_WPA_BIT_SCAN_BSSID;
        if (memcpy_s(assoc.bssid, sizeof(assoc.bssid), req->bssid, sizeof(req->bssid)) != EOK)
            return EXT_WIFI_FAIL;
    } else {
        if (memset_s(assoc.bssid, sizeof(assoc.bssid), 0, sizeof(req->bssid)) != EOK)
            return EXT_WIFI_FAIL;
    }
    if ((wifi_is_need_psk(req->auth) == 1) && (wifi_sta_enterprise_check(req->auth) != EXT_WIFI_OK)) {
        if (wifi_key_check(req, fast_request, &assoc, is_fast_connnect) != EXT_WIFI_OK) {
            wpa_error_log0(MSG_ERROR, "key check failed");
            return EXT_WIFI_FAIL;
        }
    } else {
        if (strlen(req->key) != 0) {
            wpa_error_log0(MSG_ERROR, "invalid key.length.");
            return EXT_WIFI_FAIL;
        }
    }
    assoc.auth = req->auth;
    assoc.wpa_pairwise = req->pairwise;
    assoc.hex_flag = req->hex_flag;
    assoc.ft_flag = req->ft_flag;
    if (is_fast_connnect) {
        assoc.channel = fast_request->channel;
        assoc.psk_flag = fast_request->psk_flag;
    }

    ret = los_wifi_connect(&assoc, is_fast_connnect);
    (void)memset_s(&assoc, sizeof(assoc), 0, sizeof(assoc));
    if (ret != EXT_WIFI_OK) {
        wpa_error_log1(MSG_ERROR, "%d:los_wifi_connect failed.", (int)is_fast_connnect);
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

int uapi_wifi_sta_connect(const ext_wifi_assoc_request *req)
{
    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }
    return wifi_sta_connect(req, NULL, false);
}

int wifi_channel_check(unsigned char chan)
{
    char cc[WPA_COUNTRY_CODE_LEN] = "";
    unsigned char len = WPA_COUNTRY_CODE_LEN;
    unsigned char max;
    int ret = EXT_WIFI_OK;

#ifdef CONFIG_ACS
    if (chan > 14) /* 14: chan is allowed to be 0 to 14. */
        return EXT_WIFI_FAIL;
#else
    if (chan < 1)
        return EXT_WIFI_FAIL;
#endif

    if (uapi_wifi_get_country(cc, &len) == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "fail to get country.");
        return EXT_WIFI_FAIL;
    }
    if (!strncmp(cc, WPA_COUNTRY_CODE_USA, WPA_COUNTRY_CODE_LEN - 1))
        max = WPA_CHANNEL_MAX_USA;
    else if (!strncmp(cc, WPA_COUNTRY_CODE_ZZ, WPA_COUNTRY_CODE_LEN - 1) ||
        !strncmp(cc, WPA_COUNTRY_CODE_JAPAN, WPA_COUNTRY_CODE_LEN - 1))
        max = WPA_CHANNEL_MAX_JAPAN;
    else
        max = WPA_CHANNEL_MAX_OTHERS;

    if (chan > max)
        ret = EXT_WIFI_FAIL;
    return ret;
}

int uapi_wifi_sta_fast_connect(const ext_wifi_fast_assoc_request *fast_request)
{
    int ret;
    if (fast_request == NULL) {
        wpa_error_log0(MSG_ERROR, "input param is NULL.");
        return EXT_WIFI_FAIL;
    }
    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }
    if (wifi_channel_check(fast_request->channel) == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "invalid channel.");
        return EXT_WIFI_FAIL;
    }

    if (fast_request->psk_flag == EXT_WIFI_WPA_PSK_USE_OUTER) {
        if (fast_request->psk[0] == '\0') {
            wpa_error_log0(MSG_ERROR, "invalid psk.");
            return EXT_WIFI_FAIL;
        }
        if (memcpy_s(g_quick_conn_psk, sizeof(g_quick_conn_psk), fast_request->psk, sizeof(fast_request->psk)) != EOK) {
            wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_fast_connect memcpy_s failed.");
            return EXT_WIFI_FAIL;
        }
    }
    if ((fast_request->psk_flag != EXT_WIFI_WPA_PSK_NOT_USE) &&
        (fast_request->req.auth != EXT_WIFI_SECURITY_WPAPSK) &&
        (fast_request->req.auth != EXT_WIFI_SECURITY_WPA2PSK) &&
        (fast_request->req.auth != EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX)) {
        wpa_error_log0(MSG_ERROR, "this auth not support set psk");
        return EXT_WIFI_FAIL;
    }
    g_quick_conn_psk_flag = (fast_request->psk_flag == EXT_WIFI_WPA_PSK_NOT_USE) ? WPA_FLAG_OFF : WPA_FLAG_ON;
    ret = wifi_sta_connect(&fast_request->req, fast_request, true);
    if ((fast_request->psk_flag != EXT_WIFI_WPA_PSK_NOT_USE) &&
        (memset_s(g_quick_conn_psk, sizeof(g_quick_conn_psk), 0, sizeof(g_quick_conn_psk)) != EOK)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_fast_connect memset_s failed.");
    }
    g_quick_conn_psk_flag = WPA_FLAG_OFF;
    return ret;
}

int uapi_wifi_sta_disconnect(void)
{
    int ret;
    char network_id_txt[WPA_NETWORK_ID_TXT_LEN + 1] = { 0 };

    wpa_error_log0(MSG_ERROR, " ---> ### uapi_wifi_sta_disconnect enter. \n\n\r");

    struct ext_wifi_dev *wifi_dev = wifi_dev_get(EXT_WIFI_IFTYPE_STATION);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_disconnect: get wifi dev failed\n");
        return EXT_WIFI_FAIL;
    }

    ret = wpa_cli_disconnect((struct wpa_supplicant *)(wifi_dev->priv));
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wpa_cli_disconnect fail.");
        return EXT_WIFI_FAIL;
    }

    ret = snprintf_s(network_id_txt, sizeof(network_id_txt), sizeof(network_id_txt) - 1, "%d", wifi_dev->network_id);
    if (ret < 0) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_disconnect snprintf_s faild");
        return EXT_WIFI_FAIL;
    }
    ret = wpa_cli_remove_network((struct wpa_supplicant *)(wifi_dev->priv), network_id_txt, WPA_NETWORK_ID_TXT_LEN);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wpa_cli_remove_network fail.");
        return EXT_WIFI_FAIL;
    }
    g_connecting_flag = WPA_FLAG_OFF;
    g_assoc_auth = EXT_WIFI_SEC_TYPE_INVALID;
    return EXT_WIFI_OK;
}

int uapi_wifi_sta_get_connect_info(ext_wifi_status *connect_status)
{
    struct ext_wifi_dev *wifi_dev = NULL;
    struct wpa_supplicant *wpa_s   = NULL;
    ext_wifi_status_sem wifi_status_sem = { 0 };
    errno_t rc = EXT_WIFI_OK;
    unsigned int ret_val;

    if (connect_status == NULL) {
        wpa_error_log0(MSG_ERROR, "input param is NULL.");
        return EXT_WIFI_FAIL;
    }
    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }
    rc = memset_s(connect_status, sizeof(ext_wifi_status), 0, sizeof(ext_wifi_status));
    if (rc != EOK)
        return EXT_WIFI_FAIL;

    if (memset_s(&wifi_status_sem, sizeof(ext_wifi_status_sem), 0, sizeof(ext_wifi_status_sem)) != EOK) {
        return EXT_WIFI_FAIL;
    }

    wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_STATION);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL))
        return EXT_WIFI_FAIL;

    wpa_s = (struct wpa_supplicant *)(wifi_dev->priv);
    wifi_status_sem.conn_status = connect_status;

    /* 初始化一个二值信号量 */
    if (osal_sem_binary_sem_init(&wifi_status_sem.sta_status_sem, 0) != OSAL_SUCCESS) {
        wpa_error_log0(MSG_ERROR, "binary sem init fail.");
        osal_sem_destroy(&wifi_status_sem.sta_status_sem);
        return EXT_WIFI_FAIL;
    }

    (void)wpa_cli_get_sta_status(wpa_s, &wifi_status_sem);

    if (wifi_status_sem.status != WIFI_STASTUS_OK) {
        wpa_error_log1(MSG_ERROR, "wifi status obtain fail, status = %d.", wifi_status_sem.status);
        rc = EXT_WIFI_FAIL;
    }
    (void)osal_sem_down(&wifi_status_sem.sta_status_sem);
    osal_sem_destroy(&wifi_status_sem.sta_status_sem);

    return rc;
}

static int wifi_wpa_task_create(const struct ext_wifi_dev *wifi_dev)
{
    wifi_task_attr wpa_task = {0};
    wpa_error_log0(MSG_ERROR, "---> uapi_wpa_task_create enter.");
    wpa_task.task_entry = (wifi_tsk_entry_func)wpa_supplicant_main_task;
    wpa_task.stack_size  = WPA_TASK_STACK_SIZE;
    wpa_task.task_name = "wpa_supplicant";
    wpa_task.task_prio = WPA_TASK_PRIO_NUM;
    wpa_task.arg = (void *)wifi_dev;
    wpa_task.task_policy = WIFI_TASK_STATUS_DETACHED;
    if (os_task_create(&g_wpataskid, &wpa_task) != WIFI_OS_OK) {
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

int wifi_wpa_start(const struct ext_wifi_dev *wifi_dev)
{
    int ret_val;
    unsigned int event = WPA_EVENT_WPA_START_ERROR;
    unsigned int int_save;

    if (wifi_dev == NULL)
        return EXT_WIFI_FAIL;

    wpa_error_log0(MSG_ERROR, "---> wifi_wpa_start enter.");
    os_intlock(&int_save);
    if (g_wpa_event_inited_flag == WPA_FLAG_OFF) {
        (void)os_event_init(&g_wpa_event, WIFI_EVENT_WPA);
        (void)os_event_init(&g_softap_event, WIFI_EVENT_SOFTAP);
#ifdef LOS_CONFIG_P2P
        (void)os_event_init(&g_p2p_event, WIFI_EVENT_P2P);
#endif /* LOS_CONFIG_P2P */
        g_wpa_event_inited_flag = WPA_FLAG_ON;
    }
    os_intrestore(int_save);
    int wifi_dev_count = los_count_wifi_dev_in_use();
    if (wifi_dev_count < WPA_BASE_WIFI_DEV_NUM) {
        wpa_error_log1(MSG_ERROR, "wifi_wpa_start: wifi_dev_count = %d.\n", wifi_dev_count);
        return EXT_WIFI_FAIL;
    }
    (void)os_event_clear(g_wpa_event, ~(unsigned int)WPA_EVENT_WPA_START_FLAG);
    wpa_error_log0(MSG_ERROR, "os_event_clear WPA_EVENT_WPA_START_FLAG");
    if (wifi_dev_count == WPA_BASE_WIFI_DEV_NUM) {
        ret_val = wifi_wpa_task_create(wifi_dev);
        if (ret_val != EXT_WIFI_OK) {
            wpa_error_log1(MSG_ERROR, "wifi_wpa_start: wpa_supplicant task create failed, ret_val = %x.", ret_val);
            return EXT_WIFI_FAIL;
        }
    } else {
        if (wpa_cli_if_start(NULL, wifi_dev->iftype, wifi_dev->ifname, WIFI_IFNAME_MAX_SIZE) != EXT_WIFI_OK) {
            wpa_error_log0(MSG_ERROR, "wifi_wpa_start: if start failed.");
            return EXT_WIFI_FAIL;
        }
    }
    wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_WPA_START_FLAG");
    (void)os_event_read(g_wpa_event, WPA_EVENT_WPA_START_FLAG, &event,
                        WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_WAIT_FOREVER);
    if (event == WPA_EVENT_WPA_START_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_wpa_start: successful.\n");
    } else {
        wpa_error_log1(MSG_ERROR, "wifi_wpa_start: failed, event: %x\n", event);
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

int wifi_wpa_stop(ext_wifi_iftype iftype)
{
    struct ext_wifi_dev *wifi_dev  = NULL;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    int ret;
    wpa_error_log0(MSG_ERROR, "---> wifi_wpa_stop enter.");

    wifi_dev = los_get_wifi_dev_by_iftype(iftype);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL) || (wifi_dev->iftype == EXT_WIFI_IFTYPE_AP)) {
        wpa_error_log0(MSG_ERROR, "wifi_wpa_stop: get wifi dev failed");
        return EXT_WIFI_FAIL;
    }
    if (memcpy_s(ifname, WIFI_IFNAME_MAX_SIZE + 1, wifi_dev->ifname, WIFI_IFNAME_MAX_SIZE) != EOK)
        return EXT_WIFI_FAIL;

    (void)os_event_clear(g_wpa_event, ~(unsigned int)WPA_EVENT_STA_STOP_FLAG);
    wpa_error_log0(MSG_ERROR, "os_event_clear WPA_EVENT_STA_STOP_FLAG");
    if (wpa_cli_terminate(NULL, ELOOP_TASK_WPA) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_wpa_stop: terminate failed");
        return EXT_WIFI_FAIL;
    }
    unsigned int ret_val = 0;
    (void)os_event_read(g_wpa_event, WPA_EVENT_STA_STOP_FLAG, &ret_val,
                        WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_WAIT_FOREVER);
    if (ret_val == WPA_EVENT_STA_STOP_OK) {
        wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_STA_STOP_FLAG success\n");
    } else {
        wpa_error_log1(MSG_ERROR, "os_event_read WPA_EVENT_STA_STOP_FLAG failed: %x\n", ret_val);
        return EXT_WIFI_FAIL;
    }
    ret = wal_deinit_drv_wlan_netdev(ifname);
    if (ret != EXT_WIFI_OK)
        wpa_error_log0(MSG_ERROR, "wifi_wpa_stop: wal_deinit_drv_wlan_netdev failed!");

    return ret;
}

int wifi_add_iface(const struct ext_wifi_dev *wifi_dev)
{
    struct ext_wifi_dev *old_wifi_dev = NULL;
    int ret;
    unsigned int ret_val;

    wpa_error_log0(MSG_ERROR, "---> wifi_add_iface enter.");

    if (wifi_dev == NULL) {
        wpa_error_log0(MSG_ERROR, "wifi_add_iface: wifi_dev is NULL.");
        return EXT_WIFI_FAIL;
    }

    old_wifi_dev = wpa_get_other_existed_wpa_wifi_dev(NULL);
    if ((old_wifi_dev == NULL) || (old_wifi_dev->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "no wpa wifi dev is running.");
        return EXT_WIFI_FAIL;
    }
    (void)os_event_clear(g_wpa_event, ~(unsigned int)WPA_EVENT_ADD_IFACE_FLAG);

    wpa_error_log0(MSG_ERROR, "os_event_clear WPA_EVENT_ADD_IFACE_FLAG");
    ret = wpa_cli_add_iface((struct wpa_supplicant *)old_wifi_dev->priv, wifi_dev->ifname);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wpa_add_iface: failed\n");
        return EXT_WIFI_FAIL;
    }

    (void)os_event_read(g_wpa_event, WPA_EVENT_ADD_IFACE_FLAG, &ret_val,
                        WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_WAIT_FOREVER);
    if (ret_val == WPA_EVENT_ADD_IFACE_OK) {
        wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_ADD_IFACE_FLAG successful.");
    } else {
        wpa_error_log1(MSG_ERROR, "os_event_read WPA_EVENT_ADD_IFACE_FLAG failed ret_val: %x\n", ret_val);
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

int wifi_remove_iface(struct ext_wifi_dev *wifi_dev)
{
    struct wpa_supplicant *wpa_s = NULL;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    int ret;
    unsigned int ret_val;

    if (wifi_dev == NULL)
        return EXT_WIFI_FAIL;

    wpa_s = wifi_dev->priv;
    if (memcpy_s(ifname, sizeof(ifname), wifi_dev->ifname, (size_t)wifi_dev->ifname_len) != (int)EOK)
        return EXT_WIFI_FAIL;

    (void)os_event_clear(g_wpa_event, ~(unsigned int)WPA_EVENT_REMOVE_IFACE_FLAG);
    wpa_error_log0(MSG_ERROR, "os_event_clear WPA_EVENT_REMOVE_IFACE_FLAG");
    ret = wpa_cli_remove_iface(wpa_s);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_remove_iface: failed\n");
        return EXT_WIFI_FAIL;
    }
    (void)os_event_read(g_wpa_event, WPA_EVENT_REMOVE_IFACE_FLAG, &ret_val,
                        WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_WAIT_FOREVER);
    if (ret_val == WPA_EVENT_REMOVE_IFACE_FLAG) {
        wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_REMOVE_IFACE_FLAG success");
    } else {
        wpa_error_log1(MSG_ERROR, "os_event_read WPA_EVENT_REMOVE_IFACE_FLAG failed ret_val = %x", ret_val);
    }
    (void)wal_deinit_drv_wlan_netdev(ifname);
    return EXT_WIFI_OK;
}

int uapi_wifi_sta_start(char *ifname, int *len)
{
    struct ext_wifi_dev *wifi_dev = NULL;
    int rc;
    int ret;

    if ((ifname == NULL) || (len == NULL)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_start: invalid param");
        return EXT_WIFI_FAIL;
    }

    if (try_set_lock_flag() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_start: wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }

    if (sta_precheck() != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_start: sta_precheck fail!");
        goto WIFI_STA_EXIT;
    }
    /* Sta协议模式默认使用11ax */
    if (g_sta_opt_set.hw_mode == WIFI_MODE_UNDEFINE) {
        g_sta_opt_set.hw_mode = WIFI_MODE_11B_G_N_AX;
    }
    wifi_dev = wifi_dev_creat(EXT_WIFI_IFTYPE_STATION, g_sta_opt_set.hw_mode);
    if (wifi_dev == NULL) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_start: wifi_dev_creat failed.");
        goto WIFI_STA_EXIT;
    }

    if ((los_set_wifi_dev(wifi_dev) != EXT_WIFI_OK) || (*len < wifi_dev->ifname_len + 1))
        goto WIFI_STA_ERROR;

    rc = memcpy_s(ifname, (size_t)(*len), wifi_dev->ifname, (size_t)wifi_dev->ifname_len);
    if (rc != EOK) {
        wpa_error_log0(MSG_ERROR, "Could not copy wifi dev ifname.");
        goto WIFI_STA_ERROR;
    }

    ifname[wifi_dev->ifname_len] = '\0';
    *len = wifi_dev->ifname_len;
    if ((los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_P2P_CLIENT) != NULL) ||
        (los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_P2P_GO) != NULL) ||
        (los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_P2P_DEVICE) != NULL) ||
        (los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_MESH_POINT) != NULL)) {
        ret = wifi_add_iface(wifi_dev);
    } else
        ret = wifi_wpa_start(wifi_dev);

    if (ret != EXT_WIFI_OK)
        goto WIFI_STA_ERROR;

    clr_lock_flag();
    return EXT_WIFI_OK;
WIFI_STA_ERROR:
    (void)wal_deinit_drv_wlan_netdev(wifi_dev->ifname);
    los_free_wifi_dev(wifi_dev);
WIFI_STA_EXIT:
    clr_lock_flag();
    return EXT_WIFI_FAIL;
}

int uapi_wifi_sta_stop(void)
{
    int ret = EXT_WIFI_FAIL;
    unsigned int int_save;

    if (try_set_lock_flag() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }

    struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_STATION);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_stop: get wifi dev failed");
        goto WIFI_STA_STOP_FAIL;
    }
    if (wpa_get_other_existed_wpa_wifi_dev(wifi_dev->priv) != NULL)
        ret = wifi_remove_iface(wifi_dev);
    else
        ret = wifi_wpa_stop(EXT_WIFI_IFTYPE_STATION);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wpa_stop failed");
        ret = EXT_WIFI_FAIL;
        goto WIFI_STA_STOP_FAIL;
    }
    os_intlock(&int_save);

    (void)memset_s(&g_sta_opt_set, sizeof(struct wifi_sta_opt_set), 0, sizeof(struct wifi_sta_opt_set));
    (void)memset_s(&g_reconnect_set, sizeof(struct wifi_reconnect_set), 0, sizeof(struct wifi_reconnect_set));
    (void)memset_s(&g_scan_record, sizeof(struct ext_scan_record), 0, sizeof(struct ext_scan_record));
    (void)memset_s(g_quick_conn_psk, sizeof(g_quick_conn_psk), 0, sizeof(g_quick_conn_psk));

    g_assoc_auth = EXT_WIFI_SEC_TYPE_INVALID;
    g_mesh_sta_flag = WPA_FLAG_OFF;
    g_connecting_flag = WPA_FLAG_OFF;
    g_wpa_rm_network = SOC_WPA_RM_NETWORK_END;
    os_intrestore(int_save);

#ifdef LOS_CONFIG_WPA_ENTERPRISE
    if (g_wfa_ca_cert != NULL) {
        free(g_wfa_ca_cert);
        g_wfa_ca_cert = NULL;
    }

    if (g_wfa_cli_cert != NULL) {
        free(g_wfa_cli_cert);
        g_wfa_cli_cert = NULL;
    }

    if (g_wfa_cli_key != NULL) {
        free(g_wfa_cli_key);
        g_wfa_cli_key = NULL;
    }

    if (g_wfa_ent_identity != NULL) {
        free(g_wfa_ent_identity);
        g_wfa_ent_identity = NULL;
    }
#endif

    ret = EXT_WIFI_OK;
WIFI_STA_STOP_FAIL:
    clr_lock_flag();
    return ret;
}

#ifdef CONFIG_WPS
int uapi_wifi_sta_wps_pbc(const unsigned char *bssid, unsigned int bssid_len)
{
    int ret;
    struct ext_wifi_dev *wifi_dev = NULL;
    char network_id_txt[WPA_NETWORK_ID_TXT_LEN + 1] = { 0 };
    char addr_txt[EXT_WIFI_TXT_ADDR_LEN + 1] = { 0 };

    wifi_dev = wifi_dev_get(EXT_WIFI_IFTYPE_STATION);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_wps_pbc: get wifi dev failed.");
        return EXT_WIFI_FAIL;
    }
    if (bssid != NULL) {
        if ((bssid_len != ETH_ALEN) || (addr_precheck(bssid) != EXT_WIFI_OK)) {
            wpa_error_log0(MSG_ERROR, "Invalid bssid.");
            return EXT_WIFI_FAIL;
        }
    }
    ret = snprintf_s(network_id_txt, sizeof(network_id_txt), sizeof(network_id_txt) - 1, "%d", wifi_dev->network_id);
    if (ret < 0) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_wps_pbc snprintf_s faild");
        return EXT_WIFI_FAIL;
    }
    ret = wpa_cli_remove_network((struct wpa_supplicant *)(wifi_dev->priv), network_id_txt, WPA_NETWORK_ID_TXT_LEN);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wpa_cli_remove_network fail.");
        return EXT_WIFI_FAIL;
    }
    if (bssid == NULL) {
        ret = wpa_cli_wps_pbc((struct wpa_supplicant *)(wifi_dev->priv), NULL, 0);
    } else {
        ret = snprintf_s(addr_txt, sizeof(addr_txt), sizeof(addr_txt) - 1, MACSTR, MAC2STR(bssid));
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_wps_pbc snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
        ret = wpa_cli_wps_pbc((struct wpa_supplicant *)(wifi_dev->priv), addr_txt, sizeof(addr_txt));
    }
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_wps_pbc failed");
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

static int wps_pin_check(const char *pin, unsigned int pin_len)
{
    unsigned int i;
    if ((pin_len != WPS_PIN_LENGTH) || (strnlen(pin, WPS_PIN_LENGTH + 1) != pin_len))
        return EXT_WIFI_FAIL;
    for (i = 0; i < pin_len; i++)
        if ((pin[i] < '0') || (pin[i] > '9')) {
            wpa_error_log0(MSG_ERROR, "wps_pin: pin Format should be 0 ~ 9.");
            return EXT_WIFI_FAIL;
        }
    return EXT_WIFI_OK;
}

int uapi_wifi_sta_wps_pin(const char *pin, unsigned int pin_len, const unsigned char *bssid, unsigned int bssid_len)
{
    int ret;
    char network_id_txt[WPA_NETWORK_ID_TXT_LEN + 1] = { 0 };
    struct ext_wifi_dev *wifi_dev                  = NULL;
    char addr_txt[EXT_WIFI_TXT_ADDR_LEN + 1]         = { 0 };

    wifi_dev = wifi_dev_get(EXT_WIFI_IFTYPE_STATION);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_wps_pin: get wifi dev failed\n");
        return EXT_WIFI_FAIL;
    }
    if (pin == NULL) {
        wpa_warning_log0(MSG_DEBUG, "wpa_cli_wps_pin : pin is NULL");
        return EXT_WIFI_FAIL;
    }
    if (bssid != NULL) {
        if ((bssid_len != ETH_ALEN) || (addr_precheck(bssid) != EXT_WIFI_OK)) {
            wpa_error_log0(MSG_ERROR, "Invalid bssid.");
            return EXT_WIFI_FAIL;
        }
    }
    if (wps_pin_check(pin, pin_len) == EXT_WIFI_FAIL)
        return EXT_WIFI_FAIL;
    ret = snprintf_s(network_id_txt, sizeof(network_id_txt), sizeof(network_id_txt) - 1, "%d", wifi_dev->network_id);
    if (ret < 0) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_wps_pin: network_id_txt snprintf_s faild");
        return EXT_WIFI_FAIL;
    }
    ret = wpa_cli_remove_network((struct wpa_supplicant *)(wifi_dev->priv), network_id_txt, WPA_NETWORK_ID_TXT_LEN);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wpa_cli_remove_network fail.");
        return EXT_WIFI_FAIL;
    }
    if (bssid == NULL) {
        ret = wpa_cli_wps_pin((struct wpa_supplicant *)(wifi_dev->priv), pin, pin_len, NULL, 0);
    } else {
        ret = snprintf_s(addr_txt, sizeof(addr_txt), sizeof(addr_txt) - 1, MACSTR, MAC2STR(bssid));
        if (ret < 0) {
            wpa_error_log0(MSG_ERROR, "uapi_wifi_sta_wps_pin: addr_txt snprintf_s faild");
            return EXT_WIFI_FAIL;
        }
        ret = wpa_cli_wps_pin((struct wpa_supplicant *)(wifi_dev->priv), pin, pin_len, addr_txt, sizeof(addr_txt));
    }
    return ret;
}

int uapi_wifi_sta_wps_pin_get(char *pin, unsigned int len)
{
    unsigned int val = 0;
    int ret;

    if (pin == NULL) {
        wpa_error_log0(MSG_ERROR, " ---> pin ptr is NULL.");
        return EXT_WIFI_FAIL;
    }
    if (len != (WIFI_WPS_PIN_LEN + 1)) {
        wpa_error_log1(MSG_ERROR, " ---> wps pin buffer length should be %d.", (WIFI_WPS_PIN_LEN + 1));
        return EXT_WIFI_FAIL;
    }
    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }
    ret = wps_generate_pin(&val);
    if (ret != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, " ---> wps generate pin failed.");
        return EXT_WIFI_FAIL;
    }
    ret = snprintf_s(pin, WIFI_WPS_PIN_LEN + 1, WIFI_WPS_PIN_LEN, "%08u", val);
    return (ret < 0) ? EXT_WIFI_FAIL : EXT_WIFI_OK;
}
#endif /* CONFIG_WPS */

/*****************************change STA settingts start ***********************************/
static int wifi_sta_set_cond_check(void)
{
    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi_sta_set_cond_check: wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }
    if (is_sta_on() == EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_sta_set_cond_check: sta is already in progress, set failed.");
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

int uapi_wifi_set_assoc_auth(int auth_type)
{
    if (auth_type > EXT_WIFI_SECURITY_UNKNOWN || auth_type < 0) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_set_assoc_auth: invalid auth type.");
        return EXT_WIFI_FAIL;
    }

    g_assoc_auth = auth_type;
    return EXT_WIFI_OK;
}

int uapi_wifi_set_ft_flag(int flag)
{
    if (flag != WPA_FLAG_OFF && flag != WPA_FLAG_ON) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_set_ft_flag: invalid flag.");
        return EXT_WIFI_FAIL;
    }

    g_ft_flag = flag;
    return EXT_WIFI_OK;
}

int uapi_wifi_set_pmf(wifi_pmf_option_enum pmf)
{
    if (wifi_sta_set_cond_check() == EXT_WIFI_FAIL)
        return EXT_WIFI_FAIL;

    if (pmf > WIFI_MGMT_FRAME_PROTECTION_REQUIRED) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_set_pmf: invalid pmf options.");
        return EXT_WIFI_FAIL;
    }

    g_sta_opt_set.pmf = pmf;
    g_sta_opt_set.usr_pmf_set_flag = WPA_FLAG_ON;
    return EXT_WIFI_OK;
}

int wifi_softap_set_pmf(int pmf)
{
    if (pmf > MGMT_FRAME_PROTECTION_REQUIRED) {
        wpa_error_log0(MSG_ERROR, "wifi_softap_set_pmf: invalid pmf options.");
        return EXT_WIFI_FAIL;
    }

    g_ap_opt_set.ieee80211w = pmf;
    return EXT_WIFI_OK;
}


#ifdef CONFIG_WPA3
int wifi_sta_set_sae_pwe(wifi_sae_pwe_option_enum pwe)
{
    if (pwe > WIFI_SAE_PWE_BOTH) {
        wpa_error_log0(MSG_ERROR, "wifi_sta_set_sae_pwe: invalid pwe options.");
        return EXT_WIFI_FAIL;
    }

    g_sta_opt_set.sae_pwe = pwe;
    return EXT_WIFI_OK;
}

int wifi_softap_set_sae_pwe(wifi_sae_pwe_option_enum pwe)
{
    if (pwe > WIFI_SAE_PWE_BOTH) {
        wpa_error_log0(MSG_ERROR, "wifi_softap_set_sae_pwe: invalid pwe options.");
        return EXT_WIFI_FAIL;
    }

    g_ap_opt_set.sae_pwe = pwe;
    return EXT_WIFI_OK;
}

int wifi_softap_set_transition(int transition)
{
    g_ap_opt_set.transition = transition;
    return EXT_WIFI_OK;
}

int wifi_softap_set_clog_threshold(int clog_threshold)
{
    g_ap_opt_set.clog_threshold = clog_threshold;
    return EXT_WIFI_OK;
}

int wifi_sta_set_sae_groups(int *groups, int len)
{
    if (memcpy_s(g_sta_opt_set.sae_groups, sizeof(g_ap_opt_set.sae_groups), groups, len) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_sta_set_sae_groups: invalid groups param.");
        return EXT_WIFI_FAIL;
    }

    return EXT_WIFI_OK;
}

int wifi_softap_set_sae_groups(int *groups, int len)
{
    if (memcpy_s(g_ap_opt_set.sae_groups, sizeof(g_ap_opt_set.sae_groups), groups, len) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_softap_set_sae_groups: invalid groups param.");
        return EXT_WIFI_FAIL;
    }

    return EXT_WIFI_OK;
}

#endif

int uapi_wifi_sta_set_reconnect_policy(int enable, unsigned int seconds,
                                       unsigned int period, unsigned int max_try_count)
{
    struct ext_wifi_dev *wifi_dev = NULL;
    struct wpa_supplicant *wpa_s = NULL;
    struct wpa_ssid *current_ssid = g_reconnect_set.current_ssid;
    int is_connected = WPA_FLAG_OFF;

    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }
    if (is_sta_on() != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "sta is not started, set reconnect policy failed.");
        return EXT_WIFI_FAIL;
    }

    if ((enable < WPA_FLAG_OFF) || (enable > WPA_FLAG_ON) ||
        (seconds < WIFI_MIN_RECONNECT_TIMEOUT) || (seconds > WIFI_MAX_RECONNECT_TIMEOUT) ||
        (period < WIFI_MIN_RECONNECT_PERIOD) || (period > WIFI_MAX_RECONNECT_PERIOD) ||
        (max_try_count < WIFI_MIN_RECONNECT_TIMES) || (max_try_count > WIFI_MAX_RECONNECT_TIMES)) {
        wpa_error_log0(MSG_ERROR, "input value error.");
        return EXT_WIFI_FAIL;
    }

    unsigned int int_save;
    os_task_lock(&int_save);
    (void)memset_s(&g_reconnect_set, sizeof(struct wifi_reconnect_set), 0, sizeof(struct wifi_reconnect_set));
    g_reconnect_set.enable = enable;
    g_reconnect_set.timeout = seconds;
    g_reconnect_set.period = period;
    g_reconnect_set.max_try_count = max_try_count;
    os_task_unlock(int_save);

    wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_STATION);
    os_task_lock(&int_save);
    if ((wifi_dev != NULL) && (wifi_dev->priv != NULL)) {
        wpa_s = (struct wpa_supplicant *)wifi_dev->priv;
        is_connected = (wpa_s->wpa_state == WPA_COMPLETED) ? WPA_FLAG_ON : WPA_FLAG_OFF;
        g_reconnect_set.current_ssid = (is_connected == WPA_FLAG_ON) ? wpa_s->current_ssid : current_ssid;
    }
    os_task_unlock(int_save);

    return EXT_WIFI_OK;
}

int uapi_wifi_sta_set_protocol_mode(protocol_mode_enum hw_mode)
{
    if (wifi_sta_set_cond_check() == EXT_WIFI_FAIL)
        return EXT_WIFI_FAIL;

    if (hw_mode == WIFI_MODE_UNDEFINE || hw_mode > WIFI_MODE_11B_G_N_AX) {
        wpa_error_log0(MSG_ERROR, "physical mode value is error.");
        return EXT_WIFI_FAIL;
    }
    g_sta_opt_set.hw_mode = hw_mode;
    return EXT_WIFI_OK;
}

wifi_pmf_option_enum uapi_wifi_get_pmf(void)
{
    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return WIFI_MGMT_FRAME_PROTECTION_BUTT;
    }
    return g_sta_opt_set.pmf;
}

protocol_mode_enum uapi_wifi_sta_get_protocol_mode(void)
{
    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return WIFI_MODE_UNDEFINE;
    }
    return g_sta_opt_set.hw_mode;
}

void wifi_event_task_handler(unsigned int event)
{
    if (g_wpa_event_cb != NULL) {
        g_wpa_event_cb((ext_wifi_event *)(uintptr_t)event);
    }
    g_wpa_event_running = 0;
    os_free((void *)(uintptr_t)event);
}

/* create a task and call user's cb */
void wifi_new_task_event_cb(const ext_wifi_event *event_cb)
{
    wifi_task_attr event_cb_task = {0};
    ext_wifi_event *event_new = NULL;
    unsigned int timeout = 0;

    if ((g_wpa_event_cb == NULL) || (event_cb == NULL)) {
        return;
    }
    if (g_direct_cb == 1) { /* call directly */
        g_wpa_event_cb(event_cb);
        return;
    }
    while (g_wpa_event_running != 0) {
        os_task_delay(10);   /* delay 1 tick (10ms) and try again */
        timeout++;
        if (timeout > 100) { /* 100 tick = 1s */
            printf("wifi_new_task_event_cb::current event[%d] wait timeout, last event[%d]\r\n",
                event_cb->event, g_wpa_event_running - 1);
            return;
        }
    }
    wpa_event_task_free();
    event_new = (ext_wifi_event *)os_zalloc(sizeof(ext_wifi_event));
    if (event_new == NULL) {
        wpa_error_log0(MSG_ERROR, "ext_wifi_event malloc err.");
        return;
    }
    if (memcpy_s(event_new, sizeof(ext_wifi_event), event_cb, sizeof(ext_wifi_event)) != EOK) {
        wpa_error_log0(MSG_ERROR, "memcpy event info err.");
        os_free(event_new);
        return;
    }
    event_cb_task.task_entry = (wifi_tsk_entry_func)wifi_event_task_handler;
    event_cb_task.stack_size  = g_cb_stack_size;
    event_cb_task.task_name = "wpa_event_cb";
    event_cb_task.task_prio = g_cb_task_prio; /* task prio, must lower than wifi/lwip/wpa */
    event_cb_task.arg = (void *)event_new;
    event_cb_task.task_policy = WIFI_TASK_STATUS_DETACHED;
    g_wpa_event_running = event_cb->event + 1; /* event runing */
    if (os_task_create(&g_wpa_event_taskid, &event_cb_task) != WIFI_OS_OK) {
        osal_printk("Event cb create task failed, %d.\r\n", event_cb->event);
        g_wpa_event_running = 0;
        os_free(event_new);
    }
}

int uapi_wifi_config_callback(unsigned char mode, unsigned char task_prio, unsigned short stack_size)
{
    /* only support 0 and 1 */
    if ((mode & 1) != mode) {
        wpa_error_log1(MSG_ERROR, "uapi_wifi_config_callback: invalid mode:%u.", (unsigned int)mode);
        return EXT_WIFI_FAIL;
    }
    g_direct_cb = mode;
    /* directly call don't need follow configuraion */
    if (g_direct_cb == 1)
        return EXT_WIFI_OK;

    if ((task_prio < EXT_WIFI_CB_MIN_PRIO) || (task_prio > EXT_WIFI_CB_MAX_PRIO)) {
        wpa_error_log1(MSG_ERROR, "uapi_wifi_config_callback: invalid prio:%u.", (unsigned int)task_prio);
        return EXT_WIFI_FAIL;
    }
    if (stack_size < WPA_CB_STACK_SIZE) {
        wpa_error_log1(MSG_ERROR, "uapi_wifi_config_callback: invalid stack size:%u.", (unsigned int)stack_size);
        return EXT_WIFI_FAIL;
    }
    g_cb_task_prio = task_prio;
    g_cb_stack_size = stack_size & 0xFFF0;  /* 16bytes allign */
    return EXT_WIFI_OK;
}

int uapi_wifi_register_event_callback(uapi_wifi_event_cb event_cb)
{
    g_wpa_event_cb = event_cb;
    return EXT_WIFI_OK;
}


int uapi_wifi_sta_reconnect_for_wfa_test(void)
{
    struct ext_wifi_dev *wifi_dev = NULL;
    char network_id_txt[WPA_NETWORK_ID_TXT_LEN + 1] = { 0 };
    int ret;

    wpa_error_log0(MSG_ERROR, " ---> ### uapi_wifi_sta_reconnect_for_wfa_test enter. \n\n\r");

    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }

    wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_STATION);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "sta_reconnect: get wifi dev failed\n");
        return EXT_WIFI_FAIL;
    }

    ret = snprintf_s(network_id_txt, sizeof(network_id_txt), sizeof(network_id_txt) - 1, "%d", wifi_dev->network_id);
    if (ret < 0) {
        wpa_error_log0(MSG_ERROR, "sta_reconnect: network_id_txt snprintf_s failed");
        return EXT_WIFI_FAIL;
    }
    if (wpa_cli_disconnect((struct wpa_supplicant *)(wifi_dev->priv)) < 0)
        return EXT_WIFI_FAIL;

    g_connecting_flag = WPA_FLAG_ON;

    if (wpa_cli_select_network((struct wpa_supplicant *)(wifi_dev->priv), network_id_txt, WPA_NETWORK_ID_TXT_LEN) < 0)
        return EXT_WIFI_FAIL;
    return EXT_WIFI_OK;
}

static wifi_extend_ie_index wifi_ie_index_convert(ext_wifi_extend_ie_index index)
{
    wifi_extend_ie_index ie_index;

    switch (index) {
        case EXT_WIFI_EXTEND_IE1:
            ie_index = WIFI_EXTEND_IE1;
            break;
        case EXT_WIFI_EXTEND_IE2:
            ie_index = WIFI_EXTEND_IE2;
            break;
        case EXT_WIFI_EXTEND_IE3:
            ie_index = WIFI_EXTEND_IE3;
            break;
        case EXT_WIFI_EXTEND_IE4:
            ie_index = WIFI_EXTEND_IE4;
            break;
        default:
            ie_index = WIFI_EXTEND_IE_BUTT;
            break;
    }

    return ie_index;
}

static int wifi_set_usr_app_ie(ext_wifi_iftype iftype, ext_wifi_extend_ie_index ie_index,
                               const unsigned char frame_type_bitmap,
                               const unsigned char *ie, unsigned short ie_len)
{
    struct ext_wifi_dev *wifi_dev = NULL;
    wifi_extend_ie_index index;
    uint8 set = (ie != NULL) ? 1 : 0;

    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }

    if ((iftype != EXT_WIFI_IFTYPE_STATION) && (iftype != EXT_WIFI_IFTYPE_AP) &&
        (iftype != EXT_WIFI_IFTYPE_P2P_DEVICE) && (iftype != EXT_WIFI_IFTYPE_P2P_GO)) {
        wpa_error_log0(MSG_ERROR, "wifi_set_usr_app_ie_internal: iftype invalid.");
        return EXT_WIFI_FAIL;
    }

    if (ie_index >= EXT_WIFI_EXTEND_IE_BUTT) {
        wpa_error_log0(MSG_ERROR, "wifi_set_usr_app_ie_internal: usr_ie_type or ie_index invalid.");
        return EXT_WIFI_FAIL;
    }

    if (!(((unsigned char)EXT_WIFI_BEACON_IE | (unsigned char)EXT_WIFI_PROBE_RSP_IE |
        (unsigned char)EXT_WIFI_PROBE_REQ_IE) & frame_type_bitmap))
        return EXT_WIFI_FAIL;

    index = wifi_ie_index_convert(ie_index);

    wifi_dev = wifi_dev_get(iftype);
    if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
        wpa_error_log0(MSG_ERROR, "wifi_set_usr_app_ie_internal: get wifi dev failed.");
        return EXT_WIFI_FAIL;
    }

    if (drv_soc_set_usr_app_ie(wifi_dev->ifname, set, index, frame_type_bitmap, ie, ie_len) != EXT_WIFI_OK) {
        wpa_error_log0(MSG_ERROR, "wifi_set_usr_app_ie_internal: set usr ie failed.");
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

int uapi_wifi_add_usr_app_ie(ext_wifi_iftype iftype, ext_wifi_extend_ie_index ie_index,
                             const unsigned char frame_type_bitmap,
                             const unsigned char *ie, unsigned short ie_len)
{
    if ((ie == NULL) || (ie_len > EXT_WIFI_USR_IE_MAX_SIZE) || (ie_len == 0) ||
        (ie[WPA_USE_IE_LEN_SUBSCRIPT] != ie_len - WPA_USE_IE_INIT_BYTE_NUM)) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_add_usr_app_ie: invalid input.");
        return EXT_WIFI_FAIL;
    }

    return wifi_set_usr_app_ie(iftype, ie_index, frame_type_bitmap, ie, ie_len); /* 1: add */
}

int uapi_wifi_delete_usr_app_ie(ext_wifi_iftype iftype, ext_wifi_extend_ie_index ie_index,
                                const unsigned char frame_type_bitmap)
{
    return wifi_set_usr_app_ie(iftype, ie_index, frame_type_bitmap, NULL, 0); /* 0: delete */
}

#ifdef LOS_CONFIG_WPA_ENTERPRISE
int uapi_wifi_sta_ent_set_eap_method(ext_wifi_eap_method method)
{
    if (method >= EXT_WIFI_EAP_METHOD_BUTT)
        return EXT_WIFI_FAIL;
    g_eap_method = method;
    return EXT_WIFI_OK;
}

int uapi_wifi_sta_ent_set_identity(const char *identity, unsigned int len)
{
    struct ext_wifi_dev *wifi_dev = NULL;
    int                  ret;
    unsigned int         int_save;

    if ((identity == NULL) || (len > EXT_WIFI_IDENTITY_LEN) ||
        (strnlen(identity, len + 1) != len))
        return EXT_WIFI_FAIL;

    if (is_lock_flag_off() == EXT_WIFI_FAIL) {
        wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
        return EXT_WIFI_FAIL;
    }

    wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_STATION);
    if (wifi_dev == NULL) {
        wpa_error_log0(MSG_ERROR, "get wifi dev failed");
        return EXT_WIFI_FAIL;
    }

    os_intlock(&int_save);
    ret = snprintf_s(g_eap_identity, EXT_WIFI_IDENTITY_LEN + WPA_IDENTITY_EXTRA_LEN,
                     EXT_WIFI_IDENTITY_LEN + WPA_IDENTITY_EXTRA_LEN - 1, "\"%s\"", identity);
    if (ret < 0) {
        os_intrestore(int_save);
        wpa_error_log0(MSG_ERROR, "g_eap_identity snprintf_s failed");
        return EXT_WIFI_FAIL;
    }
    os_intrestore(int_save);
    return EXT_WIFI_OK;
}

int uapi_wifi_sta_ent_set_import_callback(ext_wifi_ent_import_callback *cb)
{
    int          ret;
    unsigned int int_save;

    os_intlock(&int_save);
    ret = tls_set_import_cb(cb);
    os_intrestore(int_save);
    return ret;
}
#endif /* LOS_CONFIG_WPA_ENTERPRISE */

