/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides SFC register operation api \n
 *
 * History: \n
 * 2022-12-01, Create file. \n
 */
#include "common_def.h"
#include "tcxo.h"
#include "hal_sfc_v150_regs_op.h"

#define CONFIG_START           1
#define FLASH_CS               1
#define RD_ENABLE              1
#define WR_ENABLE              1

static uint32_t g_regs_wait_config_count = 0;
static uint32_t g_dma_wait_config_count = 0;

void hal_sfc_regs_set_opt(hal_spi_opreation_t *hal_opt)
{
    cmd_ins_t ins;
    cmd_config_t config;
    ins.d32 = ((cmd_regs_t *)g_sfc_cmd_regs)->cmd_ins;
    config.d32 = 0;
    ins.b.reg_ins = hal_opt->opt.cmd;
    config.b.mem_if_type = hal_opt->opt.iftype;
    config.b.dummy_byte_cnt = hal_opt->dummy_byte;
    config.b.data_cnt = hal_opt->data_size - 1;
    ((cmd_regs_t *)g_sfc_cmd_regs)->cmd_ins = ins.d32;
    ((cmd_regs_t *)g_sfc_cmd_regs)->cmd_config = config.d32;
}

void hal_sfc_regs_set_opt_attr(uint32_t rw, uint32_t data_en, uint32_t addr_en)
{
    cmd_config_t config;
    config.d32 = ((cmd_regs_t *)g_sfc_cmd_regs)->cmd_config;
    config.b.rw = rw;
    config.b.data_en = data_en;
    config.b.addr_en = addr_en;
    config.b.sel_cs = FLASH_CS;
    config.b.start = CONFIG_START;
    ((cmd_regs_t *)g_sfc_cmd_regs)->cmd_config = config.d32;
}

void hal_sfc_regs_wait_config(void)
{
    /* 全局变量记录轮询次数，下同。 */
    while ((((cmd_regs_t *)g_sfc_cmd_regs)->cmd_config & 0x01) != 0) {
#if defined(CONFIG_SFC_DEBUG)
        g_regs_wait_config_count++;
#endif
    }
    g_regs_wait_config_count = 0;
}

void hal_sfc_dma_wait_done(void)
{
    while ((((bus_dma_regs_t *)g_sfc_bus_dma_regs)->bus_dma_ctrl & 0x01) != 0) {
#if defined(CONFIG_SFC_DEBUG)
        g_dma_wait_config_count++;
#endif
    }
    g_dma_wait_config_count = 0;
}

void hal_sfc_regs_set_bus_read(spi_opreation_t opt_read)
{
    bus_config1_t bus_config;
    bus_config.d32 = ((bus_regs_t *)g_sfc_bus_regs)->bus_config1;
    bus_config.b.rd_enable = RD_ENABLE;
    bus_config.b.rd_ins = opt_read.cmd;
    bus_config.b.rd_mem_if_type = opt_read.iftype;
    bus_config.b.rd_dummy_bytes = opt_read.size;
    ((bus_regs_t *)g_sfc_bus_regs)->bus_config1 = bus_config.d32;
}

void hal_sfc_regs_set_bus_write(spi_opreation_t opt_write)
{
    bus_config1_t bus_config;
    bus_config.d32 = ((bus_regs_t *)g_sfc_bus_regs)->bus_config1;
    bus_config.b.wr_ins = opt_write.cmd;
    bus_config.b.wr_mem_if_type = opt_write.iftype;
    bus_config.b.wr_enable = WR_ENABLE;
    ((bus_regs_t *)g_sfc_bus_regs)->bus_config1 = bus_config.d32;
}