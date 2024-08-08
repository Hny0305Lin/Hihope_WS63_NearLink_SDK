/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  LOG MEMORY REGION SETUP MODULE INTERFACE.
 * Author:
 * Create:  2018-10-15
 */

#ifndef NON_OS_LOG_MEMORY_REGION_H
#define NON_OS_LOG_MEMORY_REGION_H

#include <stdbool.h>

/**
 * @addtogroup connectivity_drivers_non_os_log
 * @{
 */
/**
 * @brief  Initialize the log memory region and set the alias region for the other cores
 */
void log_memory_region_init(void);

/**
 * @brief  Initialize the mass data memory region and set the alias region for the other cores
 */
void massdata_memory_region_init(void);

/**
 * Used to check whether the log area is initialized.
*/
bool log_memory_is_init(void);
/**
 * @}
 */
#endif
