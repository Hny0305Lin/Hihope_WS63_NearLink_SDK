/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   LOG UART MODULE
 * Author:
 * Create:
 */
#if defined USE_CMSIS_OS
#include <string.h>
#ifdef USE_OSAL_INSTEAD_CMSIS
#include "soc_osal.h"
#include "los_sched_pri.h"
#else
#include "cmsis_os2.h"
#endif
#include "core.h"
#if CORE == MASTER_BY_ALL
#include "stdio.h"
#if (CORE_NUMS  > 1)
#include "watchdog.h"
#endif
#include "hal_uart.h"
#endif
#ifdef TEST_SUITE
#include "test_suite_uart.h"
#endif
#include "log_panic.h"
#include "log_buffer.h"
#include "log_buffer_reader.h"
#include "log_trigger.h"
#include "panic.h"
#include "non_os.h"
#ifdef SUPPORT_IPC
#include "ipc.h"
#endif

#include "uart.h"
#if defined(CONFIG_UART_SUPPORT_DMA)
#include "dma.h"
#endif  /* CONFIG_UART_SUPPORT_DMA */
#include "log_uart_porting.h"
#include "log_uart.h"
#ifdef HSO_SUPPORT
#include "securec.h"
#include "diag_adapt_sdt.h"
#endif
#ifdef IPC_NEW
#include "ipc.h"
#include "ipc_porting.h"
#endif
#include "diag_log.h"
#include "dfx_adapt_layer.h"

/**
 * Security core logging watermark level.
 * When the level is under the watermark and the Security core goes to sleep the log will stop until retriggered. */
static uint16_t g_log_uart_security_log_watermark = LOG_UART_LOG_WATERMARK_VALUE_ON_INIT;

/** Time(ms) of the oldest message left in the logging buffer. */
#define OLDEST_MESSAGE_IN_BUFFER 1000

static uint8_t g_uart_log_rx_buffer_test[LOG_UART_RX_MAX_BUFFER_SIZE];
#ifdef SW_UART_DEBUG
#ifndef SW_UART_BAUDRATE
#define SW_UART_BAUDRATE 115200UL
#endif
#endif

/** UART Settings. Define these in the C file to avoid pulling in the UART header in the header file. */
#define LOG_UART_BAUD_RATE 115200
#define LOG_UART_DATA_BITS UART_DATA_BIT_8
#define LOG_UART_STOP_BITS UART_STOP_BIT_1
#define LOG_UART_PARITY    UART_PARITY_NONE

/** Number of milliseconds to delay the trigger of a new UART write in case the UART is busy */
#define LOG_UART_DELAY_ON_UART_BUSY 1
#if CORE == MASTER_BY_ALL

/** Mutex protecting accesses to the circular buffer. */
#ifdef USE_OSAL_INSTEAD_CMSIS
static osal_semaphore g_semaphore_uart = { NULL };
static osal_semaphore g_semaphore_logs = { NULL };
#else
static osSemaphoreId_t g_semaphore_uart = NULL;
static osSemaphoreId_t g_semaphore_logs = NULL;
#endif

/** UART Handle */
static uart_bus_t g_log_uart = LOG_UART_BUS;

#if defined(CONFIG_UART_SUPPORT_DMA)
#define DMA_UART_TRANSFER_WIDTH_WORD        4
#define DMA_UART_TRANSFER_WIDTH_HALF_WORD   2
#define DMA_UART_TRANSFER_WIDTH_8           0
#define DMA_UART_TRANSFER_WIDTH_16          1
#define DMA_UART_TRANSFER_WIDTH_32          2
#define DMA_UART_BURST_TRANSACTION_LENGTH_8 2
#endif  /* CONFIG_UART_SUPPORT_DMA */

/* Local function prototypes. */
static void log_uart_tx_isr_callback(const void *buffer, uint32_t length, const void *params);

/**
 * This handler gets called when one of the cores wants to trigger the log reader
 * @param message   The IPC message
 * @param payload_p The payload, contains the reset reason
 * @param src       The core who has just started
 * @param id        The message ID (should always be 0 - the first message after starting)
 * @return
 */
