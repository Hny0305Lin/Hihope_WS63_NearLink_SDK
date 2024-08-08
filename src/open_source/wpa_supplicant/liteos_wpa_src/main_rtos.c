/*
 * WPA Supplicant / main() function for UNIX like OSes and MinGW
 * Copyright (c) 2003-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "common.h"
#include "wpa_supplicant_i.h"
#include "wpa_supplicant_if.h"
#include "wifi_api.h"
#include "eloop.h"
#ifdef CONFIG_P2P
#include "p2p_supplicant.h"
#endif /* CONFIG_P2P */
#include "securec.h"
#ifdef LOS_INLINE_FUNC_CROP
#include "drivers/driver.h"
#endif /* LOS_INLINE_FUNC_CROP */
#ifdef CONFIG_WAPI
#include "wapi_api.h"
#endif /* CONFIG_WAPI */

unsigned int g_wpa_event;
unsigned int g_softap_event;
#ifdef LOS_CONFIG_P2P
unsigned int g_p2p_event;
#endif /* LOS_CONFIG_P2P */
#ifdef CONFIG_DRIVER_SOC
static struct wpa_global *g_wpa_global = NULL;
static struct wpa_interface *g_wpa_iface = NULL;
#endif /* CONFIG_DRIVER_SOC */

struct wpa_global * wpa_supplicant_get_wpa_global(void)
{
	return g_wpa_global;
}

void wpa_supplicant_global_deinit()
{
	os_free(g_wpa_iface);
	g_wpa_iface = NULL;
	g_wpa_global = NULL;
	wpa_error_log0(MSG_ERROR, "wpa_supplicant_exit\n");
}

static int wpa_supplicant_main_init(const char *ifname, struct ext_wifi_dev **wifi_dev, struct wpa_global **global)
{
	if (ifname == NULL) {
		wpa_error_log0(MSG_ERROR, "wpa_supplicant_main: ifname is null \n");
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
		return EXT_WIFI_FAIL;
	}
	wpa_debug_level = MSG_DEBUG;
	wpa_printf(MSG_DEBUG, "wpa_supplicant_main: ifname = %s", ifname);
	*wifi_dev = los_get_wifi_dev_by_name(ifname);
	if (*wifi_dev == NULL) {
		wpa_error_log0(MSG_ERROR, "wpa_supplicant_main: los_get_wifi_dev_by_name failed \n");
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
		return EXT_WIFI_FAIL;
	}
	*global = wpa_supplicant_init();
	if (*global == NULL) {
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
		return EXT_WIFI_FAIL;
	}
	return EXT_WIFI_OK;
}

void wpa_supplicant_main(const char *ifname)
{
	struct wpa_interface *ifaces     = NULL;
	struct wpa_interface *iface      = NULL;
	struct wpa_global *global        = NULL;
	struct ext_wifi_dev *wifi_dev    = NULL;
	char driver[MAX_DRIVER_NAME_LEN] = {"soc"};
	int iface_count;
	int exitcode = EXT_WIFI_OK;
	int ret;
	wpa_error_log0(MSG_ERROR, "---> wpa_supplicant_main enter.");

	ret = wpa_supplicant_main_init(ifname, &wifi_dev, &global);
	if (ret != EXT_WIFI_OK)
		return;

	ifaces = os_zalloc(sizeof(struct wpa_interface));
	if (ifaces == NULL)
		goto OUT;
	iface = ifaces;
	iface_count = 1;
	iface->ifname = ifname;
	iface->driver = driver;

#ifdef CONFIG_DRIVER_SOC
	g_wpa_global = global;
	g_wpa_iface = ifaces;
#endif /* CONFIG_DRIVER_SOC */

	for (int i = 0; (exitcode == EXT_WIFI_OK) && (i < iface_count); i++) {
		struct wpa_supplicant *wpa_s = wpa_supplicant_add_iface(global, &ifaces[i], NULL);
		if (wpa_s == NULL) {
			exitcode = EXT_WIFI_FAIL;
			break;
		}
		unsigned int int_save;
		os_task_lock(&int_save);
		wifi_dev->priv = wpa_s;
		os_task_unlock(int_save);
		wpa_error_buf(MSG_ERROR, "wpa_supplicant_main: wifi_dev: ifname = %s\n", wifi_dev->ifname, strlen(wifi_dev->ifname));
	}
	if (wifi_dev->iftype == EXT_WIFI_IFTYPE_STATION) {
		g_connecting_ssid = NULL;

#ifdef CONFIG_WAPI
		if (wapi_init_iface((struct wpa_supplicant *)(wifi_dev->priv))) {
			wpa_error_buf(MSG_ERROR, "wpa_supplicant_main: wifi_dev: ifname = %s\n",
				wifi_dev->ifname, strlen(wifi_dev->ifname));
			exitcode = EXT_WIFI_FAIL;
		}
#endif /* CONFIG_WAPI */
	}

	if (exitcode == EXT_WIFI_OK) {
		exitcode = wpa_supplicant_run(global);
		if (exitcode != EXT_WIFI_FAIL) {
			(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_OK);
			return;
		}
	}

OUT:
	wpa_supplicant_deinit(global);
#ifdef CONFIG_DRIVER_SOC
	g_wpa_global = NULL;
	g_wpa_iface = NULL;
#endif
	unsigned int int_save;
	os_task_lock(&int_save);
	if (wifi_dev != NULL)
		wifi_dev->priv = NULL;
	os_task_unlock(int_save);

	os_free(ifaces);
	// send sta start failed event
	(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
	return;
}


 int wpa_check_reconnect_timeout_match(const struct wpa_supplicant *wpa_s)
{
	int ret;
	if ((wpa_s == NULL) || (wpa_is_sta(wpa_s) == WPA_FLAG_OFF))
		return 0;
	if ((g_reconnect_set.enable == WPA_FLAG_OFF) ||
	    (g_reconnect_set.pending_flag == WPA_FLAG_ON)) {
		return 0;
	}
	ret = ((wpa_s->wpa_state >= WPA_ASSOCIATING) && (wpa_s->wpa_state != WPA_COMPLETED)) ||
	       ((wpa_s->wpa_state == WPA_COMPLETED) && (wpa_s->current_ssid != NULL) &&
	       (g_reconnect_set.current_ssid == wpa_s->current_ssid));
	return ret;
}

void wpa_supplicant_reconnect_restart(void *eloop_ctx, void *timeout_ctx)
{
	(void)timeout_ctx;
	struct wpa_supplicant *wpa_s = eloop_ctx;
	if ((wpa_s == NULL) || (wpa_is_sta(wpa_s) == WPA_FLAG_OFF)) {
		wpa_error_log0(MSG_ERROR, "wpa_supplicant_reconnect_restart input NULL ptr!");
		return;
	}
	if ((g_reconnect_set.enable == WPA_FLAG_OFF) || (g_connecting_ssid == NULL)) {
		wpa_error_log0(MSG_ERROR, "reconnect policy disabled!");
		return;
	}
	if (wpa_s->wpa_state < WPA_AUTHENTICATING) {
		g_reconnect_set.pending_flag = WPA_FLAG_ON;
		g_connecting_flag = WPA_FLAG_ON;
		wpa_supplicant_select_network(wpa_s, g_connecting_ssid);
		wpa_error_log0(MSG_ERROR, "wpa_supplicant_reconnect_restart!");
	}

	if (g_reconnect_set.timeout > 0)
		(void)eloop_register_timeout(g_reconnect_set.timeout, 0, wpa_supplicant_reconnect_timeout, wpa_s, NULL);
}

void wpa_supplicant_reconnect_timeout(void *eloop_ctx, void *timeout_ctx)
{
	(void)timeout_ctx;
	struct wpa_supplicant *wpa_s = eloop_ctx;
	if ((wpa_s == NULL) || (wpa_is_sta(wpa_s) == WPA_FLAG_OFF)) {
		wpa_error_log0(MSG_ERROR, "wpa_supplicant_reconnect_timeout input NULL ptr!");
		return;
	}
	if (g_reconnect_set.enable == WPA_FLAG_OFF) {
		wpa_error_log0(MSG_ERROR, "reconnect policy disabled!");
		return;
	}
	if ((wpa_s->wpa_state != WPA_COMPLETED) && (g_reconnect_set.pending_flag)) {
		wpas_request_disconnection(wpa_s);
		g_connecting_flag = WPA_FLAG_OFF;
		wpa_error_log0(MSG_ERROR, "wpa reconnect timeout, try to connect next period!");
	}
	g_reconnect_set.pending_flag = WPA_FLAG_OFF;
	if (++g_reconnect_set.try_count >= g_reconnect_set.max_try_count) {
		g_reconnect_set.current_ssid = NULL;
		wpa_error_log0(MSG_ERROR, "wpa reconnect timeout, do not try to connect any more !");
		return;
	}
	if (g_reconnect_set.period > 0)
		(void)eloop_register_timeout(g_reconnect_set.period, 0, wpa_supplicant_reconnect_restart, wpa_s, NULL);
}

#ifndef EXT_CODE_CROP
static ext_wifi_event g_disconnect_delay_report_events = { 0 };

void wpa_send_disconnect_delay_report_events(void)
{
	g_disconnect_delay_report_events.event = EXT_WIFI_EVT_DISCONNECTED;
	if ((g_wpa_event_cb != NULL) && !is_zero_ether_addr(g_disconnect_delay_report_events.info.wifi_disconnected.bssid))
		wifi_new_task_event_cb(&g_disconnect_delay_report_events);
	(void)os_memset(&g_disconnect_delay_report_events, 0, sizeof(ext_wifi_event));
}

void wpa_save_disconnect_event(const struct wpa_supplicant *wpa_s, const u8 *bssid, u16 reason_code)
{
	unsigned char *old_bssid = g_disconnect_delay_report_events.info.wifi_disconnected.bssid;
	if ((wpa_s == NULL) || (wpa_is_sta(wpa_s) == WPA_FLAG_OFF) || (bssid == NULL))
		return;
	if ((!is_zero_ether_addr(old_bssid)) && (os_memcmp(old_bssid, bssid, ETH_ALEN) != 0))
		return;
	(void)os_memset(&g_disconnect_delay_report_events, 0, sizeof(ext_wifi_event));
	g_disconnect_delay_report_events.event = EXT_WIFI_EVT_DISCONNECTED;
	(void)os_memcpy(g_disconnect_delay_report_events.info.wifi_disconnected.ifname,
	              wpa_s->ifname, WIFI_IFNAME_MAX_SIZE);
	(void)os_memcpy(g_disconnect_delay_report_events.info.wifi_disconnected.bssid, bssid, ETH_ALEN);
	g_disconnect_delay_report_events.info.wifi_disconnected.reason_code = reason_code;
}

void wpa_sta_delay_report_deinit(const struct ext_wifi_dev *wifi_dev)
{
	if ((wifi_dev == NULL) || (wifi_dev->iftype != EXT_WIFI_IFTYPE_STATION))
		return;
	g_sta_delay_report_flag = WPA_FLAG_OFF;
	(void)os_memset(&g_disconnect_delay_report_events, 0, sizeof(ext_wifi_event));
}
#endif

int wpa_is_sta(const struct wpa_supplicant *wpa_s)
{
	struct ext_wifi_dev *wifi_dev = los_get_wifi_dev_by_priv(wpa_s);
	if ((wifi_dev != NULL) && (wifi_dev->iftype == EXT_WIFI_IFTYPE_STATION)) {
		return WPA_FLAG_ON;
	}
	return WPA_FLAG_OFF;
}

#ifndef CONFIG_ROAM_EXTRA_SUPPORT
void wpa_clear_passphrase(struct wpa_supplicant *wpa_s)
{
	struct wpa_ssid *ssid = wpa_s->current_ssid;
	if ((ssid != NULL) && (ssid->passphrase != NULL)) {
		/* 对passphrase进行清空操作会导致wpa3回连失败，先将密码置0，然后设置*字符 */
		if (ssid->passphrase[0] != '*') {
			os_memset(ssid->passphrase, 0, strlen(ssid->passphrase));
			os_memcpy(ssid->passphrase, "********", strlen("********"));
		} else {
			os_memset(ssid->passphrase, 0, strlen(ssid->passphrase));
			os_memcpy(ssid->passphrase, "a*******", strlen("a*******"));
		}

	}
}
#endif

#ifdef LOS_INLINE_FUNC_CROP
int wpa_drv_get_bssid(struct wpa_supplicant *wpa_s, u8 *bssid)
{
	if ((wpa_s != NULL) && (wpa_s->driver != NULL) && (wpa_s->driver->get_bssid != NULL))
		return wpa_s->driver->get_bssid(wpa_s->drv_priv, bssid);
	return -1;
}

int wpa_drv_get_ssid(struct wpa_supplicant *wpa_s, u8 *ssid)
{
	if ((wpa_s != NULL) && (wpa_s->driver != NULL) && (wpa_s->driver->get_ssid != NULL))
		return wpa_s->driver->get_ssid(wpa_s->drv_priv, ssid);
	return -1;
}

int wpa_drv_set_key(struct wpa_supplicant *wpa_s,
				  enum wpa_alg alg, const u8 *addr,
				  int key_idx, int set_tx,
				  const u8 *seq, size_t seq_len,
				  const u8 *key, size_t key_len)
{
	struct wpa_driver_set_key_params params;

	unsigned int idx = (unsigned int)key_idx;

	os_memset(&params, 0, sizeof(params));

	if ((wpa_s == NULL) || (wpa_s->driver == NULL) || (wpa_s->driver->get_ssid == NULL))
		return -1;

	if (alg != WPA_ALG_NONE) {
		if (idx <= 6)
			wpa_s->keys_cleared &= ~BIT(idx);
		else
			wpa_s->keys_cleared = 0;
	}
	if (wpa_s->driver->set_key) {
        params.ifname = wpa_s->ifname;
        params.alg = alg;
        params.addr = addr;
        params.key_idx = key_idx;
        params.set_tx = set_tx;
        params.seq = seq;
        params.seq_len = seq_len;
        params.key = key;
        params.key_len = key_len;

		return wpa_s->driver->set_key(wpa_s->drv_priv, &params);
	}
	return -1;
}
#endif /* LOS_INLINE_FUNC_CROP */
