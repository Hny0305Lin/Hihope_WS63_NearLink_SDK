/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: dfx system init
 * This file should be changed only infrequently and with great care.
 */
#ifndef DFX_SYSTEM_INIT_H
#define DFX_SYSTEM_INIT_H

#include <stdint.h>
#include "errcode.h"
#include "log_oam_logger.h"

errcode_t dfx_system_init(void);
unsigned long dfx_get_osal_queue_id(void);
int32_t msg_process_proc(uint32_t msg_id, uint8_t *data, uint32_t size);
#endif