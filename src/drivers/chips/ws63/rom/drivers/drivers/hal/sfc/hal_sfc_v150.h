/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides v150 HAL SFC \n
 *
 * History: \n
 * 2022-12-01, Create file. \n
 */
#ifndef HAL_SFC_V150_H
#define HAL_SFC_V150_H

#include "hal_sfc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sfc_v150 SFC v150
 * @ingroup  drivers_hal_sfc
 * @{
 */

/**
 * @if Eng
 * @brief  Get interface of SFC driver and SFC hal.
 * @else
 * @brief  获取Driver层SFC和HAL层SFC的接口
 * @endif
 */
hal_sfc_funcs_t *hal_sfc_v150_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif