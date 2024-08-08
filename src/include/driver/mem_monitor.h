/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides MEM_Monitor driver api \n
 *
 * History: \n
 * 2023-02-23, Create file. \n
 */
#ifndef MEM_MONITOR_H
#define MEM_MONITOR_H

#include <stdint.h>
#include "errcode.h"
#include "mem_monitor_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_mem_monitor MEM_Monitor
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the MEM_Monitor work mode.
 * @else
 * @brief  MEM_Monitor监控模式定义。
 * @endif
 */
typedef enum mem_monitor_mode {
    MEM_MONITOR_MODE_READ = 1,              /*!< @if Eng MEM_Monitor mode: read mode.
                                                 @else   MEM_Monitor模式：监控读模式。  @endif */
    MEM_MONITOR_MODE_WRITE                  /*!< @if Eng MEM_Monitor mode: write mode.
                                                 @else   MEM_Monitor模式：监控写模式。  @endif */
} mem_monitor_mode_t;

/**
 * @if Eng
 * @brief  Definition of MEM_Monitor configuration.
 * @else
 * @brief  MEM_Monitor配置类型定义。
 * @endif
 */
typedef struct mem_monitor_config {
    mem_monitor_section_t section;          /*!< @if Eng Address section for memory monitoring.
                                                 @else   内存监控地址段。  @endif */
    uint32_t addr_start;                    /*!< @if Eng Monitoring start address.
                                                 @else   监控起始地址。  @endif */
    uint32_t addr_end;                      /*!< @if Eng Monitoring end address.
                                                 @else   监控结束地址。  @endif */
    mem_monitor_mode_t mode;                /*!< @if Eng Memory monitoring mode.
                                                 @else   内存监控模式。  @endif */
} mem_monitor_config_t;

/**
 * @if Eng
 * @brief  MEM_Monitor callback function.
 * @param  [in]  addr MEM_Monitor Monitoring Address.
 * @param  [in]  mode MEM_Monitor Monitor the read/write mode.
 * @else
 * @brief  MEM_Monitor 回调类型定义。
 * @param  [in]  addr MEM_Monitor监控地址。
 * @param  [in]  mode MEM_Monitor监控读写模式。
 * @endif
 */
typedef void (*uapi_mem_monitor_callback_t)(uint32_t addr, mem_monitor_mode_t mode);

/**
 * @if Eng
 * @brief  Initialize the MEM_Monitor.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化MEM_Monitor。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_mem_monitor_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the MEM_Monitor.
 * @else
 * @brief  去初始化MEM_Monitor。
 * @endif
 */
void uapi_mem_monitor_deinit(void);

/**
 * @if Eng
 * @brief  Enable the specified MEM_Monitor.
 * @param  [in]  monitor Indicates the index of MEM_Monitor want to enable.
 * @param  [in]  callback The MEM_Monitor callback of the interrupt.
 * @else
 * @brief  使能指定的MEM_Monitor。
 * @param  [in]  monitor MEM_Monitor的索引。
 * @param  [in]  callback 中断的MEM_Monitor回调。
 * @endif
 */
void uapi_mem_monitor_enable(mem_monitor_t monitor, uapi_mem_monitor_callback_t callback);

/**
 * @if Eng
 * @brief  Disable the specified MEM_Monitor.
 * @param  [in]  monitor Indicates the index of MEM_Monitor want to disable.
 * @else
 * @brief  去使能MEM_Monitor。
 * @param  [in]  monitor MEM_Monitor的索引。
 * @endif
 */
void uapi_mem_monitor_disable(mem_monitor_t monitor);

/**
 * @if Eng
 * @brief  Set the Monitoring address and Monitoring mode for a MEM_Monitor section.
 * @param  [in]  monitor Indicates the index of MEM_Monitor.
 * @param  [in]  config Configuration of the section need to set, see @ref mem_monitor_config_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t .
 * @else
 * @brief  设置监控地址段以及工作模式。
 * @param  [in]  monitor MEM_Monitor的索引
 * @param  [in]  config 内存监控段的需要设置。 see @ref mem_monitor_config_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_mem_monitor_set_section(mem_monitor_t monitor, mem_monitor_config_t *config);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

