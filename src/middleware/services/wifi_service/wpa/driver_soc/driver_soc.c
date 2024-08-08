/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: driver_soc
 */

#include "driver_soc.h"
#include "l2_packet/l2_packet.h"
#include "ap/ap_config.h"
#include "ap/hostapd.h"
#include "driver_soc_ioctl.h"
#include "eloop.h"
#include "wpa_supplicant_if.h"
#include "mesh.h"
#include "scan.h"
#include "wpa.h"
#ifdef LOS_CONFIG_MESH
#include "soc_mesh.h"
#endif /* LOS_CONFIG_MESH */
#include "securec.h"
#include "driver_soc_at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

uint8 g_ssid_prefix_flag = WPA_FLAG_OFF;
int g_sta_delay_report_flag = WPA_FLAG_OFF;
static int g_rx_mgmt_count = 0;

#define SCAN_TIME_OUT 5
#define RX_MGMT_EVENT_MAX_COUNT 15
#define DELAY_REPORT_TIMEOUT 15
#define MAX_BUF_LEN 65535
#define WLAN_MAC_STATUS_MAX 7000

#ifdef CONFIG_ACS
#define OAL_SUCC 0
#define OAL_CONTINUE 2
#endif

static uint32 drv_soc_alg_to_cipher_suite(enum wpa_alg alg, size_t key_len);
static int32 drv_soc_is_ap_interface(ext_iftype_enum_uint8 nlmode);
static void drv_soc_key_ext_free(ext_key_ext_stru *key_ext);
static void drv_soc_ap_settings_free(ext_ap_settings_stru  *apsettings);
static int32 drv_soc_set_key(void *priv, struct wpa_driver_set_key_params *params);
static int32 drv_soc_send_mlme(void *priv, const uint8 *data, size_t data_len, int32 noack, unsigned int freq,
                               const u16 *csa_offs, size_t csa_offs_len);
static int32 drv_soc_send_eapol(void *priv, const uint8 *addr, const uint8 *data, size_t data_len, int32 encrypt,
                                const uint8 *own_addr, uint32 flags);
static int32 drv_soc_driver_send_event(const char *ifname, int32 cmd, const uint8 *buf, uint32 length);
static void drv_soc_driver_event_process(void *eloop_data, void *user_ctx);
static void * drv_soc_hapd_init(struct hostapd_data *hapd, struct wpa_init_params *params);
static void drv_soc_hw_feature_data_free(struct hostapd_hw_modes *modes, uint16 modes_num);
static void drv_soc_rx_mgmt_process(void *ctx, ext_rx_mgmt_stru *rx_mgmt, union wpa_event_data *event);
static void drv_soc_tx_status_process(void *ctx, ext_tx_status_stru *tx_status, union wpa_event_data *event);
static void drv_soc_drv_deinit(void *priv);
static void drv_soc_hapd_deinit(void *priv);
static void drv_soc_driver_scan_timeout(void *eloop_ctx, void *timeout_ctx);
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
static void drv_soc_set_rekey_info(void *priv, const uint8 *kek, size_t kek_len, const uint8 *kck,
                                   size_t kck_len, const uint8 *replay_ctr);
#endif
static ext_driver_data_stru * drv_soc_drv_init(void *ctx, const struct wpa_init_params *params);
static struct hostapd_hw_modes * drv_soc_get_hw_feature_data(void *priv, uint16 *num_modes, uint16 *flags);
static int32 drv_soc_connect(ext_driver_data_stru *drv, struct wpa_driver_associate_params *params);
static int32 drv_soc_try_connect(ext_driver_data_stru *drv, struct wpa_driver_associate_params *params);
static void drv_soc_set_conn_keys(const struct wpa_driver_associate_params *wpa_params,
                                  ext_associate_params_stru *params);
static uint32 drv_soc_cipher_to_cipher_suite(uint32 cipher);
static int32 drv_soc_disconnect(ext_driver_data_stru *drv, uint16 reason_code);
static int32 drv_soc_sta_remove(void *priv, const uint8 *addr);

static int32 drv_soc_is_ap_interface(ext_iftype_enum_uint8 nlmode)
{
	return ((nlmode == EXT_WIFI_IFTYPE_AP) || (nlmode == EXT_WIFI_IFTYPE_P2P_GO));
}

static void drv_soc_key_ext_free(ext_key_ext_stru *key_ext)
{
	if (key_ext == NULL)
		return;

	if (key_ext->addr != NULL) {
		os_free(key_ext->addr);
		key_ext->addr = NULL;
	}

	if (key_ext->seq != NULL) {
		os_free(key_ext->seq);
		key_ext->seq = NULL;
	}

	if (key_ext->key != NULL) {
		(void)memset_s(key_ext->key, key_ext->key_len, 0, key_ext->key_len);
		os_free(key_ext->key);
		key_ext->key = NULL;
	}

	os_free(key_ext);
}

static void drv_soc_ap_settings_free(ext_ap_settings_stru *apsettings)
{
	if (apsettings == NULL)
		return;

	if (apsettings->mesh_ssid != NULL) {
		os_free(apsettings->mesh_ssid);
		apsettings->mesh_ssid = NULL;
	}

	if (apsettings->ssid != NULL) {
		os_free(apsettings->ssid);
		apsettings->ssid = NULL;
	}

	if (apsettings->beacon_data.head != NULL) {
		os_free(apsettings->beacon_data.head);
		apsettings->beacon_data.head = NULL;
	}

	if (apsettings->beacon_data.tail != NULL) {
		os_free(apsettings->beacon_data.tail);
		apsettings->beacon_data.tail = NULL;
	}

	os_free(apsettings);
}

static uint32 drv_soc_alg_to_cipher_suite(enum wpa_alg alg, size_t key_len)
{
	switch (alg) {
		case WPA_ALG_WEP:
			/* key_len = 5 : WEP40, 13 : WEP104 */
			if (key_len == WPA_WEP40_KEY_LEN)
				return RSN_CIPHER_SUITE_WEP40;
			return RSN_CIPHER_SUITE_WEP104;
		case WPA_ALG_TKIP:
			return RSN_CIPHER_SUITE_TKIP;
		case WPA_ALG_CCMP:
			return RSN_CIPHER_SUITE_CCMP;
		case WPA_ALG_GCMP:
			return RSN_CIPHER_SUITE_GCMP;
		case WPA_ALG_CCMP_256:
			return RSN_CIPHER_SUITE_CCMP_256;
		case WPA_ALG_GCMP_256:
			return RSN_CIPHER_SUITE_GCMP_256;
		case WPA_ALG_BIP_CMAC_128:
			return RSN_CIPHER_SUITE_AES_128_CMAC;
		case WPA_ALG_BIP_GMAC_128:
			return RSN_CIPHER_SUITE_BIP_GMAC_128;
		case WPA_ALG_BIP_GMAC_256:
			return RSN_CIPHER_SUITE_BIP_GMAC_256;
		case WPA_ALG_BIP_CMAC_256:
			return RSN_CIPHER_SUITE_BIP_CMAC_256;
		case WPA_ALG_SMS4:
			return RSN_CIPHER_SUITE_SMS4;
		case WPA_ALG_KRK:
			return RSN_CIPHER_SUITE_KRK;
		case WPA_ALG_NONE:
			return 0;
		default:
			return 0;
	}
}

static int drv_soc_init_key(ext_key_ext_stru *key_ext, enum wpa_alg alg, const uint8 *addr, int32 key_idx, int32 set_tx,
                            const uint8 *seq, size_t seq_len, const uint8 *key, size_t key_len)
{
	key_ext->default_types = EXT_KEY_DEFAULT_TYPE_INVALID;
	key_ext->seq_len = seq_len;
	key_ext->key_len = key_len;
	key_ext->key_idx = (uint32)key_idx;
	key_ext->type = EXT_KEYTYPE_DEFAULT_INVALID;
	key_ext->cipher = drv_soc_alg_to_cipher_suite(alg, key_len);
	if ((alg != WPA_ALG_NONE) && (key != NULL) && (key_len != 0)) {
		key_ext->key = (uint8 *)os_zalloc(key_len); /* freed by drv_soc_key_ext_free */
		if ((key_ext->key == NULL) || (memcpy_s(key_ext->key, key_ext->key_len, key, key_len) != EOK))
			return -EXT_EFAIL;
	}
	if ((seq != NULL) && (seq_len != 0)) {
		key_ext->seq = (uint8 *)os_zalloc(seq_len); /* freed by drv_soc_key_ext_free */
		if ((key_ext->seq == NULL) || (memcpy_s(key_ext->seq, key_ext->seq_len, seq, seq_len) != EOK))
			return -EXT_EFAIL;
	}
	if (addr != NULL) {
		if (!is_broadcast_ether_addr(addr)) {
			key_ext->addr = (uint8 *)os_zalloc(ETH_ADDR_LEN); /* freed by drv_soc_key_ext_free */
			if ((key_ext->addr == NULL) || (memcpy_s(key_ext->addr, ETH_ADDR_LEN, addr, ETH_ADDR_LEN) != EOK))
				return -EXT_EFAIL;

			if ((alg != WPA_ALG_WEP) && (key_idx != 0) && (set_tx == 0))
				key_ext->type = (int32)EXT_KEYTYPE_GROUP;

			key_ext->default_types = EXT_KEY_DEFAULT_TYPE_UNICAST;
		} else {
			key_ext->addr = NULL;
			key_ext->default_types = EXT_KEY_DEFAULT_TYPE_MULTICAST;
		}
	}
	if (key_ext->type == EXT_KEYTYPE_DEFAULT_INVALID)
		key_ext->type = (key_ext->addr != NULL) ? EXT_KEYTYPE_PAIRWISE : EXT_KEYTYPE_GROUP;
	if ((alg == WPA_ALG_BIP_CMAC_128) || (alg == WPA_ALG_BIP_GMAC_128) ||
		(alg == WPA_ALG_BIP_GMAC_256) || (alg == WPA_ALG_BIP_CMAC_256))
		key_ext->defmgmt = EXT_TRUE;
	else
		key_ext->def = EXT_TRUE;
	return EXT_SUCC;
}

static int32 drv_soc_set_key(void *priv, struct wpa_driver_set_key_params *params)
{
	int32 ret;
	ext_key_ext_stru *key_ext = EXT_PTR_NULL;
	ext_driver_data_stru *drv = priv;
    const char *ifname = params->ifname;
    enum wpa_alg alg = params->alg;
    const u8 *addr = params->addr;
    int key_idx = params->key_idx;
    int set_tx = params->set_tx;
    const u8 *seq = params->seq;
    size_t seq_len = params->seq_len;
    const u8 *key = params->key;
    size_t key_len = params->key_len;

	/* addr, seq, key will be checked below */
	if ((ifname == NULL) || (priv == NULL))
		return -EXT_EFAIL;

	/* Ignore for P2P Device */
	if (drv->nlmode == EXT_WIFI_IFTYPE_P2P_DEVICE)
		return EXT_SUCC;

	key_ext = os_zalloc(sizeof(ext_key_ext_stru));
	if (key_ext == NULL)
		return -EXT_EFAIL;

	if (drv_soc_init_key(key_ext, alg, addr, key_idx, set_tx, seq, seq_len, key, key_len) != EXT_SUCC) {
		drv_soc_key_ext_free(key_ext);
		return -EXT_EFAIL;
	}

	if (alg == WPA_ALG_NONE) {
		ret = drv_soc_ioctl_del_key((int8 *)ifname, key_ext);
	} else {
		ret = drv_soc_ioctl_new_key((int8 *)ifname, key_ext);
		/* if set new key fail, just return without setting default key */
		if ((ret != EXT_SUCC) || (set_tx == 0) || (alg == WPA_ALG_NONE)) {
			drv_soc_key_ext_free(key_ext);
			return ret;
		}

		if ((drv_soc_is_ap_interface(drv->nlmode)) && (key_ext->addr != NULL) &&
			(!is_broadcast_ether_addr(key_ext->addr))) {
			drv_soc_key_ext_free(key_ext);
			return ret;
		}
		ret = drv_soc_ioctl_set_key((int8 *)ifname, key_ext);
	}

	drv_soc_key_ext_free(key_ext);
	return ret;
}

static void drv_soc_set_ap_freq(ext_ap_settings_stru *apsettings, const struct wpa_driver_ap_params *params)
{
	if (params->freq != NULL) {
		apsettings->freq_params.mode               = (int32)(params->freq->mode);
		apsettings->freq_params.freq               = params->freq->freq;
		apsettings->freq_params.channel            = params->freq->channel;
		apsettings->freq_params.ht_enabled         = params->freq->ht_enabled;
		apsettings->freq_params.center_freq1       = params->freq->center_freq1;
		apsettings->freq_params.bandwidth          = (int32)EXT_CHAN_WIDTH_20;
	}
}

static int drv_soc_set_ap_beacon_data(ext_ap_settings_stru *apsettings, const struct wpa_driver_ap_params *params)
{
	if ((params->head != NULL) && (params->head_len != 0)) {
		apsettings->beacon_data.head_len = params->head_len;
		/* beacon_data.head freed by drv_soc_ap_settings_free */
		apsettings->beacon_data.head = (uint8 *)os_zalloc(apsettings->beacon_data.head_len);
		if (apsettings->beacon_data.head == NULL)
			return -EXT_EFAIL;
		if (memcpy_s(apsettings->beacon_data.head, apsettings->beacon_data.head_len,
		    params->head, params->head_len) != EOK)
			return -EXT_EFAIL;
	}

	if ((params->tail != NULL) && (params->tail_len != 0)) {
		apsettings->beacon_data.tail_len = params->tail_len;
		/* beacon_data.tail freed by drv_soc_ap_settings_free */
		apsettings->beacon_data.tail = (uint8 *)os_zalloc(apsettings->beacon_data.tail_len);
		if (apsettings->beacon_data.tail == NULL)
			return -EXT_EFAIL;

		if (memcpy_s(apsettings->beacon_data.tail, apsettings->beacon_data.tail_len,
		    params->tail, params->tail_len) != EOK)
			return -EXT_EFAIL;
	}
	return EXT_SUCC;
}

static int32 drv_soc_set_sae_pwe(int pwe)
{
	int32 sae_pwe = WIFI_SAE_PWE_UNSPECIFIED;

	if (pwe == 0)
		sae_pwe = WIFI_SAE_PWE_HUNT_AND_PECK;
	else if (pwe == 1)
		sae_pwe = WIFI_SAE_PWE_HASH_TO_ELEMENT;
	else if (pwe == 2)
		sae_pwe = WIFI_SAE_PWE_BOTH;
	return sae_pwe;
}

static int32 drv_soc_set_ap(void *priv, struct wpa_driver_ap_params *params)
{
	int32 ret;
	ext_ap_settings_stru *apsettings = EXT_PTR_NULL;
	ext_driver_data_stru *drv = (ext_driver_data_stru *)priv;
	if ((priv == NULL) || (params == NULL) || (params->freq == NULL))
		return -EXT_EFAIL;

	apsettings = os_zalloc(sizeof(ext_ap_settings_stru));
	if (apsettings == NULL)
		return -EXT_EFAIL;
	apsettings->beacon_interval = params->beacon_int;
	apsettings->dtim_period     = params->dtim_period;
	apsettings->hidden_ssid     = (ext_hidden_ssid_enum_uint8)params->hide_ssid;
	apsettings->sae_pwe = drv_soc_set_sae_pwe(params->sae_pwe);
	if ((params->auth_algs & (WPA_AUTH_ALG_OPEN | WPA_AUTH_ALG_SHARED)) == (WPA_AUTH_ALG_OPEN | WPA_AUTH_ALG_SHARED))
		apsettings->auth_type = EXT_AUTHTYPE_AUTOMATIC;
	else if ((params->auth_algs & WPA_AUTH_ALG_SHARED) == WPA_AUTH_ALG_SHARED)
		apsettings->auth_type = EXT_AUTHTYPE_SHARED_KEY;
	else
		apsettings->auth_type = EXT_AUTHTYPE_OPEN_SYSTEM;

	/* wifi driver will copy mesh_ssid by itself. */
	if ((params->ssid != NULL) && (params->ssid_len != 0)) {
		apsettings->ssid_len = params->ssid_len;
		apsettings->ssid = (uint8 *)os_zalloc(apsettings->ssid_len);
		if ((apsettings->ssid == NULL) || (memcpy_s(apsettings->ssid, apsettings->ssid_len,
		    params->ssid, params->ssid_len) != EOK))
			goto FAILED;
	}
	drv_soc_set_ap_freq(apsettings, params);
	if (drv_soc_set_ap_beacon_data(apsettings, params) != EXT_SUCC)
		goto FAILED;
	if (drv->beacon_set == EXT_TRUE)
		ret = drv_soc_ioctl_change_beacon(drv->iface, apsettings);
	else
		ret = drv_soc_ioctl_set_ap(drv->iface, apsettings);
	if (ret == EXT_SUCC)
		drv->beacon_set = EXT_TRUE;
	drv_soc_ap_settings_free(apsettings);
	return ret;
FAILED:
	drv_soc_ap_settings_free(apsettings);
	return -EXT_EFAIL;
}

