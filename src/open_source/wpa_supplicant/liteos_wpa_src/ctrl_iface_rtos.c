/*
 * WPA Supplicant / UNIX domain socket -based control interface
 * Copyright (c) 2004-2014, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/common.h"
#include "wpa_supplicant/ctrl_iface.h"
#include "utils/eloop.h"
#include "wpa_supplicant_i.h"
#include "ap/hostapd.h"
#include "wifi_api.h"
/* Per-interface ctrl_iface */

struct ctrl_iface_priv *g_ctrl_iface_data = NULL;

static void wpa_supplicant_ctrl_iface_receive(void *eloop_ctx, void *sock_ctx)
{
	struct ctrl_iface_event_buf *event_buf = NULL;
	struct wpa_supplicant *wpa_s = NULL;
	char *buf = NULL;
	char *reply_buf = NULL;
	size_t reply_len = 0;
	void *packet = NULL;
	(void)eloop_ctx;
	(void)sock_ctx;

	while (1) {
		/*
		 * priv->event_queue is from eloop.events[i].user_data,
		 * while eloop.events is malloc in eloop_init and free
		 * in eloop_destroy
		 */
		if (g_ctrl_iface_data == NULL)
			break;
		packet = eloop_read_event(g_ctrl_iface_data->event_queue, 0);
		if (packet == NULL) {
			break;
		}
#ifdef LOS_CONFIG_SUPPORT_CPUP
		uapi_cpup_load_check_proc(uapi_task_get_current_id(), LOAD_SLEEP_TIME_DEFAULT);
#endif /* LOS_CONFIG_SUPPORT_CPUP */
		event_buf = (struct ctrl_iface_event_buf *)packet;
		buf = event_buf->buf;
		wpa_s = event_buf->wpa_s;
		/* reply_buf malloc */
		reply_buf = wpa_supplicant_ctrl_iface_process(wpa_s, buf, &reply_len);
		/* reply_buf free */
		if (os_strcmp(buf, "SCAN_RESULTS") == 0) {
			wpa_error_log0(MSG_ERROR, "os_event_read,wait for WPA_EVENT_SCAN_RESULT_FREE_OK");
			unsigned int ret = 0;
			(void)os_event_read(g_wpa_event, WPA_EVENT_SCAN_RESULT_FREE_OK, &ret,
			                    WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_EVENT_DELAY_5S);
		}
		os_free(reply_buf);
		/* buf free */
		os_memset(buf, 0, WPA_CTRL_CMD_LEN);
		os_free(buf);
		/* packet free */
		os_free(packet);
	}
}

struct ctrl_iface_priv * los_ctrl_iface_init(void *data)
{
	struct ctrl_iface_priv *priv = os_zalloc(sizeof(struct ctrl_iface_priv));
	if (priv == NULL) {
		wpa_error_log0(MSG_ERROR, "los_ctrl_iface_init malloc fail.");
		return NULL;
	}
	priv->wpa_s = data;
	priv->sock = -1;

	if (eloop_register_event(&priv->event_queue, sizeof(void *), wpa_supplicant_ctrl_iface_receive,
	                         data, priv) != EXT_WIFI_OK) {
		os_free(priv);
		return NULL;
	}
	g_ctrl_iface_data = priv;
	return priv;
}

struct ctrl_iface_priv * wpa_supplicant_ctrl_iface_init(struct wpa_supplicant *wpa_s)
{
	struct ctrl_iface_priv *priv = los_ctrl_iface_init(wpa_s);
	if (priv == NULL)
		return NULL;

	return priv;
}

void wpa_supplicant_ctrl_iface_deinit(struct wpa_supplicant *wpa_s, struct ctrl_iface_priv *priv)
{
	struct ext_wifi_dev *softap_wifi_dev = los_get_wifi_dev_by_iftype(EXT_WIFI_IFTYPE_AP);
	struct ext_wifi_dev *wifi_dev = NULL;
	if ((priv == NULL) || (priv->wpa_s == NULL) || (priv->event_queue == NULL))
		return;

	wifi_dev = wpa_get_other_existed_wpa_wifi_dev(priv->wpa_s);
	eloop_unregister_cli_event(priv->event_queue, sizeof(void *));
	if ((wifi_dev != NULL) && (wifi_dev->priv != NULL))
		g_ctrl_iface_data = ((struct wpa_supplicant *)(wifi_dev->priv))->ctrl_iface;
	else if ((softap_wifi_dev != NULL) && (softap_wifi_dev->priv != NULL))
		g_ctrl_iface_data = ((struct hostapd_data *)(softap_wifi_dev->priv))->ctrl_iface;
	else
		g_ctrl_iface_data = NULL;

	os_free(priv);
	wpa_error_log0(MSG_ERROR, "wpa_supplicant_ctrl_iface_deinit exit");
}
