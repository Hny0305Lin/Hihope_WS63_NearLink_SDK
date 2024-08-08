/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  common logging producer interface - need to change name of log.h in all the protocol core files
 * Author:
 * Create:
 */

#ifndef LOG_COMMON_H
#define LOG_COMMON_H

/**
 * @addtogroup connectivity_libs_log
 * @{
 */
#include "panic.h"
#include "log_types.h"
#include "log_buffer_common.h"
#include "log_oam_logger.h"
#include "log_oam_status.h"
#include "log_oam_pcm.h"
#include "log_oam_ota.h"

uint16_t get_log_sn_number(void);

void log_init(void);

void log_init_after_rtos(void);

/**
 * @brief  Log the given log message
 * @param  buffer
 * @param  length
 */
void log_event(const uint8_t *buffer, uint16_t length);

/**
 * @brief  Set local log level from system log level
 */
void log_set_local_log_level(log_level_e log_level);

/**
 * @brief  Get current log level
 * @return current log level
 */
log_level_e log_get_local_log_level(void);

#if SYS_DEBUG_MODE_ENABLE == YES
/**
 * @brief  Register oam command callback
 */
void log_register_oam_msg_callback(void);
#endif  /* end of SYS_DEBUG_MODE_ENABLE == YES */

/**
 * @brief  massdata init
 */
void massdata_init(void);

/**
 * @brief  Get missed messages count
 * @return missed messages count
 */
uint32_t log_get_missed_messages_count(void);

/**
 * @brief  Get all messages count
 * @return all messages count
 */
uint32_t log_get_all_messages_count(void);

/**
 * @}
 */
#endif
