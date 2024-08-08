/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:  RISCV31 PATCH.
 *
 * Create:  2022-2-12
 */
#include "chip_io.h"
#include "arch_barrier.h"
#include "flash_patch.h"

#define RISCV31_PATCH_FLPCTRL      0xE0000000

#define RISCV31_PATCH_FLPCTRL_OFFSET  0x0
#define RISCV31_PATCH_FLPRMP      (RISCV31_PATCH_FLPCTRL + 0x4)

#define RISCV31_PATCH_FLPLACMP0   (RISCV31_PATCH_FLPCTRL + 0x8)
#define RISCV31_PATCH_FLPLACMP1   (RISCV31_PATCH_FLPCTRL + 0xC)

#define RISCV31_PATCH_FLPIACMP0   (RISCV31_PATCH_FLPCTRL + 0x10)

#define RISCV31_PATCH_FLPCTRL_ENABLE  0x1

#define RISCV31_PATCH_FLPCTRL_PCO_WITHIN_1M  0x0
#define RISCV31_PATCH_FLPCTRL_PCO_LARGER_1M  0x1

#define RISCV31_PATCH_FLPCTRL_WPT_WRITABLE    0x0
#define RISCV31_PATCH_FLPCTRL_WPT_UNWRITABLE  0x1

#define RISCV31_PATCH_FLPCTRL_FLPLACMP0_DISABLE  0x0
#define RISCV31_PATCH_FLPCTRL_FLPLACMP1_DISABLE  0x0

#define RISCV31_PATCH_FP_CMP_CTRL_INDEX  0
#define RISCV31_PATCH_FP_CMP_REMAP_INDEX 1
#define RISCV31_PATCH_FP_CMP_COUNT_INDEX 2
#define RISCV31_PATCH_FP_CMP_MATCH_INDEX 3

typedef enum {
    GEN_BIT0 = 0,
    WPT_BIT1 = 1,
    PCO_BIT2 = 2,
    LACEN0_BIT3 = 3,
    LACEN1_BIT4 = 4,
} sema_enum;

void riscv_patch_init(riscv_cfg_t patch_cfg)
{
    reg_setbit(RISCV31_PATCH_FLPCTRL, RISCV31_PATCH_FLPCTRL_OFFSET, GEN_BIT0);
    reg_clrbit(RISCV31_PATCH_FLPCTRL, RISCV31_PATCH_FLPCTRL_OFFSET, WPT_BIT1);

    if (patch_cfg.off_region == true) {
        reg_setbit(RISCV31_PATCH_FLPCTRL, RISCV31_PATCH_FLPCTRL_OFFSET, PCO_BIT2);
    } else {
        reg_clrbit(RISCV31_PATCH_FLPCTRL, RISCV31_PATCH_FLPCTRL_OFFSET, PCO_BIT2);
    }

    for (uint32_t loop = 0; loop < RISCV31_PATCH_CMP_REG_NUM; loop++) {
        *((uint32_t *)RISCV31_PATCH_FLPIACMP0 + loop) =
            *((uint32_t *)(uintptr_t)((void *)patch_cfg.cmp_start_addr) +
            RISCV31_PATCH_FP_CMP_MATCH_INDEX + loop);
    }

    writel(RISCV31_PATCH_FLPRMP, (uint32_t)(uintptr_t)((void *)patch_cfg.remap_addr));
    reg_setbit(RISCV31_PATCH_FLPCTRL, RISCV31_PATCH_FLPCTRL_OFFSET, WPT_BIT1);
    dsb();
}