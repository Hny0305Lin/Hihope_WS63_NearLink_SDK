/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  OAM TRACE for Suzhe BT Core
 *
 * Create: 2021-10-25
 */

#ifndef OAM_TRACE_H
#define OAM_TRACE_H

#include <stdint.h>
#ifdef USE_CMSIS_OS
#include "arch/exception.h"
#endif

#include "stdbool.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

bool is_valid_txt_addr(uint32_t pc);

/**
 * @brief  Rigister Exception handle function.
 */
void register_os_exec_hook(void);

#ifdef USE_CMSIS_OS
/**
 * @brief  Invoked by exception to dump exception information.
 * @param  exc_type Exception type.
 * @param  exc_buff_addr Exception buff addr.
 */
void exec_fault_handler(uint32_t exc_type, const ExcContext *exc_buff_addr);
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif