/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#include <stdio.h>
#include <stdarg.h>
#include "securec.h"
#include "common_def.h"
#include "test_suite_channel.h"
#include "test_suite_log.h"

#define LOG_STRING_MAX_LENGTH 100

static test_suite_channel_funcs_t *g_test_suite_log_channel_funcs = NULL;
static int32_t g_test_suite_result;

void test_suite_log_get_channel_funcs(void)
{
    g_test_suite_log_channel_funcs = test_suite_channel_get_funcs();
}

void test_suite_log_char(char data)
{
    g_test_suite_log_channel_funcs->send_char(data);
}

void test_suite_log_string(const char *str)
{
    g_test_suite_log_channel_funcs->send(str);
}

void test_suite_log_stringf(const char *str, ...)
{
    static char s[LOG_STRING_MAX_LENGTH];
    va_list args;
    int32_t str_len;

    va_start(args, str);
    str_len = vsprintf_s(s, sizeof(s), str, args);
    va_end(args);

    if (str_len < 0) {
        return;
    }

    test_suite_log_string(s);
}

void test_suite_log_line(char *line)
{
    g_test_suite_log_channel_funcs->send_line(line);
}

void test_suite_log_set_test_result(int result)
{
    g_test_suite_result = result;
}

int test_suite_log_get_test_result(void)
{
    return g_test_suite_result;
}
