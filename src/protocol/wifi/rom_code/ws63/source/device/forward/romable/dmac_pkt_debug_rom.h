/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Device pkt diagnose interface.
 */

#ifndef DMAC_PKT_DEBUG_ROM_H
#define DMAC_PKT_DEBUG_ROM_H

#include "osal_types.h"
#include "frw_rom_cb_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    DBG_DIR_RX = 0,
    DBG_DIR_TX,
    DBG_DIR_NUM
} dmac_pkt_dbg_dir_enum;

typedef enum {
    DBG_RX_START = 0,
    DBG_RX_DMAC_IN = DBG_RX_START,
    DBG_RX_DMAC2HCC,
    DBG_RX_END = DBG_RX_DMAC2HCC,
    DBG_RX_POS_NUM
} dmac_pkt_dbg_rx_pos_enum;

typedef enum {
    DBG_TX_START = 0,
    DBG_TX_DMAC_IN = DBG_TX_START,
    DBG_TX_DIR_TX,
    DBG_TX_NO_QUE,
    DBG_TX_ENQUE,
    DBG_TX_DEQUE,
    DBG_TX_COMP_IN,
    DBG_TX_COMP_FREE,
    DBG_TX_ERR_FREE,
    DBG_TX_RETRY_DROP,
    DBG_TX_CACHE_DROP,
    DBG_TX_MGMT_RETRY,
    DBG_TX_DMAC2HAL,
    DBG_TX_END = DBG_TX_DMAC2HAL,
    DBG_TX_POS_NUM
} dmac_pkt_dbg_tx_pos_enum;

typedef enum {
    DBG_DMAC = 0,
    DBG_HCC,
    DBG_DEVICE_ALL,
    DBG_MODULE_NUM
} dmac_pkt_dbg_module_enum;

typedef osal_void (*dmac_pkt_dbg_cb)(osal_u8 module_id, osal_u8 dir, osal_u8 pos_num, osal_char *pos, osal_void *skb);
osal_void pkt_dbg(osal_u8 module_id, osal_u8 dir, osal_u8 pos_num, osal_char *pos, osal_void *skb);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of dmac_pkt_debug_rom.h */