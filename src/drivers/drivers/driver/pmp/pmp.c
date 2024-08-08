/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides MPU driver source \n
 *
 * History: \n
 * 2022-09-26， Create file. \n
 */

#include <stdint.h>
#include "common_def.h"
#include "hal_pmp.h"
#include "pmp_porting.h"
#include "drv_pmp.h"

#define PMP_NAPOT_ADDR_OFFSET  2

static uint32_t get_pmp_napot_size(uint32_t value)
{
    return ((value >> 1) - 1);
}

static uint32_t set_pmp_napot_addr(uint32_t value)
{
    return (value >> PMP_NAPOT_ADDR_OFFSET);
}

static hal_pmp_conf_t build_hal_params(pmp_conf_t config)
{
    hal_pmp_conf_t hal_config;
    hal_config.idx = config.idx;
    /* 处理地址 */
    if (config.conf.addr_match != PMPCFG_ADDR_MATCH_TOR) {
        hal_config.addr = (uint32_t)set_pmp_napot_addr(config.addr + get_pmp_napot_size(config.size));
    } else {
        hal_config.addr = config.addr >> PMPADDR_RIGHT_SHIFT_BIS;
    }
    hal_config.attr = config.conf.pmp_attr;
    hal_config.cfg.a = config.conf.addr_match;
    hal_config.cfg.l = config.conf.lock;
    hal_config.cfg.rwx = config.conf.rwx_permission;
    return hal_config;
}

errcode_t uapi_pmp_config(const pmp_conf_t *config, uint32_t length)
{
    hal_pmp_funcs_t *pmp_funcs;
    pmp_port_register_hal_funcs();
    pmp_funcs = hal_pmp_get_funcs();
    hal_pmp_conf_t hal_config;
    errcode_t ret = ERRCODE_SUCC;
    for (uint32_t i = 0; i < length ; i++) {
        hal_config = build_hal_params(config[i]);
        ret = pmp_funcs->config(&hal_config);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }
    dsb();
    return ERRCODE_SUCC;
}