static int32 drv_soc_send_mlme(void *priv, const uint8 *data, size_t data_len,
                               int32 noack, unsigned int freq, const u16 *csa_offs, size_t csa_offs_len)
{
	int32 ret;
	ext_driver_data_stru *drv = priv;
	ext_mlme_data_stru *mlme_data = EXT_PTR_NULL;
	errno_t rc;
	(void)freq;
	(void)csa_offs;
	(void)csa_offs_len;
	(void)noack;
	if ((priv == NULL) || (data == NULL))
		return -EXT_EFAIL;
	mlme_data = os_zalloc(sizeof(ext_mlme_data_stru));
	if (mlme_data == NULL)
		return -EXT_EFAIL;
	mlme_data->data = NULL;
	mlme_data->data_len = data_len;
	mlme_data->send_action_cookie = &(drv->send_action_cookie);
	if ((data != NULL) && (data_len != 0)) {
		mlme_data->data = (uint8 *)os_zalloc(data_len);
		if (mlme_data->data == NULL) {
			os_free(mlme_data);
			return -EXT_EFAIL;
		}
		rc = memcpy_s(mlme_data->data, mlme_data->data_len, data, data_len);
		if (rc != EOK) {
			os_free(mlme_data->data);
			os_free(mlme_data);
			return -EXT_EFAIL;
		}
	}
	ret = drv_soc_ioctl_send_mlme(drv->iface, mlme_data);
	os_free(mlme_data->data);
	os_free(mlme_data);
	if (ret != EXT_SUCC)
		ret = -EXT_EFAIL;
	return ret;
}

static void drv_soc_receive_eapol(void *ctx, const uint8 *src_addr, const uint8 *buf, uint32 len)
{
	ext_driver_data_stru *drv = ctx;
	if ((ctx == NULL) || (src_addr == NULL) || (buf == NULL) || (len < sizeof(struct l2_ethhdr))) {
		wpa_error_log0(MSG_DEBUG, "drv_soc_receive_eapol invalid input.");
		return;
	}

	drv_event_eapol_rx(drv->ctx, src_addr, buf + sizeof(struct l2_ethhdr), len - sizeof(struct l2_ethhdr));
}

static int32 drv_soc_send_eapol(void *priv, const uint8 *addr, const uint8 *data, size_t data_len,
                                int32 encrypt, const uint8 *own_addr, uint32 flags)
{
	ext_driver_data_stru *drv = priv;
	int32 ret;
	uint32 frame_len;
	uint8 *frame_buf = EXT_PTR_NULL;
	uint8 *payload = EXT_PTR_NULL;
	struct l2_ethhdr *l2_ethhdr = EXT_PTR_NULL;
	errno_t rc;
	(void)encrypt;
	(void)flags;

	if ((priv == NULL) || (addr == NULL) || (data == NULL) || (own_addr == NULL) || (data_len > MAX_BUF_LEN))
		return -EXT_EFAIL;

	frame_len = (uint32)(data_len + sizeof(struct l2_ethhdr));
	frame_buf = os_zalloc(frame_len);
	if (frame_buf == NULL)
		return -EXT_EFAIL;

	l2_ethhdr = (struct l2_ethhdr *)frame_buf;
	rc = memcpy_s(l2_ethhdr->h_dest, ETH_ADDR_LEN, addr, ETH_ADDR_LEN);
	if (rc != EOK) {
		os_free(frame_buf);
		return -EXT_EFAIL;
	}
	rc = memcpy_s(l2_ethhdr->h_source, ETH_ADDR_LEN, own_addr, ETH_ADDR_LEN);
	if (rc != EOK) {
		os_free(frame_buf);
		return -EXT_EFAIL;
	}
	l2_ethhdr->h_proto = host_to_be16(ETH_P_PAE);
	payload = (uint8 *)(l2_ethhdr + 1);
	rc = memcpy_s(payload, data_len, data, data_len);
	if (rc != EOK) {
		os_free(frame_buf);
		return -EXT_EFAIL;
	}
	ret = l2_packet_send(drv->eapol_sock, addr, ETH_P_EAPOL, frame_buf, frame_len);
	os_free(frame_buf);
	return ret;
}

static ext_driver_data_stru * drv_soc_send_event_get_drv(const char *ifname)
{
	ext_driver_data_stru *drv = NULL;
	struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_name(ifname);
	if ((wifi_dev == NULL) || (wifi_dev->priv == NULL))
		return NULL;
	if (wifi_dev->iftype == EXT_WIFI_IFTYPE_AP) {
		if (eloop_is_running(ELOOP_TASK_HOSTAPD) != EXT_WIFI_OK)
			return NULL;
		drv = ((struct hostapd_data*)(wifi_dev->priv))->drv_priv;
	} else {
		if (eloop_is_running(ELOOP_TASK_WPA) != EXT_WIFI_OK)
			return NULL;
		drv = ((struct wpa_supplicant*)(wifi_dev->priv))->drv_priv;
	}
	return drv;
}

static int32 drv_soc_driver_send_event(const char *ifname, int32 cmd, const uint8 *buf, uint32 length)
{
	int8 *data_ptr = NULL;
	int8 *packet = NULL;
	ext_driver_data_stru *drv = NULL;
	unsigned int int_save;
	int32 ret;
	errno_t rc;

	if (ifname == NULL)
		return -EXT_EFAIL;

	drv = drv_soc_send_event_get_drv(ifname);
	if ((drv == NULL) || (drv->event_queue == NULL))
		return -EXT_EFAIL;
#ifndef LOS_CONFIG_P2P
	if (cmd == EXT_ELOOP_EVENT_TX_STATUS)
		return -EXT_EFAIL;
#endif /* LOS_CONFIG_P2P */
	if ((cmd == EXT_ELOOP_EVENT_RX_MGMT) && (g_rx_mgmt_count >= RX_MGMT_EVENT_MAX_COUNT)) {
		wpa_warning_log1(MSG_DEBUG, "rx_mgmt is full : %d\n\r", g_rx_mgmt_count);
		return -EXT_EFAIL;
	}
	packet = (int8 *)os_zalloc(length + 8); /* 8 bytes of cmd and length */
	if (packet == NULL)
		return -EXT_EFAIL;

	data_ptr = packet;
	*(uint32 *)data_ptr = (uint32)cmd;
	*(uint32 *)(data_ptr + 4) = length; /* 4 bytes of cmd */
	data_ptr += 8; /* 8 bytes of cmd and length */
	/* Append the Actual Data */
	if ((buf != NULL) && (length != 0)) {
		rc = memcpy_s(data_ptr, length, buf, length);
		if (rc != EOK) {
			os_free(packet);
			return -EXT_EFAIL;
		}
	}
	/* Post message and wakeup event */
	ret = eloop_post_event(drv->event_queue, (void *)packet, 1);
	if (ret != EXT_SUCC) {
		os_free(packet);
	} else if (cmd == EXT_ELOOP_EVENT_RX_MGMT) {
		os_intlock(&int_save);
		g_rx_mgmt_count++;
		os_intrestore(int_save);
	}
	return ret;
}

static inline void drv_soc_driver_event_new_sta_process(const ext_driver_data_stru *drv, int8 *data_ptr)
{
	ext_new_sta_info_stru *new_sta_info = NULL;
	union wpa_event_data event;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));

	new_sta_info = (ext_new_sta_info_stru *)data_ptr;
	if (is_zero_ether_addr((const uint8 *)new_sta_info->macaddr)) {
		if (drv->ctx != NULL)
			wpa_supplicant_event(drv->ctx, EVENT_DISASSOC, NULL);
	} else {
		event.assoc_info.reassoc     = new_sta_info->reassoc;
		event.assoc_info.req_ies     = new_sta_info->ie;
		event.assoc_info.req_ies_len = new_sta_info->ielen;
		event.assoc_info.addr        = new_sta_info->macaddr;
		if (drv->ctx != NULL)
			wpa_supplicant_event(drv->ctx, EVENT_ASSOC, &event);
	}

	if (new_sta_info->ie != NULL) {
		os_free(new_sta_info->ie);
		new_sta_info->ie = NULL;
	}
}

static inline void drv_soc_driver_event_del_sta_process(ext_driver_data_stru *drv, const int8 *data_ptr)
{
	union wpa_event_data event;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));

	event.disassoc_info.addr = (uint8 *)data_ptr;

	if ((drv == NULL) || (data_ptr == NULL))
		return;

	if (drv->ctx != NULL)
		wpa_supplicant_event(drv->ctx, EVENT_DISASSOC, &event);
}

static inline void drv_soc_driver_event_channel_switch_process(ext_driver_data_stru *drv, const int8 *data_ptr)
{
	union wpa_event_data event;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));

	if ((drv == NULL) || (data_ptr == NULL))
		return;

	event.ch_switch.freq = (int)(((ext_ch_switch_stru *)data_ptr)->freq);

	if (drv->ctx != NULL)
		wpa_supplicant_event(drv->ctx, EVENT_CH_SWITCH, &event);
}

int32 drv_soc_get_scan_auth_type(struct wpa_scan_res *res, struct wpa_ie_data *data1, struct wpa_ie_data *data2)
{
	const u8 *ie, *ie2, *ie3, *mesh;

	ext_bool_enum_uint8 open 	  = EXT_FALSE;
	ext_bool_enum_uint8 sae       = EXT_FALSE;
	ext_bool_enum_uint8 wpa_psk   = EXT_FALSE;
	ext_bool_enum_uint8 wpa2_psk  = EXT_FALSE;
	ext_bool_enum_uint8 wpa_eap   = EXT_FALSE;
	ext_bool_enum_uint8 wpa2_eap  = EXT_FALSE;
	ext_bool_enum_uint8 eap_256   = EXT_FALSE;
	ext_bool_enum_uint8 rsn_psk   = EXT_FALSE;
	ext_bool_enum_uint8 wep       = EXT_FALSE;
	ext_bool_enum_uint8 wapi_psk  = EXT_FALSE;
	ext_bool_enum_uint8 owe       = EXT_FALSE;

	ie = get_vendor_ie((const u8 *)&res[1], res->ie_len + res->beacon_ie_len, WPA_IE_VENDOR_TYPE);
	ie2 = wpa_scan_get_ie(res, WLAN_EID_RSN);
	mesh = wpa_scan_get_ie(res, WLAN_EID_MESH_ID);

	if (ie && wpa_parse_wpa_ie(ie, 2 + ie[1], data1) >= 0) {
		if (data1->key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
			wpa_eap = EXT_TRUE;
		}
		if (data1->key_mgmt & WPA_KEY_MGMT_PSK) {
			wpa_psk = EXT_TRUE;
		}
	}

	if (ie2 && wpa_parse_wpa_ie(ie2, 2 + ie2[1], data2) >= 0) {
		if (data2->key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
			wpa2_eap = EXT_TRUE;
		}
		if (data2->key_mgmt & WPA_KEY_MGMT_IEEE8021X_SHA256) {
			eap_256 = EXT_TRUE;
		}
		if ((data2->key_mgmt & WPA_KEY_MGMT_PSK) || (data2->key_mgmt & WPA_KEY_MGMT_PSK_SHA256)) {
			if (mesh) {
				rsn_psk = EXT_TRUE;
			} else {
				wpa2_psk = EXT_TRUE;
			}
		}
		if (data2->key_mgmt & WPA_KEY_MGMT_SAE) {
			sae = EXT_TRUE;
		} else if (data2->key_mgmt & WPA_KEY_MGMT_OWE) {
			owe = EXT_TRUE;
		}
	}

	ie3 = wpa_scan_get_ie(res, WLAN_EID_WAPI);
	if (ie3) {
		wapi_psk = EXT_TRUE;
	}

	if (!ie && !ie2 && !ie3 &&
		(res->caps & IEEE80211_CAP_PRIVACY)) {
		wep = EXT_TRUE;
	}

	if (!(res->caps & IEEE80211_CAP_PRIVACY)) {
		open = EXT_TRUE;
	}

	if (sae) {
		return wpa2_psk ? EXT_WIFI_SECURITY_WPA3_WPA2_PSK_MIX : EXT_WIFI_SECURITY_SAE;
	} else if (wpa_psk && (wpa2_psk || rsn_psk)) {
		return EXT_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX;
	} else if (wpa_psk) {
		return EXT_WIFI_SECURITY_WPAPSK;
	} else if (wpa2_psk || rsn_psk) {
		return EXT_WIFI_SECURITY_WPA2PSK;
	} else if (wpa_eap) {
		return EXT_WIFI_SECURITY_WPA;
	} else if (wpa2_eap && eap_256) {
		return EXT_WIFI_SECURITY_WPA3_WPA2_MIX;
	} else if (wpa2_eap) {
		return EXT_WIFI_SECURITY_WPA2;
	} else if (eap_256) {
		return EXT_WIFI_SECURITY_WPA3;
	} else if (wep) {
		return EXT_WIFI_SECURITY_WEP;
	} else if (open) {
		return EXT_WIFI_SECURITY_OPEN;
	} else if (owe) {
		return EXT_WIFI_SECURITY_OWE;
	} else if (wapi_psk) {
		return EXT_WIFI_SECURITY_WAPI_PSK;
	} else {
		return EXT_WIFI_SECURITY_UNKNOWN;
	}
}

int32 drv_soc_raw_scan_params_convert(struct wpa_scan_res *res, ext_wifi_scan_result_stru *scan_result_raw)
{
	const u8 *ssid;
    struct wpa_ie_data data1 = {0};
    struct wpa_ie_data data2 = {0};

	ssid = wpa_scan_get_ie(res, WLAN_EID_SSID);
	if (ssid != NULL) {
		if (memcpy_s(scan_result_raw->ssid, MAX_SSID_LEN, ssid + 2, ssid[1]) != EOK) {
			wpa_error_log0(MSG_DEBUG, "drv_soc_raw_scan_params_convert copy ssid failed.");
			return -EXT_EFAIL;
		}
	}

	if (memcpy_s(scan_result_raw->bssid, ETH_ADDR_LEN, res->bssid, ETH_ADDR_LEN) != EOK) {
		wpa_error_log0(MSG_DEBUG, "drv_soc_raw_scan_params_convert copy bssid failed.");
		return -EXT_EFAIL;
	}
	los_freq_to_channel(res->freq, &(scan_result_raw->channel_num));
	scan_result_raw->rssi = res->level / 100; //驱动上报时rssi扩大了100倍
	scan_result_raw->band = 1; // 只支持2.4g
	scan_result_raw->security_type = drv_soc_get_scan_auth_type(res, &data1, &data2);
	return EXT_SUCC;
}

__attribute__((weak)) void drv_soc_driver_event_scan_result_process(ext_driver_data_stru *drv, int8 *data_ptr)
{
	ext_scan_result_stru *scan_result = NULL;
	scan_result = (ext_scan_result_stru *)data_ptr;
	struct wpa_scan_res *res = NULL;
	errno_t rc;
	ext_wifi_scan_result_stru scan_result_raw = {0};
	if ((scan_result->ie_len > MAX_BUF_LEN) || (scan_result->beacon_ie_len > MAX_BUF_LEN) ||
	    (scan_result->variable == NULL))
		goto OUT;
#ifndef EXT_SCAN_SIZE_CROP
	res = (struct wpa_scan_res *)os_zalloc(sizeof(struct wpa_scan_res) + scan_result->ie_len + scan_result->beacon_ie_len);
#else
	res = (struct wpa_scan_res *)os_zalloc(sizeof(struct wpa_scan_res) + scan_result->ie_len);
#endif /* EXT_SCAN_SIZE_CROP */
	if (res == NULL)
		goto OUT;
	res->flags      = (uint32)scan_result->flags;
	res->freq       = scan_result->freq;
	res->caps       = (uint16)scan_result->caps;
#ifndef EXT_SCAN_SIZE_CROP
	res->beacon_int = (uint16)scan_result->beacon_int;
	res->qual       = scan_result->qual;
#endif /* EXT_SCAN_SIZE_CROP */
	res->level      = scan_result->level;
	res->age        = scan_result->age;
	res->ie_len     = scan_result->ie_len;
#ifndef EXT_SCAN_SIZE_CROP
	res->beacon_ie_len = scan_result->beacon_ie_len;
#endif /* EXT_CODE_CROP */
	rc = memcpy_s(res->bssid, ETH_ADDR_LEN, scan_result->bssid, ETH_ADDR_LEN);
	if (rc != EOK)
		goto FAILED;
#ifndef EXT_SCAN_SIZE_CROP
	rc = memcpy_s(&res[1], scan_result->ie_len + scan_result->beacon_ie_len,
					scan_result->variable, scan_result->ie_len + scan_result->beacon_ie_len);
#else
	rc = memcpy_s(&res[1], scan_result->ie_len, scan_result->variable, scan_result->ie_len);
#endif /* EXT_SCAN_SIZE_CROP */
	if (rc != EOK)
		goto FAILED;
	if (drv->scan_ap_num >= SCAN_AP_LIMIT) {
		wpa_error_log0(MSG_ERROR, "drv_soc_driver_event_process: drv->scan_ap_num >= SCAN_AP_LIMIT");
		goto FAILED;
	}
	drv->res[drv->scan_ap_num++] = res;
	if (g_raw_scan_cb != NULL && drv_soc_raw_scan_params_convert(res, &scan_result_raw) == EXT_SUCC) {
		g_raw_scan_cb(&scan_result_raw);
	}
	goto OUT;
FAILED:
	if (res != NULL)
		os_free(res);
OUT:
	os_free(scan_result->variable);
	scan_result->variable = NULL;
}

static void drv_soc_driver_event_connect_result_process(ext_driver_data_stru *drv, int8 *data_ptr)
{
	ext_connect_result_stru *accoc_info = NULL;
	accoc_info = (ext_connect_result_stru *)data_ptr;
	union wpa_event_data event;
	errno_t rc;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));
	if (accoc_info->status != 0) {
		drv->associated = EXT_DISCONNECT;
		/* assoc 失败时, 底层上报错误码会加上MAC_STATUS_MAX偏移; pmkid无效时, 触发reject流程清空pmkid */
		if ((accoc_info->status == WLAN_STATUS_INVALID_PMKID + WLAN_MAC_STATUS_MAX) ||
		    (accoc_info->status == WLAN_STATUS_AUTH_TIMEOUT + WLAN_MAC_STATUS_MAX)) {
			event.assoc_reject.status_code = accoc_info->status;
			wpa_supplicant_event(drv->ctx, EVENT_ASSOC_REJECT, &event);
		} else {
			event.disassoc_info.reason_code = accoc_info->status;
			wpa_supplicant_event(drv->ctx, EVENT_DISASSOC, &event);
		}
	} else {
		drv->associated = EXT_CONNECT;
		rc = memcpy_s(drv->bssid, ETH_ALEN, accoc_info->bssid, ETH_ALEN);
		if (rc != EOK) {
			os_free(accoc_info->req_ie);
			os_free(accoc_info->resp_ie);
			accoc_info->req_ie = NULL;
			accoc_info->resp_ie = NULL;
			return;
		}
		event.assoc_info.req_ies      = accoc_info->req_ie;
		event.assoc_info.req_ies_len  = accoc_info->req_ie_len;
		event.assoc_info.resp_ies     = accoc_info->resp_ie;
		event.assoc_info.resp_ies_len = accoc_info->resp_ie_len;
		event.assoc_info.addr         = accoc_info->bssid;
		event.assoc_info.freq         = accoc_info->freq;
		wpa_supplicant_event(drv->ctx, EVENT_ASSOC, &event);
	}
	if (accoc_info->req_ie != NULL) {
		os_free(accoc_info->req_ie);
		accoc_info->req_ie = NULL;
	}
	if (accoc_info->resp_ie != NULL) {
		os_free(accoc_info->resp_ie);
		accoc_info->resp_ie = NULL;
	}
}

static inline void drv_soc_driver_event_disconnect_process(ext_driver_data_stru *drv, int8 *data_ptr)
{
	ext_disconnect_stru *discon_info = NULL;
	union wpa_event_data event;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));

#ifdef LOS_CONFIG_P2P
	struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_name(drv->iface);
	if ((wifi_dev == NULL) || (wifi_dev->priv == NULL)) {
		discon_info = (ext_disconnect_stru *)data_ptr;
		os_free(discon_info->ie);
		discon_info->ie = NULL;
		return;
	}
#endif

	drv->associated = EXT_DISCONNECT;
	discon_info = (ext_disconnect_stru *)data_ptr;
	event.disassoc_info.reason_code = discon_info->reason;
	event.disassoc_info.ie          = discon_info->ie;
	event.disassoc_info.ie_len      = discon_info->ie_len;
	if (g_wpa_rm_network == SOC_WPA_RM_NETWORK_START)
		g_wpa_rm_network = SOC_WPA_RM_NETWORK_WORKING;
#ifdef LOS_CONFIG_P2P
	if (wifi_dev->iftype == EXT_WIFI_IFTYPE_P2P_CLIENT)
		wpa_supplicant_event(drv->ctx, EVENT_DEAUTH, &event);
	else
#endif /* LOS_CONFIG_P2P */
		wpa_supplicant_event(drv->ctx, EVENT_DISASSOC, &event);

	if (discon_info->ie != NULL) {
		os_free(discon_info->ie);
		discon_info->ie = NULL;
	}
}

#ifndef EXT_CODE_CROP
static void drv_soc_driver_event_timeout_disc_process(const ext_driver_data_stru *drv)
{
	struct wpa_supplicant *wpa_s = NULL;
	struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_name(drv->iface);
	if ((wifi_dev == NULL) || (wifi_dev->priv == NULL) || (wifi_dev->iftype != EXT_WIFI_IFTYPE_STATION))
		return;
	wpa_s = wifi_dev->priv;
	if ((g_sta_delay_report_flag == WPA_FLAG_ON) && (wpa_s->wpa_state != WPA_COMPLETED)) {
		osal_printk("+NOTICE:DISCONNECTED\r\n");
		wpa_send_disconnect_delay_report_events();
	}
	g_sta_delay_report_flag = WPA_FLAG_OFF;
}
#endif

#ifdef CONFIG_WPA3
static void drv_soc_free_ext_auth(ext_external_auth_stru *external_auth_stru)
{
	if (external_auth_stru == NULL)
		return;

	if (external_auth_stru->ssid != NULL) {
		os_free(external_auth_stru->ssid);
		external_auth_stru->ssid = NULL;
	}

	if (external_auth_stru->pmkid != NULL) {
		(void)memset_s(external_auth_stru->pmkid, SAE_PMKID_LEN, 0, SAE_PMKID_LEN);
		os_free(external_auth_stru->pmkid);
		external_auth_stru->pmkid = NULL;
	}
}

static void drv_soc_driver_event_external_auth(ext_driver_data_stru *drv, int8 *data_ptr)
{
	ext_external_auth_stru *external_auth_stru = (ext_external_auth_stru *)data_ptr;
	union wpa_event_data event;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));
	event.external_auth.bssid = external_auth_stru->bssid;
	event.external_auth.ssid = external_auth_stru->ssid;
	event.external_auth.ssid_len = external_auth_stru->ssid_len;
	event.external_auth.pmkid = external_auth_stru->pmkid;
	event.external_auth.key_mgmt_suite = external_auth_stru->key_mgmt_suite;
	event.external_auth.status = external_auth_stru->status;
	event.external_auth.action = external_auth_stru->auth_action;
	wpa_supplicant_event(drv->ctx, EVENT_EXTERNAL_AUTH, &event);
	drv_soc_free_ext_auth(external_auth_stru);
}
#endif /* CONFIG_WPA3 */

#ifdef CONFIG_OWE
static void drv_soc_driver_event_owe_info(ext_driver_data_stru *drv, int8 *data_ptr)
{
	ext_update_dh_ie_stru *owe_info = (ext_update_dh_ie_stru *)data_ptr;
	union wpa_event_data event;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));
	event.update_dh.peer = owe_info->peer;
	event.update_dh.ie = owe_info->ie;
	event.update_dh.ie_len = owe_info->ie_len;
	wpa_supplicant_event(drv->ctx, EVENT_UPDATE_DH, &event);
}
#endif /* CONFIG_OWE */

#ifdef CONFIG_IEEE80211R
static void drv_soc_driver_event_ft_response(ext_driver_data_stru *drv, int8 *data_ptr)
{
    ext_ft_event_stru *ft_event_stru = (ext_ft_event_stru *)data_ptr;
    union wpa_event_data event;
    if (memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data)) != EOK) {
        wpa_error_log0(MSG_ERROR, "drv_soc_driver_event_ft_response: memset_s failed!");
        goto exit;
    }

    event.ft_ies.ies = (ft_event_stru->ies != NULL) ? ft_event_stru->ies : NULL;
    event.ft_ies.ies_len = ft_event_stru->ies_len ? ft_event_stru->ies_len : 0;
    event.ft_ies.ric_ies = (ft_event_stru->ric_ies != NULL) ? ft_event_stru->ric_ies : NULL;
    event.ft_ies.ric_ies_len = ft_event_stru->ric_ies_len ? ft_event_stru->ric_ies_len : 0;

    if (memcpy_s(event.ft_ies.target_ap, ETH_ADDR_LEN, ft_event_stru->target_ap, ETH_ADDR_LEN) != EOK) {
        wpa_error_log0(MSG_ERROR, "drv_soc_driver_event_ft_response: memcpy_s failed!");
        goto exit;
    }

    wpa_supplicant_event(drv->ctx, EVENT_FT_RESPONSE, &event);

exit:
    if (ft_event_stru->ies != NULL) {
        os_free(ft_event_stru->ies);
        ft_event_stru->ies = NULL;
    }

    if (ft_event_stru->ric_ies != NULL) {
        os_free(ft_event_stru->ric_ies);
        ft_event_stru->ric_ies = NULL;
    }
}
#endif /* CONFIG_IEEE80211R */
static inline void drv_soc_driver_event_scan_done_process(ext_driver_data_stru *drv, const int8 *data_ptr)
{
	ext_driver_scan_status_stru *status = (ext_driver_scan_status_stru *)data_ptr;
	(void)eloop_cancel_timeout(drv_soc_driver_scan_timeout, drv, drv->ctx);
	if (status->scan_status != EXT_SCAN_SUCCESS) {
		wpa_error_log1(MSG_ERROR, "drv_soc_driver_event_process: wifi driver scan not success(%u)",
		               (unsigned int)status->scan_status);
	}
	wpa_supplicant_event(drv->ctx, EVENT_SCAN_RESULTS, NULL);
}

#ifdef LOS_CONFIG_P2P
static inline void drv_soc_driver_event_remain_on_channel(ext_driver_data_stru *drv, int8 *data_ptr)
{
	ext_on_channel_stru *remain_on_channel = (ext_on_channel_stru *)data_ptr;
	union wpa_event_data event;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));
	event.remain_on_channel.duration = remain_on_channel->duration;
	event.remain_on_channel.freq = remain_on_channel->freq;
#ifdef LOS_WPA_PATCH
	uint32 channel;
	if (los_freq_to_channel((int)(event.remain_on_channel.freq), &channel) != EXT_WIFI_OK)
		return;
#endif /* LOS_WPA_PATCH */
	wpa_supplicant_event(drv->ctx, EVENT_REMAIN_ON_CHANNEL, &event);
}

static inline void drv_soc_driver_event_cancel_remain_on_channel(ext_driver_data_stru *drv, int8 *data_ptr)
{
	ext_on_channel_stru *remain_on_channel = (ext_on_channel_stru *)data_ptr;
	union wpa_event_data event;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));
	event.remain_on_channel.freq = remain_on_channel->freq;
#ifdef LOS_WPA_PATCH
	uint32 channel;
	if (los_freq_to_channel((int)(event.remain_on_channel.freq), &channel) != EXT_WIFI_OK)
		return;
#endif /* LOS_WPA_PATCH */
	wpa_supplicant_event(drv->ctx, EVENT_CANCEL_REMAIN_ON_CHANNEL, &event);
}
#endif /* LOS_CONFIG_P2P */

static void drv_soc_tx_status_process(void *ctx, ext_tx_status_stru *tx_status, union wpa_event_data *event)
{
	uint16 fc;
	struct ieee80211_hdr *hdr = NULL;

	if ((ctx == NULL) || (tx_status == NULL) || (event == NULL))
		return;

	if (tx_status->buf == NULL)
		return;

	hdr = (struct ieee80211_hdr *)tx_status->buf;
	fc = le_to_host16(hdr->frame_control);

	event->tx_status.type = WLAN_FC_GET_TYPE(fc);
	event->tx_status.stype = WLAN_FC_GET_STYPE(fc);
	event->tx_status.dst = hdr->addr1;
	event->tx_status.data = tx_status->buf;
	event->tx_status.data_len = tx_status->len;
	event->tx_status.ack = (int32)(tx_status->ack != EXT_FALSE);
	wpa_supplicant_event(ctx, EVENT_TX_STATUS, event);

	if (tx_status->buf != NULL) {
		os_free(tx_status->buf);
		tx_status->buf = NULL;
	}
	return;
}

static void drv_soc_driver_event_process_internal(ext_driver_data_stru *drv, int8 *data_ptr, uint32 cmd, uint32 length)
{
	union wpa_event_data event;
	(void)memset_s(&event, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));

	/* EXT_ELOOP_EVENT_SCAN_RESULT 表示扫描结果上报，打印过多 */
	if (cmd != EXT_ELOOP_EVENT_SCAN_RESULT) {
		wpa_warning_log1(MSG_DEBUG, "ext_driver_event %u", cmd);
	}

	switch (cmd) {
		case EXT_ELOOP_EVENT_NEW_STA:
			if (length == sizeof(ext_new_sta_info_stru))
				drv_soc_driver_event_new_sta_process(drv, data_ptr);
			break;
		case EXT_ELOOP_EVENT_DEL_STA:
			if (length == ETH_ADDR_LEN)
				drv_soc_driver_event_del_sta_process(drv, data_ptr);
			break;
		case EXT_ELOOP_EVENT_RX_MGMT:
			if (length == sizeof(ext_rx_mgmt_stru))
				drv_soc_rx_mgmt_process(drv->ctx, (ext_rx_mgmt_stru *)data_ptr, &event);
			break;
		case EXT_ELOOP_EVENT_TX_STATUS:
			if (length == sizeof(ext_tx_status_stru))
				drv_soc_tx_status_process(drv->ctx, (ext_tx_status_stru *)data_ptr, &event);
			break;
		case EXT_ELOOP_EVENT_SCAN_DONE:
			if (length == sizeof(ext_driver_scan_status_stru))
				drv_soc_driver_event_scan_done_process(drv, data_ptr);
			break;
		case EXT_ELOOP_EVENT_SCAN_RESULT:
			if (length == sizeof(ext_scan_result_stru))
				drv_soc_driver_event_scan_result_process(drv, data_ptr);
			break;
		case EXT_ELOOP_EVENT_CONNECT_RESULT:
			if (length == sizeof(ext_connect_result_stru))
				drv_soc_driver_event_connect_result_process(drv, data_ptr);
			break;
		case EXT_ELOOP_EVENT_DISCONNECT:
			if (length == sizeof(ext_disconnect_stru))
				drv_soc_driver_event_disconnect_process(drv, data_ptr);
			break;
#ifdef LOS_CONFIG_P2P
		case EXT_ELOOP_EVENT_REMAIN_ON_CHANNEL:
			if (length == sizeof(ext_on_channel_stru))
				drv_soc_driver_event_remain_on_channel(drv, data_ptr);
			break;

		case EXT_ELOOP_EVENT_CANCEL_REMAIN_ON_CHANNEL:
			if (length == sizeof(ext_on_channel_stru))
				drv_soc_driver_event_cancel_remain_on_channel(drv, data_ptr);
			break;
#endif /* LOS_CONFIG_P2P */

		case EXT_ELOOP_EVENT_CHANNEL_SWITCH:
			if (length == sizeof(ext_ch_switch_stru))
				drv_soc_driver_event_channel_switch_process(drv, data_ptr);
			break;
#ifndef EXT_CODE_CROP
		case EXT_ELOOP_EVENT_TIMEOUT_DISCONN:
			drv_soc_driver_event_timeout_disc_process(drv);
			break;
#endif
#ifdef CONFIG_WPA3
		case EXT_ELOOP_EVENT_EXTERNAL_AUTH:
			if (length == sizeof(ext_external_auth_stru))
				drv_soc_driver_event_external_auth(drv, data_ptr);
			break;
#endif /* CONFIG_WPA3 */

#ifdef CONFIG_OWE
		case EXT_ELOOP_EVENT_OWE_INFO:
			if (length == sizeof(ext_update_dh_ie_stru))
				drv_soc_driver_event_owe_info(drv, data_ptr);
			break;
#endif /* CONFIG_OWE */

#ifdef CONFIG_IEEE80211R
		case EXT_ELOOP_EVENT_FT_RESPONSE:
			if (length == sizeof(ext_ft_event_stru))
				drv_soc_driver_event_ft_response(drv, data_ptr);
			break;
#endif /* CONFIG_IEEE80211R */
		default:
			break;
	}
}

static void drv_soc_driver_event_process(void *eloop_data, void *user_ctx)
{
	ext_driver_data_stru *drv = NULL;
	void *packet = NULL;
	int8 *data_ptr = NULL;
	uint32 cmd;
	uint32 length;

	(void)user_ctx;
	if (eloop_data == NULL)
		return;
	drv = eloop_data;
	for (;;) {
		packet = eloop_read_event(drv->event_queue, 0);
		if (packet == NULL)
			break;
#ifdef LOS_CONFIG_SUPPORT_CPUP
		uapi_cpup_load_check_proc(uapi_task_get_current_id(), LOAD_SLEEP_TIME_DEFAULT);
#endif
		data_ptr = packet; /* setup a pointer to the payload */
		cmd = *(uint32 *)data_ptr;
		length = *(uint32 *)(data_ptr + 4); /* 4 bytes of length */
		data_ptr += 8; /* 8 bytes of cmd and length */
		if (data_ptr != NULL)
			drv_soc_driver_event_process_internal(drv, data_ptr, cmd, length);
		os_free(packet); /* release the packet */
		packet = NULL;
	}

	return;
}

static void drv_soc_driver_ap_event_process(void *eloop_data, void *user_ctx)
{
	/* p_user_ctx may be NULL */
	if (eloop_data == NULL)
		return;
	drv_soc_driver_event_process(eloop_data, user_ctx);
}

static void drv_soc_rx_mgmt_count_decrease(void)
{
	unsigned int int_save;

	os_intlock(&int_save);
	g_rx_mgmt_count--;
	if (g_rx_mgmt_count < 0)
		g_rx_mgmt_count = 0;
	os_intrestore(int_save);
}

static void drv_soc_rx_mgmt_process(void *ctx, ext_rx_mgmt_stru *rx_mgmt, union wpa_event_data *event)
{
	drv_soc_rx_mgmt_count_decrease();
	if ((ctx == NULL) || (rx_mgmt == NULL) || (event == NULL))
		return;
	if (rx_mgmt->buf == NULL)
		return;
	event->rx_mgmt.frame = rx_mgmt->buf;
	event->rx_mgmt.frame_len = rx_mgmt->len;
	event->rx_mgmt.ssi_signal = rx_mgmt->sig_mbm;
	event->rx_mgmt.freq = rx_mgmt->freq;

	wpa_supplicant_event(ctx, EVENT_RX_MGMT, event);

	if (rx_mgmt->buf != NULL) {
		os_free(rx_mgmt->buf);
		rx_mgmt->buf = NULL;
	}
	return;
}

static ext_driver_data_stru * drv_soc_drv_init(void *ctx, const struct wpa_init_params *params)
{
	uint8 addr_tmp[ETH_ALEN] = { 0 };
	ext_driver_data_stru *drv = EXT_PTR_NULL;
	errno_t rc;

	if ((ctx == NULL) || (params == NULL) || (params->ifname == NULL) || (params->own_addr == NULL))
		return NULL;
	drv = os_zalloc(sizeof(ext_driver_data_stru));
	if (drv == NULL)
		goto INIT_FAILED;

	drv->ctx = ctx;
	rc = memcpy_s((int8 *)drv->iface, sizeof(drv->iface), params->ifname, strlen(params->ifname));
	if (rc != EOK)
		goto INIT_FAILED;

	/*  just for ap drv init */
	if ((eloop_register_event(&drv->event_queue, sizeof(void *),
	                          drv_soc_driver_ap_event_process, drv, NULL)) != EXT_SUCC)
		goto INIT_FAILED;

	drv->eapol_sock = l2_packet_init((char *)drv->iface, NULL, ETH_P_EAPOL, drv_soc_receive_eapol, drv, 1);
	if (drv->eapol_sock == NULL)
		goto INIT_FAILED;

#ifdef CONFIG_WAPI
	if (l2_packet_get_own_addr(drv->eapol_sock, addr_tmp, sizeof(addr_tmp)))
#else
	if (l2_packet_get_own_addr(drv->eapol_sock, addr_tmp))
#endif /* CONFIG_WAPI */
		goto INIT_FAILED;

	/* The mac address is passed to the hostapd data structure for hostapd startup */
	rc = memcpy_s(params->own_addr, ETH_ALEN, addr_tmp, ETH_ALEN);
	if (rc != EOK)
		goto INIT_FAILED;
	rc = memcpy_s(drv->own_addr, ETH_ALEN, addr_tmp, ETH_ALEN);
	if (rc != EOK)
		goto INIT_FAILED;

	return drv;
INIT_FAILED:
	if (drv != NULL) {
		if (drv->eapol_sock != NULL) {
			l2_packet_deinit(drv->eapol_sock);
			drv->eapol_sock = NULL;
		}
		os_free(drv);
	}
	return NULL;
}

#define EXT_MEM_FREE(type, element) do { \
		if ((type)->element != NULL) { \
			os_free((type)->element); \
			(type)->element = NULL; \
		} \
	} while (0)

/* release wifi drive unreleased memory */
static void drv_soc_drv_remove_mem(const ext_driver_data_stru *drv)
{
	ext_new_sta_info_stru *new_sta_info = NULL;
	ext_rx_mgmt_stru *rx_mgmt = NULL;
	ext_scan_result_stru *scan_result = NULL;
	ext_connect_result_stru *accoc_info = NULL;
	ext_disconnect_stru *discon_info = NULL;
	ext_tx_status_stru *tx_status = NULL;
	void *packet = NULL;
	int8 *data_ptr = NULL;
	int32 l_cmd;
	for (;;) {
		packet = eloop_read_event(drv->event_queue, 0);
		if (packet == NULL)
			return;
		/* setup a pointer to the payload */
		data_ptr = packet;
		l_cmd    = *(int32 *)data_ptr;
		data_ptr += 8; /* 8 bytes of cmd and length */
		/* decode the buffer */
		switch (l_cmd) {
			case EXT_ELOOP_EVENT_NEW_STA:
				new_sta_info = (ext_new_sta_info_stru *)data_ptr;
				EXT_MEM_FREE(new_sta_info, ie);
				break;
			case EXT_ELOOP_EVENT_RX_MGMT:
				drv_soc_rx_mgmt_count_decrease();
				rx_mgmt = (ext_rx_mgmt_stru *)data_ptr;
				EXT_MEM_FREE(rx_mgmt, buf);
				break;
			case EXT_ELOOP_EVENT_TX_STATUS:
				tx_status = (ext_tx_status_stru *)data_ptr;
				EXT_MEM_FREE(tx_status, buf);
				break;
			case EXT_ELOOP_EVENT_SCAN_RESULT:
				scan_result = (ext_scan_result_stru *)data_ptr;
				EXT_MEM_FREE(scan_result, variable);
				break;
			case EXT_ELOOP_EVENT_CONNECT_RESULT:
				accoc_info = (ext_connect_result_stru *)data_ptr;
				EXT_MEM_FREE(accoc_info, req_ie);
				EXT_MEM_FREE(accoc_info, resp_ie);
				break;
			case EXT_ELOOP_EVENT_DISCONNECT:
				discon_info = (ext_disconnect_stru *)data_ptr;
				EXT_MEM_FREE(discon_info, ie);
				break;
			default:
				break;
		}
		os_free(packet);
		packet = NULL;
	}
}

static void * drv_soc_hapd_init(struct hostapd_data *hapd, struct wpa_init_params *params)
{
	ext_driver_data_stru *drv = NULL;
	ext_bool_enum_uint8 status;
	int32 ret;
	int32 send_event_cb_reg_flag;

	if ((hapd == NULL) || (params == NULL) || (hapd->conf == NULL))
		return NULL;

	drv = drv_soc_drv_init(hapd, params);
	if (drv == NULL)
		return NULL;

	send_event_cb_reg_flag = drv_soc_register_send_event_cb(drv_soc_driver_send_event);
	drv->hapd = hapd;
	status = true;
	/* set netdev open or stop */
	ret = drv_soc_ioctl_set_netdev(drv->iface, &status);
	if (ret != EXT_SUCC)
		goto INIT_FAILED;
	return (void *)drv;
INIT_FAILED:
	if (send_event_cb_reg_flag == EXT_SUCC)
		(void)drv_soc_register_send_event_cb(NULL);
	drv_soc_drv_deinit(drv);
	return NULL;
}

static void drv_soc_drv_deinit(void *priv)
{
	ext_driver_data_stru   *drv = EXT_PTR_NULL;
    uint16 num;

	if (priv == NULL)
		return;
	drv = (ext_driver_data_stru *)priv;
	if (drv->eapol_sock != NULL) {
		l2_packet_deinit(drv->eapol_sock);
		drv->eapol_sock = NULL;
	}

    num = ARRAY_SIZE(drv->res);
    if (num > drv->scan_ap_num) {
        num = drv->scan_ap_num;
    }
	
    for (uint32 i = 0; i < num; i++) {
		os_free(drv->res[i]);
		drv->res[i] = NULL;
	}
	drv_soc_drv_remove_mem(drv);
	eloop_unregister_event(drv->event_queue, 0); // 0:useless
	drv->event_queue = NULL;
	os_free(drv);
}

static void drv_soc_hapd_deinit(void *priv)
{
	ext_driver_data_stru *drv = EXT_PTR_NULL;
	ext_set_mode_stru set_mode;
	ext_bool_enum_uint8 status;

	if (priv == NULL)
		return;

	(void)memset_s(&set_mode, sizeof(ext_set_mode_stru), 0, sizeof(ext_set_mode_stru));
	drv = (ext_driver_data_stru *)priv;
	set_mode.iftype = EXT_WIFI_IFTYPE_STATION;
	status = EXT_FALSE;

	if ((drv_soc_ioctl_set_netdev(drv->iface, &status) != EXT_SUCC) ||
	    (drv_soc_ioctl_set_mode(drv->iface, &set_mode) != EXT_SUCC))
		wpa_error_log0(MSG_ERROR, "drv_soc_hapd_deinit , ext_ioctl_set fail.");

	if (los_count_wifi_dev_in_use() >= WPA_DOUBLE_IFACE_WIFI_DEV_NUM)
		wpa_error_log0(MSG_DEBUG, "wifidev in use");
	else
		(void)drv_soc_register_send_event_cb(NULL);

	drv_soc_drv_deinit(priv);
}

static void drv_soc_hw_feature_data_free(struct hostapd_hw_modes *modes, uint16 modes_num)
{
	uint16 loop;

	if (modes == NULL)
		return;
	for (loop = 0; loop < modes_num; ++loop) {
		if (modes[loop].channels != NULL) {
			os_free(modes[loop].channels);
			modes[loop].channels = NULL;
		}
		if (modes[loop].rates != NULL) {
			os_free(modes[loop].rates);
			modes[loop].rates = NULL;
		}
	}
	os_free(modes);
}

static struct hostapd_hw_modes * drv_soc_get_hw_feature_data(void *priv, uint16 *num_modes, uint16 *flags)
{
	struct modes modes_data[] = { { 12, (int)HOSTAPD_MODE_IEEE80211G }, { 4, (int)HOSTAPD_MODE_IEEE80211B } };
	size_t loop;
	uint32 index;
	ext_hw_feature_data_stru hw_feature_data = {0};

	if ((priv == NULL) || (num_modes == NULL) || (flags == NULL))
		return NULL;
	ext_driver_data_stru *drv = (ext_driver_data_stru *)priv;
	*num_modes = 2; /* 2: mode only for 11b + 11g */
	*flags     = 0;

	if (drv_soc_ioctl_get_hw_feature(drv->iface, &hw_feature_data) != EXT_SUCC)
		return NULL;

	struct hostapd_hw_modes *modes = os_calloc(*num_modes, sizeof(struct hostapd_hw_modes));
	if (modes == NULL)
		return NULL;

	for (loop = 0; loop < *num_modes; ++loop) {
		modes[loop].channels = NULL;
		modes[loop].rates    = NULL;
	}

	modes[0].ht_capab = hw_feature_data.ht_capab;
	for (index = 0; index < sizeof(modes_data) / sizeof(struct modes); index++) {
		modes[index].mode         = modes_data[index].mode;
		modes[index].num_channels = hw_feature_data.channel_num;
		modes[index].num_rates    = modes_data[index].modes_num_rates;
		modes[index].channels = os_calloc((size_t)hw_feature_data.channel_num, sizeof(struct hostapd_channel_data));
		modes[index].rates    = os_calloc((size_t)modes[index].num_rates, sizeof(int));
		if ((modes[index].channels == NULL) || (modes[index].rates == NULL)) {
			drv_soc_hw_feature_data_free(modes, *num_modes);
			return NULL;
		}

		for (loop = 0; loop < (size_t)hw_feature_data.channel_num; loop++) {
			modes[index].channels[loop].chan = (int16)hw_feature_data.iee80211_channel[loop].channel;
			modes[index].channels[loop].freq = (int32)hw_feature_data.iee80211_channel[loop].freq;
			modes[index].channels[loop].flag = (int32)hw_feature_data.iee80211_channel[loop].flags;
#if !defined(EXT_CODE_CROP) || defined(CONFIG_ACS)
			modes[index].channels[loop].allowed_bw = HOSTAPD_CHAN_WIDTH_20;
#endif
		}

		for (loop = 0; loop < (size_t)modes[index].num_rates; loop++)
			modes[index].rates[loop] = (int32)hw_feature_data.bitrate[loop];
	}
	return modes;
}

static void * drv_soc_wpa_init(void *ctx, const char *ifname, void *global_priv)
{
	int32 ret;
	int32 send_event_cb_reg_flag = EXT_WIFI_FAIL;
	ext_bool_enum_uint8 status = EXT_TRUE;

	(void)global_priv;
	if ((ctx == NULL) || (ifname == NULL))
		return NULL;

	ext_driver_data_stru *drv = os_zalloc(sizeof(ext_driver_data_stru));
	if (drv == NULL)
		goto INIT_FAILED;
	drv->ctx = ctx;
	if (memcpy_s((int8 *)drv->iface, sizeof(drv->iface), ifname, strlen(ifname)) != EOK)
		goto INIT_FAILED;
	send_event_cb_reg_flag = drv_soc_register_send_event_cb(drv_soc_driver_send_event);
	ret = eloop_register_event(&drv->event_queue, sizeof(void *), drv_soc_driver_event_process, drv, NULL);
	if (ret != EXT_SUCC)
		goto INIT_FAILED;

	drv->eapol_sock = l2_packet_init((char *)drv->iface, NULL, ETH_P_EAPOL, drv_soc_receive_eapol, drv, 1);
	if (drv->eapol_sock == NULL)
		goto INIT_FAILED;

#ifdef CONFIG_WAPI
	if (l2_packet_get_own_addr(drv->eapol_sock, drv->own_addr, sizeof(drv->own_addr)))
#else
	if (l2_packet_get_own_addr(drv->eapol_sock, drv->own_addr))
#endif /* CONFIG_WAPI */
		goto INIT_FAILED;

	/* set netdev open or stop */
	ret = drv_soc_ioctl_set_netdev(drv->iface, &status);
	if (ret != EXT_SUCC)
		goto INIT_FAILED;
	return drv;

INIT_FAILED:
	if (send_event_cb_reg_flag == EXT_SUCC)
		(void)drv_soc_register_send_event_cb(NULL);
	drv_soc_drv_deinit(drv);
	return NULL;
}

static void drv_soc_wpa_deinit(void *priv)
{
	ext_bool_enum_uint8 status;
	int32 ret;
	ext_driver_data_stru *drv = EXT_PTR_NULL;

	if (priv == NULL)
		return;

	status = EXT_FALSE;
	drv = (ext_driver_data_stru *)priv;
	(void)eloop_cancel_timeout(drv_soc_driver_scan_timeout, drv, drv->ctx);

	ret = drv_soc_ioctl_set_netdev(drv->iface, &status);
	if (ret != EXT_SUCC)
		wpa_error_log0(MSG_DEBUG, "drv_soc_wpa_deinit, close netdev fail");

	if (los_count_wifi_dev_in_use() >= WPA_DOUBLE_IFACE_WIFI_DEV_NUM)
		wpa_warning_log0(MSG_DEBUG, "wifidev in use");
	else
		(void)drv_soc_register_send_event_cb(NULL);

	drv_soc_drv_deinit(priv);
}

static void drv_soc_driver_scan_timeout(void *eloop_ctx, void *timeout_ctx)
{
	(void)eloop_ctx;
	if (timeout_ctx == NULL)
		return;
	wpa_supplicant_event(timeout_ctx, EVENT_SCAN_RESULTS, NULL);
}

static void drv_soc_scan_free(ext_scan_stru *scan_params)
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

	if (scan_params->extra_ies != NULL) {
		os_free(scan_params->extra_ies);
		scan_params->extra_ies = NULL;
	}

	if (scan_params->freqs != NULL) {
		os_free(scan_params->freqs);
		scan_params->freqs = NULL;
	}

	os_free(scan_params);
	scan_params = NULL;
}

static int drv_soc_scan_process_ssid(struct wpa_driver_scan_params *params, ext_scan_stru *scan_params)
{
	errno_t rc;
	size_t loop;
#ifdef CONFIG_ACS
	if (params->acs_scan_flag)
		return (params->num_ssids == 0) ? EXT_SUCC : (-EXT_EFAIL);
#endif
	if (params->num_ssids == 0)
		return -EXT_EFAIL;

	scan_params->num_ssids = (uint8)params->num_ssids;
	/* scan_params->ssids freed by drv_soc_scan_free */
	scan_params->ssids = (ext_driver_scan_ssid_stru *)os_zalloc(sizeof(ext_driver_scan_ssid_stru) * params->num_ssids);
	if (scan_params->ssids == NULL)
		return -EXT_EFAIL;

	for (loop = 0; (loop < params->num_ssids) && (loop < WPAS_MAX_SCAN_SSIDS); loop++) {
		wpa_hexdump_ascii(MSG_MSGDUMP, "soc: Scan SSID", params->ssids[loop].ssid, params->ssids[loop].ssid_len);

		if (params->ssids[loop].ssid_len > MAX_SSID_LEN) {
			wpa_error_log3(MSG_ERROR, "params->ssids[%zu].ssid_len is %zu, num %zu:",
			               loop, params->ssids[loop].ssid_len, params->num_ssids);
			params->ssids[loop].ssid_len = MAX_SSID_LEN;
		}
		if (params->ssids[loop].ssid_len) {
			rc = memcpy_s(scan_params->ssids[loop].ssid, MAX_SSID_LEN,
			              params->ssids[loop].ssid, params->ssids[loop].ssid_len);
			if (rc != EOK)
				return -EXT_EFAIL;
		}
		scan_params->ssids[loop].ssid_len = params->ssids[loop].ssid_len;
	}

	return EXT_SUCC;
}

static int drv_soc_scan_process_bssid(const struct wpa_driver_scan_params *params, ext_scan_stru *scan_params)
{
	errno_t rc;
	if (params->bssid != NULL) {
		/* scan_params->bssid freed by drv_soc_scan_free */
		scan_params->bssid = (uint8 *)os_zalloc(ETH_ALEN);
		if (scan_params->bssid == NULL)
			return -EXT_EFAIL;
		rc = memcpy_s(scan_params->bssid, ETH_ALEN, params->bssid, ETH_ALEN);
		if (rc != EOK) {
			return -EXT_EFAIL;
		}
	}
	return EXT_SUCC;
}

static int drv_soc_scan_process_extra_ies(const struct wpa_driver_scan_params *params, ext_scan_stru *scan_params)
{
	errno_t rc;
	if ((params->extra_ies != NULL) && (params->extra_ies_len != 0)) {
		wpa_hexdump(MSG_MSGDUMP, "soc: Scan extra IEs", params->extra_ies, params->extra_ies_len);
		/* scan_params->extra_ies freed by drv_soc_scan_free */
		scan_params->extra_ies = (uint8 *)os_zalloc(params->extra_ies_len);
		if (scan_params->extra_ies == NULL)
			return -EXT_EFAIL;

		rc = memcpy_s(scan_params->extra_ies, params->extra_ies_len, params->extra_ies, params->extra_ies_len);
		if (rc != EOK) {
			return -EXT_EFAIL;
		}
		scan_params->extra_ies_len = (int32)params->extra_ies_len;
	}
	return EXT_SUCC;
}

static int drv_soc_scan_process_freq(const struct wpa_driver_scan_params *params, ext_scan_stru *scan_params)
{
	uint32 num_freqs;
	int32 *freqs = NULL;
	errno_t rc;

	if (params->freqs != NULL) {
		num_freqs = 0;
		freqs = params->freqs;

		/* Calculate the number of channels, non-zero is a valid value */
		for (; *freqs != 0; freqs++) {
			num_freqs++;
			if (num_freqs > 14) /* 14 is the max channel number */
				return -EXT_EFAIL;
		}
		scan_params->num_freqs = (uint8)num_freqs;
		/* scan_params->freqs freed by drv_soc_scan_free */
		scan_params->freqs = (int32 *)os_zalloc(num_freqs * (sizeof(int32)));
		if (scan_params->freqs == NULL)
			return -EXT_EFAIL;
		rc = memcpy_s(scan_params->freqs, num_freqs * (sizeof(int32)), params->freqs, num_freqs * (sizeof(int32)));
		if (rc != EOK) {
			return -EXT_EFAIL;
		}
	}
	return EXT_SUCC;
}

static int32 drv_soc_scan(void *priv, struct wpa_driver_scan_params *params)
{
	ext_scan_stru *scan_params = NULL;
	ext_driver_data_stru *drv = NULL;
	int32 timeout;
	int32 ret;

	if ((priv == NULL) || (params == NULL) || (params->num_ssids > WPAS_MAX_SCAN_SSIDS))
		return -EXT_EFAIL;
	drv = (ext_driver_data_stru *)priv;
	scan_params = (ext_scan_stru *)os_zalloc(sizeof(ext_scan_stru));
	if (scan_params == NULL)
		return -EXT_EFAIL;

	if ((drv_soc_scan_process_ssid(params, scan_params) != EXT_SUCC) ||
	    (drv_soc_scan_process_bssid(params, scan_params) != EXT_SUCC) ||
	    (drv_soc_scan_process_extra_ies(params, scan_params) != EXT_SUCC) ||
	    (drv_soc_scan_process_freq(params, scan_params) != EXT_SUCC)) {
		drv_soc_scan_free(scan_params);
		return -EXT_EFAIL;
	}
	scan_params->fast_connect_flag = g_fast_connect_flag ? WPA_FLAG_ON : WPA_FLAG_OFF;
	scan_params->prefix_ssid_scan_flag = g_ssid_prefix_flag;
#ifdef CONFIG_ACS
	scan_params->acs_scan_flag = params->acs_scan_flag;
#endif
	drv->scan_ap_num = 0; /* 初始化 */
	wpa_error_log1(MSG_ERROR, "prefix_ssid_scan_flag = %u", scan_params->prefix_ssid_scan_flag);
	ret = drv_soc_ioctl_scan(drv->iface, scan_params);
	drv_soc_scan_free(scan_params);

	timeout = SCAN_TIME_OUT;
	if (drv->ctx != NULL) {
		(void)eloop_cancel_timeout(drv_soc_driver_scan_timeout, drv, drv->ctx);
		(void)eloop_register_timeout((unsigned int)timeout, 0, drv_soc_driver_scan_timeout, drv, drv->ctx);
	}
	return ret;
}

static int32 drv_soc_wifi_app_service_handle(void *priv, void *params)
{
    int32 ret;
    ext_driver_data_stru *drv = NULL;

    if ((priv == NULL) || (params == NULL)) {
        return -EXT_EFAIL;
    }

    drv = (ext_driver_data_stru *)priv;
    return drv_soc_ioctl_wifi_app_service_handle(drv->iface, params);
}

/*****************************************************************************
* Name         : drv_soc_get_scan_results
* Description  : get scan results
* Input param  : void *priv
* Return value : struct wpa_scan_results *
*****************************************************************************/
__attribute__((weak)) struct wpa_scan_results * drv_soc_get_scan_results(void *priv)
{
	struct wpa_scan_results *results = EXT_PTR_NULL;
	ext_driver_data_stru *drv = priv;
	uint32 loop;
	errno_t rc;

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
	results->res = (struct wpa_scan_res **)os_zalloc(results->num * sizeof(struct wpa_scan_res *));
	if (results->res == NULL) {
		os_free(results);
		return NULL;
	}
	rc = memcpy_s(results->res, results->num * sizeof(struct wpa_scan_res *),
	              drv->res, results->num * sizeof(struct wpa_scan_res *));
	if (rc != EOK) {
		os_free(results->res);
		os_free(results);
		return NULL;
	}
	drv->scan_ap_num = 0;
	for (loop = 0; loop < SCAN_AP_LIMIT; loop++)
		drv->res[loop] = NULL;
	wpa_warning_log1(MSG_DEBUG, "Received scan results (%u BSSes)", (uint32)results->num);
	return results;
}

/*****************************************************************************
* Name         : drv_soc_cipher_to_cipher_suite
* Description  : get cipher suite from cipher
* Input param  : uint32 cipher
* Return value : uint32
*****************************************************************************/
static uint32 drv_soc_cipher_to_cipher_suite(uint32 cipher)
{
	switch (cipher) {
		case WPA_CIPHER_CCMP_256:
			return RSN_CIPHER_SUITE_CCMP_256;
		case WPA_CIPHER_GCMP_256:
			return RSN_CIPHER_SUITE_GCMP_256;
		case WPA_CIPHER_CCMP:
			return RSN_CIPHER_SUITE_CCMP;
		case WPA_CIPHER_GCMP:
			return RSN_CIPHER_SUITE_GCMP;
		case WPA_CIPHER_TKIP:
			return RSN_CIPHER_SUITE_TKIP;
		case WPA_CIPHER_WEP104:
			return RSN_CIPHER_SUITE_WEP104;
		case WPA_CIPHER_WEP40:
			return RSN_CIPHER_SUITE_WEP40;
		case WPA_CIPHER_GTK_NOT_USED:
			return RSN_CIPHER_SUITE_NO_GROUP_ADDRESSED;
		default:
			return 0;
	}
}

static void drv_soc_set_conn_keys(const struct wpa_driver_associate_params *wpa_params,
                                  ext_associate_params_stru *params)
{
	int32 loop;
	uint8 privacy = 0; /* The initial value means unencrypted */
	errno_t rc;

	if ((wpa_params == NULL) || (params == NULL))
		return;

	for (loop = 0; loop < 4; loop++) { /* 4: the upper limit of loop */
		if (wpa_params->wep_key[loop] == NULL)
			continue;
		privacy = 1;
		break;
	}

	if ((wpa_params->wps == WPS_MODE_PRIVACY) ||
		((wpa_params->pairwise_suite != 0) && (wpa_params->pairwise_suite != WPA_CIPHER_NONE)))
		privacy = 1;
	if (privacy == 0)
		return;
	params->privacy = privacy;
	for (loop = 0; loop < 4; loop++) { /* 4: the upper limit of loop */
		if (wpa_params->wep_key[loop] == NULL)
			continue;

		params->key_len = (uint8)wpa_params->wep_key_len[loop];
		params->key = (uint8 *)os_zalloc(params->key_len);
		if (params->key == NULL)
			return;

		rc = memcpy_s(params->key, params->key_len, wpa_params->wep_key[loop], wpa_params->wep_key_len[loop]);
		if (rc != EOK) {
			os_free(params->key);
			params->key = NULL;
			return;
		}
		params->key_idx = (uint8)wpa_params->wep_tx_keyidx;
		break;
	}

	return;
}

static void drv_soc_connect_free(ext_associate_params_stru *assoc_params)
{
	if (assoc_params == NULL)
		return;

	if (assoc_params->ie != NULL) {
		os_free(assoc_params->ie);
		assoc_params->ie = NULL;
	}
	if (assoc_params->crypto != NULL) {
		os_free(assoc_params->crypto);
		assoc_params->crypto = NULL;
	}
	if (assoc_params->ssid != NULL) {
		os_free(assoc_params->ssid);
		assoc_params->ssid = NULL;
	}
	if (assoc_params->bssid != NULL) {
		os_free(assoc_params->bssid);
		assoc_params->bssid = NULL;
	}
	if (assoc_params->key != NULL) {
		(void)memset_s(assoc_params->key, assoc_params->key_len, 0, assoc_params->key_len);
		os_free(assoc_params->key);
		assoc_params->key = NULL;
	}

	os_free(assoc_params);
	assoc_params = NULL;
}


static int drv_soc_assoc_params_set(ext_driver_data_stru *drv,
                                    struct wpa_driver_associate_params *params,
                                    ext_associate_params_stru *assoc_params)
{
	if (params->bssid != NULL) {
		assoc_params->bssid = (uint8 *)os_zalloc(ETH_ALEN); /* freed by drv_soc_connect_free */
		if (assoc_params->bssid == NULL)
			return -EXT_EFAIL;

		if (memcpy_s(assoc_params->bssid, ETH_ALEN, params->bssid, ETH_ALEN) != EOK)
			return -EXT_EFAIL;
	}

	if (params->freq.freq != 0)
		assoc_params->freq = (uint32)params->freq.freq;

	assoc_params->auto_conn = (uint8)g_sta_delay_report_flag;

	if (params->ssid_len > MAX_SSID_LEN)
		params->ssid_len = MAX_SSID_LEN;

	if ((params->ssid != NULL) && (params->ssid_len != 0)) {
		assoc_params->ssid = (uint8 *)os_zalloc(params->ssid_len); /* freed by drv_soc_connect_free */
		if (assoc_params->ssid == NULL)
			return -EXT_EFAIL;
		assoc_params->ssid_len = params->ssid_len;
		if (memcpy_s(assoc_params->ssid, assoc_params->ssid_len, params->ssid, params->ssid_len) != EOK)
			return -EXT_EFAIL;
		if (memset_s(drv->ssid, MAX_SSID_LEN, 0, MAX_SSID_LEN) != EOK)
			return -EXT_EFAIL;
		if (memcpy_s(drv->ssid, MAX_SSID_LEN, params->ssid, params->ssid_len) != EOK)
			return -EXT_EFAIL;
		drv->ssid_len = params->ssid_len;
	}

	if ((params->wpa_ie != NULL) && (params->wpa_ie_len != 0)) {
		assoc_params->ie = (uint8 *)os_zalloc(params->wpa_ie_len); /* freed by drv_soc_connect_free */
		if (assoc_params->ie == NULL)
			return -EXT_EFAIL;
		assoc_params->ie_len = params->wpa_ie_len;
		if (memcpy_s(assoc_params->ie, assoc_params->ie_len, params->wpa_ie, params->wpa_ie_len) != EOK)
			return -EXT_EFAIL;
	}

	return EXT_SUCC;
}

