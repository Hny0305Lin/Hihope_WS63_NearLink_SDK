/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   LOG UART MODULE INTERFACE
 * Author:
 * Create:
 */

#ifndef LOG_UART_H
#define LOG_UART_H

/**
 * @addtogroup connectivity_libs_log_reader
 * @{
 */
#if defined USE_CMSIS_OS

#include "log_memory_definitions.h"
#include "log_oam_msg.h"

#define LOG_UART_LOG_WATERMARK_VALUE_ON_INIT    0
#define LOG_UART_BT_LOG_WATERMARK_VALUE_HIGH    ((BT_CORE_LOGGING_LENGTH)*8 / 10)

typedef enum {
    NXLOG_OPERATION_ENABLE = 0,
    NXLOG_OPERATION_DISABLE,
    NXLOG_OPERATION_DELETE,
    NXLOG_OPERATION_DUMP_TO_AT_UART,
    NXLOG_OPERATION_DUMP_TO_DEBUG_UART,
} nxlog_operation_t;

#if SYS_DEBUG_MODE_ENABLE == YES
void log_register_default_rx_callback(void);
#endif  /* end of SYS_DEBUG_MODE_ENABLE == YES */

/**
 * @brief  Initialises the uart logger before RTOS functions are available.
 */
void log_uart_init(void);

/**
 * @brief  Send buffer used log uart by write uart tx FIFO
 */
void log_uart_send_buffer(const uint8_t *buffer, uint16_t length);
/**
 * @brief  Send buffer used log uart by write uart tx FIFO
 */
void log_uart_write_blocking(const void *buf, uint32_t len);

/**
 * @brief  output response to at uart callback
 * @param  at_string The at string to send out through at uart.
 */
typedef void (*log_uart_output_response_to_at_uart_callback)(const char *at_string, uint16_t len);

/**
 * @brief  Initialises the UART logging when RTOS functions are available.
 * This should be called before the RTOS scheduler starts, but when we can create mutexes.
 */
void log_uart_init_after_rtos(void);

/**
 * @brief  Triggers the uart logger.
 * It can be called from program context or ISR.
 */
void log_uart_trigger(void);

/**
 * @brief  Check if the xlog is enabled.
 * @return 1 if the xlog is enabled.
 */
uint32_t log_uart_get_nxlog_state(void);

/**
 * @brief  Reconfigure the baud rate when the frequency changes.
 */
void log_uart_reset_baud_rate(void);

/**
 * @brief  Main log uart task
 * @param  unused_p unused parameter
 */
void log_main(const void *unused_p);

/**
 * @brief  Update the watermark level for a given logging region.
 * @note   Currently just LOG_MEMORY_REGION_SECTION_1 is supported.
 */
void log_uart_update_watermark_level(log_memory_region_section_t log_mem_region, uint16_t new_watermark_val);

#if (CORE_NUMS > 1) && (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (defined DFX_YES) && \
    (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
/**
 * @brief  Handler for informing LOG thread to send log contents.
 */
void log_reader_info_action_handler(void);
#endif

#else
#error "log to uart not implemented in non-os version"
#endif  // defined USE_CMSIS_OS

/**
 * @}
 */
#endif
