/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc channel.
 * Author:
 * Create: 2021-09-23
 */

#ifndef HCC_CHANNEL_HEADER
#define HCC_CHANNEL_HEADER

#include "td_type.h"
#include "hcc.h"
#include "hcc_cfg_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
td_s32 hcc_add_handler(hcc_handler *hcc);
td_void hcc_delete_handler(td_u8 chl);
hcc_handler *hcc_get_bus_handler(td_u8 bus_type);

td_bool hcc_chan_is_busy(td_u8 chl);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HCC_CHANNEL_HEADER */