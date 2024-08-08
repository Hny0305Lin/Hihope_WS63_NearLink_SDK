/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: service_list.h 的头文件
 */

#ifndef EXT_WIFI_SERVICE_ENVENT_H
#define EXT_WIFI_SERVICE_ENVENT_H
#include "wifi_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* Config Methods */
#define WPS_USBA 0x0001
#define WPS_ETHERNET 0x0002
#define WPS_LABEL 0x0004
#define WPS_DISPLAY 0x0008
#define WPS_EXT_NFC_TOKEN 0x0010
#define WPS_INT_NFC_TOKEN 0x0020
#define WPS_NFC_INTERFACE 0x0040
#define WPS_PUSHBUTTON 0x0080
#define WPS_KEYPAD 0x0100
#define WPS_VIRT_PUSHBUTTON 0x0280
#define WPS_PHY_PUSHBUTTON 0x0480
#define WPS_P2PS 0x1000
#define WPS_VIRT_DISPLAY 0x2008
#define WPS_PHY_DISPLAY 0x4008

#define off_set_of(type, member) ((unsigned int)&((type *)0)->member)

#define dl_list_entry(item, type, member) \
    ((type *)(void *)((char *)(item) - off_set_of(type, member)))

#define dl_list_for_each_entry(item, list, type, member)         \
    for ((item) = dl_list_entry((list)->next, type, member);       \
        &(item)->member != (list);                               \
        (item) = dl_list_entry((item)->member.next, type, member))

#define dl_list_for_each_entry_safe(item, next, list, type, member)               \
    for ((item) = dl_list_entry((list)->next, type, member),                     \
        (next) = dl_list_entry((item)->member.next, type, member);              \
        &(item)->member != (list);                                                   \
        (item) = (next), (next) = dl_list_entry((item)->member.next, type, member))

#define dl_list_first(object) ((object)->next)

#define service_error_log0(msg_level, fmt)         printf(fmt"\r\n")
#define service_error_log1(msg_level, fmt, p1)         printf(fmt"\r\n", p1)
#define service_error_log2(msg_level, fmt, p1, p2)         printf(fmt"\r\n", p1, p2)
#define service_error_log3(msg_level, fmt, p1, p2, p3)         printf(fmt"\r\n", p1, p2, p3)
#define service_error_log4(msg_level, fmt, p1, p2, p3, p4)         printf(fmt"\r\n", p1, p2, p3, p4)

enum {
    SERVICE_EXCESSIVE, SERVICE_MSGDUMP, SERVICE_DEBUG, SERVICE_INFO, SERVICE_WARNING, SERVICE_ERROR
};

typedef struct dl_list {
    struct dl_list *prev; /* < Current node's pointer to the previous node */
    struct dl_list *next; /* < Current node's pointer to the next node */
} dl_list;

typedef struct service_event_cb {
    dl_list   node;
    wifi_event_stru service_cb;
} service_event_cb;

static inline void list_init(dl_list *list)
{
    list->next = list;
    list->prev = list;
}

static inline void list_delinit(dl_list *list)
{
    list->next->prev = list->prev;
    list->prev->next = list->next;
    list_init(list);
}

static inline void list_add_node(dl_list *list, dl_list *node)
{
    node->next = list->next;
    node->prev = list;
    list->next->prev = node;
    list->next = node;
}

static inline void list_delete_node(dl_list *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->next = NULL;
    node->prev = NULL;
}

static inline void list_tail_insert(dl_list *list, dl_list *node)
{
    list_add_node(list->prev, node);
}

static inline int list_empty(const dl_list *list)
{
    return (list->next == list);
}
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
