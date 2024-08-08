/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag msg
 * This file should be changed only infrequently and with great care.
 */

#ifndef __ZDIAG_MSG_H__
#define __ZDIAG_MSG_H__

#include "errcode.h"
#include "diag_common.h"
#include "diag_channel.h"

errcode_t diag_msg_proc(uint16_t msg_id, uint8_t *msg, uint32_t msg_len);

void uapi_zdiag_set_offline_log_enable(bool enable);
bool uapi_zdiag_offline_log_is_enable(void);

#endif