static int drv_soc_assoc_param_crypto_set(const struct wpa_driver_associate_params *params,
                                          ext_associate_params_stru *assoc_params)
{
	unsigned int wpa_ver = 0;
	uint32 akm_suites_num = 0;
	uint32 ciphers_pairwise_num = 0;
	uint32 mgmt = RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X;
	/* assoc_params->crypto freed by drv_soc_connect_free */
	assoc_params->crypto = (ext_crypto_settings_stru *)os_zalloc(sizeof(ext_crypto_settings_stru));
	if (assoc_params->crypto == NULL)
		return -EXT_EFAIL;

	assoc_params->crypto->sae_pwe = drv_soc_set_sae_pwe(params->sae_pwe);

	if (params->wpa_proto != 0) {
		if (params->wpa_proto & WPA_PROTO_WPA)
			wpa_ver |= (unsigned int)EXT_WPA_VERSION_1;
		if (params->wpa_proto & WPA_PROTO_RSN)
			wpa_ver |= (unsigned int)EXT_WPA_VERSION_2;
		assoc_params->crypto->wpa_versions = wpa_ver;
	}

	if (params->pairwise_suite != WPA_CIPHER_NONE) {
		assoc_params->crypto->ciphers_pairwise[ciphers_pairwise_num++]
			= drv_soc_cipher_to_cipher_suite(params->pairwise_suite);
		assoc_params->crypto->n_ciphers_pairwise = (int32)ciphers_pairwise_num;
	}

	if (params->group_suite != WPA_CIPHER_NONE)
		assoc_params->crypto->cipher_group = drv_soc_cipher_to_cipher_suite(params->group_suite);

	if (params->key_mgmt_suite == WPA_KEY_MGMT_PSK ||
	    params->key_mgmt_suite == WPA_KEY_MGMT_SAE ||
#ifdef CONFIG_IEEE80211R
	    params->key_mgmt_suite == WPA_KEY_MGMT_FT_PSK ||
        params->key_mgmt_suite == WPA_KEY_MGMT_FT_SAE ||
#endif /* CONFIG_IEEE80211R */
#ifdef LOS_CONFIG_WPA_ENTERPRISE
	    params->key_mgmt_suite == WPA_KEY_MGMT_IEEE8021X ||
#endif /* LOS_CONFIG_WPA_ENTERPRISE */
	    params->key_mgmt_suite == WPA_KEY_MGMT_PSK_SHA256) {
		switch (params->key_mgmt_suite) {
			case WPA_KEY_MGMT_PSK_SHA256:
				mgmt = RSN_AUTH_KEY_MGMT_PSK_SHA256;
				break;
			case WPA_KEY_MGMT_SAE:
				mgmt = RSN_AUTH_KEY_MGMT_SAE;
				break;
#ifdef CONFIG_IEEE80211R
			case WPA_KEY_MGMT_FT_PSK:
				mgmt = RSN_AUTH_KEY_MGMT_FT_PSK;
				break;
            case WPA_KEY_MGMT_FT_SAE:
                mgmt = RSN_AUTH_KEY_MGMT_FT_SAE;
                break;
#endif /* CONFIG_IEEE80211R */
#ifdef LOS_CONFIG_WPA_ENTERPRISE
			case WPA_KEY_MGMT_IEEE8021X:
				mgmt = RSN_AUTH_KEY_MGMT_UNSPEC_802_1X;
				break;
#endif /* LOS_CONFIG_WPA_ENTERPRISE */
			case WPA_KEY_MGMT_PSK: /* fall through */
			default:
				mgmt = RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X;
				break;
		}
		assoc_params->crypto->akm_suites[akm_suites_num++] = mgmt;
		assoc_params->crypto->n_akm_suites = (int32)akm_suites_num;
	}

	return EXT_SUCC;
}

static int32 drv_soc_try_connect(ext_driver_data_stru *drv, struct wpa_driver_associate_params *params)
{
	ext_associate_params_stru *assoc_params = EXT_PTR_NULL;
	ext_auth_type_enum type = EXT_AUTHTYPE_BUTT;
	int32 algs = 0;

	if ((drv == NULL) || (params == NULL))
		return -EXT_EFAIL;

	assoc_params = (ext_associate_params_stru *)os_zalloc(sizeof(ext_associate_params_stru));
	if (assoc_params == NULL)
		goto failure;

	if (drv_soc_assoc_params_set(drv, params, assoc_params) != EXT_SUCC)
		goto failure;

	if (drv_soc_assoc_param_crypto_set(params, assoc_params) != EXT_SUCC)
		goto failure;

	assoc_params->mfp = (uint8)(params->mgmt_frame_protection);
	wpa_error_log1(MSG_ERROR, "soc try connect: pmf = %u", assoc_params->mfp);

	if ((unsigned int)(params->auth_alg) & WPA_AUTH_ALG_OPEN)
		algs++;
	if ((unsigned int)(params->auth_alg) & WPA_AUTH_ALG_SHARED)
		algs++;
	if ((unsigned int)(params->auth_alg) & WPA_AUTH_ALG_LEAP)
		algs++;
	if (algs > 1) {
		assoc_params->auth_type = EXT_AUTHTYPE_AUTOMATIC;
		goto skip_auth_type;
	}

	if ((unsigned int)params->auth_alg & WPA_AUTH_ALG_OPEN)
		type = EXT_AUTHTYPE_OPEN_SYSTEM;
	else if ((unsigned int)params->auth_alg & WPA_AUTH_ALG_SHARED)
		type = EXT_AUTHTYPE_SHARED_KEY;
	else if ((unsigned int)params->auth_alg & WPA_AUTH_ALG_LEAP)
		type = EXT_AUTHTYPE_NETWORK_EAP;
	else if ((unsigned int)params->auth_alg & WPA_AUTH_ALG_FT)
		type = EXT_AUTHTYPE_FT;
	else if ((unsigned int)params->auth_alg & WPA_AUTH_ALG_SAE)
		type = EXT_AUTHTYPE_SAE;
	else
		goto failure;

	assoc_params->auth_type = (uint8)type;

skip_auth_type:
	drv_soc_set_conn_keys(params, assoc_params);
	if (drv_soc_ioctl_assoc(drv->iface, assoc_params) != EXT_SUCC)
		goto failure;

	wpa_warning_log0(MSG_DEBUG, "Connect request send successfully");

	drv_soc_connect_free(assoc_params);
	return EXT_SUCC;

failure:
	drv_soc_connect_free(assoc_params);
	return -EXT_EFAIL;
}

static int32 drv_soc_connect(ext_driver_data_stru *drv, struct wpa_driver_associate_params *params)
{
	int32 ret;
	if ((drv == NULL) || (params == NULL))
		return -EXT_EFAIL;

	ret = drv_soc_try_connect(drv, params);
	if (ret != EXT_SUCC) {
		if (drv_soc_disconnect(drv, WLAN_REASON_PREV_AUTH_NOT_VALID))
			return -EXT_EFAIL;
		ret = drv_soc_try_connect(drv, params);
	}
	return ret;
}


static int32 drv_soc_associate(void *priv, struct wpa_driver_associate_params *params)
{
	int32 ret;
	ext_driver_data_stru *drv = priv;

	if ((drv == NULL) || (params == NULL))
		return -EXT_EFAIL;

#ifdef LOS_CONFIG_P2P
	ext_set_mode_stru set_mode = {0};

	(void)memcpy_s(set_mode.bssid, ETH_ADDR_LEN, drv->own_addr, ETH_ADDR_LEN);

	if (params->mode == (int32)IEEE80211_MODE_AP) {
		set_mode.iftype = params->p2p ? EXT_WIFI_IFTYPE_P2P_GO : EXT_WIFI_IFTYPE_AP;
		return drv_soc_ioctl_set_mode(drv->iface, &set_mode);
	}

	set_mode.iftype = params->p2p ? EXT_WIFI_IFTYPE_P2P_CLIENT : EXT_WIFI_IFTYPE_STATION;
	ret = drv_soc_ioctl_set_mode(drv->iface, &set_mode);
	if (ret != EXT_SUCC)
		return -EXT_EFAIL;
#endif /* LOS_CONFIG_P2P */

	return drv_soc_connect(drv, params);
}

static int32 drv_soc_disconnect(ext_driver_data_stru *drv, uint16 reason_code)
{
	int32 ret;
	uint16 new_reason_code;
	if (drv == NULL)
		return -EXT_EFAIL;
	new_reason_code = reason_code;
	ret = drv_soc_ioctl_disconnet(drv->iface, &new_reason_code);
	if (ret == EXT_SUCC)
		drv->associated = EXT_DISCONNECT;
	return ret;
}

static int32 drv_soc_deauthenticate(void *priv, const uint8 *addr, uint16 reason_code)
{
	ext_driver_data_stru *drv = priv;
	(void)addr;
	if (priv == NULL)
		return -EXT_EFAIL;
	return drv_soc_disconnect(drv, reason_code);
}

static int32 drv_soc_get_bssid(void *priv, uint8 *bssid)
{
	ext_driver_data_stru *drv = priv;
	errno_t rc;
	if ((priv == NULL) || (bssid == NULL))
		return -EXT_EFAIL;
	if (drv->associated == EXT_DISCONNECT)
		return -EXT_EFAIL;

	rc = memcpy_s(bssid, ETH_ALEN, drv->bssid, ETH_ALEN);
	if (rc != EOK)
		return -EXT_EFAIL;
	return EXT_SUCC;
}

static int32 drv_soc_get_ssid(void *priv, uint8 *ssid)
{
	ext_driver_data_stru *drv = priv;
	errno_t rc;
	if ((priv == NULL) || (ssid == NULL))
		return -EXT_EFAIL;
	if (drv->associated == EXT_DISCONNECT)
		return -EXT_EFAIL;
	rc = memcpy_s(ssid, MAX_SSID_LEN, drv->ssid, drv->ssid_len);
	if (rc != EOK)
		return -EXT_EFAIL;
	return (int32)drv->ssid_len;
}

#ifdef LOS_CONFIG_P2P
/*****************************************************************************
 函 数 名  : drv_soc_probe_req_report
 功能描述  : 获取probe_req
 修改历史      :
  1.日    期   : 2019年4月20日
    修改内容   : 新生成函数
*****************************************************************************/
static int32 drv_soc_probe_req_report(void *priv, int32 report)
{
	ext_driver_data_stru *drv = priv;

	if (priv == NULL)
		return -EXT_EFAIL;

	return drv_soc_ioctl_probe_request_report(drv->iface, &report);
}

/*****************************************************************************
 函 数 名  : drv_soc_remain_on_channel
 功能描述  : 保持在指定信道
 修改历史      :
  1.日    期   : 2019年4月20日
    修改内容   : 新生成函数
*****************************************************************************/
static int32 drv_soc_remain_on_channel(void *priv, uint32 freq, uint32 duration)
{
	int32 ret;
	ext_driver_data_stru *drv = priv;
	ext_on_channel_stru *params = NULL;

	if (priv == NULL)
		return -EXT_EFAIL;

	/* Send freq, duration to the driver layer */
	params = (ext_on_channel_stru *)os_zalloc(sizeof(ext_on_channel_stru));
	if (params == NULL)
		return -EXT_EFAIL;
	params->freq = freq;
	params->duration = duration;

	ret = drv_soc_ioctl_remain_on_channel(drv->iface, params);

	os_free(params);

	return ret;
}

/*****************************************************************************
 函 数 名  : drv_soc_cancel_remain_on_channel
 功能描述  : 取消在指定信道
 修改历史      :
  1.日    期   : 2019年4月20日
    修改内容   : 新生成函数
*****************************************************************************/
static int32 drv_soc_cancel_remain_on_channel(void *priv)
{
	ext_driver_data_stru *drv = priv;

	if (priv == NULL)
		return -EXT_EFAIL;

	return drv_soc_ioctl_cancel_remain_on_channel(drv->iface, "");
}

/*****************************************************************************
 函 数 名  : drv_soc_add_if
 功能描述  : 添加接口
 修改历史      :
  1.日    期   : 2019年5月20日
    修改内容   : 新生成函数
*****************************************************************************/
static int32 drv_soc_add_if(void *priv, enum wpa_driver_if_type type, const char *ifname, const u8 *addr,
                            void *bss_ctx, void **drv_priv, char *force_ifname, u8 *if_addr, const char *bridge,
                            int use_existing, int setup_ap)
{
	ext_driver_data_stru *drv = priv;
	ext_if_add_stru *params = NULL;
	int32 ret;

	(void)ifname;
	(void)addr;
	(void)bss_ctx;
	(void)drv_priv;
	(void)force_ifname;
	(void)if_addr;
	(void)bridge;
	(void)use_existing;
	(void)setup_ap;
	if (priv == NULL)
		return -EXT_EFAIL;

	/* Send freq, duration to the driver layer */
	params = (ext_if_add_stru *)os_zalloc(sizeof(ext_if_add_stru));
	if (params == NULL)
		return -EXT_EFAIL;

	switch (type) {
		case WPA_IF_STATION:
			params->type = EXT_WIFI_IFTYPE_STATION;
			break;
		case WPA_IF_P2P_GROUP:
		case WPA_IF_P2P_CLIENT:
			params->type = EXT_WIFI_IFTYPE_P2P_CLIENT;
			break;
		case WPA_IF_AP_VLAN:
			params->type = EXT_WIFI_IFTYPE_AP_VLAN;
			break;
		case WPA_IF_AP_BSS:
			params->type = EXT_WIFI_IFTYPE_AP;
			break;
		case WPA_IF_P2P_GO:
			params->type = EXT_WIFI_IFTYPE_P2P_GO;
			break;
		case WPA_IF_P2P_DEVICE:
			params->type = EXT_WIFI_IFTYPE_P2P_DEVICE;
			break;
		case WPA_IF_MESH:
			params->type = EXT_WIFI_IFTYPE_MESH_POINT;
			break;
		default:
			wpa_error_log0(MSG_ERROR, "drv_soc_add_if not support interface type!\n");
	}
	ret = drv_soc_ioctl_add_if(drv->iface, params);
	os_free(params);
	return ret;
}

/*****************************************************************************
 函 数 名  : drv_soc_remove_if
 功能描述  : 删除接口
 修改历史      :
  1.日    期   : 2019年7月20日
    修改内容   : 新生成函数
*****************************************************************************/
static int32 drv_soc_remove_if(void *priv, enum wpa_driver_if_type type, const char *ifname)
{
	ext_driver_data_stru *drv = priv;
	ext_if_remove_stru *params = NULL;
	int32 ret;
	errno_t rc;

	(void)type;
	if ((priv == NULL) || (ifname == NULL))
		return -EXT_EFAIL;

	if (strnlen(ifname, IFNAMSIZ) >= IFNAMSIZ)
		return -EXT_EFAIL;

	/* Send ifname, to the driver layer */
	params = (ext_if_remove_stru *)os_zalloc(sizeof(ext_if_remove_stru));
	if (params == NULL) {
		return -EXT_EFAIL;
	}
	rc = memcpy_s(params->ifname, IFNAMSIZ, ifname, os_strlen(ifname) + 1);
	if (rc != EOK) {
		os_free(params);
		return -EXT_EFAIL;
	}

	ret = drv_soc_ioctl_remove_if(drv->iface, params);
	os_free(params);
	return ret;
}

/*****************************************************************************
 函 数 名  : ext_p2p_get_mac_addr
 功能描述  : 获取p2p GO/GC MAC接口
 修改历史      :
  1.日    期   : 2019年8月6日
    修改内容   : 新生成函数
 *****************************************************************************/
static int32 drv_soc_get_p2p_mac_addr(void *priv, enum wpa_driver_if_type type, uint8 *mac_addr)
{
	ext_driver_data_stru *drv = NULL;
	ext_get_p2p_addr_stru *params = NULL;
	int32 ret;
	errno_t rc;

	if ((priv == NULL) || (mac_addr == NULL))
		return -EXT_EFAIL;

	drv = (ext_driver_data_stru *)priv;

	/* Send type to the driver layer */
	params = (ext_get_p2p_addr_stru *)os_zalloc(sizeof(ext_get_p2p_addr_stru));
	if (params == NULL)
		return -EXT_EFAIL;

	switch (type) {
		case WPA_IF_P2P_GROUP: /* fall-through */
		case WPA_IF_P2P_CLIENT:
			params->type = EXT_IFTYPE_P2P_CLIENT;
			break;
		case WPA_IF_P2P_GO:
			params->type = EXT_IFTYPE_P2P_GO;
			break;
		case WPA_IF_P2P_DEVICE:
			params->type = EXT_IFTYPE_P2P_DEVICE;
			break;
		default:
			os_free(params);
			return -EXT_EFAIL;
	}

	ret = drv_soc_ioctl_get_p2p_addr(drv->iface, params);
	if (ret != EXT_SUCC) {
		os_free(params);
		return -EXT_EFAIL;
	}
	rc = memcpy_s(mac_addr, ETH_ALEN, params->mac_addr, ETH_ALEN);
	if (rc != EOK) {
		os_free(params);
		return -EXT_EFAIL;
	}
	os_free(params);
	return EXT_SUCC;
}

#ifdef ANDROID_P2P
/*****************************************************************************
 功能描述  : 设置P2P的NOA节能方式
 修改历史
  1.日    期   : 2019年8月26日
    修改内容   : 新生成函数
 *****************************************************************************/
static int32 drv_soc_set_p2p_noa(void *priv, uint8 count, int32 start, int32 duration)
{
	ext_driver_data_stru *drv = NULL;
	ext_p2p_noa_stru *params = NULL;
	int32 ret;

	if (priv == NULL)
		return -EXT_EFAIL;

	drv = (ext_driver_data_stru *)priv;

	params = (ext_p2p_noa_stru *)os_zalloc(sizeof(ext_p2p_noa_stru));
	if (params == NULL) {
		return -EXT_EFAIL;
	}
	params->count = count;
	params->start = start;
	params->duration = duration;

	ret = drv_soc_ioctl_set_p2p_noa(drv->iface, params);
	if (ret != EXT_SUCC) {
		os_free(params);
		return -EXT_EFAIL;
	}

	os_free(params);
	return EXT_SUCC;
}
#endif

/*****************************************************************************
 功能描述  : 设置P2P的PowerSave节能方式
 修改历史
  1.日    期   : 2019年8月26日
    修改内容   : 新生成函数
 *****************************************************************************/
static int32 drv_soc_set_p2p_powersave(void *priv, int32 legacy_ps, int32 opp_ps, int32 ctwindow)
{
	ext_driver_data_stru *drv = NULL;
	ext_p2p_power_save_stru *params = NULL;
	int32 ret;

	if (priv == NULL)
		return -EXT_EFAIL;

	drv = (ext_driver_data_stru *)priv;

	params = (ext_p2p_power_save_stru *)os_zalloc(sizeof(ext_p2p_power_save_stru));
	if (params == NULL) {
		return -EXT_EFAIL;
	}
	params->legacy_ps = legacy_ps;
	/* opp_ps: 0 = disable, 1 = enable, -1 = no change */
	params->opp_ps = (int8)opp_ps;
	/* ctwindow: 0..254 = change (msec), -1 = no change */
	params->ctwindow = (uint8)ctwindow;

	ret = drv_soc_ioctl_set_p2p_powersave(drv->iface, params);
	if (ret != EXT_SUCC) {
		wpa_error_log0(MSG_ERROR, "drv_soc_set_p2p_powersave: drv_soc_ioctl_set_p2p_powersave failed.");
		os_free(params);
		return -EXT_EFAIL;
	}

	os_free(params);
	return EXT_SUCC;
}

#ifdef CONFIG_WPS_AP
/*****************************************************************************
 功能描述  : 设置app ie 到wifi驱动
 修改历史      :
  1.日    期   : 2016年7月12日
    作    者   : panhu
    修改内容   : 新生成函数
*****************************************************************************/
static int32 drv_soc_set_ap_wps_p2p_ie(void *priv, const struct wpabuf *beacon,
                                       const struct wpabuf *probresp, const struct wpabuf *assocresp)
{
	int32 loops;
	int32 ret = EXT_SUCC;
	ext_app_ie_stru *app_ie = NULL;
	struct wpabuf *wpabuf_tmp = NULL;
	ext_driver_data_stru *drv = (ext_driver_data_stru *)priv;
	errno_t rc;
	ext_cmd_stu cmd_arr[4] = { {0x0, beacon}, {0x2, probresp}, {0x4, assocresp}, {-1, NULL} };

	if ((priv == NULL) || ((beacon == NULL) && (probresp == NULL) && (assocresp == NULL)))
		return -EXT_EFAIL;

	app_ie = (ext_app_ie_stru *)os_zalloc(sizeof(ext_app_ie_stru));
	if (app_ie == NULL)
		return -EXT_EFAIL;

	/* 获取app ie结构体 */
	for (loops = 0; cmd_arr[loops].cmd != -1; loops++) {
		wpabuf_tmp = (struct wpabuf *)cmd_arr[loops].src;
		if (wpabuf_tmp == NULL)
			continue;
		/* 赋值app ie 长度和类型 */
		app_ie->app_ie_type = (uint8)cmd_arr[loops].cmd;
		app_ie->ie_len = wpabuf_len(wpabuf_tmp);
		if (app_ie->ie_len == 0)
			continue;

		if ((wpabuf_tmp->buf != NULL) && (app_ie->ie_len != 0)) {
			/* 赋值app ie 内容 */
			app_ie->ie = os_zalloc(app_ie->ie_len);
			if (app_ie->ie == NULL) {
				os_free(app_ie);
				return -EXT_EFAIL;
			}

			rc = memcpy_s(app_ie->ie, app_ie->ie_len,
						  wpabuf_head(wpabuf_tmp), wpabuf_len(wpabuf_tmp));
			if (rc != EOK) {
				os_free(app_ie->ie);
				os_free(app_ie);
				return -EXT_EFAIL;
			}
		}
		ret = drv_soc_ioctl_set_ap_wps_p2p_ie((const int8 *)drv->iface, (const void *)app_ie);
		os_free(app_ie->ie);
		app_ie->ie = NULL;

		if (ret < 0)
			break;
	}
	os_free(app_ie);
	app_ie = NULL;
	return ret;
}
#endif

static int drv_soc_sta_deauth(void *priv, const u8 *own_addr, const u8 *addr, u16 reason)
{
	struct i802_bss *bss = priv;
	struct ieee80211_mgmt mgmt;
	errno_t rc;

	if ((priv == NULL) || (own_addr == NULL) || (addr == NULL))
		return -EXT_EFAIL;

	(void)memset_s(&mgmt, sizeof(mgmt), 0, sizeof(mgmt));
	mgmt.frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT, WLAN_FC_STYPE_DEAUTH);
	rc = memcpy_s(mgmt.da, ETH_ALEN, addr, ETH_ALEN);
	if (rc != EOK)
		return -EXT_EFAIL;
	rc = memcpy_s(mgmt.sa, ETH_ALEN, own_addr, ETH_ALEN);
	if (rc != EOK)
		return -EXT_EFAIL;
	rc = memcpy_s(mgmt.bssid, ETH_ALEN, own_addr, ETH_ALEN);
	if (rc != EOK)
		return -EXT_EFAIL;

	mgmt.u.deauth.reason_code = host_to_le16(reason);
	return drv_soc_send_mlme(bss, (u8 *) &mgmt, IEEE80211_HDRLEN + sizeof(mgmt.u.deauth), 0, 0, NULL, 0);
}

static int drv_soc_deinit_ap(void *priv)
{
	ext_driver_data_stru *drv = priv;
	ext_set_mode_stru mode = {0};
	int32 ret;
	int32 tmp = 0;

	if (priv == NULL)
		return -EXT_EFAIL;

	ret = drv_soc_ioctl_del_beacon(drv->iface, &tmp);
	if (ret != EXT_SUCC) {
		wpa_error_log0(MSG_ERROR, "drv_soc_deinit_ap: drv_soc_ioctl_del_beacon failed.");
		return -EXT_EFAIL;
	}
	ret = drv_soc_ioctl_get_mode(drv->iface, &mode);
	if (ret != EXT_SUCC) {
		wpa_error_log0(MSG_ERROR, "drv_soc_deinit_ap: drv_soc_ioctl_get_mode failed.");
		return -EXT_EFAIL;
	}
	if (mode.iftype != (int)EXT_WIFI_IFTYPE_P2P_GO)
		return -EXT_EFAIL;
	mode.iftype = EXT_WIFI_IFTYPE_P2P_DEVICE;
	ret = drv_soc_ioctl_set_mode(drv->iface, &mode);
	if (ret != EXT_SUCC) {
		wpa_error_log0(MSG_ERROR, "drv_soc_deinit_ap: drv_soc_ioctl_set_mode failed.");
		return -EXT_EFAIL;
	}
	drv->beacon_set = EXT_FALSE;

	return EXT_SUCC;
}

static int drv_soc_deinit_p2p_cli(void *priv)
{
	ext_driver_data_stru *drv = priv;
	ext_set_mode_stru mode = {0};
	int32 ret;

	if (priv == NULL)
		return -EXT_EFAIL;

	ret = drv_soc_ioctl_get_mode(drv->iface, &mode);
	if (ret != EXT_SUCC) {
		wpa_error_log0(MSG_ERROR, "drv_soc_deinit_ap: drv_soc_ioctl_get_mode failed.");
		return -EXT_EFAIL;
	}
	if (mode.iftype != (int)EXT_WIFI_IFTYPE_P2P_CLIENT)
		return -EXT_EFAIL;
	mode.iftype = EXT_WIFI_IFTYPE_P2P_DEVICE;

	ret = drv_soc_ioctl_set_mode(drv->iface, &mode);

	return ret;
}
#endif /* LOS_CONFIG_P2P */

static int32 drv_soc_get_drv_flags(void *priv, uint64 *drv_flags)
{
	ext_driver_data_stru *drv = priv;
	ext_get_drv_flags_stru *params = NULL;
	int32 ret;

	if ((priv == NULL) || (drv_flags == NULL))
		return -EXT_EFAIL;

	/* get drv_flags from the driver layer */
	params = (ext_get_drv_flags_stru *)os_zalloc(sizeof(ext_get_drv_flags_stru));
	if (params == NULL)
		return -EXT_EFAIL;
	params->drv_flags = 0;

	ret = drv_soc_ioctl_get_drv_flags(drv->iface, params);
	if (ret != EXT_SUCC) {
		wpa_error_log0(MSG_ERROR, "drv_soc_get_drv_flags: drv_soc_ioctl_get_drv_flags failed.");
		os_free(params);
		return -EXT_EFAIL;
	}
	*drv_flags = params->drv_flags;
	os_free(params);
	return ret;
}

static const uint8 * drv_soc_get_mac_addr(void *priv)
{
	ext_driver_data_stru *drv = priv;
	if (priv == NULL)
		return NULL;
	return drv->own_addr;
}

static int32 drv_soc_wpa_send_eapol(void *priv, const uint8 *dest, uint16 proto, const uint8 *data, uint32 data_len)
{
	ext_driver_data_stru *drv = priv;
	int32 ret;
	uint32 frame_len;
	uint8 *frame_buf = EXT_PTR_NULL;
	uint8 *payload = EXT_PTR_NULL;
	struct l2_ethhdr *l2_ethhdr = EXT_PTR_NULL;
	errno_t rc;

	if ((priv == NULL) || (data == NULL) || (dest == NULL) || (data_len > MAX_BUF_LEN))
		return -EXT_EFAIL;

	frame_len = data_len + (uint32)(sizeof(struct l2_ethhdr));
	frame_buf = os_zalloc(frame_len);
	if (frame_buf == NULL)
		return -EXT_EFAIL;

	l2_ethhdr = (struct l2_ethhdr *)frame_buf;
	rc = memcpy_s(l2_ethhdr->h_dest, ETH_ADDR_LEN, dest, ETH_ADDR_LEN);
	if (rc != EOK) {
		os_free(frame_buf);
		return -EXT_EFAIL;
	}
	rc = memcpy_s(l2_ethhdr->h_source, ETH_ADDR_LEN, drv->own_addr, ETH_ADDR_LEN);
	if (rc != EOK) {
		os_free(frame_buf);
		return -EXT_EFAIL;
	}
	l2_ethhdr->h_proto = host_to_be16(proto);

	payload = (uint8 *)(l2_ethhdr + 1);
	rc = memcpy_s(payload, data_len, data, data_len);
	if (rc != EOK) {
		os_free(frame_buf);
		return -EXT_EFAIL;
	}
	ret = l2_packet_send(drv->eapol_sock, dest, host_to_be16(proto), frame_buf, frame_len);
	os_free(frame_buf);
	return ret;
}

#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
static void drv_soc_set_rekey_info(void *priv, const uint8 *kek, size_t kek_len,
                                   const uint8 *kck, size_t kck_len, const uint8 *replay_ctr)
{
	ext_driver_data_stru *drv = EXT_PTR_NULL;
	ext_rekey_offload_stru *rekey_offload = EXT_PTR_NULL;
	errno_t rc;
	if ((priv == EXT_PTR_NULL) || (kek == EXT_PTR_NULL) || (kck == EXT_PTR_NULL) || (replay_ctr == EXT_PTR_NULL))
		return;
	drv = (ext_driver_data_stru *)priv;
	rekey_offload = os_zalloc(sizeof(ext_rekey_offload_stru));
	if (rekey_offload == EXT_PTR_NULL)
		return;
	rc = memcpy_s(rekey_offload->kck, EXT_REKEY_OFFLOAD_KCK_LEN, kck, kck_len);
	rc = (errno_t)((unsigned int)rc | (unsigned int)memcpy_s(rekey_offload->kek, EXT_REKEY_OFFLOAD_KEK_LEN, kek,
	                                                         kek_len));
	rc = (errno_t)((unsigned int)rc | (unsigned int)memcpy_s(rekey_offload->replay_ctr,
	                                                         EXT_REKEY_OFFLOAD_REPLAY_COUNTER_LEN,
	                                                         replay_ctr, EXT_REKEY_OFFLOAD_REPLAY_COUNTER_LEN));
	if (rc != (errno_t)EOK) {
		(void)memset_s(rekey_offload, sizeof(ext_rekey_offload_stru), 0, sizeof(ext_rekey_offload_stru));
		os_free(rekey_offload);
		return;
	}
	if (drv_soc_ioctl_set_rekey_info(drv->iface, rekey_offload) != EXT_SUCC)
		wpa_error_log0(MSG_ERROR, "drv_soc_set_rekey_info set rekey info failed!");
	(void)memset_s(rekey_offload, sizeof(ext_rekey_offload_stru), 0, sizeof(ext_rekey_offload_stru));
	os_free(rekey_offload);
	return;
}
#endif

#ifdef CONFIG_MESH
int32 drv_soc_mesh_enable_flag(const char *ifname,
                               enum ext_mesh_enable_flag_type flag_type, uint8 enable_flag)
{
	int32 ret;
	ext_mesh_enable_flag_stru *enable_flag_stru = NULL;
	if (ifname == NULL)
		return -EXT_EFAIL;
	enable_flag_stru = os_zalloc(sizeof(ext_mesh_enable_flag_stru));
	if (enable_flag_stru == NULL)
		return -EXT_EFAIL;
	enable_flag_stru->enable_flag = enable_flag;
	ret = drv_soc_ioctl_mesh_enable_flag((int8 *)ifname, flag_type, enable_flag_stru);
	os_free(enable_flag_stru);
	return ret;
}
#endif /* CONFIG_MESH */

int32 drv_soc_set_csi_config(const char *ifname, const ext_csi_config *config)
{
    int32 ret;
    if (ifname == NULL || config == NULL)
        return -EXT_EFAIL;
    return drv_soc_ioctl_set_csi_config((int8 *)ifname, (const void *)config);
}

int32 drv_soc_csi_switch(const char *ifname, const int *switch_flag)
{
    int32 ret;
    if (ifname == NULL || switch_flag == NULL) {
        return -EXT_EFAIL;
    }

    return drv_soc_ioctl_csi_switch((int8 *)ifname, (const void *)switch_flag);
}

int32 drv_soc_set_usr_app_ie(const char *ifname, uint8 set, wifi_extend_ie_index ie_index,
                             const unsigned char frame_type_bitmap, const uint8 *ie, uint16 ie_len)
{
	int32 ret;
	ext_usr_app_ie_stru *app_ie = NULL;
	if ((ifname == NULL) || (set && ((ie == NULL) || (ie_len == 0)))) {
		wpa_error_log0(MSG_ERROR, "drv_soc_set_usr_app_ie: input error.");
		return -EXT_EFAIL;
	}
	app_ie = (ext_usr_app_ie_stru *)os_zalloc(sizeof(ext_usr_app_ie_stru));
	if (app_ie == NULL)
		return -EXT_EFAIL;
	app_ie->set      = set;
	app_ie->ie_index = ie_index;
	app_ie->bitmap   = frame_type_bitmap;
	app_ie->ie_len   = ie_len;
	if (app_ie->set) {
		app_ie->ie = os_zalloc(app_ie->ie_len);
		if (app_ie->ie == NULL) {
			os_free(app_ie);
			return -EXT_EFAIL;
		}
		if (memcpy_s(app_ie->ie, app_ie->ie_len, ie, ie_len) != EOK) {
			ret = -EXT_EFAIL;
			goto FAIL;
		}
	}

	ret = drv_soc_ioctl_set_usr_app_ie(ifname, (const void *)app_ie);
FAIL:
	os_free(app_ie->ie);
	os_free(app_ie);
	return ret;
}

static int32 drv_soc_set_delay_report(const char *ifname, int enable_flag)
{
	int32 ret;
	ext_delay_report_stru *delay_report_stru = NULL;
	if ((ifname == NULL) || ((enable_flag != WPA_FLAG_OFF) && (enable_flag != WPA_FLAG_ON)))
		return -EXT_EFAIL;
	delay_report_stru = os_zalloc(sizeof(ext_delay_report_stru));
	if (delay_report_stru == NULL)
		return -EXT_EFAIL;
	delay_report_stru->enable = (uint8)enable_flag;
	delay_report_stru->timeout = DELAY_REPORT_TIMEOUT;
	ret = drv_soc_ioctl_set_delay_report((int8 *)ifname, delay_report_stru);
	if ((ret == EXT_SUCC) && (enable_flag == WPA_FLAG_ON))
		g_sta_delay_report_flag = WPA_FLAG_ON;
	os_free(delay_report_stru);
	return ret;
}

