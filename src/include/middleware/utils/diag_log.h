/**
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
* Description: header file for diag log. \n
*
*/

#ifndef DIAG_LOG_H
#define DIAG_LOG_H

#include "soc_diag_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_utils_dfx_log Log
 * @ingroup  middleware_utils_dfx
 * @{
 */

/**
 * @if Eng
 * @brief  Print Error-level diag logs.
 * @param  [in]  id log ID (only 0 is supported now)
 * @param  [in]  fmt the format information of the log
 * @param  [in]  args the parameter list of log (variable number, up to 10)
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  Error级别diag日志打印
 * @param  [in]  id 日志ID（当前只支持0）
 * @param  [in]  fmt 打印输出的格式字符串
 * @param  [in]  args 参数列表（最多支持10个参数）
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @endif
 */
#define uapi_diag_error_log(id, fmt, args...)         uapi_diag_error_logx(id, fmt, ##args)

/**
 * @if Eng
 * @brief  Print Warning-level diag logs.
 * @param  [in]  id log ID (only 0 is supported now)
 * @param  [in]  fmt the format information of the log
 * @param  [in]  args the parameter list of log (variable number, up to 10)
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  Warning级别diag日志打印
 * @param  [in]  id 日志ID（当前只支持0）
 * @param  [in]  fmt 打印输出的格式字符串
 * @param  [in]  args 参数列表（最多支持10个参数）
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @endif
 */
#define uapi_diag_warning_log(id, fmt, args...)       uapi_diag_warning_logx(id, fmt, ##args)

/**
 * @if Eng
 * @brief  Print Info-level diag logs.
 * @param  [in]  id log ID (only 0 is supported now)
 * @param  [in]  fmt the format information of the log
 * @param  [in]  args the parameter list of log (variable number, up to 10)
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  Info级别diag日志打印
 * @param  [in]  id 日志ID（当前只支持0）
 * @param  [in]  fmt 打印输出的格式字符串
 * @param  [in]  args 参数列表（最多支持10个参数）
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @endif
 */
#define uapi_diag_info_log(id, fmt, args...)          uapi_diag_info_logx(id, fmt, ##args)

/**
 * @if Eng
 * @brief  Print Debug-level diag logs.
 * @param  [in]  id log ID (only 0 is supported now)
 * @param  [in]  fmt the format information of the log
 * @param  [in]  args the parameter list of log (variable number, up to 10)
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  Debug级别diag日志打印
 * @param  [in]  id 日志ID（当前只支持0）
 * @param  [in]  fmt 打印输出的格式字符串
 * @param  [in]  args 参数列表（最多支持10个参数）
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @endif
 */
#define uapi_diag_debug_log(id, fmt, args...)         uapi_diag_debug_logx(id, fmt, ##args)

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DIAG_LOG_H */
