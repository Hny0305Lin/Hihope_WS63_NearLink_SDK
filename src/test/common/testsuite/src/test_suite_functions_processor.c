/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#include <stdint.h>
#include <string.h>
#include "securec.h"
#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE
#include "debug_print.h"
#include "test_suite_ipc.h"
#endif
#include "test_suite_log.h"
#include "test_suite_console.h"
#include "test_suite_errors.h"
#include "test_suite.h"
#include "test_suite_functions_processor.h"

#define MIN_DOT_BUFFER_NUMBER 10

/* Maximum number of functions */
#ifdef CONFIG_TEST_SUITE_FUNCTIONS_MAX_FUNCS
#define TEST_SUITE_FUNCTIONS_MAX_FUNCS CONFIG_TEST_SUITE_FUNCTIONS_MAX_FUNCS
#else
#define TEST_SUITE_FUNCTIONS_MAX_FUNCS 400
#endif
/* Maximum number of arguments */
#define TEST_SUITE_FUNCTIONS_MAX_ARGS 20

typedef struct {
    char *func_name;
    char *func_description;
    test_suite_function_callback_t func;
} test_suite_functions_t;

static uint32_t g_function_count = 0;
static test_suite_functions_t g_test_suite_functions_known_funcs[TEST_SUITE_FUNCTIONS_MAX_FUNCS];
static int32_t g_test_suite_functions_argc;
static char *g_test_suite_functions_argv[TEST_SUITE_FUNCTIONS_MAX_ARGS];

static char cmd_helper_split(char *result[], char res_len, char *string, char split_char)
{
    /* replace split_char with '\0' and return the positions of the start of the strings */
    int32_t i = 0;
    bool new = true;
    char *char_buf = string;

    while (*char_buf != 0) {
        if (new) {
            result[i] = char_buf;
            if (++i >= res_len) {
                return (char)(i - 1);
            }
            new = false;
        }
        if (*char_buf == split_char) {
            *char_buf = '\0';
            new = true;
        }
        char_buf++;
    }
    return (char)i;
}

static void test_suite_function_unknown_func_reply(void)
{
    test_suite_log_line("this is not a known function, try ? to list registered function");
}

static void test_suite_function_execute_func(char argc, char *argv[])
{
    uint16_t function_to_execute;
    uint16_t i;
    int test_status;
    char func_argc = argc;
    char **func_argv = argv;

    function_to_execute = (uint16_t)g_function_count;  /* set by default a wrong value */

    for (i = 0; i < g_function_count; i++) {
        if (strcmp(g_test_suite_functions_known_funcs[i].func_name, *argv) == 0) {
            function_to_execute = i;
            func_argc--;
            func_argv++;
            break;
        }
    }

    if (function_to_execute >= (signed)g_function_count) {
        test_suite_function_unknown_func_reply();
        test_status = TEST_SUITE_UNKNOWN_FUNCTION;
        test_suite_log_set_test_result(test_status);
#if (CORE == TEST_SUITE_CORE_IMPLEMENTING_THE_CONSOLE)
        test_suite_console_display_test_status(test_status);
#else
        test_suite_log_send_test_status(test_status);
#endif
        return;
    }
    test_suite_log_string("Starting function ");
    test_suite_log_line(g_test_suite_functions_known_funcs[function_to_execute].func_name);

    test_status = g_test_suite_functions_known_funcs[function_to_execute].func(func_argc, func_argv);
    test_suite_log_set_test_result(test_status);
#if (CORE == TEST_SUITE_CORE_IMPLEMENTING_THE_CONSOLE)
        test_suite_console_display_test_status(test_status);
#else
        test_suite_log_send_test_status(test_status);
#endif
}

errcode_t test_suite_function_add_func(char *name, char *description, test_suite_function_callback_t func)
{
    if (g_function_count + 1 >= TEST_SUITE_FUNCTIONS_MAX_FUNCS) {
        return ERRCODE_FAIL;
    }
    g_test_suite_functions_known_funcs[g_function_count].func_name = name;
    g_test_suite_functions_known_funcs[g_function_count].func_description = description;
    g_test_suite_functions_known_funcs[g_function_count].func = func;
    g_function_count++;
    return ERRCODE_SUCC;
}

void test_suite_function_init(void)
{
    g_function_count = 0;
    (void)memset_s(g_test_suite_functions_known_funcs, sizeof(test_suite_functions_t) * \
                   TEST_SUITE_FUNCTIONS_MAX_FUNCS, 0, sizeof(test_suite_functions_t) * \
                   TEST_SUITE_FUNCTIONS_MAX_FUNCS);
}

void test_suite_function_deinit(void)
{
    g_function_count = 0;
    (void)memset_s(g_test_suite_functions_known_funcs, sizeof(test_suite_functions_t) * \
                   TEST_SUITE_FUNCTIONS_MAX_FUNCS, 0, sizeof(test_suite_functions_t) * \
                   TEST_SUITE_FUNCTIONS_MAX_FUNCS);
}

void test_suite_function_list_func(void)
{
    uint32_t strlen_max = 0;
    uint32_t l, i, j, c;

    for (i = 0; i < g_function_count; i++) {
        l = (uint32_t)strlen((char *)g_test_suite_functions_known_funcs[i].func_name);
        if (l > strlen_max) {
            strlen_max = l;
        }
    }
    for (i = 0; i < g_function_count; i++) {
        test_suite_log_string(g_test_suite_functions_known_funcs[i].func_name);
        c = (uint32_t)(strlen_max + MIN_DOT_BUFFER_NUMBER -
                       strlen((char *)g_test_suite_functions_known_funcs[i].func_name));
        test_suite_log_string(" ");
        for (j = 1; j < c; j++) {
            test_suite_log_string(".");
        }
        test_suite_log_line(g_test_suite_functions_known_funcs[i].func_description);
    }
    return;
}

void test_suite_function_execute_command(char *command)
{
    /* initialise the command array to zeros. */
    for (int32_t i = 0; i < TEST_SUITE_FUNCTIONS_MAX_ARGS; i++) {
        g_test_suite_functions_argv[i] = 0;
    }
    g_test_suite_functions_argc = cmd_helper_split(g_test_suite_functions_argv, TEST_SUITE_FUNCTIONS_MAX_ARGS,
                                                   command, ' ');
    test_suite_function_execute_func((char)g_test_suite_functions_argc, g_test_suite_functions_argv);
}
