/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: software nv by zdiag
 */
#ifndef ZDIAG_NV_H
#define ZDIAG_NV_H

#include "diag.h"

errcode_t zdiag_adapt_nv_read(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t zdiag_adapt_nv_write(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

#endif /* ZDIAG_NV_H */