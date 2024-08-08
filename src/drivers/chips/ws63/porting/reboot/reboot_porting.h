/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pmp port template \n
 *
 * History: \n
 * 2022-09-26， Create file. \n
 */
#ifndef REBOOT_PORTING_H
#define REBOOT_PORTING_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HAL_REBOOT_REASON_MCPU_WDT      0x2002
#define HAL_REBOOT_REASON_DSP_WDT       0x4002
#define HAL_REBOOT_REASON_BCPU_WDT      0x8002
#define HAL_REBOOT_REASON_BCPU_CHIP_WDT 0x8004

#if MASTER_BY_WS53_ONLY
#define HAL_CHIP_RESET_REG                  (ULP_AON_CTL_RB_ADDR + CHIP_RESET_OFF)
#else
#define HAL_CHIP_RESET_REG                  (GLB_CTL_M_RB_BASE + CHIP_RESET_OFF)
#endif

#define HAL_CHIP_RESET_REG_OFFSET           0x0
#define HAL_CHIP_RESET_REG_ENABLE_RESET_BIT 2

#define HAL_PMU_PROTECT_STATUS_REG     (PMU1_CTL_RB_BASE + 0x370)
#define HAL_PMU_PROTECT_STATUS_CLR_REG (PMU1_CTL_RB_BASE + 0x370)

// buck short circuit protection, if this case occur, cpu can't read, only ssi can read the status.
#define HAL_PMU_PROTECT_BUCK2_SCP_BIT 2
#define HAL_PMU_PROTECT_BUCK1_SCP_BIT 3
#define HAL_PMU_PROTECT_CHIP_WDG_BIT  4
#define HAL_PMU_PROTECT_CHIP_WDG_BIT_CLR  0

#define HAL_BCPU_RESET_STS_REG (GLB_CTL_M_RB_BASE + 0x30)
#define HAL_MCPU_RESET_STS_REG (GLB_CTL_M_RB_BASE + 0x34)

// This status is valid when CLDO2 power on.
#define HAL_DSP_RESET_STS_REG            (GLB_CTL_B_RB_BASE + 0x38)
#define HAL_RESET_STS_COMMON_RESET_BIT   0 // Control by RST_PULSE0 at GLB_CTL_B + 0x70
#define HAL_RESET_STS_CORE_RESET_BIT     1 // Control by RST_PULSE0 at GLB_CTL_B + 0x70
#define HAL_RESET_STS_CORE_WDT_RESET_BIT 2
#define HAL_RESET_STS_CLEAR_REG (GLB_CTL_M_RB_BASE + 0x3c)
#define HAL_RESET_STS_CLEAR_ALL 0xFF

// Software dedicated whether a hard power failure occurs
#define HAL_ULP_AON_GENERAL_REG         (ULP_AON_CTL_RB_ADDR + 0x14)

#define HAL_ULP_AON_NO_POWEROFF_FLAG    0xA5A5

/**
 * @defgroup drivers_port_reboot REBOOT
 * @ingroup  drivers_port
 * @{
 */

typedef enum reboot_port_rst_reason {
    RST_REASON_WDT_RST = 0,
    RST_REASON_SOFT_RST,
    RST_REASON_HARD_RST,
    RST_REASON_INVALID
} reboot_port_rst_reason_t;

void reboot_port_reboot_chip(void);
uint32_t reboot_port_get_rst_reason(void);
void reboot_mem_flash_erase(void);
void reboot_mem_save_to_flash(void);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif