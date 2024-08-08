/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides v150 SFC register operation api \n
 *
 * History: \n
 * 2022-12-01， Create file. \n
 */
#ifndef HAL_SFC_V150_REGS_OP_H
#define HAL_SFC_V150_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "hal_sfc_v150_regs_def.h"
#include "sfc_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sfc_v150_regs_op SFC v150 Regs Operation
 * @ingroup  drivers_hal_sfc
 * @{
 */

extern uintptr_t g_sfc_global_conf_regs;
extern uintptr_t g_sfc_bus_regs;
extern uintptr_t g_sfc_bus_dma_regs;
extern uintptr_t g_sfc_cmd_regs;
extern uintptr_t g_sfc_cmd_databuf;

/**
 * @brief  SPI mode opreation base info.
 */
typedef struct hal_spi_opreation {
    spi_opreation_t opt;            /*!< SPI opreation info */
    uint32_t data_size;             /*!< SPI opreation data size */
    uint32_t dummy_byte;            /*!< SPI opreation dummy byte */
} hal_spi_opreation_t;

/**
 * @brief  Set cmd config and cmd ins by basic command info
 * @param  [in] hal_opt  SPI mode opreation parameters.
 */
void hal_sfc_regs_set_opt(hal_spi_opreation_t hal_opt);

/**
 * @brief  Set cmd config by command attribute
 * @param  [in] rw  region rw of @ref cmd_config_t
 * @param  [in] data_en  region data_en of @ref cmd_config_t
 * @param  [in] addr_en  region addr_en of @ref cmd_config_t
 */
void hal_sfc_regs_set_opt_attr(uint32_t rw, uint32_t data_en, uint32_t addr_en);

/**
 * @brief  Wait region start of @ref cmd_config_t be 0
 */
void hal_sfc_regs_wait_config(void);

/**
 * @brief  Wait region dma_start of @ref bus_dma_ctrl_t be 0
 */
void hal_sfc_dma_wait_done(void);

/**
 * @brief  Set read command info in bus mode
 * @param  [in] opt_read  read opreation info.
 */
void hal_sfc_regs_set_bus_read(spi_opreation_t opt_read);

/**
 * @brief  Set write command info in bus mode
 * @param  [in] opt_write  write opreation info.
 */
void hal_sfc_regs_set_bus_write(spi_opreation_t opt_write);

/**
 * @brief  Set timing
 */
static inline void hal_sfc_regs_set_timing(void)
{
    ((global_conf_regs_t *)g_sfc_global_conf_regs)->timing = 0x1;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_flash_saddr
 * @param  [in]  flash_saddr The value of @ref bus_dma_regs_t.bus_dma_flash_saddr
 */
static inline void hal_sfc_regs_set_bus_dma_flash_saddr(uint32_t flash_saddr)
{
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs)->bus_dma_flash_saddr = flash_saddr;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_mem_saddr
 * @param  [in]  dma_buffer data buffer to read or write
 */
static inline void hal_sfc_regs_set_bus_dma_mem_addr(uint8_t *dma_buffer)
{
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs)->bus_dma_mem_saddr = (uintptr_t)dma_buffer;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_len
 * @param  [in]  length The value of @ref bus_dma_regs_t.bus_dma_len
 */
static inline void hal_sfc_regs_set_bus_dma_len(uint32_t length)
{
    bus_dma_len_t len_conf;
    len_conf.d32 = ((bus_dma_regs_t *)g_sfc_bus_dma_regs)->bus_dma_len;
    len_conf.b.dma_len = length - 1;
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs)->bus_dma_len = len_conf.d32;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_ahb_ctrl
 */
static inline void hal_sfc_regs_set_bus_dma_ahb_ctrl(void)
{
    bus_dma_ahb_ctrl_t ahb_ctrl;
    ahb_ctrl.d32 = 0x7;
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs)->bus_dma_ahb_ctrl = ahb_ctrl.d32;
}

/**
 * @brief  Set the value of @ref bus_dma_regs_t.bus_dma_ctrl
 * @param  [in]  rw The value of @ref bus_dma_ctrl_t.dma_rw
 */
static inline void hal_sfc_regs_set_bus_dma_ctrl(uint32_t rw)
{
    bus_dma_ctrl_t dma_ctrl;
    dma_ctrl.d32 = ((bus_dma_regs_t *)g_sfc_bus_dma_regs)->bus_dma_ctrl;
    dma_ctrl.b.dma_sel_cs = 0x1;
    dma_ctrl.b.dma_rw = rw;
    dma_ctrl.b.dma_start = 0x1;
    ((bus_dma_regs_t *)g_sfc_bus_dma_regs)->bus_dma_ctrl = dma_ctrl.d32;
}

/**
 * @brief  Set the value of @ref cmd_databufs_t
 * @param  [in]  index The index of @ref cmd_databufs_t.cmd_databuf
 * @param  [in]  val The value of @ref cmd_databufs_t.cmd_databuf[index]
 */
static inline void hal_sfc_regs_set_databuf(uint32_t index, uint32_t val)
{
    ((cmd_databufs_t *)g_sfc_cmd_databuf)->cmd_databuf[index] = val;
}

/**
 * @brief  Get the value of @ref cmd_databufs_t
 * @param  [in]  index The index of @ref cmd_databufs_t.cmd_databuf
 */
static inline uint32_t hal_sfc_regs_get_databuf(uint32_t index)
{
    return ((cmd_databufs_t *)g_sfc_cmd_databuf)->cmd_databuf[index];
}

/**
 * @brief  Set the value of @ref bus_regs_t.bus_base_addr_cs1
 * @param  [in]  val The value of @ref bus_regs_t.bus_base_addr_cs1
 */
static inline void hal_sfc_regs_set_bus_baseaddr(uint32_t val)
{
    bus_base_addr_t base_addr;
    base_addr.d32 = ((bus_regs_t *)g_sfc_bus_regs)->bus_base_addr_cs1;
    base_addr.b.bus_base_addr_high_cs = val;
    ((bus_regs_t *)g_sfc_bus_regs)->bus_base_addr_cs1 = base_addr.d32;
}

/**
 * @brief  Set the value of @ref bus_regs_t.bus_flash_size
 * @param  [in]  val The value of @ref bus_flash_size_t.flash_size_cs1
 */
static inline void hal_sfc_regs_set_bus_flash_size(uint32_t val)
{
    bus_flash_size_t flash_size;
    flash_size.d32 = ((bus_regs_t *)g_sfc_bus_regs)->bus_flash_size;
    flash_size.b.flash_size_cs1 = val;
    ((bus_regs_t *)g_sfc_bus_regs)->bus_flash_size = flash_size.d32;
}

/**
 * @brief  Set the value of @ref cmd_regs_t.cmd_addr
 * @param  [in]  val The value of @ref cmd_addr_t.cmd_addr
 */
static inline void hal_sfc_regs_set_cmd_addr(uint32_t val)
{
    cmd_addr_t addr;
    addr.d32 = ((cmd_regs_t *)g_sfc_cmd_regs)->cmd_addr;
    addr.b.cmd_addr = val;
    ((cmd_regs_t *)g_sfc_cmd_regs)->cmd_addr = addr.d32;
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