/*
 * hostapd / main()
 * Copyright (c) 2002-2011, Jouni Malinen <j@w1.fi>
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2014-2019. All rights reserved.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/common.h"
#include "ap/ap_drv_ops.h"
#include "ap/ap_config.h"
#include "ctrl_iface.h"
#include "hostapd_if.h"
#include "utils/eloop.h"
#include "eap_server/eap.h"
#include "wifi_api.h"
#include "securec.h"

typedef struct hostapd_conf hostapd_conf;

struct hostapd_data *g_hapd = NULL;

struct hapd_global {
	void **drv_priv;
	size_t drv_count;
};

static struct hapd_global global;
hostapd_conf g_global_conf;
static struct hapd_interfaces *g_interfaces = NULL;

/**
 * hostapd_driver_init - Preparate driver interface
 */
static int hostapd_driver_init(struct hostapd_iface *iface)
{
	struct wpa_init_params params;
	size_t i;
	struct hostapd_data *hapd = iface->bss[0];
	struct hostapd_bss_config *conf = hapd->conf;
	u8 *b = conf->bssid;

	if ((hapd->driver == NULL) || (hapd->driver->hapd_init == NULL)) {
		wpa_error_log0(MSG_ERROR, "No hostapd driver wrapper available");
		return -1;
	}

	/* Initialize the driver interface */
	if (!(b[0] | b[1] | b[2] | b[3] | b[4] | b[5]))
		b = NULL;

	(void)os_memset(&params, 0, sizeof(params));
	for (i = 0; wpa_drivers[i] != NULL; i++) {
		if (wpa_drivers[i] != hapd->driver)
			continue;

		params.global_priv = global.drv_priv[i];
		break;
	}
	params.bssid = b;
	params.ifname = hapd->conf->iface;
	params.driver_params = hapd->iconf->driver_params;
	params.own_addr = hapd->own_addr;
	wpa_error_buf(MSG_ERROR, "hostapd_driver_init: ifname %s", params.ifname, os_strlen(params.ifname));

	hapd->drv_priv = hapd->driver->hapd_init(hapd, &params);
	if (hapd->drv_priv == NULL) {
		wpa_error_log0(MSG_ERROR, "hapd driver initialization failed.");
		hapd->driver = NULL;
		return -1;
	}

	return 0;
}

/**
 * hostapd_interface_init - Read configuration file and init BSS data
 *
 * This function is used to parse configuration file for a full interface (one
 * or more BSSes sharing the same radio) and allocate memory for the BSS
 * interfaces. No actiual driver operations are started.
 */
static struct hostapd_iface * hostapd_interface_init(struct hapd_interfaces *interfaces,
                                                     const char *config, int debug)
{
	struct hostapd_iface *iface = NULL;

	(void)debug;
	iface = hostapd_init(interfaces, config);
	if ((iface == NULL) || (iface->conf == NULL))
		return NULL;
	iface->interfaces = interfaces;

	if (iface->conf->bss[0]->iface[0] == '\0') {
		wpa_error_log0(MSG_ERROR, "Interface name not specified");
		hostapd_interface_deinit_free(iface);
		return NULL;
	}

	g_hapd = iface->bss[0];
	return iface;
}

static int eap_server_register_methods(void)
{
	int ret = 0;
#ifdef EAP_SERVER_IDENTITY
	if (ret == 0)
		ret = eap_server_identity_register();
#endif /* EAP_SERVER_IDENTITY */

#ifdef EAP_SERVER_WSC
	if (ret == 0)
		ret = eap_server_wsc_register();
#endif /* EAP_SERVER_WSC */
	return ret;
}

static int hostapd_global_init(const struct hapd_interfaces *interfaces, const char *entropy_file)
{
	int i;
	(void)interfaces;
	(void)entropy_file;

	(void)os_memset(&global, 0, sizeof(global));
	if (eap_server_register_methods()) {
		wpa_error_log0(MSG_ERROR, "Failed to register EAP methods");
		return -1;
	}
	if (eloop_init(ELOOP_TASK_HOSTAPD)) {
		wpa_error_log0(MSG_ERROR, "Failed to initialize event loop");
		return -1;
	}

	for (i = 0; wpa_drivers[i]; i++)
		global.drv_count++;

	if (global.drv_count == 0) {
		wpa_error_log0(MSG_ERROR, "No drivers enabled");
		return -1;
	}
	global.drv_priv = os_calloc(global.drv_count, sizeof(void *));
	if (global.drv_priv == NULL)
		return -1;
	return 0;
}

void hostapd_global_deinit(void)
{
	os_free(global.drv_priv);
	global.drv_priv = NULL;
	g_hapd = NULL;
	eloop_destroy(ELOOP_TASK_HOSTAPD);
	eap_server_unregister_methods();
}

void hostapd_global_interfaces_deinit(void)
{
	if (g_interfaces != NULL)
		os_free(g_interfaces->iface);
	os_free(g_interfaces);
	g_interfaces = NULL;
	wpa_error_log0(MSG_ERROR, "hostapd_exit\n");
}

static int hostapd_global_run(const struct hapd_interfaces *ifaces, int daemonize, const char *pid_file)
{
	(void)pid_file;
	(void)daemonize;
	(void)ifaces;
	return eloop_run(ELOOP_TASK_HOSTAPD);
}

static int hostapd_bss_init(struct hostapd_bss_config *bss, const hostapd_conf *conf, const char *ifname)
{
	int errors = 0;
	if (ifname != NULL)
		(void)os_strlcpy(bss->iface, ifname, strlen(ifname) + 1);
	bss->ssid.ssid_len = os_strlen(conf->ssid);
	if ((bss->ssid.ssid_len > SSID_MAX_LEN) || (bss->ssid.ssid_len < 1)) {
		wpa_error_buf(MSG_ERROR, "invalid SSID '%s'", conf->ssid, strlen(conf->ssid));
		errors++;
	} else {
		(void)os_memcpy(bss->ssid.ssid, conf->ssid, bss->ssid.ssid_len);
		bss->ssid.ssid_set = 1;
	}
	bss->auth_algs = conf->auth_algs;
	bss->wpa_key_mgmt = conf->wpa_key_mgmt;
	bss->wpa = conf->wpa;
	if (bss->wpa) {
		bss->eapol_version = 1;
		if (conf->wpa_pairwise)
			bss->wpa_pairwise = conf->wpa_pairwise;
	}
	if (bss->wpa && bss->wpa_key_mgmt != WPA_KEY_MGMT_OWE) {
		size_t len = os_strlen((char *)conf->key);
		if (len < 8 || len > 64) { /* 8: MIN passphrase len, 64: MAX passphrase len */
			wpa_error_log1(MSG_ERROR, "invalid WPA passphrase length %u (expected 8..63)", len);
			errors++;
		} else if (len == 64) { /* 64: MAX passphrase len */
			if (bss->ssid.wpa_psk != NULL)
				(void)memset_s(bss->ssid.wpa_psk, sizeof(*(bss->ssid.wpa_psk)), 0, sizeof(*(bss->ssid.wpa_psk)));
			os_free(bss->ssid.wpa_psk);
			bss->ssid.wpa_psk = os_zalloc(sizeof(struct hostapd_wpa_psk));
			if (bss->ssid.wpa_psk == NULL)
				errors++;
			else if (hexstr2bin((char *)conf->key, bss->ssid.wpa_psk->psk, PMK_LEN) ||
			         conf->key[PMK_LEN * 2] != '\0') { /* 2: HEX String to BIN */
				errors++;
			} else {
				bss->ssid.wpa_psk->group = 1;
				os_free(bss->ssid.wpa_passphrase);
				bss->ssid.wpa_passphrase = NULL;
			}
		} else {
			os_free(bss->ssid.wpa_passphrase);
			bss->ssid.wpa_passphrase = os_strdup((char *)conf->key);
			if (bss->ssid.wpa_psk != NULL)
				(void)memset_s(bss->ssid.wpa_psk, sizeof(*(bss->ssid.wpa_psk)), 0, sizeof(*(bss->ssid.wpa_psk)));
			os_free(bss->ssid.wpa_psk);
			bss->ssid.wpa_psk = NULL;
		}
	}
	bss->eap_server = 1;
#ifdef CONFIG_DRIVER_SOC
	bss->ignore_broadcast_ssid = conf->ignore_broadcast_ssid; /* set hidden AP */
#endif /* CONFIG_DRIVER_SOC */
	return errors;
}

