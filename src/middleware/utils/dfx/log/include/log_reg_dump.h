/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:   LOG REG DUMP MODULE
 * Author:
 * Create: 2020-09-15
 */

#ifndef LOG_REG_DUMP_H
#define LOG_REG_DUMP_H

#include "core.h"

#if (defined (BUILD_APPLICATION_STANDARD) || defined (TEST_SUITE))
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
/**
 * @brief  Dump register address config check.
 */
void log_exception_dump_reg_check(void);

/**
 * @brief  Dump register to sdt.
 */
void log_exception_dump_reg(void);
#endif
#endif
#endif

