/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides V154 adc register operation api \n
 *
 * History: \n
 * 2024-02-01， Create file. \n
 */

#ifndef HAL_ADC_V154_REGS_OP_H
#define HAL_ADC_V154_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "hal_adc_v154_regs_def.h"
#include "adc_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

extern adc_regs_t *g_adc_regs;
extern cldo_crg_cfg_t *g_cldo_cfg;

static inline bool hal_adc_reg_irg_fifo_stat_get(void)
{
    adc_irg_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_2;
    return (bool)data.b.rxris;
}

static inline void hal_adc_reg_irg_fifo_interrupt_mask(void)
{
    adc_irg_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_2;
    data.b.rxim = 1;
    g_adc_regs->lsadc_ctrl_2 = data.d32;
}

static inline bool hal_adc_reg_fifo_isnt_empty_get(void)
{
    adc_fifo_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_1;
    return (bool)data.b.rne;
}

static inline uint32_t hal_adc_reg_fifo_data_get(void)
{
    adc_fifo_read_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_9;
    return data.b.data;
}

static inline void hal_adc_soft_rst(uint32_t value)
{
    cldo_rst_soft_ctl_t data;
    data.d32 = g_cldo_cfg->cldo_crg_rst_soft_cfg_1;
    data.b.soft_rst_lsadc_n = value;
    data.b.soft_rst_lsadc_bus_n = value;
    g_cldo_cfg->cldo_crg_rst_soft_cfg_1 = data.d32;
}

static inline void hal_adc_simu_cfg1(void)
{
    uint32_t data;
    data = 0x155240;
    g_adc_regs->da_lsadc_rwreg_1 = data;
}

static inline void hal_adc_simu_cfg2(void)
{
    uint32_t data;
    data = 0x5;
    g_adc_regs->da_lsadc_rwreg_2 = data;
}

static inline void hal_adc_simu_cfg3(void)
{
    uint32_t data;
    data = 0x100;
    g_adc_regs->da_lsadc_rwreg_3 = data;
}

inline uint32_t test_hal_adc_simu_cfg3(void)
{
    uint32_t data;
    data = (uintptr_t)&(g_adc_regs->da_lsadc_rwreg_3);
    return data;
}

static inline void hal_adc_simu_ldo_start(void)
{
    adc_enable_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_11;
    data.b.da_lsadc_en |= 0x7000;
    g_adc_regs->lsadc_ctrl_11 = data.d32;
}

static inline void hal_adc_simu_cfg4(void)
{
    adc_enable_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_11;
    data.b.da_lsadc_en |= 0xE7F;
    g_adc_regs->lsadc_ctrl_11 = data.d32;
}

static inline void hal_adc_simu_cfg5(void)
{
    adc_enable_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_11;
    data.b.da_lsadc_en |= 0x100;
    g_adc_regs->lsadc_ctrl_11 = data.d32;
}

static inline void hal_adc_simu_cfg6(void)
{
    adc_enable_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_11;
    data.b.da_lsadc_en |= 0x80;
    g_adc_regs->lsadc_ctrl_11 = data.d32;
}

static inline void hal_adc_simu_rst_clr(void)
{
    adc_enable_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_11;
    data.b.da_lsadc_rstn = 1;
    g_adc_regs->lsadc_ctrl_11 = data.d32;
}

static inline void hal_adc_offset_cali_state_clr(void)
{
    adc_offset_cali_state_clr_data_t data;
    data.d32 = g_adc_regs->cfg_adc_offset_cali_state_clr;
    data.b.offset_cali_finish_clr = 1;
    g_adc_regs->cfg_adc_offset_cali_state_clr = data.d32;
    data.d32 = g_adc_regs->cfg_adc_offset_cali_state_clr;
    data.b.offset_cali_finish_clr = 0;
    g_adc_regs->cfg_adc_offset_cali_state_clr = data.d32;
}

static inline void hal_adc_offset_cali_data_spi_refresh(void)
{
    adc_offset_cali_spi_refresh_data_t data;
    data.d32 = g_adc_regs->cfg_adc_offset_cali_data_spi_refresh;
    data.b.offset_cali_spi_refresh = 1;
    g_adc_regs->cfg_adc_offset_cali_data_spi_refresh = data.d32;
    data.d32 = g_adc_regs->cfg_adc_offset_cali_data_spi_refresh;
    data.b.offset_cali_spi_refresh = 0;
    g_adc_regs->cfg_adc_offset_cali_data_spi_refresh = data.d32;
}

static inline void hal_adc_offset_cali_set(void)
{
    adc_offset_cali_data_t data;
    data.d32 = g_adc_regs->cfg_adc_offset_cali;
    data.b.offset_cali_mode = 1;
    data.b.offset_cali_loop_sel = 0;
    data.b.offset_cali_inverse = 0;
    data.b.offset_cali_acc_cycle_sel = 1;
    data.b.offset_cali_range_sel = 0;
    g_adc_regs->cfg_adc_offset_cali = data.d32;
}

static inline void hal_adc_offset_cali_enable(uint32_t value)
{
    adc_offset_cali_en_data_t data;
    data.d32 = g_adc_regs->cfg_adc_offset_cali_en;
    data.b.offset_cali_en = value;
    g_adc_regs->cfg_adc_offset_cali_en = data.d32;
}

static inline uint32_t hal_adc_offset_cali_sts(void)
{
    adc_offset_cali_sts_data_t data;
    data.d32 = g_adc_regs->rpt_adc_offset_cali_finish_sts;
    return data.b.offset_cali_finish;
}

static inline uint32_t hal_adc_auto_offset_cali_data_get(void)
{
    adc_rpt_offset_cali_data_t data;
    data.d32 = g_adc_regs->rpt_adc_offset_cali_data;
    return data.b.offset_cali_data;
}

static inline void hal_adc_manual_offset_cali_set(uint32_t value)
{
    adc_offset_cali_data_t data;
    data.d32 = g_adc_regs->cfg_adc_offset_cali;
    data.b.offset_cali_mode = 0;
    data.b.offset_cali_data_spi = value;
    g_adc_regs->cfg_adc_offset_cali = data.d32;
}

static inline void hal_adc_cfg_offset_cali_data_enable(void)
{
    adc_cfg_offset_cali_en_data_t data;
    data.d32 = g_adc_regs->cfg_adc_offset_cali_data_en;
    data.b.offset_data_en = 1;
    g_adc_regs->cfg_adc_offset_cali_data_en = data.d32;
}

static inline void hal_adc_cfg_offset_cali_data_disable(void)
{
    adc_cfg_offset_cali_en_data_t data;
    data.d32 = g_adc_regs->cfg_adc_offset_cali_data_en;
    data.b.offset_data_en = 0;
    g_adc_regs->cfg_adc_offset_cali_data_en = data.d32;
}

static inline void hal_adc_cfg_cap_cali_finish_clr(void)
{
    cfg_cap_cali_finish_clr_data_t data;
    data.d32 = g_adc_regs->cfg_cap_cali_finish_clr;
    data.b.finish_clr = 1;
    g_adc_regs->cfg_cap_cali_finish_clr = data.d32;
    data.d32 = g_adc_regs->cfg_cap_cali_finish_clr;
    data.b.finish_clr = 0;
    g_adc_regs->cfg_cap_cali_finish_clr = data.d32;
}

static inline void hal_adc_cfg_intr_gain_state_clr(void)
{
    cfg_intr_gain_state_clr_data_t data;
    data.d32 = g_adc_regs->cfg_intr_gain_state_clr;
    data.b.gain_state_clr = 1;
    g_adc_regs->cfg_intr_gain_state_clr = data.d32;
    data.d32 = g_adc_regs->cfg_intr_gain_state_clr;
    data.b.gain_state_clr = 0;
    g_adc_regs->cfg_intr_gain_state_clr = data.d32;
}

static inline void hal_adc_date_spi_refresh_clr(void)
{
    cfg_cap_cali_data_spi_refresh_data_t data;
    data.d32 = g_adc_regs->cfg_cap_cali_data_spi_refresh;
    data.b.spi_refresh = 1;
    g_adc_regs->cfg_cap_cali_data_spi_refresh = data.d32;
    data.d32 = g_adc_regs->cfg_cap_cali_data_spi_refresh;
    data.b.spi_refresh = 0;
    g_adc_regs->cfg_cap_cali_data_spi_refresh = data.d32;
}

static inline void hal_adc_cfg_cap_cali_set(void)
{
    cfg_adc_cap_cali_data_t data;
    data.d32 = g_adc_regs->cfg_adc_cap_cali;
    data.b.mode = 1;
    data.b.alg_sel = 0;
    data.b.acc_cycle_sel = 0;
    data.b.start_index = 0x3;
    data.b.weight_sel = 1;
    g_adc_regs->cfg_adc_cap_cali = data.d32;
}

static inline void hal_adc_cfg_cap_cali_enable(uint32_t value)
{
    cfg_adc_cap_cali_en_data_t data;
    data.d32 = g_adc_regs->cfg_adc_cap_cali_en;
    data.b.en = value;
    g_adc_regs->cfg_adc_cap_cali_en = data.d32;
}

static inline uint32_t hal_adc_rpt_cap_cali_sts(void)
{
    rpt_cap_cali_sts0_data_t data;
    data.d32 = g_adc_regs->rpt_cap_cali_sts_0;
    return data.b.finish;
}

static inline void hal_adc_cfg_gain_cali_enable(void)
{
    cfg_gain_cali_en_data_t data;
    data.d32 = g_adc_regs->cfg_gain_cali_en;
    data.b.en = 1;
    g_adc_regs->cfg_gain_cali_en = data.d32;
}

static inline uint32_t hal_adc_gain_unit_get(void)
{
    rpt_intr_gain_cali_gain_data_t data;
    data.d32 = g_adc_regs->rpt_intr_gain_cali_gain;
    return data.b.intr_gain_uint;
}

static inline void hal_adc_fifo_waterline_set(void)
{
    adc_fifo_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_1;
    data.b.rxintsize = 0x7;
    g_adc_regs->lsadc_ctrl_1 = data.d32;
}

static inline void hal_adc_start_sample(void)
{
    adc_scan_start_and_stop_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_8;
    data.b.lsadc_start = 1;
    g_adc_regs->lsadc_ctrl_8 = data.d32;
}

inline uint32_t test_hal_adc_start(void)
{
    uint32_t data;
    data = (uintptr_t)&(g_adc_regs->lsadc_ctrl_8);
    return data;
}

static inline void hal_adc_stop_sample(void)
{
    adc_scan_start_and_stop_data_t data;
    data.d32 = g_adc_regs->lsadc_ctrl_8;
    data.b.lsadc_stop = 1;
    g_adc_regs->lsadc_ctrl_8 = data.d32;
}

void hal_adc_auto_scan_mode_set(adc_channel_t ch, bool en);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif