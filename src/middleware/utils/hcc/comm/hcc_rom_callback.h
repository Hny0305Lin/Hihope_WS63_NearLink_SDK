/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hcc rom patch callback function.
 * Author:
 * Create: 2023-05-18
 */

#ifndef HCC_ROM_CALLBACK_H
#define HCC_ROM_CALLBACK_H
#include "hcc_types.h"
#include "hcc.h"
#include "hcc_queue.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef ext_errno (*hcc_tx_data_cb)(td_u8 chl, td_u8 *buf, td_u16 len, hcc_transfer_param *param);
typedef ext_errno (*hcc_tx_queue_cb)(hcc_handler *hcc, hcc_trans_queue *queue, hcc_transfer_param *param);
typedef td_void (*hcc_rx_enqueue_cb)(hcc_handler *hcc, hcc_queue_type queue_id, hcc_unc_struc *unc_buf);
typedef ext_errno (*hcc_rx_data_cb)(hcc_handler *hcc, hcc_trans_queue *queue, td_u16 *pkt_cnt);
typedef td_void (*hcc_rx_proc_cb)(hcc_header *hcc_head, td_u8 *buf, td_u8 *user_param,
    hcc_adapt_priv_rx_process rx_proc);
typedef td_void (*hcc_thread_watchdog)(td_void);

typedef enum {
    HCC_CB_TX = 0,
    HCC_CB_TX_QUEUE,
    HCC_CB_BT_TX,
    HCC_CB_RX_ENQUE,
    HCC_CB_RX_DATA,
    HCC_CB_RX_PROC,
    HCC_CB_WATCHDOG,
    HCC_CB_BUTT,
} hcc_cb_offset;

typedef struct _hcc_rom_callback {
    hcc_tx_data_cb tx_data;
    hcc_tx_queue_cb tx_queue;
    hcc_tx_data_cb bt_tx_data;
    hcc_rx_enqueue_cb rx_enque;
    hcc_rx_data_cb rx_data;
    hcc_rx_proc_cb rx_proc;
    hcc_thread_watchdog watchdog;
} hcc_rom_callback;

td_void hcc_rom_cb_register(hcc_rom_callback *cb);
td_void *hcc_get_rom_cb(hcc_cb_offset offset);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

