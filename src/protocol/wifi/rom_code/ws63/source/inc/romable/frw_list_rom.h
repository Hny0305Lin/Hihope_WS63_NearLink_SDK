/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: wifi dmac os adapt layer
 */

#ifndef __FRW_LIST_ROM_H__
#define __FRW_LIST_ROM_H__

#include "osal_types.h"
#include "osal_list.h"

/* 删除链表中的指定节点,不负责释放，不判断链表是否为空，请注意 */
static INLINE__ void osal_dlist_delete_entry(struct osal_list_head * const entry)
{
    if (osal_unlikely((entry->next == OSAL_NULL) || (entry->prev == OSAL_NULL))) {
        return;
    }

    osal___list_del(entry->prev, entry->next);
    entry->next = OSAL_NULL;
    entry->prev = OSAL_NULL;
}

/* 将链表2 加入链表1的尾部 */
static INLINE__ void osal_dlist_join_tail(struct osal_list_head * const head1, const struct osal_list_head *head2)
{
    struct osal_list_head *dlist1_tail;
    struct osal_list_head *dlist2_tail;
    struct osal_list_head *dlist2_first;

    dlist1_tail = head1->prev;
    dlist2_tail = head2->prev;
    dlist2_first = head2->next;
    dlist1_tail->next = dlist2_first;
    dlist2_first->prev = dlist1_tail;
    head1->prev = dlist2_tail;
    dlist2_tail->next = head1;
}

/* 将链表2 加入链表1的头部 也可用于将新链表 加入链表的指定节点后 */
static INLINE__ void osal_dlist_join_head(struct osal_list_head * const head1, const struct osal_list_head *head2)
{
    struct osal_list_head *head2_first = OSAL_NULL;
    struct osal_list_head *head2_tail = OSAL_NULL;
    struct osal_list_head *head1_first = OSAL_NULL;

    if (osal_list_empty(head2) == OSAL_TRUE) {
        return;
    }

    head2_first = head2->next;
    head2_tail = head2->prev;
    head1_first = head1->next;

    head1->next = head2_first;
    head2_first->prev = head1;
    head2_tail->next = head1_first;
    head1_first->prev = head2_tail;
}

#endif // endif __FRW_LIST_ROM_H__

