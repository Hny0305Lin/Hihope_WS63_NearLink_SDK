/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT async notify \n
 */
 
#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
#include "at_product.h"
#include "at_base.h"
#include "at_msg.h"
#include "at_process.h"
#include "at_notify.h"
 
#define STRING_END_FLAG_LEN 1
 
typedef struct at_async_notify_t {
    char *rsp_string;
    uint16_t string_len;
    at_channel_id_t channel_id;
    struct at_async_notify_t *next;
} at_async_notify_t;
 
static at_async_notify_t *g_notify_list = NULL;
static void *g_notify_mutex_handle = NULL;
 
static void at_notify_append_node(at_async_notify_t *node)
{
    if (g_notify_list == NULL) {
        g_notify_list = node;
    } else {
        at_async_notify_t *next_node = g_notify_list;
        while (next_node->next != NULL) {
            next_node = next_node->next;
        }
        next_node->next = node;
    }
}
 
/* Ensured that the input is not null */
static void at_notify_free_node(at_async_notify_t *node)
{
    if (node->rsp_string != NULL) {
        at_free(node->rsp_string);
        node->rsp_string = NULL;
    }
 
    at_free(node);
}
 
static at_async_notify_t* at_notify_find_next_node(void)
{
    if (g_notify_list == NULL) {
        return NULL;
    }
 
    at_async_notify_t *node = g_notify_list;
    g_notify_list = g_notify_list->next;
    return node;
}
 
static errcode_t at_notify_send_msg(void)
{
    at_msg_block_t msg;
    msg.type = AT_CMD_URC_REPORT_MSG;
 
    return at_msg_send(&msg);
}

errcode_t uapi_at_urc_to_channel(at_channel_id_t channel_id, const char *msg, uint32_t msg_len)
{
    if (msg == NULL || msg_len == 0) {
        return ERRCODE_INVALID_PARAM;
    }
 
    at_async_notify_t *node = at_malloc(sizeof(at_async_notify_t));
    if (node == NULL) {
        return ERRCODE_MALLOC;
    }
 
    node->rsp_string = at_malloc(msg_len + STRING_END_FLAG_LEN);
    if (node->rsp_string == NULL) {
        return ERRCODE_MALLOC;
    }
    node->string_len = msg_len;
    node->channel_id = channel_id;
    memset_s(node->rsp_string, msg_len + STRING_END_FLAG_LEN, 0, msg_len + STRING_END_FLAG_LEN);
    if (memcpy_s(node->rsp_string, msg_len, msg, msg_len) != EOK) {
        at_free(node->rsp_string);
        at_free(node);
        return ERRCODE_MEMCPY;
    }
    node->next = NULL;
    
    at_mutex_acquire(g_notify_mutex_handle);
    at_notify_append_node(node);
    at_mutex_release(g_notify_mutex_handle);
    return at_notify_send_msg();
}
 
void at_notify_process_list(void)
{
    at_mutex_acquire(g_notify_mutex_handle);
    at_async_notify_t *node = at_notify_find_next_node();
    while (node != NULL) {
        uapi_at_report_to_single_channel(node->channel_id, node->rsp_string);
        at_notify_free_node(node);
        node = at_notify_find_next_node();
    }
    at_mutex_release(g_notify_mutex_handle);
}
 
void at_notify_init(void)
{
    g_notify_mutex_handle = at_mutex_create();
}
#endif
