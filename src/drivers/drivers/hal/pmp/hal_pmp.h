/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pmp hal source \n
 *
 * History: \n
 * 2022-09-26, Create file. \n
 */
#ifndef HAL_PMP_H
#define HAL_PMP_H

#include <stdint.h>
#include "errcode.h"
#include "pmp_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pmp_api PMP
 * @ingroup  drivers_hal_pmp
 * @{
 */

/**
 * @if Eng
 * @brief  hal-PMP region configuration parameters.
 * @else
 * @brief  PMP hal层域配置参数
 * @endif
 */
typedef struct hal_pmpx_config {
    uint8_t rwx               : 3;         /*!< @if Eng Read,write and execute permission.
                                                @else   读写执行权限 @endif */
    uint8_t a                 : 2;         /*!< @if Eng Address Matching mode.
                                                @else   地址匹配模式 @endif */
    uint8_t resv_0            : 2;         /*!< @if Eng Reserved.
                                                @else   保留。 @endif */
    uint8_t l                 : 1;         /*!< @if Eng Locking.
                                                @else   Lock位配置。  @endif */
} hal_pmpx_config_t;

/**
 * @if Eng
 * @brief  hal-PMP configuration parameters
 * @else
 * @brief  PMP hal层配置参数
 * @endif
 */
typedef struct hal_pmp_conf {
    uint32_t idx;                   /*!< @if Eng Region number.
                                         @else   域编号 @endif */
    uint32_t addr;                  /*!< @if Eng Base address, First Address in NAPOT mode and End Address in TOR mode.
                                         @else   基地址，在NAPOT/NA4模式为首地址，TOR模式为尾地址  @endif */
    uint8_t  attr;                  /*!< @if Eng Read-write mode configuration.
                                         @else   读写方式配置  @endif */
    hal_pmpx_config_t cfg;             /*!< @if Eng PMP region configuration.
                                         @else   PMP域配置  @endif */
} hal_pmp_conf_t;

/**
 * @if Eng
 * @brief  PMP Region Configuration.
 * @param  [in]  config PMP configuration parameters.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  PMP域配置。
 * @param  [in]  config PMP配置参数。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_pmp_config)(hal_pmp_conf_t *config);

/**
 * @if Eng
 * @brief  PMP config handler structure.
 * @else
 * @brief  PMP配置处理结构体
 * @endif
 */
typedef struct hal_pmp_funcs {
    hal_pmp_config   config;        /*!< @if Eng PMP config.
                                         @else   PMP配置 @endif */
} hal_pmp_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_pmp_funcs_t into the g_hal_pmp_funcs.
 * @param  [in]  funcs Interface between pmp driver and pmp hal.
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_pmp_funcs_t 到 g_hal_pmp_funcs
 * @param  [in]  funcs Driver层pmp和HAL层pmp的接口实例
 * @return ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_pmp_register_funcs(hal_pmp_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_pmp_funcs_t from the g_hal_pmp_funcs.
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  从g_hal_pmp_funcs注销 @ref hal_pmp_funcs_t
 * @return ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_pmp_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between pmp driver and pmp hal, see @ref hal_pmp_funcs_t.
 * @return Interface between pmp driver and pmp hal, see @ref hal_pmp_funcs_t.
 * @else
 * @brief  获取Driver层pmp和HAL层pmp的接口实例，参考 @ref hal_pmp_funcs_t.
 * @return Driver层pmp和HAL层pmp的接口实例，参考 @ref hal_pmp_funcs_t.
 * @endif
 */
hal_pmp_funcs_t *hal_pmp_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif