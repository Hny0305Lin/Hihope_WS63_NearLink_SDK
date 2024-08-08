/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  LOG MEMORY SECTION MODULE.
 * Author:
 * Create:  2018-10-15
 */

#include "log_memory_section.h"

/*
 * Public function definitions
 */
void log_memory_section_get(log_memory_region_section_t section, log_memory_section_params_t *section_data)
{
    const log_memory_region_control_t *log_region_control =
        (const log_memory_region_control_t *)(uintptr_t)LOG_MEMORY_REGION_CONTROL_BLOCK_POINTER;
    section_data->start = (uint8_t *)(uintptr_t)(LOGGING_REGION_START + log_region_control->offset[section]);
#if CORE_NUMS > 1
    // section_data has been previously initialized by the security core.
    if (section == LOG_MEMORY_REGION_MAX_NUMBER - 1) {  // If it is the last section use the length in stead
        section_data->length = log_region_control->length - log_region_control->offset[section];
    } else {
        section_data->length = log_region_control->offset[(uint8_t)section + 1] -
                               log_region_control->offset[(uint8_t)section];
    }
#else
    if (section == LOG_MEMORY_REGION_MAX_NUMBER - 1) {  // If it is the last section use the length in stead
        section_data->length = log_region_control->length - log_region_control->offset[section];
    }
#endif
}

log_memory_section_control_t *log_memory_section_get_control(log_memory_region_section_t section)
{
    log_memory_region_control_t *log_region_control =
        (log_memory_region_control_t *)LOG_MEMORY_REGION_CONTROL_BLOCK_POINTER;
    return &log_region_control->section_control[section];
}
