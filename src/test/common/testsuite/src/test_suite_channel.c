/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#include "common_def.h"
#include "test_suite_channel.h"

static test_suite_channel_funcs_t *g_test_suite_channel_funcs = NULL;

static void test_suite_channel_register_funcs(test_suite_channel_funcs_t *funcs)
{
    if (funcs == NULL) {
        return;
    }
    g_test_suite_channel_funcs = funcs;
}

static void test_suite_channel_unregister_funcs(void)
{
    g_test_suite_channel_funcs = NULL;
}

void test_suite_channel_init(test_suite_channel_funcs_t *funcs)
{
    test_suite_channel_register_funcs(funcs);
    if (g_test_suite_channel_funcs == NULL) {
        return;
    }
    g_test_suite_channel_funcs->init();
}

void test_suite_channel_deinit(void)
{
    if (g_test_suite_channel_funcs == NULL) {
        return;
    }
    g_test_suite_channel_funcs->deinit();
    test_suite_channel_unregister_funcs();
}

test_suite_channel_funcs_t *test_suite_channel_get_funcs(void)
{
    return g_test_suite_channel_funcs;
}
