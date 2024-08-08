/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include "common_def.h"
#include "ctype.h"
#include "securec.h"
#include "timer.h"
#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE
#include "ipc.h"
#include "debug_print.h"
#include "osal_addr.h"
#endif
#include "test_suite_ipc.h"
#include "test_suite_errors.h"
#include "test_suite.h"
#include "test_suite_functions_processor.h"
#include "test_suite_log.h"
#include "test_suite_console.h"
#include "test_suite_commands_processor.h"

#define TEST_SUITE_COMMANDS_ARGUMENT_SIZE   16
#define TEST_SUITE_DEFAULT_TIMEOUT          15
#define TEST_SUITE_COMMAND_SEPARATOR        "#\0"
#define TEST_SUITE_TIMEOUT_BASE             10

#define APPS_SHORT_PREFIX                   "a"
#define APPS_PREFIX                         "apps"
#define BT_SHORT_PREFIX                     "b"
#define BT_PREFIX                           "bt"

void test_suite_commands_timeout_callback(uintptr_t data)
{
    if (data != 0) {
        (void) uapi_timer_delete((timer_handle_t)data);
    }

    test_suite_console_set_color(TERM_COLOR_RED);
    test_suite_log_string("\r\n[TEST_FAILED : TIMED_OUT]");
    test_suite_console_set_color(TERM_COLOR_WHITE);
#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE
    test_suite_ipc_clear_core_flag();
#endif
}

static void test_suite_commands_unknown_command_reply(void)
{
    test_suite_log_set_test_result(TEST_SUITE_UNKNOWN_FUNCTION);
    test_suite_log_line("this is not a known command, try \"?\"");
}

static void test_suite_commands_unrecognized_reply(void)
{
    test_suite_log_set_test_result(TEST_SUITE_UNKNOWN_FUNCTION);
    test_suite_log_line("Command separator or terminator not recognized, try \"?\"");
}

static uint8_t test_suite_cmd_argument_get(char *argument, const char *string, char split_char)
{
    uint8_t i = 0;
    uint16_t index = 0;
    while (string[index] != 0) {
        if (string[index] == split_char || (i == TEST_SUITE_COMMANDS_ARGUMENT_SIZE - 1)) {
            break;  /* we have found the first element */
        }
        argument[index] = string[index];
        i++;
        index++;
    }
    argument[index] = '\0';
    return i;
}

static void test_suite_commands_execute_in_this_core(char *command, uint32_t timeout)
{
    static timer_handle_t timer = 0;
    if (timeout != 0) {
        uapi_timer_create(DEFAULT_TIMER, &timer);
        uapi_timer_start(timer, timeout * TEST_SUITE_TIMER_S_TO_US, test_suite_commands_timeout_callback, 0);
    }
    test_suite_function_execute_command(command);
    if (timeout != 0) {
        uapi_timer_stop(timer);
        uapi_timer_delete(timer);
    }
}

/*
 * -- Public function definitions
 */
static void test_suite_commands_process_cmd(char *command, char *first_argument, uint32_t timeout)
{
    if ((strcasecmp(first_argument, APPS_PREFIX) == 0) || (strcasecmp(first_argument, APPS_SHORT_PREFIX) == 0)) {
        test_suite_commands_execute_in_this_core(command, timeout);
#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE
    } else if ((strcasecmp(first_argument, BT_PREFIX) == 0) || (strcasecmp(first_argument, BT_SHORT_PREFIX) == 0)) {
        test_suite_commands_execute_in_external_core(CORES_BT_CORE, command, timeout);
#endif
    } else if ((strcasecmp("help", first_argument) == 0) || (strcasecmp("?", first_argument) == 0)) {
        test_suite_function_list_func();
#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE
        test_suite_commands_execute_in_external_core(CORES_BT_CORE, "list_functions", timeout);
#endif
    } else {
        /* any of the previous ones. Default reply. */
        test_suite_commands_unknown_command_reply();
    }
}

static bool test_suite_utils_is_numeric(char *s)
{
    bool numeric = true;
    char *string = s;

    if (*string == 0) {
        return false;
    }

    while (*string != 0) {
        if (!isdigit((int32_t)*string)) {
            numeric = false;
            break;
        }
        string++;
    }
    return numeric;
}

void test_suite_commands_prase(char *commands)
{
    char first_argument[TEST_SUITE_COMMANDS_ARGUMENT_SIZE] = { 0 };
    char second_argument[TEST_SUITE_COMMANDS_ARGUMENT_SIZE] = { 0 };
    uint8_t argument_length;
    uint32_t timeout = TEST_SUITE_DEFAULT_TIMEOUT;
    char *cmd_sep = TEST_SUITE_COMMAND_SEPARATOR;
    char *current_cmd;
    char *remain_cmd;

    current_cmd = strtok_s(commands, cmd_sep, &remain_cmd);
    if (current_cmd == NULL) {
        test_suite_commands_unrecognized_reply();
        return;
    }
    while (current_cmd != NULL) {
        /* Get the first argument */
        argument_length = test_suite_cmd_argument_get(first_argument, current_cmd, ' ');
        /* if it is empty just return */
        if (argument_length == 0) {
            return;  /* EARLY RETURN */
        }

        if ((uint8_t)strlen(current_cmd) >= argument_length + 1) {
            current_cmd += argument_length + 1;
            argument_length = test_suite_cmd_argument_get(second_argument, current_cmd, ' ');
            if (argument_length == 0) {
                return;  /* EARLY RETURN */
            }
        }

        if (test_suite_utils_is_numeric((char *)second_argument)) {
            timeout = (uint32_t)strtol(second_argument, NULL, TEST_SUITE_TIMEOUT_BASE);
            current_cmd += argument_length + 1;
        }

        test_suite_commands_process_cmd(current_cmd, (char *)first_argument, timeout);
        current_cmd = strtok_s(remain_cmd, cmd_sep, &remain_cmd);
    }
}
