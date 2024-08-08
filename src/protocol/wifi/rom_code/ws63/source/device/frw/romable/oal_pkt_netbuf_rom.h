/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: pkt ram netbuf management interface.
 */

#ifndef OAL_PKT_NETBUF_ROM_H
#define OAL_PKT_NETBUF_ROM_H
#include "oal_net_pkt_rom.h"
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 用来保存内存块管理结构体oal_mem_struc，整个内存块的结构如下所示。          */
/* +-------------------+---------------------------------------+------------+ */
/* |          payload                         |dog tag|dbg flag|_return_addr| */
/* +-------------------+---------------------------------------+------------+ */
/* |                                          |2 byte | 2 byte |   4 byte   | */
/* +-------------------+---------------------------------------+------------+ */
#define OAL_DOG_TAG 0xdeadbeaf /* 狗牌，用于检测内存越界 */
#define OAL_DOG_FLAG 0xbeaf    /* 狗牌，用于检测内存越界 */

#ifdef _PRE_MEM_DEBUG
#define OAL_DOG_TAG_SIZE 4 /* 狗牌大小(4字节) */
#define OAL_RETURN_ADDR 4  /* 保存_return_address信息 */
#else
#define OAL_DOG_TAG_SIZE 0
#define OAL_RETURN_ADDR 0
#endif

typedef enum {
    OAL_PKT_TOTAL_CNT,
    OAL_PKT_USED_CNT,
    OAL_PKT_ALLOC_CNT,
    OAL_PKT_FAIL_CNT,

    OAL_PKT_GET_CNT_BUTT,
} oal_pkt_cnt_type;

/* 裸系统下netbuf子内存池结构体 */
typedef struct {
    osal_u16 free_cnt;     /* 本子内存池可用内存块数 */
    osal_u16 total_cnt;    /* 本子内存池内存块总数 */
    osal_u16 min_free_cnt; /* 本子内存池历史可用最小内存块数 */
    osal_u16 rsv_cnt;      /* 被保留的块数 */
    osal_u16 start_index;  /* 本子内存池paylaod的起始index */
    osal_u16 len;          /* 本子内存池的内存块长度真实整体长度 */
    osal_u32 pkt_addr;     /* 本子内存池pktram内存起始地址 */
    /* 记录oal_dmac_netbuf_stru可用内存索引表的栈顶元素，其内容为oal_dmac_netbuf_stru指针 */
    oal_dmac_netbuf_stru *free_stack;
} oal_netbuf_subpool_stru;

/* 子内存池配置结构体 */
typedef struct {
    osal_u16 size; /* 本子内存池内存块大小 */
    osal_u16 cnt;  /* 本子内存池内存块个数 */
} oal_mem_subpool_cfg_stru;

typedef struct {
    osal_u8 priority;
    osal_u8 subpool_id;
    osal_u8 end_subpool;
    osal_u8 rsv;
} oal_pkt_param;

typedef osal_void oal_pkt_find_blk_custom(osal_u8 *subpool_id);
osal_u32 oal_pkt_get_subpool_free_cnt(const osal_void *handler, osal_u8 subpool_id);
osal_u32 oal_pkt_get_subpool_used_cnt(const osal_void *handler, osal_u8 subpool_id);
osal_u32 oal_pkt_get_subpool_len(const osal_void *handler, osal_u8 subpool_id);

oal_netbuf_subpool_stru *oal_pkt_get_subpool_by_netbuf(const osal_void *handler, const oal_dmac_netbuf_stru *netbuf);
osal_void *oal_pkt_init_cfg(osal_u32 start_addr, osal_u32 end_addr, const oal_mem_subpool_cfg_stru *cfg_table,
    osal_u32 table_size);

oal_dmac_netbuf_stru *oal_pkt_netbuf_alloc(osal_void *handler, const oal_pkt_param *para,
    oal_pkt_find_blk_custom func, osal_u32 return_addr);
osal_void oal_pkt_print_netbuf_pool_info(const osal_void *handler);
osal_void oal_pkt_free_pool_safty(osal_void **handler);
osal_u32 oal_pkt_netbuf_free(osal_void *handler, oal_dmac_netbuf_stru *netbuf, osal_u32 return_addr);
osal_u8 oal_pkt_netbuf_valid_check(const osal_void *handler, const oal_dmac_netbuf_stru *netbuf);
osal_u32 oal_pkt_get_pool_cnt_by_type(const osal_void *handler, osal_u8 type);
osal_u32 oal_pkt_rsv(osal_void *handler, osal_u32 rsv_size, osal_u32 return_addr);
osal_void oal_pkt_restore(osal_void *handler, osal_u32 return_addr);
osal_void oal_pkt_clear_pool(osal_void *handler);
osal_u32 oal_pkt_get_pool_info(osal_void *handler, osal_u32 index, oal_mem_subpool_cfg_stru *cfg_table);
typedef osal_u32 (*oal_pkt_free_pool_cb)(osal_void **handler);
typedef osal_u32 (*oal_pkt_rsv_cb)(osal_void *handler, osal_u32 rsv_size);
typedef osal_u32 (*oal_pkt_restore_cb)(osal_void *handler);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
