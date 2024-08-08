/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 sio register operation api \n
 *
 * History: \n
 * 2023-03-07， Create file. \n
 */
#ifndef HAL_SIO_V151_REGS_OP_H
#define HAL_SIO_V151_REGS_OP_H

#include <stdint.h>
#include "hal_sio_v151_regs_def.h"
#include "sio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sio_v151_regs_op SIO V151 Regs Operation
 * @ingroup  drivers_hal_sio
 * @{
 */
extern uintptr_t g_hal_sio_regs[I2S_MAX_NUMBER];
#define sios_v151_regs(bus) ((sio_v151_regs_t *)g_hal_sio_regs[bus])

/**
 * @brief  Init the sio which will set the base address of registers.
 */
void hal_sio_v151_regs_init(sio_bus_t bus);

/**
 * @brief  Deinit the sio which will clear the base address of registers.
 */
void hal_sio_v151_regs_deinit(sio_bus_t bus);

/**
 * @brief  Get the value of @ref sio_v151_version_data.loop.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v151_version_data.loop.
 */
static inline uint32_t hal_sio_v151_version_get_loop(sio_bus_t bus)
{
    sio_v151_version_data_t version;
    version.d32 = sios_v151_regs(bus)->version;
    return version.b.loop;
}

/**
 * @brief  Set the value of @ref sio_v151_version_data.loop.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_version_data.loop.
 */
static inline void hal_sio_v151_version_set_loop(sio_bus_t bus, uint32_t val)
{
    sio_v151_version_data_t version;
    version.d32 = sios_v151_regs(bus)->version;
    version.b.loop = val;
    sios_v151_regs(bus)->version = version.d32;
}

/**
 * @brief  Get the value of @ref sio_v151_mode_data.mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v151_mode_data.mode.
 */
static inline uint32_t hal_sio_v151_mode_get_mode(sio_bus_t bus)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    return mode.b.mode;
}

/**
 * @brief  Set the value of @ref sio_v151_mode_data.mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_mode_data.mode.
 */
static inline void hal_sio_v151_mode_set_mode(sio_bus_t bus, uint32_t val)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    mode.b.mode = val;
    sios_v151_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v151_mode_data.pcm_mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v151_mode_data.pcm_mode.
 */
static inline uint32_t hal_sio_v151_mode_get_pcm_mode(sio_bus_t bus)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    return mode.b.pcm_mode;
}

/**
 * @brief  Set the value of @ref sio_v151_mode_data.pcm_mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_mode_data.pcm_mode.
 */
static inline void hal_sio_v151_mode_set_pcm_mode(sio_bus_t bus, uint32_t val)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    mode.b.pcm_mode = val;
    sios_v151_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v151_mode_data.rxws_select.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v151_mode_data.rxws_select.
 */
static inline uint32_t hal_sio_v151_mode_get_rxws_select(sio_bus_t bus)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    return mode.b.rxws_select;
}

/**
 * @brief  Get the value of @ref sio_v151_mode_data.ext_rec_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v151_mode_data.ext_rec_en.
 */
static inline uint32_t hal_sio_v151_mode_get_ext_rec_en(sio_bus_t bus)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    return mode.b.ext_rec_en;
}

/**
 * @brief  Set the value of @ref sio_v151_mode_data.ext_rec_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_mode_data.ext_rec_en.
 */
static inline void hal_sio_v151_mode_set_ext_rec_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    mode.b.ext_rec_en = val;
    sios_v151_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v151_mode_data.chn_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v151_mode_data.chn_num.
 */
static inline uint32_t hal_sio_v151_mode_get_chn_num(sio_bus_t bus)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    return mode.b.chn_num;
}

/**
 * @brief  Set the value of @ref sio_v151_mode_data.chn_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_mode_data.chn_num.
 */
static inline void hal_sio_v151_mode_set_chn_num(sio_bus_t bus, uint32_t val)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    mode.b.chn_num = val;
    sios_v151_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v151_mode_data.clk_edge.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v151_mode_data.clk_edge.
 */
static inline uint32_t hal_sio_v151_mode_get_clk_edge(sio_bus_t bus)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    return mode.b.clk_edge;
}

/**
 * @brief  Set the value of @ref sio_v151_mode_data.clk_edge.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_mode_data.clk_edge.
 */
static inline void hal_sio_v151_mode_set_clk_edge(sio_bus_t bus, uint32_t val)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    mode.b.clk_edge = val;
    sios_v151_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v151_mode_data.cfg_i2s_ms_mode_sel.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v151_mode_data.cfg_i2s_ms_mode_sel.
 */
static inline uint32_t hal_sio_v151_mode_get_cfg_i2s_ms_mode_sel(sio_bus_t bus)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    return mode.b.cfg_i2s_ms_mode_sel;
}

/**
 * @brief  Set the value of @ref sio_v151_mode_data.cfg_i2s_ms_mode_sel.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_mode_data.cfg_i2s_ms_mode_sel.
 */
static inline void hal_sio_v151_mode_set_cfg_i2s_ms_mode_sel(sio_bus_t bus, uint32_t val)
{
    sio_v151_mode_data_t mode;
    mode.d32 = sios_v151_regs(bus)->mode;
    mode.b.cfg_i2s_ms_mode_sel = val;
    sios_v151_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_intstatus_data.rx_intr.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_intstatus_data.rx_intr.
 */
static inline uint32_t hal_sio_v151_intstatus_get_rx_intr(sio_bus_t bus)
{
    sio_v151_intstatus_data_t intstatus;
    intstatus.d32 = sios_v151_regs(bus)->intstatus;
    return intstatus.b.rx_intr;
}

/**
 * @brief  Set the value of @ref sio_v151_intstatus_data.tx_intr.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_intstatus_data.tx_intr.
 */
static inline uint32_t hal_sio_v151_intstatus_get_tx_intr(sio_bus_t bus)
{
    sio_v151_intstatus_data_t intstatus;
    intstatus.d32 = sios_v151_regs(bus)->intstatus;
    return intstatus.b.tx_intr;
}

/**
 * @brief  Set the value of @ref sio_v151_intstatus_data.rx_right_fifo_over.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_intstatus_data.rx_right_fifo_over.
 */
static inline uint32_t hal_sio_v151_intstatus_get_rx_right_fifo_over(sio_bus_t bus)
{
    sio_v151_intstatus_data_t intstatus;
    intstatus.d32 = sios_v151_regs(bus)->intstatus;
    return intstatus.b.rx_right_fifo_over;
}

/**
 * @brief  Set the value of @ref sio_v151_intstatus_data.rx_left_fifo_over.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_intstatus_data.rx_left_fifo_over.
 */
static inline uint32_t hal_sio_v151_intstatus_get_rx_left_fifo_over(sio_bus_t bus)
{
    sio_v151_intstatus_data_t intstatus;
    intstatus.d32 = sios_v151_regs(bus)->intstatus;
    return intstatus.b.rx_left_fifo_over;
}

/**
 * @brief  Set the value of @ref sio_v151_intstatus_data.tx_right_fifo_under.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_intstatus_data.tx_right_fifo_under.
 */
static inline uint32_t hal_sio_v151_intstatus_get_tx_right_fifo_under(sio_bus_t bus)
{
    sio_v151_intstatus_data_t intstatus;
    intstatus.d32 = sios_v151_regs(bus)->intstatus;
    return intstatus.b.tx_right_fifo_under;
}

/**
 * @brief  Set the value of @ref sio_v151_intstatus_data.tx_left_fifo_under.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_intstatus_data.tx_left_fifo_under.
 */
static inline uint32_t hal_sio_v151_intstatus_get_tx_left_fifo_under(sio_bus_t bus)
{
    sio_v151_intstatus_data_t intstatus;
    intstatus.d32 = sios_v151_regs(bus)->intstatus;
    return intstatus.b.tx_left_fifo_under;
}

/**
 * @brief  Set the value of @ref sio_v151_intclr_data.rx_intr.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_intclr_data.rx_intr.
 */
static inline void hal_sio_v151_intclr_set_rx_intr(sio_bus_t bus, uint32_t val)
{
    sio_v151_intclr_data_t intclr;
    intclr.d32 = sios_v151_regs(bus)->intclr;
    intclr.b.rx_intr = val;
    sios_v151_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_intclr_data.tx_intr.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_intclr_data.tx_intr.
 */
static inline void hal_sio_v151_intclr_set_tx_intr(sio_bus_t bus, uint32_t val)
{
    sio_v151_intclr_data_t intclr;
    intclr.d32 = sios_v151_regs(bus)->intclr;
    intclr.b.tx_intr = val;
    sios_v151_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_intclr_data.rx_right_fifo_over.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_intclr_data.rx_right_fifo_over.
 */
static inline void hal_sio_v151_intclr_set_rx_right_fifo_over(sio_bus_t bus, uint32_t val)
{
    sio_v151_intclr_data_t intclr;
    intclr.d32 = sios_v151_regs(bus)->intclr;
    intclr.b.rx_right_fifo_over = val;
    sios_v151_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_intclr_data.rx_left_fifo_over.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_intclr_data.rx_left_fifo_over.
 */
static inline void hal_sio_v151_intclr_set_rx_left_fifo_over(sio_bus_t bus, uint32_t val)
{
    sio_v151_intclr_data_t intclr;
    intclr.d32 = sios_v151_regs(bus)->intclr;
    intclr.b.rx_left_fifo_over = val;
    sios_v151_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_intclr_data.tx_right_fifo_under.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_intclr_data.tx_right_fifo_under.
 */
static inline void hal_sio_v151_intclr_set_tx_right_fifo_under(sio_bus_t bus, uint32_t val)
{
    sio_v151_intclr_data_t intclr;
    intclr.d32 = sios_v151_regs(bus)->intclr;
    intclr.b.tx_right_fifo_under = val;
    sios_v151_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_intclr_data.tx_left_fifo_under.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_intclr_data.tx_left_fifo_under.
 */
static inline void hal_sio_v151_intclr_set_tx_left_fifo_under(sio_bus_t bus, uint32_t val)
{
    sio_v151_intclr_data_t intclr;
    intclr.d32 = sios_v151_regs(bus)->intclr;
    intclr.b.tx_left_fifo_under = val;
    sios_v151_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_i2s_left_xd_data.tx_left_data.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_left_xd_data.tx_left_data.
 */
static inline void hal_sio_v151_i2s_left_xd_set_tx_left_data(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_left_xd_data_t i2s_left_xd;
    i2s_left_xd.d32 = sios_v151_regs(bus)->left_tx;
    i2s_left_xd.b.tx_left_data = val;
    sios_v151_regs(bus)->left_tx = i2s_left_xd.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_i2s_right_xd_data.tx_right_data.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_right_xd_data.tx_right_data.
 */
static inline void hal_sio_v151_i2s_right_xd_set_tx_right_data(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_right_xd_data_t i2s_right_xd;
    i2s_right_xd.d32 = sios_v151_regs(bus)->right_tx;
    i2s_right_xd.b.tx_right_data = val;
    sios_v151_regs(bus)->right_tx = i2s_right_xd.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_left_rd_data.rx_left_data.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_left_rd_data.rx_left_data.
 */
static inline uint32_t hal_sio_v151_i2s_left_rd_get_rx_left_data(sio_bus_t bus)
{
    sio_v151_i2s_left_rd_data_t i2s_left_rd;
    i2s_left_rd.d32 = sios_v151_regs(bus)->left_rx;
    return i2s_left_rd.b.rx_left_data;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_right_rd_data.rx_right_data.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_right_rd_data.rx_right_data.
 */
static inline uint32_t hal_sio_v151_i2s_right_rd_get_rx_right_data(sio_bus_t bus)
{
    sio_v151_i2s_right_rd_data_t i2s_right_rd;
    i2s_right_rd.d32 = sios_v151_regs(bus)->right_rx;
    return i2s_right_rd.b.rx_right_data;
}

/**
 * @brief Get the value of @ref sio_v151_ct_set_data.tx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_set_data.tx_data_merge_en.
 */
static inline uint32_t hal_sio_v151_ct_set_get_tx_data_merge_en(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    return ct_set.b.tx_data_merge_en;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_set_data.tx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_set_data.tx_data_merge_en.
 */
static inline void hal_sio_v151_ct_set_set_tx_data_merge_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.tx_data_merge_en = val;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_set_data.rx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_set_data.rx_data_merge_en.
 */
static inline uint32_t hal_sio_v151_ct_set_get_rx_data_merge_en(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    return ct_set.b.rx_data_merge_en;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_set_data.rx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_set_data.rx_data_merge_en.
 */
static inline void hal_sio_v151_ct_set_set_rx_data_merge_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.rx_data_merge_en = val;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_set_data.tx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_set_data.tx_fifo_disable.
 */
static inline uint32_t hal_sio_v151_ct_set_get_tx_fifo_disable(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    return ct_set.b.tx_fifo_disable;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_set_data.tx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_set_data.tx_fifo_disable.
 */
static inline void hal_sio_v151_ct_set_set_tx_fifo_disable(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.tx_fifo_disable = val;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_set_data.rx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_set_data.rx_fifo_disable.
 */
static inline uint32_t hal_sio_v151_ct_set_get_rx_fifo_disable(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    return ct_set.b.rx_fifo_disable;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_set_data.rx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_set_data.rx_fifo_disable.
 */
static inline void hal_sio_v151_ct_set_set_rx_fifo_disable(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.rx_fifo_disable = val;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_set_data.tx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_set_data.tx_enable.
 */
static inline uint32_t hal_sio_v151_ct_set_get_tx_enable(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    return ct_set.b.tx_enable;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_set_data.tx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_set_data.tx_enable.
 */
static inline void hal_sio_v151_ct_set_set_tx_enable(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.tx_enable = val;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_set_data.rx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_set_data.rx_enable.
 */
static inline uint32_t hal_sio_v151_ct_set_get_rx_enable(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    return ct_set.b.rx_enable;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_set_data.rx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_set_data.rx_enable.
 */
static inline void hal_sio_v151_ct_set_set_rx_enable(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.rx_enable = val;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_set_data.intr_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_set_data.intr_en.
 */
static inline uint32_t hal_sio_v151_ct_set_get_intr_en(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    return ct_set.b.intr_en;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_set_data.intr_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_set_data.intr_en.
 */
static inline void hal_sio_v151_ct_set_set_intr_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.intr_en = val;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v151_ct_set_data.rst_n.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_set_data.rst_n.
 */
static inline uint32_t hal_sio_v151_ct_set_get_rst_n(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    return ct_set.b.rst_n;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_set_data.rst_n.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_set_data.rst_n.
 */
static inline void hal_sio_v151_ct_set_set_rst_n(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.rst_n = val;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_clr_data.tx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_clr_data.tx_data_merge_en.
 */
static inline void hal_sio_v151_ct_clr_set_tx_data_merge_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    ct_clr.b.tx_data_merge_en = val;
    sios_v151_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_clr_data.rx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_clr_data.rx_data_merge_en.
 */
static inline uint32_t hal_sio_v151_ct_clr_get_rx_data_merge_en(sio_bus_t bus)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    return ct_clr.b.rx_data_merge_en;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_clr_data.rx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_clr_data.rx_data_merge_en.
 */
static inline void hal_sio_v151_ct_clr_set_rx_data_merge_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    ct_clr.b.rx_data_merge_en = val;
    sios_v151_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_clr_data.tx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_clr_data.tx_fifo_disable.
 */
static inline uint32_t hal_sio_v151_ct_clr_get_tx_fifo_disable(sio_bus_t bus)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    return ct_clr.b.tx_fifo_disable;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_clr_data.tx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_clr_data.tx_fifo_disable.
 */
static inline void hal_sio_v151_ct_clr_set_tx_fifo_disable(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    ct_clr.b.tx_fifo_disable = val;
    sios_v151_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_clr_data.rx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_clr_data.rx_fifo_disable.
 */
static inline uint32_t hal_sio_v151_ct_clr_get_rx_fifo_disable(sio_bus_t bus)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    return ct_clr.b.rx_fifo_disable;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_clr_data.rx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_clr_data.rx_fifo_disable.
 */
static inline void hal_sio_v151_ct_clr_set_rx_fifo_disable(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    ct_clr.b.rx_fifo_disable = val;
    sios_v151_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_clr_data.tx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_clr_data.tx_enable.
 */
static inline uint32_t hal_sio_v151_ct_clr_get_tx_enable(sio_bus_t bus)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    return ct_clr.b.tx_enable;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_clr_data.tx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_clr_data.tx_enable.
 */
static inline void hal_sio_v151_ct_clr_set_tx_enable(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    ct_clr.b.tx_enable = val;
    sios_v151_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_clr_data.rx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_clr_data.rx_enable.
 */
static inline uint32_t hal_sio_v151_ct_clr_get_rx_enable(sio_bus_t bus)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    return ct_clr.b.rx_enable;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_clr_data.rx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_clr_data.rx_enable.
 */
static inline void hal_sio_v151_ct_clr_set_rx_enable(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    ct_clr.b.rx_enable = val;
    sios_v151_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_clr_data.intr_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_clr_data.intr_en.
 */
static inline void hal_sio_v151_ct_clr_set_intr_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    ct_clr.b.intr_en = val;
    sios_v151_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief Get the value of @ref sio_v151_ct_clr_data.intr_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_ct_clr_data.intr_en.
 */
static inline uint32_t hal_sio_v151_ct_clr_get_intr_en(sio_bus_t bus)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    return ct_clr.b.intr_en;
}

/**
 * @brief  Set the value of @ref sio_v151_ct_clr_data.rst_n.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_ct_clr_data.rst_n.
 */
static inline void hal_sio_v151_ct_clr_set_rst_n(sio_bus_t bus, uint32_t val)
{
    sio_v151_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v151_regs(bus)->ct_clr;
    ct_clr.b.rst_n = val;
    sios_v151_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief Get the value of @ref sio_v151_fifo_threshold_data.rx_fifo_threshold.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_fifo_threshold_data.rx_fifo_threshold.
 */
static inline uint32_t hal_sio_v151_fifo_threshold_get_rx_fifo_threshold(sio_bus_t bus)
{
    sio_v151_fifo_threshold_data_t fifo_threshold;
    fifo_threshold.d32 = sios_v151_regs(bus)->fifo_threshold;
    return fifo_threshold.b.rx_fifo_threshold;
}

/**
 * @brief  Set the value of @ref sio_v151_fifo_threshold_data.rx_fifo_threshold.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_fifo_threshold_data.rx_fifo_threshold.
 */
static inline void hal_sio_v151_fifo_threshold_set_rx_fifo_threshold(sio_bus_t bus, uint32_t val)
{
    sio_v151_fifo_threshold_data_t fifo_threshold;
    fifo_threshold.d32 = sios_v151_regs(bus)->fifo_threshold;
    fifo_threshold.b.rx_fifo_threshold = val;
    sios_v151_regs(bus)->fifo_threshold = fifo_threshold.d32;
}

/**
 * @brief Get the value of @ref sio_v151_fifo_threshold_data.tx_fifo_threshold.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_fifo_threshold_data.tx_fifo_threshold.
 */
static inline uint32_t hal_sio_v151_fifo_threshold_get_tx_fifo_threshold(sio_bus_t bus)
{
    sio_v151_fifo_threshold_data_t fifo_threshold;
    fifo_threshold.d32 = sios_v151_regs(bus)->fifo_threshold;
    return fifo_threshold.b.tx_fifo_threshold;
}

/**
 * @brief  Set the value of @ref sio_v151_fifo_threshold_data.tx_fifo_threshold.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_fifo_threshold_data.tx_fifo_threshold.
 */
static inline void hal_sio_v151_fifo_threshold_set_tx_fifo_threshold(sio_bus_t bus, uint32_t val)
{
    sio_v151_fifo_threshold_data_t fifo_threshold;
    fifo_threshold.d32 = sios_v151_regs(bus)->fifo_threshold;
    fifo_threshold.b.tx_fifo_threshold = val;
    sios_v151_regs(bus)->fifo_threshold = fifo_threshold.d32;
}

/**
 * @brief Get the value of @ref sio_v151_rx_sta_data.rx_left_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_rx_sta_data.rx_left_depth.
 */
static inline uint32_t hal_sio_v151_rx_sta_get_rx_left_depth(sio_bus_t bus)
{
    sio_v151_rx_sta_data_t rx_sta;
    rx_sta.d32 = sios_v151_regs(bus)->rx_sta;
    return rx_sta.b.rx_left_depth;
}

/**
 * @brief Get the value of @ref sio_v151_rx_sta_data.rx_right_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_rx_sta_data.rx_right_depth.
 */
static inline uint32_t hal_sio_v151_rx_sta_get_rx_right_depth(sio_bus_t bus)
{
    sio_v151_rx_sta_data_t rx_sta;
    rx_sta.d32 = sios_v151_regs(bus)->rx_sta;
    return rx_sta.b.rx_right_depth;
}

/**
 * @brief Get the value of @ref sio_v151_tx_sta_data.tx_left_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_tx_sta_data.tx_left_depth.
 */
static inline uint32_t hal_sio_v151_tx_sta_get_tx_left_depth(sio_bus_t bus)
{
    sio_v151_tx_sta_data_t tx_sta;
    tx_sta.d32 = sios_v151_regs(bus)->tx_sta;
    return tx_sta.b.tx_left_depth;
}

/**
 * @brief  Set the value of @ref sio_v151_tx_sta_data.tx_left_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_tx_sta_data.tx_left_depth.
 */
static inline void hal_sio_v151_tx_sta_set_tx_left_depth(sio_bus_t bus, uint32_t val)
{
    sio_v151_tx_sta_data_t tx_sta;
    tx_sta.d32 = sios_v151_regs(bus)->tx_sta;
    tx_sta.b.tx_left_depth = val;
    sios_v151_regs(bus)->tx_sta = tx_sta.d32;
}

/**
 * @brief Get the value of @ref sio_v151_tx_sta_data.tx_right_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_tx_sta_data.tx_right_depth.
 */
static inline uint32_t hal_sio_v151_tx_sta_get_tx_right_depth(sio_bus_t bus)
{
    sio_v151_tx_sta_data_t tx_sta;
    tx_sta.d32 = sios_v151_regs(bus)->tx_sta;
    return tx_sta.b.tx_right_depth;
}

/**
 * @brief  Set the value of @ref sio_v151_tx_sta_data.tx_right_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_tx_sta_data.tx_right_depth.
 */
static inline void hal_sio_v151_tx_sta_set_tx_right_depth(sio_bus_t bus, uint32_t val)
{
    sio_v151_tx_sta_data_t tx_sta;
    tx_sta.d32 = sios_v151_regs(bus)->tx_sta;
    tx_sta.b.tx_right_depth = val;
    sios_v151_regs(bus)->tx_sta = tx_sta.d32;
}

/**
 * @brief Get the value of @ref sio_v151_data_width_set_data.tx_mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_data_width_set_data.tx_mode.
 */
static inline uint32_t hal_sio_v151_data_width_get_tx_mode(sio_bus_t bus)
{
    sio_v151_data_width_set_data_t data_width_set;
    data_width_set.d32 = sios_v151_regs(bus)->data_width_set;
    return data_width_set.b.tx_mode;
}

/**
 * @brief  Set the value of @ref sio_v151_data_width_set_data.tx_mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_data_width_set_data.tx_mode.
 */
static inline void hal_sio_v151_data_width_set_tx_mode(sio_bus_t bus, uint32_t val)
{
    sio_v151_data_width_set_data_t data_width_set;
    data_width_set.d32 = sios_v151_regs(bus)->data_width_set;
    data_width_set.b.tx_mode = val;
    sios_v151_regs(bus)->data_width_set = data_width_set.d32;
}

/**
 * @brief Get the value of @ref sio_v151_data_width_set_data.rx_mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_data_width_set_data.rx_mode.
 */
static inline uint32_t hal_sio_v151_data_width_get_rx_mode(sio_bus_t bus)
{
    sio_v151_data_width_set_data_t data_width_set;
    data_width_set.d32 = sios_v151_regs(bus)->data_width_set;
    return data_width_set.b.rx_mode;
}

/**
 * @brief  Set the value of @ref sio_v151_data_width_set_data.rx_mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_data_width_set_data.rx_mode.
 */
static inline void hal_sio_v151_data_width_set_rx_mode(sio_bus_t bus, uint32_t val)
{
    sio_v151_data_width_set_data_t data_width_set;
    data_width_set.d32 = sios_v151_regs(bus)->data_width_set;
    data_width_set.b.rx_mode = val;
    sios_v151_regs(bus)->data_width_set = data_width_set.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_start_pos_data.start_post_read.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_start_pos_data.start_post_read.
 */
static inline uint32_t hal_sio_v151_start_pos_get_read(sio_bus_t bus)
{
    sio_v151_i2s_start_pos_data_t start_pos;
    start_pos.d32 = sios_v151_regs(bus)->i2s_start_pos;
    return start_pos.b.start_post_read;
}

/**
 * @brief  Set the value of @ref sio_v151_i2s_start_pos_data.start_post_read.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_start_pos_data.start_post_read.
 */
static inline void hal_sio_v151_start_pos_set_read(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_start_pos_data_t start_pos;
    start_pos.d32 = sios_v151_regs(bus)->i2s_start_pos;
    start_pos.b.start_post_read = val;
    sios_v151_regs(bus)->i2s_start_pos = start_pos.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_start_pos_data.start_pos_write.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_start_pos_data.start_pos_write.
 */
static inline uint32_t hal_sio_v151_start_pos_get_write(sio_bus_t bus)
{
    sio_v151_i2s_start_pos_data_t start_pos;
    start_pos.d32 = sios_v151_regs(bus)->i2s_start_pos;
    return start_pos.b.start_pos_write;
}

/**
 * @brief  Set the value of @ref sio_v151_i2s_start_pos_data.start_pos_write.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_start_pos_data.start_pos_write.
 */
static inline void hal_sio_v151_start_pos_set_write(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_start_pos_data_t start_pos;
    start_pos.d32 = sios_v151_regs(bus)->i2s_start_pos;
    start_pos.b.start_pos_write = val;
    sios_v151_regs(bus)->i2s_start_pos = start_pos.d32;
}

/**
 * @brief Get the value of @ref sio_v151_pos_flag_data.start_post_read.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_pos_flag_data.start_post_read.
 */
static inline uint32_t hal_sio_v151_pos_flag_get_read(sio_bus_t bus)
{
    sio_v151_pos_flag_data_t pos_flag;
    pos_flag.d32 = sios_v151_regs(bus)->i2s_pos_flag;
    return pos_flag.b.start_post_read;
}

/**
 * @brief  Set the value of @ref sio_v151_pos_flag_data.start_post_read.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_pos_flag_data.start_post_read.
 */
static inline void hal_sio_v151_pos_flag_set_read(sio_bus_t bus, uint32_t val)
{
    sio_v151_pos_flag_data_t pos_flag;
    pos_flag.d32 = sios_v151_regs(bus)->i2s_pos_flag;
    pos_flag.b.start_post_read = val;
    sios_v151_regs(bus)->i2s_pos_flag = pos_flag.d32;
}

/**
 * @brief Get the value of @ref sio_v151_pos_flag_data.start_pos_write.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_pos_flag_data.start_pos_write.
 */
static inline uint32_t hal_sio_v151_pos_flag_get_write(sio_bus_t bus)
{
    sio_v151_pos_flag_data_t pos_flag;
    pos_flag.d32 = sios_v151_regs(bus)->i2s_pos_flag;
    return pos_flag.b.start_pos_write;
}

/**
 * @brief  Set the value of @ref sio_v151_pos_flag_data.start_pos_write.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_pos_flag_data.start_pos_write.
 */
static inline void hal_sio_v151_pos_flag_set_write(sio_bus_t bus, uint32_t val)
{
    sio_v151_pos_flag_data_t pos_flag;
    pos_flag.d32 = sios_v151_regs(bus)->i2s_pos_flag;
    pos_flag.b.start_pos_write = val;
    sios_v151_regs(bus)->i2s_pos_flag = pos_flag.d32;
}

/**
 * @brief Get the value of @ref sio_v151_signed_ext_data.signed_ext_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_signed_ext_data.signed_ext_en.
 */
static inline uint32_t hal_sio_v151_signed_ext_get_en(sio_bus_t bus)
{
    sio_v151_signed_ext_data_t signed_ext;
    signed_ext.d32 = sios_v151_regs(bus)->signed_ext;
    return signed_ext.b.signed_ext_en;
}

/**
 * @brief Set the value of @ref sio_v151_signed_ext_data.signed_ext_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_signed_ext_data.signed_ext_en.
 */
static inline void hal_sio_v151_signed_ext_set_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_signed_ext_data_t signed_ext;
    signed_ext.d32 = sios_v151_regs(bus)->signed_ext;
    signed_ext.b.signed_ext_en = val;
    sios_v151_regs(bus)->signed_ext = signed_ext.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_pos_merge_en_data.merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_pos_merge_en_data.merge_en.
 */
static inline uint32_t hal_sio_v151_pos_merge_get_en(sio_bus_t bus)
{
    sio_v151_i2s_pos_merge_en_data_t pos_merge;
    pos_merge.d32 = sios_v151_regs(bus)->i2s_pos_merge_en;
    return pos_merge.b.merge_en;
}

/**
 * @brief Set the value of @ref sio_v151_i2s_pos_merge_en_data.merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_pos_merge_en_data.merge_en.
 */
static inline void hal_sio_v151_pos_merge_set_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_pos_merge_en_data_t pos_merge;
    pos_merge.d32 = sios_v151_regs(bus)->i2s_pos_merge_en;
    pos_merge.b.merge_en = val;
    sios_v151_regs(bus)->i2s_pos_merge_en = pos_merge.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_crg_data.i2s_bclk_div_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_crg_data.i2s_bclk_div_en.
 */
static inline uint32_t hal_sio_v151_i2s_crg_get_bclk_div_en(sio_bus_t bus)
{
    sio_v151_i2s_crg_data_t i2s_crg;
    i2s_crg.d32 = sios_v151_regs(bus)->i2s_crg;
    return i2s_crg.b.i2s_bclk_div_en;
}

/**
 * @brief Set the value of @ref sio_v151_i2s_crg_data.i2s_bclk_div_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_crg_data.i2s_bclk_div_en.
 */
static inline void hal_sio_v151_i2s_crg_set_bclk_div_en(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_crg_data_t i2s_crg;
    i2s_crg.d32 = sios_v151_regs(bus)->i2s_crg;
    i2s_crg.b.i2s_bclk_div_en = val;
    sios_v151_regs(bus)->i2s_crg = i2s_crg.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_crg_data.i2s_crg_clken.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_crg_data.i2s_crg_clken.
 */
static inline uint32_t hal_sio_v151_i2s_crg_get_crg_clken(sio_bus_t bus)
{
    sio_v151_i2s_crg_data_t i2s_crg;
    i2s_crg.d32 = sios_v151_regs(bus)->i2s_crg;
    return i2s_crg.b.i2s_crg_clken;
}

/**
 * @brief Set the value of @ref sio_v151_i2s_crg_data.i2s_crg_clken.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_crg_data.i2s_crg_clken.
 */
static inline void hal_sio_v151_i2s_crg_set_crg_clken(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_crg_data_t i2s_crg;
    i2s_crg.d32 = sios_v151_regs(bus)->i2s_crg;
    i2s_crg.b.i2s_crg_clken = val;
    sios_v151_regs(bus)->i2s_crg = i2s_crg.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_crg_data.i2s_bclk_sel.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_crg_data.i2s_bclk_sel.
 */
static inline uint32_t hal_sio_v151_i2s_crg_get_bclk_sel(sio_bus_t bus)
{
    sio_v151_i2s_crg_data_t i2s_crg;
    i2s_crg.d32 = sios_v151_regs(bus)->i2s_crg;
    return i2s_crg.b.i2s_bclk_sel;
}

/**
 * @brief Set the value of @ref sio_v151_i2s_crg_data.i2s_bclk_sel.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_crg_data.i2s_bclk_sel.
 */
static inline void hal_sio_v151_i2s_crg_set_bclk_sel(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_crg_data_t i2s_crg;
    i2s_crg.d32 = sios_v151_regs(bus)->i2s_crg;
    i2s_crg.b.i2s_bclk_sel = val;
    sios_v151_regs(bus)->i2s_crg = i2s_crg.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_crg_data.i2s_fs_sel.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_crg_data.i2s_fs_sel.
 */
static inline uint32_t hal_sio_v151_i2s_crg_get_fs_sel(sio_bus_t bus)
{
    sio_v151_i2s_crg_data_t i2s_crg;
    i2s_crg.d32 = sios_v151_regs(bus)->i2s_crg;
    return i2s_crg.b.i2s_fs_sel;
}

/**
 * @brief Set the value of @ref sio_v151_i2s_crg_data.i2s_fs_sel.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_crg_data.i2s_fs_sel.
 */
static inline void hal_sio_v151_i2s_crg_set_fs_sel(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_crg_data_t i2s_crg;
    i2s_crg.d32 = sios_v151_regs(bus)->i2s_crg;
    i2s_crg.b.i2s_fs_sel = val;
    sios_v151_regs(bus)->i2s_crg = i2s_crg.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_bclk_div_num_data.bclk_div_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_bclk_div_num_data.bclk_div_num.
 */
static inline uint32_t hal_sio_v151_bclk_div_num_get_num(sio_bus_t bus)
{
    sio_v151_i2s_bclk_div_num_data_t i2s_bclk;
    i2s_bclk.d32 = sios_v151_regs(bus)->i2s_bclk_div_num;
    return i2s_bclk.b.bclk_div_num;
}

/**
 * @brief Set the value of @ref sio_v151_i2s_bclk_div_num_data.bclk_div_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_bclk_div_num_data.bclk_div_num.
 */
static inline void hal_sio_v151_bclk_div_num_set_num(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_bclk_div_num_data_t i2s_bclk;
    i2s_bclk.d32 = sios_v151_regs(bus)->i2s_bclk_div_num;
    i2s_bclk.b.bclk_div_num = val;
    sios_v151_regs(bus)->i2s_bclk_div_num = i2s_bclk.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_fs_div_num_data.fs_div_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_fs_div_num_data.fs_div_num.
 */
static inline uint32_t hal_sio_v151_fs_div_num_get_num(sio_bus_t bus)
{
    sio_v151_i2s_fs_div_num_data_t i2s_fs;
    i2s_fs.d32 = sios_v151_regs(bus)->i2s_fs_div_num;
    return i2s_fs.b.fs_div_num;
}

/**
 * @brief Set the value of @ref sio_v151_i2s_fs_div_num_data.fs_div_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_fs_div_num_data.fs_div_num.
 */
static inline void hal_sio_v151_fs_div_num_set_num(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_fs_div_num_data_t i2s_fs;
    i2s_fs.d32 = sios_v151_regs(bus)->i2s_fs_div_num;
    i2s_fs.b.fs_div_num = val;
    sios_v151_regs(bus)->i2s_fs_div_num = i2s_fs.d32;
}

/**
 * @brief Get the value of @ref sio_v151_i2s_fs_div_ratio_num_data.fs_div_ratio_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v151_i2s_fs_div_ratio_num_data.fs_div_ratio_num.
 */
static inline uint32_t hal_sio_v151_fs_div_ratio_num_get_num(sio_bus_t bus)
{
    sio_v151_i2s_fs_div_ratio_num_data_t i2s_fs;
    i2s_fs.d32 = sios_v151_regs(bus)->i2s_fs_div_ratio_num;
    return i2s_fs.b.fs_div_ratio_num;
}

/**
 * @brief Set the value of @ref sio_v151_i2s_fs_div_ratio_num_data.fs_div_ratio_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v151_i2s_fs_div_ratio_num_data.fs_div_ratio_num.
 */
static inline void hal_sio_v151_fs_div_ratio_num_set_num(sio_bus_t bus, uint32_t val)
{
    sio_v151_i2s_fs_div_ratio_num_data_t i2s_fs;
    i2s_fs.d32 = sios_v151_regs(bus)->i2s_fs_div_ratio_num;
    i2s_fs.b.fs_div_ratio_num = val;
    sios_v151_regs(bus)->i2s_fs_div_ratio_num = i2s_fs.d32;
}

static inline void hal_sio_fifo_threshold_set_tx_fifo_threshold(sio_bus_t bus, uint32_t val)
{
    uint32_t threshold = sios_v151_regs(bus)->fifo_threshold;
    threshold = (threshold & 0xff00);
    threshold = (threshold | (val & 0xff));
    sios_v151_regs(bus)->fifo_threshold = threshold;
}

#define SIO_RX_FIFO_SHIFT_MASK 8

static inline void hal_sio_fifo_threshold_set_rx_fifo_threshold(sio_bus_t bus, uint32_t val)
{
    uint32_t threshold = sios_v151_regs(bus)->fifo_threshold;
    threshold = (threshold & 0xff);
    threshold = (threshold | ((val & 0xff) << SIO_RX_FIFO_SHIFT_MASK));
    sios_v151_regs(bus)->fifo_threshold = threshold;
}

static inline void hal_sio_set_intmask(sio_bus_t bus, uint32_t mask)
{
    sios_v151_regs(bus)->intmask = mask;
}

static inline uint32_t hal_sio_get_merge_rx_data(sio_bus_t bus)
{
    uint32_t i2s_rd;
    i2s_rd = sios_v151_regs(bus)->i2s_dual_rx_chn;
    return i2s_rd;
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