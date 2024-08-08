/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides Systick driver api \n
 *
 * History: \n
 * 2022-08-01， Create file. \n
 */
#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_systick Systick
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize the Systick.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化Systick。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void uapi_systick_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the Systick.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化Systick。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void uapi_systick_deinit(void);

/**
 * @if Eng
 * @brief  Clear Systick count.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  清除Systick计数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_systick_count_clear(void);

/**
 * @if Eng
 * @brief  Get Systick count.
 * @retval Systick current count.
 * @else
 * @brief  获取Systick计数值。
 * @retval Systick当前计数值。
 * @endif
 */
uint64_t uapi_systick_get_count(void);

/**
 * @if Eng
 * @brief  Get the Systick second.
 * @retval The current second of Systick.
 * @else
 * @brief  获取Systick计数秒值。
 * @retval Systick当前计数秒值。
 * @endif
 */
uint64_t uapi_systick_get_s(void);

/**
 * @if Eng
 * @brief  Get the Systick ms.
 * @retval The current ms of Systick.
 * @else
 * @brief  获取Systick计数毫秒值。
 * @retval Systick当前计数毫秒值。
 * @endif
 */
uint64_t uapi_systick_get_ms(void);

/**
 * @if Eng
 * @brief  Get the Systick us.
 * @retval The current us of Systick.
 * @else
 * @brief  获取Systick计数微秒值。
 * @retval Systick当前计数微秒值。
 * @endif
 */
uint64_t uapi_systick_get_us(void);

/**
 * @if Eng
 * @brief  Delay by count.
 * @param  [in]  c_delay Delay time.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  按计数延时
 * @param  [in]  c_delay 延时时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_systick_delay_count(uint64_t c_delay);

/**
 * @if Eng
 * @brief  Delay by s.
 * @param  [in]  s_delay Delay time.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  按秒数延时。
 * @param  [in]  s_delay 延时时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_systick_delay_s(uint32_t s_delay);

/**
 * @if Eng
 * @brief  Delay by ms.
 * @param  [in]  m_delay Delay time.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  按毫秒数延时。
 * @param  [in]  m_delay 延时时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_systick_delay_ms(uint32_t m_delay);

/**
 * @if Eng
 * @brief  Delay by us.
 * @param  [in]  u_delay Delay time.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  按微秒数延时。
 * @param  [in]  u_delay 延时时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_systick_delay_us(uint32_t u_delay);

#if defined(CONFIG_SYSTICK_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend the Systick.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起Systick。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_systick_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  Resume the Systick.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复Systick。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_systick_resume(uintptr_t arg);
#endif  /* CONFIG_SYSTICK_SUPPORT_LPM */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif