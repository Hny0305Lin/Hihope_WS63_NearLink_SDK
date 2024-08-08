/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   LOG MEMORY SECTION INTERFACE
 * Author:
 * Create:  2018-10-15
 */

#ifndef NON_OS_LOG_MEMORY_SECTION_H
#define NON_OS_LOG_MEMORY_SECTION_H

#include "log_memory_definitions.h"

/**
 * @addtogroup connectivity_drivers_non_os_log
 * @{
 */
/**
 * @brief  Log memory section parameters
 */
typedef struct {
    uint8_t *start;
    uint32_t length;
} log_memory_section_params_t;

/**
 * @brief  get the section data for a given region
 * @param  section log memory section
 * @param  section_data pointer where the section data will be stored
 */
void log_memory_section_get(log_memory_region_section_t section, log_memory_section_params_t *section_data);

/**
 * @brief  Get a pointer to the section control for a given log memory section
 * @param  section log memory section
 * @return pointer to the section control for log memory section
 */
log_memory_section_control_t *log_memory_section_get_control(log_memory_region_section_t section);

/** Macro to check if a pointer is in the log memory region */
#define pointer_in_log_memory_region(x) (((uint32_t)(x) >= (LOGGING_REGION_START)) && \
                                         ((uint32_t)(x) <= ((LOGGING_REGION_START) + (LOGGING_REGION_LENGTH))))

/**
 * @}
 */
#endif
