/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides common utils for security_unified service layer. \n
 *
 * History: \n
 * 2023-03-14, Create file. \n
 */
#ifndef SL_COMMON_H
#define SL_COMMON_H

#include <stdint.h>
#include "errcode.h"
#ifdef CONFIG_SECURITY_UNIFIED_SUPPORT_DEEP_SLEEP
#include "pm_veto.h"
#else
#define uapi_pm_add_sleep_veto(...)
#define uapi_pm_remove_sleep_veto(...)
#define PM_SECURITY_VETO_ID
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  Convert security-driven error codes to standard error codes.
 * @param  [in]  from_errcode Error codes returned by the security driver interface.
 * @retval Converted error code. For details, see @ref errcode_t
 * @else
 * @brief  将安全驱动的错误码转换成标准错误码
 * @param  [in]  from_errcode 安全驱动接口返回的错误码
 * @retval 转换后的错误码，参考 @ref errcode_t
 * @endif
 */
errcode_t crypto_sl_common_get_errcode(int32_t from_errcode);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif