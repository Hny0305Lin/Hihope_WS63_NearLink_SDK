/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 网络适配实现 （此文件为DEMO，需集成方适配修改）
 */
#include "hilink_network_adapter.h"

#include <stdbool.h>
#include <stddef.h>
#include "wifi_device.h"
#include "wifi_event.h"
#include "wifi_device_config.h"
#include "hilink_sal_defines.h"
#include "hilink_socket_adapter.h"
#include "hilink_str_adapter.h"
#include "securec.h"
#include "hilink_thread_adapter.h"
#include "hilink_mem_adapter.h"
#include "soc_wifi_api.h"
#include "lwip/netifapi.h"
#include "nv.h"
#include "nv_common_cfg.h"
#include "efuse_porting.h"

#define MAX_IP_LEN          40
#define MAX_SCAN_TIMES      4
#define DEF_SCAN_TIMEOUT    15
#define MS_PER_SECOND       1000
#define WIFI_SCANNING 1

static bool g_isRegisterWifiEvent = false;
static bool g_isStaScanSuccess = false;
static wifi_event_stru g_eventHandler;
static unsigned short g_disconnectReason = 0;
static bool g_isReasonRefresh = false;
static int g_offline_mode_scan_flag = 0;

unsigned int get_wifi_recovery_type(void)
{
    return (0x01 | 0x02); /* 0x01|0x02 : 连线网络优化功能开启 */
}

