/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provide pm pwr dbg source. \n
 *
 * History: \n
 * 2023-11-13, Create file. \n
 */

#if defined(CONFIG_PM_SUPPORT_POWER_EXCEPTION_DEBUG) && (CONFIG_PM_SUPPORT_POWER_EXCEPTION_DEBUG == 1)

#include "chip_io.h"
#include "platform_core.h"
#include "debug_print.h"
#include "soc_osal.h"

#define PWR_EXCEP_CLR_RG      0x40003190
#define EXCEP_CFG_RG1         0x4000318C
#define PWR_EXCEP_RO_RG_STICK 0x40003194

// 滤毛刺信号
#define PMU0_UVLO_GRM_STICK           11
#define PMU_CLDO_OCP_GRM_STICK         1
#define PMU_LDO33_OCP_GRM_STICK        3
#define BUCK_SCP_GRM_STICK             5
#define BUCK_OCP_GRM_STICK             7
#define BUCK_OVP_GRM_STICK             9

#define PMU0_UVLO_GRM_1STICK_CLR      16
#define PMU_OCP_GRM_1STICK_CLR         4
#define PMU_LDO33_OCP_GRM_1STICK_CLR  12
#define BUCK_SCP_GRM_1STICK_CLR        8
#define BUCK_OCP_GRM_1STICK_CLR       10
#define BUCK_OVP_GRM_1STICK_CLR       14

#define PMU0_UVLO_GRM_INT_EN	      16
#define PMU_OCP_GRM_INT_EN	           4
#define PMU_LDO33_OCP_GRM_INT_EN      12
#define BUCK_SCP_GRM_INT_EN	           8
#define BUCK_OCP_GRM_INT_EN	          10
#define BUCK_OVP_GRM_INT_EN	          14

// 原始信号
#define PMU0_UVLO_RAW_1STICK       10
#define CLDO_OCP_RAW_1STICK         0
#define PMU_LDO33_OCP_RAW_1STICK    2
#define BUCK_SCP_RAW_1STICK         4
#define BUCK_OCP_RAW_1STICK         6
#define BUCK_OVP_RAW_1STICK         8

#define PMU0_UVLO_RAW_1STICK_CLR     17
#define PMU_OCP_RAW_1STICK_CLR	     5
#define PMU_LDO33_OCP_RAW_1STICK_CLR 13
#define BUCK_SCP_RAW_1STICK_CLR	     9
#define BUCK_OCP_RAW_1STICK_CLR	     11
#define BUCK_OVP_RAW_1STICK_CLR	     15

#define PMU0_UVLO_RAW_INT_EN	     17
#define PMU_OCP_RAW_INT_EN	          5
#define PMU_LDO33_OCP_RAW_INT_EN     13
#define BUCK_SCP_RAW_INT_EN	          9
#define BUCK_OCP_RAW_INT_EN	         11
#define BUCK_OVP_RAW_INT_EN	         15

struct pwr_dbg_reg {
    uint8_t val_bit;
    uint8_t clr_bit;
    uint8_t int_bit;
    uint8_t rsv;
    char *pwr_excep;
};

const struct pwr_dbg_reg g_pwr_dbg[] = {
    {PMU0_UVLO_RAW_1STICK, PMU0_UVLO_RAW_1STICK_CLR, PMU0_UVLO_RAW_INT_EN, 0, "uvlo_raw"},
    {PMU0_UVLO_GRM_STICK, PMU0_UVLO_GRM_1STICK_CLR, PMU0_UVLO_GRM_INT_EN, 0, "uvlo_grm"},
    {CLDO_OCP_RAW_1STICK, PMU_OCP_RAW_1STICK_CLR, PMU_OCP_RAW_INT_EN, 0, "cldo_ocp_raw"},
    {PMU_CLDO_OCP_GRM_STICK, PMU_OCP_GRM_1STICK_CLR, PMU_OCP_GRM_INT_EN, 0, "cldo_ocp_grm"},
    {PMU_LDO33_OCP_RAW_1STICK, PMU_LDO33_OCP_RAW_1STICK_CLR, PMU_LDO33_OCP_RAW_INT_EN, 0, "ldo33_ocp_raw"},
    {PMU_LDO33_OCP_GRM_STICK, PMU_LDO33_OCP_GRM_1STICK_CLR, PMU_LDO33_OCP_GRM_INT_EN, 0, "ldo33_ocp_grm"},
    {BUCK_SCP_RAW_1STICK, BUCK_SCP_RAW_1STICK_CLR, BUCK_SCP_RAW_INT_EN, 0, "bk_scp_raw"},
    {BUCK_SCP_GRM_STICK,  BUCK_SCP_GRM_1STICK_CLR, BUCK_SCP_GRM_INT_EN, 0, "bk_scp_grm"},
    {BUCK_OCP_RAW_1STICK, BUCK_OCP_RAW_1STICK_CLR, BUCK_OCP_RAW_INT_EN, 0, "bk_ocp_raw"},
    {BUCK_OCP_GRM_STICK,  BUCK_OCP_GRM_1STICK_CLR, BUCK_OCP_GRM_INT_EN, 0, "bk_ocp_grm"},
    {BUCK_OVP_RAW_1STICK,  BUCK_OVP_RAW_1STICK_CLR, BUCK_OVP_RAW_INT_EN, 0, "bk_ovp_raw"},
    {BUCK_OVP_GRM_STICK,  BUCK_OVP_GRM_1STICK_CLR, BUCK_OVP_GRM_INT_EN, 0, "bk_ovp_grm"}
};

static void pm_pwr_dbg_dump(void)
{
    uint32_t i;
    uint32_t pwr;
   
    pwr = readl(PWR_EXCEP_RO_RG_STICK);
    if (pwr == 0) {
        return;
    }

    PRINT("pwr exp detected: \t\n");
    for (i = 0; i < sizeof(g_pwr_dbg)/sizeof(g_pwr_dbg[0]); i++) {
        if ((pwr & (1U << g_pwr_dbg[i].val_bit)) == 0U) {
            continue;
        }
        writel(PWR_EXCEP_CLR_RG, 1 << g_pwr_dbg[i].clr_bit);
        PRINT("\t%s\t", g_pwr_dbg[i].pwr_excep);
    }
    PRINT("\t\n");
}

static void pm_pwr_dbg_isr(void)
{
    osal_irq_clear(PMU_CMU_ERR_IRQN);
    pm_pwr_dbg_dump();
    return;
}

static void pm_pwr_dbg_int_enable(void)
{
    uint32_t i;
    uint32_t int_en = 0;

    for (i = 0; i < sizeof(g_pwr_dbg)/sizeof(g_pwr_dbg[0]); i++) {
        int_en |= (1 << g_pwr_dbg[i].int_bit);
    }
    writel(EXCEP_CFG_RG1, 1 << int_en);
    return;
}

void pm_pwr_dbg_init(void)
{
    osal_irq_request(PMU_CMU_ERR_IRQN, (osal_irq_handler)pm_pwr_dbg_isr, NULL, NULL, NULL);
    pm_pwr_dbg_int_enable();
    osal_irq_enable(PMU_CMU_ERR_IRQN);
    return;
}

#endif