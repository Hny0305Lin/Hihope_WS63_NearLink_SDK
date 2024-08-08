/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: eloop API
 */

#include "common.h"
#include "eloop_ltos.h"
#include "wpa_supplicant_if.h"
#include "hostapd_if.h"
#include "wifi_api.h"
#include "wpa_supplicant/ctrl_iface.h"
#include "securec.h"

static int g_eloop_task_flag[(int)ELOOP_MAX_TASK_TYPE_NUM] = {0};
static int g_eloop_softap_terminate_flag = WPA_FLAG_OFF;
static int g_eloop_wpa_terminate_flag = WPA_FLAG_OFF;

int wpa_supplicant_main_task(void *param)
{
	struct ext_wifi_dev *wifi_dev = (struct ext_wifi_dev *)param;
	int ret = EXT_WIFI_OK;
	if (wifi_dev == NULL) {
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_ERROR);
		ret = EXT_WIFI_FAIL;
		goto exit;
	}
	if (wifi_dev->iftype == EXT_WIFI_IFTYPE_AP) {
		hostapd_main(wifi_dev->ifname);
		ret = EXT_WIFI_OK;
		goto exit;
	}
	wpa_supplicant_main(wifi_dev->ifname);
exit:
	wpa_event_task_free();
	os_task_delete(g_wpataskid);
	return ret;
}

void wpa_supplicant_exit(void)
{
	struct wpa_global *wpa_global = wpa_supplicant_get_wpa_global();
	if (wpa_global == NULL)
		return;
	wpa_supplicant_deinit(wpa_global);
	wpa_supplicant_global_deinit();
}


void hostapd_exit(void)
{
	struct hapd_interfaces *interfaces = hostapd_get_interfaces();
	if (interfaces == NULL)
		return;
	hostapd_pre_quit(interfaces);
	los_free_wifi_dev(los_get_wifi_dev_by_priv(g_hapd));
	hostapd_global_deinit();
	hostapd_global_interfaces_deinit();
}


int eloop_is_running(eloop_task_type eloop_task)
{
	if (eloop_task >= ELOOP_MAX_TASK_TYPE_NUM)
		return EXT_WIFI_FAIL;

	unsigned int int_save;
	os_task_lock(&int_save);
	if (g_eloop_task_flag[eloop_task] != WPA_FLAG_ON) {
		os_task_unlock(int_save);
		return EXT_WIFI_FAIL;
	}
	os_task_unlock(int_save);
	return EXT_WIFI_OK;
}

int global_eloop_is_running(void)
{
	return ((g_eloop_task_flag[ELOOP_TASK_WPA] == WPA_FLAG_ON) ||
	        (g_eloop_task_flag[ELOOP_TASK_HOSTAPD] == WPA_FLAG_ON));
}

int eloop_terminate_wpa_process(void)
{
	int count = 0;
	if (g_eloop_wpa_terminate_flag == WPA_FLAG_ON) {
		g_eloop_wpa_terminate_flag = WPA_FLAG_OFF;
		count++;
		wpa_supplicant_exit();
		(void)os_event_write(g_wpa_event, WPA_EVENT_STA_STOP_OK);
	}
	if (g_eloop_softap_terminate_flag == WPA_FLAG_ON) {
		g_eloop_softap_terminate_flag = WPA_FLAG_OFF;
		count++;
		hostapd_exit();
		// send softap stop event
		(void)os_event_write(g_softap_event, WPA_EVENT_AP_STOP_OK);
	}
	return count ? EXT_WIFI_OK : EXT_WIFI_FAIL;
}

void eloop_run_process_normal_event(unsigned int *ret_flags, struct eloop_data *eloop)
{
	if ((ret_flags == NULL) || (eloop == NULL))
		return;
	for (int i = 0; i < ELOOP_MAX_EVENTS; i++) {
		if (eloop->events[i].flag & *ret_flags) {
			if (eloop->events[i].handler)
				eloop->events[i].handler(eloop->events[i].eloop_data, eloop->events[i].user_data);
			*ret_flags &= ~eloop->events[i].flag;
			if (eloop->events[i].handler == (eloop_event_handler)eloop_terminate)
				break;
		}
	}
}

int eloop_start_running(eloop_task_type eloop_task)
{
	if (global_eloop_is_running()) {
		g_eloop_task_flag[eloop_task] = WPA_FLAG_ON;
		(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_OK);
		return ELOOP_ALREADY_RUNNING;
	}

	g_eloop_task_flag[eloop_task] = WPA_FLAG_ON;
	return ELOOP_NOT_RUNNING;
}

void eloop_terminate(eloop_task_type eloop_task)
{
	if ((eloop_task != ELOOP_TASK_WPA) && (eloop_task != ELOOP_TASK_HOSTAPD))
		return;
	if (g_eloop_task_flag[eloop_task] != WPA_FLAG_ON)
		return;
	g_eloop_task_flag[eloop_task] = WPA_FLAG_OFF;

	if (eloop_task == ELOOP_TASK_WPA)
		g_eloop_wpa_terminate_flag = WPA_FLAG_ON;
	else
		g_eloop_softap_terminate_flag = WPA_FLAG_ON;
}

int eloop_terminated(eloop_task_type eloop_task)
{
	if ((eloop_task != ELOOP_TASK_WPA) && (eloop_task != ELOOP_TASK_HOSTAPD))
		return EXT_WIFI_FAIL;
	return !g_eloop_task_flag[eloop_task];
}