static int sta_setup_dhcp(void)
{
    int ret = 0;
    struct netif *netif_p = netifapi_netif_find("wlan0");
    if (netif_p == NULL) {
        return HILINK_SAL_NOK;
    }
    ret = netifapi_dhcp_start(netif_p);
    if (ret == 0) {
        return HILINK_SAL_OK;
    }

    return HILINK_SAL_NOK;
}
static int AddDeviceConfig(wifi_sta_config_stru *config)
{
    if (config == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    hilink_connect_info_t connect_info;
    (void)memset_s(&connect_info, sizeof(connect_info), 0, sizeof(connect_info));

    if (memcpy_s(connect_info.ssid, sizeof(connect_info.ssid), config->ssid, strlen((char *)config->ssid)) != EOK) {
        HILINK_SAL_ERROR("memcpy_s error\r\n");
        return HILINK_SAL_NOK;
    }

    if (memcpy_s(connect_info.pwd, sizeof(connect_info.pwd), config->pre_shared_key,
        strlen((char *)config->pre_shared_key)) != EOK) {
        HILINK_SAL_ERROR("memcpy_s error\r\n");
        return HILINK_SAL_NOK;
    }

    if (memcpy_s(connect_info.bssid, sizeof(connect_info.bssid), config->bssid,
        sizeof(config->bssid)) != EOK) {
        HILINK_SAL_ERROR("memcpy_s error\r\n");
        return HILINK_SAL_NOK;
    }

    uint32_t nv_ret = ERRCODE_FAIL;
    nv_ret = uapi_nv_write(NV_ID_HILINK_CONNECT_INFO, (uint8_t *)&connect_info, sizeof(connect_info));
    if (nv_ret != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("read write connect info to nv failed\r\n");
        return HILINK_SAL_NOK;
    }
    return HILINK_SAL_OK;
}

static int GetDeviceConfigs(wifi_sta_config_stru *config)
{
    if (config == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    hilink_connect_info_t connect_info;
    (void)memset_s(&connect_info, sizeof(connect_info), 0, sizeof(connect_info));

    uint16_t connect_info_len = 0;
    uint32_t nv_ret = ERRCODE_FAIL;
    nv_ret = uapi_nv_read(NV_ID_HILINK_CONNECT_INFO, sizeof(hilink_connect_info_t), &connect_info_len,
        (uint8_t *)&connect_info);
    if (nv_ret != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("read hilink connect info  from nv failed\r\n");
        return HILINK_SAL_NOK;
    }

    if (memcpy_s(config->ssid, sizeof(config->ssid), connect_info.ssid, sizeof(connect_info.ssid)) != EOK) {
        HILINK_SAL_ERROR("memcpy_s error\r\n");
        return HILINK_SAL_NOK;
    }

    if (memcpy_s(config->pre_shared_key, sizeof(config->pre_shared_key), connect_info.pwd, sizeof(connect_info.pwd)) !=
        EOK) {
        HILINK_SAL_ERROR("memcpy_s error\r\n");
        return HILINK_SAL_NOK;
    }

    if (memcpy_s(config->bssid, sizeof(config->bssid), connect_info.bssid, sizeof(connect_info.bssid)) !=
        EOK) {
        HILINK_SAL_ERROR("memcpy_s error\r\n");
        return HILINK_SAL_NOK;
    }

    return HILINK_SAL_OK;
}
static int RemoveDevice(void)
{
    hilink_connect_info_t connect_info;
    (void)memset_s(&connect_info, sizeof(connect_info), 0, sizeof(connect_info));

    uint32_t nv_ret = ERRCODE_FAIL;
    nv_ret = uapi_nv_write(NV_ID_HILINK_CONNECT_INFO, (uint8_t *)&connect_info, sizeof(connect_info));
    if (nv_ret != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("remove connect info to nv failed.\r\n");
    }

    return HILINK_SAL_OK;
}
static int get_softap_local_ip(char *localIp, unsigned char len)
{
    struct netif *lwip_netif = netifapi_netif_find("ap0");
    if (lwip_netif == NULL) {
        lwip_netif = netifapi_netif_find("wlan0");
        if (lwip_netif == NULL) {
            return HILINK_SAL_GET_IP_ERR;
        }
    }

    if (memcpy_s(localIp, len, ip4addr_ntoa(&(lwip_netif->ip_addr.u_addr.ip4)),
        strlen(ip4addr_ntoa(&(lwip_netif->ip_addr.u_addr.ip4)))) != EOK) {
        return HILINK_SAL_GET_IP_ERR;
    }

    return HILINK_SAL_OK;
}
int HILINK_GetLocalIp(char *localIp, unsigned char len)
{
    if ((localIp == NULL) || (len == 0)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    /* 避免循环调用导致的打印刷屏 */
    static bool isPrint = false;
    if (get_softap_local_ip(localIp, len) != HILINK_SAL_OK) {
        if (!isPrint) {
            HILINK_SAL_NOTICE("HILINK_GetLocalIp fail.\r\n");
            isPrint = true;
        }
        return HILINK_SAL_GET_IP_ERR;
    }
    isPrint = false;

    return HILINK_SAL_OK;
}

int HILINK_GetMacAddr(unsigned char *mac, unsigned char len)
{
    if ((mac == NULL) || (len < WIFI_MAC_LEN)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    /* 从efuse获取mac地址 */
    unsigned char efuse_left_count = 0;
    if (efuse_read_mac(mac, WIFI_MAC_LEN, &efuse_left_count) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("get device macaddr from efuse fail\r\n");
        return HILINK_SAL_GET_MAC_ERR;
    }

    return HILINK_SAL_OK;
}

static int GetWifiConfigFromOhos(wifi_sta_config_stru *config)
{
    if (config == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    wifi_sta_config_stru wifiConfig;
    (void)memset_s(&wifiConfig, sizeof(wifiConfig), 0, sizeof(wifiConfig));

    static bool isPrint = false;
    if (GetDeviceConfigs(&wifiConfig) != HILINK_SAL_OK) {
        if (!isPrint) {
            HILINK_SAL_ERROR("get device config fail\r\n");
            isPrint = true;
        }
        return HILINK_SAL_GET_WIFI_INFO_ERR;
    }
    isPrint = false;

    if (memcpy_s(config, sizeof(wifi_sta_config_stru), &wifiConfig, sizeof(wifi_sta_config_stru)) != EOK) {
        HILINK_SAL_ERROR("memcpy error\r\n");
        return HILINK_SAL_MEMCPY_ERR;
    }
    return HILINK_SAL_OK;
}

int HILINK_GetWiFiSsid(char *ssid, unsigned int *ssidLen)
{
    if ((ssid == NULL) || (ssidLen == NULL) || (*ssidLen == 0)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    wifi_sta_config_stru result;
    (void)memset_s(&result, sizeof(result), 0, sizeof(result));
    /* 避免循环调用导致的打印刷屏 */
    static bool isPrint = false;

    int ret = GetWifiConfigFromOhos(&result);
    if ((ret != HILINK_SAL_OK) || (result.ssid[0] == '\0') || (HILINK_Strlen((char *)result.ssid) >=
        sizeof(result.ssid))) {
        if (!isPrint) {
            HILINK_SAL_NOTICE("get wifi ssid fail\r\n");
            isPrint = true;
        }
        /* 初次配网获取不到ssid */
        return HILINK_SAL_OK;
    }
    isPrint = false;
    if (strcpy_s(ssid, WIFI_MAX_SSID_LEN, (char *)result.ssid) != EOK) {
        HILINK_SAL_ERROR("strcpy error\r\n");
        return HILINK_SAL_STRCPY_ERR;
    }
    *ssidLen = HILINK_Strlen(ssid);

    return HILINK_SAL_OK;
}

int HILINK_SetWiFiInfo(const char *ssid, unsigned int ssidLen, const char *pwd, unsigned int pwdLen)
{
    wifi_sta_config_stru config;
    (void)memset_s(&config, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));

    /* 删除旧wifi配置 */
    if (RemoveDevice() != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("remove config error\r\n");
        return HILINK_SAL_REMOVE_WIFI_ERR;
    }

    if ((ssidLen != 0) && (ssid != NULL)) {
        if (memcpy_s(config.ssid, sizeof(config.ssid), ssid, ssidLen) != EOK) {
            HILINK_SAL_ERROR("memcpy error\r\n");
            return HILINK_SAL_MEMCPY_ERR;
        }
    } else {
        HILINK_SAL_NOTICE("clear wifi info\r\n");
        return HILINK_SAL_OK;
    }

    if ((pwdLen != 0) && (pwd != NULL)) {
        if (memcpy_s(config.pre_shared_key, sizeof(config.pre_shared_key), pwd, pwdLen) != EOK) {
            HILINK_SAL_ERROR("memcpy error\r\n");
            return HILINK_SAL_MEMCPY_ERR;
        }
        config.security_type = WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX;
    } else {
        config.security_type = WIFI_SEC_TYPE_OPEN;
    }

    int ret = AddDeviceConfig(&config);
    (void)memset_s(&config, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
    if (ret != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("add device config error %d\r\n", ret);
        return HILINK_SAL_SET_WIFI_ERR;
    }

    return HILINK_SAL_OK;
}

void HILINK_ReconnectWiFi(void)
{
    if (wifi_sta_disconnect() != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("disconnect wifi error\r\n");
        return;
    }
    if (HILINK_ConnectWiFi() != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("connect wifi fail\r\n");
        return;
    }
}

static void OnWifiScanStateChangedCallback(int state, int size)
{
    (void)size;
    if (state == WIFI_STATE_AVALIABLE) {
        g_isStaScanSuccess = true;
    }
}

static void OnWifiConnectionChangedCallback(int state, const wifi_linked_info_stru *info, int reason_code)
{
    (void)state;
    (void)info;

     /* 0x8002,0x800e,0x800f: 密码错误场景对端返回的错误码 */
    if ((reason_code == 0x8002) || (reason_code == 0x800e) || (reason_code == 0x800f)) {
        g_disconnectReason = 15; /* 15: 密码错误场景统一上报错误码 15 */
    } else {
        g_disconnectReason = reason_code;
    }
    HILINK_SAL_NOTICE("state: %d, disconnectedReason %u\r\n", state, g_disconnectReason);
    g_isReasonRefresh = true;
}

static void OnHotspotStaJoinCallback(const wifi_sta_info_stru *info)
{
    (void)info;
    HILINK_SAL_NOTICE("OnHotspotStaJoinCallback...\r\n");
}

static void OnHotspotStaLeaveCallback(const wifi_sta_info_stru *info)
{
    (void)info;
    HILINK_SAL_NOTICE("OnHotspotStaLeaveCallback...\r\n");
}

static void OnHotspotStateChangedCallback(int state)
{
    (void)state;
    HILINK_SAL_NOTICE("OnHotspotStateChangedCallback, state[%d]\r\n", state);
}

static int RegisterWifiEventToOhos(void)
{
    if (g_isRegisterWifiEvent) {
        HILINK_SAL_NOTICE("wifievent has been registered.\r\n");
        return HILINK_SAL_OK;
    }

    g_eventHandler.wifi_event_scan_state_changed = OnWifiScanStateChangedCallback;
    g_eventHandler.wifi_event_connection_changed = OnWifiConnectionChangedCallback;
    g_eventHandler.wifi_event_softap_sta_join = OnHotspotStaJoinCallback;
    g_eventHandler.wifi_event_softap_sta_leave = OnHotspotStaLeaveCallback;
    g_eventHandler.wifi_event_softap_state_changed = OnHotspotStateChangedCallback;

    if (wifi_register_event_cb(&g_eventHandler) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("Register wifi event fail.\r\n");
        return HILINK_SAL_SET_WIFI_ERR;
    }
    g_isRegisterWifiEvent = true;
    return HILINK_SAL_OK;
}

static int AdvanceScanWifiByOhos(const wifi_sta_config_stru *config)
{
    int scanTimeout = DEF_SCAN_TIMEOUT;
    g_isStaScanSuccess = false;

    wifi_scan_params_stru scanParams;
    (void)memset_s(&scanParams, sizeof(scanParams), 0, sizeof(scanParams));

    if (memcpy_s(scanParams.ssid, sizeof(scanParams.ssid), config->ssid,
        HILINK_Strlen((char *)config->ssid)) != EOK) {
        HILINK_SAL_ERROR("memcpy error\r\n");
        return HILINK_SAL_MEMCPY_ERR;
    }
    scanParams.scan_type = WIFI_SSID_SCAN;
    scanParams.ssid_len = (char)HILINK_Strlen((char *)config->ssid);

    if (wifi_sta_scan_advance(&scanParams) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("wifi advance scan fail\r\n");
        return HILINK_SAL_SCAN_WIFI_ERR;
    }

    while (scanTimeout > 0) {
        HILINK_MilliSleep(MS_PER_SECOND);
        scanTimeout--;
        if (g_isStaScanSuccess == true) {
            break;
        }
    }
    if (scanTimeout == 0) {
        HILINK_SAL_ERROR("wifi advance scan timeout\r\n");
        return HILINK_SAL_SCAN_WIFI_ERR;
    }
    return HILINK_SAL_OK;
}

static bool GetScanWifiResultFromOhos(const wifi_sta_config_stru *config, wifi_scan_info_stru *info)
{
    bool ret = false;
    unsigned int size = WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = (wifi_scan_info_stru *)HILINK_Malloc(sizeof(wifi_scan_info_stru) * size);
    if (result == NULL) {
        HILINK_SAL_ERROR("malloc error\r\n");
        return false;
    }
    (void)memset_s(result, sizeof(wifi_scan_info_stru) * size, 0, sizeof(wifi_scan_info_stru) * size);

    if (wifi_sta_get_scan_info(result, &size) !=  ERRCODE_SUCC) {
        HILINK_SAL_ERROR("Get wifi scan info fail.\r\n");
        HILINK_Free(result);
        return false;
    }

    if ((size == 0) || (size > WIFI_SCAN_AP_LIMIT)) {
        HILINK_SAL_WARN("can not scan any wifi or scan size over limit, size:%u\r\n", size);
    } else {
        for (unsigned int i = 0; i < size; ++i) {
            /* 匹配目标wifi：ssid完全匹配，且WiFi加密类型应同时open或者同时不为open */
            if ((HILINK_Strcmp((char *)result[i].ssid, (char *)config->ssid) == 0) &&
                ((config->security_type != WIFI_SEC_TYPE_OPEN && result[i].security_type != WIFI_SEC_TYPE_OPEN) ||
                 (config->security_type == WIFI_SEC_TYPE_OPEN && result[i].security_type == WIFI_SEC_TYPE_OPEN))) {
                HILINK_SAL_NOTICE("find target ssid success\r\n");
                if (memcpy_s(info, sizeof(wifi_scan_info_stru), &result[i], sizeof(wifi_scan_info_stru)) != 0) {
                    HILINK_SAL_ERROR("memcpy error\r\n");
                    break;
                }
                ret = true;
                break;
            }
        }
    }

    HILINK_Free(result);
    return ret;
}

static void SetSecurityTypeByScanInfo(wifi_sta_config_stru *config, const wifi_scan_info_stru *info)
{
    wifi_sta_config_stru tempConfig;
    (void)memset_s(&tempConfig, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
    if (memcpy_s(&tempConfig, sizeof(wifi_sta_config_stru), config, sizeof(wifi_sta_config_stru)) != EOK) {
        HILINK_SAL_ERROR("memcpy_s error\r\n");
        return;
    }

    if (memset_s(config->bssid, sizeof(config->bssid), 0, sizeof(config->bssid)) != EOK) {
        HILINK_SAL_ERROR("memset_s error\r\n");
    }

    if (info->security_type == WIFI_SEC_TYPE_INVALID) {
        /* 扫描出的加密方式非法时根据有无密码使用默认的PSK或OPEN加密方式 */
        config->security_type = (config->pre_shared_key[0] == '\0') ? WIFI_SEC_TYPE_OPEN :
            WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX;
    } else {
        config->security_type = info->security_type;
    }

    if (config->security_type == WIFI_SEC_TYPE_WEP) {
        unsigned char pwdLen = HILINK_Strlen((char *)config->pre_shared_key);
        /* 设置密码，海思要求WEP的5和13位ASCII密码需要用双引号包起来，对应长度需要-2处理 */
        if ((pwdLen == 5) || (pwdLen == 13)) {
            char tmpSharedKey[WIFI_MAX_KEY_LEN] = {0};
            tmpSharedKey[0] = '\"';
            if (memcpy_s(tmpSharedKey + 1, sizeof(tmpSharedKey) - 2, config->pre_shared_key, pwdLen) != EOK) {
                (void)memset_s(&tempConfig, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
                return;
            }
            tmpSharedKey[pwdLen + 1] = '\"';
            if (memcpy_s(config->pre_shared_key, sizeof(config->pre_shared_key), tmpSharedKey,
                WIFI_MAX_KEY_LEN) != EOK) {
                (void)memset_s(&tempConfig, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
                (void)memset_s(tmpSharedKey, sizeof(tmpSharedKey), 0, sizeof(tmpSharedKey));
                return;
            }
            (void)memset_s(tmpSharedKey, sizeof(tmpSharedKey), 0, sizeof(tmpSharedKey));
        }
    }

    /* wifi配置与扫描后的结果对比，如果不同，需要刷新wifi配置 */
    if (HILINK_Memcmp(config, &tempConfig, sizeof(wifi_sta_config_stru)) != 0) {
        if (RemoveDevice() != HILINK_SAL_OK) {
            HILINK_SAL_ERROR("remove config error\r\n");
            (void)memset_s(&tempConfig, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
            return;
        }
        if (AddDeviceConfig(config) != HILINK_SAL_OK) {
            HILINK_SAL_ERROR("add config fail\r\n");
            (void)memset_s(&tempConfig, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
            return;
        }
    }

    (void)memset_s(&tempConfig, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
    return;
}

int HILINK_RestartWiFi(void)
{
    int ret;
    if (wifi_is_sta_enabled() == 1) {
        ret = wifi_sta_disable();
        if (ret != ERRCODE_SUCC) {
            HILINK_SAL_ERROR("disable wifi error, ret = %d\r\n", ret);
            return HILINK_SAL_NOK;
        }
    }

    ret = wifi_sta_enable();
    if (ret != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("disable wifi error, ret = %d\r\n", ret);
        return HILINK_SAL_NOK;
    }

    return HILINK_SAL_OK;
}

static int AddBssidToWifiConfig(int securityType, const unsigned char *bssid, unsigned int len)
{
    /* 读取当前WiFi配置 */
    wifi_sta_config_stru config;
    (void)memset_s(&config, sizeof(config), 0, sizeof(config));
    if ((GetWifiConfigFromOhos(&config) != HILINK_SAL_OK) || (config.ssid[0] == '\0')) {
        HILINK_SAL_ERROR("get wifi config fail\r\n");
        (void)memset_s(&config, sizeof(config), 0, sizeof(config));
        return HILINK_SAL_GET_WIFI_INFO_ERR;
    }

    /* 拷贝bssid */
    if (memcpy_s(config.bssid, sizeof(config.bssid), bssid, len) != EOK) {
        HILINK_SAL_ERROR("memcpy bssid failed\r\n");
        (void)memset_s(&config, sizeof(config), 0, sizeof(config));
        return HILINK_SAL_MEMCPY_ERR;
    }

    /* harmonyos中，-1表示无效加密类型，仅更新有效加密类型 */
    if (securityType != -1) {
        config.security_type = securityType;
    }

    /* 更新WiFi配置 */
    if (RemoveDevice() != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("remove config error\r\n");
        (void)memset_s(&config, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
        return HILINK_SAL_REMOVE_WIFI_ERR;
    }
    if (AddDeviceConfig(&config) != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("add config fail\r\n");
        (void)memset_s(&config, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
        return HILINK_SAL_ADD_WIFI_ERR;
    }
    (void)memset_s(&config, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
    return HILINK_SAL_OK;
}

static int RemoveBssidFromWifiConfig(void)
{
    /* 读取当前WiFi配置 */
    wifi_sta_config_stru config;
    (void)memset_s(&config, sizeof(config), 0, sizeof(config));
    if ((GetWifiConfigFromOhos(&config) != HILINK_SAL_OK) || (config.ssid[0] == '\0')) {
        HILINK_SAL_ERROR("get wifi config fail\r\n");
        (void)memset_s(&config, sizeof(config), 0, sizeof(config));
        return HILINK_SAL_GET_WIFI_INFO_ERR;
    }

    /* 清除bssid */
    if (memset_s(config.bssid, sizeof(config.bssid), 0, sizeof(config.bssid)) != EOK) {
        HILINK_SAL_ERROR("memcpy bssid failed\r\n");
        (void)memset_s(&config, sizeof(config), 0, sizeof(config));
        return HILINK_SAL_MEMSET_ERR;
    }

    /* 更新WiFi配置 */
    if (RemoveDevice() != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("remove config error\r\n");
        (void)memset_s(&config, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
        return HILINK_SAL_REMOVE_WIFI_ERR;
    }

    if (AddDeviceConfig(&config) != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("add config fail\r\n");
        (void)memset_s(&config, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
        return HILINK_SAL_ADD_WIFI_ERR;
    }
    (void)memset_s(&config, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
    return HILINK_SAL_OK;
}

int HILINK_ConnectWiFiByBssid(int securityType, const unsigned char *bssid, unsigned int len)
{
    if ((bssid == NULL) || (len == 0)) {
        HILINK_SAL_ERROR("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }
    /* 初始化WiFi */
    if (wifi_is_sta_enabled() != 1) {
        if (wifi_sta_enable() != ERRCODE_SUCC) {
            HILINK_SAL_ERROR("enable wifi fail\r\n");
            return HILINK_SAL_SET_WIFI_ERR;
        }
    }

    if (RegisterWifiEventToOhos() != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("register wifi event fail\r\n");
        return HILINK_SAL_SET_WIFI_ERR;
    }

    g_isReasonRefresh = false;

    if (AddBssidToWifiConfig(securityType, bssid, len) != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("add bssid to config failed\r\n");
        return HILINK_SAL_ADD_WIFI_ERR;
    }

    int netConnect = WIFI_DISCONNECTED;
    if (HILINK_GetNetworkState(&netConnect) != HILINK_SAL_OK) {
        /* 网络状态获取失败，不退出继续连接 */
        HILINK_SAL_ERROR("get network state failed\r\n");
    }
    if (netConnect == WIFI_CONNECTED) {
        /* 断开当前连接并重连 */
        if (wifi_sta_disconnect() != ERRCODE_SUCC) {
            HILINK_SAL_ERROR("disconnect wifi error\r\n");
            return HILINK_SAL_CONENCT_WIFI_ERR;
        }
    }

    wifi_sta_config_stru config;
    memset_s(&config, sizeof(config), 0, sizeof(config));
    if (GetWifiConfigFromOhos(&config) != HILINK_SAL_OK) {
        return HILINK_SAL_GET_WIFI_INFO_ERR;
    }

    config.ip_type = DHCP;
    if (wifi_sta_connect(&config) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("connect to wifi fail.\n");
        return HILINK_SAL_CONENCT_WIFI_ERR;
    }

    if (sta_setup_dhcp() != 0) {
        HILINK_SAL_ERROR("set sta dhcp failed\r\n");
        return HILINK_SAL_CONENCT_WIFI_ERR;
    }

    /* 指定完BSSID连接后及时清除BSSID配置，避免下次自动重连 */
    if (RemoveBssidFromWifiConfig() != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("remove bssid to config failed\r\n");
        return HILINK_SAL_ADD_WIFI_ERR;
    }

    return HILINK_SAL_OK;
}

int HILINK_GetLastConnectResult(int *result)
{
    if (result == NULL) {
        HILINK_SAL_ERROR("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    if (!g_isReasonRefresh) {
        return HILINK_SAL_NOK;
    }

    *result = g_disconnectReason;
    g_isReasonRefresh = false;
    return HILINK_SAL_OK;
}

static int BuildScanParam(const HILINK_APScanParam *param, wifi_scan_params_stru *scanParams)
{
    HILINK_WifiScanType scanType = param->scanType;
    HILINK_SAL_DEBUG("scan type: %d\r\n", scanType);

    switch (scanType) {
        case WIFI_SCAN_TPYE_SSID: {
            unsigned int ssidLen = HILINK_Strlen((char *)param->ssid);
            if ((param->ssid[0] == '\0') || (ssidLen != param->ssidLen)) {
                HILINK_SAL_ERROR("invalid ssid param, len: %u, strlen: %u\r\n", param->ssidLen, ssidLen);
                return HILINK_SAL_PARAM_INVALID;
            }
            if (memcpy_s(scanParams->ssid, sizeof(scanParams->ssid), param->ssid, param->ssidLen) != EOK) {
                HILINK_SAL_ERROR("memcpy error\r\n");
                return HILINK_SAL_MEMCPY_ERR;
            }
            scanParams->scan_type = WIFI_SSID_SCAN;
            scanParams->ssid_len = param->ssidLen;
            break;
        }
        case WIFI_SCAN_TPYE_BSSID: {
            if (memcpy_s(scanParams->bssid, sizeof(scanParams->bssid), param->bssid, sizeof(param->ssid)) != EOK) {
                HILINK_SAL_ERROR("memcpy error\r\n");
                return HILINK_SAL_MEMCPY_ERR;
            }
            scanParams->scan_type = WIFI_BSSID_SCAN;
            break;
        }
        case WIFI_SCAN_TPYE_FREQ: {
            scanParams->scan_type = WIFI_CHANNEL_SCAN;
            break;
        }
        default:
            HILINK_SAL_ERROR("not support scan type, type: %d\r\n", scanType);
            return HILINK_SAL_PARAM_INVALID;
    }

    return HILINK_SAL_OK;
}

int HILINK_ScanAP(const HILINK_APScanParam *param)
{
    if (param == NULL) {
        HILINK_SAL_ERROR("scan param invalid\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    /* 激活WiFi */
    if (wifi_is_sta_enabled() != 1) {
        if (wifi_sta_enable() != ERRCODE_SUCC) {
            HILINK_SAL_ERROR("enable wifi fail\r\n");
            return HILINK_SAL_SET_WIFI_ERR;
        }
    }
    if ((get_wifi_recovery_type() != 0) && (g_offline_mode_scan_flag == 0)) {
        wifi_scan_strategy_stru scan_strategy;
        scan_strategy.scan_time = 105; /* 105: 单信道扫描停留时间105ms */
        scan_strategy.scan_cnt = 2; /* 2: 每个信道扫描2次 */
        scan_strategy.single_probe_send_times = 1; /* 1: 单信道扫描每次发送一个probe request */
        scan_strategy.reserved = 0;
        if (wifi_sta_set_scan_policy(IFTYPE_STA, &scan_strategy) != ERRCODE_SUCC) {
            HILINK_SAL_ERROR("sta_set_scan_policy failed\r\n");
        }
        g_offline_mode_scan_flag = 1; /* 1: 已完成离线优化模式扫描策略设置 */
    }

    if (RegisterWifiEventToOhos() != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("register wifi event fail\r\n");
        return HILINK_SAL_SET_WIFI_ERR;
    }

    /* 指定WiFi的ssid进行扫描 */
    g_isStaScanSuccess = false;

    /* 组装扫描参数，填充SSID */
    wifi_scan_params_stru scanParams;
    (void)memset_s(&scanParams, sizeof(scanParams), 0, sizeof(scanParams));
    int ret = BuildScanParam(param, &scanParams);
    if (ret != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("build scan param failed\r\n");
        return ret;
    }
    if (wifi_sta_scan_advance(&scanParams) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("wifi advance scan fail\r\n");
        return HILINK_SAL_SCAN_WIFI_ERR;
    }
    (void)memset_s(&scanParams, sizeof(scanParams), 0, sizeof(scanParams));

    return HILINK_SAL_OK;
}

static int GetScanWifiResultList(wifi_scan_info_stru **list, unsigned int *listSize)
{
    unsigned int size = WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = (wifi_scan_info_stru *)HILINK_Malloc(sizeof(wifi_scan_info_stru) * size);
    if (result == NULL) {
        HILINK_SAL_ERROR("malloc error\r\n");
        return HILINK_SAL_MALLOC_ERR;
    }
    (void)memset_s(result, sizeof(wifi_scan_info_stru) * size, 0, sizeof(wifi_scan_info_stru) * size);

    if (wifi_sta_get_scan_info(result, &size) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("Get wifi scan info fail.\r\n");
        HILINK_Free(result);
        return HILINK_SAL_SCAN_WIFI_ERR;
    }
    *list = result;
    *listSize = size;
    return HILINK_SAL_OK;
}

static int CopyScanWifiResultList(HILINK_APList *scanList, wifi_scan_info_stru *result, unsigned int resSize)
{
    HILINK_APInfo *info = (HILINK_APInfo *)HILINK_Malloc(sizeof(HILINK_APInfo) * resSize);
    if (info == NULL) {
        HILINK_SAL_ERROR("malloc error\r\n");
        return HILINK_SAL_MALLOC_ERR;
    }
    (void)memset_s(info, sizeof(HILINK_APInfo) * resSize, 0, sizeof(HILINK_APInfo) * resSize);
    for (unsigned int i = 0; i < resSize; i++) {
        if ((strcpy_s(info[i].ssid, sizeof(info[i].ssid), result[i].ssid) != EOK) ||
            (memcpy_s(info[i].bssid, sizeof(info[i].bssid), result[i].bssid, sizeof(result[i].bssid)) != EOK)) {
            HILINK_Free(info);
            return HILINK_SAL_MEMCPY_ERR;
        }
        info[i].rssi = result[i].rssi;
        info[i].band = result[i].band;
        info[i].securityType = result[i].security_type;
        info[i].frequency = result[i].channel_num;
    }
    scanList->apList = info;
    scanList->num = resSize;
    return HILINK_SAL_OK;
}

int HILINK_GetAPScanResult(HILINK_APList *scanList)
{
    if (scanList == NULL) {
        HILINK_SAL_ERROR("invalid params\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    if (!g_isStaScanSuccess) {
        HILINK_SAL_NOTICE("AP scanning is not complete.\r\n");
        return WIFI_SCANNING;
    }

    unsigned int size = 0;
    wifi_scan_info_stru *scanInfo = NULL;
    int ret = GetScanWifiResultList(&scanInfo, &size);
    if (ret != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("get scan wifi list failed, ret = %d.\r\n", ret);
        return ret;
    }

    HILINK_SAL_NOTICE("scan result size: %u\r\n", size);
    if (size == 0) {
        HILINK_Free(scanInfo);
        scanList->apList = NULL;
        scanList->num = 0;
        return HILINK_SAL_OK;
    }

    ret = CopyScanWifiResultList(scanList, scanInfo, size);
    if (ret != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("copy wifi list failed, ret = %d.\r\n", ret);
        (void)memset_s(scanInfo, size * sizeof(wifi_scan_info_stru), 0, size * sizeof(wifi_scan_info_stru));
        HILINK_Free(scanInfo);
        scanInfo = NULL;
        return ret;
    }

    (void)memset_s(scanInfo, size * sizeof(wifi_scan_info_stru), 0, size * sizeof(wifi_scan_info_stru));
    HILINK_Free(scanInfo);
    scanInfo = NULL;

    return HILINK_SAL_OK;
}

int HILINK_ConnectWiFi(void)
{
    int ret;
    wifi_sta_config_stru config;
    (void)memset_s(&config, sizeof(config), 0, sizeof(config));
    wifi_scan_info_stru info;
    (void)memset_s(&info, sizeof(info), 0, sizeof(info));

    if (wifi_is_sta_enabled() != 1) {
        if (wifi_sta_enable() != ERRCODE_SUCC) {
            HILINK_SAL_ERROR("enable wifi fail\r\n");
            return HILINK_SAL_SET_WIFI_ERR;
        }
    }

    if (RegisterWifiEventToOhos() != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("register wifi event fail\r\n");
        return HILINK_SAL_SET_WIFI_ERR;
    }

    if (GetWifiConfigFromOhos(&config) != HILINK_SAL_OK) {
        HILINK_SAL_ERROR("get wifi config fail\r\n");
        return HILINK_SAL_GET_WIFI_INFO_ERR;
    }

    if (config.ssid[0] == '\0') {
        HILINK_SAL_ERROR("ssid null\r\n");
        return HILINK_SAL_GET_WIFI_INFO_ERR;
    }

    for (unsigned int i = 0; i < MAX_SCAN_TIMES; ++i) {
        if (AdvanceScanWifiByOhos(&config) != HILINK_SAL_OK) {
            HILINK_SAL_ERROR("advance scan wifi fail\r\n");
            (void)memset_s(&config, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));
            return HILINK_SAL_SCAN_WIFI_ERR;
        }
        if (GetScanWifiResultFromOhos(&config, &info)) {
            SetSecurityTypeByScanInfo(&config, &info);
            break;
        }
        HILINK_SAL_NOTICE("not find target wifi, try again\r\n");
    }

    /* 1 5 10 5 ：非离线优化模式使能自动重连，单次重连超时时间 5秒，重连间隔10秒 ，最大重连次数3600次 */
    if (wifi_sta_set_reconnect_policy(1, 5, 10, 3600) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("set reconnect policy error.\r\n");
    }

    config.ip_type = DHCP;
    if (wifi_sta_connect(&config) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("connect to wifi fail.\n");
        return HILINK_SAL_CONENCT_WIFI_ERR;
    }

    if (sta_setup_dhcp() != 0) {
        HILINK_SAL_ERROR("set sta dhcp failed\r\n");
        return HILINK_SAL_CONENCT_WIFI_ERR;
    }
    return HILINK_SAL_OK;
}

int HILINK_GetNetworkState(int *state)
{
    if (state == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    wifi_linked_info_stru info;
    (void)memset_s(&info, sizeof(info), 0, sizeof(info));
    /* 获取不到linkinfo时认为网络未连接 */
    if (wifi_sta_get_ap_info(&info) != ERRCODE_SUCC) {
        info.conn_state = WIFI_DISCONNECTED;
    }

    *state = (info.conn_state == WIFI_CONNECTED) ? 1 : 0;
    return HILINK_SAL_OK;
}

int HILINK_GetWiFiBssid(unsigned char *bssid, unsigned char *bssidLen)
{
    if ((bssid == NULL) || (bssidLen == NULL) || (*bssidLen == 0)) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    wifi_linked_info_stru info;
    (void)memset_s(&info, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));

    if (wifi_sta_get_ap_info(&info) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("get wifi linked info fail\r\n");
        return HILINK_SAL_GET_WIFI_INFO_ERR;
    }

    if (memcpy_s(bssid, *bssidLen, info.bssid, sizeof(info.bssid)) != EOK) {
        HILINK_SAL_ERROR("memcpy error\r\n");
        return HILINK_SAL_MEMCPY_ERR;
    }

    *bssidLen = WIFI_MAC_LEN;
    return HILINK_SAL_OK;
}

int HILINK_GetWiFiRssi(signed char *rssi)
{
    if (rssi == NULL) {
        HILINK_SAL_WARN("invalid param\r\n");
        return HILINK_SAL_PARAM_INVALID;
    }

    wifi_linked_info_stru info;
    (void)memset_s(&info, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));
    if (wifi_sta_get_ap_info(&info) != ERRCODE_SUCC) {
        HILINK_SAL_ERROR("get wifi linked info fail\r\n");
        return HILINK_SAL_GET_WIFI_INFO_ERR;
    }

    *rssi = (signed char)info.rssi;
    return HILINK_SAL_OK;
}