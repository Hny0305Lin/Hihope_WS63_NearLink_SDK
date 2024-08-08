/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx soc log impl str
 * This file should be changed only infrequently and with great care.
 */

#ifndef SOC_LOG_IMPL_STR_H
#define SOC_LOG_IMPL_STR_H
#include <stdint.h>
#include "errcode.h"
#include "dfx_write_interface.h"

typedef int32_t (*dfx_soc_log_output_handler)(const uint8_t *data,  uint32_t len);

errcode_t dfx_serial_get_write_data_impl(dfx_write_data_interface_t *impl);
void dfx_register_soc_log_output_handler(dfx_soc_log_output_handler handler);

#endif
