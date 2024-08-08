/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides riscv hal mpu \n
 *
 * History: \n
 * 2022-09-26, Create file. \n
 */
#include <stdint.h>
#include "hal_pmp_riscv31_regs_op.h"
#include "hal_pmp_riscv31.h"

static errcode_t hal_pmp_riscv31_config(hal_pmp_conf_t *config)
{
    uint32_t idx = config->idx;
    uint32_t addr = config->addr;
    uint8_t attr = config->attr;
    if (idx >= PMP_REGION_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    hal_pmpx_config_t pmpx_config;
    hal_pmp_riscv31_regs_set_pmpaddr(idx, addr);
    hal_pmp_riscv31_regs_set_memxattr(idx, attr);
    pmpx_config.rwx = config->cfg.rwx;
    pmpx_config.resv_0 = 0;
    pmpx_config.a = 0;
    pmpx_config.l = 0;
    hal_pmp_riscv31_regs_set_pmpxcfg(idx, pmpx_config);

    /* All other PMP registers are configured before the PMP region is enabled (pmp[n]cfg.A). */
    pmpx_config.a = config->cfg.a;
    pmpx_config.l = config->cfg.l;
    hal_pmp_riscv31_regs_set_pmpxcfg(idx, pmpx_config);
    dsb();
    return ERRCODE_SUCC;
}

static hal_pmp_funcs_t g_hal_pmp_riscv31_funcs = {
    .config = hal_pmp_riscv31_config
};

hal_pmp_funcs_t *hal_pmp_riscv31_funcs_get(void)
{
    return &g_hal_pmp_riscv31_funcs;
}