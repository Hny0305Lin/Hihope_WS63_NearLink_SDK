/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __AT_WIFI_H__
#define __AT_WIFI_H__

#include "td_base.h"
#include "wifi_device_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define EXT_RECONN_PERIOD_MAX 65535
#define EXT_RECONN_PERIOD_MIN 1
#define EXT_RECONN_COUNT_MAX 65535
#define EXT_RECONN_COUNT_MIN 1
#define EXT_RECONN_TIMEOUT_MIN 2
#define EXT_WIFI_AP_KEY_LEN_MAX 64
#define EXT_WIFI_AP_KEY_LEN_MIN 8
#define EXT_WIFI_AP_WEP_KEY_LEN_MAX 13
#define EXT_WIFI_AP_WEP_KEY_LEN_MIN 5
#define EXT_WIFI_AP_WEP_HEX_KEY_LEN_MAX 26
#define EXT_WIFI_AP_WEP_HEX_KEY_LEN_MIN 10
#define EXT_WIFI_MS_KEY_LEN_MAX 63
#define EXT_WIFI_MS_KEY_LEN_MIN 8
#define EXT_WIFI_CONTRY_CODE_LEN 3
#define EXT_WIFI_AP_KEY_LEN 64
#define WIFI_IFNAME_MAX_SIZE 16
#define EXT_WIFI_TXT_ADDR_LEN 17
#define WIFI_SCAN_AP_LIMIT 64
#define EXT_WIFI_USR_IE_MAX_SIZE 257
#define WIFI_WPS_PIN_LEN 8
#define EXT_WIFI_OK 0
#define EXT_WIFI_FAIL (-1)
#define WIFI_MAX_NUM_USER 6

typedef enum wifi_wps_method {
    WIFI_WPS_PBC,            /**< Connection mode of WPS PBC. CNcomment:WPS PBC连接方式.CNend */
    WIFI_WPS_PIN,            /**< Connection mode of WPS PIN. CNcomment:WPS PIN连接方式.CNend */
    WIFI_WPS_BUTT,
} wifi_wps_method;

typedef struct wifi_wps_config {
    wifi_wps_method wps_method;                   /**< Connection mode of WPS.CNcomment:WPS的连接方式.CNend */
    unsigned char bssid[WIFI_MAC_LEN];         /**< Basic service set ID (BSSID).CNcomment:BSSID.CNend */
    char wps_pin[WIFI_WPS_PIN_MAX_LEN_NUM];    /**< Pin Code of WPS.CNcomment:WPS的Pin码.CNend */
} wifi_wps_config;

TD_CONST char *at_ssid_txt(TD_CONST td_u8 *ssid, td_u32 ssid_len);
td_void los_at_wifi_cmd_register(td_void);
td_void los_at_sta_factory_test_cmd_register(td_void);
td_void los_at_mesh_cmd_register(td_void);
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
td_void los_at_mfg_ccpriv_cmd_register(td_void);
#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of app_sta.h */
