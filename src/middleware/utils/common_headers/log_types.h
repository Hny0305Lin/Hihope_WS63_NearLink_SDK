/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: log type header
 */
#ifndef LOG_TYPES_H
#define LOG_TYPES_H

/**
 * @defgroup connectivity_libs_common_log LOG Types
 * @ingroup  connectivity_libs_common
 * @{
 */
/**
 * @brief  Log level enum.
 */
typedef enum {
    LOG_LEVEL_NONE,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DBG,
    LOG_LEVEL_MAX,
} log_level_e;

#endif
