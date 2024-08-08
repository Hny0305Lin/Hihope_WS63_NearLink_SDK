/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NON OS NMI
 * Author:
 * Create:  2023-9-16
 */

#include "non_os.h"
#include "chip_io.h"
#include "soc_osal.h"
#include "non_os_nmi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define NON_OS_NMI_EN_REG           NMI_CTL_REG_BASE_ADDR
#define NON_OS_NMI_STATUS_REG       (NMI_CTL_REG_BASE_ADDR + 0x8)
#define NON_OS_NMI_RAW_STATUS_REG   (NMI_CTL_REG_BASE_ADDR + 0x4)

static volatile uint16_t g_nmi_raw_en_status = 0;
static volatile uint16_t g_nmi_raw_interrupt_status = 0;

void non_os_nmi_config(nmi_types_e nmi, bool on)
{
    uint32_t irq = osal_irq_lock();
    if (on) {
        reg16_setbit(NON_OS_NMI_EN_REG, nmi);
    } else {
        reg16_clrbit(NON_OS_NMI_EN_REG, nmi);
    }
    g_nmi_raw_en_status = reg16(NON_OS_NMI_EN_REG);
    osal_irq_restore(irq);
}

uint16_t non_os_get_nmi_mask_status(void)
{
    return reg16(NON_OS_NMI_STATUS_REG);
}

uint16_t non_os_get_nmi_raw_status(void)
{
    g_nmi_raw_interrupt_status = reg16(NON_OS_NMI_RAW_STATUS_REG);
    return g_nmi_raw_interrupt_status;
}

#if CORE == APPS
nmi_types_e non_os_get_nmi_type(void)
{
    uint16_t nmi_mask_sts = non_os_get_nmi_mask_status();
    if ((nmi_mask_sts & (1 << NMI_CWDT)) != 0) {
        return NMI_CWDT;
    } else if ((nmi_mask_sts & (1 << NMI_XIP_CTRL)) != 0) {
        return NMI_XIP_CTRL;
    } else if ((nmi_mask_sts & (1 << NMI_XIP_CACHE)) != 0) {
        return NMI_XIP_CACHE;
    } else if ((nmi_mask_sts & (1 << NMI_MDMA)) != 0) {
        return NMI_MDMA;
    } else if ((nmi_mask_sts & (1 << NMI_SMDMA)) != 0) {
        return NMI_SMDMA;
    } else {
        return NMI_NONE;
    }
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif