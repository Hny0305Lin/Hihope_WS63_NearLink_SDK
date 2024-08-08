/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL SFC \n
 *
 * History: \n
 * 2022-12-01, Create file. \n
 */
#include "common_def.h"
#include "hal_sfc.h"

uintptr_t g_sfc_global_conf_regs = NULL;
uintptr_t g_sfc_bus_regs = NULL;
uintptr_t g_sfc_bus_dma_regs = NULL;
uintptr_t g_sfc_cmd_regs = NULL;
uintptr_t g_sfc_cmd_databuf = NULL;

errcode_t hal_sfc_regs_init(void)
{
    g_sfc_global_conf_regs = sfc_port_get_sfc_global_conf_base_addr();
    g_sfc_bus_regs = sfc_port_get_sfc_bus_regs_base_addr();
    g_sfc_bus_dma_regs = sfc_port_get_sfc_bus_dma_regs_base_addr();
    g_sfc_cmd_regs = sfc_port_get_sfc_cmd_regs_base_addr();
    g_sfc_cmd_databuf = sfc_port_get_sfc_cmd_databuf_base_addr();
    if ((g_sfc_global_conf_regs == NULL) || (g_sfc_bus_regs == NULL) ||
        (g_sfc_bus_dma_regs == NULL) || (g_sfc_cmd_regs == NULL) ||
        (g_sfc_cmd_databuf == NULL)) {
        return ERRCODE_SFC_REG_ADDR_INVALID;
    }

    return ERRCODE_SUCC;
}

void hal_sfc_regs_deinit(void)
{
    g_sfc_global_conf_regs = NULL;
    g_sfc_bus_regs = NULL;
    g_sfc_bus_dma_regs = NULL;
    g_sfc_cmd_regs = NULL;
    g_sfc_cmd_databuf = NULL;
}