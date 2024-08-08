/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE
#include "ipc.h"
#include "timer.h"
#include "securec.h"
#include "stdarg.h"
#include "osal_addr.h"
#include "debug_print.h"
#include "test_suite_console.h"
#ifndef CONFIG_TESTSUITE_SUPPORT_PRINT_USE_IPC
#include "test_suite_uart.h"
#endif
#include "test_suite_channel.h"
#include "test_suite_commands_processor.h"
#include "test_suite_task.h"
#include "test_suite_ipc.h"

#define MIN_DOT_BUFFER_NUMBER           10

#define TEST_SUITE_UART_LEN_MAX         512
#ifndef CONFIG_TESTSUITE_SUPPORT_PRINT_USE_IPC
#define TEST_SUITE_UART_RX_BUFFER_SIZE  128
#endif

#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE
/*
 * -- Defines
 */
#define core_flag(x)                    (1 << (x))

/*
 * -- Types
 */
typedef struct {
    bool inuse;
    uint16_t length;
    char msg[IPC_TEST_SUITE_STRING_LENTGH + 1]; /* Include trailing \0 */
    char status;
} feed_back_buffer;
#endif

static char g_test_suite_log_buffer[IPC_TEST_SUITE_STRING_LENTGH];

static uint8_t g_test_suite_log_buffer_pos = 0;
static bool g_test_suite_log_buffer_cr = false;
static bool g_test_suite_log_buffer_nl = false;
void test_suite_log_helper_char(char data);
static void g_test_suite_log_reset_buffer(void);
bool g_test_suite_log_buffer_write_char(char data);

#ifndef CONFIG_TESTSUITE_SUPPORT_PRINT_USE_IPC
static uart_bus_t g_test_suite_uart = TEST_SUITE_UART_BUS;
static uint8_t g_test_suite_uart_rx_buffer_test[TEST_SUITE_UART_RX_BUFFER_SIZE];
#endif

static char g_test_suite_function_processor_command_buffer[TEST_SUITE_PRIVATE_COMMAND_BUFFER_SIZE + 1];

#ifdef CONFIG_TESTSUITE_SUPPORT_MULTI_CORE
static volatile feed_back_buffer g_response[CORES_MAX_NUMBER_PHYSICAL];
static volatile uint8_t g_echo_cores;
static volatile uint8_t g_unexpected_log; /* uses the same flags as g_echo_cores */
/** Color codes for security radio and apps */
static term_color_t g_term_colors[CORES_MAX_NUMBER_PHYSICAL] = { TERM_COLOR_PINK, TERM_COLOR_CYAN, TERM_COLOR_WHITE };
#endif

void test_suite_log_send_test_status(int status)
{
    /* Use IPC to send to CORES_CORE_IMPLEMENTING_THE_CONSOLE core! */
    ipc_payload payload;
    if (memset_s((void *)&payload, sizeof(ipc_payload), 0, sizeof(ipc_payload)) != EOK) {
        return;
    }

    payload.testsuite.length = 0;
    payload.testsuite.status = status;

    /* Horrible... Waiting for ever!  At least not in interrupt context! */
    while (true) {
        if (ipc_send_message (CORES_CORE_IMPLEMENTING_THE_CONSOLE,
            IPC_ACTION_TS_MESSAGE_CNF, &payload, payload.testsuite.length + 0x4,
            IPC_PRIORITY_LOWEST, false) != IPC_STATUS_MAILBOX_BUSY) {
            break;
        }
    }

    /* Reset test_suite_log_buffer. */
    g_test_suite_log_buffer_pos = 0;
}
#ifndef CONFIG_TESTSUITE_SUPPORT_PRINT_USE_IPC
static void test_suite_uart_write(const char *buffer, uint16_t length)
{
#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
    if (g_test_uart_mode == TEST_USB_SERIAL) {
        usb_serial_write(0, buffer, length);
    } else {
        uapi_uart_write(TEST_SUITE_UART_BUS, (uint8_t *)buffer, (uint32_t)length, 0);
    }
#else
    uapi_uart_write(TEST_SUITE_UART_BUS, (uint8_t *)buffer, (uint32_t)length, 0);
#endif
}
#endif

static void test_suite_log_send_msg(void)
{
#ifdef CONFIG_TESTSUITE_SUPPORT_PRINT_USE_IPC
    /* Use IPC to send to the core implementing the console! */
    ipc_payload payload;
    if (memset_s((void *)&payload, sizeof(ipc_payload), 0, sizeof(ipc_payload)) != EOK) {
        return;
    }

    payload.testsuite.length = g_test_suite_log_buffer_pos;
    if (memcpy_s((void *)payload.testsuite.msg, payload.testsuite.length,
                 g_test_suite_log_buffer, g_test_suite_log_buffer_pos) != EOK) {
        return;
    }

    payload.testsuite.status = CMD_TEST_RUNNING;

    /* Horrible... Waiting for ever!  At least not in interrupt context! */
    while (true) {
        if (ipc_send_message (CORES_CORE_IMPLEMENTING_THE_CONSOLE,
            IPC_ACTION_TS_MESSAGE_CNF, &payload, payload.testsuite.length,
            IPC_PRIORITY_LOWEST, false) != IPC_STATUS_MAILBOX_BUSY) {
            break;
        }
    }
#else
    test_suite_uart_write(g_test_suite_log_buffer, g_test_suite_log_buffer_pos);
#endif
    g_test_suite_log_reset_buffer();
}

void test_suite_log_flush(void)
{
    /* If there is something to transmit do it */
    if (g_test_suite_log_buffer_pos > 0) {
        test_suite_log_send_msg();
    }

    return;
}

bool g_test_suite_log_buffer_write_char(char data)
{
    g_test_suite_log_buffer[g_test_suite_log_buffer_pos++] = data;

    if (data == '\r') {
        g_test_suite_log_buffer_cr = true;
    }
    if (data == '\n') {
        g_test_suite_log_buffer_cr = true; /* Change to make it more unix compatible */
        g_test_suite_log_buffer_nl = true;
    }

    return ((g_test_suite_log_buffer_pos >= IPC_TEST_SUITE_STRING_LENTGH) ||
            (g_test_suite_log_buffer_cr && g_test_suite_log_buffer_nl));
}

void g_test_suite_log_reset_buffer(void)
{
    /* Reset test_suite_log_buffer. */
    g_test_suite_log_buffer_pos = 0;
    g_test_suite_log_buffer_cr = false;
    g_test_suite_log_buffer_nl = false;
}

void test_suite_log_helper_char(char data)
{
    bool terminate;
    terminate = g_test_suite_log_buffer_write_char(data);
    /* Ensure \0 terminated by leaving additional space! */
    if (terminate == true) {
        test_suite_log_send_msg();
    }

    return;
}

void test_suite_log_helper_send(const char *str)
{
    for (uint32_t i = 0; i < strlen(str); i++) {
        test_suite_log_helper_char(str[i]);
    }
    test_suite_log_flush();
}

void test_suite_log_helper_sendf(const char *str, ...)
{
    static char s[TEST_SUITE_UART_LEN_MAX];  /* This needs to be large enough to store the string */
    int32_t str_len;

    va_list args;
    va_start(args, str);
    str_len = vsprintf_s(s, sizeof(s), str, args);
    va_end(args);

    if (str_len < 0) {
        return;
    }

    test_suite_log_helper_send(s);
    test_suite_log_flush();
}

void test_suite_log_helper_send_line(const char *str)
{
    test_suite_log_helper_send(str);
    test_suite_log_helper_send("\r\n");
    test_suite_log_flush();
}

/**
 * From security interrupt line
 */
static bool security_message_handler(ipc_action_t message,
    const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    if (message != IPC_ACTION_TS_MESSAGE_REQ) {
        PRINT("received action is invalid : %d.\r\n", message);
        return false;
    }
    if (src != CORES_CORE_IMPLEMENTING_THE_CONSOLE) {
        PRINT("received src is invalid : %d.\r\n", src);
        return false;
    }
    unused(id);
    test_suite_write_msgqueue((uint8_t *)payload_p->testsuite.msg, payload_p->testsuite.length);
    /* Message fully processed. */
    return true;
}

void test_suite_ipc_init(void)
{
#ifndef CONFIG_TESTSUITE_SUPPORT_PRINT_USE_IPC
    uart_buffer_config_t uart_buffer_config;

    uart_pin_config_t uart_pin_config = {
        .tx_pin = TEST_SUITE_UART_TX_PIN,
        .rx_pin = TEST_SUITE_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    uart_attr_t uart_line_config = {
        .baud_rate = TEST_SUITE_UART_BAUD_RATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    uart_buffer_config.rx_buffer_size = TEST_SUITE_UART_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_test_suite_uart_rx_buffer_test;
    (void)uapi_uart_init(TEST_SUITE_UART_BUS, &uart_pin_config, &uart_line_config, NULL, &uart_buffer_config);
#endif
    /* Register IRQ callback for IPC... */
    /* Should have been done earlier really, but hey... */
    ipc_register_handler(IPC_ACTION_TS_MESSAGE_REQ, security_message_handler);
}

void test_suite_ipc_deinit(void)
{
}

/**
 * @brief  Gets test suite ipc functions
 * @return test suite ipc functions address
 * @else
 * @brief  获取测试套件ipc功能函数
 * @return 测试套件ipc功能函数接口地址
 * @endif
 */
test_suite_channel_funcs_t *test_suite_ipc_funcs_get(void)
{
    static test_suite_channel_funcs_t test_suite_ipc_funcs = {
        .init = test_suite_ipc_init,
        .deinit = test_suite_ipc_deinit,
        .send_char = test_suite_log_helper_char,
        .send = test_suite_log_helper_send,
        .sendf = test_suite_log_helper_sendf,
        .send_line = test_suite_log_helper_send_line,
    };
    return &test_suite_ipc_funcs;
}

void test_suite_commands_echo_clear(cores_t core)
{
    switch (core) {
        case CORES_BT_CORE:
            g_echo_cores &= ~core_flag(CORES_BT_CORE);
            break;
        case CORES_PROTOCOL_CORE:
            g_echo_cores &= ~core_flag(CORES_PROTOCOL_CORE);
            break;
        case CORES_APPS_CORE:
            g_echo_cores &= ~core_flag(CORES_APPS_CORE);
            break;
        default:
            PRINT("input core is invalid : %d \r\n", core);
            break;
    }
}

static void test_suite_commands_send_to_core(cores_t core, char *cmd)
{
    ipc_payload payload;
    if (memset_s((void *)&payload, sizeof(ipc_payload), 0, sizeof(ipc_payload)) != EOK) {
        return;
    }

    payload.testsuite.length = strlen(cmd) + 1; /* include the null terminated */
    if (strncpy_s((char *)payload.testsuite.msg, IPC_TEST_SUITE_STRING_LENTGH,
                  (char *)cmd, payload.testsuite.length) != EOK) {
        return;
    }
    /* Horrible... Waiting for ever! */
    while (1) {
        if (ipc_send_message(core, IPC_ACTION_TS_MESSAGE_REQ, &payload, payload.testsuite.length + 0x4,
                             IPC_PRIORITY_LOWEST, false) != IPC_STATUS_MAILBOX_BUSY) {
            break;
        }
    }
}

/*
 * Iterate through all the cores and show the output if there is an
 * unexpected log or is the core line we are polling (the one that is asserting echo_cores)
*/
static void test_suite_commands_poll_line(cores_t core)
{
    for (cores_t core_i = CORES_BT_CORE; core_i < CORES_MAX_NUMBER_PHYSICAL; core_i++) {
        if (((core_i == core) || (g_unexpected_log & core_flag(core_i))) && g_response[core_i].inuse) {
            if (g_response[core_i].length) {
                test_suite_console_set_color(g_term_colors[core_i]);
                test_suite_uart_send((char *)g_response[core_i].msg);
            } else {
                /* No string in the packet */
                test_suite_console_display_test_status(g_response[core_i].status);
                test_suite_commands_echo_clear(core_i);
            }

            g_response[core_i].inuse = false;
        }

        /* if it is not the core having the control report unrequested "[U]" log */
        if ((core_i != core) && (g_unexpected_log & core_flag(core_i))) {
            test_suite_console_set_color(TERM_COLOR_RED);
            test_suite_uart_send("[U]");
            g_unexpected_log &= ~core_flag(core_i);
        }
    }
}

void test_suite_ipc_clear_core_flag(void)
{
    if (g_echo_cores & core_flag(CORES_APPS_CORE)) {
        g_echo_cores &= ~(core_flag(CORES_APPS_CORE));
    }
    if (g_echo_cores & core_flag(CORES_BT_CORE)) {
        g_echo_cores &= ~(core_flag(CORES_BT_CORE));
    }
}

/**
 * Blocking, command is the command witout the name of the core
 */
void test_suite_commands_execute_in_external_core(cores_t core, char *command, uint32_t timeout)
{
    static timer_handle_t timer = 0;
    g_echo_cores |= core_flag(core);
    test_suite_commands_send_to_core(core, command);
    if (timeout != 0) {
        uapi_timer_create(DEFAULT_TIMER, &timer);
        uapi_timer_start(timer, timeout * TEST_SUITE_TIMER_S_TO_US, test_suite_commands_timeout_callback, 0);
    }

    /* Wait for cores to signal finish state */
    while ((g_echo_cores & core_flag(core)) != 0) {
        feed_back_buffer *buffer = NULL;
        uint32_t buffer_len = TEST_SUITE_MSG_QUEUE_MAX_SIZE;
        buffer = osal_kmalloc(buffer_len, OSAL_GFP_KERNEL);
        if (buffer == NULL) {
            PRINT("[%s][%d]:malloc faile.\r\n", __func__, __LINE__);
            return;
        }
        memset_s(buffer, TEST_SUITE_MSG_QUEUE_MAX_SIZE, 0, buffer_len);
        test_suite_receive_cmd_queue(buffer, &buffer_len);
        test_suite_commands_poll_line(core);
        osal_kfree(buffer);
    }
    if (timeout != 0) {
        uapi_timer_stop(timer);
        uapi_timer_delete(timer);
    }
}

bool test_suite_commands_ipc_message_handler(ipc_action_t message,
    const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    volatile feed_back_buffer *buffer = NULL;
    bool release = true;

    unused(id);
    unused(message);
    unused(src);
    if (message != IPC_ACTION_TS_MESSAGE_CNF) {
        PRINT("receive action is invalid : %d .\r\n", message);
        return false;
    }
    if (src >= CORES_MAX_NUMBER_PHYSICAL) {
        PRINT("receive src is invalid: %d.\r\n", src);
        return false;
    }

    buffer = &(g_response[src]);

    if (!buffer->inuse) {
        if (memcpy_s((char *)buffer->msg, IPC_TEST_SUITE_STRING_LENTGH,
                     (char *)payload_p->testsuite.msg, payload_p->testsuite.length) != EOK) {
            return false;
        }

        buffer->msg[payload_p->testsuite.length] = '\0';

        buffer->length = payload_p->testsuite.length;
        buffer->status = payload_p->testsuite.status;

        buffer->inuse = true;

        /* A message from a core that was apparently not running a test has been received. */
        if ((g_echo_cores & core_flag(src)) == 0) {
            g_unexpected_log |= core_flag(src);
        }
    } else {
        release = false;
    }

    test_suite_write_cmd_queue(&buffer, sizeof(feed_back_buffer));
    return release;
}

#endif