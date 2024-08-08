/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: SOC lOG implement using uart out put
 */
#ifndef SOC_LOG_UART_INSTANCE_H
#define SOC_LOG_UART_INSTANCE_H

#include "soc_log_strategy.h"
#include "dfx_write_interface.h"

errcode_t soc_log_register_write_impl(dfx_write_data_interface_t *impl);
#endif