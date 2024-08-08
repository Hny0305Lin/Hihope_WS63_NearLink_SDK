/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Test pm port source. \n
 *
 * History: \n
 * 2023-11-13, Create file. \n
 */

#include "pm_porting.h"
#include "test_suite.h"
#include "test_suite_errors.h"
#include "test_pm_port.h"

static int test_pm_port_enter_low_power(int argc, char *argv[])
{
    unused(argc);
    unused(argv);

    pm_port_enter_lowpower();

    return TEST_SUITE_OK;
}

static int test_pm_port_exit_low_power(int argc, char *argv[])
{
    unused(argc);
    unused(argv);

    pm_port_exit_lowpower();

    return TEST_SUITE_OK;
}

void add_pm_port_test_case(void)
{
    uapi_test_suite_add_function("test_pm_port_enter_low_power", "Params: none", test_pm_port_enter_low_power);
    uapi_test_suite_add_function("test_pm_port_exit_low_power", "Params: none", test_pm_port_exit_low_power);
}
