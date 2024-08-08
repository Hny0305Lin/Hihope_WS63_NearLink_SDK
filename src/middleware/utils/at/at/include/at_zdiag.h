/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: software at by zdiag
 */
#ifndef ZDIAG_AT_H
#define ZDIAG_AT_H

#ifdef CONFIG_AT_SUPPORT_ZDIAG
#include "diag.h"

void zdiag_at_init(void);
errcode_t zdiag_at_proc(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

#endif
#endif /* ZDIAG_AT_H */