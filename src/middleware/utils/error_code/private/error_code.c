/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  error_code_e module implementation
 * Author:
 * Create: 2018-10-15
 */

#include "error_code.h"
#include "non_os.h"

static uint32_t g_error_code = 0;

void error_code_reset(void)
{
    g_error_code = 0;
}

void error_code_set(error_code_e id)
{
    non_os_enter_critical();
    g_error_code |= BIT(id);
    non_os_exit_critical();
}

void error_code_clear(error_code_e id)
{
    non_os_enter_critical();
    g_error_code &= (~BIT(id));
    non_os_exit_critical();
}

uint32_t error_code_get(void)
{
    return g_error_code;
}
