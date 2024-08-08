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
#include "at_wifi.h"
#include "at.h"
#include "wifi_device.h"
#include "wifi_alg.h"
#include "wifi_hotspot.h"
#include "at_cmd_register.h"
#include "lwip/netifapi.h"
#include "at_register.h"
#include "at_cmd_register.h"
#include "at_ccpriv.h"
#include "at_utils.h"
#include "at_wifi_cmd_table.h"
#include "debug_print.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define los_unref_param(P)  ((P) = (P))
#define EXT_AT_SSID_MAX_LEN 32
#define SAE_GROUPS_MAX_NUM 4

typedef enum {
    EXT_WIFI_BW_HIEX_5M,     /**< 5M bandwidth. CNcomment: 窄带5M带宽.CNend */
    EXT_WIFI_BW_HIEX_10M,    /**< 10M bandwidth. CNcomment: 窄带10M带宽.CNend */
    EXT_WIFI_BW_LEGACY_20M,  /**< 20M bandwidth. CNcomment: 20M带宽.CNend */
    EXT_WIFI_BW_BUTT
} ext_wifi_bw;
ext_wifi_bw g_bw_setup_value = EXT_WIFI_BW_LEGACY_20M;
#ifdef _PRE_PSK_CALC_USER
unsigned char g_psk_calc[EXT_WIFI_STA_PSK_LEN] = {0};
#endif
int g_csi_report_count = 0;

typedef struct {
    char ifname[WIFI_IFNAME_MAX_SIZE + 1];
    wifi_if_type_enum iftype;
} ext_ifname_to_iftype;

TD_PRV int at_pading(char *str, td_u32 size, TD_CONST char *format, ...)
{
    va_list ap = TD_NULL;
    int ret;

    va_start(ap, format);
    ret = vsnprintf_s(str, EXT_AT_SSID_MAX_LEN * 4 + 1, size, format, ap); /* 4 length */
    va_end(ap);
    if (size > 0) {
        str[size - 1] = '\0';
    }
    return ret;
}

TD_PRV td_void at_printf_encode(char *txt, td_u32 maxlen, TD_CONST td_u8 *data, td_u32 len)
{
    char *end = txt + maxlen;
    td_u32 i;

    for (i = 0; i < len; i++) {
        if (txt + 4 >= end) {   /* add 4 */
            break;
        }

        switch (data[i]) {
            case '\"':
                *txt++ = '\\';
                *txt++ = '\"';
                break;
            case '\\':
                *txt++ = '\\';
                *txt++ = '\\';
                break;
            case '\033':
                *txt++ = '\\';
                *txt++ = 'e';
                break;
            case '\n':
                *txt++ = '\\';
                *txt++ = 'n';
                break;
            case '\r':
                *txt++ = '\\';
                *txt++ = 'r';
                break;
            case '\t':
                *txt++ = '\\';
                *txt++ = 't';
                break;
            default:
                if (data[i] >= 32 && data[i] <= 126) {  /* range [32,126] */
                    *txt++ = data[i];
                } else {
                    txt += at_pading(txt, end - txt, "\\x%02x", data[i]);
                }
                break;
        }
    }

    *txt = '\0';
}

