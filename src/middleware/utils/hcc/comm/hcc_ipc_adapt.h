/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: the header file for hcc ipc adapt layer.
 * Author:
 * Create: 2023-02-21
 */
#ifndef HCC_IPC_ADAPT_H
#define HCC_IPC_ADAPT_H

#include "td_type.h"
#include "hcc_bus.h"
#include "hcc_queue.h"
#include "hcc_cfg_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_HCC_SUPPORT_IPC

#define IPC_MEM_MSG_MAX    4
#define HCC_PREMEM_LEN     4
#define HCC_LOSS_MEM_MAX   8
#define IPC_MAX_XFER_LEN   2048

#define IPC_CHAN_DATA    0
#define IPC_CHAN_MSG     1

#define IPC_MEM_PRIV_MSG_FLAG 0x5A
#define IPC_MEM_PRIV_MSG_FLAG_OFFSET 8
#define IPC_MEM_PRIV_MSG     (IPC_MEM_PRIV_MSG_FLAG << IPC_MEM_PRIV_MSG_FLAG_OFFSET)

enum {
    HCC_IPC_MEM_REQUEST = 1,
    HCC_IPC_MEM_REPLY,
    HCC_IPC_MEM_TX_COMPLETE,
    HCC_IPC_MEM_RX_START,
    HCC_IPC_MEM_RX_COMPLETE,
    HCC_IPC_MEM_PRIV_MSG = IPC_MEM_PRIV_MSG,
    HCC_IPC_MEM_CLEAR_PREMEM,
};

typedef struct hcc_ipc_mem_request {
    td_u8 type;
    td_u8 hcc_queue_id;
    td_u8 cnt;
    td_u8 rsv;
    td_u16 data_len[IPC_MEM_MSG_MAX];
} hcc_ipc_mem_request_t;

typedef struct hcc_ipc_mem_reply {
    td_u8 type;
    td_u8 hcc_queue_id;
    td_u8 cnt;
    td_u8 rsv;
    td_u16 data_len[IPC_MEM_MSG_MAX];
    td_u8 *addr[IPC_MEM_MSG_MAX];
} hcc_ipc_mem_reply_t;

typedef struct hcc_ipc_mem_complete {
    td_u8 type;
    td_u8 hcc_queue_id;
    td_u8 cnt;
    td_u8 rsv;
    td_u8 *addr[IPC_MEM_MSG_MAX];
} hcc_ipc_mem_complete_t;

typedef struct hcc_ipc_cfg {
    td_u8 premem_len;
    td_u8 lossmem_max;
} hcc_ipc_cfg_t;

td_void hcc_ipc_complement_mem(td_void);
hcc_bus *hcc_adapt_ipc_load(hcc_handler *hcc);
td_void hcc_adapt_ipc_unload(td_void);
td_bool hcc_ipc_tx_dma_is_busy(hcc_queue_dir dir, hcc_trans_queue *queue);
td_void hcc_ipc_flowctrl_on(hcc_bus *hcc_bus, td_bool unc_check);
td_void hcc_ipc_flowctrl_off(hcc_bus *hcc_bus, td_bool force_check);
td_s32 hcc_adapt_ipc_cfg(hcc_ipc_cfg_t *cfg);
bus_dev_ops* hcc_ipc_get_bus_ops(td_void);
#endif   /* CONFIG_HCC_SUPPORT_IPC */
td_void hcc_ipc_clear_premem(td_void);

static inline td_bool hcc_check_pre_req_queue(TD_CONST hcc_trans_queue *queue)
{
    return queue->queue_ctrl->transfer_mode == HCC_IPC_TRANS_PREMEM;
}
struct osal_list_head *hcc_ipc_get_pre_mem_list(td_u8 queue_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HCC_IPC_ADAPT_H */
