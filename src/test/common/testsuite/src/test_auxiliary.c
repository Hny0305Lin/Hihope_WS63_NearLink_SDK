/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#include "test_auxiliary.h"
#include <stdlib.h>
#include "panic.h"
#include "common_def.h"
#include "test_suite_log.h"
#include "test_suite.h"
#include "test_suite_errors.h"
#include "test_suite_functions_processor.h"

#define LOG_32ASHEX_VALUE_H_SHIFT  16
#define LOG_16ASHEX_VALUE_H_SHIFT  8
#define LOG_8ASHEX_VALUE_H_SHIFT   4
#define LOG_OUTPUT_NUM             16

static void log_uint_8_as_hex(uint8_t value)
{
    int8_t table[LOG_OUTPUT_NUM] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    uint8_t a = (value >> LOG_8ASHEX_VALUE_H_SHIFT) & 0x0F;
    uint8_t b = value & 0x0F;

    test_suite_log_char(table[a]);
    test_suite_log_char(table[b]);
}

static void log_uint_16_as_hex(uint16_t value)
{
    log_uint_8_as_hex((uint8_t)((value & 0xFF00) >> LOG_16ASHEX_VALUE_H_SHIFT));
    log_uint_8_as_hex((uint8_t)(value & 0x00FF));
}

static void log_uint_32_as_hex(uint32_t value)
{
    log_uint_16_as_hex((uint16_t)((value & 0xFFFF0000) >> LOG_32ASHEX_VALUE_H_SHIFT));
    log_uint_16_as_hex((uint16_t)(value & 0x0000FFFF));
}

static void log_mem_32_as_hex(uint32_t *src_p, uint16_t length, uint32_t addr)
{
    uint16_t i = 0;

    test_suite_log_stringf("%08x: ", addr);
    while (i < length) {
        log_uint_32_as_hex(src_p[i]);
        i++;

        if (i % 4 == 0) {  /* line breaks if uart outputs 4 * 8 characters */
            test_suite_log_char('\r');
            test_suite_log_char('\n');

            if (i < length) {
                /* Print the address every 4 * 8 characters */
                test_suite_log_stringf("%08x: ", addr + (i * 4));
            }
        } else {
            test_suite_log_char(' ');
        }
    }

    test_suite_log_char('\r');
    test_suite_log_char('\n');
}

static int echo_parameter_function(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++) {
        if (i > 0) {
            test_suite_log_string(" ");
        }

        test_suite_log_string(argv[i]);
    }
    test_suite_log_string("\r\n");
    return TEST_SUITE_OK;
}

static int test_auxiliary_infinite_loop(int argc, char *argv[])
{
    unused(argc);
    unused(argv);
    while (1) { //lint !e716
        panic(PANIC_TESTSUIT, __LINE__);
    }; //lint !e527
    return TEST_SUITE_OK; //lint !e527
}

static int test_auxiliary_read_mem32(int argc, char *argv[])
{
    uint32_t *addr = NULL;
    uint16_t ne;

    if (argc < TEST_PARAM_ARGC_1) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    addr = (uint32_t *)strtol(argv[0], NULL, 0);
    if (argc >= TEST_PARAM_ARGC_2) {
        ne = (uint16_t)strtol(argv[1], NULL, 0);
    } else {
        ne = 0;
    }

    test_suite_log_stringf("Reading %d bytes from 0x%08x\r\n", ne, addr);
    log_mem_32_as_hex(addr, ne, (uint32_t)addr);
    return TEST_SUITE_OK;
}

static int test_auxiliary_write_mem32(int argc, char *argv[])
{
    if (argc != TEST_PARAM_ARGC_2) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }

    uint32_t addr = (uint32_t)strtol(argv[0], NULL, 0);
    uint32_t val = (uint32_t)strtol(argv[1], NULL, 0);

    *(volatile uint32_t *)(uintptr_t)(addr) = (uint32_t)(val);
    test_suite_log_stringf("Write addr %8#x to %8#x\r\n", addr, *(volatile uint32_t *)(uintptr_t)(addr));
    return TEST_SUITE_OK;
}

void add_auxiliary_functions(void)
{
    uapi_test_suite_add_function("list_functions", "Lists all the known functions",
                                 (test_suite_function_callback_t)test_suite_function_list_func);
    uapi_test_suite_add_function(
        "echo", "Echos back the parameter you pass into this function", echo_parameter_function);
    uapi_test_suite_add_function("infinite_loop", "Goes to an infinite loop", test_auxiliary_infinite_loop);
    uapi_test_suite_add_function(
        "mem32", "Read 32-bit items. Params: <Addr>, <NumItems> (hex or decimal)", test_auxiliary_read_mem32);
    uapi_test_suite_add_function(
        "w4", "Write 32-bit items. Params: <Addr>, <Value> (hex or decimal)", test_auxiliary_write_mem32);
}
