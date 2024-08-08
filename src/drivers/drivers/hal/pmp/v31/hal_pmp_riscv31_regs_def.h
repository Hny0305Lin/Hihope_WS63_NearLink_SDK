/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides RISCV pmp register \n
 *
 * History: \n
 * 2022-09-24, Create file. \n
 */
#ifndef HAL_PMP_RISCV31_REGS_DEF_H
#define HAL_PMP_RISCV31_REGS_DEF_H

#include <stdint.h>
#include "hal_pmp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pmp_riscv31_regs_def PMP RISCV31 Regs Definition
 * @ingroup  drivers_hal_pmp
 * @{
 */

#define PMPXCFG_NUM_PER_REG     4
#define MEMXATTR_NUM_PER_REG    8
#define PMP_REGION_NUM          16

#define PMPCFG0    0
#define PMPCFG1    1
#define PMPCFG2    2
#define PMPCFG3    3

#define PMPADDR0    0
#define PMPADDR1    1
#define PMPADDR2    2
#define PMPADDR3    3
#define PMPADDR4    4
#define PMPADDR5    5
#define PMPADDR6    6
#define PMPADDR7    7
#define PMPADDR8    8
#define PMPADDR9    9
#define PMPADDR10   10
#define PMPADDR11   11
#define PMPADDR12   12
#define PMPADDR13   13
#define PMPADDR14   14
#define PMPADDR15   15

/**
 * @brief  This union represents the bit fields in the pmpaddr register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the region config using the <i>b</i> elements.
 */
typedef union hal_pmpcfg {
    uint32_t d32;                           /*!< Raw register data. */
    hal_pmpx_config_t b[PMPXCFG_NUM_PER_REG];  /*!< PMP regions configurations. */
} hal_pmp_riscv31_cfg_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif