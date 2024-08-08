/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides PM_Clock header \n
 *
 * History: \n
 * 2023-01-11, Create file. \n
 */

#ifndef PM_CLOCK_H
#define PM_CLOCK_H

#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "pm_clock_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_pm_clock PM Clock
 * @ingroup  drivers_driver_pm
 * @{
 */

#if defined(CONFIG_PM_SUPPORT_CLOCK_CRG_API) && (CONFIG_PM_SUPPORT_CLOCK_CRG_API == 1)
/**
 * @if Eng
 * @brief  Config the common CRG CLOCK.
 * @param  [in]  id The common CRG CLOCK ID.
 * @param  [in]  clk_src The CLOCK source.
 * @param  [in]  clk_div The CLOCK divider.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置公共CRG时钟。
 * @param  [in]  id 公共CRG时钟模块ID。
 * @param  [in]  clk_src 时钟源。
 * @param  [in]  clk_div 时钟分频系数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_clock_crg_config(clock_crg_id_t id, clock_clk_src_t clk_src, uint8_t clk_div);

/**
 * @if Eng
 * @brief  Get the common CRG CLOCK frequency.
 * @param  [in]  id The common CRG CLOCK ID.
 * @return The common CRG CLOCK frequency.
 * @else
 * @brief  获取公共CRG时钟模块频率。
 * @param  [in]  id 公共CRG时钟模块ID。
 * @return 公共CRG时钟模块频率。
 * @endif
 */
uint32_t uapi_clock_crg_get_freq(clock_crg_id_t id);
#endif

/**
 * @if Eng
 * @brief  Config the CLOCK gating.
 * @param  [in]  id The CLOCK gating ID.
 * @param  [in]  clk_en The CLOCK is enabled when the value is true. The CLOCK is disabled when the value is false.
 * @else
 * @brief  配置时钟门控。
 * @param  [in]  id 时钟门控ID。
 * @param  [in]  clk_en 当值为true时，时钟开启。当值为false时，时钟关闭。
 * @endif
 */
void uapi_clock_clken_config(clock_clken_id_t id, bool clk_en);

/**
 * @if Eng
 * @brief  Config the CLOCK selector.
 * @param  [in]  id The CLOCK selector ID.
 * @param  [in]  clk_sel Configured value of the CLOCK selector.
 * @else
 * @brief  配置时钟选择器。
 * @param  [in]  id 时钟选择器ID。
 * @param  [in]  clk_sel 时钟选择器配置值。
 * @endif
 */
void uapi_clock_clksel_config(clock_clksel_id_t id, uint8_t clk_sel);

/**
 * @if Eng
 * @brief  Config the CLOCK frequency divisor.
 * @param  [in]  id The CLOCK divider ID.
 * @param  [in]  clk_div The CLOCK frequency divisor.
 * @else
 * @brief  配置时钟分频系数。
 * @param  [in]  id 时钟分频器ID。
 * @param  [in]  clk_div 时钟频率分频系数。
 * @endif
 */
void uapi_clock_clkdiv_config(clock_clkdiv_id_t id, uint8_t clk_div);

/**
 * @if Eng
 * @brief  Get the CLOCK frequency divisor.
 * @param  [in]  id The CLOCK divider ID.
 * @return The CLOCK frequency divisor.
 * @else
 * @brief  获取时钟分频系数。
 * @param  [in]  id 时钟分频器ID。
 * @return 时钟分频系数。
 * @endif
 */
uint8_t uapi_clock_get_clkdiv(clock_clkdiv_id_t id);

/**
 * @if Eng
 * @brief  Clock control API.
 * @param  [in]  type Clock control type.
 * @param  [in]  param Clock control parameter.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  时钟控制API。
 * @param  [in]  type 时钟控制类型。
 * @param  [in]  param 时钟控制参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_clock_control(clock_control_type_t type, uint8_t param);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
