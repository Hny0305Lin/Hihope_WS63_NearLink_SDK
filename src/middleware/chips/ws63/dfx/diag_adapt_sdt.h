/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: adapt sdt
 * This file should be changed only infrequently and with great care.
 */

#ifndef ZDIAG_ADAPT_SDT_H
#define ZDIAG_ADAPT_SDT_H

#include "diag.h"
#include "log_oam_msg.h"
#include "log_oam_logger.h"

void zdiag_adapt_sdt_msg_proc(uint8_t *buf1, uint32_t len1, uint8_t *buf2, uint32_t len2);
errcode_t zdiag_adapt_sdt_msg_dispatch(uint32_t msg_id, uint8_t *data, uint32_t data_len);
void diag_register_debug_cmd_callback(oam_cmd_handle_callback func);
void diag_debug_cmd_proc(uint8_t *data, uint32_t len);

#ifndef FORBID_AUTO_LOG_REPORT
void diag_auto_log_report_enable(void);
#endif

#endif
