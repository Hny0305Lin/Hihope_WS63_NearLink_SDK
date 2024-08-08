/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: extern interface
 */

#ifndef OAL_EXT_IF_ROM_H
#define OAL_EXT_IF_ROM_H

#include "oal_types_device_rom.h"
#include "frw_util_rom.h"
#include "oal_mem_rom.h"
#include "oal_net_rom.h"
#include "oal_fsm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WIFI_DMAC_TCM_TEXT __attribute__((section(".wifi.tcm.text")))
#define WIFI_DMAC_TCM_RODATA __attribute__((section(".wifi.tcm.rodata")))

typedef enum {
    OAL_TRACE_ENTER_FUNC,
    OAL_TRACE_EXIT_FUNC,
    OAL_TRACE_DIRECT_BUTT
} oal_trace_direction_enum;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_ext_if_rom.h */
