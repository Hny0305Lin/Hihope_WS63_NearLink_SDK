/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides TCXO driver api \n
 *
 * History: \n
 * 2022-08-16， Create file. \n
 */

#ifndef TCXO_H
#define TCXO_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_tcxo TCXO
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize TCXO.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化TCXO。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tcxo_init(void);

/**
 * @if Eng
 * @brief  Deinitialize TCXO.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化TCXO。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tcxo_deinit(void);

/**
 * @if Eng
 * @brief  Get TCXO count.
 * @retval TCXO current count.
 * @else
 * @brief  获取TCXO计数值。
 * @retval TCXO当前计数值。
 * @endif
 */
uint64_t uapi_tcxo_get_count(void);

 /**
 * @if Eng
 * @brief  Get the TCXO ms.
 * @retval The current ms of TCXO.
 * @else
 * @brief  获取TCXO计数毫秒值。
 * @retval TCXO当前计数毫秒值。
 * @endif
 */
uint64_t uapi_tcxo_get_ms(void);

/**
 * @if Eng
 * @brief  Get the TCXO us.
 * @retval The current us of TCXO.
 * @else
 * @brief  获取TCXO计数微秒值。
 * @retval TCXO当前计数微秒值。
 * @endif
 */
uint64_t uapi_tcxo_get_us(void);

/**
 * @if Eng
 * @brief  Sets the delay time of ms.
 * @param  [in]  m_delay The ms times to delay.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置延迟毫秒数。
 * @param  [in]  m_delay 延迟毫秒数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tcxo_delay_ms(uint32_t m_delay);

/**
 * @if Eng
 * @brief  Sets the delay time of us.
 * @param  [in]  u_delay The us times to delay.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置延迟微秒数。
 * @param  [in]  u_delay 延迟微秒数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tcxo_delay_us(uint32_t u_delay);

#if defined(CONFIG_TCXO_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  Suspend the TCXO.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起TCXO。
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tcxo_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  Resume the TCXO.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复TCXO。
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_tcxo_resume(uintptr_t arg);
#endif  /* CONFIG_TCXO_SUPPORT_LPM */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