#if defined (CONFIG_WPA3) || defined (CONFIG_HOSTAPD_WPA3)
static int32 drv_soc_send_ext_auth_status(void *priv, struct external_auth *params)
{
	int32 ret;
	ext_driver_data_stru *drv = priv;
	ext_external_auth_stru *external_auth_stru = NULL;

	if ((priv == NULL) || (params == NULL) || (params->bssid == NULL))
		return -EXT_EFAIL;
	external_auth_stru = os_zalloc(sizeof(ext_external_auth_stru));
	if (external_auth_stru == NULL)
		return -EXT_EFAIL;
	if (memcpy_s(external_auth_stru->bssid, ETH_ADDR_LEN, params->bssid, ETH_ADDR_LEN) != EOK) {
		os_free(external_auth_stru);
		return -EXT_EFAIL;
	}
#if defined(CONFIG_HOSTAPD_WPA3) && defined(CONFIG_HOSTAPD_WPA3_PMKSA)
	if ((drv->hapd != NULL) && (params->pmkid != NULL)) {
		external_auth_stru->pmkid = os_zalloc(SAE_PMKID_LEN);
		if (external_auth_stru->pmkid == NULL) {
			os_free(external_auth_stru);
			return -EXT_EFAIL;
		}
		if (memcpy_s(external_auth_stru->pmkid, SAE_PMKID_LEN, params->pmkid, SAE_PMKID_LEN) != EOK) {
			(void)memset_s(external_auth_stru->pmkid, SAE_PMKID_LEN, 0, SAE_PMKID_LEN);
			os_free(external_auth_stru->pmkid);
			os_free(external_auth_stru);
			return -EXT_EFAIL;
		}
	}
#endif /* CONFIG_HOSTAPD_WPA3 && CONFIG_HOSTAPD_WPA3_PMKSA */
	external_auth_stru->status = params->status;
	ret = drv_soc_ioctl_send_ext_auth_status(drv->iface, external_auth_stru);
#if defined(CONFIG_HOSTAPD_WPA3) && defined(CONFIG_HOSTAPD_WPA3_PMKSA)
	if (drv->hapd != NULL) {
		(void)memset_s(external_auth_stru->pmkid, SAE_PMKID_LEN, 0, SAE_PMKID_LEN);
		os_free(external_auth_stru->pmkid);
	}
#endif /* CONFIG_HOSTAPD_WPA3 && CONFIG_HOSTAPD_WPA3_PMKSA */
	os_free(external_auth_stru);
	return ret;
}
#endif /* CONFIG_WPA3 || CONFIG_HOSTAPD_WPA3 */

#ifdef CONFIG_OWE
static int32 drv_soc_update_dh_ie(void *priv, const u8 *peer_mac, u16 reason_code, const u8 *ie, size_t ie_len)
{
	int32 ret;
	ext_driver_data_stru *drv = priv;
	ext_update_dh_ie_stru *dh_ie_stru = NULL;

	if ((priv == NULL) || (peer_mac == NULL) || (ie == NULL) || (ie_len == 0)) {
		return -EXT_EFAIL;
	}
	dh_ie_stru = os_zalloc(sizeof(ext_update_dh_ie_stru));
	if (dh_ie_stru == NULL) {
		return -EXT_EFAIL;
	}
	memset_s(dh_ie_stru, sizeof(ext_update_dh_ie_stru), 0, sizeof(ext_update_dh_ie_stru));
	if (memcpy_s(dh_ie_stru->peer, sizeof(dh_ie_stru->peer), peer_mac, ETH_ADDR_LEN) != EOK) {
		os_free(dh_ie_stru);
		return -EXT_EFAIL;
	}
	dh_ie_stru->ie_len = ie_len;
	dh_ie_stru->ie = os_zalloc(ie_len);
	if (dh_ie_stru->ie == NULL) {
		os_free(dh_ie_stru);
		return -EXT_EFAIL;
	}
	if (memcpy_s(dh_ie_stru->ie, dh_ie_stru->ie_len, ie, ie_len) != EOK) {
		os_free(dh_ie_stru->ie);
		os_free(dh_ie_stru);
		return -EXT_EFAIL;
	}
	dh_ie_stru->status = reason_code;
	ret = drv_soc_ioctl_update_dh_ie(drv->iface, dh_ie_stru);
	os_free(dh_ie_stru->ie);
	os_free(dh_ie_stru);
	return ret;
}
#endif /* CONFIG_OWE */

static int32 drv_soc_send_action(void *priv, unsigned int freq, unsigned int wait, const u8 *dst, const u8 *src,
                                 const u8 *bssid, const u8 *data, size_t data_len, int no_cck)
{
	int ret = -EXT_EFAIL;
	struct ieee80211_hdr *hdr = EXT_PTR_NULL;
	if ((priv == NULL) || (data == NULL) || (dst == NULL) || (src == NULL) || (bssid == NULL))
		return -EXT_EFAIL;

	wpa_printf(MSG_DEBUG, "drv_soc_send_action: Send Action frame ("
		   "freq=%u MHz wait=%u ms no_cck=%d)",
		   freq, wait, no_cck);

	u8 *buf = os_zalloc(IEEE80211_HDRLEN + data_len);
	if (buf == NULL)
		return ret;

	if (memcpy_s(buf + IEEE80211_HDRLEN, data_len, data, data_len) != EOK)
		goto cleanup;

	hdr = (struct ieee80211_hdr *)buf;
	hdr->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT, WLAN_FC_STYPE_ACTION);
	if ((memcpy_s(hdr->addr1, ETH_ALEN, dst, ETH_ALEN) != EOK) ||
	    (memcpy_s(hdr->addr2, ETH_ALEN, src, ETH_ALEN) != EOK) ||
	    (memcpy_s(hdr->addr3, ETH_ALEN, bssid, ETH_ALEN) != EOK))
		goto cleanup;

	ret = drv_soc_send_mlme(priv, buf, IEEE80211_HDRLEN + data_len, 0, freq, NULL, 0);
cleanup:
	os_free(buf);
	return ret;
}

static int32 drv_soc_sta_remove(void *priv, const uint8 *addr)
{
	ext_driver_data_stru *drv = EXT_PTR_NULL;
	int32 ret;

	if ((priv == EXT_PTR_NULL) || (addr == EXT_PTR_NULL))
		return -EXT_EFAIL;
	drv = (ext_driver_data_stru *)priv;
	ret = drv_soc_ioctl_sta_remove(drv->iface, addr);
	if (ret != EXT_SUCC)
		return -EXT_EFAIL;
	return EXT_SUCC;
}

#ifndef LOS_CONFIG_NO_PMKSA
static void drv_soc_pmkid_free(ext_pmkid_params *pmkid_params)
{
	if (pmkid_params == NULL)
		return;

	if (pmkid_params->pmkid != NULL) {
		(void)memset_s(pmkid_params->pmkid, PMKID_LEN, 0, PMKID_LEN);
		os_free(pmkid_params->pmkid);
		pmkid_params->pmkid = NULL;
	}

	os_free(pmkid_params);
	pmkid_params = NULL;
}

static int32 drv_soc_pmkid_opr(void *priv, struct wpa_pmkid_params *params,
	int32 (*cb)(const int8 *ifname, const void *buf))
{
	int32 ret;
	ext_driver_data_stru *drv = priv;
	ext_pmkid_params *pmkid_params = NULL;

	if ((priv == NULL) || (params == NULL) || (params->bssid == NULL) || (params->pmkid == NULL) || (cb == NULL))
		return -EXT_EFAIL;
	pmkid_params = os_zalloc(sizeof(ext_pmkid_params));
	if (pmkid_params == NULL)
		return -EXT_EFAIL;
	if ((params->bssid != NULL) && (memcpy_s(pmkid_params->bssid, ETH_ADDR_LEN, params->bssid, ETH_ADDR_LEN) != EOK)) {
		os_free(pmkid_params);
		return -EXT_EFAIL;
	}

	pmkid_params->pmkid = os_zalloc(PMKID_LEN);
	if ((pmkid_params->pmkid == NULL) || (memcpy_s(pmkid_params->pmkid, PMKID_LEN, params->pmkid, PMKID_LEN) != EOK)) {
		drv_soc_pmkid_free(pmkid_params);
		return -EXT_EFAIL;
	}
	ret = cb(drv->iface, pmkid_params);

	drv_soc_pmkid_free(pmkid_params);
	return ret;
}

static int32 drv_soc_add_pmkid(void *priv, struct wpa_pmkid_params *params)
{
	return drv_soc_pmkid_opr(priv, params, drv_soc_ioctl_add_pmkid);
}

static int32 drv_soc_remove_pmkid(void *priv, struct wpa_pmkid_params *params)
{
	return drv_soc_pmkid_opr(priv, params, drv_soc_ioctl_remove_pmkid);
}

static int32 drv_soc_flush_pmkid(void *priv)
{
	int32 ret;
	ext_driver_data_stru *drv = priv;
	if (priv == NULL)
		return -EXT_EFAIL;
	ret = drv_soc_ioctl_flush_pmkid(drv->iface, "");
	return ret;
}

#endif /* LOS_CONFIG_NO_PMKSA */

#ifdef CONFIG_ACS
static void drv_soc_add_survey(void *info, uint32 ifidx, void *survey_results)
{
	struct freq_survey *survey = NULL;
	oal_survey_info_stru *sinfo = (oal_survey_info_stru *)info;
	struct dl_list *survey_list = NULL;

	if  ((info == NULL) || (survey_results == NULL))
		return;

	survey = os_zalloc(sizeof(struct freq_survey));
	if  (survey == NULL)
		return;

	survey->ifidx = ifidx + 1;
	if (sinfo->channel != NULL)
		survey->freq = sinfo->channel->center_freq;

	survey->nf = sinfo->noise;
	survey->channel_time = sinfo->channel_time;
	survey->channel_time_busy = sinfo->channel_time_busy;
	survey->channel_time_rx = sinfo->channel_time_rx;
	survey->channel_time_tx = sinfo->channel_time_tx;

	survey->filled = sinfo->filled; /* The filled value depends on the driver. */

	survey_list = &((struct survey_results *)survey_results)->survey_list;
	dl_list_add_tail(survey_list, &survey->list);
}

static void drv_soc_clean_survey_results(struct survey_results *survey_results)
{
	struct freq_survey *survey_element = NULL;
	struct freq_survey *temp = NULL;

	if (dl_list_empty(&survey_results->survey_list))
		return;

	dl_list_for_each_safe(survey_element, temp, &survey_results->survey_list,
			      struct freq_survey, list) {
		dl_list_del(&survey_element->list);
		os_free(survey_element);
		survey_element = NULL;
	}
}

static int drv_soc_get_survey(void *priv, unsigned int freq)
{
	ext_driver_data_stru *drv = priv;
	int err = OAL_CONTINUE;
	ext_survey_results_stru survey = { 0 };
	union wpa_event_data data;

	if (priv == NULL)
		return -EXT_EFAIL;

	(void)memset_s(&data, sizeof(union wpa_event_data), 0, sizeof(union wpa_event_data));

	survey.survey_results = &data.survey_results;
	survey.add_survey = drv_soc_add_survey;

	dl_list_init(&data.survey_results.survey_list);

	if (freq)
		data.survey_results.freq_filter = freq;

	for (uint32 idx = 0; idx < drv->hapd->iface->current_mode->num_channels; idx++) { // channel
		if ((drv->hapd->iface->current_mode->channels[idx].flag & HOSTAPD_CHAN_DISABLED) == 1) {
			wpa_printf(MSG_ERROR, "nl80211: ch %d invalid", idx);
			continue;
		}
		do {
			wpa_printf(MSG_DEBUG, "nl80211: Fetch survey data");
			survey.ifidx = idx;
			err = drv_soc_ioctl_get_survey(drv->iface, &survey);
		} while (err == OAL_CONTINUE);

		if (err != OAL_SUCC)
			break;
	}

	if (err)
		wpa_printf(MSG_ERROR, "nl80211: Failed to process survey data");
	else
		wpa_supplicant_event(drv->ctx, EVENT_SURVEY, &data);

	drv_soc_clean_survey_results(&data.survey_results);
	return err;
}
#endif

#ifdef CONFIG_IEEE80211R
static int drv_soc_update_ft_ies(void *priv, const u8 *md, const u8 *ies, size_t ies_len)
{
    ext_driver_data_stru *drv = priv;
    ext_ft_ies_stru ft_ies;
    int ret;

    if ((drv == NULL) || (md == NULL) || (ies == NULL) || (ies_len == 0)) {
        return -EXT_EFAIL;
    }

    ft_ies.md = (uint16)(((uint16)(md[1]) << 8) | md[0]);
    ft_ies.ie = ies;
    ft_ies.ie_len = (uint16)ies_len;
    ret = drv_soc_ioctl_update_ft_ies(drv->iface, &ft_ies);
    return ret;
}
#endif /* CONFIG_IEEE80211R */

/* wpa_supplicant driver ops */
#ifndef WIN32
const struct wpa_driver_ops wpa_driver_ext_ops = {
	.name                     = "soc",
	.desc                     = "soc liteos driver",
	.get_bssid                = drv_soc_get_bssid,
	.get_ssid                 = drv_soc_get_ssid,
	.set_key                  = drv_soc_set_key,
	.scan2                    = drv_soc_scan,
	.get_scan_results2        = drv_soc_get_scan_results,
	.wifi_app_service_handle  = drv_soc_wifi_app_service_handle,
	.deauthenticate           = drv_soc_deauthenticate,
	.associate                = drv_soc_associate,
	.send_eapol               = drv_soc_wpa_send_eapol,
	.init2                    = drv_soc_wpa_init,
	.deinit                   = drv_soc_wpa_deinit,
	.set_ap                   = drv_soc_set_ap,
	.send_mlme                = drv_soc_send_mlme,
	.get_hw_feature_data      = drv_soc_get_hw_feature_data,

#ifndef LOS_CONFIG_NO_PMKSA
	.add_pmkid                   = drv_soc_add_pmkid,
	.remove_pmkid                = drv_soc_remove_pmkid,
	.flush_pmkid                 = drv_soc_flush_pmkid,
#endif /* LOS_CONFIG_NO_PMKSA */

#ifdef CONFIG_MESH
	.init_mesh                = drv_soc_mesh_init,
	.sta_add                  = drv_soc_mesh_sta_add,
#endif /* CONFIG_MESH */
	.sta_remove               = drv_soc_sta_remove,
	.hapd_send_eapol          = drv_soc_send_eapol,
	.hapd_init                = drv_soc_hapd_init,
	.hapd_deinit              = drv_soc_hapd_deinit,
	.send_action              = drv_soc_send_action,
#ifdef LOS_CONFIG_P2P
	.if_add                   = drv_soc_add_if,
	.if_remove                = drv_soc_remove_if,
	.remain_on_channel        = drv_soc_remain_on_channel,
	.cancel_remain_on_channel = drv_soc_cancel_remain_on_channel,
	.probe_req_report         = drv_soc_probe_req_report,
	.get_p2p_mac_addr         = drv_soc_get_p2p_mac_addr,
	.set_p2p_powersave        = drv_soc_set_p2p_powersave,
	.sta_deauth               = drv_soc_sta_deauth,
	.deinit_ap                = drv_soc_deinit_ap,
	.deinit_p2p_cli           = drv_soc_deinit_p2p_cli,
#ifdef CONFIG_WPS_AP
	.set_ap_wps_ie            = drv_soc_set_ap_wps_p2p_ie,
#endif /* CONFIG_WPS_AP */
#ifdef ANDROID_P2P
	.set_noa                  = drv_soc_set_p2p_noa,
	.get_noa                  = NULL,
#endif /* ANDROID_P2P */
#endif /* LOS_CONFIG_P2P */
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
	.set_rekey_info           = drv_soc_set_rekey_info,
#else
	.set_rekey_info           = NULL,
#endif
#ifdef CONFIG_TDLS
	.send_tdls_mgmt           = NULL,
	.tdls_oper                = NULL,
#endif /* CONFIG_TDLS */
	.get_drv_flags            = drv_soc_get_drv_flags,
	.get_mac_addr             = drv_soc_get_mac_addr,
#ifdef CONFIG_ACS
	.get_survey               = drv_soc_get_survey,
#endif
	.set_delay_report         = drv_soc_set_delay_report,
#if defined (CONFIG_WPA3) || defined (CONFIG_HOSTAPD_WPA3)
	.send_external_auth_status = drv_soc_send_ext_auth_status,
#endif /* CONFIG_WPA3 || CONFIG_HOSTAPD_WPA3 */
#ifdef CONFIG_OWE
    .update_dh_ie             = drv_soc_update_dh_ie,
#endif
#ifdef ANDROID
	.driver_cmd               = NULL,
#endif /* ANDROID */
#ifdef CONFIG_IEEE80211R
    .update_ft_ies            = drv_soc_update_ft_ies
#endif /* CONFIG_IEEE80211R */
};
#endif

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif
