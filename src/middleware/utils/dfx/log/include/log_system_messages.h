/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   system log messages
 * Author:
 * Create:
 */

#ifndef LOG_SYSTEM_MESSAGES_H
#define LOG_SYSTEM_MESSAGES_H

#include "log_common.h"

/**
 * @addtogroup connectivity_libs_log
 * @{
 */
typedef struct {
    uint32_t counter;  // !< Number of missed messages.
} log_system_missed_message_t;

typedef enum {
    LOG_SYSTEM_MISSED_MESSAGE_BUFFER,  // missed messages due to shared memeory being full
} log_system_message_id_t;

typedef enum {
    LOG_SYSTEM_MODULE_LOG,  // logging
} log_system_module_t;

/**
 * @}
 */
#endif