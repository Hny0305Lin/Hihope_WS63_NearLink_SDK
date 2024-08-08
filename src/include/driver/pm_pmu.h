/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides PMU header \n
 *
 * History: \n
 * 2023-01-12， Create file. \n
 */

#ifndef PM_PMU_H
#define PM_PMU_H

#include "common_def.h"
#include "errcode.h"
#include "pm_pmu_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_pm_pmu PM PMU
 * @ingroup  drivers_driver_pm
 * @{
 */

/**
 * @if Eng
 * @brief  Set the LDO voltage.
 * @param  [in]  id The LDO to set voltage.
 * @param  [in]  vset The voltage value to be configured.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置LDO电压。
 * @param  [in]  id 用来设置电压的LDO。
 * @param  [in]  vset 要配置的电压值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmu_ldo_set_voltage(pmu_ldo_id_t id, uint8_t vset);

/**
 * @if Eng
 * @brief  Get the LDO voltage.
 * @param  [in]  id The LDO to get the voltage.
 * @return The LDO current voltage.
 * @else
 * @brief  获取LDO电压。
 * @param  [in]  id 用来获取电压的LDO。
 * @return LDO当前电压值。
 * @endif
 */
uint8_t uapi_pmu_ldo_get_voltage(pmu_ldo_id_t id);

/**
 * @if Eng
 * @brief  PMU control API.
 * @param  [in]  type PMU control type.
 * @param  [in]  param PMU control parameter.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  PMU控制API。
 * @param  [in]  type PMU控制类型。
 * @param  [in]  param PMU控制参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pmu_control(pmu_control_type_t type, uint8_t param);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
