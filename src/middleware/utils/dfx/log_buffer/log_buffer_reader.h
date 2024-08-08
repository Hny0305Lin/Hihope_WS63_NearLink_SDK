/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: LOG BUFFER READER MODULE INTERFACE.
 * Author:
 * Create:  2018-10-15
 */

#ifndef NON_OS_LOG_BUFFER_READER_H
#define NON_OS_LOG_BUFFER_READER_H

#include "log_memory_definitions.h"
#include "log_buffer_common.h"

/**
 * @addtogroup connectivity_drivers_non_os_log
 * @{
 */
typedef enum {
    LOG_READER_RET_OK,
    LOG_READER_RET_BUSY,
    LOG_READER_RET_ERROR,
    LOG_READER_RET_TIMEDOUT,
    LOG_READER_RET_ERROR_OVERFLOW_ON_DISCARDING,
    LOG_READER_RET_THERE_IS_NO_NEXT_MESSAGE,
    LOG_READER_RET_ERROR_IN_PARAMS,
    LOG_READER_RET_ERROR_TRYING_TO_DISCARD_WHEN_EMPTY,
    LOG_READER_RET_ERROR_CORRUPT_SHARED_MEMORY,
} log_reader_ret_t;

/**
 * @brief  Initialize the log buffer reader module
 */
log_reader_ret_t log_buffer_reader_init(void);

/**
 * @brief  Lock the next message in the log buffer and returns its parameters.
 * @param  lmsec log memory section of the region holding the next message
 * @param  lb_header The log_buffer_header_t for the next log will be stored here
 * @return LOG_READER_RET_OK if the next message was locked, LOG_READER_RET_THERE_IS_NO_NEXT_MESSAGE if there is no
 * next message to lock, an error code otherwise.
 */
log_reader_ret_t log_buffer_reader_lock_next(log_memory_region_section_t *lmsec, const log_buffer_header_t *lb_header);

/**
 * @brief  Claim the next message in the buffer.
 * @param  lmsec log memory section
 * @param  r1 (return value) a pointer to the first part of the message
 * @param  len1 (return value) length of the first part of the message
 * @param  r2 (return value) a pointer to the first part of the message ( in case is not in a contiguous memory block)
 * @param  len2 (return value) length of the first part of the message  ( in case is not in a contiguous memory block)
 * @return LOG_READER_RET_OK or an error code
 */
log_reader_ret_t log_buffer_reader_claim_next(log_memory_region_section_t lmsec, uint8_t **r1,
                                              uint32_t *len1, uint8_t **r2, uint32_t *len2);

/**
 * @brief  Discard a previously claimed message
 * @param  lmsec log memory section
 * @return LOG_READER_RET_OK or an error code
 */
log_reader_ret_t log_buffer_reader_discard(log_memory_region_section_t lmsec);

/**
 * @brief  Get the used space in a log memory section.
 * @param  lmsec log memory section
 * @param  used_space used space in bytes
 * @return LOG_READER_RET_OK or an error code
 */
log_reader_ret_t log_buffer_get_used_space(log_memory_region_section_t lmsec, uint32_t *used_space);

/**
 * @brief  Recovery error log region.
 * @param  lmsec log memory section
 */
void log_buffer_reader_error_recovery(log_memory_region_section_t lmsec);

/**
 * @}
 */
#endif