TD_PRV int at_start_softap(softap_config_stru *hapd_conf)
{
    if (hapd_conf->security_type == WIFI_SEC_TYPE_WPA2PSK || hapd_conf->security_type == WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX ||
        hapd_conf->security_type == WIFI_SEC_TYPE_WPA3_WPA2_PSK_MIX) {
        // 此类加密方式hex固定秘钥长度为64
        if (strlen(hapd_conf->pre_shared_key) == 64) {
            hapd_conf->wifi_psk_type = 1;
        }
    }

    /* WEP加密：HEX密码长度只能为10或者26 */
    if (hapd_conf->security_type == WIFI_SEC_TYPE_WEP || hapd_conf->security_type == WIFI_SEC_TYPE_WEP_OPEN) {
        /* HEX密码长度为10bytes or 26bytes */
        if (strlen(hapd_conf->pre_shared_key) == 10 || strlen(hapd_conf->pre_shared_key) == 26) {
            hapd_conf->wifi_psk_type = 1;
        }
        /* ASCII密码长度为5bytes or 13bytes */
        if (strlen(hapd_conf->pre_shared_key) == 5 || strlen(hapd_conf->pre_shared_key) == 13) {
            hapd_conf->wifi_psk_type = 0;
        }
    }
    if (wifi_softap_enable(hapd_conf) != ERRCODE_SUCC) {
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

TD_PRV int at_set_softap_advance(softap_config_advance_stru *config, const setapadv_args_t *argv)
{
    config->protocol_mode = ((argv->para_map & 1) != 0) ? argv->protocol_mode : WIFI_MODE_11B_G_N_AX;
    /* 100:beacon_interval默认值 */
    config->beacon_interval = ((argv->para_map & (1 << 1)) != 0) ? (uint32_t)argv->beacon_interval : 100;
    /* 2:左移位数 2:dtim_period默认值        */
    config->dtim_period = ((argv->para_map & (1 << 2)) != 0) ? (uint32_t)argv->dtim_period : 2;
    /* 3:左移位数 86400:dtim_period默认值 */
    config->group_rekey = ((argv->para_map & (1 << 3)) != 0) ? (uint32_t)argv->group_rekey : 86400;
    /* 4:左移位数 */
    config->hidden_ssid_flag = ((argv->para_map & (1 << 4)) != 0) ? (uint32_t)argv->hidden_ssid_flag : 1;
    /* 5:左移位数 */
    config->gi = ((argv->para_map & (1 << 5)) != 0) ? (uint32_t)argv->gi : 1;

    return EXT_ERR_SUCCESS;
}

TD_PRV int at_filter_sta_key(wifi_sta_config_stru *assoc_req, TD_CONST td_char *buf, td_s32 len)
{
#ifndef AT_COMMAND
    if ((*buf != '\"') || (*(buf + len - 1) != '\"')) {
        return EXT_ERR_FAILURE;
    }
#endif

    if (assoc_req->security_type == WIFI_SEC_TYPE_WEP || assoc_req->security_type == WIFI_SEC_TYPE_WAPI_PSK) {
        if ((*(buf + 1) != '\"') || (*(buf + len - 2) != '\"')) {
            assoc_req->wifi_psk_type = 1;
        } else {
            /* len minus 4, buf add 2 */
            if (memcpy_s((td_char*)assoc_req->pre_shared_key, WIFI_MAX_KEY_LEN, buf + 2, len - 4) != EOK) {
                return EXT_ERR_FAILURE;
            }
            return EXT_ERR_SUCCESS;
        }
    }
    /* len minus 2 */
    if (memcpy_s((td_char*)assoc_req->pre_shared_key, WIFI_MAX_KEY_LEN, buf + 1, len - 2) != EOK) {
        return EXT_ERR_FAILURE;
    }

    int key_len = strlen(assoc_req->pre_shared_key);
    assoc_req->pre_shared_key[key_len] = '\0';
    if (assoc_req->security_type == WIFI_SEC_TYPE_WPA2PSK || assoc_req->security_type == WIFI_SEC_TYPE_WPA2_WPA_PSK_MIX ||
        assoc_req->security_type == WIFI_SEC_TYPE_WPA3_WPA2_PSK_MIX || assoc_req->security_type == WIFI_SEC_TYPE_WPAPSK) {
        // 此类加密方式hex固定秘钥长度为64
        if (key_len == 64) {
            assoc_req->wifi_psk_type = 1;
        }
    }
    return EXT_ERR_SUCCESS;
}

/* 去掉关联时AT输入密码的双引号 */
at_ret_t at_strip_sta_key(wifi_sta_config_stru *assoc_req, TD_CONST td_char *at_sta_key, td_s32 len)
{
    /* AT输入的密码前后必须带有" */
    if ((*at_sta_key != '\"') || (*(at_sta_key + len - 1) != '\"')) {
        return AT_RET_SYNTAX_ERROR;
    }

    /* 密钥使用一层双引号，ASCII与HEX密钥类型用psk_type参数区分 */
    if (memcpy_s((td_char*)assoc_req->pre_shared_key, sizeof(assoc_req->pre_shared_key),
        at_sta_key + 1, len - 2) != EOK) {
        return AT_RET_SYNTAX_ERROR;
    }

    assoc_req->pre_shared_key[strlen(assoc_req->pre_shared_key)] = '\0';
    return AT_RET_OK;
}

at_ret_t at_check_security_key_len(wifi_security_enum security_type, int len)
{
    if (security_type == WIFI_SEC_TYPE_WEP || security_type == WIFI_SEC_TYPE_WEP_OPEN) { /* WEP加密 */
        if ((len != EXT_WIFI_AP_WEP_KEY_LEN_MAX + 2) && (len != EXT_WIFI_AP_WEP_KEY_LEN_MIN + 2) && /* len plus 2 */
            (len != EXT_WIFI_AP_WEP_HEX_KEY_LEN_MAX + 2) && (len != EXT_WIFI_AP_WEP_HEX_KEY_LEN_MIN + 2)) { /* plus 2 */
            return AT_RET_SYNTAX_ERROR;
        }
    } else {    /* 其他加密方式 */
        if ((len > EXT_WIFI_AP_KEY_LEN_MAX + 2) || (len < EXT_WIFI_AP_KEY_LEN_MIN + 2)) { /* argc 4, len plus 2 */
            return AT_RET_SYNTAX_ERROR;
        }
    }
    return AT_RET_OK;
}

/**
 * at_ssid_txt - Convert SSID to a printable string
 * @ssid: SSID (32-octet string)
 * @ssid_len: Length of ssid in octets
 * Returns: Pointer to a printable string
 *
 * This function can be used to convert SSIDs into printable form. In most
 * cases, SSIDs do not use unprintable characters, but IEEE 802.11 standard
 * does not limit the used character set, so anything could be used in an SSID.
 *
 * This function uses a TD_PRV buffer, so only one call can be used at the
 * time, i.e., this is not re-entrant and the returned buffer must be used
 * before calling this again.
 */
TD_CONST char *at_ssid_txt(TD_CONST td_u8 *ssid, td_u32 ssid_len)
{
    TD_PRV char ssid_txt[EXT_AT_SSID_MAX_LEN * 4 + 1]; /* 4 */

    if (ssid == TD_NULL) {
        ssid_txt[0] = '\0';
        return ssid_txt;
    }

    at_printf_encode(ssid_txt, sizeof(ssid_txt), ssid, ssid_len);
    return ssid_txt;
}

at_ret_t cmd_send_custom_pkt(const sendpkt_args_t *args)
{
    td_u32 len;
    td_u32 iftype = args->para1;
    TD_CONST td_char *data = args->para2;
    unsigned char *buf = TD_NULL;

    /* data是以16进制写入，两个16进制的字符组成一个字节，所以报文的字节数为data的长度除以2，取整数部分 */
    len = strlen(data) / 2;
    buf = malloc(len);
    if (buf == TD_NULL) {
        osal_printk("wifi_send_custom_pkt malloc len[%d] fail.\r\n", len);
        return AT_RET_SYNTAX_ERROR;
    }
    (td_void)memset_s(buf, len, 0, len);
    at_str_to_hex(data, strlen(data), buf);
    if (wifi_send_custom_pkt((wifi_if_type_enum)iftype, buf, len) != ERRCODE_SUCC) {
        free(buf);
        osal_printk("wifi_send_custom_pkt fail.\r\n");
        return AT_RET_SYNTAX_ERROR;
    }

    free(buf);
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t ssid_prefix_scan(const scanssid_args_t *args, td_u32 prefix_flag)
{
    td_s32  ret;
    errno_t rc;
    char   *tmp = TD_NULL;
    td_u32  ssid_len = 0;
    wifi_scan_params_stru scan_params = {0};
    td_s32 argc = convert_bin_to_dec(args->para_map);

    if (argc != 1) {
        return AT_RET_SYNTAX_ERROR;
    }

    /* ssid hex to string */
    tmp = at_parse_string(args->para1, &ssid_len);
    if (tmp == TD_NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    if ((ssid_len > EXT_AT_SSID_MAX_LEN) || (ssid_len == 0)) {
        osal_kfree(tmp);
        return AT_RET_SYNTAX_ERROR;
    }
    scan_params.ssid_len = (unsigned char)ssid_len;
    if ((scan_params.ssid_len > EXT_AT_SSID_MAX_LEN) || (scan_params.ssid_len == 0)) {
        osal_kfree(tmp);
        return AT_RET_SYNTAX_ERROR;
    }
    rc = memcpy_s(scan_params.ssid, EXT_AT_SSID_MAX_LEN + 1, tmp, strlen(tmp) + 1);
    osal_kfree(tmp);
    if (rc != EOK) {
        return AT_RET_SYNTAX_ERROR;
    }

    scan_params.ssid[scan_params.ssid_len] = '\0';

    scan_params.scan_type = (prefix_flag == 1) ? WIFI_SSID_PREFIX_SCAN : WIFI_SSID_SCAN;

    ret = wifi_sta_scan_advance(&scan_params);
    if (ret != EXT_WIFI_OK) {
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}


/*****************************************************************************
* Func description: wpa ssid scan
*****************************************************************************/
at_ret_t cmd_wpa_ssid_scan(const scanssid_args_t *args)
{
    td_u32 ret = ssid_prefix_scan(args, 0);
    return ret;
}

/*****************************************************************************
* Func description: wpa channel scan
*****************************************************************************/
at_ret_t cmd_wpa_channel_scan(const scanchn_args_t *args)
{
    td_s32 ret;
    wifi_scan_params_stru scan_params = {0};
    scan_params.channel_num = args->para1;
    scan_params.scan_type = WIFI_CHANNEL_SCAN;
    ret = wifi_sta_scan_advance(&scan_params);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: bssid scan
* example: AT+SCANBSSID=XX:XX:XX:XX:XX:XX
*****************************************************************************/
at_ret_t cmd_wpa_bssid_scan(const scanbssid_args_t *args)
{
    wifi_scan_params_stru scan_params = {0};
    td_s32 ret;
    td_s32 argc = convert_bin_to_dec(args->para_map);

    if ((argc != 1) || (strlen(args->para1) != EXT_WIFI_TXT_ADDR_LEN)) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (cmd_strtoaddr(args->para1, scan_params.bssid, WIFI_MAC_LEN) != EXT_WIFI_OK) {
        return AT_RET_SYNTAX_ERROR;
    }

    scan_params.scan_type = WIFI_BSSID_SCAN;
    ret = wifi_sta_scan_advance(&scan_params);
    if (ret != EXT_WIFI_OK) {
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: wpa scan
*****************************************************************************/
at_ret_t cmd_wpa_scan(void)
{
    td_s32 ret;
    uapi_at_report("SCAN \r\n");

    ret = wifi_sta_scan();
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: cmd_wpa_scan_result_clear
*****************************************************************************/
at_ret_t cmd_wpa_scan_result_clear(void)
{
    td_u32 ret;
    uapi_at_report("CLEAR SCAN RESULT \r\n");

    ret = wifi_sta_scan_result_clear();
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: ssid prefix scan
* example: AT+SCANPRSSID="CompanyName"
*****************************************************************************/
at_ret_t cmd_ssid_prefix_scan(const scanssid_args_t *args)
{
    td_u32 ret = ssid_prefix_scan(args, 1);
    return ret;
}

TD_PRV td_u32 at_check_ccharacter(TD_CONST td_char *tmp)
{
    if (tmp == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    for (; *tmp != '\0'; tmp++) {
        if (*tmp == '\\') {
            if (*(tmp + 1) == '\\') {
                tmp++;
                continue;
            } else if (*(tmp + 1) == 'x') {
                return EXT_ERR_SUCCESS;
            }
        }
    }
    return EXT_ERR_FAILURE;
}

/*****************************************************************************
* Func description: wpa get scan results
*****************************************************************************/
at_ret_t cmd_wpa_scan_results(void)
{
    td_u32  num = WIFI_SCAN_AP_LIMIT;
    td_char ssid_str[EXT_AT_SSID_MAX_LEN * 4 + 3]; /* ssid length should less 32*4+3 */

    wifi_scan_info_stru *results = malloc(sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT);
    if (results == TD_NULL) {
        return AT_RET_MEM_API_ERROR;
    }

    memset_s(results, sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT, 0, sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT);

    td_s32 ret = wifi_sta_get_scan_info(results, &num);
    if (ret != ERRCODE_SUCC) {
        free(results);
        return AT_RET_MEM_API_ERROR;
    }

    for (td_u32 ul_loop = 0; (ul_loop < num) && (ul_loop < WIFI_SCAN_AP_LIMIT); ul_loop++) {
        if ((results[ul_loop].security_type < WIFI_SEC_TYPE_OPEN) ||
            (results[ul_loop].security_type > WIFI_SEC_TYPE_UNKNOWN)) {
            results[ul_loop].security_type = WIFI_SEC_TYPE_UNKNOWN;
        }

        td_u32 service_flag = 0;


        td_u32 ssid_len = strlen(results[ul_loop].ssid);
        TD_CONST char* tmp = at_ssid_txt((unsigned char*)results[ul_loop].ssid, ssid_len);
        if (at_check_ccharacter(tmp) == EXT_ERR_SUCCESS) {
            ret = sprintf_s(ssid_str, EXT_AT_SSID_MAX_LEN * 4 + 3, "P\"%s\"", tmp); /* ssid len should less 32*4+3 */
        } else {
            ret = sprintf_s(ssid_str, EXT_AT_SSID_MAX_LEN * 4 + 3, "%s", results[ul_loop].ssid); /* less 32*4+3 */
        }
        if (ret < 0) {
            free(results);
            return AT_RET_MEM_API_ERROR;
        }

        osal_printk("+SCANRESULT:%s,"EXT_AT_MACSTR",%d,%d,%d\r\n",
                       ssid_str, ext_at_mac2str(results[ul_loop].bssid),
                       results[ul_loop].channel_num, results[ul_loop].rssi, results[ul_loop].security_type);
    }

    free(results);

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: get country code
*****************************************************************************/
at_ret_t cmd_wpa_get_country(void)
{
    td_s32 ret;
    td_s8 cc[EXT_WIFI_CONTRY_CODE_LEN];
    td_u8 len = EXT_WIFI_CONTRY_CODE_LEN;

    memset_s(cc, EXT_WIFI_CONTRY_CODE_LEN, 0, EXT_WIFI_CONTRY_CODE_LEN);
    ret = wifi_get_country_code(cc, &len);

    if (ret != ERRCODE_SUCC) {
        osal_printk("ERROR\r\n");
        return AT_RET_MEM_API_ERROR;
    }
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: get country code
*****************************************************************************/
at_ret_t cmd_wpa_set_country(const cc_args_t *args)
{
    td_s32 ret;
    td_s32 argc = convert_bin_to_dec(args->para_map);

    if ((argc != 1) || (args->para1 == TD_NULL)) {
        osal_printk("ERROR\r\n");
        return AT_RET_MEM_API_ERROR;
    }
    /* 这里传递国家码长度需带上末尾结束符 */
    ret = wifi_set_country_code(args->para1, strlen(args->para1) + 1);
    if (ret != ERRCODE_SUCC) {
        osal_printk("ERROR\r\n");
        return AT_RET_MEM_API_ERROR;
    }

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_set_sae_pwe(const pwe_args_t *args)
{
    td_u32 ret = wifi_sta_set_sae_pwe(args->para1);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    ret = wifi_softap_set_sae_pwe(args->para1);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_set_sae_groups(const grp_args_t *args)
{
    td_u32 ret;
    td_u32 i = 0;
    td_u32 groups[SAE_GROUPS_MAX_NUM] = {0};

    for (i = 0; i < SAE_GROUPS_MAX_NUM; i++) {
        groups[i] = *(&(args->para1) + i);
    }

    ret = wifi_sta_set_sae_groups(groups, sizeof(groups));
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }

    ret = wifi_softap_set_sae_groups(groups, sizeof(groups));
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_set_sae_transition(const trans_args_t *args)
{
    td_u32 ret;

    ret = wifi_softap_set_transition(args->para1);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_set_sae_anti_clog_threshold(const clog_args_t *args)
{
    td_u32 ret;

    ret = wifi_softap_set_clog_threshold(args->para1);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_set_sta_assoc_auth(const sta_auth_type_args_t *args)
{
    td_u32 ret;

    ret = uapi_wifi_set_assoc_auth(args->para1);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}
/*****************************************************************************
* Func description: start sta
*****************************************************************************/
at_ret_t cmd_sta_start(void)
{
    td_s32  ret;

    uapi_at_report("EnableSta 1\r\n");
    ret = wifi_sta_enable();
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    uapi_at_report("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_sta_user_ie_set(const userieset_args_t *args)
{
    wifi_if_type_enum iftype = IFTYPES_BUTT;
    ie_index_enmu ie_index;
    td_u8 frame_type_bitmap;
    td_u8 user_ie[EXT_WIFI_USR_IE_MAX_SIZE];
    td_u16 user_ie_len;
    td_u32 ret;

    ext_ifname_to_iftype get_iftype[] = {
        {"wlan0",      IFTYPE_STA},     // sta request
        {"ap0",        IFTYPE_AP},          // ap beacon response
        {"p2p0",       IFTYPE_P2P_DEVICE},  // p2p request
        {"p2p-p2p0-0", IFTYPE_P2P_GO}       // p2p beacon response
    };

    for (int i = 0; i < sizeof(get_iftype) / sizeof(get_iftype[0]); i++) {
        if (strcmp(args->ifname, get_iftype[i].ifname) == 0) {
            iftype = get_iftype[i].iftype;
            break;
        }
    }

    switch(args->index) {
        case 1:               /* 1: WIFI_EXTEND_IE1 */
            ie_index = IE_FIRST;
            break;
        case 2:               /* 2: WIFI_EXTEND_IE2 */
            ie_index = IE_SECOND;
            break;
        case 3:               /* 3: WIFI_EXTEND_IE3 */
            ie_index = IE_THIRD;
            break;
        case 4:               /* 4: WIFI_EXTEND_IE4 */
            ie_index = IE_FORTH;
            break;
        default:
            ie_index = IE_BUTT;
            return AT_RET_SYNTAX_ERROR;
    }

    frame_type_bitmap = args->frame_type_bitmap;

    if (strcmp(args->cmd, "set") == 0) {
        user_ie_len = args->ie_len;
        for (int i = 0; i < user_ie_len + 2; i++) { /* len plus 2 */
            user_ie[i] = *(&(args->ie_type) + i);
        }

        ret = wifi_set_app_ie(iftype, ie_index, frame_type_bitmap, user_ie, user_ie_len + 2); /* len plus 2 */
    } else if (strcmp(args->cmd, "del") == 0) {
        ret = wifi_del_app_ie(iftype, ie_index, frame_type_bitmap);
    } else {
        return AT_RET_SYNTAX_ERROR;
    }

    if (ret != 0)
        return AT_RET_SYNTAX_ERROR;

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

TD_PRV td_u32 sta_start_adv_param(const startsta_args_t *args)
{
    td_s32  ret, protocol_mode, pmf_option;

    protocol_mode = ((args->para_map & 1) != 0) ? args->para1 : WIFI_MODE_11B_G_N_AX;
    pmf_option = ((args->para_map & (1 << 1)) != 0) ? args->para2 : 1;
#ifndef CONFIG_FACTORY_TEST_MODE
    ret = wifi_sta_set_protocol_mode((protocol_mode_enum)protocol_mode);
    if (ret != EXT_WIFI_OK) {
        return EXT_ERR_FAILURE;
    }

    ret = wifi_sta_set_pmf_mode((wifi_pmf_option_enum)pmf_option);
    if (ret != EXT_WIFI_OK) {
        return EXT_ERR_FAILURE;
    }
#endif

    los_unref_param(ret);
    return EXT_ERR_SUCCESS;
}

at_ret_t cmd_sta_start_adv(const startsta_args_t *args)
{
    td_s32 ret;
    td_s32 argc = convert_bin_to_dec(args->para_map);

    if (argc > 2) {
        return AT_RET_SYNTAX_ERROR;
    }

    ret = (td_s32)sta_start_adv_param(args);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
	uapi_at_report("EnableSta \r\n");
#ifndef CONFIG_FACTORY_TEST_MODE
    ret = wifi_sta_enable();
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
#endif
    uapi_at_report("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: stop station
*****************************************************************************/
at_ret_t cmd_sta_stop(void)
{
    uapi_at_report("cmd_sta_stop \r\n");
#ifndef CONFIG_FACTORY_TEST_MODE
    td_s32 ret = wifi_sta_disable();
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
#ifdef AT_DEBUG_EN
    for (int index = 0; index < DNS_MAX_SERVERS; index++) {
        lwip_dns_setserver(index, TD_NULL);
    }
#endif
#endif
    uapi_at_report("OK\r\n");
    return AT_RET_OK;
}

TD_PRV td_u32 cmd_sta_connect_get_ssid(const conn_args_t *args, wifi_sta_config_stru *assoc_req,
    wifi_fast_connect_stru *fast_assoc_req, td_u32 fast_flag)
{
    td_u32 ssid_len = 0;
    errno_t rc;

    /* ssid hex to string */
    td_char *tmp = at_parse_string((td_char *)(args->ssid), &ssid_len);
    if (tmp == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    if ((ssid_len > EXT_AT_SSID_MAX_LEN) || (ssid_len == 0)) {
        osal_kfree(tmp);
        return EXT_ERR_FAILURE;
    }

    if ((fast_flag == 0) && (assoc_req != TD_NULL)) {
        rc = memcpy_s(assoc_req->ssid, EXT_AT_SSID_MAX_LEN + 1, tmp, strlen(tmp) + 1);
        osal_kfree(tmp);
        if (rc != EOK) {
            return EXT_ERR_FAILURE;
        }
    } else if ((fast_flag == 1) && (fast_assoc_req != TD_NULL)) {
        rc = memcpy_s(fast_assoc_req->config.ssid, EXT_AT_SSID_MAX_LEN + 1, tmp, strlen(tmp) + 1);
        osal_kfree(tmp);
        if (rc != EOK) {
            return EXT_ERR_FAILURE;
        }
    } else {
        osal_kfree(tmp);
    }
    return EXT_ERR_SUCCESS;
}

TD_PRV td_u32 at_is_hex_char(td_char ch)
{
    if (((ch >= '0') && (ch <= '9')) ||
        ((ch >= 'A') && (ch <= 'F')) ||
        ((ch >= 'a') && (ch <= 'f'))) {
        return EXT_ERR_SUCCESS;
    }
    return EXT_ERR_FAILURE;
}

TD_PRV td_u32 at_is_hex_string(TD_CONST td_char *data, td_u16 len)
{
    td_u16 i;
    for (i = 0; i < len; i++) {
        if (at_is_hex_char(data[i]) == EXT_ERR_FAILURE) {
            return EXT_ERR_FAILURE;
        }
    }
    return EXT_ERR_SUCCESS;
}

TD_PRV td_u32 cmd_sta_connect_get_key(td_s32 argc, const fconn_args_t *args, wifi_fast_connect_stru *fast_assoc_req)
{
    if (wifi_is_need_psk(fast_assoc_req->config.security_type) == 1) { /* 5 parameters  */
        if (args->para5 == TD_NULL) { /* 4:key */
            return EXT_ERR_FAILURE;
        }
        TD_CONST td_char *buf = args->para5; /* 4:key */

        td_u32 len = strlen(args->para5); /* 4:key */
        if ((args->para4 == WIFI_SEC_TYPE_WEP) && (len != 9) && (len != 17) && /* 3:encrypt type, 9 17:len */
            (len != 12) && (len != 28)) { /* 12 28: password len */
            return EXT_ERR_FAILURE;
        } else if ((args->para4 != WIFI_SEC_TYPE_WEP && args->para4 != WIFI_SEC_TYPE_WAPI_PSK) &&
                   ((len > EXT_WIFI_AP_KEY_LEN_MAX + 2) || (len < EXT_WIFI_AP_KEY_LEN_MIN + 2))) {     /* len plus 2 */
            return EXT_ERR_FAILURE;
        } else if ((len == EXT_WIFI_AP_KEY_LEN_MAX + 2) &&       /* 2: len plus 2 */
            (at_is_hex_string(buf + 1, EXT_WIFI_AP_KEY_LEN_MAX) == EXT_ERR_FAILURE)) {
            return EXT_ERR_FAILURE;
        }
        if ((buf == TD_NULL) || (at_filter_sta_key(&fast_assoc_req->config, buf, len) != EXT_ERR_SUCCESS)) {
            return EXT_ERR_FAILURE;
        }
    }
    return EXT_ERR_SUCCESS;
}

/*****************************************************************************
* Func description: station connect network
* example: AT+CONN="CompanyName",,"123456789"[,0]
*****************************************************************************/
at_ret_t cmd_sta_connect(const conn_args_t *args)
{
    wifi_sta_config_stru assoc_req;
    td_s32 argc = convert_bin_to_dec(args->para_map);

    (void)memset_s(&assoc_req, sizeof(wifi_sta_config_stru), 0, sizeof(wifi_sta_config_stru));

    if (argc > 3) {     /* argc 3 */
        return AT_RET_SYNTAX_ERROR;
    }

    /* get ssid */
    if (args->ssid != TD_NULL) {
        osal_printk("cmd_sta_connect ssid=%s\r\n", args->ssid);
    }

    if ((args->ssid != TD_NULL) && (cmd_sta_connect_get_ssid(args, &assoc_req, TD_NULL, 0) != EXT_ERR_SUCCESS)) {
        return AT_RET_SYNTAX_ERROR;
    }

    /* get bssid */
    if (args->bssid == TD_NULL) {
        memset_s(assoc_req.bssid, sizeof(assoc_req.bssid), 0, sizeof(assoc_req.bssid));
    } else if (strlen((td_char *)(args->bssid)) == EXT_WIFI_TXT_ADDR_LEN) {
        if (cmd_strtoaddr((td_char *)(args->bssid), assoc_req.bssid, WIFI_MAC_LEN) != EXT_WIFI_OK) {
            return AT_RET_SYNTAX_ERROR;
        }
    } else {
        return AT_RET_SYNTAX_ERROR;
    }

    /* 校验并过滤AT输入密码时带上的双引号删除 */
    if (args->passwd != NULL) {
        if (at_strip_sta_key(&assoc_req, (td_char *)(args->passwd), strlen((td_char *)(args->passwd))) != AT_RET_OK) {
            uapi_at_report("passwd check err\r\n");
            return AT_RET_SYNTAX_ERROR;
        }
    }

    /* get psk_type */
    assoc_req.wifi_psk_type = ((args->para_map & (1 << 3)) != 0) ? args->psk_type : 0;  // 检查para3是否有值

    uapi_at_report("ConnectToNetwork \r\n");

    assoc_req.ip_type = DHCP;
    if (wifi_sta_connect(&assoc_req) != EXT_WIFI_OK) {
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_report("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: station quick connect
* example: AT+FCONN="CompanyName",,6,2,"xxxxxxxx"
           AT+FCONN=,XX:XX:XX:XX:XX:XX,6,2,"xxxxxxxx"
*****************************************************************************/
at_ret_t cmd_sta_quick_connect(const fconn_args_t *args)
{
    wifi_fast_connect_stru fast_assoc_req = {0};
    td_u32 ret;
    td_s32 argc = convert_bin_to_dec(args->para_map);
    if ((argc < 3) || (argc > 5)) { /* argc 3 / 4 / 5 */
        return AT_RET_SYNTAX_ERROR;
    }

    /* get ssid */
    if (args->para1 != TD_NULL) {
        osal_printk("cmd_sta_quick_connect ssid=%s\r\n", args->para1);
        ret = cmd_sta_connect_get_ssid(args, TD_NULL, &fast_assoc_req, 1);
        if (ret != EXT_ERR_SUCCESS) {
            return AT_RET_SYNTAX_ERROR;
        }
    }

    /* get bssid */
    if (args->para2 == TD_NULL) {
        memset_s(fast_assoc_req.config.bssid, sizeof(fast_assoc_req.config.bssid), 0, sizeof(fast_assoc_req.config.bssid));
    } else if (strlen(args->para2) == EXT_WIFI_TXT_ADDR_LEN) {
        if (cmd_strtoaddr(args->para2, fast_assoc_req.config.bssid, WIFI_MAC_LEN) != EXT_WIFI_OK) {
            return AT_RET_SYNTAX_ERROR;
        }
    } else {
        return AT_RET_SYNTAX_ERROR;
    }

    fast_assoc_req.channel_num = (td_uchar)(args->para3); /* 2 */

    /* get auth_type */
    if ((args->para4 < WIFI_SEC_TYPE_OPEN) ||
        (args->para4 == WIFI_SEC_TYPE_WPA) || (args->para4 == WIFI_SEC_TYPE_WPA2) ||
        (args->para4 == WIFI_SEC_TYPE_WPAPSK) || (args->para4 >= WIFI_SEC_TYPE_UNKNOWN) ||
        ((wifi_is_need_psk(args->para4) != 1) && (args->para5 != TD_NULL)) || // open不带密码
        ((wifi_is_need_psk(args->para4) == 1) && (args->para5 == TD_NULL))) { // 加密需带密码
        return AT_RET_SYNTAX_ERROR;
    }

    fast_assoc_req.config.security_type = (wifi_security_enum)args->para4; /* 3 */

    /* get key */
    ret = cmd_sta_connect_get_key(argc, args, &fast_assoc_req);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    fast_assoc_req.config.ip_type = DHCP;
    if (wifi_sta_fast_connect(&fast_assoc_req) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: sta disconnect network
*****************************************************************************/
at_ret_t cmd_sta_disconnect(void)
{
    td_s32 ret = wifi_sta_disconnect();
    if (ret != ERRCODE_SUCC) {
        osal_printk("ERROR\r\n");
        return AT_RET_MEM_API_ERROR;
    }

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: get station connection status
*****************************************************************************/
at_ret_t cmd_sta_status(void)
{
    td_s32 ret;
    wifi_linked_info_stru wifi_status;

    memset_s(&wifi_status, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));

    uapi_at_report("cmd_sta_status \r\n");

    ret = wifi_sta_get_ap_info(&wifi_status);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    if (wifi_status.conn_state == WIFI_CONNECTED) {
        TD_CONST td_char *tmp = at_ssid_txt((unsigned char*)wifi_status.ssid, strlen(wifi_status.ssid));
        if (at_check_ccharacter(tmp) == EXT_ERR_SUCCESS) {
            osal_printk("+STASTAT:1,P\"%s\","EXT_AT_MACSTR",%d,%d\r\n", tmp, ext_at_mac2str(wifi_status.bssid),
                wifi_status.channel_num, wifi_status.rssi);
        } else {
            osal_printk("+STASTAT:1,%s,"EXT_AT_MACSTR",%d,%d\r\n", wifi_status.ssid,
                ext_at_mac2str(wifi_status.bssid), wifi_status.channel_num, wifi_status.rssi);
        }
    } else {
        osal_printk("+STASTAT:0,0,0,0,0\r\n");
    }
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

#ifdef CONFIG_WPS_SUPPORT

/*****************************************************************************
* Func description: using wps pbc to connect network
* example: sta wps_pbc <bssid>
*****************************************************************************/
at_ret_t cmd_wpa_wps_pbc(void)
{
    wifi_wps_config wps_config = {0};
    wps_config.wps_method = WIFI_WPS_PBC;
    td_s32 ret = wps_connect(&wps_config);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: get wps pin value
*****************************************************************************/
at_ret_t cmd_wpa_wps_pin_get(void)
{
    td_char pin_txt[WIFI_WPS_PIN_LEN + 1] = {0};
    td_u32  len = WIFI_WPS_PIN_LEN + 1;
    td_s32  ret;

    ret = uapi_wifi_sta_wps_pin_get(pin_txt, len);
    if (ret != EXT_WIFI_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
    pin_txt[WIFI_WPS_PIN_LEN] = '\0';

    osal_printk("+PINSHOW:%s\r\n", pin_txt);
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: using wps pin to connect network
* example: AT+PIN=03882368
*****************************************************************************/
at_ret_t cmd_wpa_wps_pin(const pin_args_t *args)
{
    wifi_wps_config wps_config = {0};
    wps_config.wps_method = WIFI_WPS_PIN;

    if (memcpy_s(wps_config.wps_pin, WIFI_WPS_PIN_LEN + 1, args->pin_code, WIFI_WPS_PIN_LEN) != EOK) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (wps_connect(&wps_config) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    osal_printk("OK\r\n");
    return AT_RET_OK;
}
#endif

at_ret_t cmd_set_reconn(const reconn_args_t *args)
{
    td_s32 enable = args->enable;
    td_s32 seconds = EXT_RECONN_TIMEOUT_MIN;
    td_u32 period = EXT_RECONN_PERIOD_MIN;
    td_u32 max_try_count = EXT_RECONN_COUNT_MIN;
    td_s32 argc = convert_bin_to_dec(args->para_map);
   /* AT+RECONN=<enable>[,<period>,<count>[,<timeout>]] 参数个数只能是1\3\4 */
    if ((argc != 1) && (argc != 3) && (argc != 4)) {
        return AT_RET_MEM_API_ERROR;
    }
    /* 1个参数场景,enable只能是0: AT+RECONN=0 */
    if (argc == 1) {
        if (enable == 1) {
            return AT_RET_MEM_API_ERROR;
        }
    } else {
        /* 多参数场景,enable不能是0 */
        if (enable == 0) {
            return AT_RET_MEM_API_ERROR;
        }
    }
    if (argc > 1) {
        period = args->period;
        max_try_count = args->count;
        /* 4参数场景才需要输入timeout */
        if (argc == 4) {
            seconds = args->timeout;
        }
    }
    td_s32 ret = wifi_sta_set_reconnect_policy(enable, seconds, period, max_try_count);
    if (ret != EXT_WIFI_OK) {
        return AT_RET_MEM_API_ERROR;
    }

    uapi_at_printf("OK\r\n");

    return AT_RET_OK;
}

TD_PRV td_u32 cmd_sta_set_powersave(td_s32 argc, TD_CONST td_char *argv[])
{
    td_u8  ps_switch;
    td_s32 ret;

    if ((argc != 1) || (integer_check(argv[0]) != EXT_ERR_SUCCESS)) {
        return EXT_ERR_FAILURE;
    }

    ps_switch = (td_u8)atoi(argv[0]);
    ret = uapi_wifi_set_pm_switch(ps_switch, 0);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    uapi_at_printf("OK\r\n");
    return EXT_ERR_SUCCESS;
}

at_ret_t cmd_sta_show_config(void)
{
    protocol_mode_enum mode;
    wifi_pmf_option_enum pmf_flag;

    mode = wifi_sta_get_protocol_mode();
    pmf_flag = uapi_wifi_get_pmf();
    osal_printk("+mode:%d\r\n", mode);
    osal_printk("+pmfflag:%d\r\n", pmf_flag);

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_sta_reconnect_for_wfa_test(void)
{
    td_s32 ret;

    ret = uapi_wifi_sta_reconnect_for_wfa_test();
    if (ret != EXT_WIFI_OK) {
        return AT_RET_MEM_API_ERROR;
    }
    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

TD_CONST at_cmd_entry_t g_sta_func_tbl[] = {
    {"PS", 3, 0, TD_NULL, TD_NULL, (at_set_func_t)cmd_sta_set_powersave, TD_NULL, TD_NULL},
};

#define AT_STA_FUNC_NUM (sizeof(at_wifi_cmd_parse_table) / sizeof(at_wifi_cmd_parse_table[0]))

td_void los_at_wifi_cmd_register(td_void)
{
    uapi_at_register_cmd(at_wifi_cmd_parse_table, AT_STA_FUNC_NUM);
}

TD_CONST at_cmd_entry_t g_sta_factory_test_func_tbl[] = {
    {"USERIESET", 10, 0, TD_NULL, TD_NULL, (at_set_func_t)cmd_sta_user_ie_set, TD_NULL, TD_NULL},
};

#define AT_STA_FACTORY_TEST_FUNC_NUM (sizeof(g_sta_factory_test_func_tbl) / sizeof(g_sta_factory_test_func_tbl[0]))

td_void los_at_sta_factory_test_cmd_register(td_void)
{
    uapi_at_register_cmd(g_sta_factory_test_func_tbl, AT_STA_FACTORY_TEST_FUNC_NUM);
}

/*****************************************************************************
* Func description: show mesh or softap connected sta information
*****************************************************************************/
at_ret_t cmd_softap_show_sta(void)
{
    td_s32 ret;
    td_u32 sta_index;
    td_u32 sta_num = WIFI_MAX_NUM_USER;
    wifi_sta_info_stru *result = (wifi_sta_info_stru *)malloc(sizeof(wifi_sta_info_stru) * sta_num);

    if (result == TD_NULL) {
        osal_printk("cmd_softap_show_sta result alloc fail\r\n");
        return AT_RET_MEM_API_ERROR;
    }

    (void)memset_s(result, sizeof(result), 0, sizeof(result));

    ret = wifi_softap_get_sta_list(result, &sta_num);
    if (ret != ERRCODE_SUCC) {
        free(result);
        return AT_RET_SYNTAX_ERROR;
    }

    for (sta_index = 0; sta_index < sta_num; sta_index++) {
        osal_printk("+SHOWSTA:"EXT_AT_MACSTR", rssi:%d, rate:%ukbps\r\n",
            ext_at_mac2str(result[sta_index].mac_addr), result[sta_index].rssi, result[sta_index].best_rate);
        osal_printk("\r\n");
    }
    free(result);
    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

void print_csi_data(unsigned char* csi_data, int csi_data_len)
{
    osal_printk("======csi    data   report======\n");
    if (g_csi_report_count == 50) {    /* 每上报50次打印一次数据 */
        g_csi_report_count = 0;
        osal_printk("==========csi data======start======\n");
        for (int index = 0; index < csi_data_len; index++) {
            osal_printk("%02x ", csi_data[index]);
            if ((index + 1) % 24 == 0) { /* 每行打印24个DWORD */
                osal_printk("\n");
            }
        }
        osal_printk("\n==========csi data======end======\n");
    }
    g_csi_report_count++;
}

at_ret_t cmd_promise_enable(const startpromise_args_t *args)
{
    wifi_ptype_filter_stru filter;
    filter.mdata_en = args->para3;
    filter.udata_en = args->para4;
    filter.mmngt_en = args->para5;
    filter.umngt_en = args->para6;

    wifi_set_promis_mode(args->para1, args->para2, &filter);
    osal_printk("OK\r\n");
    return AT_RET_OK;
}
#ifdef _PRE_WLAN_FEATURE_SDP
at_ret_t cmd_sdp_enable(const startsdp_args_t *args)
{
    wifi_set_sdp_mode(args->para1, args->para2, args->para3);
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_sdp_subscribe(const sdp_subscribe_args_t *args)
{
    wifi_set_sdp_subscribe(args->para1, (td_char *)(args->para2), args->para3);
    osal_printk("OK\r\n");
    return AT_RET_OK;
}
#endif
at_ret_t cmd_csi_set_config(const csisetconfig_args_t *args)
{
    const unsigned char *ifname;
    csi_config_stru config;

    (void)memset_s(&config, sizeof(csi_config_stru), 0, sizeof(csi_config_stru));
    ifname = args->ifname;
    config.user_index = args->user_id;
    config.enable = args->enable;

    if (config.enable == 1) {
        config.match_ta_ra_select = args->cfg_match_ta_ra_sel;
        if (cmd_strtoaddr((td_char *)(args->mac), config.mac_addr, WIFI_MAC_LEN) != EXT_WIFI_OK) {
            osal_printk("cmd_csi_set_config: addr set error.\r\n");
            return AT_RET_SYNTAX_ERROR;
        }
        config.frame_filter_bitmap = args->frame_type_filter_bitmap;
        config.sub_type_filter_enable = args->sub_type_filter_enable;
        config.sub_type_filter = args->sub_type_filter;
        config.ppdu_filter_bitmap = args->ppdu_filter_bitmap;
        config.period = args->period;
    }

    if (wifi_set_csi_config(ifname, &config) != EXT_WIFI_OK) {
        osal_printk("cmd_csi_set_config: CSI set error.\r\n");
        return AT_RET_SYNTAX_ERROR;
    }
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_csi_start(void)
{
    wifi_register_csi_report_cb(print_csi_data);
    wifi_csi_start();
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_csi_stop(void)
{
    wifi_register_csi_report_cb(NULL);
    wifi_csi_stop();
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_autorate_fix_switch(const trc_args_t *args)
{
    td_u8 switch_flag;

    switch_flag = args->para1;
    if ((switch_flag != 0) && (switch_flag != 1)) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (uapi_alg_ar_fix_cfg_etc(!switch_flag) != EXT_WIFI_OK) {
        return EXT_WIFI_FAIL;
    }

    osal_printk("OK\r\n");

    return AT_RET_OK;
}

at_ret_t cmd_autorate_set_rate(const setrate_args_t *args)
{
    alg_param_stru alg_param = {0};

    alg_param.rate_value = args->para1;
    if (wifi_set_fixed_tx_rate(0, &alg_param) != EXT_WIFI_OK) {
        return EXT_WIFI_FAIL;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_get_rate(const getrate_args_t *args)
{
    td_uchar       mac_addr[WIFI_MAC_LEN] = {0};
    td_u8          mac_len;
    td_u32         rate;

    if ((args->para1 == TD_NULL) || (strlen(args->para1) != EXT_WIFI_TXT_ADDR_LEN)) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (cmd_strtoaddr(args->para1, mac_addr, WIFI_MAC_LEN) != EXT_WIFI_OK) {
        return AT_RET_SYNTAX_ERROR;
    }

    mac_len = sizeof(mac_addr);

    if (wifi_get_negotiated_rate(mac_addr, mac_len, &rate) != EXT_WIFI_OK) {
        return EXT_WIFI_FAIL;
    }

    uapi_at_printf("OK, rate:%d\r\n", rate);
    return AT_RET_OK;
}

at_ret_t cmd_tpc_switch(const tpc_args_t *args)
{
    td_u32 tpc_value;

    tpc_value = args->para1;

    if (wifi_set_tpc_mode(tpc_value) != EXT_WIFI_OK) {
        return EXT_WIFI_FAIL;
    }

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_rts_switch(const rts_args_t *args)
{
    td_u8          mode;
    td_u16         pkt_length;

    mode = args->para1;
    pkt_length = args->para2;

    if (wifi_set_rts_mode(mode, pkt_length) != EXT_WIFI_OK) {
        return EXT_WIFI_FAIL;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_cca_switch(const cca_args_t *args)
{
    td_u8          mode;
    td_s8          threshold;

    mode = args->para1;
    threshold = args->para2;

    if (wifi_set_cca_threshold(mode, threshold) != EXT_WIFI_OK) {
        return EXT_WIFI_FAIL;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_intrf_switch(const intrf_args_t *args)
{
    const unsigned char *ifname;
    td_u8                enable;
    td_u16               cfg_flag;

    ifname = args->para1;
    enable = args->para2;
    cfg_flag = args->para3;

    if (wifi_set_intrf_mode(ifname, enable, cfg_flag) != EXT_WIFI_OK) {
        return EXT_WIFI_FAIL;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: start softap
* example: AT+STARTAP="CompanyName",0,6,1,"123456789"
           AT+STARTAP="CompanyName",0,6,0
*****************************************************************************/
at_ret_t cmd_start_softap(const startap_args_t *args)
{
    softap_config_stru hapd_conf          = {0};
    td_s32  len;
    td_s32 argc = convert_bin_to_dec(args->para_map);

    if ((argc < 3) || (argc > 4)) {  /* argc 3/4 */
        return AT_RET_SYNTAX_ERROR;
    }

    /* get ssid */
    if (memcpy_s(hapd_conf.ssid, EXT_AT_SSID_MAX_LEN + 1, args->para1 + 1,
        strlen((td_char *)(args->para1)) - 2) != EOK) { /* para1 len minus 2 (not include double quotes) */
        return AT_RET_SYNTAX_ERROR;
    }

    /* get channel */
    hapd_conf.channel_num = (td_uchar)(args->para2); /* para2 */

	/* get authmode */
    if ((wifi_is_need_psk(args->para3) != 1) && (argc != 3)) {    /* open or owe, only include 3 params */
        return AT_RET_SYNTAX_ERROR;
    }
    hapd_conf.security_type = (wifi_security_enum)(args->para3); /* args 3 */

    /* get password */
    if (wifi_is_need_psk(hapd_conf.security_type) == 1) {
        if (argc != 4) {    /* argc 4 */
            return AT_RET_SYNTAX_ERROR;
        }
        /* check key len in different encrypted */
        len = (int)strlen((td_char *)(args->para4));
        if (at_check_security_key_len(hapd_conf.security_type, len) != AT_RET_OK) {
            return AT_RET_SYNTAX_ERROR;
        }
        TD_CONST td_char *buf = (td_char *)(args->para4);   /* args 4 */
        if ((*buf != '\"') || (*(buf + len - 1) != '\"') ||
            /* len minus 2 */
            (memcpy_s((td_char*)hapd_conf.pre_shared_key, EXT_WIFI_AP_KEY_LEN + 1, buf + 1, len - 2) != EOK)) {
            return AT_RET_SYNTAX_ERROR;
        }
    }

    osal_printk("at_start_softap ssid[%s]\r\n", hapd_conf.ssid);

    if (at_start_softap(&hapd_conf) != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (uapi_wifi_set_bandwidth(EXT_DEFAULT_IFNAME_AP, strlen(EXT_DEFAULT_IFNAME_AP) + 1, g_bw_setup_value) != EXT_ERR_SUCCESS) {
        wifi_softap_disable();
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_report("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: start softap
* example: AT+SETAPADV=2,10,100,2,3600,0
*****************************************************************************/
at_ret_t cmd_set_softap_advance(const setapadv_args_t *args)
{
    softap_config_advance_stru config = {0};

    if (at_set_softap_advance(&config, args) != EXT_ERR_SUCCESS) {
        return AT_RET_MEM_API_ERROR;
    }

    if (wifi_set_softap_config_advance(&config) != ERRCODE_SUCC) {
        return AT_RET_MEM_API_ERROR;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: show softap configuration
*****************************************************************************/
at_ret_t cmd_softap_show_config(void)
{
    td_s32 ret;
    softap_config_stru base_config = {0};
    softap_config_advance_stru advanceConfig = {0};

    ret = wifi_get_softap_config(&base_config);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_MEM_API_ERROR;
    }
    ret = wifi_get_softap_config_advance(&advanceConfig);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_MEM_API_ERROR;
    }
    osal_printk("+ssid:%s\r\n", base_config.ssid);
    osal_printk("+security_type:%d\r\n", base_config.security_type);
    osal_printk("+channel:%d\r\n", base_config.channel_num);
    osal_printk("+beacon_interval:%d\r\n", advanceConfig.beacon_interval);
    osal_printk("+dtim_period:%d\r\n", advanceConfig.dtim_period);
    osal_printk("+group_rekey:%d\r\n", advanceConfig.group_rekey);
    osal_printk("+hidden_ssid_flag:%d\r\n", advanceConfig.hidden_ssid_flag);
    osal_printk("+gi:%d\r\n", advanceConfig.gi);
    osal_printk("+protocol_mode:%d\r\n", advanceConfig.protocol_mode);
    osal_printk("\r\n");

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: stop softap
*****************************************************************************/
at_ret_t cmd_stop_softap(void)
{
    uapi_at_report("cmd_stop_softap \r\n");

    if (wifi_softap_disable() != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_report("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
* Func description: softap disconnect station
* example: AT+DEAUTHSTA=90:2B:D2:E4:CE:28
*****************************************************************************/
at_ret_t cmd_softap_deauth_sta(const deauthsta_args_t *args)
{
    td_uchar mac_addr[WIFI_MAC_LEN + 1] = {0};
    td_s32 ret;

    if (cmd_strtoaddr((td_char *)args->para1, mac_addr, WIFI_MAC_LEN) != EXT_WIFI_OK) {
        return AT_RET_SYNTAX_ERROR;
    }

    ret = wifi_softap_deauth_sta(mac_addr, WIFI_MAC_LEN);
    if (ret != EXT_WIFI_OK) {
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_softap_scan(void)
{
    td_s32 ret;

    ret = wifi_ap_scan();
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    uapi_at_printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_set_sta_pm(const ps_args_t *args)
{
    td_s32 ret;

    ret = wifi_sta_set_pm((td_u8)args->ps_switch);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_wifi_init(void)
{
    td_s32 ret;

    ret = wifi_init();
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_wifi_uninit(void)
{
    td_s32 ret;

    ret = wifi_deinit();
    if (ret != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t cmd_wifi_is_init(void)
{
    td_s32 ret = wifi_is_wifi_inited();

    osal_printk("OK wifi_is_init:%d \r\n", ret);
    return AT_RET_OK;
}

/* AT+PSD=enable,cycle,duration PSD Control */
at_ret_t cmd_control_psd(const psd_args_t *args)
{
    ext_psd_option_param psd_option = {0};
    psd_option.enable = args->enable;
    psd_option.duration = args->duration;
    psd_option.cycle = args->cycle;
    wifi_set_psd_mode(&psd_option);
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

#ifdef LOSCFG_APP_MESH
td_u32 cmd_mesh_auto_get_ssid(TD_CONST td_char *argv[], ext_mesh_autolink_config *mesh_auto_config)
{
    td_u32 ssid_len = 0;
    errno_t rc;

    /* get ssid */
    if (argv[1][0] == 'P') {
        if (strlen(argv[1]) > EXT_AT_SSID_MAX_LEN * 4 + 3) { /* ssid length should less than 32*4+3 */
            return EXT_ERR_FAILURE;
        }
    } else if (strlen(argv[1]) > EXT_AT_SSID_MAX_LEN + 2) { /* ssid length should less than 32+2 */
        return EXT_ERR_FAILURE;
    }

    /* ssid hex to string */
    td_char *tmp = at_parse_string(argv[1], &ssid_len);
    if (tmp == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    if ((ssid_len > EXT_AT_SSID_MAX_LEN) || (ssid_len == 0)) {
        osal_kfree(tmp);
        return EXT_ERR_FAILURE;
    }

    rc = memcpy_s(mesh_auto_config->ssid, EXT_AT_SSID_MAX_LEN + 1, tmp, strlen(tmp) + 1);
    osal_kfree(tmp);
    if (rc != EOK) {
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

/*****************************************************************************
* Func description: start mesh auto join
* example: AT+MAUTOJOIN=1,"router_mesh",0
AT+MAUTOJOIN=1,"router_mesh",1,"123456789"
*****************************************************************************/
td_s32 cmd_mesh_auto_connect(td_s32 argc, TD_CONST td_char* argv[])
{
    ext_mesh_autolink_config mesh_auto_config = {0};

    if ((argc < 3) || (argc > 4)) { /* argc 3/4 */
        return EXT_ERR_FAILURE;
    }

    /* get usr config mesh type */
    if ((integer_check(argv[0]) != EXT_ERR_SUCCESS) || (atoi(argv[0]) < EXT_MESH_USRCONFIG_MBR) ||
        (atoi(argv[0]) > EXT_MESH_USRCONFIG_AUTO)) {
        return EXT_ERR_FAILURE;
    }
    mesh_auto_config.usr_cfg_role = (ext_mesh_autolink_usrcfg_node_type)atoi(argv[0]);

    /* get ssid */
    if (cmd_mesh_auto_get_ssid(argv, &mesh_auto_config) != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    /* get auth_type */
    if ((integer_check(argv[2]) == EXT_ERR_FAILURE) ||   /* argc 2 */
        ((atoi(argv[2]) != EXT_MESH_OPEN) && (atoi(argv[2]) != EXT_MESH_AUTH)) || /* argc 2 */
        ((atoi(argv[2]) == EXT_MESH_OPEN) && (argc != 3))) { /* argc 2/3 */
        return EXT_ERR_FAILURE;
    }
    mesh_auto_config.auth = (ext_mesh_autolink_auth_type)atoi(argv[2]); /* 2 */

    /* get key */
    if (mesh_auto_config.auth == EXT_MESH_AUTH) {
        if ((argc != 4) || (strlen(argv[3]) > EXT_WIFI_MS_KEY_LEN_MAX + 2) || /* 4,3,2 */
            (strlen(argv[3]) < EXT_WIFI_MS_KEY_LEN_MIN + 2)) { /* 3,2 */
            return EXT_ERR_FAILURE;
        }
        TD_CONST td_char *buf = argv[3]; /* 3 */
        td_u32 len = strlen(argv[3]); /* 3 */
        if ((*buf != '\"') || (*(buf + len - 1) != '\"') ||
            (memcpy_s((td_char*)mesh_auto_config.key,
                      EXT_WIFI_MESH_KEY_LEN_MAX + 1, buf + 1, len - 2) != EOK)) { /* 2 */
            return EXT_ERR_FAILURE;
        }
    }

    if (uapi_mesh_start_autolink(&mesh_auto_config) != EXT_WIFI_OK) {
        return EXT_ERR_FAILURE;
    }

    uapi_at_printf("OK\r\n");
    return EXT_ERR_SUCCESS;
}

/*****************************************************************************
* Func description: exit mesh network
* example: AT+MEXIT
* AT+MEXIT
*****************************************************************************/
td_s32 cmd_mesh_exit_network(td_s32 argc, TD_CONST td_char* argv[])
{
    td_s32  ret;

    los_unref_param(argv);
    los_unref_param(argc);

    ret = uapi_mesh_exit_autolink();
    if (ret != EXT_WIFI_OK) {
        return EXT_ERR_FAILURE;
    }

    uapi_at_printf("OK\r\n");
    return EXT_ERR_SUCCESS;
}

/*****************************************************************************
* Func description: set mesh router rssi threshold
* example: AT+MSETRSSI=-50
*****************************************************************************/
td_s32 cmd_mesh_set_rssi_threshold(td_s32 argc, TD_CONST td_char* argv[])
{
    td_s32 ret;
    td_s32 usr_rssi_config;

    if ((argc != 1) || (argv[0] == TD_NULL)) {
        return EXT_ERR_FAILURE;
    }

    usr_rssi_config = atoi(argv[0]);
    if ((usr_rssi_config < -127) || (usr_rssi_config > 10)) {   /* range -127,10 */
        return EXT_ERR_FAILURE;
    }
    ret = uapi_mesh_autolink_set_router_rssi_threshold(usr_rssi_config);
    if (ret != EXT_WIFI_OK) {
        return EXT_ERR_FAILURE;
    }

    uapi_at_printf("OK\r\n");
    return EXT_ERR_SUCCESS;
}

/*****************************************************************************
* Func description: set mesh autonetwork bw value
* example: AT+MSETBW=5
*****************************************************************************/
td_s32 cmd_mesh_set_autonetwork_bw(td_s32 argc, TD_CONST td_char* argv[])
{
    ext_wifi_bw bw_value;
    if (argc != 1) { /* "+MSETBW"命令固定1个命令参数 */
        return EXT_ERR_FAILURE;
    }

    if ((argv[0] != TD_NULL) && (integer_check(argv[0]) != EXT_ERR_SUCCESS)) {
        return EXT_ERR_FAILURE;
    }

    if ((argv[0] != TD_NULL) && (atoi(argv[0]) == 5)) { /* bw 5 */
        bw_value = EXT_WIFI_BW_HIEX_5M;
    } else if ((argv[0] != TD_NULL) && (atoi(argv[0]) == 10)) { /* bw 10 */
        bw_value = EXT_WIFI_BW_HIEX_10M;
    } else {
        bw_value = EXT_WIFI_BW_LEGACY_20M;
    }

    uapi_mesh_autolink_set_bw(bw_value);
    uapi_at_printf("OK\r\n");
    return EXT_ERR_SUCCESS;
}

TD_CONST at_cmd_entry_t g_at_mesh_func_tbl[] = {
    {"MAUTOJOIN", 10, 0, TD_NULL, TD_NULL, (at_set_func_t)cmd_mesh_auto_connect, TD_NULL, TD_NULL},
    {"MEXIT", 6, 0, TD_NULL, (at_cmd_func_t)cmd_mesh_exit_network, TD_NULL, TD_NULL, TD_NULL},
    {"MSETRSSI", 9, 0, TD_NULL, TD_NULL, (at_set_func_t)cmd_mesh_set_rssi_threshold, TD_NULL, TD_NULL},
    {"MSETBW", 7, 0, TD_NULL, TD_NULL, (at_set_func_t)cmd_mesh_set_autonetwork_bw, TD_NULL, TD_NULL},
};

#define AT_MESH_FUNC_NUM (sizeof(g_at_mesh_func_tbl) / sizeof(g_at_mesh_func_tbl[0]))

td_void los_at_mesh_cmd_register(td_void)
{
    uapi_at_register_cmd(g_at_mesh_func_tbl, AT_MESH_FUNC_NUM);
}
#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
