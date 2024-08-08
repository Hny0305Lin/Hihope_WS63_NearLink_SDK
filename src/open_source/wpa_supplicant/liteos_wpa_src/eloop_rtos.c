/*
 * Event loop based on select() loop
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include <assert.h>

#include "common.h"
#include "trace.h"
#include "list.h"
#include "eloop_ltos.h"
#include "wpa_supplicant_if.h"
#include "hostapd_if.h"
#include "wifi_api.h"
#include "wpa_supplicant/ctrl_iface.h"
#include "securec.h"

#define ELOOP_TIMEOUT_REGISTERED       1
#ifndef OSS_TICKS_PER_SEC
#define OSS_TICKS_PER_SEC              100
#endif
#define TICKS_TO_MSEC(ticks) (((ticks) * 1000) / OSS_TICKS_PER_SEC)
#define MSEC_TO_TICKS(ms)    (((ms) * OSS_TICKS_PER_SEC) / 1000)

struct eloop_timeout {
	struct dl_list list;
	struct os_reltime time;
	void *eloop_data;
	void *user_data;
	eloop_timeout_handler handler;
	WPA_TRACE_REF(eloop);
	WPA_TRACE_REF(user);
	WPA_TRACE_INFO
};

struct eloop_message {
	struct dl_list list;
	char *buf;
};

static struct eloop_data eloop;

int eloop_init(eloop_task_type eloop_task)
{
	if ((eloop_task != ELOOP_TASK_WPA) && (eloop_task != ELOOP_TASK_HOSTAPD))
		return EXT_WIFI_FAIL;

	if (eloop.events != NULL)
		return EXT_WIFI_OK;
	(void)os_memset(&eloop, 0, sizeof(eloop));
	(void)os_event_init(&eloop.levent, WIFI_EVENT_ELOOP);
	dl_list_init(&eloop.timeout);

	int ret = os_mux_create(&eloop.mutex_handle);
	if (ret != WIFI_OS_OK) {
		wpa_error_log0(MSG_ERROR, "---> eloop_init MuxCreate fail.");
		return EXT_WIFI_FAIL;
	}
	eloop.events = os_zalloc(sizeof(struct eloop_event) * ELOOP_MAX_EVENTS);
	if (eloop.events == NULL) {
		(void)os_mux_delete(eloop.mutex_handle);
		return EXT_WIFI_FAIL;
	}
	return EXT_WIFI_OK;
}

int eloop_register_timeout(unsigned int secs, unsigned int usecs,
                           eloop_timeout_handler handler,
                           void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout = NULL;
	struct eloop_timeout *tmp = NULL;
	os_time_t now_sec;
	if (handler == NULL)
		return EXT_WIFI_FAIL;
	timeout = os_zalloc(sizeof(*timeout));
	if (timeout == NULL)
		return EXT_WIFI_FAIL;
	if (os_get_reltime(&timeout->time) < 0) {
		os_free(timeout);
		return EXT_WIFI_FAIL;
	}
	now_sec = timeout->time.sec;
	timeout->time.sec += (os_time_t)secs;
	if (timeout->time.sec < now_sec) {
		/*
		 * Integer overflow - assume long enough timeout to be assumed
		 * to be infinite, i.e., the timeout would never happen.
		 */
		os_free(timeout);
		return EXT_WIFI_OK;
	}
	timeout->time.usec += (os_time_t)usecs;
	while (timeout->time.usec >= 1000000) {
		timeout->time.sec++;
		timeout->time.usec -= 1000000;
	}
	timeout->eloop_data = eloop_data;
	timeout->user_data = user_data;
	timeout->handler = handler;
	wpa_trace_add_ref(timeout, eloop, eloop_data);
	wpa_trace_add_ref(timeout, user, user_data);
	wpa_trace_record(timeout);

	/* Maintain timeouts in order of increasing time */
	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
		if (os_reltime_before(&timeout->time, &tmp->time)) {
			dl_list_add(tmp->list.prev, &timeout->list);
			(void)os_mux_post(eloop.mutex_handle);
			return EXT_WIFI_OK;
		}
	}
	dl_list_add_tail(&eloop.timeout, &timeout->list);
	(void)os_mux_post(eloop.mutex_handle);
	return EXT_WIFI_OK;
}

static void eloop_remove_timeout(struct eloop_timeout *timeout)
{
	if (timeout == NULL)
		return;
	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	dl_list_del(&timeout->list);
	(void)os_mux_post(eloop.mutex_handle);

	wpa_trace_remove_ref(timeout, eloop, timeout->eloop_data);
	wpa_trace_remove_ref(timeout, user, timeout->user_data);
	os_free(timeout);
}

int eloop_cancel_timeout(eloop_timeout_handler handler,
			 void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout = NULL;
	struct eloop_timeout *prev = NULL;
	int removed = 0;

	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	dl_list_for_each_safe(timeout, prev, &eloop.timeout,
				  struct eloop_timeout, list) {
		if (timeout->handler == handler &&
			(timeout->eloop_data == eloop_data ||
			 eloop_data == ELOOP_ALL_CTX) &&
			(timeout->user_data == user_data ||
			 user_data == ELOOP_ALL_CTX)) {
			eloop_remove_timeout(timeout);
			removed++;
		}
	}
	(void)os_mux_post(eloop.mutex_handle);

	return removed;
}

int eloop_cancel_timeout_one(eloop_timeout_handler handler,
                             void *eloop_data, void *user_data,
                             struct os_reltime *remaining)
{
	struct eloop_timeout *timeout = NULL;
	struct eloop_timeout *prev = NULL;
	int removed = 0;
	struct os_reltime now;

	(void)os_get_reltime(&now);
	if (remaining == NULL)
		return EXT_WIFI_FAIL;
	remaining->sec = 0;
	remaining->usec = 0;

	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	dl_list_for_each_safe(timeout, prev, &eloop.timeout,
				  struct eloop_timeout, list) {
		if (timeout->handler == handler &&
			(timeout->eloop_data == eloop_data) &&
			(timeout->user_data == user_data)) {
			removed = 1;
			if (os_reltime_before(&now, &timeout->time))
				os_reltime_sub(&timeout->time, &now, remaining);
			eloop_remove_timeout(timeout);
			break;
		}
	}
	(void)os_mux_post(eloop.mutex_handle);
	return removed;
}

int eloop_is_timeout_registered(eloop_timeout_handler handler,
				void *eloop_data, void *user_data)
{
	struct eloop_timeout *tmp = NULL;

	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
		if (tmp->handler == handler &&
			tmp->eloop_data == eloop_data &&
			tmp->user_data == user_data) {
			(void)os_mux_post(eloop.mutex_handle);
			return ELOOP_TIMEOUT_REGISTERED;
		}
	}
	(void)os_mux_post(eloop.mutex_handle);
	return EXT_WIFI_OK;
}

int eloop_deplete_timeout(unsigned int req_secs, unsigned int req_usecs,
                          eloop_timeout_handler handler, void *eloop_data,
                          void *user_data)
{
	struct os_reltime now;
	struct os_reltime requested;
	struct os_reltime remaining;
	struct eloop_timeout *tmp = NULL;

	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
		if (tmp->handler == handler &&
			tmp->eloop_data == eloop_data &&
			tmp->user_data == user_data) {
			requested.sec = (os_time_t)req_secs;
			requested.usec = (os_time_t)req_usecs;
			(void)os_get_reltime(&now);
			os_reltime_sub(&tmp->time, &now, &remaining);
			if (os_reltime_before(&requested, &remaining)) {
				(void)eloop_cancel_timeout(handler, eloop_data,
							 user_data);
				(void)eloop_register_timeout(requested.sec,
							   requested.usec,
							   handler, eloop_data,
							   user_data);
				(void)os_mux_post(eloop.mutex_handle);
				return ELOOP_TIMEOUT_REGISTERED;
			}
			(void)os_mux_post(eloop.mutex_handle);
			return EXT_WIFI_OK;
		}
	}
	(void)os_mux_post(eloop.mutex_handle);
	return EXT_WIFI_FAIL;
}

int eloop_replenish_timeout(unsigned int req_secs, unsigned int req_usecs,
				eloop_timeout_handler handler, void *eloop_data,
				void *user_data)
{
	struct os_reltime now;
	struct os_reltime requested;
	struct os_reltime remaining;
	struct eloop_timeout *tmp = NULL;

	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
		if (tmp->handler == handler &&
			tmp->eloop_data == eloop_data &&
			tmp->user_data == user_data) {
			requested.sec = (os_time_t)req_secs;
			requested.usec = (os_time_t)req_usecs;
			(void)os_get_reltime(&now);
			os_reltime_sub(&tmp->time, &now, &remaining);
			if (os_reltime_before(&remaining, &requested)) {
				(void)eloop_cancel_timeout(handler, eloop_data,
							 user_data);
				(void)eloop_register_timeout(requested.sec,
							   requested.usec,
							   handler, eloop_data,
							   user_data);
				(void)os_mux_post(eloop.mutex_handle);
				return ELOOP_TIMEOUT_REGISTERED;
			}
			(void)os_mux_post(eloop.mutex_handle);
			return EXT_WIFI_OK;
		}
	}
	(void)os_mux_post(eloop.mutex_handle);
	return EXT_WIFI_FAIL;
}

