/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 HAL tcxo \n
 *
 * History: \n
 * 2022-08-16， Create file. \n
 */
#ifndef HAL_TCXO_V150_H
#define HAL_TCXO_V150_H

#include "hal_tcxo.h"
#include "hal_tcxo_v150_regs_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_tcxo_v150 TCXO V150
 * @ingroup  drivers_hal_tcxo
 * @{
 */

/**
 * @if Eng
 * @brief  Get the instance of v150.
 * @return The instance of v150.
 * @else
 * @brief  获取v150实例。
 * @return v150实例。
 * @endif
 */
hal_tcxo_funcs_t *hal_tcxo_v150_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif