/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: last dump
 * This file should be changed only infrequently and with great care.
 */
#include "diag.h"
#include "last_dump.h"
#include "last_dump_st.h"
#include "memory_config.h"
#include "last_dump_adapt.h"

typedef struct {
    char *name;
    uintptr_t start_addr;
    uintptr_t end_addr;
} diag_reg_dump_t;

typedef struct {
    char *name;
    uintptr_t start_addr;
    uint32_t len;
} diag_mem_dump_t;


#ifdef DUMP_REG_SUPPORT
static diag_reg_dump_t g_reg_dump_info[] = {
    { "PMU1_CTL", 0x57004000, 0x570047e0 },      // PMU1_CTL
    { "PMU2_CTL", 0x57008000, 0x57008490 },      // PMU2_CTL
    { "ULP_AON_CTL", 0x5702c000, 0x5702c418 },   // ULP_AON_CTL
};

static void dfx_dump_reg(void)
{
    uint16_t count = sizeof(g_reg_dump_info) / sizeof(diag_reg_dump_t);
    for (uint8_t i = 0; i < count; i++) {
        dfx_last_dump_data(g_reg_dump_info[i].name, g_reg_dump_info[i].start_addr,
                           g_reg_dump_info[i].end_addr - g_reg_dump_info[i].start_addr);
    }
}
#endif /* DUMP_REG_SUPPORT */

#ifdef DUMP_MEM_SUPPORT
static diag_mem_dump_t g_mem_dump_info[] = {
    { "APP_ITCM_ORIGIN", APP_ITCM_ORIGIN, APP_ITCM_LENGTH },
    { "APP_DTCM_ORIGIN", APP_DTCM_ORIGIN, APP_DTCM_LENGTH },
#ifndef UNSUPPORT_OTHER_MEM
    { "SHARED_MEM", SHARED_MEM_START, SHARED_MEM_LENGTH },
    { "MCPU_TRACE_MEM_REGION", MCPU_TRACE_MEM_REGION_START, CPU_TRACE_MEM_REGION_LENGTH },
    { "BT_RAM_ORIGIN_APP_MAPPING", BT_RAM_ORIGIN_APP_MAPPING, BT_RAM_ORIGIN_APP_MAPPING_LENGTH },
    { "BCPU_TRACE_MEM_REGION", BCPU_TRACE_MEM_REGION_START, CPU_TRACE_MEM_REGION_LENGTH },
#endif
};

static void dfx_dump_mem(void)
{
    uint16_t count = (uint16_t)(sizeof(g_mem_dump_info) / sizeof(diag_mem_dump_t));
    for (uint8_t i = 0; i < count; i++) {
        dfx_last_dump_data(g_mem_dump_info[i].name, g_mem_dump_info[i].start_addr,
                           g_mem_dump_info[i].len);
    }
}
#endif /* DUMP_MEM_SUPPORT */

void dfx_last_dump(void)
{
    uint32_t file_num = 0;
#ifdef DUMP_MEM_SUPPORT
    file_num += (uint32_t)(sizeof(g_mem_dump_info) / sizeof(diag_mem_dump_t));
#endif
#ifdef DUMP_REG_SUPPORT
    file_num += (uint32_t)(sizeof(g_reg_dump_info) / sizeof(diag_reg_dump_t));
#endif

#if (defined(DUMP_MEM_SUPPORT) || defined(DUMP_REG_SUPPORT))
    dfx_last_dump_start(file_num);
#endif

#ifdef DUMP_MEM_SUPPORT
    dfx_dump_mem();
#endif
#ifdef DUMP_REG_SUPPORT
    dfx_dump_reg();
#endif
    unused(file_num);
}