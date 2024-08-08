/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common_def.h"
#include "ctype.h"
#include "securec.h"
#include "soc_osal.h"
#include "panic.h"
#include "test_suite_log.h"
#include "test_suite_errors.h"
#include "test_suite.h"
#include "test_suite_channel.h"
#include "test_suite_console.h"

/*
 * -- Configuration parameters
 */
#ifdef CONFIG_TEST_CONSOLE_HISTORY_LEN
#define TEST_CONSOLE_HISTORY_LEN CONFIG_TEST_CONSOLE_HISTORY_LEN
#else
#define TEST_CONSOLE_HISTORY_LEN 10  /* max number of lines to add to the console command history */
#endif

#define COLOR_S_LEN     6
#define CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE   300

#define test_suite_console_sendf(fmt, arg...) g_test_suite_console_channel_funcs->sendf(fmt, ##arg)

/*
 * -- Private variables
 */
static const char *g_logo[] = {
    "Welcome to Test Suite",
};

static char g_test_suite_console_history_commands[TEST_CONSOLE_HISTORY_LEN][CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE + 1];
static uint8_t g_test_suite_console_history_pos;
static uint8_t g_test_suite_console_history_begin;
static uint8_t g_test_suite_console_history_end;
static uint8_t g_h_index;
static uint8_t g_h_full;
static int8_t g_h_pos = 0;
static uint16_t g_cmd_len = 0;
static uint16_t g_cmd_pos = 0;
static char *g_test_suite_console_command_buffer;
static volatile bool g_consle_enable = false;
static test_suite_channel_funcs_t *g_test_suite_console_channel_funcs = NULL;

static void test_suite_console_history_add_current_command(void);
static uint8_t test_suite_console_history_put_previous_in_command_buffer(void);
static uint8_t test_suite_console_history_put_next_in_command_buffer(void);

/*
 * -- Cursor write
 */
static void test_suite_console_write_string(const char *s)
{
    g_test_suite_console_channel_funcs->send(s);
}

static void test_suite_console_write_char(const char c)
{
    g_test_suite_console_channel_funcs->send_char(c);
}

static void test_suite_console_write_line(const char *line)
{
    g_test_suite_console_channel_funcs->send_line(line);
}

static void test_suite_console_helper_repeated_write(char c, uint8_t times)
{
    uint8_t i;
    for (i = 0; i < times; i++) {
        test_suite_console_write_char(c);
    }
}

/*
 * -- console public funtions
 */
static void test_suite_console_cursor_left(void)
{
    if (g_cmd_pos > 0) {
        g_cmd_pos--;
        test_suite_console_write_char(CONSOLE_CHAR_BACKSPACE);
    }
}

static void test_suite_console_cursor_right(void)
{
    if (g_cmd_pos < g_cmd_len) {
        g_cmd_pos++;
        test_suite_console_write_char(CONSOLE_CHAR_ESC);
        test_suite_console_write_char('[');
        test_suite_console_write_char('C');
    }
}

static void test_suite_console_cursor_up(void)
{
    if (g_test_suite_console_history_begin != g_test_suite_console_history_end) {
        /* if there was something written, delete it */
        test_suite_console_helper_repeated_write(CONSOLE_CHAR_BACKSPACE, (uint8_t)g_cmd_pos);
        test_suite_console_helper_repeated_write(' ', (uint8_t)g_cmd_len);
        test_suite_console_helper_repeated_write(CONSOLE_CHAR_BACKSPACE, (uint8_t)g_cmd_len);
        g_cmd_len = test_suite_console_history_put_previous_in_command_buffer();
        test_suite_console_write_string((const char *)g_test_suite_console_command_buffer);
        g_cmd_pos = (uint8_t)g_cmd_len;
    }
}

static void test_suite_console_cursor_down(void)
{
    if (g_test_suite_console_history_begin != g_test_suite_console_history_end) {
        test_suite_console_helper_repeated_write(CONSOLE_CHAR_BACKSPACE, (uint8_t)g_cmd_pos);
        test_suite_console_helper_repeated_write(' ', (uint8_t)g_cmd_len);
        test_suite_console_helper_repeated_write(CONSOLE_CHAR_BACKSPACE, (uint8_t)g_cmd_len);
        g_cmd_len = test_suite_console_history_put_next_in_command_buffer();
        test_suite_console_write_string((const char *)g_test_suite_console_command_buffer);
        g_cmd_pos = g_cmd_len;
    }
}

static void test_suite_console_home(void)
{
    test_suite_console_helper_repeated_write(CONSOLE_CHAR_BACKSPACE, (uint8_t)g_cmd_pos);
    g_cmd_pos = 0;
}

static void test_suite_console_end(void)
{
    while (g_cmd_pos < g_cmd_len) {
        g_cmd_pos++;
        test_suite_console_write_char(CONSOLE_CHAR_ESC);
        test_suite_console_write_char('[');
        test_suite_console_write_char('C');
    }
}

static void test_suite_console_pageup(void)
{
    if (g_h_full != 0) {
        return;
    }

    if (g_h_pos < (g_h_index - 1)) {
        g_h_pos = (signed char)(g_h_index - 1);//lint !e734
        for (int i = 0; i < g_cmd_pos; i++) {
            test_suite_console_write_char(CONSOLE_CHAR_BACKSPACE);    // Rewind cursor
        }
        char *i = g_test_suite_console_history_commands[g_h_index - g_h_pos - 1];
        while ((*i) != 0) {
            test_suite_console_write_char(*i++);
        }
        int l = i - g_test_suite_console_history_commands[g_h_index - g_h_pos - 1];
        int n = g_cmd_pos - l;
        for (int j = 0; j < n; j++) { //lint !e578
            test_suite_console_write_char(' ');
        }
        for (int k = 0; k < n; k++) { //lint !e578
            test_suite_console_write_char(CONSOLE_CHAR_BACKSPACE);
        }
        //lint -e{732}
        g_cmd_pos = g_cmd_len = l; //lint !e734
    }
}

static void test_suite_console_pagedown(void)
{
    if (g_h_full != 0) {
        return;
    }

    if (g_h_pos >= 0) {
        g_h_pos = -1;
        for (int i = 0; i < g_cmd_pos; i++) {
            test_suite_console_write_char(CONSOLE_CHAR_BACKSPACE);    // Rewind cursor
        }
        char *i = g_test_suite_console_command_buffer;
        while ((*i) != 0) {
            test_suite_console_write_char(*i++);
        }
        int l = i - g_test_suite_console_command_buffer;
        int n = g_cmd_pos - l;
        for (int j = 0; j < n; j++) { //lint !e578
            test_suite_console_write_char(' ');
        }
        for (int k = 0; k < n; k++) { //lint !e578
            test_suite_console_write_char(CONSOLE_CHAR_BACKSPACE);
        }
        //lint -e{732}
        g_cmd_pos = g_cmd_len = l; //lint !e734
    }
}

/*
 * -- cursor init
 */
static void test_suite_console_get_channel(void)
{
    g_test_suite_console_channel_funcs = test_suite_channel_get_funcs();
}

static void test_suite_console_clear_screen(void)
{
    test_suite_console_write_char(CONSOLE_CHAR_ESC);
    test_suite_console_write_string(CONSOLE_COMMAND_CLEAR);

    /* ESC [ 1 1 H (move to 1,1) */
    test_suite_console_write_char(CONSOLE_CHAR_ESC);
    test_suite_console_write_string(CONSOLE_COMMAND_MOVE_1_1);
}

static void test_suite_console_print_logo(void)
{
    uint8_t i = 0;
    while (i < (sizeof(g_logo) / sizeof(g_logo[0]))) {
        test_suite_console_write_line(g_logo[i++]);
    }
}

static void test_suite_console_monitor_init(void)
{
    test_suite_console_clear_screen();
    test_suite_console_set_color(TERM_COLOR_RED);

    test_suite_console_print_logo();
}

void test_suite_console_init(void)
{
    test_suite_console_get_channel();
    test_suite_console_monitor_init();

    g_cmd_pos = 0;
    g_h_full = false;
    g_h_index = 0;
    g_cmd_len = 0;
    g_h_pos = -1;
    g_test_suite_console_history_begin = 0;
    g_test_suite_console_history_end = 0;
    g_test_suite_console_history_pos = TEST_CONSOLE_HISTORY_LEN;
}

void test_suite_console_register_command_buffer(char *buffer_to_register)
{
    g_test_suite_console_command_buffer = buffer_to_register;
}

static void test_suite_console_print_ps(void)
{
    test_suite_console_set_color(TERM_COLOR_BLUE);
    test_suite_log_string("\nEnter Command >>> ");
}

void test_suite_console_enable(void)
{
#ifndef SUPPORT_AUDIO_LIEYIN_TOOL
    test_suite_console_print_ps();
    g_cmd_pos = 0;
    g_test_suite_console_command_buffer[g_cmd_pos] = '\0';
#endif
    g_cmd_len = 0;
    g_consle_enable = true;
}

void test_suite_console_disable(void)
{
    g_consle_enable = false;
    test_suite_console_write_line("\n");
}

bool test_suite_console_is_enabled(void)
{
    return g_consle_enable;
}

static inline uint8_t test_suite_console_history_helper_get_next_index(uint8_t index)
{
    uint8_t next_index = index;
    next_index++;
    if (next_index >= TEST_CONSOLE_HISTORY_LEN) {
        next_index = 0;
    }
    return next_index;
}

static inline uint8_t test_suite_console_history_helper_get_previous_index(uint8_t index)
{
    uint8_t pre_index = index;
    if (pre_index == 0) {
        pre_index = TEST_CONSOLE_HISTORY_LEN - 1;
    } else {
        pre_index--;
    }
    return pre_index;
}

/*
 * -- Test suite add history command
 */
static void test_suite_console_history_add_current_command(void)
{
    uint8_t last_command_index;
    bool put_in_history = true;
    int32_t ret;
    /* if there is something on the history */
    if (g_test_suite_console_history_end != g_test_suite_console_history_begin) {
        /* check last command executed */
        last_command_index = test_suite_console_history_helper_get_previous_index(g_test_suite_console_history_end);
        if (strcmp((char *)g_test_suite_console_history_commands[last_command_index],
            (char *)g_test_suite_console_command_buffer) == 0) {
            put_in_history = false;
        }
    }
    if (put_in_history) {
        ret = strcpy_s((char *)g_test_suite_console_history_commands[g_test_suite_console_history_end],
                       CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE + 1, (char *)g_test_suite_console_command_buffer);
        if (ret != EOK) {
            return;
        }
        g_test_suite_console_history_end =
            test_suite_console_history_helper_get_next_index(g_test_suite_console_history_end);
        if (g_test_suite_console_history_end == g_test_suite_console_history_begin) {
            g_test_suite_console_history_begin =
                test_suite_console_history_helper_get_next_index(g_test_suite_console_history_begin);
        }
    }
    /* reset the history position to the "not in use" value */
    g_test_suite_console_history_pos = TEST_CONSOLE_HISTORY_LEN;
}

static uint8_t test_suite_console_history_put_previous_in_command_buffer(void)
{
    int32_t ret;
    /* Move the position index only if it is not at the beginning
     * write the command in the position index
     * check it has been called with something in the buffer history */
    if (g_test_suite_console_history_end == g_test_suite_console_history_begin) {
        panic(PANIC_TESTSUIT, __LINE__);
    }
    /* if it is the first time being called set position to the end */
    if (g_test_suite_console_history_pos == TEST_CONSOLE_HISTORY_LEN) {
        g_test_suite_console_history_pos =
            test_suite_console_history_helper_get_previous_index(g_test_suite_console_history_end);
    /* otherwise if we have not reached the first command decrement the position */
    } else if (g_test_suite_console_history_pos != g_test_suite_console_history_begin) {
        g_test_suite_console_history_pos =
            test_suite_console_history_helper_get_previous_index(g_test_suite_console_history_pos);
    }
    /* copy it into the buffer */
    ret = strcpy_s(g_test_suite_console_command_buffer, CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE + 1,
                   g_test_suite_console_history_commands[g_test_suite_console_history_pos]);
    if (ret != EOK) {
        return 0;
    }
    return (uint8_t)strlen(g_test_suite_console_command_buffer);
}

static uint8_t test_suite_console_history_put_next_in_command_buffer(void)
{
    uint8_t next;
    int32_t ret;
    /* check it has been called with something in the buffer history */
    if (g_test_suite_console_history_end == g_test_suite_console_history_begin) {
        panic(PANIC_TESTSUIT, __LINE__);
    }
    /* if it is the first time being called set position to the end */
    if (g_test_suite_console_history_pos == TEST_CONSOLE_HISTORY_LEN) {
        g_test_suite_console_history_pos =
            test_suite_console_history_helper_get_previous_index(g_test_suite_console_history_end);
    }
    next = test_suite_console_history_helper_get_next_index(g_test_suite_console_history_pos);
    /* otherwise if we have not reached the first command decrement the position */
    if (next != g_test_suite_console_history_end) {
        g_test_suite_console_history_pos = next;
    }
    /* copy it into the buffer */
    ret = strcpy_s(g_test_suite_console_command_buffer, CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE + 1,
                   g_test_suite_console_history_commands[g_test_suite_console_history_pos]);
    if (ret != EOK) {
        return 0;
    }
    return (uint8_t)strlen(g_test_suite_console_command_buffer);
}

static void test_suite_console_backspace_char(char c)
{
    if (g_h_pos >= 0) {
        if (strcpy_s(g_test_suite_console_command_buffer, CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE + 1,
                     g_test_suite_console_history_commands[g_h_index - g_h_pos - 1]) != EOK) {
            return;
        }
        g_h_pos = -1;
    }
    if (g_cmd_pos < g_cmd_len) {
        /* Delete char: move chars from g_cmd_pos+ to the left */
        test_suite_console_write_char(c);
        for (uint8_t i = (uint8_t)g_cmd_pos; i < g_cmd_len; i++) {
            test_suite_console_write_char(g_test_suite_console_command_buffer[i]);
            g_test_suite_console_command_buffer[i - 1] = g_test_suite_console_command_buffer[i];
        }
        test_suite_console_write_char(' ');
        for (uint8_t i = (uint8_t)g_cmd_pos; i < g_cmd_len; i++) {
            test_suite_console_write_char(CONSOLE_CHAR_BACKSPACE);  /* cursor back */
        }
        --g_cmd_pos;
    } else {
        test_suite_console_write_char(c);
        test_suite_console_write_char(' ');
        g_test_suite_console_command_buffer[--g_cmd_pos] = '\0';
    }
    test_suite_console_write_char(c);
    --g_cmd_len;
}

static void test_suite_console_delete_char(void)
{
    if (g_h_pos >= 0) {
        if (strcpy_s(g_test_suite_console_command_buffer, CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE + 1,
                     g_test_suite_console_history_commands[g_h_index - g_h_pos - 1]) != EOK) {
            return;
        }
        g_h_pos = -1;
    }
    /* Delete char: move chars from g_cmd_pos+ to the left */
    for (uint8_t i = (uint8_t)g_cmd_pos; i < g_cmd_len - 1; i++) {
        g_test_suite_console_command_buffer[i] = g_test_suite_console_command_buffer[i + 1];
        test_suite_console_write_char(g_test_suite_console_command_buffer[i]);
    }
    test_suite_console_write_char(' ');
    for (uint8_t i = (uint8_t)g_cmd_pos; i < g_cmd_len; i++) {
        test_suite_console_write_char(CONSOLE_CHAR_BACKSPACE);  /* cursor back */
    }
    g_cmd_len--;
}

static void test_suite_console_enter_char(void)
{
    g_test_suite_console_command_buffer[g_cmd_len] = '\0';  /* NULL terminate it */
    if (g_cmd_len > 0) {
        test_suite_console_history_add_current_command();
    }
    test_suite_console_disable();
}

static void test_suite_console_save_char(char c)
{
    if (g_cmd_pos == 0 && c == ' ') {
        return;  /* Eat leading spaces */
    }

    if (g_h_pos >= 0) {
        if (strcpy_s(g_test_suite_console_command_buffer, CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE + 1,
                     g_test_suite_console_history_commands[g_h_index - g_h_pos - 1]) != EOK) {
            return;
        }
        g_h_pos = -1;
    }

    if (g_cmd_len < CONFIG_TEST_SUITE_COMMAND_BUFFER_SIZE) {
        if (g_cmd_pos < g_cmd_len) {
            /* Insert char: move chars from g_cmd_pos to the right */
            for (int32_t i = g_cmd_len - 1; i >= g_cmd_pos; i--) {
                g_test_suite_console_command_buffer[i + 1] = g_test_suite_console_command_buffer[i];
            }
            g_test_suite_console_command_buffer[g_cmd_pos] = c;
            for (uint8_t i = (uint8_t)g_cmd_pos; i <= g_cmd_len; i++) {
                test_suite_console_write_char(g_test_suite_console_command_buffer[i]);
            }
            for (uint8_t i = (uint8_t)g_cmd_pos; i < g_cmd_len; i++) {
                test_suite_console_write_char(CONSOLE_CHAR_BACKSPACE);  /* cursor back */
            }
        } else {
            g_test_suite_console_command_buffer[g_cmd_pos] = c;
            test_suite_console_write_char(c);
        }
        g_cmd_len++;
        g_cmd_pos++;
    }
}

/**
 * Test suite process csi char from console
 * @param c The char to process.
 * @return true Need do more process or false no need do more process;
 */
static bool test_suite_console_process_char_csi(char c, char *escape_code)
{
    switch (c) {
        case CONSOLE_CHAR_CSI:
            break;
        case CONSOLE_CHAR_LEFT:
            test_suite_console_cursor_left();
            *escape_code = 0;
            break;
        case CONSOLE_CHAR_RIGHT:
            test_suite_console_cursor_right();
            *escape_code = 0;
            break;
        case CONSOLE_CHAR_UP:
            test_suite_console_cursor_up();
            *escape_code = 0;
            break;
        case CONSOLE_CHAR_DOWN:
            test_suite_console_cursor_down();
            *escape_code = 0;
            break;
        case CONSOLE_CHAR_PAGE_HOME:
        case CONSOLE_CHAR_PAGE_END:
        case CONSOLE_CHAR_PAGE_UP:
        case CONSOLE_CHAR_PAGE_DOWN:
            *escape_code = c;
            break;
        default:
            return true;
    }
    return false;
}

/**
 * Test suite process 7e char from console
 * @param c The char to process.
 * @return true Need do more process or false no need do more process;
 */
static bool test_suite_console_process_char_7e(char *escape_code)
{
    switch (*escape_code) {
        case CONSOLE_CHAR_PAGE_HOME:
            test_suite_console_home();
            *escape_code = 0;
            break;
        case CONSOLE_CHAR_PAGE_END:
            test_suite_console_end();
            *escape_code = 0;
            break;
        case CONSOLE_CHAR_PAGE_UP:
            test_suite_console_pageup();
            *escape_code = 0;
            break;
        case CONSOLE_CHAR_PAGE_DOWN:
            test_suite_console_pagedown();
            *escape_code = 0;
            break;
        default:
            return true;
    }
    return false;
}

void test_suite_console_process_char(char c)
{
    static char escape_code = 0;

    /* if the console is disabled just discard the data */
    if (!test_suite_console_is_enabled()) { return; }

    /* --------------- *
     * left   = [0x1b][0x5b][0x44]
     * right  = [0x1b][0x5b][0x43]
     * up     = [0x1b][0x5b][0x41]
     * down   = [0x1b][0x5b][0x42]
     * home   = [0x1b][0x5b][0x31][0x7e]
     * end    = [0x1b][0x5b][0x34][0x7e]
     * pg up  = [0x1b][0x5b][0x35][0x7e]
     * pg dn  = [0x1b][0x5b][0x36][0x7e]
     * tab    = [0x09]
     * dtab   = [0x09][0x09]
     * cancel = [0x03]
     * reset  = [0x12]
     * --------------- */
    if (escape_code != 0) {
        if (escape_code == CONSOLE_CHAR_ESC && (c == CONSOLE_CHAR_CSI || c == CONSOLE_CHAR_NONE)) {
            escape_code = c;
            return;
        } else if (escape_code == CONSOLE_CHAR_CSI) {
            if (!test_suite_console_process_char_csi(c, &escape_code)) {
                return;
            }
        } else if (c == CONSOLE_CHAR_PAGE_7E) {
            if (!test_suite_console_process_char_7e(&escape_code)) {
                return;
            }
        }

        /* process cancel */
        escape_code = 0;
    }

    if (isprint((int32_t)c)) {
        test_suite_console_save_char(c);
    } else if (c == CONSOLE_CHAR_BACKSPACE && (g_cmd_pos > 0)) {
        test_suite_console_backspace_char(c);
    } else if (c == CONSOLE_CHAR_DELETE && g_cmd_pos < g_cmd_len) {
        test_suite_console_delete_char();
    } else if (c == CONSOLE_CHAR_ENTER) {
        test_suite_console_enter_char();
    } else if (c == CONSOLE_CHAR_ESC) {
        escape_code = c;
    }
}

void test_suite_console_set_color(term_color_t color)
{
    static term_color_t current_color = TERM_COLOR_RESET;
    char s[COLOR_S_LEN] = "\x1b[  m";
    if (color != current_color) {
        /* ESC [ termColor m */
        s[2] = (char)((int8_t)'0' + ((int8_t)color / 10)); /* (color / 10) Convert to characters reserved in s[2] */
        s[3] = (char)((int8_t)'0' + ((int8_t)color % 10)); /* (color % 10) Convert to characters reserved in s[3] */
        test_suite_console_write_string((const char *)s);
        current_color = color;
    }
}

void test_suite_console_display_test_status(int status)
{
    if (status < 0) { /* Error codes in TEST_SUITE_ERRORS */
        test_suite_console_set_color(TERM_COLOR_RED);
        test_suite_console_write_string("[TEST_FAILED : ");
        if (status == TEST_SUITE_ERROR_TIMED_OUT) {
            test_suite_console_write_string("MAXIMUM_TIMEOUT_EXCEEDED]");
        } else if (status == TEST_SUITE_UNKNOWN_FUNCTION) {
            test_suite_console_write_string("UNKNOWN_FUNCTION]");
        } else {
            test_suite_console_write_string("ERROR_CODE_RETURNED]");
        }
        test_suite_console_write_string(" [ERROR = ");
        test_suite_console_sendf("0x%x]\n", status);
    } else if (status == 0) { /* OK in TEST_SUITE_ERRORS */
        test_suite_console_set_color(TERM_COLOR_GREEN);
        test_suite_console_write_line("[TEST_PASSED!]");
    } else {
        if (status == 1) {
            test_suite_console_set_color(TERM_COLOR_GREEN);
            test_suite_console_write_line("[CONTROL RETURNED]");
        }
    }
    test_suite_console_set_color(TERM_COLOR_WHITE);
}
