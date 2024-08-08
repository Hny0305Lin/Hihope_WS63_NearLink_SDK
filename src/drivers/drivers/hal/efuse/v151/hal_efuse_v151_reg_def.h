/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides efuse register \n
 * \n
 * History: \n
 * 2022-3-4, Create file. \n
 */
#ifndef HAL_EFUSE_V151_REG_DEF_H
#define HAL_EFUSE_V151_REG_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_efuse_ip1_reg_def Efuse Regs Definition
 * @ingroup  drivers_hal_efuse
 * @{
 */

typedef union efuse_sts {
    uint32_t d32;
    struct  {
        uint32_t efuse_man_sts          : 2;
        uint32_t efuse_boot0_done       : 1;
        uint32_t efuse_boot1_done       : 1;
        uint32_t efuse_boot2_done       : 1;
    } b;
} efuse_sts_t;

typedef union efuse_ctl {
    uint32_t d32;
    struct  {
        uint32_t efuse_wr_rd            : 16;
    } b;
} efuse_ctl_t;

typedef union clock_period {
    uint32_t d32;
    struct  {
        uint32_t clock_period          : 8;
    } b;
} clock_period_t;

typedef union efuse_avdd_ctl {
    uint32_t d32;
    struct  {
        uint32_t efuse_avdd_sw          : 1;
    } b;
} efuse_avdd_ctl_t;

#if defined(CONFIG_EFUSE_SWITCH_EN)
typedef union efuse_switch_en_ctl {
    uint32_t d32;
    struct  {
        uint32_t efuse_switch_enable          : 16;
    } b;
} efuse_switch_en_ctl_t;
#endif

/**
 * @brief  Registers associated with Efuse.
 */
typedef struct efuse_regs {
    volatile efuse_ctl_t efuse_ctl_data;
    volatile clock_period_t efuse_clk_period_data;
    volatile uint32_t reserved;
    volatile efuse_avdd_ctl_t efuse_avdd_ctl_data;
} efuse_regs_t;

/**
 * @brief  Registers associated with Efuse boot done.
 */
typedef struct efuse_boot_done_regs {
    volatile efuse_sts_t efuse_sts_data;
} efuse_boot_done_regs_t;

#if defined(CONFIG_EFUSE_SWITCH_EN)
/**
 * @brief  Registers associated with Efuse boot done.
 */
typedef struct efuse_switch_en_regs {
    volatile efuse_switch_en_ctl_t efuse_switch_en;
} efuse_switch_en_regs_t;
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
