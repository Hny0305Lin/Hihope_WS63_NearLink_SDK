/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: List Header
 */

#ifndef LIST_H
#define LIST_H

#include "common_def.h"

/** @defgroup DRIVER_PLATFORM_LIST CHIP Platform LIST Driver
  * @ingroup DRIVER_PLATFORM
  * @{
  */
#define LIST_POISON1 ((void *)0x10100100)
#define LIST_POISON2 ((void *)0x20200200)

struct list_head {
    struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *node)
{
    node->next = node;
    node->prev = node;
}

static inline void list_add_node(struct list_head *cur, struct list_head *prev, struct list_head *next)
{
    next->prev = cur;
    cur->next = next;
    cur->prev = prev;
    prev->next = cur;
}

static inline void list_add(struct list_head *cur, struct list_head *head)
{
    list_add_node(cur, head, head->next);
}

static inline void list_add_tail(struct list_head *cur, struct list_head *head)
{
    list_add_node(cur, head->prev, head);
}

static inline void list_del_node(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *node)
{
    list_del_node(node->prev, node->next);
    node->next = (struct list_head*)LIST_POISON1;
    node->prev = (struct list_head*)LIST_POISON2;
}

static inline int list_empty(const struct list_head *list)
{
    return list->next == list;
}

#ifndef LOSCFG_LIB_LIBC
#ifndef offsetof
#define offsetof(type, member) ((unsigned int)&((type *)0)->member)
#endif
#endif

#ifndef container_of
#define container_of(cur, type, member) ((type *)((char *)(cur) - offsetof(type, member)))
#endif

#define list_entry(cur, type, member) container_of(cur, type, member)

#define list_first_entry(cur, type, member) container_of((cur)->next, type, member)

#define list_next_entry(cur, type, member) container_of((cur)->member.next, type, member)

#define list_for_each_entry(cur, node_type, head, member)   \
    for ((cur) = list_first_entry(head, node_type, member); \
         &(cur)->member != (head);                          \
         (cur) = list_next_entry(cur, node_type, member))

#define list_for_each_entry_safe(cur, cur_next, node_type, head, member) \
    for ((cur) = list_first_entry(head, node_type, member),              \
         (cur_next) = list_next_entry(cur, node_type, member);           \
         &(cur)->member != (head);                                       \
         (cur) = (cur_next), (cur_next) = list_next_entry(cur, node_type, member))

/**
  * @}
  */
#endif
