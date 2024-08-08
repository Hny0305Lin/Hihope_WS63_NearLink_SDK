/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V151 dma register operation api \n
 *
 * History: \n
 * 2023-2-25， Create file. \n
 */

#ifndef HAL_DMAC_V151_REGS_OP_H
#define HAL_DMAC_V151_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "hal_dmac_v151_regs_def.h"
#include "dma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_dma_v151_regs_op DMA V151 Regs Operation
 * @ingroup  drivers_hal_dma
 * @{
 */

extern uintptr_t g_dma_regs;
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
extern uintptr_t g_sdma_regs;
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

/**
 * @brief  Set the value of @ref dma_v151_cfg_reg_data_t.dma_enable.
 * @param  [in]  val The value of @ref dma_v151_cfg_reg_data_t.dma_enable.
 */
static inline void hal_dma_cfg_set_en(uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_v151_cfg_reg_data_t dma_cfg_reg;
    dma_cfg_reg.d32 = ((dma_v151_regs_t *)dma_regs)->configuration.d32;
    dma_cfg_reg.b.dma_enable = val;
    ((dma_v151_regs_t *)dma_regs)->configuration = dma_cfg_reg;
}

/**
 * @brief  Set the value of @ref dma_cfg_data_t.ch_enable.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_data_t.ch_enable.
 */
static inline void hal_dma_ch_cfg_set_ch_en(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_cfg_data_t ch_data;
    ch_data.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg.d32;
    ch_data.b.ch_enable = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg = ch_data;
}

/**
 * @brief  Get the value of @ref dma_ctrl_data_t.transfersize.
 * @param  [in]  channel The DMA channel.
 * @return The value of @ref dma_ctrl_data_t.transfersize.
 */
static inline uint32_t hal_dma_ctrl_get_tranf_size(dma_channel_t channel, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    return dma_ctrl.b.transfersize;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.transfersize.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.transfersize.
 */
static inline void hal_dma_ctrl_set_tranf_size(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.transfersize = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.dwsize.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.dwsize.
 */
static inline void hal_dma_ctrl_set_dst_tr_width(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.dwsize = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.swsize.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.swsize.
 */
static inline void hal_dma_ctrl_set_src_tr_width(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.swsize = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.dest_inc.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.dest_inc.
 */
static inline void hal_dma_ctrl_set_dest_inc(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.dest_inc = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.src_inc.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.src_inc.
 */
static inline void hal_dma_ctrl_set_src_inc(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.src_inc = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.sbsize.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.sbsize.
 */
static inline void hal_dma_ctrl_set_src_bsize(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.sbsize = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.dbsize.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.dbsize.
 */
static inline void hal_dma_ctrl_set_dest_bsize(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.dbsize = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.dest_ms_sel.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.dest_ms_sel.
 */
static inline void hal_dma_ctrl_set_dest_ms_sel(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.dest_ms_sel = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.src_ms_sel.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.src_ms_sel.
 */
static inline void hal_dma_ctrl_set_src_ms_sel(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.src_ms_sel = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.tc_int_en.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.tc_int_en.
 */
static inline void hal_dma_ctrl_set_tc_int_en(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.tc_int_en = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_cfg_data.int_err_mask.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_data.int_err_mask.
 */
static inline void hal_dma_cfg_set_int_err_mask(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_cfg_data_t dma_cfg;
    dma_cfg.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg.d32;
    dma_cfg.b.int_err_mask = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg = dma_cfg;
}

/**
 * @brief  Set the value of @ref dma_cfg_data.tc_int_mask.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_data.tc_int_mask.
 */
static inline void hal_dma_cfg_set_tc_int_mask(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_cfg_data_t dma_cfg;
    dma_cfg.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg.d32;
    dma_cfg.b.tc_int_mask = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg = dma_cfg;
}

/**
 * @brief  Set the value of @ref dma_cfg_data.fc_tt.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_data.fc_tt.
 */
static inline void hal_dma_cfg_set_fc_tt(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_cfg_data_t dma_cfg;
    dma_cfg.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg.d32;
    dma_cfg.b.fc_tt = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg = dma_cfg;
}

/**
 * @brief  Set the value of @ref dma_cfg_data.dest_per.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_data.dest_per.
 */
static inline void hal_dma_cfg_set_dest_per(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_cfg_data_t dma_cfg;
    dma_cfg.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg.d32;
    dma_cfg.b.dest_per = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg = dma_cfg;
}

/**
 * @brief  Set the value of @ref dma_cfg_data.src_per.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_data.src_per.
 */
static inline void hal_dma_cfg_set_src_per(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_cfg_data_t dma_cfg;
    dma_cfg.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg.d32;
    dma_cfg.b.src_per = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg = dma_cfg;
}

/**
 * @brief  Set the value of @ref dma_ctrl_data.protection.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_data.protection.
 */
static inline void hal_dma_ctrl_set_protection(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_ctrl_data_t dma_ctrl;
    dma_ctrl.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl.d32;
    dma_ctrl.b.protection = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].ctrl = dma_ctrl;
}

/**
 * @brief  Set the value of @ref dma_cfg_data.lock.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_data.lock.
 */
static inline void hal_dma_cfg_set_lock(dma_channel_t channel, uint32_t val, dma_v151_regs_t *dma_regs)
{
    dma_cfg_data_t dma_cfg;
    dma_cfg.d32 = ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg.d32;
    dma_cfg.b.lock = val;
    ((dma_v151_regs_t *)dma_regs)->ch_config[channel].cfg = dma_cfg;
}

/**
 * @brief  Get the status of transfer complete interrupt.
 * @param  [in]  channel The DMA channel.
 * @return The status of transfer complete interrupt.
 */
bool hal_dma_regs_data_get_int_tc_st(dma_channel_t channel, dma_v151_regs_t *dma_regs);

/**
 * @brief  Get the status of transfer error interrupt.
 * @param  [in]  channel The DMA channel.
 * @return The status of transfer error interrupt.
 */
bool hal_dma_regs_data_get_int_err_st(dma_channel_t channel, dma_v151_regs_t *dma_regs);

/**
 * @brief  Clear the status of transfer complete interrupt.
 * @param  [in]  channel The DMA channel. For details, see @ref dma_channel_t.
 */
void hal_dma_interrupt_clear_tc(dma_channel_t channel, dma_v151_regs_t *dma_regs);

/**
 * @brief  Clear the status of transfer error interrupt.
 * @param  [in]  channel The DMA channel. For details, see @ref dma_channel_t.
 */
void hal_dma_interrupt_clear_err(dma_channel_t channel, dma_v151_regs_t *dma_regs);

/**
 * @brief  Get the status of channel enable.
 * @param  [in]  channel The DMA channel.
 * @return The status of channel enable.
 */
bool hal_dma_regs_data_get_ch_en(dma_channel_t channel, dma_v151_regs_t *dma_regs);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif