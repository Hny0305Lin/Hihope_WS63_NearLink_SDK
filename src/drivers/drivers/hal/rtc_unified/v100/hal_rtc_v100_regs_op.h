/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V100 rtc register operation API \n
 *
 * History: \n
 * 2023-03-02, Create file. \n
 */
#ifndef HAL_RTC_V100_REGS_OP_H
#define HAL_RTC_V100_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "rtc_porting.h"
#include "hal_rtc_v100_regs_def.h"
#include "soc_osal.h"
#include "debug/osal_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_rtc_v100_regs_op RTC V100 Regs Operation
 * @ingroup  drivers_hal_rtc
 * @{
 */

extern uintptr_t g_rtc_comm_regs;
extern uintptr_t g_rtc_regs[CONFIG_RTC_MAX_NUM];

/**
 * @brief  Get the value of @ref rtc_control_reg_data.enable.
 * @param  [in]  index Index for one of the rtc.
 * @return The value of @ref rtc_control_reg_data.enable.
 */
static inline uint32_t hal_rtc_control_reg_get_enable(rtc_index_t index)
{
    rtc_control_reg_data_t rtc_control_reg;
    rtc_control_reg.d32 = ((rtc_regs_info_t *)g_rtc_regs[index])->control_reg;
    return rtc_control_reg.b.enable;
}

/**
 * @brief  Set the value of @ref rtc_control_reg_data.enable.
 * @param  [in]  index Index for one of the rtc.
 * @param  [in]  val Enable or not.
 */
static inline void hal_rtc_control_reg_set_enable(rtc_index_t index, uint32_t val)
{
    rtc_control_reg_data_t rtc_control_reg;
    rtc_control_reg.d32 = ((rtc_regs_info_t *)g_rtc_regs[index])->control_reg;
    rtc_control_reg.b.enable = val;
    ((rtc_regs_info_t *)g_rtc_regs[index])->control_reg = rtc_control_reg.d32;
}

/**
 * @brief  Get the value of @ref rtc_control_reg_data.mode.
 * @param  [in]  index Index for one of the rtc.
 * @return The value of @ref rtc_control_reg_data.mode.
 */
static inline uint32_t hal_rtc_control_reg_get_mode(rtc_index_t index)
{
    rtc_control_reg_data_t rtc_control_reg;
    rtc_control_reg.d32 = ((rtc_regs_info_t *)g_rtc_regs[index])->control_reg;
    return rtc_control_reg.b.mode;
}

/**
 * @brief  Set the value of @ref rtc_control_reg_data.mode.
 * @param  [in]  index Index for one of the rtc.
 * @param  [in]  val The work mode of rtc.
 */
static inline void hal_rtc_control_reg_set_mode(rtc_index_t index, uint32_t val)
{
    rtc_control_reg_data_t rtc_control_reg;
    rtc_control_reg.d32 = ((rtc_regs_info_t *)g_rtc_regs[index])->control_reg;
    rtc_control_reg.b.mode = val;
    ((rtc_regs_info_t *)g_rtc_regs[index])->control_reg = rtc_control_reg.d32;
}

/**
 * @brief  Get the value of @ref rtc_regs_info.load_count.
 * @param  [in]  index Index for one of the rtc.
 * @return The value of @ref rtc_regs_info.load_count.
 */
static inline uint32_t hal_rtc_load_count_get(rtc_index_t index)
{
    return ((rtc_regs_info_t *)g_rtc_regs[index])->load_count;
}

/**
 * @brief  Set the value of @ref rtc_regs_info.load_count.
 * @param  [in]  index Index for one of the rtc.
 * @param  [in]  val The load count value.
 */
static inline void hal_rtc_load_count_set(rtc_index_t index, uint32_t val)
{
    ((rtc_regs_info_t *)g_rtc_regs[index])->load_count = val;
}

/**
 * @brief  Get the value of @ref rtc_regs_info.current_value.
 * @param  [in]  index Index for one of the rtc.
 * @return The value of @ref rtc_regs_info.current_value.
 */
static inline uint32_t hal_rtc_get_current_load(rtc_index_t index)
{
    return ((rtc_regs_info_t *)g_rtc_regs[index])->current_value;
}

/**
 * @brief  To clear pending interrupt. Get the value of @ref rtc_regs_info.eoi.
 * @param  [in]  index Index for one of the rtc.
 * @return The value of @ref rtc_regs_info.eoi.
 */
static inline uint32_t hal_rtc_get_eoi(rtc_index_t index)
{
    return ((rtc_regs_info_t *)g_rtc_regs[index])->eoi;
}

#if defined(CONFIG_RTC_SUPPORT_LPM)
/**
 * @brief  Get rtc interrupt status.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @return The value of @ref rtc_regs_info.int_status.
 */
static inline uint32_t hal_rtc_v100_get_int_sts(rtc_index_t index)
{
    return ((rtc_regs_info_t *)g_rtc_regs[index])->int_status;
}
#endif /* CONFIG_RTC_SUPPORT_LPM */
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif