/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Implementation for wifi os api
 */

#if defined(CONFIG_OS_OSAL)

#include "stdio.h"
#include "securec.h"
#include "wifi_osdep.h"
#include "std_def.h"

static osal_event g_wifi_event[(int)WIFI_EVENT_MAX] = {0};

unsigned long long os_tick_cnt_get(void)
{
	return osal_get_jiffies();
}

void os_intlock(unsigned int *int_save)
{
	if (int_save == NULL) {
		return;
	}
	*int_save = osal_irq_lock();
}

void os_intrestore(unsigned int int_save)
{
	osal_irq_restore(int_save);
}

/* os task api */
void os_task_lock(unsigned int *int_save)
{
	(void)int_save;
	osal_kthread_lock();
}

void os_task_unlock(unsigned int int_save)
{
	(void)int_save;
	osal_kthread_unlock();
}

void os_task_delay(unsigned int ms)
{
	(void)osal_msleep(ms);
}

int os_task_create(unsigned int *task_id, wifi_task_attr *attr)
{
	if (task_id == NULL || attr == NULL) {
		return WIFI_OS_FAIL;
	}

	osal_task *task = osal_kthread_create((osal_kthread_handler)attr->task_entry, (void *)attr->arg,
										   attr->task_name, attr->stack_size);
	if (task == NULL) {
		WIFI_ERR_LOG("task %s create failed.\n", attr->task_name);
		return WIFI_OS_FAIL;
	}

	(void)osal_kthread_set_priority(task, attr->task_prio);
	*task_id = (uintptr_t)task;
	return WIFI_OS_OK;
}

void os_task_delete(unsigned int task_id)
{
	osal_kthread_destroy((osal_task *)task_id, 0);
}

/* os event api */
int os_event_init(unsigned int *id, wifi_event_enum num)
{
	if (num >= WIFI_EVENT_MAX || id == NULL) {
		return WIFI_OS_FAIL;
	}
	if (g_wifi_event[num].event != NULL) {
		goto exit;
	}
	if (osal_event_init(&g_wifi_event[num]) != OSAL_SUCCESS) {
		WIFI_ERR_LOG("event init failed.\n");
		return WIFI_OS_FAIL;
	}
exit:
	*id = (unsigned int)num;
	return WIFI_OS_OK;
}

int os_event_read(unsigned int id, unsigned int mask, unsigned int *event_bits,
	unsigned int flag, unsigned int timeout_ms)
{
	if (id >= WIFI_EVENT_MAX || event_bits == NULL) {
		*event_bits = 0;
		return WIFI_OS_FAIL;
	}

	int ret = osal_event_read(&g_wifi_event[id], mask, timeout_ms, flag);
	/* Currently unable to sense whether it has timed out */
	if (ret == OSAL_FAILURE) {
		*event_bits = 0;
		return WIFI_OS_FAIL;
	}
	*event_bits = (unsigned int)ret;
	return WIFI_OS_OK;
}

int os_event_poll(unsigned int id, unsigned int mask, unsigned int *event_bits, unsigned int flag)
{
	if (id >= WIFI_EVENT_MAX || event_bits == NULL) {
		*event_bits = 0;
		return WIFI_OS_FAIL;
	}

	/* osal dont support event poll */
	return os_event_read(id, mask, event_bits, flag, 0);
}

int os_event_write(unsigned int id, unsigned int events)
{
	if (id >= WIFI_EVENT_MAX) {
		return WIFI_OS_FAIL;
	}

	if (osal_event_write(&g_wifi_event[id], events) != OSAL_SUCCESS) {
		WIFI_ERR_LOG("event write failed.\n");
		return WIFI_OS_FAIL;
	}
	return WIFI_OS_OK;
}

int os_event_clear(unsigned int id, unsigned int events)
{
	if (id >= WIFI_EVENT_MAX) {
		return WIFI_OS_FAIL;
	}

	if (osal_event_clear(&g_wifi_event[id], ~events) != OSAL_SUCCESS) {
		WIFI_ERR_LOG("event clear failed.\n");
		return WIFI_OS_FAIL;
	}
	return WIFI_OS_OK;
}

int os_event_del(unsigned int id)
{
	if (id >= WIFI_EVENT_MAX) {
		return WIFI_OS_FAIL;
	}

	if (osal_event_destroy(&g_wifi_event[id]) != OSAL_SUCCESS) {
		WIFI_ERR_LOG("event destroy failed.\n");
		return WIFI_OS_FAIL;
	}
	return WIFI_OS_OK;
}

/* os mux api */
int os_mux_create(unsigned int *mux_id)
{
	if (mux_id == NULL) {
		return WIFI_OS_FAIL;
	}

	osal_mutex *mux = osal_kmalloc(sizeof(osal_mutex), OSAL_GFP_KERNEL);
	if (mux == NULL) {
		return WIFI_OS_FAIL;
	}
	(void)memset_s(mux, sizeof(osal_mutex), 0, sizeof(osal_mutex));

	if (osal_mutex_init(mux) != OSAL_SUCCESS) {
		osal_kfree(mux);
		WIFI_ERR_LOG("mux init failed.\n");
		return WIFI_OS_FAIL;
	}
	*mux_id = (unsigned int)(uintptr_t)mux;
	return WIFI_OS_OK;
}

int os_mux_pend(unsigned int mux_id, unsigned int timeout_ms)
{
	osal_mutex *mux = (osal_mutex *)(uintptr_t)(mux_id);

	if (osal_mutex_lock_timeout(mux, timeout_ms) != OSAL_SUCCESS) {
		WIFI_ERR_LOG("mux pend failed.\n");
		return WIFI_OS_FAIL;
	}
	return WIFI_OS_OK;
}

int os_mux_post(unsigned int mux_id)
{
	osal_mutex *mux = (osal_mutex *)(uintptr_t)(mux_id);

	osal_mutex_unlock(mux);

	return WIFI_OS_OK;
}

int os_mux_delete(unsigned int mux_id)
{
	osal_mutex *mux = (osal_mutex *)(uintptr_t)(mux_id);

	osal_mutex_destroy(mux);
	osal_kfree(mux);

	return WIFI_OS_OK;
}

#endif /* CONFIG_OS_OSAL */