static inline void eloop_timeout_event_process(struct eloop_timeout *timeout)
{
	void *eloop_data = timeout->eloop_data;
	void *user_data = timeout->user_data;
	eloop_timeout_handler handler = timeout->handler;
	eloop_remove_timeout(timeout);
	if (handler != NULL)
		handler(eloop_data, user_data);
}

static inline void eloop_run_process_timeout_event(void)
{
	struct eloop_timeout *timeout = NULL;
	struct os_reltime now;
	/* check if some registered timeouts have occurred */
	timeout = dl_list_first(&eloop.timeout, struct eloop_timeout, list);
	if (timeout != NULL) {
		(void)os_get_reltime(&now);
		if (!os_reltime_before(&now, &timeout->time))
			eloop_timeout_event_process(timeout);
	}
}

int eloop_run(eloop_task_type eloop_task)
{
	unsigned long time;
	struct os_reltime tv;
	struct os_reltime now;
	int start_flag = WPA_FLAG_ON;

	if (eloop_task >= ELOOP_MAX_TASK_TYPE_NUM)
		return EXT_WIFI_FAIL;

	if (eloop_start_running(eloop_task) != ELOOP_NOT_RUNNING)
		return ELOOP_ALREADY_RUNNING;

	while (global_eloop_is_running()) {
		unsigned int ret_flags;
		struct eloop_timeout *timeout = NULL;
		if (start_flag) {
			start_flag = WPA_FLAG_OFF;
			(void)os_event_write(g_wpa_event, WPA_EVENT_WPA_START_OK);
		}
#ifdef LOS_CONFIG_SUPPORT_CPUP
		uapi_cpup_load_check_proc(uapi_task_get_current_id(), LOAD_SLEEP_TIME_DEFAULT);
#endif /* LOS_CONFIG_SUPPORT_CPUP */
		timeout = dl_list_first(&eloop.timeout, struct eloop_timeout, list);
		if (timeout != NULL) {
			(void)os_get_reltime(&now);
			if (os_reltime_before(&now, &timeout->time))
				os_reltime_sub(&timeout->time, &now, &tv);
			else
				tv.sec = tv.usec = 0;
			time = (unsigned long)(tv.sec * 1000 + tv.usec / 1000);
			if (time)
				(void)os_event_read(eloop.levent, eloop.flags, &ret_flags, WIFI_WAITMODE_OR, time);
			else
				(void)os_event_poll(eloop.levent, eloop.flags, &ret_flags, WIFI_WAITMODE_OR);
		} else {
			(void)os_event_read(eloop.levent, eloop.flags, &ret_flags, WIFI_WAITMODE_OR, WIFI_WAIT_FOREVER);
		}

		(void)os_event_clear(eloop.levent, ~ret_flags);

		eloop_run_process_timeout_event();
		eloop_run_process_normal_event(&ret_flags, &eloop);

		if (eloop_terminate_wpa_process() == EXT_WIFI_OK) {
			ret_flags = 0;
			continue;
		}
	}
	(void)eloop_terminate_wpa_process();
	return EXT_WIFI_OK;
}

void eloop_destroy(eloop_task_type eloop_task)
{
	struct eloop_timeout *timeout = NULL;
	struct eloop_timeout *prev = NULL;
	struct os_reltime now;
	if ((eloop_task != ELOOP_TASK_WPA) && (eloop_task != ELOOP_TASK_HOSTAPD))
		return;
	if (global_eloop_is_running())
		return;
	(void)os_get_reltime(&now);
	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	dl_list_for_each_safe(timeout, prev, &eloop.timeout,
				  struct eloop_timeout, list) {
		eloop_remove_timeout(timeout);
	}
	(void)os_mux_post(eloop.mutex_handle);
	os_free(eloop.events);
	eloop.events = NULL;
	(void)os_mux_delete(eloop.mutex_handle);
}

int eloop_register_event(void *event, size_t event_size,
			 eloop_event_handler handler, void *eloop_data, void *user_data)
{
	unsigned int i;
	struct eloop_event *tmp = NULL;

	(void)event_size;
	if (eloop.event_count >= ELOOP_MAX_EVENTS)
		return EXT_WIFI_FAIL;

	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);

	for (i = 0; i < ELOOP_MAX_EVENTS; i++) {
		if (eloop.events[i].flag == 0) {
			tmp = &eloop.events[i];
			break;
		}
	}
	if ((i == ELOOP_MAX_EVENTS) || (tmp == NULL)) {
		(void)os_mux_post(eloop.mutex_handle);
		return EXT_WIFI_FAIL;
	}

	tmp->eloop_data = eloop_data;
	tmp->user_data = user_data;
	tmp->handler = handler;
	tmp->flag = 1 << i;
	dl_list_init(&tmp->message);

	eloop.flags |= tmp->flag;
	eloop.event_count++;

	if (event != NULL)
		*(struct eloop_event **)event = tmp;

	(void)os_mux_post(eloop.mutex_handle);
	return EXT_WIFI_OK;
}

void eloop_unregister_event(void *event, size_t size)
{
	int i;
	struct eloop_event *tmp = (struct eloop_event *)event;
	struct eloop_message *message = NULL;

	(void)size;
	if ((tmp == NULL) || (eloop.event_count == 0))
		return;
	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	for (i = 0; i < ELOOP_MAX_EVENTS; i++) {
		if (&eloop.events[i] == tmp)
			break;
	}
	if (i == ELOOP_MAX_EVENTS) {
		(void)os_mux_post(eloop.mutex_handle);
		return;
	}
	eloop.flags &= ~tmp->flag;

	tmp->flag = 0;
	tmp->handler = NULL;

	while (1) {
		message = dl_list_first(&tmp->message, struct eloop_message, list);
		if (message == NULL)
			break;

		dl_list_del(&message->list);
		os_free(message->buf);
		os_free(message);
	}

	eloop.event_count--;
	(void)os_mux_post(eloop.mutex_handle);
}

void eloop_unregister_cli_event(void *event, size_t size)
{
	int i;
	struct eloop_event *tmp = (struct eloop_event *)event;
	struct eloop_message *message = NULL;
	(void)size;
	if ((tmp == NULL) || (eloop.event_count == 0))
		return;

	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);
	for (i = 0; i < ELOOP_MAX_EVENTS; i++) {
		if (&eloop.events[i] == tmp)
			break;
	}
	if (i == ELOOP_MAX_EVENTS) {
		(void)os_mux_post(eloop.mutex_handle);
		return;
	}
	eloop.flags &= ~tmp->flag;

	tmp->flag = 0;
	tmp->handler = NULL;

	while (1) {
		message = dl_list_first(&tmp->message, struct eloop_message, list);
		if (message == NULL)
			break;

		dl_list_del(&message->list);
		if (message->buf != NULL)
			os_free(((struct ctrl_iface_event_buf *)(message->buf))->buf);
		os_free(message->buf);
		os_free(message);
	}
	eloop.event_count--;
	(void)os_mux_post(eloop.mutex_handle);
}

int eloop_post_event(void *event, void *buf, int set_event)
{
	unsigned int int_save;
	int i;
	struct eloop_event *tmp = (struct eloop_event *)event;
	struct eloop_message *message = NULL;

	(void)set_event;
	if (tmp == NULL)
		return EXT_WIFI_FAIL;

	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);

	for (i = 0; i < ELOOP_MAX_EVENTS; i++) {
		if (&eloop.events[i] == tmp)
			break;
	}
	if (i == ELOOP_MAX_EVENTS) {
		(void)os_mux_post(eloop.mutex_handle);
		return EXT_WIFI_FAIL;
	}
	if ((eloop.flags & (1 << i)) == 0) {
		(void)os_mux_post(eloop.mutex_handle);
		return EXT_WIFI_FAIL;
	}

	message = os_malloc(sizeof(struct eloop_message));
	if (message == NULL) {
		(void)os_mux_post(eloop.mutex_handle);
		return EXT_WIFI_FAIL;
	}
	message->buf = buf;
	dl_list_add_tail(&tmp->message, &message->list);
	(void)os_mux_post(eloop.mutex_handle);
	(void)os_event_write(eloop.levent, tmp->flag);
	return EXT_WIFI_OK;
}

void *eloop_read_event(void *event, int timeout)
{
	unsigned int int_save;
	void *ret = NULL;
	int i;
	struct eloop_event *tmp = (struct eloop_event *)event;
	struct eloop_message *message = NULL;

	(void)timeout;
	if (tmp == NULL)
		return NULL;

	(void)os_mux_pend(eloop.mutex_handle, WIFI_WAIT_FOREVER);

	for (i = 0; i < ELOOP_MAX_EVENTS; i++) {
		if (&eloop.events[i] == tmp)
			break;
	}
	if (i == ELOOP_MAX_EVENTS) {
		(void)os_mux_post(eloop.mutex_handle);
		return NULL;
	}
	if ((eloop.flags & (1 << i)) == 0) {
		(void)os_mux_post(eloop.mutex_handle);
		return NULL;
	}

	message = dl_list_first(&tmp->message, struct eloop_message, list);
	if (message != NULL) {
		ret = message->buf; // copy data before free.

		dl_list_del(&message->list);
		os_free(message);
	}
	(void)os_mux_post(eloop.mutex_handle);
	return ret;
}
