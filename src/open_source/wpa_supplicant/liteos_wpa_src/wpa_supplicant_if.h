/*
 * WPA Supplicant
 * Copyright (c) 2003-2016, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file implements functions for registering and unregistering
 * %wpa_supplicant interfaces. In addition, this file contains number of
 * functions for managing network connections.
 */

#ifndef _WPA_SUPPLICANT_H_
#define _WPA_SUPPLICANT_H_

#include "wpa_supplicant_i.h"
#include "wifi_api.h"

extern int g_sta_delay_report_flag;
extern unsigned int g_wpataskid;

void wpa_supplicant_main(const char *ifname);

void los_wpa_supplicant_exit(void);
int wpa_supplicant_main_task(void *param);
char * wpa_supplicant_ctrl_iface_process(struct wpa_supplicant *wpa_s, char *buf, size_t *resp_len);
void wpa_supplicant_reconnect_timeout(void *eloop_ctx, void *timeout_ctx);
void wpa_supplicant_reconnect_restart(void *eloop_ctx, void *timeout_ctx);
int wpa_check_reconnect_timeout_match(const struct wpa_supplicant *wpa_s);
void wpa_send_disconnect_delay_report_events(void);
void wpa_save_disconnect_event(const struct wpa_supplicant *wpa_s, const u8 *bssid, u16 reason_code);
void wpa_sta_delay_report_deinit(const struct ext_wifi_dev *wifi_dev);
void wpa_register_timeout_periodic_cleanup(struct wpa_supplicant *wpa_s);
struct wpa_global * wpa_supplicant_get_wpa_global(void);
void wpa_supplicant_global_deinit(void);
int wpa_is_sta(const struct wpa_supplicant *wpa_s);
void wpa_clear_passphrase(struct wpa_supplicant *wpa_s);
#endif /* _WPA_SUPPLICANT_H_ */
