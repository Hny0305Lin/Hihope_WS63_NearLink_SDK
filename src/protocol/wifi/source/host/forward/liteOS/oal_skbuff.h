/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: skb function.
 * Author:
 * Create: 2022-04-07
 */

#ifndef LITEOS_SKBUFF_H
#define LITEOS_SKBUFF_H

/*****************************************************************************
    1 其他头文件包含
*****************************************************************************/
#include <stdlib.h>
#ifndef FREERTOS_DEFINE
#include <linux/spinlock.h>
#endif
#include "osal_adapt.h"
#include "td_type.h"
#include "td_base.h"
#include "osal_types.h"
#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
#include "oal_net_pkt_rom.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    2 宏定义
*****************************************************************************/
/* Don't change this without changing skb_csum_unnecessary! */
#define CHECKSUM_NONE           0
#define CHECKSUM_UNNECESSARY    1
#define CHECKSUM_COMPLETE       2
#define CHECKSUM_PARTIAL        3

#define L1_CACHE_BYTES          (1 << 5)
#define SMP_CACHE_BYTES         L1_CACHE_BYTES
#define skb_data_align(x)       (((x) + (SMP_CACHE_BYTES - 1)) & ~(SMP_CACHE_BYTES - 1))

/* return minimum truesize of one skb containing X bytes of data */
#define skb_truesize(x)         ((x) + skb_data_align(sizeof(struct sk_buff)))

#ifndef NET_SKB_PAD
#define NET_SKB_PAD     80
#endif

#define NUMA_NO_NODE    (-1)

#define USB_CACHE_ALIGN_SIZE 32

#define SKB_ALLOC_FCLONE    0x01
#define SKB_ALLOC_RX        0x02

#ifndef OFFSETOF
#ifdef HAVE_PCLINT_CHECK
#define oal_offsetof(type, member) 0
#else
#define oal_offsetof(type, member) ((long) &((type *) 0)->member)
#endif
#endif

typedef td_u32 gfp_t;
typedef td_u32 sk_buff_data_t;

/*****************************************************************************
    3 结构体定义
*****************************************************************************/
struct sk_buff_head {
    /* These two members must be first. */
    struct sk_buff  *next;
    struct sk_buff  *prev;

    td_u32          qlen;
    osal_spinlock   lock;
};

struct sk_buff {
    /* These two members must be first. */
    struct sk_buff *next;
    struct sk_buff *prev;
#ifdef _PRE_LWIP_ZERO_COPY_MEM_ALLOC_PKT_BUF
    oal_dmac_netbuf_stru *pkt_buf;
#endif
    td_void        *dev;               /* for hwal_netif_rx */
    td_u32          len;
    td_u32          data_len;
    td_u16          queue_mapping;

    /* These elements must be at the end, see alloc_skb() for details. */
    sk_buff_data_t  tail;
    sk_buff_data_t  end;

    td_s8           cb[48];  /* 48: SIZE(0..48) */
    td_u8          *head;
    td_u8          *data;

    td_u32          truesize;
    td_u32          priority;
    osal_atomic       users;

    /* use for lwip_pbuf zero_copy:actual start addr of memory space */
    td_u8           *mem_head;
    td_u32          protocol;

    td_u16          mac_header;
    td_u8           resv2;
    td_u8           resv;
#ifdef TIMESTAMP_RECORD_DEBUG
    td_u32          times[19]; /* timestamp for debug */
#endif
};

typedef struct sk_buff oal_netbuf_stru;
typedef struct sk_buff_head oal_netbuf_head_stru;

/*****************************************************************************
    4 函数声明
*****************************************************************************/
td_void skb_trim(struct sk_buff *skb, td_u32 len);
struct sk_buff *skb_unshare(struct sk_buff *skb, td_u32 pri);
td_s32 pskb_expand_head(struct sk_buff *skb, td_u32 nhead, td_u32 ntail, td_s32 gfp_mask);
struct sk_buff *alloc_skb(td_u32 size);
struct sk_buff *skb_dequeue(struct sk_buff_head *list);
td_void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);
td_void dev_kfree_skb(struct sk_buff *skb);
struct sk_buff *dev_alloc_skb(td_u32 length);
td_u8 *skb_put(struct sk_buff *skb, td_u32 len);
#define dev_kfree_skb_any(a) dev_kfree_skb(a)

/*****************************************************************************
    5 内联函数
*****************************************************************************/
static inline td_void _skb_queue_head_init(struct sk_buff_head *list)
{
    list->prev = list->next = (struct sk_buff *)(uintptr_t)list;
    list->qlen = 0;
}

static inline td_void skb_queue_head_init(struct sk_buff_head *list)
{
#ifndef FREERTOS_DEFINE
    /* liteos list操作不用osal_spin_lock_init, 因为给lock申请的内存 可能没有流程保障释放 */
    spin_lock_init(&list->lock);
#else
    /* 其他系统下 init函数中不会给lock申请内存 */
    osal_spin_lock_init(&list->lock);
#endif
    _skb_queue_head_init(list);
}

static inline td_void skb_reset_tail_pointer(struct sk_buff *skb)
{
    skb->tail = (sk_buff_data_t)(skb->data - skb->head);
}

static inline td_u8 *skb_tail_pointer(const struct sk_buff *skb)
{
    td_u8 *phead = skb->head;
    return (phead + skb->tail);
}

static inline td_s32 skb_queue_empty(const struct sk_buff_head *list)
{
    return list->next == (struct sk_buff *)(uintptr_t)list;
}

static inline td_void skb_reserve(struct sk_buff *skb, td_u32 len)
{
    skb->data += len;
    skb->tail += len;
}

static inline struct sk_buff *_dev_alloc_skb(td_u32 length)
{
    struct sk_buff *skb = alloc_skb(length + NET_SKB_PAD);
    if (skb != NULL) {
        skb_reserve(skb, NET_SKB_PAD);
    }
    return skb;
}

static inline td_void skb_unlink(struct sk_buff *skb, struct sk_buff_head *list)
{
    struct sk_buff *next = NULL;
    struct sk_buff *prev = NULL;

    list->qlen--;
    next       = skb->next;
    prev       = skb->prev;
    skb->next  = skb->prev = NULL;
    next->prev = prev;
    prev->next = next;
}

static inline struct sk_buff *skb_get(struct sk_buff *skb)
{
    osal_adapt_atomic_inc(&skb->users);
    return skb;
}

static inline struct sk_buff *skb_peek(const struct sk_buff_head *list_)
{
    struct sk_buff *list = ((const struct sk_buff *)(uintptr_t)list_)->next;
    if (list == (struct sk_buff *)(uintptr_t)list_) {
        list = NULL;
    }
    return list;
}

static inline struct sk_buff *skb_peek_next(const struct sk_buff *skb, const struct sk_buff_head *list_)
{
    struct sk_buff *next = skb->next;

    if (next == (struct sk_buff *)(uintptr_t)list_) {
        next = NULL;
    }
    return next;
}

static inline struct sk_buff *skb_peek_tail(const struct sk_buff_head *list_)
{
    struct sk_buff *skb = list_->prev;

    if (skb == (struct sk_buff *)(uintptr_t)list_) {
        skb = NULL;
    }
    return skb;
}

static inline struct sk_buff *_skb_dequeue(struct sk_buff_head *list)
{
    struct sk_buff *skb = skb_peek(list);
    if (skb) {
        skb_unlink(skb, list);
    }
    return skb;
}

static inline struct sk_buff *_skb_dequeue_tail(struct sk_buff_head *list)
{
    struct sk_buff *skb = skb_peek_tail(list);
    if (skb) {
        skb_unlink(skb, list);
    }
    return skb;
}

static inline td_s32 skb_headlen(const struct sk_buff *skb)
{
    return (td_s32)(skb->len - skb->data_len);
}

static inline td_void _skb_insert(struct sk_buff *newsk,
                                  struct sk_buff *prev, struct sk_buff *next,
                                  struct sk_buff_head *list)
{
    newsk->next = next;
    newsk->prev = prev;
    next->prev  = prev->next = newsk;
    list->qlen++;
}

static inline td_void _skb_queue_before(struct sk_buff_head *list, struct sk_buff *next, struct sk_buff *newsk)
{
    _skb_insert(newsk, next->prev, next, list);
}

static inline td_void __skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk)
{
    _skb_queue_before(list, (struct sk_buff *)(uintptr_t)list, newsk);
}

static inline td_void _skb_queue_splice(const struct sk_buff_head *list, struct sk_buff *prev, struct sk_buff *next)
{
    struct sk_buff *first = list->next;
    struct sk_buff *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}

static inline td_void skb_queue_splice(const struct sk_buff_head *list, struct sk_buff_head *head)
{
    if (skb_queue_empty(list) == 0) {
        _skb_queue_splice(list, (struct sk_buff *) head, head->next);
        head->qlen += list->qlen;
    }
}

static inline td_void skb_queue_splice_tail_init(struct sk_buff_head *list, struct sk_buff_head *head)
{
    if (skb_queue_empty(list) == 0) {
        _skb_queue_splice(list, head->prev, (struct sk_buff *) head);
        head->qlen += list->qlen;
        _skb_queue_head_init(list);
    }
}

static inline td_void skb_queue_splice_init(struct sk_buff_head *list, struct sk_buff_head *head)
{
    if (skb_queue_empty(list) == 0) {
        _skb_queue_splice(list, (struct sk_buff *)(uintptr_t)head, head->next);
        head->qlen += list->qlen;
        _skb_queue_head_init(list);
    }
}

static inline td_u8 *skb_pull(struct sk_buff *skb, td_u32 len)
{
    skb->len -= len;
    return skb->data += len;
}

static inline td_s32 skb_headroom(const struct sk_buff *skb)
{
    return (td_s32)(skb->data - skb->head);
}

static inline bool skb_is_nonlinear(const struct sk_buff *skb)
{
    return skb->data_len;
}

static inline td_void skb_set_tail_pointer(struct sk_buff *skb, const td_u32 offset)
{
    skb_reset_tail_pointer(skb);
    skb->tail += offset;
}

static inline td_void _skb_trim(struct sk_buff *skb, td_u32 len)
{
    if (skb_is_nonlinear(skb)) {
        return;
    }
    skb->len = len;
    skb_set_tail_pointer(skb, len);
}

static inline td_u8 *skb_push(struct sk_buff *skb, td_u32 len)
{
    if (skb->data - len < skb->head) {
        return NULL;
    }

    skb->data -= len;
    skb->len  += len;
    return skb->data;
}

static inline td_u32 skb_tailroom(const struct sk_buff *skb)
{
    return skb_is_nonlinear(skb) ? 0 : skb->end - skb->tail;
}

static inline bool skb_queue_is_last(const struct sk_buff_head *list, const struct sk_buff *skb)
{
    return skb->next == (struct sk_buff *)list;
}

static inline td_u8 *skb_end_pointer(const struct sk_buff *skb)
{
    return skb->head + skb->end;
}

static inline td_u32 skb_end_offset(const struct sk_buff *skb)
{
    return skb->end;
}

static inline td_void _skb_queue_after(struct sk_buff_head *list, struct sk_buff *prev, struct sk_buff *newsk)
{
    _skb_insert(newsk, prev, prev->next, list);
}

static inline td_void skb_queue_head(struct sk_buff_head *list, struct sk_buff *newsk)
{
    _skb_queue_after(list, (struct sk_buff *)(uintptr_t)list, newsk);
}

static inline td_void skb_set_queue_mapping(struct sk_buff *skb, td_u16 queue_mapping)
{
    skb->queue_mapping = queue_mapping;
}

static inline td_u16 skb_get_queue_mapping(const struct sk_buff *skb)
{
    return skb->queue_mapping;
}

static inline td_void skb_copy_queue_mapping(struct sk_buff *to, const struct sk_buff *from)
{
    to->queue_mapping = from->queue_mapping;
}

static inline td_u32 skb_queue_len(const struct sk_buff_head *list_)
{
    return list_->qlen;
}

#endif  /* LITEOS_SKBUFF_H */
