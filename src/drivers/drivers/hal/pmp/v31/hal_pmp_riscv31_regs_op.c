/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides systick register operation api \n
 *
 * History: \n
 * 2022-09-28, Create file. \n
 */
#include <stdint.h>
#include "hal_pmp_riscv31_regs_op.h"

void hal_pmp_riscv31_regs_set_pmpaddr(uint32_t idx, uint32_t pmpaddr)
{
    /* Because like that pmpaddr0 is RISCV CSR, so cannot be performed using table lookup mode. */
    if (idx == PMPADDR0) {
        write_csr(pmpaddr0, pmpaddr);
    } else if (idx == PMPADDR1) {
        write_csr(pmpaddr1, pmpaddr);
    } else if (idx == PMPADDR2) {
        write_csr(pmpaddr2, pmpaddr);
    } else if (idx == PMPADDR3) {
        write_csr(pmpaddr3, pmpaddr);
    } else if (idx == PMPADDR4) {
        write_csr(pmpaddr4, pmpaddr);
    } else if (idx == PMPADDR5) {
        write_csr(pmpaddr5, pmpaddr);
    } else if (idx == PMPADDR6) {
        write_csr(pmpaddr6, pmpaddr);
    } else if (idx == PMPADDR7) {
        write_csr(pmpaddr7, pmpaddr);
    } else if (idx == PMPADDR8) {
        write_csr(pmpaddr8, pmpaddr);
    } else if (idx == PMPADDR9) {
        write_csr(pmpaddr9, pmpaddr);
    } else if (idx == PMPADDR10) {
        write_csr(pmpaddr10, pmpaddr);
    } else if (idx == PMPADDR11) {
        write_csr(pmpaddr11, pmpaddr);
    } else if (idx == PMPADDR12) {
        write_csr(pmpaddr12, pmpaddr);
    } else if (idx == PMPADDR13) {
        write_csr(pmpaddr13, pmpaddr);
    } else if (idx == PMPADDR14) {
        write_csr(pmpaddr14, pmpaddr);
    } else if (idx == PMPADDR15) {
        write_csr(pmpaddr15, pmpaddr);
    } else {
        return;
    }
}

void hal_pmp_riscv31_regs_set_memxattr(uint32_t idx, uint8_t attr)
{
    uint32_t memattr;
    uint8_t idx_temp;
    if (idx < MEMXATTR_NUM_PER_REG) {
        memattr = read_custom_csr(MEMATTRL);
        idx_temp = (uint8_t)(idx << MEMXATTR_VECTOR);
        memattr = (memattr & ~(0xFUL << idx_temp)) | (attr << idx_temp);
        write_custom_csr_val(MEMATTRL, memattr);
    } else {
        memattr = read_custom_csr(MEMATTRH);
        idx_temp = (uint8_t)((idx - MEMXATTR_NUM_PER_REG) << MEMXATTR_VECTOR);
        memattr = (memattr & ~(0xFUL << idx_temp)) | (attr << idx_temp);
        write_custom_csr_val(MEMATTRH, memattr);
    }
}

void hal_pmp_riscv31_regs_set_pmpxcfg(uint32_t idx, hal_pmpx_config_t cfg)
{
    hal_pmp_riscv31_cfg_t pmp_cfg;

    /* Because like that pmpaddr0 is RISCV CSR, so cannot be performed using table lookup mode. */
    switch (idx / PMPXCFG_NUM_PER_REG) {
        case PMPCFG0:
            pmp_cfg.d32 = read_csr(pmpcfg0);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg0, pmp_cfg.d32);
            break;
        case PMPCFG1:
            pmp_cfg.d32 = read_csr(pmpcfg1);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg1, pmp_cfg.d32);
            break;
        case PMPCFG2:
            pmp_cfg.d32 = read_csr(pmpcfg2);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg2, pmp_cfg.d32);
            break;
        case PMPCFG3:
            pmp_cfg.d32 = read_csr(pmpcfg3);
            pmp_cfg.b[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg3, pmp_cfg.d32);
            break;
        default:
            break;
    }
}