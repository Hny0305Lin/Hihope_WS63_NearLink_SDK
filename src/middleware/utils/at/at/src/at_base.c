/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT base source \n
 */

#include "at_base.h"

static at_base_api_t g_at_base_api = {0};
static bool g_at_init_status = false;
#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
static void *g_at_timer_handler = NULL;
#endif

void at_base_toupper(char *str, uint32_t len)
{
    char *ch = str;
    for (uint32_t index = 0; index < len; index++) {
        if (*ch <= 'z' && *ch >= 'a') {
            *ch = (char)((uint8_t)*ch - (uint8_t)'a' + (uint8_t)'A');
        }
        ch++;
    }
}

static errcode_t at_base_api_register_base_api_check(at_base_api_t base_api)
{
    if ((base_api.msg_queue_create_func == NULL) ||
        (base_api.msg_queue_read_func == NULL) ||
        (base_api.msg_queue_write_func == NULL) ||
        (base_api.malloc_func == NULL) ||
        (base_api.free_func == NULL) ||
        (base_api.task_pause_func == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

#ifdef CONFIG_AT_SUPPORT_CMD_ATTR
    if (base_api.cmd_attr_func == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
#endif
    return ERRCODE_SUCC;
}

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
static errcode_t at_base_api_register_time_api_check(at_base_api_t *base_api)
{
    if ((base_api->timer_create_func == NULL) ||
        (base_api->timer_delete_func == NULL) ||
        (base_api->timer_start_func == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }
    return ERRCODE_SUCC;
}
#endif

#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
static errcode_t at_base_api_register_notify_api_check(at_base_api_t *base_api)
{
    if ((base_api->create_mutex_func == NULL) ||
        (base_api->acquire_mutex_func == NULL) ||
        (base_api->release_mutex_func == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }
    return ERRCODE_SUCC;
}
#endif

errcode_t uapi_at_base_api_register(at_base_api_t base_api)
{
    errcode_t err = ERRCODE_SUCC;
    err = at_base_api_register_base_api_check(base_api);
    if (err != ERRCODE_SUCC) {
        return err;
    }

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
    err = at_base_api_register_time_api_check(&base_api);
    if (err != ERRCODE_SUCC) {
        return err;
    }
#endif

#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
    err = at_base_api_register_notify_api_check(&base_api);
    if (err != ERRCODE_SUCC) {
        return err;
    }
#endif

    if (memcpy_s(&g_at_base_api, sizeof(at_base_api_t), &base_api, sizeof(at_base_api_t)) != EOK) {
        return ERRCODE_MEMCPY;
    }
    g_at_init_status = true;
    return ERRCODE_SUCC;
}

bool at_base_is_at_init(void)
{
    return g_at_init_status;
}

void* at_malloc(uint32_t size)
{
    if (g_at_base_api.malloc_func != NULL) {
        return g_at_base_api.malloc_func(size);
    }
    return NULL;
}

void at_free(void *addr)
{
    if (g_at_base_api.free_func != NULL) {
        g_at_base_api.free_func(addr);
    }
    return;
}

void at_msg_queue_create(uint32_t msg_count, uint32_t msg_size, unsigned long *queue_id)
{
    if (g_at_base_api.msg_queue_create_func != NULL) {
        g_at_base_api.msg_queue_create_func(msg_count, msg_size, queue_id);
    }
    return;
}

uint32_t at_msg_queue_write(unsigned long queue_id, void *msg_ptr, uint32_t msg_size, uint32_t timeout)
{
    if (g_at_base_api.msg_queue_write_func != NULL) {
        return g_at_base_api.msg_queue_write_func(queue_id, msg_ptr, msg_size, timeout);
    }
    return 0;
}

uint32_t at_msg_queue_read(unsigned long queue_id, void *buf_ptr, uint32_t *buf_size, uint32_t timeout)
{
    if (g_at_base_api.msg_queue_read_func != NULL) {
        return g_at_base_api.msg_queue_read_func(queue_id, buf_ptr, buf_size, timeout);
    }
    return 0;
}

void at_yield(void)
{
    if (g_at_base_api.task_pause_func != NULL) {
        g_at_base_api.task_pause_func();
    }
    return;
}

void at_log(const char *buf, uint16_t buf_size, uint8_t level)
{
#ifndef CONFIG_AT_SUPPORT_LOG
    unused(buf);
    unused(buf_size);
    unused(level);
#else
    if (g_at_base_api.log_func != NULL) {
        g_at_base_api.log_func(buf, buf_size, level);
    }
    return;
#endif
}

bool at_cmd_attr(uint16_t attr)
{
#ifdef CONFIG_AT_SUPPORT_CMD_ATTR
    if (g_at_base_api.cmd_attr_func != NULL) {
        return g_at_base_api.cmd_attr_func(attr);
    }
    return true;
#else
    unused(attr);
    return true;
#endif
}

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
void at_timer_delete(void)
{
    if ((g_at_timer_handler != NULL) && (g_at_base_api.timer_delete_func != NULL)) {
        g_at_base_api.timer_delete_func(g_at_timer_handler);
        g_at_timer_handler = NULL;
    }
    return;
}
 
void at_timer_start(uint32_t time_us, at_timer_callback_func_t call_back, void *argument)
{
    at_timer_delete();
    if ((g_at_base_api.timer_create_func != NULL) && (g_at_base_api.timer_start_func != NULL)) {
        g_at_timer_handler = g_at_base_api.timer_create_func(call_back, argument);
        g_at_base_api.timer_start_func(g_at_timer_handler, time_us);
    }
    return;
}
#endif
 
#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
void* at_mutex_create(void)
{
    if (g_at_base_api.create_mutex_func != NULL) {
        return g_at_base_api.create_mutex_func();
    }
    return NULL;
}
 
void at_mutex_acquire(void *handle)
{
    if (g_at_base_api.acquire_mutex_func != NULL) {
        g_at_base_api.acquire_mutex_func(handle);
    }
    return;
}
 
void at_mutex_release(void *handle)
{
    if (g_at_base_api.release_mutex_func != NULL) {
        g_at_base_api.release_mutex_func(handle);
    }
    return;
}
#endif
