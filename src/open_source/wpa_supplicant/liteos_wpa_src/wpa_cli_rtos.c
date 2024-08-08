/*
 * WPA Supplicant - command line interface for wpa_supplicant daemon
 * Copyright (c) 2004-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/common.h"
#include "eloop.h"
#include "wifi_api.h"
#include "ctrl_iface.h"
#ifdef CONFIG_LITEOS_WPA
#include "wpa_supplicant_i.h"
#endif /* CONFIG_LITEOS_WPA */
#include "securec.h"

#define MAX_WPA_CLI_ARGC_NUM 10

static int wpa_ctrl_command(void *buf)
{
	if (buf == NULL)
		return EXT_WIFI_FAIL;
	int ret;
	unsigned int int_save;

	os_intlock(&int_save);
	if (g_ctrl_iface_data == NULL) {
		os_intrestore(int_save);
		wpa_error_log0(MSG_ERROR, "ctrl command: ctrl iface data is NULL\n");

		return EXT_WIFI_FAIL;
	}
	ret = eloop_post_event(g_ctrl_iface_data->event_queue, buf, 1);
	os_intrestore(int_save);
	return ret;
}

static int write_cmd(char *buf, size_t buflen, int argc, char *argv[])
{
	int i, res;
	char *pos = NULL;
	char *end = NULL;

	if ((buflen < 1) || (argc <= 0) || (argc > MAX_WPA_CLI_ARGC_NUM))
		return EXT_WIFI_FAIL;

	pos = buf;
	end = buf + buflen;

	res = os_snprintf(pos, end - pos, "%s", argv[0]);
	if (os_snprintf_error(end - pos, res))
		goto fail;
	pos += res;

	argc--;
	argv++;

	for (i = 0; i < argc; i++) {
		res = os_snprintf(pos, end - pos, " %s", argv[i]);
		if (os_snprintf_error(end - pos, res))
			goto fail;
		pos += res;
	}

	buf[buflen - 1] = '\0';
	return EXT_WIFI_OK;

fail:
	wpa_error_log0(MSG_ERROR, "write_cmd: cmd is too Long \n");
	return EXT_WIFI_FAIL;
}

int wpa_cli_cmd(struct wpa_supplicant *wpa_s, int argc, char *argv[])
{
	struct ctrl_iface_event_buf *event_buf  = NULL;
	char *buf = NULL;
	event_buf = (struct ctrl_iface_event_buf *)os_malloc(sizeof(struct ctrl_iface_event_buf));
	if (event_buf == NULL)
		return EXT_WIFI_FAIL;

	buf = (char *)os_malloc(WPA_CTRL_CMD_LEN);
	if (buf == NULL)
		goto fail_event;

	if (write_cmd(buf, WPA_CTRL_CMD_LEN, argc, argv) < 0)
		goto fail;

	event_buf->wpa_s = wpa_s;
	event_buf->buf   = buf;
	if (wpa_ctrl_command(event_buf) < 0) {
		wpa_printf(MSG_ERROR, "wpa_cli_cmd fail!\n");
		goto fail;
	}

	return EXT_WIFI_OK;
fail:
	os_memset(buf, 0, WPA_CTRL_CMD_LEN);
	os_free(buf);
fail_event:
	os_free(event_buf);
	return EXT_WIFI_FAIL;
}

#ifdef CONFIG_WPS
int wpa_cli_wps_pbc(struct wpa_supplicant *wpa_s, const char *bssid, size_t len)
{
	char *cmd_bssid_null[] = {"WPS_PBC"};
	char *cmd_bssid[] = {"WPS_PBC", (char *)bssid};

	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_wps_pbc");
	if (bssid == NULL) {
		wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_wps_pbc cmd_bssid_null");
		return wpa_cli_cmd(wpa_s, 1, cmd_bssid_null);
	} else {
		if (len == 0)
			return EXT_WIFI_FAIL;
		return wpa_cli_cmd(wpa_s, 2, cmd_bssid);
	}
}

int wpa_cli_wps_pin(struct wpa_supplicant *wpa_s, const char *pin, size_t pin_len, const char *bssid, size_t len)
{
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_wps_pin");
	if ((pin == NULL) || (pin_len != 8)) {    // 8: WPS PIN length
		wpa_warning_log0(MSG_DEBUG, "wpa_cli_wps_pin : pin is NULL");
		return EXT_WIFI_FAIL;
	}
	if (bssid != NULL) {
		if (len == 0)
			return EXT_WIFI_FAIL;
		char *cmd_bssid_pin[] = {"WPS_PIN", (char *)bssid, (char *)pin};
		return wpa_cli_cmd(wpa_s, 3, cmd_bssid_pin);
	} else {
		char *cmd_null_pin[] = {"WPS_PIN any", (char *)pin};
		return wpa_cli_cmd(wpa_s, 2, cmd_null_pin);
	}
}
#endif /* CONFIG_WPS */

#ifdef CONFIG_WNM
int wpa_cli_wnm_bss_query(struct wpa_supplicant *wpa_s, const char *buf, size_t len)
{
	char *cmd[] = {"WNM_BSS_QUERY", (char *)buf};
	if ((len == 0) && (os_strlen(buf) != 0))
		return EXT_WIFI_FAIL;

	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_wnm_notify(struct wpa_supplicant *wpa_s, const char *buf, size_t len)
{
	char *cmd[] = {"WNM_Notify", (char *)buf};
	if ((len == 0) && (os_strlen(buf) != 0))
		return EXT_WIFI_FAIL;

	return wpa_cli_cmd(wpa_s, 2, cmd);
}
#endif /* CONFIG_WNM */

int wpa_cli_scan(struct wpa_supplicant *wpa_s, const char *buf, size_t len)
{
	char *cmd[] = {"SCAN", (char *)buf};
	if ((len == 0) && (os_strlen(buf) != 0))
		return EXT_WIFI_FAIL;

	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_scan_results(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"SCAN_RESULTS"};

	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_channel_scan(struct wpa_supplicant *wpa_s, int channel)
{
	int freq;
	char buf[10] = {0};
	int ret;

	if ((channel > 14) || (channel < 1)) {
		wpa_warning_log0(MSG_DEBUG, "set channel error\n");
		return EXT_WIFI_FAIL;
	}

	if (channel != 14)
		freq = (channel - 1) * 5 + 2412;
	else
		freq = 2484;
	ret = os_snprintf(buf, sizeof(buf), "freq=%d", freq);
	if (os_snprintf_error(sizeof(buf), ret))
		return EXT_WIFI_FAIL;

	char *cmd[] = {"SCAN", buf};

	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_ap_deauth(struct wpa_supplicant *wpa_s, const char *buf, size_t len)
{
	char *cmd[] = {"AP_DEAUTH", (char *)buf};
	if (len == 0)
		return EXT_WIFI_FAIL;

	return wpa_cli_cmd(wpa_s, 2, cmd);
}

#ifdef LOS_CONFIG_MESH
int wpa_cli_mesh_deauth(struct wpa_supplicant *wpa_s, const char *buf, size_t len)
{
	char *cmd[] = {"MESH_DEAUTH", (char *)buf};
	if (len == 0)
		return EXT_WIFI_FAIL;

	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_join_mesh(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"JOIN_MESH"};

	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_mesh_set_accept(struct wpa_supplicant *wpa_s, unsigned char enable, enum ext_mesh_enable_flag_type flag)
{
	char *cmd_sta_accept_enable[] = {"MESH_SET_ACCEPT", "STA", "1"};
	char *cmd_sta_accept_disable[] = {"MESH_SET_ACCEPT", "STA", "0"};
	char *cmd_peer_accept_enable[] = {"MESH_SET_ACCEPT", "PEER", "1"};
	char *cmd_peer_accept_disable[] = {"MESH_SET_ACCEPT", "PEER", "0"};
	char **tmp = NULL;
	if (flag == EXT_MESH_ENABLE_ACCEPT_STA) {
		tmp = (enable == WPA_FLAG_ON) ? cmd_sta_accept_enable : cmd_sta_accept_disable;
		return wpa_cli_cmd(wpa_s, 3, tmp);
	}
	if (flag == EXT_MESH_ENABLE_ACCEPT_PEER) {
		tmp = (enable == WPA_FLAG_ON) ? cmd_peer_accept_enable : cmd_peer_accept_disable;
		return wpa_cli_cmd(wpa_s, 3, tmp);
	}
	return EXT_WIFI_FAIL;
}

#endif /* LOS_CONFIG_MESH */

int wpa_cli_ap_scan(struct wpa_supplicant *wpa_s, const char *mode, size_t len)
{
	char *cmd[] = {"AP_SCAN", (char *)mode};
	if (len == 0)
		return EXT_WIFI_FAIL;

	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_add_iface(struct wpa_supplicant *wpa_s, const char *ifname)
{
	char *cmd[] = {"ADD_IFACE", (char *)ifname};

	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_add_network(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"ADD_NETWORK"};

	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_disconnect(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"DISCONNECT"};

	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_remove_network(struct wpa_supplicant *wpa_s, const char *id, size_t len)
{
	char *cmd_remove_all[] = {"REMOVE_NETWORK all"};
	char *cmd_remove[] = {"REMOVE_NETWORK", (char *)id};
	unsigned int ret;
	unsigned int int_save;

	os_intlock(&int_save);
	g_wpa_rm_network = SOC_WPA_RM_NETWORK_START;
	os_intrestore(int_save);
	(void)os_event_clear(g_wpa_event, ~WPA_EVENT_STA_RM_NETWORK_OK);
	if (id == NULL) {
		(void)wpa_cli_cmd(wpa_s, 1, cmd_remove_all);
	} else {
		if (len == 0)
			return EXT_WIFI_FAIL;
		(void)wpa_cli_cmd(wpa_s, 2, cmd_remove);
	}
	wpa_error_log0(MSG_ERROR, "os_event_read,wait for WPA_EVENT_STA_RM_NETWORK_OK");

	(void)os_event_read(g_wpa_event, WPA_EVENT_STA_RM_NETWORK_OK, &ret,
	                    WIFI_WAITMODE_OR | WIFI_WAITMODE_CLR, WIFI_EVENT_DELAY_5S);
	if (ret == WPA_EVENT_STA_RM_NETWORK_OK)
		wpa_error_log0(MSG_INFO, "wpa_cli_remove_network successful.");

	g_wpa_rm_network = SOC_WPA_RM_NETWORK_END;
	return (ret == WPA_EVENT_STA_RM_NETWORK_OK) ? EXT_WIFI_OK : EXT_WIFI_FAIL;
}

int wpa_cli_terminate(struct wpa_supplicant *wpa_s, eloop_task_type e_type)
{
	char buf[10] = {0};
	int ret = os_snprintf(buf, sizeof(buf), "%d", e_type);
	if (os_snprintf_error(sizeof(buf), ret))
		return EXT_WIFI_FAIL;

	char *cmd[] = {"ELOOP_TERMINATE", buf};

	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_remove_iface(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"REMOVE_IFACE"};

	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_select_network(struct wpa_supplicant *wpa_s, const char *id, size_t len)
{
	char *cmd[] = {"SELECT_NETWORK", (char *)id};
	if (len == 0)
		return EXT_WIFI_FAIL;

	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_set_network(struct wpa_supplicant *wpa_s, const char *id, size_t id_len, const char *param,
	const char *value, size_t value_len)
{
	char *cmd[] = {"SET_NETWORK", (char *)id, (char *)param, (char *)value};
	if ((id_len == 0) || (value_len == 0))
		return EXT_WIFI_FAIL;
	return wpa_cli_cmd(wpa_s, 4, cmd);
}

int wpa_cli_show_sta(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"SHOW_STA"};

	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_list_networks(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"LIST_NETWORKS"};

	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_sta_status(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"STA_STATUS"};

	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_get_sta_status(struct wpa_supplicant *wpa_s, ext_wifi_status_sem *connect_status)
{
    char addr[20];  /* 2^64需要20个字符来存储 */

    (void)snprintf(addr, sizeof(addr), "%d", connect_status);
    char *cmd[] = {"STA_STATUS", (char *)addr};

    return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_configure_wep(struct wpa_supplicant *wpa_s, const char *id, const struct wpa_assoc_request *assoc)
{
	char command[33];
	int ret;
	if ((assoc == NULL) || (assoc->auth != EXT_WIFI_SECURITY_WEP))
		return EXT_WIFI_FAIL;

	if ((wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "auth_alg",
	    	"OPEN SHARED", os_strlen("OPEN SHARED")) != EXT_WIFI_OK) ||
	    (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "key_mgmt",
	    	"NONE", os_strlen("NONE")) != EXT_WIFI_OK))
		return EXT_WIFI_FAIL;
	ret = os_snprintf(command, sizeof(command), "wep_key%d", 0);
	if (os_snprintf_error(sizeof(command), ret))
		return EXT_WIFI_FAIL;

	if (wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, command,
	    	assoc->key, os_strlen(assoc->key)) != EXT_WIFI_OK)
		return EXT_WIFI_FAIL;

	ret = os_snprintf(command, sizeof(command), "%d", 0);
	if (os_snprintf_error(sizeof(command), ret))
		return EXT_WIFI_FAIL;

	if ((wpa_cli_set_network(wpa_s, id, WPA_NETWORK_ID_TXT_LEN, "wep_tx_keyidx",
	    	command, os_strlen(command)) != EXT_WIFI_OK))
		return EXT_WIFI_FAIL;

	return EXT_WIFI_OK;
}

int wpa_cli_if_start(struct wpa_supplicant *wpa_s, ext_wifi_iftype iftype, const char *ifname, size_t len)
{
	wpa_error_log0(MSG_ERROR, "---> wpa_cli_if_start enter.");
	char *cmd_softap[] = {"SOFTAP_START", (char *)ifname};
	char *cmd_wpa[] = {"WPA_START", (char *)ifname};
	if (len == 0)
		return EXT_WIFI_FAIL;

	if (iftype == EXT_WIFI_IFTYPE_AP)
		return wpa_cli_cmd(wpa_s, 2, cmd_softap);

	return wpa_cli_cmd(wpa_s, 2, cmd_wpa);
}

int wpa_cli_sta_set_delay_report(struct wpa_supplicant *wpa_s, int enable)
{
	char *cmd_enable[] = {"STA_SET_DELAY_RP", "1"};
	char *cmd_disable[] = {"STA_SET_DELAY_RP", "0"};
	if (enable == WPA_FLAG_ON)
		return wpa_cli_cmd(wpa_s, 2, cmd_enable);
	else
		return wpa_cli_cmd(wpa_s, 2, cmd_disable);
}

#ifdef LOS_CONFIG_P2P
int wpa_cli_p2p_find(struct wpa_supplicant *wpa_s, char *sec, size_t len)
{
	char *cmd[] = {"P2P_FIND", sec};
	if (len == 0)
		return EXT_WIFI_FAIL;

	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_find");
	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_p2p_listen(struct wpa_supplicant *wpa_s, char *time, size_t len)
{
	char *cmd[] = {"P2P_EXT_LISTEN", time};
	if (len == 0)
		return EXT_WIFI_FAIL;

	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_listen");
	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_p2p_cancel(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"P2P_CANCEL"};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_cancel");
	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_log_level(struct wpa_supplicant *wpa_s, char *param, size_t len)
{
	char *cmd[2] = {"LOG_LEVEL", param};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_log_level");
	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_p2p_connect(struct wpa_supplicant *wpa_s, int persistent, const p2p_connect_param *param)
{
	if (param == NULL)
		return EXT_WIFI_FAIL;
	char *cmd_intent[6] = {"P2P_CONNECT", (char *)param->addr_txt, (char *)param->wps_txt,
		(char *)param->join_txt,"persistent", (char *)param->intent_param};
	char *cmd[5] = {"P2P_CONNECT", (char *)param->addr_txt, (char *)param->wps_txt, (char *)param->join_txt,
		(char *)param->intent_param};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_connect");
	if (persistent == 1) {
		return wpa_cli_cmd(wpa_s, 6, cmd_intent);
	} else {
		return wpa_cli_cmd(wpa_s, 5, cmd);
	}
}

int wpa_cli_p2p_reject(struct wpa_supplicant *wpa_s, char *mac, size_t len)
{
	char *cmd[2] = {"P2P_REJECT", mac};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_reject");
	if (len == 0)
		return EXT_WIFI_FAIL;
	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_p2p_stop_find(struct wpa_supplicant *wpa_s)
{
	char *cmd[] = {"P2P_STOP_FIND"};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_stop_find");
	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_p2p_peer(struct wpa_supplicant *wpa_s)
{
	char *cmd[1] = {"P2P_PEER "};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_peer");
	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_p2p_status(struct wpa_supplicant *wpa_s)
{
	char *cmd[1] = {"STATUS"};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_status");
	return wpa_cli_cmd(wpa_s, 1, cmd);
}

int wpa_cli_p2p_user_config(struct wpa_supplicant *wpa_s, char *param, size_t len)
{
	char *cmd[] = {"P2P_UPDATE_CONFIG", param};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_update_config");
	if (len == 0)
		return EXT_WIFI_FAIL;
	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_p2p_group_remove(struct wpa_supplicant *wpa_s, char *ifname, size_t len)
{
	char *cmd[] = {"P2P_GROUP_REMOVE", ifname};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_group_remove");
	if (len == 0)
		return EXT_WIFI_FAIL;
	return wpa_cli_cmd(wpa_s, 2, cmd);
}

int wpa_cli_p2p_remove_client(struct wpa_supplicant *wpa_s, char *param, size_t len)
{
	char *cmd[] = {"P2P_REMOVE_CLIENT", param};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_client_remove");
	if (len == 0)
		return EXT_WIFI_FAIL;
	return wpa_cli_cmd(wpa_s, 2, cmd); /* 2表示参数个数 */
}

int wpa_cli_p2p_invite(struct wpa_supplicant *wpa_s, const p2p_invite_param *param)
{
	if (param == NULL)
		return EXT_WIFI_FAIL;
	char *cmd_per[3] = {"P2P_INVITE", (char *)param->per_id_set, (char *)param->mac_set};
	char *cmd[3] = {"P2P_INVITE", (char *)param->group, (char *)param->mac_set};
	wpa_warning_log0(MSG_DEBUG, "Enter wpa_cli_p2p_invite");
	if (os_strlen(param->per_id_set) > 0) {
		return wpa_cli_cmd(wpa_s, 3, cmd_per);
	} else {
		return wpa_cli_cmd(wpa_s, 3, cmd);
	}
}
#endif /* LOS_CONFIG_P2P */

