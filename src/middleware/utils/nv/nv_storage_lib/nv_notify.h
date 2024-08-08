/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: nv notify header
 */

#ifndef NV_NOTIFY_H
#define NV_NOTIFY_H
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM

#include "stdbool.h"
#include "nv.h"
#include "nv_porting.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    nv_changed_notify_func func;
    uint16_t min_key;
    uint16_t max_key;
} nv_changed_proc_t;

typedef struct {
    uint32_t nv_sem_handle;
    nv_changed_proc_t *nv_change_notify_list;  /* Nv changed notify functions table */
    uint8_t notify_regitser_max_nums;          /* Max number of nv changed notify functions */
    uint8_t notify_registered_nums;            /* The number of nv changed notify functions */
    uint16_t rsv2;
} nv_direct_ctrl_t;

bool nv_change_notify_segment_is_valid(nv_direct_ctrl_t *nv_ctrl, uint16_t min_key, uint16_t max_key);
errcode_t nv_direct_notify_list_init(void);
void nv_change_notify(uint16_t key);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
#endif /* NV_NOTIFY_H */
