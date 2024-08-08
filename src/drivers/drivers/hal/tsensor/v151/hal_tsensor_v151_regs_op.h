/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 TSENSOR register operation api \n
 *
 * History: \n
 * 2023-02-28， Create file. \n
 */

#ifndef HAL_TSENSOR_V151_REGS_OP_H
#define HAL_TSENSOR_V151_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "hal_tsensor_v151_regs_def.h"
#include "tsensor_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_tsensor_v151_regs_op Tsensor V151 Regs Operation
 * @ingroup  drivers_hal_tsensor
 * @{
 */
#define TSENSOR_ENABLE          1
#define TSENSOR_DISABLE         0
#define TSENSOR_CALIB_ENABLE    0
#define TSENSOR_CALIB_DISABLE   1

extern uintptr_t g_hal_tsensor_regs;

static inline tsensor_comm_regs_t *hal_tsensor_get_comm_regs(void)
{
    return ((tsensor_comm_regs_t *)(g_hal_tsensor_regs + TSENSOR_COMM_REGS_OFFSET));
}

static inline tsensor_ctrl_regs_t *hal_tsensor_get_ctrl_regs(void)
{
    return ((tsensor_ctrl_regs_t *)(g_hal_tsensor_regs + TSENSOR_CTRL_REGS_OFFSET));
}

static inline void hal_tsensor_v151_reg_set_start(void)
{
    tsensor_start_data_t tsensor_start;
    tsensor_start.d32 = hal_tsensor_get_ctrl_regs()->tsensor_start;
    tsensor_start.b.tsensor_start = TSENSOR_ENABLE;
    hal_tsensor_get_ctrl_regs()->tsensor_start = tsensor_start.d32;
}

static inline void hal_tsensor_v151_reg_set_tsensor_enable(void)
{
    tsensor_ctrl_data_t tsensor_ctrl;
    tsensor_ctrl.d32 = hal_tsensor_get_ctrl_regs()->tsensor_ctrl;
    tsensor_ctrl.b.tsensor_enable = TSENSOR_ENABLE;
    hal_tsensor_get_ctrl_regs()->tsensor_ctrl = tsensor_ctrl.d32;
}

static inline void hal_tsensor_v151_reg_set_tsensor_disable(void)
{
    tsensor_ctrl_data_t tsensor_ctrl;
    tsensor_ctrl.d32 = hal_tsensor_get_ctrl_regs()->tsensor_ctrl;
    tsensor_ctrl.b.tsensor_enable = TSENSOR_DISABLE;
    hal_tsensor_get_ctrl_regs()->tsensor_ctrl = tsensor_ctrl.d32;
}

static inline void hal_tsensor_v151_reg_set_tsensor_mode(uint32_t mode)
{
    tsensor_ctrl_data_t tsensor_ctrl;
    tsensor_ctrl.d32 = hal_tsensor_get_ctrl_regs()->tsensor_ctrl;
    tsensor_ctrl.b.tsensor_mode = mode;
    hal_tsensor_get_ctrl_regs()->tsensor_ctrl = tsensor_ctrl.d32;
}

static inline uint32_t hal_tsensor_v151_reg_is_tsensor_ready(void)
{
    tsensor_sts_data_t tsensor_sts;
    tsensor_sts.d32 = hal_tsensor_get_ctrl_regs()->tsensor_sts;
    return tsensor_sts.b.tsensor_rdy;
}

static inline void hal_tsensor_v151_reg_clear_sts(void)
{
    tsensor_sts_data_t tsensor_sts;
    tsensor_sts.d32 = hal_tsensor_get_ctrl_regs()->tsensor_sts;
    tsensor_sts.b.tsensor_clr = TSENSOR_ENABLE;
    hal_tsensor_get_ctrl_regs()->tsensor_sts = tsensor_sts.d32;
}

static inline uint16_t hal_tsensor_v151_reg_get_data(void)
{
    tsensor_sts_data_t tsensor_sts;
    tsensor_sts.d32 = hal_tsensor_get_ctrl_regs()->tsensor_sts;
    return tsensor_sts.b.tsensor_data;
}

static inline void hal_tsensor_v151_reg_set_calib_enable(void)
{
    tsensor_ctrl1_data_t tsensor_ctrl1;
    tsensor_ctrl1.d32 = hal_tsensor_get_ctrl_regs()->tsensor_ctrl1;
    tsensor_ctrl1.b.temp_calib = TSENSOR_CALIB_ENABLE;
    hal_tsensor_get_ctrl_regs()->tsensor_ctrl1 = tsensor_ctrl1.d32;
}

static inline void hal_tsensor_v151_reg_set_calib_disable(void)
{
    tsensor_ctrl1_data_t tsensor_ctrl1;
    tsensor_ctrl1.d32 = hal_tsensor_get_ctrl_regs()->tsensor_ctrl1;
    tsensor_ctrl1.b.temp_calib = TSENSOR_CALIB_DISABLE;
    hal_tsensor_get_ctrl_regs()->tsensor_ctrl1 = tsensor_ctrl1.d32;
}

static inline void hal_tsensor_v151_reg_set_ct_sel(uint8_t temp_ct_sel)
{
    tsensor_ctrl1_data_t tsensor_ctrl1;
    tsensor_ctrl1.d32 = hal_tsensor_get_ctrl_regs()->tsensor_ctrl1;
    tsensor_ctrl1.b.temp_ct_sel = temp_ct_sel;
    hal_tsensor_get_ctrl_regs()->tsensor_ctrl1 = tsensor_ctrl1.d32;
}

static inline void hal_tsensor_v151_reg_set_temp_set(uint8_t temp_set)
{
    tsensor_ctrl1_data_t tsensor_ctrl1;
    tsensor_ctrl1.d32 = hal_tsensor_get_ctrl_regs()->tsensor_ctrl1;
    tsensor_ctrl1.b.temp_set = temp_set;
    hal_tsensor_get_ctrl_regs()->tsensor_ctrl1 = tsensor_ctrl1.d32;
}

static inline void hal_tsensor_v151_reg_set_temp_scan_dft(uint8_t temp_scan_dft)
{
    tsensor_ctrl1_data_t tsensor_ctrl1;
    tsensor_ctrl1.d32 = hal_tsensor_get_ctrl_regs()->tsensor_ctrl1;
    tsensor_ctrl1.b.temp_scan_dft = temp_scan_dft;
    hal_tsensor_get_ctrl_regs()->tsensor_ctrl1 = tsensor_ctrl1.d32;
}

static inline void hal_tsensor_v151_reg_set_temp_high_limit(uint16_t temp_code)
{
    tsensor_temp_high_limit_data_t tsensor_temp_high_limit;
    tsensor_temp_high_limit.d32 = hal_tsensor_get_ctrl_regs()->tsensor_temp_high_limit;
    tsensor_temp_high_limit.b.tsensor_temp_high_limit = temp_code;
    hal_tsensor_get_ctrl_regs()->tsensor_temp_high_limit = tsensor_temp_high_limit.d32;
}

static inline void hal_tsensor_v151_reg_set_temp_low_limit(uint16_t temp_code)
{
    tsensor_temp_low_limit_data_t tsensor_temp_low_limit;
    tsensor_temp_low_limit.d32 = hal_tsensor_get_ctrl_regs()->tsensor_temp_low_limit;
    tsensor_temp_low_limit.b.tsensor_temp_low_limit = temp_code;
    hal_tsensor_get_ctrl_regs()->tsensor_temp_low_limit = tsensor_temp_low_limit.d32;
}

static inline void hal_tsensor_v151_reg_set_overtemp_threshold(uint16_t temp_code)
{
    tsensor_over_temp_data_t tsensor_over_temp;
    tsensor_over_temp.d32 = hal_tsensor_get_ctrl_regs()->tsensor_over_temp;
    tsensor_over_temp.b.tsensor_overtemp_thresh = temp_code;
    hal_tsensor_get_ctrl_regs()->tsensor_over_temp = tsensor_over_temp.d32;
}

static inline void hal_tsensor_v151_reg_set_overtemp_protect_enable(void)
{
    tsensor_over_temp_data_t tsensor_over_temp;
    tsensor_over_temp.d32 = hal_tsensor_get_ctrl_regs()->tsensor_over_temp;
    tsensor_over_temp.b.tsensor_overtemp_thresh_en = TSENSOR_ENABLE;
    hal_tsensor_get_ctrl_regs()->tsensor_over_temp = tsensor_over_temp.d32;
}

static inline void hal_tsensor_v151_reg_set_overtemp_protect_disable(void)
{
    tsensor_over_temp_data_t tsensor_over_temp;
    tsensor_over_temp.d32 = hal_tsensor_get_ctrl_regs()->tsensor_over_temp;
    tsensor_over_temp.b.tsensor_overtemp_thresh_en = TSENSOR_DISABLE;
    hal_tsensor_get_ctrl_regs()->tsensor_over_temp = tsensor_over_temp.d32;
}

static inline void hal_tsensor_v151_reg_set_tsensor_done_intr_enable(uint8_t value)
{
    tsensor_temp_int_en_data_t tsensor_temp_int_en;
    tsensor_temp_int_en.d32 = hal_tsensor_get_ctrl_regs()->tsensor_temp_int_en;
    tsensor_temp_int_en.b.tsensor_done_int_en = value;
    hal_tsensor_get_ctrl_regs()->tsensor_temp_int_en = tsensor_temp_int_en.d32;
}

static inline void hal_tsensor_v151_reg_set_out_temp_threshold_intr_enable(uint8_t value)
{
    tsensor_temp_int_en_data_t tsensor_temp_int_en;
    tsensor_temp_int_en.d32 = hal_tsensor_get_ctrl_regs()->tsensor_temp_int_en;
    tsensor_temp_int_en.b.tsensor_out_thresh_int_en = value;
    hal_tsensor_get_ctrl_regs()->tsensor_temp_int_en = tsensor_temp_int_en.d32;
}

static inline void hal_tsensor_v151_reg_set_overtemp_intr_enable(uint8_t value)
{
    tsensor_temp_int_en_data_t tsensor_temp_int_en;
    tsensor_temp_int_en.d32 = hal_tsensor_get_ctrl_regs()->tsensor_temp_int_en;
    tsensor_temp_int_en.b.tsensor_overtemp_int_en = value;
    hal_tsensor_get_ctrl_regs()->tsensor_temp_int_en = tsensor_temp_int_en.d32;
}

static inline uint32_t hal_tsensor_v151_reg_get_temp_intr_enable_status(void)
{
    return hal_tsensor_get_ctrl_regs()->tsensor_temp_int_en;
}

static inline void hal_tsensor_v151_reg_clr_temp_intr(void)
{
    tsensor_temp_int_clr_data_t tsensor_temp_int_clr;
    tsensor_temp_int_clr.d32 = hal_tsensor_get_ctrl_regs()->tsensor_temp_int_clr;
    tsensor_temp_int_clr.b.tsensor_int_clr = TSENSOR_ENABLE;
    hal_tsensor_get_ctrl_regs()->tsensor_temp_int_clr = tsensor_temp_int_clr.d32;
}

static inline uint32_t hal_tsensor_v151_reg_get_temp_intr_status(void)
{
    return hal_tsensor_get_ctrl_regs()->tsensor_temp_int_sts;
}

static inline void hal_tsensor_v151_reg_set_auto_refresh_period(uint32_t period)
{
    tsensor_auto_refresh_period_data_t tsensor_auto_refresh_period;
    tsensor_auto_refresh_period.d32 = hal_tsensor_get_ctrl_regs()->tsensor_auto_refresh_period;
    tsensor_auto_refresh_period.b.tsensor_auto_refresh_period = period;
    hal_tsensor_get_ctrl_regs()->tsensor_auto_refresh_period = tsensor_auto_refresh_period.d32;
}

static inline void hal_tsensor_v151_reg_set_auto_refresh_enable(uint8_t value)
{
    tsensor_auto_refresh_cfg_data_t tsensor_auto_refresh_cfg;
    tsensor_auto_refresh_cfg.d32 = hal_tsensor_get_ctrl_regs()->tsensor_auto_refresh_cfg;
    tsensor_auto_refresh_cfg.b.tsensor_auto_refresh_enable = value;
    hal_tsensor_get_ctrl_regs()->tsensor_auto_refresh_cfg = tsensor_auto_refresh_cfg.d32;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