#if (CORE_NUMS  > 1) && defined(IPC_NEW)
static void log_reader_info_action_handler(uint8_t *payload_addr, uint32_t payload_len)
{
    unused(payload_addr);
    unused(payload_len);
    log_uart_trigger();
}
#elif (CORE_NUMS > 1) && (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
void log_reader_info_action_handler(void)
{
    dfx_log_reader_irq_clr();      /* Clear Software interrupt. */
    log_uart_trigger();
}

#elif (CORE_NUMS  > 1)
static bool log_reader_info_action_handler(ipc_action_t message,
                                           const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    unused(message);
    unused(id);
    unused(payload_p);
    unused(src);
    log_uart_trigger();
    return true;
}
#endif

/* Initialise the UART and open it at the specified speed! */
void log_uart_init(void)
{
#if (CORE_NUMS > 1)
    unused(log_reader_info_action_handler);
#endif

    /* Configure and open the UART port for use. */
    uart_pin_config_t log_uart_pins = {
        .tx_pin = CODELOADER_UART_TX_PIN, /* TEST_SUITE_UART_TX_PIN */
        .rx_pin = CODELOADER_UART_RX_PIN, /* TEST_SUITE_UART_RX_PIN */
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    uart_attr_t uart_line_config = {
        .baud_rate = LOG_UART_BAUD_RATE,
        .data_bits = LOG_UART_DATA_BITS,
        .stop_bits = LOG_UART_STOP_BITS,
        .parity = LOG_UART_PARITY
    };
    uart_buffer_config_t uart_buffer_config;

    uart_buffer_config.rx_buffer_size = LOG_UART_RX_MAX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_uart_log_rx_buffer_test;

    /* Open and configure the UART */
    g_log_uart = LOG_UART_BUS;
#if defined(CONFIG_UART_SUPPORT_DMA)
    uart_extra_attr_t extra_attr = {
        .tx_dma_enable = 1,
        .tx_int_threshold = 0,
        .rx_dma_enable = 1,
        .rx_int_threshold = 0
    };

    uapi_dma_init();
    uapi_dma_open();
    (void)uapi_uart_init(LOG_UART_BUS, &log_uart_pins, &uart_line_config, &extra_attr, &uart_buffer_config);
#else
    (void)uapi_uart_init(LOG_UART_BUS, &log_uart_pins, &uart_line_config, NULL, &uart_buffer_config);
#endif  /* CONFIG_UART_SUPPORT_DMA */

#if SYS_DEBUG_MODE_ENABLE == YES
    uapi_uart_register_rx_callback(LOG_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   LOG_UART_RX_MAX_BUFFER_SIZE, log_uart_rx_callback);
#endif
}

void log_uart_send_buffer(const uint8_t *buffer, uint16_t length)
{
    uapi_uart_write(g_log_uart, (const void *)buffer, length, 0);
}

void log_uart_update_watermark_level(log_memory_region_section_t log_mem_region, uint16_t new_watermark_val)
{
#if CORE_NUMS > 1
    if (log_mem_region == LOG_MEMORY_REGION_SECTION_1) {
        g_log_uart_security_log_watermark = new_watermark_val;
    }
#else
    if (log_mem_region == LOG_MEMORY_REGION_SECTION_0) {
        g_log_uart_security_log_watermark = new_watermark_val;
    }
#endif
}

/* Initialises the UART when RTOS functions are available. This should be called before the RTOS scheduler starts,
 * but when we can create the mutex.
 */
// Run after OS, but before threads are started, so in a single thread.
void log_uart_init_after_rtos(void)
{
    /* Create the mutex used to control logs written to . */
#ifdef USE_OSAL_INSTEAD_CMSIS
    osal_sem_binary_sem_init(&g_semaphore_logs, 1);
    if (g_semaphore_logs.sem == NULL) {
#else
    g_semaphore_logs = osSemaphoreNew(1, 0, NULL);
    if (!g_semaphore_logs) {
#endif
        panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        return;
    }
    register_log_trigger(log_uart_trigger);

#ifdef USE_OSAL_INSTEAD_CMSIS
    osal_sem_binary_sem_init(&g_semaphore_uart, 1);
    if (g_semaphore_uart.sem == NULL) {
#else
    g_semaphore_uart = osSemaphoreNew(1, 0, NULL);
    if (!g_semaphore_uart) {
#endif

        panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        return;
    }

    log_reader_ret_t lret = log_buffer_reader_init();
    if (lret != LOG_READER_RET_OK) {
        panic(PANIC_LOG, __LINE__);
        return;
    }

    // Register IPC handler to handle log trigger notifications from other cores
#if (CORE_NUMS > 1) && defined(IPC_NEW)
    if (non_os_is_driver_initialised(DRIVER_INIT_IPC) == false) {
        uapi_ipc_init();
    }
    ipc_rx_handler_info_t handler_info;
    handler_info.msg_id = IPC_MSG_LOG_INFO;
    handler_info.cb = log_reader_info_action_handler;
    (void)uapi_ipc_register_rx_handler(&handler_info);
#elif (CORE_NUMS > 1) && (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
    dfx_log_read_info_init();
#elif (CORE_NUMS > 1)
    if (non_os_is_driver_initialised(DRIVER_INIT_IPC) == false) {
        ipc_init();
    }

    ipc_register_handler(IPC_ACTION_LOG_INFO, log_reader_info_action_handler);
#endif
}

/*
 * UART has trasnmitted some data. Call in interrupt context,
 * simply schedule the read index update and additional data check.
 */
static void log_uart_tx_isr_callback(const void *buffer, uint32_t length, const void *params)
{
    unused(buffer);
    unused(params);
    unused(length);

    /* Pass the amount of data actually transmitted, so we can update the read pointer. */
#ifdef USE_OSAL_INSTEAD_CMSIS
    osal_sem_up(&g_semaphore_uart);
#else
    (void)osSemaphoreRelease(g_semaphore_uart);
#endif
}

void log_uart_trigger(void)
{
    /* Give the semaphore to ensure the main logging task will be active */
#ifdef USE_OSAL_INSTEAD_CMSIS
    osal_sem_up(&g_semaphore_logs);
#else
    (void)osSemaphoreRelease(g_semaphore_logs);
#endif
}

#if defined(CONFIG_UART_SUPPORT_DMA)
static inline bool uart_dma_align_check_word(uint32_t data_buf, uint32_t data_len, uint32_t align_data)
{
    bool ret1 = (data_buf % align_data) == 0 ? true : false;
    bool ret2 = (data_len % align_data) == 0 ? true : false;
    return ret1 && ret2;
}

static uint8_t uart_dma_get_mem_width(uint32_t buff, uint32_t bytes)
{
    if (uart_dma_align_check_word(buff, bytes, (uint32_t)DMA_UART_TRANSFER_WIDTH_WORD)) {
        return DMA_UART_TRANSFER_WIDTH_32;
    }
    if (uart_dma_align_check_word(buff, bytes, (uint32_t)DMA_UART_TRANSFER_WIDTH_HALF_WORD)) {
        return DMA_UART_TRANSFER_WIDTH_16;
    }
    return DMA_UART_TRANSFER_WIDTH_8;
}

static uart_write_dma_config_t dma_cfg = { 0 };
static void log_uart_write_blocking_dma(const void *buf, uint32_t len)
{
    dma_cfg.src_width = uart_dma_get_mem_width((uint32_t)(uintptr_t)buf, len);
    dma_cfg.dest_width = DMA_UART_TRANSFER_WIDTH_8;
    dma_cfg.burst_length = DMA_UART_BURST_TRANSACTION_LENGTH_8;
    if (uapi_uart_write_by_dma(g_log_uart, buf, (uint16_t)len, &dma_cfg) != (int32_t)len) {
        return;
    }
    while (uapi_uart_has_pending_transmissions(g_log_uart)) {};
}
#endif  /* CONFIG_UART_SUPPORT_DMA */

static void log_uart_write_blocking_int(const void *buf, uint32_t len)
{
    // Write The buffer to the uart
    while (uapi_uart_write_int(g_log_uart, buf, (uint16_t)len, NULL, log_uart_tx_isr_callback) != ERRCODE_SUCC) {
        // If it was unsuccessful try again after some ticks
#ifdef USE_OSAL_INSTEAD_CMSIS
        if (OS_SCHEDULER_ALL_ACTIVE) {
            (void)osal_msleep(LOG_UART_DELAY_ON_UART_BUSY);
        }
#else
        if (osKernelGetState() == osKernelRunning) {
            (void)osDelay(LOG_UART_DELAY_ON_UART_BUSY);
        }
#endif
    }

    // Wait until the uart write has been completed
#ifdef USE_OSAL_INSTEAD_CMSIS
    if (OS_SCHEDULER_ALL_ACTIVE && osal_sem_down(&g_semaphore_uart) != osOK) {
        panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        return;
    }
#else
    if (osKernelGetState() == osKernelRunning && osSemaphoreAcquire(g_semaphore_uart, osWaitForever) != osOK) {
        panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        return;
    }
#endif
    else {
        while (uapi_uart_has_pending_transmissions(g_log_uart)) {};
    }
}

void log_uart_write_blocking(const void *buf, uint32_t len)
{
#if defined(CONFIG_UART_SUPPORT_DMA)
    if (len <= CONFIG_UART_FIFO_DEPTH) {
        log_uart_write_blocking_int(buf, len);
    } else {
        log_uart_write_blocking_dma(buf, len);
    }
#else
    log_uart_write_blocking_int(buf, len);
#endif  /* CONFIG_UART_SUPPORT_DMA */
}

#define IBRD_NEED_BAUD_OFFSET_NUM      3
#define REMAINDER_NEED_BAUD_OFFSET_NUM 3
#define FBRD_NEED_REMAINDER_OFFSET_NUM 4

static void log_uart_set_baud_rate(uart_bus_t bus, uint32_t baud)
{
    uart_attr_t uart_line_config = {
        .baud_rate = baud,
        .data_bits = LOG_UART_DATA_BITS,
        .stop_bits = LOG_UART_STOP_BITS,
        .parity = LOG_UART_PARITY
    };
    uapi_uart_set_attr(bus, &uart_line_config);
}

void log_uart_reset_baud_rate(void)
{
    log_uart_set_baud_rate(LOG_UART_BUS, LOG_UART_BAUD_RATE);

#ifdef SW_UART_DEBUG
    log_uart_set_baud_rate(SW_DEBUG_UART_BUS, SW_UART_BAUDRATE);
#endif
#ifdef TEST_SUITE
    log_uart_set_baud_rate(TEST_SUITE_UART_BUS, TEST_SUITE_UART_BAUD_RATE);
#endif
}

void log_main(const void *unused_p)
{
    unused(unused_p);

    log_reader_ret_t lr_ret;
    log_memory_region_section_t lregion;
    log_buffer_header_t lb_header = { 0 };
    uint8_t *b1 = NULL;
    uint32_t l1 = 0;
    uint8_t *b2 = NULL;
    uint32_t l2 = 0;

    for (;;) {
        // Check if there are messages
        while (log_buffer_reader_lock_next(&lregion, &lb_header) == LOG_READER_RET_OK) {
            // Claim the message available
            lr_ret = log_buffer_reader_claim_next(lregion, &b1, &l1, &b2, &l2);
            // we are sure there is a new message
            if ((lr_ret != LOG_READER_RET_OK) || ((lb_header.length - sizeof(lb_header)) != (l1 + l2))) {
                uapi_diag_error_log(0, "[log_uart]data error, r1=0x%x, len1=0x%x, r2=0x%x, len2=0x%x",
                    (uintptr_t)b1, l1, (uintptr_t)b2, l2);
                log_buffer_reader_error_recovery(lregion);
                break;
            }
#ifdef HSO_SUPPORT
            zdiag_adapt_sdt_msg_proc(b1, l1, b2, l2);
#else
            /* Copy SYNC, TIME, SEQUENCE and MSG to circular buffer. */
            if ((g_log_uart != UART_BUS_NONE) && (l1 > 0)) {
                log_uart_write_blocking((const void *)b1, l1);
            }
            if ((g_log_uart != UART_BUS_NONE) && (l2 > 0)) {
                log_uart_write_blocking((const void *)b2, l2);
            }
#endif
            log_buffer_reader_discard(lregion);
            dfx_watchdog_kick();
        }
        // If there has been log indicator get run again
#ifdef USE_OSAL_INSTEAD_CMSIS
        if (OS_SCHEDULER_ALL_ACTIVE && osal_sem_down(&g_semaphore_logs) != OSAL_SUCCESS) {
            panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        }
#else
        if (osKernelGetState() == osKernelRunning && osSemaphoreAcquire(g_semaphore_logs, osWaitForever) != osOK) {
            panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        }
#endif
    }
}
#endif

#endif  // defined USE_CMSIS_OS
