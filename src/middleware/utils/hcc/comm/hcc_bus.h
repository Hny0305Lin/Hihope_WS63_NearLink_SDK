/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc adapt bus completion.
 * Author:
 * Create: 2021-05-13
 */

#ifndef HCC_BUS_INTERFACE_HEADER
#define HCC_BUS_INTERFACE_HEADER

#include "td_type.h"
#include "hcc.h"
#include "hcc_cfg_comm.h"
#include "hcc_queue.h"
#include "hcc_bus_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef hcc_bus *(* hcc_bus_load_func)(hcc_handler *hcc);
typedef td_void (* hcc_bus_unload_func)(td_void);
typedef struct _bus_load_ {
    hcc_bus_load_func load;
    hcc_bus_unload_func unload;
} bus_load_unload;

hcc_bus *hcc_get_channel_bus(td_u8 chl);
bus_dev_ops *hcc_get_bus_ops(hcc_bus *bus);
td_void hcc_set_bus_ops(hcc_bus *bus, bus_dev_ops *dev_ops);
hcc_bus *hcc_alloc_bus(td_void);
td_void hcc_free_bus(hcc_bus *bus);
td_u32 hcc_bus_load(hcc_bus_type bus_type, hcc_handler *hcc);
td_void hcc_bus_unload(TD_CONST hcc_bus *bus);
td_u32 hcc_bus_tx_proc(hcc_bus *bus, hcc_trans_queue *queue, td_u16 *remain_pkt_nums);
td_bool hcc_bus_is_busy(hcc_bus *bus, hcc_queue_dir dir);
td_u32 hcc_bus_call_rx_message(hcc_bus *bus, hcc_rx_msg_type msg);
td_void hcc_bus_update_credit(hcc_bus *bus, td_u32 credit);
td_s32 hcc_bus_get_credit(hcc_bus *bus, td_u32 *credit);
td_s32 hcc_bus_reinit(hcc_bus *bus);
td_s32 hcc_bus_patch_read(hcc_bus *bus, td_u8 *buff, td_s32 len, td_u32 timeout);
td_s32 hcc_bus_patch_write(hcc_bus *bus, td_u8 *buff, td_s32 len);
td_void hcc_force_update_queue_id(hcc_bus *bus, td_u8 value);
td_s32 hcc_bus_power_action(hcc_bus *bus, hcc_bus_power_action_type action);
td_s32 hcc_bus_sleep_request(hcc_bus *bus);
td_s32 hcc_bus_wakeup_request(hcc_bus *bus);
td_s32 hcc_set_bus_func(td_u8 bus_type, bus_load_unload *bus_load_opt);
td_u32 hcc_get_pkt_max_len(td_u8 chl);


#define BUS_LOG_SYMBOL_SIZE 4
#define BUS_LOG_SYMBOL_NUM 5
extern td_s8 g_loglevel[BUS_LOG_SYMBOL_NUM][BUS_LOG_SYMBOL_SIZE];

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HCC_BUS_INTERFACE_HEADER */
