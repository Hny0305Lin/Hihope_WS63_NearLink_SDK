/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides efuse register operation api \n
 *
 * History: \n
 * 2023-3-4， Create file. \n
 */
#ifndef HAL_EFUSE_V151_REGS_OP_H
#define HAL_EFUSE_V151_REGS_OP_H

#include <stdint.h>
#include "hal_efuse_v151_reg_def.h"

#define EFUSE_REGION_INDEX0 0
#define EFUSE_REGION_INDEX1 1
#define EFUSE_REGION_INDEX2 2

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_efuse_regs_op Efuse Regs Operation
 * @ingroup  drivers_hal_efuse
 * @{
 */

extern efuse_regs_t *g_efuse_regs[CONFIG_EFUSE_REGION_NUM];
extern efuse_boot_done_regs_t *g_efuse_boot_done_regs;
#if defined(CONFIG_EFUSE_SWITCH_EN)
extern efuse_switch_en_regs_t *g_efuse_switch_en_regs;
#endif

/**
 * @brief  Init the efuse which will set the base address of registers.
 * @param i efuse region
 * @return 0 indicates the base address of registers has been configured success, -1 indicates failure.
 */
int32_t hal_efuse_regs_init(int32_t i);

/**
 * @brief  Deinit the hal_efuse which will clear the base address of registers has been set
 *         by @ref hal_efuse_regs_init.
 */
void hal_efuse_regs_deinit(void);


static inline uint32_t hal_efuse_sts_get(uint8_t region)
{
    efuse_sts_t efuse_sts;
    efuse_sts.d32 = g_efuse_boot_done_regs->efuse_sts_data.d32;
    if (region == 0) { return efuse_sts.b.efuse_boot0_done; }
#if CONFIG_EFUSE_REGION_NUM > EFUSE_REGION_INDEX1
    if (region == EFUSE_REGION_INDEX1) { return efuse_sts.b.efuse_boot1_done; }
#endif
#if CONFIG_EFUSE_REGION_NUM > EFUSE_REGION_INDEX2
    if (region == EFUSE_REGION_INDEX2) { return efuse_sts.b.efuse_boot2_done; }
#endif
    return 0;
}

static inline void hal_efuse_ctl_set(uint8_t region, uint32_t val)
{
    efuse_ctl_t efuse_ctl;
    efuse_ctl.d32 = g_efuse_regs[region]->efuse_ctl_data.d32;
    efuse_ctl.b.efuse_wr_rd = val;
    g_efuse_regs[region]->efuse_ctl_data.d32 = efuse_ctl.d32;
}

static inline void hal_efuse_clock_period_set(uint8_t region, uint32_t val)
{
    clock_period_t clock_period;
    clock_period.d32 = g_efuse_regs[region]->efuse_clk_period_data.d32;
    clock_period.b.clock_period = val;
    g_efuse_regs[region]->efuse_clk_period_data.d32 = clock_period.d32;
}

static inline void hal_efuse_avdd_ctl_set(uint8_t region, uint32_t val)
{
    efuse_avdd_ctl_t efuse_avdd_ctl;
    efuse_avdd_ctl.d32 = g_efuse_regs[region]->efuse_avdd_ctl_data.d32;
    efuse_avdd_ctl.b.efuse_avdd_sw = val;
    g_efuse_regs[region]->efuse_avdd_ctl_data.d32 = efuse_avdd_ctl.d32;
}

#if defined(CONFIG_EFUSE_SWITCH_EN)
static inline void hal_efuse_switch_en_set(uint32_t val)
{
    efuse_switch_en_ctl_t efuse_switch_en;
    efuse_switch_en.d32 = g_efuse_switch_en_regs->efuse_switch_en.d32;
    efuse_switch_en.b.efuse_switch_enable = val;
    g_efuse_switch_en_regs->efuse_switch_en.d32 = efuse_switch_en.d32;
}
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
