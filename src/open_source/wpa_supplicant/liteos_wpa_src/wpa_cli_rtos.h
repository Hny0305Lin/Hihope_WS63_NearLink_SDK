/*
 * wpa_supplicant/hostapd / Debug prints
 * Copyright (c) 2002-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPA_CLI_RTOS_H
#define WPA_CLI_RTOS_H

int wpa_cli_scan(struct wpa_supplicant *wpa_s, const char *buf, int len);
int wpa_cli_scan_results(struct wpa_supplicant *wpa_s);
int wpa_cli_channel_scan(struct wpa_supplicant *wpa_s, int channel);
int wpa_cli_ap_scan(struct wpa_supplicant *wpa_s, const char *mode);
int wpa_cli_add_network(struct wpa_supplicant *wpa_s);
int wpa_cli_disconnect(struct wpa_supplicant *wpa_s);
int wpa_cli_remove_network(struct wpa_supplicant *wpa_s, const char *id, size_t len);
int wpa_cli_remove_iface(struct wpa_supplicant *wpa_s);
int wpa_cli_select_network(struct wpa_supplicant *wpa_s, const char *id, size_t len);
int wpa_cli_set_network(struct wpa_supplicant *wpa_s, const char *id, size_t id_len, const char *param,
	const char *value, size_t value_len);
int wpa_cli_show_sta(struct wpa_supplicant *wpa_s);
int wpa_cli_sta_status(struct wpa_supplicant *wpa_s);
int wpa_cli_get_sta_status(struct wpa_supplicant *wpa_s, ext_wifi_status_sem *connect_status);
int wpa_cli_configure_wep(struct wpa_supplicant *wpa_s, const char *id, const struct wpa_assoc_request *assoc);
int wpa_cli_if_start(struct wpa_supplicant *wpa_s, ext_wifi_iftype iftype, const char *ifname, size_t len);
int wpa_cli_add_iface(struct wpa_supplicant *wpa_s, const char *ifname);
int wpa_cli_terminate(struct wpa_supplicant *wpa_s, eloop_task_type e_type);
int wpa_cli_ap_deauth(struct wpa_supplicant *wpa_s, const char *buf, size_t len);
#ifdef CONFIG_WPS
int wpa_cli_wps_pbc(struct wpa_supplicant *wpa_s, const char *bssid, size_t len);
int wpa_cli_wps_pin(struct wpa_supplicant *wpa_s, const char *pin, size_t pin_len, const char *bssid, size_t len);
#endif /* CONFIG_WPS */
#ifdef LOS_CONFIG_MESH
int wpa_cli_mesh_deauth(struct wpa_supplicant *wpa_s, const char *buf, size_t len);
int wpa_cli_join_mesh(struct wpa_supplicant *wpa_s);
int wpa_cli_mesh_set_accept(struct wpa_supplicant *wpa_s, unsigned char enable, enum ext_mesh_enable_flag_type flag);
#endif /* LOS_CONFIG_MESH */
int wpa_cli_sta_set_delay_report(struct wpa_supplicant *wpa_s, int enable);
int wpa_cli_list_networks(struct wpa_supplicant *wpa_s);
#ifdef LOS_CONFIG_P2P
int wpa_cli_p2p_find(struct wpa_supplicant *wpa_s, char *sec, size_t len);
int wpa_cli_p2p_connect(struct wpa_supplicant *wpa_s, int persistent, const p2p_connect_param *param);
int wpa_cli_p2p_reject(struct wpa_supplicant *wpa_s, char *mac, size_t len);
int wpa_cli_p2p_stop_find(struct wpa_supplicant *wpa_s);
int wpa_cli_p2p_group_remove(struct wpa_supplicant *wpa_s, char *ifname, size_t len);
int wpa_cli_p2p_remove_client(struct wpa_supplicant *wpa_s, char *param, size_t len);
int wpa_cli_p2p_user_config(struct wpa_supplicant *wpa_s, char *param, size_t len);
int wpa_cli_p2p_invite(struct wpa_supplicant *wpa_s, const p2p_invite_param *param);
int wpa_cli_p2p_peer(struct wpa_supplicant *wpa_s);
int wpa_cli_p2p_status(struct wpa_supplicant *wpa_s);
int wpa_cli_p2p_listen(struct wpa_supplicant *wpa_s, char *time, size_t len);
int wpa_cli_p2p_cancel(struct wpa_supplicant *wpa_s);
int wpa_cli_log_level(struct wpa_supplicant *wpa_s, char *param, size_t len);
#endif /* LOS_CONFIG_P2P */
#ifdef CONFIG_WNM /* CONFIG_WNM */
int wpa_cli_wnm_bss_query(struct wpa_supplicant *wpa_s, const char *buf, size_t len);
int wpa_cli_wnm_notify(struct wpa_supplicant *wpa_s, const char *buf, size_t len);
#endif  /* CONFIG_WNM */

#endif /* WPA_CLI_RTOS_H */
