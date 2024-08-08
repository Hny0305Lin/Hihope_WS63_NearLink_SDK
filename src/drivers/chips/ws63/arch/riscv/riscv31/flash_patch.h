/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:  RISCV31 PATCH.
 *
 * Create:  2022-2-12
 */
#ifndef RISCV31_PATCH_H
#define RISCV31_PATCH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define RISCV31_PATCH_CMP_REG_NUM  192

typedef struct {
    uint32_t cmp_start_addr;
    uint32_t remap_addr;
    bool off_region;   /*!< The jump range, false for within 1MB, true for out of the 1M range. */
    uint8_t flplacmp0_en;
    uint8_t flplacmp1_en;
} riscv_cfg_t;

/**
 * @brief  Initialises the patch.
 * @param  patch_cfg RISC-V patch configuration.
 */
void riscv_patch_init(riscv_cfg_t patch_cfg);

#ifdef __cplusplus
#if __cplusplus
    }
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
