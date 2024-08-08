/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides hal efuse \n
 *
 * History: \n
 * 2023-3-4, Create file. \n
 */
#ifndef HAL_EFUSE_V151_H
#define HAL_EFUSE_V151_H

#include "hal_efuse.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_efuse Efuse
 * @ingroup  drivers_hal_efuse
 * @{
 */

/**
 * @if Eng
 * @brief  Get functions of the efuse .
 * @else
 * @brief  获取 efuse的实例
 * @endif
 */
hal_efuse_funcs_t *hal_efuse_funcs_get(void);

void hal_efuse_set_clock_period(uint32_t clock_period);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

