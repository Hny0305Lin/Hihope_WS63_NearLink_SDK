/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: LOG TRIGGER MODULE INTERRFACE
 * Author:
 * Create:
 */

#ifndef SRC_LIB_LOG_PRIVATE_LOG_TRIGGER_H
#define SRC_LIB_LOG_PRIVATE_LOG_TRIGGER_H

/**
 * @addtogroup connectivity_libs_log
 * @{
 */
#include "core.h"
#define CORE_LOGGING APPS
typedef void (*log_trigger_callback_t)(void);

/**
 * @brief  Ensure the log reader will be triggered.
 * This function should be called when the logger has detected it has written to an empty buffer.
 */
void log_trigger(void);

/**
 * @brief  Ensure the log trigger should be registered.
 * This function should be called when the log module init.
 * @param  callback: should match the type<log_trigger_callback_t>.
 */
void register_log_trigger(log_trigger_callback_t callback);

/**
 * @brief  trigger ipc to inform A core to save data from share mem in flash
 */
#if (BTH_WITH_SMART_WEAR == NO)
#if (CORE == BT)
void massdata_trigger(void *pay_i, uint8_t core, uint8_t type);
#endif
#endif

/**
 * @}
 */
#endif
