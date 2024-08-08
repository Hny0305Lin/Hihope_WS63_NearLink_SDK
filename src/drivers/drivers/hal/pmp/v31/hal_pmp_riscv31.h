/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides RISCV hal pmp \n
 *
 * History: \n
 * 2022-09-26, Create file. \n
 */
#ifndef HAL_PMP_RISCV31_H
#define HAL_PMP_RISCV31_H

#include "hal_pmp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pmp_riscv31 PMP RISCV31
 * @ingroup  drivers_hal_pmp
 * @{
 */

/**
 * @if Eng
 * @brief  Get interface between pmp driver and pmp hal.
 * @return pmp event handler structure
 * @else
 * @brief  获取Driver层pmp和HAL层pmp的接口
 * @return PMP 事件处理结构体地址
 * @endif
 */
hal_pmp_funcs_t *hal_pmp_riscv31_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif