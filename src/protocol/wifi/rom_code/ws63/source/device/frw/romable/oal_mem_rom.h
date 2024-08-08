/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: netbuf memory mamage interface
 */
#ifndef OAL_MEM_ROM_H
#define OAL_MEM_ROM_H
#include "oal_net_rom.h"
#include "frw_util_rom.h"
#include "oal_netbuf_common_rom.h"
#include "oal_pkt_netbuf_rom.h"
#include "oal_net_pkt_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* ****************************************************************************
  枚举名  : oal_naked_system_mem_pool_id_enum_uint8
  协议表格:
  枚举说明: 裸系统DEVICE侧内存池ID
**************************************************************************** */
/* 与host定义冲突，增加dmac前缀 */
typedef enum {
    OAL_DMAC_MEM_POOL_ID_LOCAL = 0, /* 本地变量内存池 */
    OAL_DMAC_MEM_POOL_ID_MIB,       /* MIB内存池 */
    OAL_DMAC_MEM_POOL_ID_RX_DSCR,   /* 共享接收描述符内存池 */
    OAL_DMAC_MEM_POOL_ID_TX_DSCR,   /* 共享发送描述符1内存池 */
    OAL_DMAC_MEM_POOL_ID_NETBUF,    /* netbuf内存池 */
    OAL_DMAC_MEM_POOL_ID_BUTT
} oal_dmac_mem_pool_id_enum;
typedef osal_u8 oal_dmac_mem_pool_id_enum_uint8;
/* 与host定义冲突，增加dmac前缀 */

/* ****************************************************************************
  枚举名  : oal_mem_netbuf_pool_id_enum_uint8
  协议表格:
  枚举说明: netbuf内存池ID
**************************************************************************** */
typedef enum {
    OAL_MEM_NETBUF_POOL_ID_SHORT_PKT = 0, /* 短帧内存池 */
    OAL_MEM_NETBUF_POOL_ID_SMGMT_PKT,     /* 短管理帧 */
    OAL_MEM_NETBUF_POOL_ID_MGMT_PKT,      /* 管理帧 */
    OAL_MEM_NETBUF_POOL_ID_LARGE_PKT,     /* 长帧内存池 */

    OAL_MEM_NETBUF_POOL_ID_BUTT
} oal_mem_netbuf_pool_id_enum;
typedef osal_u8 oal_mem_netbuf_pool_id_enum_uint8;

/* ****************************************************************************
  结构说明: PKT内存配置结构体
**************************************************************************** */
typedef struct {
    osal_u32 start_addr; /* PKT内存块的首地址 */
    osal_u32 length;      /* 内存块长度 */
} oal_pkt_cfg_stru;

/* ****************************************************************************
  枚举说明: PKT内存段使用模块列表
**************************************************************************** */
typedef enum {
    OAL_MEM_ID_TXBFEB_PKT,  /* txbfer发送ndpa帧内存地址 */
    OAL_MEM_ID_TXBFEE_BUFF, /* txbfee逻辑回复矩阵内存地址 */
    OAL_MEM_ID_PROTECT,     /* one packet的保护帧内存地址 */
    OAL_MEM_ID_COEX,        /* coex保护帧内存地址 */
    OAL_MEM_ID_LOCAL,   /* 本地变量内存池内存地址 */
    OAL_MEM_ADDR_BEACON,
    OAL_MEM_ID_NETBUF,
    OAL_MEM_ID_BUTT
} oal_mem_addr_enum;
typedef osal_u8 oal_mem_addr_enum_uint8;

/* ****************************************************************************
  10 函数声明
**************************************************************************** */
osal_u32 oal_memory_init(osal_u32 start_addr, osal_u32 end_addr, const oal_mem_subpool_cfg_stru *cfg_table,
    osal_u32 table_size);
osal_void oal_memory_exit(osal_void);

#ifdef _PRE_WLAN_DFR_STAT
osal_u32 oal_mem_get_pool_cnt_by_type(osal_u8 type);
#endif
osal_u8 oal_dmac_mem_netbuf_valid_check(const oal_dmac_netbuf_stru *netbuf);
osal_void oal_set_netbuf_dfx_switch(osal_bool loca_dfx_is_open);
osal_u8 *oal_mem_get_pkt_payload(oal_mem_addr_enum_uint8 idx);
osal_u8 *oal_netbuf_dscr(const oal_dmac_netbuf_stru *netbuf);
osal_u32 oal_get_netbuf_pool_free_cnt(oal_mem_netbuf_pool_id_enum_uint8 subpool_id);
osal_u32 oal_get_netbuf_pool_used_cnt(oal_mem_netbuf_pool_id_enum_uint8 subpool_id);
osal_u32 oal_get_netbuf_pool_len(oal_mem_netbuf_pool_id_enum_uint8 subpool_id);
osal_void oal_mem_print_pool_info(osal_void);
osal_u32 oal_mem_rsv(osal_u32 rsv_size);
osal_void oal_mem_restore(osal_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_mm.h */
