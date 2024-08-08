/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides v150 sio register operation api \n
 *
 * History: \n
 * 2023-01-15， Create file. \n
 */
#ifndef HAL_SIO_V150_REGS_OP_H
#define HAL_SIO_V150_REGS_OP_H

#include <stdint.h>
#include "hal_sio_v150_regs_def.h"
#include "sio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sio_v150_regs_op SIO V150 Regs Operation
 * @ingroup  drivers_hal_sio
 * @{
 */

extern uintptr_t g_hal_sio_regs[I2S_MAX_NUMBER];
#define sios_v150_regs(bus) ((sio_v150_regs_t *)g_hal_sio_regs[bus])

/**
 * @brief  Init the sio which will set the base address of registers.
 */
void hal_sio_v150_regs_init(sio_bus_t bus);


static inline void hal_sio_v150_pos_merge_set_en(sio_bus_t bus, uint32_t val)
{
    sio_v150_i2s_pos_merge_en_data_t pos_merge = {.d32 = sios_v150_regs(bus)->i2s_pos_merge_en};
    pos_merge.b.merge_en = val;
    sios_v150_regs(bus)->i2s_pos_merge_en = pos_merge.d32;
}

/**
 * @brief  Deinit the hal_sio which will clear the base address of registers has been
 *         set by @ref hal_sio_v150_regs_init.
 */
void hal_sio_v150_regs_deinit(sio_bus_t bus);

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.rx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.rx_enable.
 */
void hal_sio_v150_ct_set_set_rx_enable(sio_bus_t bus, uint32_t val);

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.tx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.tx_enable.
 */
void hal_sio_v150_ct_set_set_tx_enable(sio_bus_t bus, uint32_t val);

/**
 * @brief  Get the value of @ref sio_v150_version_data.loop.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_version_data.loop.
 */
static inline uint32_t hal_sio_v150_version_get_loop(sio_bus_t bus)
{
    sio_v150_version_data_t version;
    version.d32 = sios_v150_regs(bus)->version;
    return version.b.loop;
}

/**
 * @brief  Set the value of @ref sio_v150_version_data.loop.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_version_data.loop.
 */
static inline void hal_sio_v150_version_set_loop(sio_bus_t bus, uint32_t val)
{
    sio_v150_version_data_t version = {.d32 = sios_v150_regs(bus)->version};
    version.b.loop = val;
    sios_v150_regs(bus)->version = version.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_mode_data.mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_mode_data.mode.
 */
static inline uint32_t hal_sio_v150_mode_get_mode(sio_bus_t bus)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    return mode.b.mode;
}

/**
 * @brief  Set the value of @ref sio_v150_mode_data.mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_mode_data.mode.
 */
static inline void hal_sio_v150_mode_set_mode(sio_bus_t bus, uint32_t val)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    mode.b.mode = val;
    sios_v150_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_mode_data.pcm_mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_mode_data.pcm_mode.
 */
static inline uint32_t hal_sio_v150_mode_get_pcm_mode(sio_bus_t bus)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    return mode.b.pcm_mode;
}

/**
 * @brief  Set the value of @ref sio_v150_mode_data.pcm_mode.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_mode_data.pcm_mode.
 */
static inline void hal_sio_v150_mode_set_pcm_mode(sio_bus_t bus, uint32_t val)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    mode.b.pcm_mode = val;
    sios_v150_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_mode_data.ext_rec_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_mode_data.ext_rec_en.
 */
static inline uint32_t hal_sio_v150_mode_get_ext_rec_en(sio_bus_t bus)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    return mode.b.ext_rec_en;
}

/**
 * @brief  Set the value of @ref sio_v150_mode_data.ext_rec_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_mode_data.ext_rec_en.
 */
static inline void hal_sio_v150_mode_set_ext_rec_en(sio_bus_t bus, uint32_t val)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    mode.b.ext_rec_en = val;
    sios_v150_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_mode_data.chn_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_mode_data.chn_num.
 */
static inline uint32_t hal_sio_v150_mode_get_chn_num(sio_bus_t bus)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    return mode.b.chn_num;
}

/**
 * @brief  Set the value of @ref sio_v150_mode_data.chn_num.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_mode_data.chn_num.
 */
static inline void hal_sio_v150_mode_set_chn_num(sio_bus_t bus, uint32_t val)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    mode.b.chn_num = val;
    sios_v150_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_mode_data.clk_edge.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_mode_data.clk_edge.
 */
static inline uint32_t hal_sio_v150_mode_get_clk_edge(sio_bus_t bus)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    return mode.b.clk_edge;
}

/**
 * @brief  Set the value of @ref sio_v150_mode_data.clk_edge.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_mode_data.clk_edge.
 */
static inline void hal_sio_v150_mode_set_clk_edge(sio_bus_t bus, uint32_t val)
{
    sio_v150_mode_data_t mode;
    mode.d32 = sios_v150_regs(bus)->mode;
    mode.b.clk_edge = val;
    sios_v150_regs(bus)->mode = mode.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_intstatus_data.rx_intr.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_intstatus_data.rx_intr.
 */
static inline uint32_t hal_sio_v150_intstatus_get_rx_intr(sio_bus_t bus)
{
    sio_v150_intstatus_data_t intstatus;
    intstatus.d32 = sios_v150_regs(bus)->intstatus;
    return intstatus.b.rx_intr;
}

/**
 * @brief  Get the value of @ref sio_v150_intstatus_data.tx_intr.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_intstatus_data.tx_intr.
 */
static inline uint32_t hal_sio_v150_intstatus_get_tx_intr(sio_bus_t bus)
{
    sio_v150_intstatus_data_t intstatus;
    intstatus.d32 = sios_v150_regs(bus)->intstatus;
    return intstatus.b.tx_intr;
}

/**
 * @brief  Get the value of @ref sio_v150_intstatus_data.rx_right_fifo_over.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_intstatus_data.rx_right_fifo_over.
 */
static inline uint32_t hal_sio_v150_intstatus_get_rx_right_fifo_over(sio_bus_t bus)
{
    sio_v150_intstatus_data_t intstatus;
    intstatus.d32 = sios_v150_regs(bus)->intstatus;
    return intstatus.b.rx_right_fifo_over;
}

/**
 * @brief  Get the value of @ref sio_v150_intstatus_data.rx_left_fifo_over.
 * @param  [in]  bus The index of sio. @ref sio_bus_t
 * @return The value of @ref sio_v150_intstatus_data.rx_left_fifo_over.
 */
static inline uint32_t hal_sio_v150_intstatus_get_rx_left_fifo_over(sio_bus_t bus)
{
    sio_v150_intstatus_data_t intstatus;
    intstatus.d32 = sios_v150_regs(bus)->intstatus;
    return intstatus.b.rx_left_fifo_over;
}

/**
 * @brief  Get the value of @ref sio_v150_intstatus_data.tx_right_fifo_under.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_intstatus_data.tx_right_fifo_under.
 */
static inline uint32_t hal_sio_v150_intstatus_get_tx_right_fifo_under(sio_bus_t bus)
{
    sio_v150_intstatus_data_t intstatus;
    intstatus.d32 = sios_v150_regs(bus)->intstatus;
    return intstatus.b.tx_right_fifo_under;
}

/**
 * @brief  Get the value of @ref sio_v150_intstatus_data.tx_left_fifo_under.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_intstatus_data.tx_left_fifo_under.
 */
static inline uint32_t hal_sio_v150_intstatus_get_tx_left_fifo_under(sio_bus_t bus)
{
    sio_v150_intstatus_data_t intstatus;
    intstatus.d32 = sios_v150_regs(bus)->intstatus;
    return intstatus.b.tx_left_fifo_under;
}

/**
 * @brief  Set the value of @ref sio_v150_intclr_data.rx_intr.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_intclr_data.rx_intr.
 */
static inline void hal_sio_v150_intclr_set_rx_intr(sio_bus_t bus, uint32_t val)
{
    sio_v150_intclr_data_t intclr;
    intclr.d32 = sios_v150_regs(bus)->intclr;
    intclr.b.rx_intr = val;
    sios_v150_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v150_intclr_data.tx_intr.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_intclr_data.tx_intr.
 */
static inline void hal_sio_v150_intclr_set_tx_intr(sio_bus_t bus, uint32_t val)
{
    sio_v150_intclr_data_t intclr;
    intclr.d32 = sios_v150_regs(bus)->intclr;
    intclr.b.tx_intr = val;
    sios_v150_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v150_intclr_data.rx_right_fifo_over.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_intclr_data.rx_right_fifo_over.
 */
static inline void hal_sio_v150_intclr_set_rx_right_fifo_over(sio_bus_t bus, uint32_t val)
{
    sio_v150_intclr_data_t intclr;
    intclr.d32 = sios_v150_regs(bus)->intclr;
    intclr.b.rx_right_fifo_over = val;
    sios_v150_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v150_intclr_data.rx_left_fifo_over.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_intclr_data.rx_left_fifo_over.
 */
static inline void hal_sio_v150_intclr_set_rx_left_fifo_over(sio_bus_t bus, uint32_t val)
{
    sio_v150_intclr_data_t intclr;
    intclr.d32 = sios_v150_regs(bus)->intclr;
    intclr.b.rx_left_fifo_over = val;
    sios_v150_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v150_intclr_data.tx_right_fifo_under.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_intclr_data.tx_right_fifo_under.
 */
static inline void hal_sio_v150_intclr_set_tx_right_fifo_under(sio_bus_t bus, uint32_t val)
{
    sio_v150_intclr_data_t intclr;
    intclr.d32 = sios_v150_regs(bus)->intclr;
    intclr.b.tx_right_fifo_under = val;
    sios_v150_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v150_intclr_data.tx_left_fifo_under.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_intclr_data.tx_left_fifo_under.
 */
static inline void hal_sio_v150_intclr_set_tx_left_fifo_under(sio_bus_t bus, uint32_t val)
{
    sio_v150_intclr_data_t intclr;
    intclr.d32 = sios_v150_regs(bus)->intclr;
    intclr.b.tx_left_fifo_under = val;
    sios_v150_regs(bus)->intclr = intclr.d32;
}

/**
 * @brief  Set the value of @ref sio_v150_left_tx_data.data.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_left_tx_data.data.
 */
static inline void hal_sio_v150_left_tx_set_data(sio_bus_t bus, uint32_t val)
{
    sio_v150_left_tx_data_t left_tx;
    left_tx.d32 = sios_v150_regs(bus)->left_tx;
    left_tx.b.data = val;
    sios_v150_regs(bus)->left_tx = left_tx.d32;
}

/**
 * @brief  Set the value of @ref sio_v150_right_tx_data.data.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_right_tx_data.data.
 */
static inline void hal_sio_v150_right_tx_set_data(sio_bus_t bus, uint32_t val)
{
    sio_v150_right_tx_data_t right_tx;
    right_tx.d32 = sios_v150_regs(bus)->right_tx;
    right_tx.b.data = val;
    sios_v150_regs(bus)->right_tx = right_tx.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_left_rx_data.data.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_left_rx_data.data.
 */
static inline uint32_t hal_sio_v150_left_rx_get_data(sio_bus_t bus)
{
    sio_v150_left_rx_data_t left_rx;
    left_rx.d32 = sios_v150_regs(bus)->left_rx;
    return left_rx.b.data;
}

/**
 * @brief  Get the value of @ref sio_v150_right_rx_data.data.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_right_rx_data.data.
 */
static inline uint32_t hal_sio_v150_right_rx_get_data(sio_bus_t bus)
{
    sio_v150_right_rx_data_t right_rx;
    right_rx.d32 = sios_v150_regs(bus)->right_rx;
    return right_rx.b.data;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.tx_fifo_threshold.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.tx_fifo_threshold.
 */
static inline uint32_t hal_sio_v150_ct_set_get_tx_fifo_threshold(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.tx_fifo_threshold;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.tx_fifo_threshold.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.tx_fifo_threshold.
 */
static inline void hal_sio_v150_ct_set_set_tx_fifo_threshold(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    ct_set.b.tx_fifo_threshold = val;
    sios_v150_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.rx_fifo_threshold.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.rx_fifo_threshold.
 */
static inline uint32_t hal_sio_v150_ct_set_get_rx_fifo_threshold(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.rx_fifo_threshold;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.rx_fifo_threshold.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.rx_fifo_threshold.
 */
static inline void hal_sio_v150_ct_set_set_rx_fifo_threshold(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    ct_set.b.rx_fifo_threshold = val;
    sios_v150_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.tx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.tx_data_merge_en.
 */
static inline uint32_t hal_sio_v150_ct_set_get_tx_data_merge_en(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.tx_data_merge_en;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.tx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.tx_data_merge_en.
 */
static inline void hal_sio_v150_ct_set_set_tx_data_merge_en(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    ct_set.b.tx_data_merge_en = val;
    sios_v150_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.rx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.rx_data_merge_en.
 */
static inline uint32_t hal_sio_v150_ct_set_get_rx_data_merge_en(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.rx_data_merge_en;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.rx_data_merge_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.rx_data_merge_en.
 */
static inline void hal_sio_v150_ct_set_set_rx_data_merge_en(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    ct_set.b.rx_data_merge_en = val;
    sios_v150_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.tx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.tx_fifo_disable.
 */
static inline uint32_t hal_sio_v150_ct_set_get_tx_fifo_disable(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.tx_fifo_disable;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.tx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.tx_fifo_disable.
 */
static inline void hal_sio_v150_ct_set_set_tx_fifo_disable(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    ct_set.b.tx_fifo_disable = val;
    sios_v150_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.rx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.rx_fifo_disable.
 */
static inline uint32_t hal_sio_v150_ct_set_get_rx_fifo_disable(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.rx_fifo_disable;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.rx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.rx_fifo_disable.
 */
static inline void hal_sio_v150_ct_set_set_rx_fifo_disable(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    ct_set.b.rx_fifo_disable = val;
    sios_v150_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.tx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.tx_enable.
 */
static inline uint32_t hal_sio_v150_ct_set_get_tx_enable(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.tx_enable;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.rx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.rx_enable.
 */
static inline uint32_t hal_sio_v150_ct_set_get_rx_enable(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.rx_enable;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.intr_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.rx_enable.
 */
static inline uint32_t hal_sio_v150_ct_set_get_intr_en(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.intr_en;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.intr_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.intr_en.
 */
static inline void hal_sio_v150_ct_set_set_intr_en(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    ct_set.b.intr_en = val;
    sios_v150_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_set_data.rst_n.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_set_data.rst_n.
 */
static inline uint32_t hal_sio_v150_ct_set_get_rst_n(sio_bus_t bus)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    return ct_set.b.rst_n;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_set_data.rst_n.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_set_data.rst_n.
 */
static inline void hal_sio_v150_ct_set_set_rst_n(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_set_data_t ct_set;
    ct_set.d32 = sios_v150_regs(bus)->ct_set;
    ct_set.b.rst_n = val;
    sios_v150_regs(bus)->ct_set = ct_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_rx_sta_data.rx_right_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_rx_sta_data.rx_right_depth.
 */
static inline uint32_t hal_sio_v150_rx_sta_get_rx_right_depth(sio_bus_t bus)
{
    sio_v150_rx_sta_data_t rx_sta;
    rx_sta.d32 = sios_v150_regs(bus)->rx_sta;
    return rx_sta.b.rx_right_depth;
}

/**
 * @brief  Get the value of @ref sio_v150_rx_sta_data.rx_left_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_rx_sta_data.rx_left_depth.
 */
static inline uint32_t hal_sio_v150_rx_sta_get_rx_left_depth(sio_bus_t bus)
{
    sio_v150_rx_sta_data_t rx_sta;
    rx_sta.d32 = sios_v150_regs(bus)->rx_sta;
    return rx_sta.b.rx_left_depth;
}

/**
 * @brief  Get the value of @ref sio_v150_tx_sta_data.tx_right_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_tx_sta_data.tx_right_depth.
 */
static inline uint32_t hal_sio_v150_tx_sta_get_tx_right_depth(sio_bus_t bus)
{
    sio_v150_tx_sta_data_t tx_sta;
    tx_sta.d32 = sios_v150_regs(bus)->tx_sta;
    return tx_sta.b.tx_right_depth;
}

/**
 * @brief  Get the value of @ref sio_v150_tx_sta_data.tx_left_depth.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_tx_sta_data.tx_left_depth.
 */
static inline uint32_t hal_sio_v150_tx_sta_get_tx_left_depth(sio_bus_t bus)
{
    sio_v150_tx_sta_data_t tx_sta;
    tx_sta.d32 = sios_v150_regs(bus)->tx_sta;
    return tx_sta.b.tx_left_depth;
}

/**
 * @brief  Get the value of @ref sio_v150_data_width_set_data.tx.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_data_width_set_data.tx.
 */
static inline uint32_t hal_sio_v150_data_width_set_get_tx(sio_bus_t bus)
{
    sio_v150_data_width_set_data_t data_width_set;
    data_width_set.d32 = sios_v150_regs(bus)->data_width_set;
    return data_width_set.b.tx;
}

/**
 * @brief  Set the value of @ref sio_v150_data_width_set_data.tx.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_data_width_set_data.tx.
 */
static inline void hal_sio_v150_data_width_set_set_tx(sio_bus_t bus, uint32_t val)
{
    sio_v150_data_width_set_data_t data_width_set;
    data_width_set.d32 = sios_v150_regs(bus)->data_width_set;
    data_width_set.b.tx = val;
    sios_v150_regs(bus)->data_width_set = data_width_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_data_width_set_data.rx.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_data_width_set_data.rx.
 */
static inline uint32_t hal_sio_v150_data_width_set_get_rx(sio_bus_t bus)
{
    sio_v150_data_width_set_data_t data_width_set;
    data_width_set.d32 = sios_v150_regs(bus)->data_width_set;
    return data_width_set.b.rx;
}

/**
 * @brief  Set the value of @ref sio_v150_data_width_set_data.rx.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_data_width_set_data.rx.
 */
static inline void hal_sio_v150_data_width_set_set_rx(sio_bus_t bus, uint32_t val)
{
    sio_v150_data_width_set_data_t data_width_set;
    data_width_set.d32 = sios_v150_regs(bus)->data_width_set;
    data_width_set.b.rx = val;
    sios_v150_regs(bus)->data_width_set = data_width_set.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_i2s_start_pos_data.start_pos_write.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_i2s_start_pos_data.start_pos_write.
 */
static inline uint32_t hal_sio_v150_start_pos_get_start_pos_write(sio_bus_t bus)
{
    sio_v150_i2s_start_pos_data_t i2s_start_pos;
    i2s_start_pos.d32 = sios_v150_regs(bus)->i2s_start_pos;
    return i2s_start_pos.b.start_pos_write;
}

/**
 * @brief  Set the value of @ref sio_v150_i2s_start_pos_data.start_pos_write.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_i2s_start_pos_data.start_pos_write.
 */
static inline void hal_sio_v150_start_pos_set_start_pos_write(sio_bus_t bus, uint32_t val)
{
    sio_v150_i2s_start_pos_data_t i2s_start_pos;
    i2s_start_pos.d32 = sios_v150_regs(bus)->i2s_start_pos;
    i2s_start_pos.b.start_pos_write = val;
    sios_v150_regs(bus)->i2s_start_pos = i2s_start_pos.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_i2s_start_pos_data.start_pos_read.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_i2s_start_pos_data.start_pos_read.
 */
static inline uint32_t hal_sio_v150_start_pos_get_start_pos_read(sio_bus_t bus)
{
    sio_v150_i2s_start_pos_data_t i2s_start_pos;
    i2s_start_pos.d32 = sios_v150_regs(bus)->i2s_start_pos;
    return i2s_start_pos.b.start_pos_read;
}

/**
 * @brief  Set the value of @ref sio_v150_i2s_start_pos_data.start_pos_read.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_i2s_start_pos_data.start_pos_read.
 */
static inline void hal_sio_v150_start_pos_set_start_pos_read(sio_bus_t bus, uint32_t val)
{
    sio_v150_i2s_start_pos_data_t i2s_start_pos;
    i2s_start_pos.d32 = sios_v150_regs(bus)->i2s_start_pos;
    i2s_start_pos.b.start_pos_read = val;
    sios_v150_regs(bus)->i2s_start_pos = i2s_start_pos.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_i2s_pos_flag_data.start_pos_write.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_i2s_pos_flag_data.start_pos_write.
 */
static inline uint32_t hal_sio_v150_pos_flag_get_start_pos_write(sio_bus_t bus)
{
    sio_v150_i2s_pos_flag_data_t i2s_pos_flag;
    i2s_pos_flag.d32 = sios_v150_regs(bus)->i2s_pos_flag;
    return i2s_pos_flag.b.start_pos_write;
}

/**
 * @brief  Set the value of @ref sio_v150_i2s_pos_flag_data.start_pos_write.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_i2s_pos_flag_data.start_pos_write.
 */
static inline void hal_sio_v150_pos_flag_set_start_pos_write(sio_bus_t bus, uint32_t val)
{
    sio_v150_i2s_pos_flag_data_t i2s_pos_flag;
    i2s_pos_flag.d32 = sios_v150_regs(bus)->i2s_pos_flag;
    i2s_pos_flag.b.start_pos_write = val;
    sios_v150_regs(bus)->i2s_pos_flag = i2s_pos_flag.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_i2s_pos_flag_data.start_pos_read.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @return The value of @ref sio_v150_i2s_pos_flag_data.start_pos_read.
 */
static inline uint32_t hal_sio_v150_pos_flag_get_start_pos_read(sio_bus_t bus)
{
    sio_v150_i2s_pos_flag_data_t i2s_pos_flag;
    i2s_pos_flag.d32 = sios_v150_regs(bus)->i2s_pos_flag;
    return i2s_pos_flag.b.start_pos_read;
}

/**
 * @brief  Set the value of @ref sio_v150_i2s_pos_flag_data.start_pos_read.
 * @param  [in]  bus The index of sio. @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_i2s_pos_flag_data.start_pos_read.
 */
static inline void hal_sio_v150_pos_flag_set_start_pos_read(sio_bus_t bus, uint32_t val)
{
    sio_v150_i2s_pos_flag_data_t i2s_pos_flag;
    i2s_pos_flag.d32 = sios_v150_regs(bus)->i2s_pos_flag;
    i2s_pos_flag.b.start_pos_read = val;
    sios_v150_regs(bus)->i2s_pos_flag = i2s_pos_flag.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_clr_data.tx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_clr_data.tx_fifo_disable.
 */
static inline uint32_t hal_sio_v150_ct_clr_get_tx_fifo_disable(sio_bus_t bus)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    return ct_clr.b.tx_fifo_disable;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_clr_data.tx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_clr_data.tx_fifo_disable.
 */
static inline void hal_sio_v150_ct_clr_set_tx_fifo_disable(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    ct_clr.b.tx_fifo_disable = val;
    sios_v150_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_clr_data.rx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_clr_data.rx_fifo_disable.
 */
static inline uint32_t hal_sio_v150_ct_clr_get_rx_fifo_disable(sio_bus_t bus)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    return ct_clr.b.rx_fifo_disable;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_clr_data.rx_fifo_disable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_clr_data.rx_fifo_disable.
 */
static inline void hal_sio_v150_ct_clr_set_rx_fifo_disable(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    ct_clr.b.rx_fifo_disable = val;
    sios_v150_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_clr_data.tx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_clr_data.tx_enable.
 */
static inline uint32_t hal_sio_v150_ct_clr_get_tx_enable(sio_bus_t bus)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    return ct_clr.b.tx_enable;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_clr_data.tx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_clr_data.tx_enable.
 */
static inline void hal_sio_v150_ct_clr_set_tx_enable(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    ct_clr.b.tx_enable = val;
    sios_v150_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_clr_data.rx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_clr_data.rx_enable.
 */
static inline uint32_t hal_sio_v150_ct_clr_get_rx_enable(sio_bus_t bus)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    return ct_clr.b.rx_enable;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_clr_data.rx_enable.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_clr_data.rx_enable.
 */
static inline void hal_sio_v150_ct_clr_set_rx_enable(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    ct_clr.b.rx_enable = val;
    sios_v150_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_clr_data.intr_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_clr_data.intr_en.
 */
static inline void hal_sio_v150_ct_clr_set_intr_en(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    ct_clr.b.intr_en = val;
    sios_v150_regs(bus)->ct_clr = ct_clr.d32;
}

/**
 * @brief  Get the value of @ref sio_v150_ct_clr_data.intr_en.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @return The value of @ref sio_v150_ct_clr_data.intr_en.
 */
static inline uint32_t hal_sio_v150_ct_clr_get_intr_en(sio_bus_t bus)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    return ct_clr.b.intr_en;
}

/**
 * @brief  Set the value of @ref sio_v150_ct_clr_data.rst_n.
 * @param  [in]  bus The index of sio. @ref sio_bus_t @ref sio_bus_t.
 * @param  [in]  val The value of @ref sio_v150_ct_clr_data.rst_n.
 */
static inline void hal_sio_v150_ct_clr_set_rst_n(sio_bus_t bus, uint32_t val)
{
    sio_v150_ct_clr_data_t ct_clr;
    ct_clr.d32 = sios_v150_regs(bus)->ct_clr;
    ct_clr.b.rst_n = val;
    sios_v150_regs(bus)->ct_clr = ct_clr.d32;
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