#ifdef CONFIG_IEEE80211AX
static void hostapd_config_80211ax(struct hostapd_config * conf)
{
	// refer to hostapd_config_defaults() function
	conf->he_op.he_rts_threshold = HE_OPERATION_RTS_THRESHOLD_MASK >>
		HE_OPERATION_RTS_THRESHOLD_OFFSET;
	/* Set default basic MCS/NSS set to single stream MCS 0-7 */
	conf->he_op.he_basic_mcs_nss_set = 0xfffc;

	// refer to hostapd.conf file
	conf->he_phy_capab.he_su_beamformer = 0;
	conf->he_phy_capab.he_su_beamformee = 1;
	conf->he_phy_capab.he_mu_beamformer = 0;
	conf->he_op.he_bss_color = 1;
	conf->he_op.he_default_pe_duration = 0;
	conf->he_op.he_twt_required = 0;
	conf->he_op.he_rts_threshold = 0;

	// 0 = 20 or 40 MHz operating Channel width
	// 1 = 80 MHz channel width
	// 2 = 160 MHz channel width
	// 3 = 80+80 MHz channel width
	conf->he_oper_chwidth = 0;

	conf->he_mu_edca.he_qos_info |= (8 << 0);
	conf->he_mu_edca.he_qos_info |= (1 << 4);
	conf->he_mu_edca.he_qos_info |= (1 << 5);
	conf->he_mu_edca.he_qos_info |= (1 << 6);
	conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_ACI_IDX] |= (0 << 0);
	conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_ECW_IDX] |= (0xf << 0);
	conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_ECW_IDX] |= (0xf << 4);
	conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_TIMER_IDX] = 0xff;
	conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_ACI_IDX] |= (0 << 0);
	conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_ACI_IDX] |= (1 << 5);
	conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_ECW_IDX] |= (0xf << 0);
	conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_ECW_IDX] |= (0xf << 4);
	conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_TIMER_IDX] = 0xff;
	conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_ECW_IDX] |= (0xf << 0);
	conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_ECW_IDX] |= (0xf << 4);
	conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_ACI_IDX] |= (0 << 0);
	conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_ACI_IDX] |= (2 << 5);
	conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_TIMER_IDX] = 0xff;
	conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_ACI_IDX] |= (0 << 0);
	conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_ACI_IDX] |= (3 << 5);
	conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_ECW_IDX] |= (0xf << 0);
	conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_ECW_IDX] |= (0xf << 4);
	conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_TIMER_IDX] = 0xff;

	conf->spr.sr_control =  0xff;
}
#endif

#ifdef CONFIG_WEP
static int hostapd_config_read_wep(struct hostapd_wep_keys *wep, int keyidx,
				   char *val)
{
	size_t len = os_strlen(val);

    if (keyidx < 0 || keyidx > 3)
		return -1;

	if (len == 0) {
		int i, set = 0;

		bin_clear_free(wep->key[keyidx], wep->len[keyidx]);
		wep->key[keyidx] = NULL;
		wep->len[keyidx] = 0;
		for (i = 0; i < NUM_WEP_KEYS; i++) {
			if (wep->key[i])
				set++;
		}
		if (!set)
			wep->keys_set = 0;
		return 0;
	}

	if (wep->key[keyidx] != NULL)
		return -1;

	if (val[0] == '"') {
		if (len < 2 || val[len - 1] != '"')
			return -1;
		len -= 2;
		wep->key[keyidx] = os_memdup(val + 1, len);
		if (wep->key[keyidx] == NULL)
			return -1;
		wep->len[keyidx] = len;
	} else {
		if (len & 1)
			return -1;
		len /= 2;
		wep->key[keyidx] = os_malloc(len);
		if (wep->key[keyidx] == NULL)
			return -1;
		wep->len[keyidx] = len;
		if (hexstr2bin(val, wep->key[keyidx], len) < 0)
			return -1;
	}

	wep->keys_set++;

	return 0;
}
#endif

struct hostapd_config * hostapd_config_read2(const char *ifname)
{
	int errors = 0;
	hostapd_conf *hconf = &g_global_conf;
	if (ifname == NULL)
		return NULL;
	struct hostapd_config *conf = hostapd_config_defaults();
	if (conf == NULL)
		return NULL;

	conf->ht_capab |= HT_CAP_INFO_SHORT_GI20MHZ; /* default setting. */
	conf->hw_mode = HOSTAPD_MODE_IEEE80211G;
#ifdef CONFIG_IEEE80211N
	conf->ieee80211n = 1;
#endif
	if (g_ap_opt_set.short_gi_off == WPA_FLAG_ON)
		conf->ht_capab &= ~HT_CAP_INFO_SHORT_GI20MHZ;

#ifdef CONFIG_IEEE80211AX
	conf->ieee80211ax = (g_ap_opt_set.hw_mode == WIFI_MODE_11B_G_N_AX);
	if (conf->ieee80211ax) {
		hostapd_config_80211ax(conf);
	}
#endif

	/* set default driver based on configuration */
	for (size_t i = 0; wpa_drivers[i] != NULL; i++) {
		if (os_strcmp(hconf->driver, wpa_drivers[i]->name) == 0) {
			conf->driver = wpa_drivers[i];
			break;
		}
	}
	if (conf->driver == NULL) {
		wpa_error_log0(MSG_ERROR, "No driver wrappers registered!");
		hostapd_config_free(conf);
		return NULL;
	}

	struct hostapd_bss_config *bss = conf->bss[0];
	conf->last_bss = conf->bss[0];
	conf->channel = (u8)hconf->channel_num;

#ifdef CONFIG_WEP
	/* WEP加密设置keys */
    if ((hconf->auth_algs == 1 || hconf->auth_algs == 2) && hconf->wep_idx == 0) {
        bss->ssid.wep.len[0] = (size_t)os_strlen((char *)hconf->key);
        if (bss->ssid.wep.len[0] == 0) {
            bss->ssid.wep.keys_set = 0;
            wpa_error_log0(MSG_ERROR, "hostapd_config_read2::Invalid password\r\n");
        }
        bss->ssid.wep.key[0] = os_memdup(&hconf->key[0], bss->ssid.wep.len[0]);
        if (bss->ssid.wep.key[0] == NULL) {
            wpa_error_log0(MSG_ERROR, "hostapd_config_read2::copy fail\r\n");
            return NULL;
        }
        bss->ssid.wep.keys_set = 1;
    }
#endif /* CONFIG_WEP */

	errors += hostapd_bss_init(bss, hconf, ifname);
	for (size_t i = 0; i < conf->num_bss; i++)
		hostapd_set_security_params(conf->bss[i], 1);

	if (hostapd_config_check(conf, 1))
		errors++;

#ifndef WPA_IGNORE_CONFIG_ERRORS
	if (errors) {
		wpa_error_log1(MSG_ERROR, "%d errors found in configuration file ", errors);
		wpa_error_buf(MSG_ERROR, "'%s'", ifname, strlen(ifname));
		hostapd_config_free(conf);
		conf = NULL;
	}
#endif /* WPA_IGNORE_CONFIG_ERRORS */

	return conf;
}

void hostapd_pre_quit(struct hapd_interfaces *interfaces)
{
	if (interfaces == NULL)
		return;

	hostapd_global_ctrl_iface_deinit(interfaces);
	if (interfaces->iface == NULL)
		return;
	/* Deinitialize all interfaces */
	for (size_t i = 0; i < interfaces->count; i++) {
		if (interfaces->iface[i] == NULL)
			continue;
		hostapd_interface_deinit_free(interfaces->iface[i]);
		interfaces->iface[i] = NULL;
	}
}

static void hostapd_quit(struct hapd_interfaces *interfaces, struct ext_wifi_dev *wifi_dev)
{
	hostapd_pre_quit(interfaces);

	hostapd_global_deinit();

	os_free(interfaces->iface);
	interfaces->iface = NULL;
	os_free(interfaces);
	interfaces = NULL;
	g_interfaces = NULL;

	unsigned int int_save;
	os_task_lock(&int_save);
	if (wifi_dev != NULL)
		wifi_dev->priv = NULL;
	os_task_unlock(int_save);

	/* send sta start failed event, softAP and STA share one task */
	(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
}

static struct hapd_interfaces *hostapd_interfaces_init(const char *ifname)
{
	struct hapd_interfaces *interfaces = NULL;
	(void)ifname;
	interfaces = os_zalloc(sizeof(struct hapd_interfaces));
	if (interfaces == NULL) {
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
		return NULL;
	}
	interfaces->reload_config = hostapd_reload_config;
	interfaces->config_read_cb = hostapd_config_read2;
	interfaces->for_each_interface = hostapd_for_each_interface;
	interfaces->ctrl_iface_init = hostapd_ctrl_iface_init;
	interfaces->ctrl_iface_deinit = hostapd_ctrl_iface_deinit;
	interfaces->driver_init = hostapd_driver_init;
	interfaces->count = 1;

	interfaces->iface = os_calloc(interfaces->count, sizeof(struct hostapd_iface *));
	if (interfaces->iface == NULL) {
		wpa_error_log0(MSG_ERROR, "malloc failed");
		os_free(interfaces);
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
		return NULL;
	}

	if (hostapd_global_init(interfaces, NULL)) {
		wpa_error_log0(MSG_ERROR, "Failed to initilize global context");
		hostapd_global_deinit();
		os_free(interfaces->iface);
		os_free(interfaces);
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
		return NULL;
	}

	return interfaces;
}

static struct ext_wifi_dev * hostapd_get_wifi_dev(const char* ifname)
{
	if (ifname == NULL) {
		wpa_error_log0(MSG_ERROR, "hostapd_main: ifname is null \n");
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
		return NULL;
	}

	wpa_debug_level = MSG_DEBUG;

	wpa_printf(MSG_DEBUG, "hostapd_main: ifname = %s", ifname);

	struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_name(ifname);
	if (wifi_dev == NULL) {
		wpa_error_log0(MSG_ERROR, "hostapd_main: los_get_wifi_dev_by_name failed \n");
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
		return NULL;
	}

	return wifi_dev;
}

 void hostapd_main(const char *ifname)
{
	wpa_error_log0(MSG_ERROR, "---> hostapd_main enter.");

	struct ext_wifi_dev *wifi_dev = hostapd_get_wifi_dev(ifname);
	if (wifi_dev == NULL)
		return;

	struct hapd_interfaces *interfaces = hostapd_interfaces_init(ifname);
	if (interfaces == NULL)
		return;

	/* Allocate and parse configuration for full interface files */
	for (size_t i = 0; i < interfaces->count; i++) {
		interfaces->iface[i] = hostapd_interface_init(interfaces, ifname, 0);
		if (interfaces->iface[i] == NULL) {
			wpa_error_log0(MSG_ERROR, "Failed to initialize interface");
			goto OUT;
		}
	}

	unsigned int int_save;
	os_task_lock(&int_save);
	wifi_dev->priv = g_hapd;
	os_task_unlock(int_save);

	g_interfaces = interfaces;

	/*
	 * Enable configured interfaces. Depending on channel configuration,
	 * this may complete full initialization before returning or use a
	 * callback mechanism to complete setup in case of operations like HT
	 * co-ex scans, ACS, or DFS are needed to determine channel parameters.
	 * In such case, the interface will be enabled from eloop context within
	 * hostapd_global_run().
	 */
	interfaces->terminate_on_error = interfaces->count;
	for (size_t i = 0; i < interfaces->count; i++) {
		if (hostapd_driver_init(interfaces->iface[i]) || hostapd_setup_interface(interfaces->iface[i]))
			goto OUT;
	}

	(void)hostapd_global_ctrl_iface_init(interfaces);

	wpa_error_buf(MSG_ERROR, "hostapd_main: wifi_dev: ifname = %s\n", wifi_dev->ifname, strlen(wifi_dev->ifname));
	// return 0 on running success, 1 on already running, -1 on running fail.
	if (hostapd_global_run(interfaces, 0, NULL) >= 0) {
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_OK);
		return;
	}

	wpa_error_log0(MSG_ERROR, "Failed to start eloop");
OUT:
	hostapd_quit(interfaces, wifi_dev);
}


struct hapd_interfaces * hostapd_get_interfaces(void)
{
	return g_interfaces;
}

