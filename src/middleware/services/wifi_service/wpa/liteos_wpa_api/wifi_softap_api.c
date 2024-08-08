/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: wifi softap APIs
 */

#include "utils/common.h"
#include "wifi_api.h"
#include "utils/eloop.h"
#include "wpa_supplicant_i.h"
#include "wpa_cli_rtos.h"
#include "common/ieee802_11_common.h"
#include "config_ssid.h"
#include "driver_soc.h"
#include "securec.h"

static int g_softap_pairwise = 0;

static int is_hex_char(char ch)
{
	if (((ch >= '0') && (ch <= '9')) ||
		((ch >= 'A') && (ch <= 'F')) ||
		((ch >= 'a') && (ch <= 'f'))) {
		return EXT_WIFI_OK;
	}
	return EXT_WIFI_FAIL;
}


int is_hex_string(const char *data, size_t len)
{
	size_t i;
	if ((data == NULL) || (len == 0))
		return EXT_WIFI_FAIL;
	for (i = 0; i < len; i++) {
		if (is_hex_char(data[i]) == EXT_WIFI_FAIL)
			return EXT_WIFI_FAIL;
	}
	return EXT_WIFI_OK;
}


static int softap_start_precheck(void)
{
	int i;
	unsigned int int_save;
	unsigned int ret = (unsigned int)(los_count_wifi_dev_in_use() >= WPA_DOUBLE_IFACE_WIFI_DEV_NUM);

	os_intlock(&int_save);
	for (i = 0; i < WPA_MAX_WIFI_DEV_NUM; i++) {
		if (g_wifi_dev[i] != NULL)
			ret |= (unsigned int)(g_wifi_dev[i]->iftype > EXT_WIFI_IFTYPE_STATION);
	}
	ret |= (unsigned int)(g_mesh_flag == WPA_FLAG_ON);
	os_intrestore(int_save);
	return ret ? EXT_WIFI_FAIL : EXT_WIFI_OK;
}

static int wifi_softap_check_wep_key(wifi_softap_config *hconf)
{
    /* check wep key len and ssid len */
    if ((os_strlen((char *)hconf->key) != WPA_WEP40_KEY_LEN) &&
        (os_strlen((char *)hconf->key) != WPA_WEP104_KEY_LEN) &&
        (os_strlen((char *)hconf->key) != WPA_WEP40_HEX_KEY_LEN) &&
        (os_strlen((char *)hconf->key) != WPA_WEP104_HEX_KEY_LEN)) {
        wpa_error_log0(MSG_ERROR, "Invalid wep ssid or wep key length! \n");
        return EXT_WIFI_FAIL;
    }

    /* check wep hex key */
    if (((os_strlen((char *)hconf->key) == WPA_WEP40_HEX_KEY_LEN) &&
        (is_hex_string((char *)hconf->key, WPA_WEP40_HEX_KEY_LEN) == EXT_WIFI_FAIL))) {
        wpa_error_log0(MSG_ERROR, "Invalid wep40 HEX passphrase character");
        return EXT_WIFI_FAIL;
    } else if (((os_strlen((char *)hconf->key) == WPA_WEP104_HEX_KEY_LEN) &&
        (is_hex_string((char *)hconf->key, WPA_WEP104_HEX_KEY_LEN) == EXT_WIFI_FAIL))) {
        wpa_error_log0(MSG_ERROR, "Invalid wep104 HEX passphrase character");
        return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

static int wifi_softap_check_key_config(wifi_softap_config *hconf)
{
    /* check ssid length */
    if (os_strlen((char *)hconf->ssid) > WPA_MAX_ESSID_LEN) {
        wpa_error_log0(MSG_ERROR, "Invalid ssid length! \n");
        return EXT_WIFI_FAIL;
    }

    /* check WEP key length */
    if (hconf->authmode == EXT_WIFI_SECURITY_WEP || hconf->authmode == EXT_WIFI_SECURITY_WEP_OPEN) {
        if (wifi_softap_check_wep_key(hconf) == EXT_WIFI_FAIL) {
            wpa_error_log0(MSG_ERROR, "Invalid wep passphrase character");
            return EXT_WIFI_FAIL;
        }
    } else if ((wifi_is_need_psk(hconf->authmode) == 1) && ((os_strlen((char *)hconf->key) < WPA_MIN_KEY_LEN) ||
        (os_strlen((char *)hconf->key) > WPA_MAX_KEY_LEN))) {
        /* check other authmode key length expect OPEN and OWE */
        wpa_error_log0(MSG_ERROR, "Invalid key length! \n");
        return EXT_WIFI_FAIL;
    }

    /* check HEX key */
    if ((os_strlen((char *)hconf->key) == WPA_MAX_KEY_LEN) &&
        (is_hex_string((char *)hconf->key, WPA_MAX_KEY_LEN) == EXT_WIFI_FAIL)) {
        wpa_error_log0(MSG_ERROR, "Invalid passphrase character");
        return EXT_WIFI_FAIL;
    }

    return EXT_WIFI_OK;
}

static int wifi_softap_set_security(wifi_softap_config *hconf)
{
    switch (hconf->authmode) {
        case EXT_WIFI_SECURITY_OPEN:
            hconf->auth_algs = 0;
            hconf->wpa_key_mgmt = (int)WPA_KEY_MGMT_NONE;
            break;
#ifdef CONFIG_OWE
        case EXT_WIFI_SECURITY_OWE:
            hconf->wpa_key_mgmt = (int)WPA_KEY_MGMT_OWE;
            hconf->wpa = 2; /* 2: RSN */
            hconf->wpa_pairwise = (int)WPA_CIPHER_CCMP;
            break;
#endif /* CONFIG_OWE  */
        case EXT_WIFI_SECURITY_WEP:
            hconf->auth_algs = 2;   /* 2表示WEP SHARED加密 */
            hconf->wep_idx = 0; /* 配置第一个密钥 */
            break;
        case EXT_WIFI_SECURITY_WEP_OPEN:
            hconf->auth_algs = 1;   /* 1表示WEP OPEN加密 */
            hconf->wep_idx = 0; /* 配置第一个密钥 */
            break;
        case EXT_WIFI_SECURITY_WPA2PSK:
            hconf->wpa_key_mgmt = (int)WPA_KEY_MGMT_PSK;
            hconf->wpa = 2; /* 2: WPA2-PSK */
            if (hconf->wpa_pairwise == 0)
                hconf->wpa_pairwise = (int)WPA_CIPHER_CCMP;
            break;
        case EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX:
            hconf->wpa_key_mgmt = (int)WPA_KEY_MGMT_PSK;
            hconf->wpa = 3; /* 3: WPA2-PSK|WPA-PSK */
            if (hconf->wpa_pairwise == 0)
                hconf->wpa_pairwise = (int)WPA_CIPHER_CCMP;
            break;
#ifdef CONFIG_HOSTAPD_WPA3
        case EXT_WIFI_SECURITY_SAE:
            hconf->wpa_key_mgmt = (int)WPA_KEY_MGMT_SAE;
            hconf->wpa = 2; /* 2: RSN */
            hconf->wpa_pairwise = (int)WPA_CIPHER_CCMP;
            break;
        case EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX:
            hconf->wpa_key_mgmt = (int)(WPA_KEY_MGMT_SAE | WPA_KEY_MGMT_PSK);
            hconf->wpa = 2; /* 2: RSN */
            hconf->wpa_pairwise = (int)WPA_CIPHER_CCMP;
            break;
#endif /* CONFIG_HOSTAPD_WPA3  */
        default:
            wpa_error_log1(MSG_ERROR, "error, Unsupported authmode: %d \n", (int)hconf->authmode);
            hconf->auth_algs = 0;
            hconf->wpa_key_mgmt = (int)WPA_KEY_MGMT_NONE;
            return EXT_WIFI_FAIL;
    }
    return EXT_WIFI_OK;
}

#if defined(CONFIG_HOSTAPD_WPA3) || defined(CONFIG_OWE)
static void wifi_pmf_set(wifi_softap_config *hconf)
{
    if ((hconf->authmode == EXT_WIFI_SECURITY_SAE) || (hconf->authmode == EXT_WIFI_SECURITY_OWE))
        g_ap_opt_set.ieee80211w = MGMT_FRAME_PROTECTION_REQUIRED;
    else if (hconf->authmode == EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX)
        g_ap_opt_set.ieee80211w = MGMT_FRAME_PROTECTION_OPTIONAL;
}
#endif /* defined(CONFIG_HOSTAPD_WPA3) || defined(CONFIG_OWE) */

#ifdef CONFIG_HOSTAPD_WPA3
static void wifi_sae_pwe_set(wifi_softap_config *hconf)
{
    if (hconf->authmode == EXT_WIFI_SECURITY_SAE) {
		if (g_ap_opt_set.sae_pwe == WIFI_SAE_PWE_UNSPECIFIED) {
			g_ap_opt_set.sae_pwe = WIFI_SAE_PWE_BOTH;
		}
	} else {
		if (g_ap_opt_set.sae_pwe == WIFI_SAE_PWE_UNSPECIFIED) {
			g_ap_opt_set.sae_pwe = WIFI_SAE_PWE_HUNT_AND_PECK;
		}
	}
}

#endif /* CONFIG_HOSTAPD_WPA3 */

static int wifi_softap_set_config(wifi_softap_config *hconf)
{
	char *res = NULL;
	char tmp_ssid[EXT_WIFI_MAX_SSID_LEN + 1 + 2] = { 0 }; /* 1: string terminator; 2: two Double quotes */
	size_t len  = 0;
	int ret;
	errno_t rc;

	if (hconf == NULL) {
		wpa_error_log0(MSG_ERROR, " ---> ### los_softap_set_config error: the hconf is NULL . \n\r");
		return EXT_WIFI_FAIL;
	}
	if (wifi_channel_check((unsigned char)hconf->channel_num) == EXT_WIFI_FAIL) {
		wpa_error_log0(MSG_ERROR, "Invalid channel_num! \n");
		return EXT_WIFI_FAIL;
	}

	ret = snprintf_s(tmp_ssid, sizeof(tmp_ssid), sizeof(tmp_ssid) - 1, "\"%s\"", hconf->ssid);
	if (ret < 0)
		return EXT_WIFI_FAIL;
	rc = memset_s(hconf->ssid, sizeof(hconf->ssid), 0, sizeof(hconf->ssid));
	if (rc != EOK) {
		wpa_error_log0(MSG_ERROR, "los_softap_set_config: memset_s failed");
		return EXT_WIFI_FAIL;
	}

	res = wpa_config_parse_string(tmp_ssid, &len);
	if ((res != NULL) && (len <= SSID_MAX_LEN)) {
		rc = memcpy_s(hconf->ssid, (size_t)sizeof(hconf->ssid), res, len);
		if (rc != EOK) {
			wpa_error_log0(MSG_ERROR, "los_softap_set_config: memcpy_s failed");
			ret = EXT_WIFI_FAIL;
			goto AP_CONFIG_PRE_CHECK_OUT;
		}
	} else {
		wpa_error_log0(MSG_ERROR, "los_softap_config_pre_check: wpa_config_parse_string failed");
		ret = EXT_WIFI_FAIL;
		goto AP_CONFIG_PRE_CHECK_OUT;
	}
#if defined(CONFIG_HOSTAPD_WPA3) || defined(CONFIG_OWE)
	wifi_pmf_set(hconf);
#endif /* defined(CONFIG_HOSTAPD_WPA3) || defined(CONFIG_OWE) */
#ifdef CONFIG_HOSTAPD_WPA3
#ifndef CONFIG_SAE_NO_PW_ID
	wifi_sae_pwe_set(hconf);
#endif /* CONFIG_SAE_NO_PW_ID */
#endif /* CONFIG_HOSTAPD_WPA3 */
    if ((wifi_softap_check_key_config(hconf) != EXT_WIFI_OK) || (wifi_softap_set_security(hconf) != EXT_WIFI_OK)) {
        ret = EXT_WIFI_FAIL;
        goto AP_CONFIG_PRE_CHECK_OUT;
    }
    ret = EXT_WIFI_OK;

AP_CONFIG_PRE_CHECK_OUT:
	if (res != NULL) {
		os_free(res);
	}
	return ret;
}

static int wifi_softap_start(wifi_softap_config *hconf, char *ifname, int *len)
{
	struct ext_wifi_dev *wifi_dev = NULL;
	errno_t rc;

	if (softap_start_precheck() != EXT_WIFI_OK) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_start: precheck fail!");
		return EXT_WIFI_FAIL;
	}

	if (wifi_softap_set_config(hconf) != EXT_WIFI_OK) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_start: set_config fail!");
		return EXT_WIFI_FAIL;
	}

    /* Softap协议模式默认使用11ax */
    if (g_ap_opt_set.hw_mode == WIFI_MODE_UNDEFINE) {
        g_ap_opt_set.hw_mode = WIFI_MODE_11B_G_N_AX;
    }

	(void)memcpy_s(&g_global_conf, sizeof(struct hostapd_conf), hconf, sizeof(struct hostapd_conf));
	wifi_dev = wifi_dev_creat(EXT_WIFI_IFTYPE_AP, g_ap_opt_set.hw_mode);
	if (wifi_dev == NULL) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_start: wifi_dev_creat failed.");
		return EXT_WIFI_FAIL;
	}
	if ((los_set_wifi_dev(wifi_dev) != EXT_WIFI_OK) || (*len < wifi_dev->ifname_len + 1))
		goto WIFI_AP_ERROR;
	rc = memcpy_s(ifname, (size_t)(*len), wifi_dev->ifname, (size_t)wifi_dev->ifname_len);
	if (rc != EOK) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_start: memcpy_s failed");
		goto WIFI_AP_ERROR;
	}
	ifname[wifi_dev->ifname_len] = '\0';
	*len = wifi_dev->ifname_len;
	if (wifi_wpa_start(wifi_dev) == EXT_WIFI_OK) {
		return EXT_WIFI_OK;
	}
WIFI_AP_ERROR:
	(void)wal_deinit_drv_wlan_netdev(wifi_dev->ifname);
	los_free_wifi_dev(wifi_dev);
	return EXT_WIFI_FAIL;
}

static int wifi_softap_config_check(const ext_wifi_softap_config *conf, const char *ifname, const int *len)
{
	if ((conf == NULL) || (ifname == NULL) || (len == NULL)) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_config_check: invalid param");
		return EXT_WIFI_FAIL;
	}

	if (strnlen(conf->ssid, EXT_WIFI_MAX_SSID_LEN + 1) > EXT_WIFI_MAX_SSID_LEN) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_config_check: invalid ssid length.");
		return EXT_WIFI_FAIL;
	}

	if (conf->channel_num > WPA_24G_CHANNEL_NUMS) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_config_check: invalid channel number!.");
		return EXT_WIFI_FAIL;
	}

	if ((conf->ssid_hidden < 0) || (conf->ssid_hidden > 1)) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_config_check: invalid ignore_broadcast_ssid.");
		return EXT_WIFI_FAIL;
	}

	if (strnlen(conf->key, EXT_WIFI_AP_KEY_LEN + 1) > EXT_WIFI_AP_KEY_LEN) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_config_check: invalid key.");
		return EXT_WIFI_FAIL;
	}
#ifdef CONFIG_HOSTAPD_WPA3
	if ((conf->authmode >= EXT_WIFI_SECURITY_UNKNOWN) || (conf->authmode == EXT_WIFI_SECURITY_WPA) ||
		(conf->authmode == EXT_WIFI_SECURITY_WPA2)) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_config_check Invalid authmode.");
		return EXT_WIFI_FAIL;
	}
	/* The length of the password is 8 ~ 63 in wpa3 or wpa2/wpa3 encryption */
	if ((conf->authmode == EXT_WIFI_SECURITY_SAE) ||
	    (conf->authmode == EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX)) {
		if ((strlen(conf->key) < WPA_MIN_KEY_LEN) || (strlen(conf->key) > WPA_MAX_KEY_LEN - 1)) {
			wpa_error_log0(MSG_ERROR, "wifi_softap_config_check Invalid key len.");
			return EXT_WIFI_FAIL;
		}
		if ((conf->pairwise != EXT_WIFI_PAIRWISE_AES) && (conf->pairwise != EXT_WIFI_PARIWISE_UNKNOWN)) {
			wpa_error_log0(MSG_ERROR, "wifi_softap_config_check Invalid pairwise.");
			return EXT_WIFI_FAIL;
		}
	}
#else
	if (conf->authmode > EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX) {
		wpa_error_log0(MSG_ERROR, "wifi_softap_config_check Invalid authmode.");
		return EXT_WIFI_FAIL;
	}
#endif /* CONFIG_HOSTAPD_WPA3 */
	return EXT_WIFI_OK;
}

int uapi_wifi_softap_set_pairwise(int pairwise)
{
    if (pairwise > WPA_CIPHER_GTK_NOT_USED) {
        wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_set_pairwise: invalid pairwise options.");
        return EXT_WIFI_FAIL;
    }

    g_softap_pairwise = pairwise;
    return EXT_WIFI_OK;
}

int uapi_wifi_softap_start(ext_wifi_softap_config *conf, char *ifname, int *len)
{
	struct hostapd_conf hapd_conf = { 0 };
	errno_t rc;

	if (wifi_softap_config_check(conf, ifname, len) == EXT_WIFI_FAIL)
		return EXT_WIFI_FAIL;

	if (try_set_lock_flag() == EXT_WIFI_FAIL) {
		wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_start: wifi dev start or stop is running.");
		return EXT_WIFI_FAIL;
	}

	rc = strcpy_s(hapd_conf.driver, MAX_DRIVER_NAME_LEN, "soc");
	if (rc != EOK)
		goto WIFI_AP_FAIL;

	hapd_conf.channel_num = conf->channel_num;
	rc = memcpy_s(hapd_conf.ssid, (size_t)sizeof(hapd_conf.ssid), conf->ssid, (size_t)(strlen(conf->ssid) + 1));
	if (rc != EOK)
		goto WIFI_AP_FAIL;
	hapd_conf.ignore_broadcast_ssid = conf->ssid_hidden;
	hapd_conf.authmode = conf->authmode;

    if (wifi_is_need_psk(conf->authmode) == 1) {
		rc = memcpy_s((char *)hapd_conf.key, (size_t)sizeof(hapd_conf.key), conf->key, (size_t)(strlen(conf->key) + 1));
		if (rc != EOK)
			goto WIFI_AP_FAIL;
	}

	if (g_softap_pairwise != 0) {
        hapd_conf.wpa_pairwise = (int)g_softap_pairwise;
    } else if ((hapd_conf.authmode == EXT_WIFI_SECURITY_WPA2PSK) ||
	    (hapd_conf.authmode == EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX)) { // pairwise
		if (conf->pairwise == EXT_WIFI_PAIRWISE_TKIP)
			hapd_conf.wpa_pairwise = (int)WPA_CIPHER_TKIP;
		else if ((conf->pairwise == EXT_WIFI_PAIRWISE_AES) || (conf->pairwise == EXT_WIFI_PARIWISE_UNKNOWN))
			hapd_conf.wpa_pairwise = (int)WPA_CIPHER_CCMP;
		else if (conf->pairwise == EXT_WIFI_PAIRWISE_TKIP_AES_MIX)
			hapd_conf.wpa_pairwise = (int)(WPA_CIPHER_TKIP | WPA_CIPHER_CCMP);
		else if (conf->pairwise == EXT_WIFI_PAIRWISE_CCMP256)
			hapd_conf.wpa_pairwise = (int)(WPA_CIPHER_CCMP_256);
		else {
			wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_start: invalid encryption.");
			goto WIFI_AP_FAIL;
		}
	}
	if (wifi_softap_start(&hapd_conf, ifname, len) != EXT_WIFI_OK) {
		wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_start: softap start failed.");
		goto WIFI_AP_FAIL;
	}
	(void)memset_s(&hapd_conf, sizeof(hapd_conf), 0, sizeof(hapd_conf));
	clr_lock_flag();
	return EXT_WIFI_OK;
WIFI_AP_FAIL:
	(void)memset_s(&hapd_conf, sizeof(hapd_conf), 0, sizeof(hapd_conf));
	clr_lock_flag();
	return EXT_WIFI_FAIL;
}

int uapi_wifi_softap_stop(void)
{
	errno_t rc;
	int ret;
	unsigned int ret_val;
	struct ext_wifi_dev *wifi_dev  = NULL;
	char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};

	if (try_set_lock_flag() == EXT_WIFI_FAIL) {
		wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_start: wifi dev start or stop is running.");
		return EXT_WIFI_FAIL;
	}

	wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_AP);
	if (wifi_dev == NULL) {
		wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_stop: get wifi dev failed\n");
		goto WIFI_AP_STOP_EXIT;
	}
	g_ap_sta_info = NULL;
	g_softap_pairwise = 0;
	(void)memset_s(&g_global_conf, sizeof(g_global_conf), 0, sizeof(g_global_conf));
	rc = memset_s(&g_ap_opt_set, sizeof(struct wifi_ap_opt_set), 0, sizeof(struct wifi_ap_opt_set));
	if (rc != EOK)
		goto WIFI_AP_STOP_EXIT;
	rc = memcpy_s(ifname, (size_t)WIFI_IFNAME_MAX_SIZE, wifi_dev->ifname, (size_t)wifi_dev->ifname_len);
	if (rc != EOK)
		goto WIFI_AP_STOP_EXIT;
	(void)os_event_clear(g_softap_event, ~WPA_EVENT_AP_STOP_OK);
	wpa_error_log0(MSG_ERROR, "os_event_clear WPA_EVENT_AP_STOP_OK");

	if (wpa_cli_terminate(NULL, ELOOP_TASK_HOSTAPD) != EXT_WIFI_OK) {
		wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_stop: wpa_cli_terminate failed");
		goto WIFI_AP_STOP_EXIT;
	}
	(void)os_event_read(g_softap_event, WPA_EVENT_AP_STOP_OK, &ret_val,
	                    WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_WAIT_FOREVER);
	if (ret_val == WPA_EVENT_AP_STOP_OK)
		wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_AP_STOP_OK success");
	else
		wpa_error_log1(MSG_ERROR, "os_event_read WPA_EVENT_AP_STOP_OK failed ret_val = %x", ret_val);

	ret = wal_deinit_drv_wlan_netdev(ifname);
	if (ret != EXT_SUCC)
		wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_stop: wal_deinit_drv_wlan_netdev failed!");

	clr_lock_flag();
	return ret;
WIFI_AP_STOP_EXIT:
	clr_lock_flag();
	return EXT_WIFI_FAIL;
}

int uapi_wifi_softap_deauth_sta(const unsigned char *addr, unsigned char addr_len)
{
	struct hostapd_data *hapd      = NULL;
	struct ext_wifi_dev *wifi_dev = NULL;
	char addr_txt[EXT_WIFI_TXT_ADDR_LEN + 1] = { 0 };
	int ret;
	unsigned int ret_val;

	if ((addr_len != ETH_ALEN) || (addr == NULL)) {
		wpa_error_log0(MSG_ERROR, "Invalid addr.");
		return EXT_WIFI_FAIL;
	}

	wifi_dev = wifi_dev_get(EXT_WIFI_IFTYPE_AP);
	if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
		wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_deauth_sta: get wifi dev failed.");
		return EXT_WIFI_FAIL;
	}

	hapd = wifi_dev->priv;
	if (hapd == NULL) {
		wpa_error_log0(MSG_ERROR, "hapd get fail.");
		return EXT_WIFI_FAIL;
	}
	ret = snprintf_s(addr_txt, sizeof(addr_txt), sizeof(addr_txt) - 1, MACSTR, MAC2STR(addr));
	if (ret < 0) {
		wpa_error_log0(MSG_ERROR, "uapi_wifi_mesh_connect snprintf_s faild");
		return EXT_WIFI_FAIL;
	}
	(void)os_event_clear(g_softap_event, ~WPA_EVENT_AP_DEAUTH_FLAG);
	if (wpa_cli_ap_deauth((struct wpa_supplicant *)hapd, addr_txt, sizeof(addr_txt)) != EXT_WIFI_OK) {
		wpa_error_log0(MSG_ERROR, "wpa_cli_ap_deauth faild");
		return EXT_WIFI_FAIL;
	}

	(void)os_event_read(g_softap_event, WPA_EVENT_AP_DEAUTH_FLAG, &ret_val,
	                    WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_EVENT_DELAY_5S);
	if (ret_val != WPA_EVENT_AP_DEAUTH_OK) {
		wpa_error_log1(MSG_ERROR, "os_event_read WPA_EVENT_AP_DEAUTH_FLAG failed ret_val = %x", ret_val);
		return EXT_WIFI_FAIL;
	}
	return EXT_WIFI_OK;
}

static int wifi_softap_set_cond_check(void)
{
	if (is_lock_flag_off() == EXT_WIFI_FAIL) {
		wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
		return EXT_WIFI_FAIL;
	}
#ifdef LOS_CONFIG_P2P
	if (is_ap_mesh_or_p2p_on() == EXT_WIFI_OK) {
		wpa_warning_log0(MSG_INFO, "ap or mesh is already in progress, set ap config failed.");
		return EXT_WIFI_FAIL;
	}
#endif
	return EXT_WIFI_OK;
}

int uapi_wifi_softap_set_protocol_mode(protocol_mode_enum hw_mode)
{
	if (wifi_softap_set_cond_check() == EXT_WIFI_FAIL)
		return EXT_WIFI_FAIL;

	if (hw_mode > WIFI_MODE_11B_G_N_AX) {
		wpa_error_log0(MSG_ERROR, "physical mode value is error.");
		return EXT_WIFI_FAIL;
	}
	g_ap_opt_set.hw_mode = hw_mode;
	return EXT_WIFI_OK;
}

int uapi_wifi_softap_set_shortgi(int flag)
{
	if (wifi_softap_set_cond_check() == EXT_WIFI_FAIL)
		return EXT_WIFI_FAIL;

	if ((flag < WPA_FLAG_OFF) || (flag > WPA_FLAG_ON)) {
		wpa_error_log0(MSG_ERROR, "input flag error.");
		return EXT_WIFI_FAIL;
	}
	g_ap_opt_set.short_gi_off = !flag;
	return EXT_WIFI_OK;
}

int uapi_wifi_softap_set_beacon_period(int beacon_period)
{
	if (wifi_softap_set_cond_check() == EXT_WIFI_FAIL)
		return EXT_WIFI_FAIL;

	if ((beacon_period < WPA_AP_MIN_BEACON) || (beacon_period > WPA_AP_MAX_BEACON)) {
		wpa_error_log2(MSG_ERROR, "beacon value must be %d ~ %d.", WPA_AP_MIN_BEACON, WPA_AP_MAX_BEACON);
		return EXT_WIFI_FAIL;
	}
	g_ap_opt_set.beacon_period = beacon_period;
	return EXT_WIFI_OK;
}

int uapi_wifi_softap_set_dtim_period(int dtim_period)
{
	if (wifi_softap_set_cond_check() == EXT_WIFI_FAIL)
		return EXT_WIFI_FAIL;

	if ((dtim_period > WPA_AP_MAX_DTIM) || (dtim_period < WPA_AP_MIN_DTIM)) {
		wpa_error_log2(MSG_ERROR, "dtim_period must be %d ~ %d.", WPA_AP_MIN_DTIM, WPA_AP_MAX_DTIM);
		return EXT_WIFI_FAIL;
	}
	g_ap_opt_set.dtim_period = dtim_period;
	return EXT_WIFI_OK;
}

int uapi_wifi_softap_set_group_rekey(int wifi_group_rekey)
{
	if (wifi_softap_set_cond_check() == EXT_WIFI_FAIL)
		return EXT_WIFI_FAIL;

	if ((wifi_group_rekey > WPA_MAX_REKEY_TIME) || (wifi_group_rekey < WPA_MIN_REKEY_TIME)) {
		wpa_error_log2(MSG_ERROR, "group_rekey must be %d ~ %d.", WPA_MIN_REKEY_TIME, WPA_MAX_REKEY_TIME);
		return EXT_WIFI_FAIL;
	}
	g_ap_opt_set.wpa_group_rekey = wifi_group_rekey;
	return EXT_WIFI_OK;
}

protocol_mode_enum uapi_wifi_softap_get_protocol_mode(void)
{
	if (is_lock_flag_off() == EXT_WIFI_FAIL) {
		wpa_error_log0(MSG_ERROR, "wifi dev start or stop is running.");
		return WIFI_MODE_UNDEFINE;
	}
	wpa_error_log1(MSG_ERROR, "softap phymode:%u.", (unsigned int)g_ap_opt_set.hw_mode);
	return g_ap_opt_set.hw_mode;
}

int uapi_wifi_softap_get_connected_sta(ext_wifi_ap_sta_info *sta_list, unsigned int *sta_num)
{
	unsigned int ret_val;
	unsigned int int_save;

	if ((sta_list == NULL) || (sta_num == NULL)) {
		wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_get_connected_sta: input params error.");
		return EXT_WIFI_FAIL;
	}

	struct ext_wifi_dev *wifi_dev = wifi_dev_get(EXT_WIFI_IFTYPE_AP);
	if ((wifi_dev == NULL) || (wifi_dev->priv == NULL) || (g_ap_sta_info != NULL)) {
		wpa_error_log0(MSG_ERROR, "uapi_wifi_softap_get_connected_sta: softap may not start or busy.");
		return EXT_WIFI_FAIL;
	}

	struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)(wifi_dev->priv);

	os_intlock(&int_save);
	g_ap_sta_info = sta_list;
	g_sta_num = *sta_num;
	os_intrestore(int_save);

	(void)os_event_clear(g_softap_event, ~WPA_EVENT_AP_SHOW_STA_FLAG);
	(void)wpa_cli_show_sta(wpa_s);
	wpa_error_log0(MSG_ERROR, "os_event_read WPA_EVENT_AP_SHOW_STA_FLAG");
	(void)os_event_read(g_softap_event, WPA_EVENT_AP_SHOW_STA_FLAG, &ret_val,
	                    WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_EVENT_DELAY_5S);
	g_ap_sta_info = NULL;
	if ((ret_val == WPA_EVENT_AP_SHOW_STA_ERROR) || (ret_val == WIFI_ERRNO_EVENT_READ_TIMEOUT)) {
		wpa_error_log1(MSG_ERROR, "os_event_read WPA_EVENT_AP_SHOW_STA_FLAG ret_val = %x", ret_val);
		*sta_num = 0;
		g_sta_num = 0;
		return EXT_WIFI_FAIL;
	}
	*sta_num = g_sta_num;
	g_sta_num = 0;
	return EXT_WIFI_OK;
}

