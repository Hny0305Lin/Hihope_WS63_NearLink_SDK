/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 ssi register operation api \n
 *
 * History: \n
 * 2022-08-08, Create file. \n
 */
#ifndef HAL_SPI_V151_REGS_OP_H
#define HAL_SPI_V151_REGS_OP_H

#include <stdint.h>
#include "common_def.h"
#include "hal_spi_v151_regs_def.h"
#include "spi_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_spi_v151_regs_op SPI V151 Regs Operation
 * @ingroup  drivers_hal_spi
 * @{
 */

/**
 * @brief  The registers list of ssi interrupt.
 */
typedef enum spi_v151_int_reg {
    SPI_INMAR_REG,               /*!< Interrupt Mask Register. */
    SPI_INSR_REG,                /*!< Interrupt Status Register. */
    SPI_RAINSR_REG               /*!< Raw Interrupt Status Register. */
} spi_v151_int_reg_t;

extern uintptr_t g_hal_spis_regs[SPI_BUS_MAX_NUM];
#define spis_v151_regs(bus) ((spi_v151_regs_t *)g_hal_spis_regs[bus])

/**
 * @brief  Get the value of @ref spi_ctra_data.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.d32;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.
 */
static inline void hal_spi_v151_spi_ctra_set(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.dfs16.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.dfs16.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_dfs16(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.dfs16;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.dfs16.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.dfs16.
 */
static inline void hal_spi_v151_spi_ctra_set_dfs16(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.dfs16 = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.prs.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.prs.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_prs(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.prs;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.prs.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.prs.
 */
static inline void hal_spi_v151_spi_ctra_set_prs(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.prs = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.scph.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.scph.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_scph(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.scph;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.scph.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.scph.
 */
static inline void hal_spi_v151_spi_ctra_set_scph(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.scph = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.scpol.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.scpol.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_scpol(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.scpol;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.scpol.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.scpol.
 */
static inline void hal_spi_v151_spi_ctra_set_scpol(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.scpol = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.trsm.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.trsm.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_trsm(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.trsm;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.trsm.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.trsm.
 */
static inline void hal_spi_v151_spi_ctra_set_trsm(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.trsm = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.soe.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.soe.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_soe(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.soe;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.soe.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.soe.
 */
static inline void hal_spi_v151_spi_ctra_set_soe(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.soe = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.srlt.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.srlt.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_srlt(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.srlt;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.srlt.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.srlt.
 */
static inline void hal_spi_v151_spi_ctra_set_srlt(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.srlt = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.cfs16.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.cfs16.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_cfs16(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.cfs16;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.cfs16.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.cfs16.
 */
static inline void hal_spi_v151_spi_ctra_set_cfs16(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.cfs16 = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.dfs32.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.dfs32.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_dfs32(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.dfs32;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.dfs32.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.dfs32.
 */
static inline void hal_spi_v151_spi_ctra_set_dfs32(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.dfs32 = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.enhff.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.enhff.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_enhff(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.enhff;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.enhff.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.enhff.
 */
static inline void hal_spi_v151_spi_ctra_set_enhff(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.enhff = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctra_data.ssn_te.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctra_data.ssn_te.
 */
static inline uint32_t hal_spi_v151_spi_ctra_get_ssn_te(spi_bus_t bus)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    return spi_ctra.b.ssn_te;
}

/**
 * @brief  Set the value of @ref spi_ctra_data.ssn_te.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctra_data.ssn_te.
 */
static inline void hal_spi_v151_spi_ctra_set_ssn_te(spi_bus_t bus, uint32_t val)
{
    spi_ctra_data_t spi_ctra;
    spi_ctra.d32 = spis_v151_regs(bus)->spi_ctra;
    spi_ctra.b.ssn_te = val;
    spis_v151_regs(bus)->spi_ctra = spi_ctra.d32;
}

/**
 * @brief  Get the value of @ref spi_ctrb_data.nrdf.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_ctrb_data.nrdf.
 */
static inline uint32_t hal_spi_v151_spi_ctrb_get_nrdf(spi_bus_t bus)
{
    spi_ctrb_data_t spi_ctrb;
    spi_ctrb.d32 = spis_v151_regs(bus)->spi_ctrb;
    return spi_ctrb.b.nrdf;
}

/**
 * @brief  Set the value of @ref spi_ctrb_data.nrdf.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_ctrb_data.nrdf.
 */
static inline void hal_spi_v151_spi_ctrb_set_nrdf(spi_bus_t bus, uint32_t val)
{
    spi_ctrb_data_t spi_ctrb;
    spi_ctrb.d32 = spis_v151_regs(bus)->spi_ctrb;
    spi_ctrb.b.nrdf = val;
    spis_v151_regs(bus)->spi_ctrb = spi_ctrb.d32;
}

/**
 * @brief  Get the value of @ref spi_er_data.start_en.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_er_data.start_en.
 */
static inline uint32_t hal_spi_v151_spi_er_get_start_en(spi_bus_t bus)
{
    spi_er_data_t spi_er;
    spi_er.d32 = spis_v151_regs(bus)->spi_er;
    return spi_er.b.start_en;
}

/**
 * @brief  Set the value of @ref spi_er_data.start_en.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_er_data.start_en.
 */
static inline void hal_spi_ssienr_set_ssi_en(spi_bus_t bus, uint32_t val)
{
    spi_er_data_t spi_er;
    spi_er.d32 = spis_v151_regs(bus)->spi_er;
    spi_er.b.start_en = val;
    spis_v151_regs(bus)->spi_er = spi_er.d32;
}

/**
 * @brief  Get the value of @ref spi_mcr_data.mss.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_mcr_data.mss.
 */
static inline uint32_t hal_spi_v151_spi_mcr_get_mss(spi_bus_t bus)
{
    spi_mcr_data_t spi_mcr;
    spi_mcr.d32 = spis_v151_regs(bus)->spi_mcr;
    return spi_mcr.b.mss;
}

/**
 * @brief  Set the value of @ref spi_mcr_data.mss.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_mcr_data.mss.
 */
static inline void hal_spi_v151_spi_mcr_set_mss(spi_bus_t bus, uint32_t val)
{
    spi_mcr_data_t spi_mcr;
    spi_mcr.d32 = spis_v151_regs(bus)->spi_mcr;
    spi_mcr.b.mss = val;
    spis_v151_regs(bus)->spi_mcr = spi_mcr.d32;
}

/**
 * @brief  Get the value of @ref spi_mcr_data.mtrc.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_mcr_data.mtrc.
 */
static inline uint32_t hal_spi_v151_spi_mcr_get_mtrc(spi_bus_t bus)
{
    spi_mcr_data_t spi_mcr;
    spi_mcr.d32 = spis_v151_regs(bus)->spi_mcr;
    return spi_mcr.b.mtrc;
}

/**
 * @brief  Set the value of @ref spi_mcr_data.mtrc.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_mcr_data.mtrc.
 */
static inline void hal_spi_v151_spi_mcr_set_mtrc(spi_bus_t bus, uint32_t val)
{
    spi_mcr_data_t spi_mcr;
    spi_mcr.d32 = spis_v151_regs(bus)->spi_mcr;
    spi_mcr.b.mtrc = val;
    spis_v151_regs(bus)->spi_mcr = spi_mcr.d32;
}

/**
 * @brief  Get the value of @ref spi_mcr_data.mh.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_mcr_data.mh.
 */
static inline uint32_t hal_spi_v151_spi_mcr_get_mh(spi_bus_t bus)
{
    spi_mcr_data_t spi_mcr;
    spi_mcr.d32 = spis_v151_regs(bus)->spi_mcr;
    return spi_mcr.b.mh;
}

/**
 * @brief  Set the value of @ref spi_mcr_data.mh.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_mcr_data.mh.
 */
static inline void hal_spi_v151_spi_mcr_set_mh(spi_bus_t bus, uint32_t val)
{
    spi_mcr_data_t spi_mcr;
    spi_mcr.d32 = spis_v151_regs(bus)->spi_mcr;
    spi_mcr.b.mh = val;
    spis_v151_regs(bus)->spi_mcr = spi_mcr.d32;
}

/**
 * @brief  Get the value of @ref spi_slenr_data.ssef.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_slenr_data.ssef.
 */
static inline uint32_t hal_spi_v151_spi_slenr_get_ssef(spi_bus_t bus)
{
    spi_slenr_data_t spi_slenr;
    spi_slenr.d32 = spis_v151_regs(bus)->spi_slenr;
    return spi_slenr.b.ssef;
}

/**
 * @brief  Set the value of @ref spi_slenr_data.ssef.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_slenr_data.ssef.
 */
static inline void hal_spi_ser_set_ser(spi_bus_t bus, uint32_t val)
{
    spi_slenr_data_t spi_slenr;
    spi_slenr.d32 = spis_v151_regs(bus)->spi_slenr;
    spi_slenr.b.ssef = val;
    spis_v151_regs(bus)->spi_slenr = spi_slenr.d32;
}

/**
 * @brief  Get the value of @ref spi_brs_data.frdv.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_brs_data.frdv.
 */
static inline uint32_t hal_spi_v151_spi_brs_get_frdv(spi_bus_t bus)
{
    spi_brs_data_t spi_brs;
    spi_brs.d32 = spis_v151_regs(bus)->spi_brs;
    return spi_brs.b.frdv;
}

/**
 * @brief  Set the value of @ref spi_brs_data.frdv.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_brs_data.frdv.
 */
static inline void hal_spi_baudr_set_sckdv(spi_bus_t bus, uint32_t val)
{
    spi_brs_data_t spi_brs;
    spi_brs.d32 = spis_v151_regs(bus)->spi_brs;
    spi_brs.b.frdv = val;
    spis_v151_regs(bus)->spi_brs = spi_brs.d32;
}

/**
 * @brief  Get the value of @ref spi_twlr_data.twl.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_twlr_data.twl.
 */
static inline uint32_t hal_spi_v151_spi_twlr_get_twl(spi_bus_t bus)
{
    spi_twlr_data_t spi_twlr;
    spi_twlr.d32 = spis_v151_regs(bus)->spi_twlr;
    return spi_twlr.b.twl;
}

/**
 * @brief  Set the value of @ref spi_twlr_data.twl.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_twlr_data.twl.
 */
static inline void hal_spi_txftlr_set_tft(spi_bus_t bus, uint32_t val)
{
    spi_twlr_data_t spi_twlr;
    spi_twlr.d32 = spis_v151_regs(bus)->spi_twlr;
    spi_twlr.b.twl = val;
    spis_v151_regs(bus)->spi_twlr = spi_twlr.d32;
}

/**
 * @brief  Get the value of @ref spi_rwlr_data.rwl.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_rwlr_data.rwl.
 */
static inline uint32_t hal_spi_v151_spi_rwlr_get_rwl(spi_bus_t bus)
{
    spi_rwlr_data_t spi_rwlr;
    spi_rwlr.d32 = spis_v151_regs(bus)->spi_rwlr;
    return spi_rwlr.b.rwl;
}

/**
 * @brief  Set the value of @ref spi_rwlr_data.rwl.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_rwlr_data.rwl.
 */
static inline void hal_spi_rxftlr_set_rft(spi_bus_t bus, uint32_t val)
{
    spi_rwlr_data_t spi_rwlr;
    spi_rwlr.d32 = spis_v151_regs(bus)->spi_rwlr;
    spi_rwlr.b.rwl = val;
    spis_v151_regs(bus)->spi_rwlr = spi_rwlr.d32;
}

/**
 * @brief  Get the value of @ref spi_tlr_data.tfl.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_tlr_data.tfl.
 */
static inline uint32_t hal_spi_txflr_get_txtfl(spi_bus_t bus)
{
    spi_tlr_data_t spi_tlr;
    spi_tlr.d32 = spis_v151_regs(bus)->spi_tlr;
    return spi_tlr.b.tfl;
}

/**
 * @brief  Get the value of @ref spi_rlr_data.rfl.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_rlr_data.rfl.
 */
static inline uint32_t hal_spi_rxflr_get_rxtfl(spi_bus_t bus)
{
    spi_rlr_data_t spi_rlr;
    spi_rlr.d32 = spis_v151_regs(bus)->spi_rlr;
    return spi_rlr.b.rfl;
}

/**
 * @brief  Get the value of @ref spi_wsr_data.sbf.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_wsr_data.sbf.
 */
static inline uint32_t hal_spi_sr_get_busy(spi_bus_t bus)
{
    spi_wsr_data_t spi_wsr;
    spi_wsr.d32 = spis_v151_regs(bus)->spi_wsr;
    return spi_wsr.b.sbf;
}

/**
 * @brief  Get the value of @ref spi_wsr_data.tfnf.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_wsr_data.tfnf.
 */
static inline uint32_t hal_spi_v151_spi_wsr_get_tfnf(spi_bus_t bus)
{
    spi_wsr_data_t spi_wsr;
    spi_wsr.d32 = spis_v151_regs(bus)->spi_wsr;
    return spi_wsr.b.tfnf;
}

/**
 * @brief  Get the value of @ref spi_wsr_data.tfe.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_wsr_data.tfe.
 */
static inline uint32_t hal_spi_v151_spi_wsr_get_tfe(spi_bus_t bus)
{
    spi_wsr_data_t spi_wsr;
    spi_wsr.d32 = spis_v151_regs(bus)->spi_wsr;
    return spi_wsr.b.tfe;
}

/**
 * @brief  Get the value of @ref spi_wsr_data.rfne.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_wsr_data.rfne.
 */
static inline uint32_t hal_spi_v151_spi_wsr_get_rfne(spi_bus_t bus)
{
    spi_wsr_data_t spi_wsr;
    spi_wsr.d32 = spis_v151_regs(bus)->spi_wsr;
    return spi_wsr.b.rfne;
}

/**
 * @brief  Get the value of @ref spi_wsr_data.rffe.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_wsr_data.rffe.
 */
static inline uint32_t hal_spi_v151_spi_wsr_get_rffe(spi_bus_t bus)
{
    spi_wsr_data_t spi_wsr;
    spi_wsr.d32 = spis_v151_regs(bus)->spi_wsr;
    return spi_wsr.b.rffe;
}

/**
 * @brief  Get the value of @ref spi_wsr_data.dcfe_tfee.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_wsr_data.dcfe_tfee.
 */
static inline uint32_t hal_spi_v151_spi_wsr_get_dcfe_tfee(spi_bus_t bus)
{
    spi_wsr_data_t spi_wsr;
    spi_wsr.d32 = spis_v151_regs(bus)->spi_wsr;
    return spi_wsr.b.dcfe_tfee;
}

/**
 * @brief  Get the register address of ssi interrupt set interface.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to get.
 * @return The register address of ssi interrupt set interface.
 */
volatile uint32_t *hal_spi_v151_int_set_reg(spi_bus_t bus, spi_v151_int_reg_t reg);

/**
 * @brief  Get the register address of ssi interrupt get interface.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to get.
 * @return The register address of ssi interrupt get interface.
 */
volatile uint32_t *hal_spi_v151_int_get_reg(spi_bus_t bus, spi_v151_int_reg_t reg);

/**
 * @brief  Set the value of @ref spi_v151_interrupt_data
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref spi_v151_interrupt_data.
 */
static inline void hal_spi_v151_int_set(spi_bus_t bus, spi_v151_int_reg_t reg, uint32_t val)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_set_reg(bus, reg);

    data.d32 = val;
    if (reg_addr) {
        *reg_addr = data.d32;
    }
}

/**
 * @brief  Set the value of @ref spi_v151_interrupt_data.tfeis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref spi_v151_interrupt_data.tfeis.
 */
static inline void hal_spi_v151_int_set_tfeis(spi_bus_t bus, spi_v151_int_reg_t reg, uint32_t val)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.tfeis = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref spi_v151_interrupt_data.tfeis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref spi_v151_interrupt_data.tfeis.
 */
static inline uint32_t hal_spi_v151_int_get_tfeis(spi_bus_t bus, spi_v151_int_reg_t reg)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.tfeis;
}

/**
 * @brief  Set the value of @ref spi_v151_interrupt_data.tfofis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref spi_v151_interrupt_data.tfofis.
 */
static inline void hal_spi_v151_int_set_tfofis(spi_bus_t bus, spi_v151_int_reg_t reg, uint32_t val)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.tfofis = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref spi_v151_interrupt_data.tfofis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref spi_v151_interrupt_data.tfofis.
 */
static inline uint32_t hal_spi_v151_int_get_tfofis(spi_bus_t bus, spi_v151_int_reg_t reg)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.tfofis;
}

/**
 * @brief  Set the value of @ref spi_v151_interrupt_data.rfufis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref spi_v151_interrupt_data.rfufis.
 */
static inline void hal_spi_v151_int_set_rfufis(spi_bus_t bus, spi_v151_int_reg_t reg, uint32_t val)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.rfufis = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref spi_v151_interrupt_data.rfufis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref spi_v151_interrupt_data.rfufis.
 */
static inline uint32_t hal_spi_v151_int_get_rfufis(spi_bus_t bus, spi_v151_int_reg_t reg)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.rfufis;
}

/**
 * @brief  Set the value of @ref spi_v151_interrupt_data.rfofis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref spi_v151_interrupt_data.rfofis.
 */
static inline void hal_spi_v151_int_set_rfofis(spi_bus_t bus, spi_v151_int_reg_t reg, uint32_t val)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.rfofis = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref spi_v151_interrupt_data.rfofis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref spi_v151_interrupt_data.rfofis.
 */
static inline uint32_t hal_spi_v151_int_get_rfofis(spi_bus_t bus, spi_v151_int_reg_t reg)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.rfofis;
}

/**
 * @brief  Set the value of @ref spi_v151_interrupt_data.rffis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref spi_v151_interrupt_data.rffis.
 */
static inline void hal_spi_v151_int_set_rffis(spi_bus_t bus, spi_v151_int_reg_t reg, uint32_t val)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.rffis = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref spi_v151_interrupt_data.rffis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref spi_v151_interrupt_data.rffis.
 */
static inline uint32_t hal_spi_v151_int_get_rffis(spi_bus_t bus, spi_v151_int_reg_t reg)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.rffis;
}

/**
 * @brief  Set the value of @ref spi_v151_interrupt_data.mmcris.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref spi_v151_interrupt_data.mmcris.
 */
static inline void hal_spi_v151_int_set_mmcris(spi_bus_t bus, spi_v151_int_reg_t reg, uint32_t val)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.mmcris = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref spi_v151_interrupt_data.mmcris.
 * @param  [in]  bus The index of spi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref spi_v151_interrupt_data.mmcris.
 */
static inline uint32_t hal_spi_v151_int_get_mmcris(spi_bus_t bus, spi_v151_int_reg_t reg)
{
    union spi_v151_interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v151_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.mmcris;
}

/**
 * @brief  Get the value of spi_tfoficr @ref spi_tfoficr_data_t.
 * @param  [in]  bus The index of spi.
 * @return The value of spi_tfoficr @ref spi_tfoficr_data_t.
 */
static inline uint32_t hal_spi_v151_spi_tfoficr_get_spi_tfoficr(spi_bus_t bus)
{
    spi_tfoficr_data_t spi_tfoficr;
    spi_tfoficr.d32 = spis_v151_regs(bus)->spi_tfoficr;
    return spi_tfoficr.b.cis;
}

/**
 * @brief  Get the value of spi_rxoicr @ref spi_rxoicr_data_t.
 * @param  [in]  bus The index of spi.
 * @return The value of spi_rxoicr @ref spi_rxoicr_data_t.
 */
static inline uint32_t hal_spi_v151_spi_rxoicr_get_spi_rxoicr(spi_bus_t bus)
{
    spi_rxoicr_data_t spi_rxoicr;
    spi_rxoicr.d32 = spis_v151_regs(bus)->spi_rxoicr;
    return spi_rxoicr.b.cis;
}

/**
 * @brief  Get the value of spi_rfficr @ref spi_rfficr_data_t.
 * @param  [in]  bus The index of spi.
 * @return The value of spi_rfficr @ref spi_rfficr_data_t.
 */
static inline uint32_t hal_spi_v151_spi_rfficr_get_spi_rfficr(spi_bus_t bus)
{
    spi_rfficr_data_t spi_rfficr;
    spi_rfficr.d32 = spis_v151_regs(bus)->spi_rfficr;
    return spi_rfficr.b.cis;
}

/**
 * @brief  Get the value of spi_msticr @ref spi_msticr_data_t.
 * @param  [in]  bus The index of spi.
 * @return The value of spi_msticr @ref spi_msticr_data_t.
 */
static inline uint32_t hal_spi_v151_spi_msticr_get_spi_msticr(spi_bus_t bus)
{
    spi_msticr_data_t spi_msticr;
    spi_msticr.d32 = spis_v151_regs(bus)->spi_msticr;
    return spi_msticr.b.cis;
}

/**
 * @brief  Get the value of @ref spi_icr_data.cis.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_icr_data.cis.
 */
static inline uint32_t hal_spi_v151_spi_icr_get_cis(spi_bus_t bus)
{
    spi_icr_data_t spi_icr;
    spi_icr.d32 = spis_v151_regs(bus)->spi_icr;
    return spi_icr.b.cis;
}

/**
 * @brief  Get the value of @ref spi_dcr_data.rden.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_dcr_data.rden.
 */
static inline uint32_t hal_spi_v151_spi_dcr_get_rden(spi_bus_t bus)
{
    spi_dcr_data_t spi_dcr;
    spi_dcr.d32 = spis_v151_regs(bus)->spi_dcr;
    return spi_dcr.b.rden;
}

/**
 * @brief  Set the value of @ref spi_dcr_data.rden.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_dcr_data.rden.
 */
static inline void hal_spi_dmacr_set_rdmae(spi_bus_t bus, uint32_t val)
{
    spi_dcr_data_t spi_dcr;
    spi_dcr.d32 = spis_v151_regs(bus)->spi_dcr;
    spi_dcr.b.rden = val;
    spis_v151_regs(bus)->spi_dcr = spi_dcr.d32;
}

/**
 * @brief  Get the value of @ref spi_dcr_data.tden.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_dcr_data.tden.
 */
static inline uint32_t hal_spi_v151_spi_dcr_get_tden(spi_bus_t bus)
{
    spi_dcr_data_t spi_dcr;
    spi_dcr.d32 = spis_v151_regs(bus)->spi_dcr;
    return spi_dcr.b.tden;
}

/**
 * @brief  Set the value of @ref spi_dcr_data.tden.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_dcr_data.tden.
 */
static inline void hal_spi_v151_spi_dcr_set_tden(spi_bus_t bus, uint32_t val)
{
    spi_dcr_data_t spi_dcr;
    spi_dcr.d32 = spis_v151_regs(bus)->spi_dcr;
    spi_dcr.b.tden = val;
    spis_v151_regs(bus)->spi_dcr = spi_dcr.d32;
}

/**
 * @brief  Get the value of dmatdl @ref spi_drdl_data_t.
 * @param  [in]  bus The index of spi.
 * @return The value of dmatdl @ref spi_drdl_data_t.
 */
static inline uint32_t hal_spi_v151_spi_drdl_data_get_dl(spi_bus_t bus)
{
    spi_drdl_data_t spi_drdl;
    spi_drdl.d32 = spis_v151_regs(bus)->spi_drdl;
    return spi_drdl.b.dl;
}

/**
 * @brief  Set the value of dmatdl @ref spi_drdl_data_t.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of dmatdl @ref spi_drdl_data_t.
 */
static inline void hal_spi_dmardlr_set_dmardl(spi_bus_t bus, uint32_t val)
{
    spi_drdl_data_t spi_drdl;
    spi_drdl.d32 = spis_v151_regs(bus)->spi_drdl;
    spi_drdl.b.dl = val;
    spis_v151_regs(bus)->spi_drdl = spi_drdl.d32;
}

/**
 * @brief  Get the value of dmardl @ref spi_dtdl_data_t.
 * @param  [in]  bus The index of spi.
 * @return The value of dmardl @ref spi_dtdl_data_t.
 */
static inline uint32_t hal_spi_v151_spi_dtdl_data_get_dl(spi_bus_t bus)
{
    spi_dtdl_data_t spi_dtdl;
    spi_dtdl.d32 = spis_v151_regs(bus)->spi_dtdl;
    return spi_dtdl.b.dl;
}

/**
 * @brief  Set the value of dmardl @ref spi_dtdl_data_t.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of dmardl @ref spi_dtdl_data_t.
 */
static inline void hal_spi_v151_spi_dtdl_data_set_dl(spi_bus_t bus, uint32_t val)
{
    spi_dtdl_data_t spi_dtdl;
    spi_dtdl.d32 = spis_v151_regs(bus)->spi_dtdl;
    spi_dtdl.b.dl = val;
    spis_v151_regs(bus)->spi_dtdl = spi_dtdl.d32;
}

/**
 * @brief  Get the value of @ref spi_id_data.id.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_id_data.id.
 */
static inline uint32_t hal_spi_v151_spi_id_get_id(spi_bus_t bus)
{
    spi_id_data_t spi_id;
    spi_id.d32 = spis_v151_regs(bus)->spi_id;
    return spi_id.b.id;
}

/**
 * @brief  Get data from data register.
 * @param  [in]  bus The index of spi.
 * @return The data from data register.
 */
static inline uint32_t hal_spi_dr_get_dr(spi_bus_t bus)
{
    return spis_v151_regs(bus)->spi_drnm[0];
}

/**
 * @brief  Set data into data register.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The data need to set into data register.
 */
static inline void hal_spi_dr_set_dr(spi_bus_t bus, uint32_t val)
{
    spis_v151_regs(bus)->spi_drnm[0] = val;
}

/**
 * @brief  Get the value of @ref spi_rsdr_data.rxdsd.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_rsdr_data.rxdsd.
 */
static inline uint32_t hal_spi_v151_spi_rsdr_data_get_rxdsd(spi_bus_t bus)
{
    spi_rsdr_data_t spi_rsdr;
    spi_rsdr.d32 = spis_v151_regs(bus)->spi_rsdr;
    return spi_rsdr.b.rxdsd;
}

/**
 * @brief  Set the value of @ref spi_rsdr_data.rxdsd.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_rsdr_data.rxdsd.
 */
static inline void hal_spi_rx_sample_dly_set_rsd(spi_bus_t bus, uint32_t val)
{
    spi_rsdr_data_t spi_rsdr;
    spi_rsdr.d32 = spis_v151_regs(bus)->spi_rsdr;
    spi_rsdr.b.rxdsd = val;
    spis_v151_regs(bus)->spi_rsdr = spi_rsdr.d32;
}

/**
 * @brief  Get the value of @ref spi_enhctl_data.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_enhctl_data.
 */
static inline uint32_t hal_spi_v151_spi_enhctl_get(spi_bus_t bus)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    return spi_enhctl.d32;
}

/**
 * @brief  Set the value of @ref spi_enhctl_data.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_enhctl_data.
 */
static inline void hal_spi_v151_spi_enhctl_set(spi_bus_t bus, uint32_t val)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = val;
    spis_v151_regs(bus)->spi_enhctl = spi_enhctl.d32;
}

/**
 * @brief  Get the value of @ref spi_enhctl_data.aaitf.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_enhctl_data.aaitf.
 */
static inline uint32_t hal_spi_v151_spi_enhctl_get_aaitf(spi_bus_t bus)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    return spi_enhctl.b.aaitf;
}

/**
 * @brief  Set the value of @ref spi_enhctl_data.aaitf.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_enhctl_data.aaitf.
 */
static inline void hal_spi_v151_spi_enhctl_set_aaitf(spi_bus_t bus, uint32_t val)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    spi_enhctl.b.aaitf = val;
    spis_v151_regs(bus)->spi_enhctl = spi_enhctl.d32;
}

/**
 * @brief  Get the value of @ref spi_enhctl_data.addrlen.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_enhctl_data.addrlen.
 */
static inline uint32_t hal_spi_v151_spi_enhctl_get_addrlen(spi_bus_t bus)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    return spi_enhctl.b.addrlen;
}

/**
 * @brief  Set the value of @ref spi_enhctl_data.addrlen.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_enhctl_data.addrlen.
 */
static inline void hal_spi_v151_spi_enhctl_set_addrlen(spi_bus_t bus, uint32_t val)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    spi_enhctl.b.addrlen = val;
    spis_v151_regs(bus)->spi_enhctl = spi_enhctl.d32;
}

/**
 * @brief  Get the value of @ref spi_enhctl_data.ilen.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_enhctl_data.ilen.
 */
static inline uint32_t hal_spi_v151_spi_enhctl_get_ilen(spi_bus_t bus)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    return spi_enhctl.b.ilen;
}

/**
 * @brief  Set the value of @ref spi_enhctl_data.ilen.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_enhctl_data.ilen.
 */
static inline void hal_spi_v151_spi_enhctl_set_ilen(spi_bus_t bus, uint32_t val)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    spi_enhctl.b.ilen = val;
    spis_v151_regs(bus)->spi_enhctl = spi_enhctl.d32;
}

/**
 * @brief  Get the value of @ref spi_enhctl_data.waitnum.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_enhctl_data.waitnum.
 */
static inline uint32_t hal_spi_v151_spi_enhctl_get_waitnum(spi_bus_t bus)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    return spi_enhctl.b.waitnum;
}

/**
 * @brief  Set the value of @ref spi_enhctl_data.waitnum.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_enhctl_data.waitnum.
 */
static inline void hal_spi_v151_spi_enhctl_set_waitnum(spi_bus_t bus, uint32_t val)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    spi_enhctl.b.waitnum = val;
    spis_v151_regs(bus)->spi_enhctl = spi_enhctl.d32;
}

/**
 * @brief  Get the value of @ref spi_enhctl_data.iddren.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_enhctl_data.iddren.
 */
static inline uint32_t hal_spi_v151_spi_enhctl_get_iddren(spi_bus_t bus)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    return spi_enhctl.b.iddren;
}

/**
 * @brief  Set the value of @ref spi_enhctl_data.iddren.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_enhctl_data.iddren.
 */
static inline void hal_spi_v151_spi_enhctl_set_iddren(spi_bus_t bus, uint32_t val)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    spi_enhctl.b.iddren = val;
    spis_v151_regs(bus)->spi_enhctl = spi_enhctl.d32;
}

/**
 * @brief  Get the value of @ref spi_enhctl_data.rdsen.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_enhctl_data.rdsen.
 */
static inline uint32_t hal_spi_v151_spi_enhctl_get_rdsen(spi_bus_t bus)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    return spi_enhctl.b.rdsen;
}

/**
 * @brief  Set the value of @ref spi_enhctl_data.rdsen.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_enhctl_data.rdsen.
 */
static inline void hal_spi_v151_spi_enhctl_set_rdsen(spi_bus_t bus, uint32_t val)
{
    spi_enhctl_data_t spi_enhctl;
    spi_enhctl.d32 = spis_v151_regs(bus)->spi_enhctl;
    spi_enhctl.b.rdsen = val;
    spis_v151_regs(bus)->spi_enhctl = spi_enhctl.d32;
}

/**
 * @brief  Get the value of @ref spi_tder_data.txdde.
 * @param  [in]  bus The index of spi.
 * @return The value of @ref spi_tder_data.txdde.
 */
static inline uint32_t hal_spi_v151_spi_tder_data_get_txdde(spi_bus_t bus)
{
    spi_tder_data_t spi_tder_data;
    spi_tder_data.d32 = spis_v151_regs(bus)->spi_tder;
    return spi_tder_data.b.txdde;
}

/**
 * @brief  Set the value of @ref spi_tder_data.txdde.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_tder_data.txdde.
 */
static inline void hal_spi_v151_spi_tder_data_set_txdde(spi_bus_t bus, uint32_t val)
{
    spi_tder_data_t spi_tder_data;
    spi_tder_data.d32 = spis_v151_regs(bus)->spi_tder;
    spi_tder_data.b.txdde = val;
    spis_v151_regs(bus)->spi_tder = spi_tder_data.d32;
}

/**
 * @brief  Set the value of @ref spi_inmar_data_t.rffis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_inmar_data_t.rffis.
 */
static inline void hal_spi_v151_spi_inmar_set_rffis(spi_bus_t bus, uint32_t val)
{
    spi_inmar_data_t spi_inmar_data;
    spi_inmar_data.d32 = spis_v151_regs(bus)->spi_inmar;
    spi_inmar_data.b.rffis = val;
    spis_v151_regs(bus)->spi_inmar = spi_inmar_data.d32;
}

/**
 * @brief  Set the value of @ref spi_inmar_data_t.tfeis.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_inmar_data_t.tfeis.
 */
static inline void hal_spi_v151_spi_inmar_set_tfeis(spi_bus_t bus, uint32_t val)
{
    spi_inmar_data_t spi_inmar_data;
    spi_inmar_data.d32 = spis_v151_regs(bus)->spi_inmar;
    spi_inmar_data.b.tfeis = val;
    spis_v151_regs(bus)->spi_inmar = spi_inmar_data.d32;
}

/**
 * @brief  Set the value of @ref spi_inmar_data_t.mmcris.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of @ref spi_inmar_data_t.mmcris.
 */
static inline void hal_spi_v151_spi_inmar_set_mmcris(spi_bus_t bus, uint32_t val)
{
    spi_inmar_data_t spi_inmar_data;
    spi_inmar_data.d32 = spis_v151_regs(bus)->spi_inmar;
    spi_inmar_data.b.mmcris = val;
    spis_v151_regs(bus)->spi_inmar = spi_inmar_data.d32;
}

/**
 * @brief  Set the value of @ref spi_icr_data_t.cis.
 * @param  [in]  bus The index of spi.
 */
static inline void hal_spi_v151_icr_set_any(spi_bus_t bus)
{
    spi_icr_data_t icr_data;
    icr_data.d32 = spis_v151_regs(bus)->spi_icr;
    icr_data.b.cis = 1;
    spis_v151_regs(bus)->spi_icr = icr_data.d32;
}

/**
 * @brief  Get the value of spi_rsvd.
 * @param  [in]  bus The index of spi.
 * @return The value of spi_rsvd.
 */
static inline uint32_t hal_spi_v151_spi_rsvd_get(spi_bus_t bus)
{
    return spis_v151_regs(bus)->spi_rsvd;
}

/**
 * @brief  Set the value of spi_rsvd.
 * @param  [in]  bus The index of spi.
 * @param  [in]  val The value of spi_rsvd.
 */
static inline void hal_spi_v151_spi_rsvd_set(spi_bus_t bus, uint32_t val)
{
    spis_v151_regs(bus)->spi_rsvd = val;
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