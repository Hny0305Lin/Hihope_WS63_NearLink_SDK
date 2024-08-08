/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description:  LOG OAM PCM MODULE
 */
#include "non_os.h"
#include "log_common.h"
#include "log_buffer.h"
#include "log_oam_ota.h"
#ifdef SDT_LOG_BY_UART
#include "sdt_by_uart_external.h"
#endif

void log_oml_ota_init(void)
{
    /* Set initialization success flag */
    log_oml_ota_set(OM_OTA_OPEN);
    log_oml_ota_write_register_callback(log_oml_ota_write_deal);
}

void log_oml_ota_write_deal(uint8_t mode_id, uint16_t msg_id, uint16_t length, const uint8_t *msg_buffer)
{
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    if (log_get_local_log_level() == LOG_LEVEL_NONE) {
        return;
    }

    om_ota_header_t ota_header;
    uint8_t tail = OM_FRAME_DELIMITER;
#ifndef SDT_LOG_BY_UART
    uint32_t available = 0;
#endif

    /* Check if initialization or OTA can output */
    if (log_oml_ota_get() == OM_OTA_CLOSED) {
        return;
    }

    /* Check parameter */
    if (length > OTA_DATA_MAX_SIZE || msg_buffer == NULL) {
        return;
    }

    /* Fill in the structure */
    ota_header.header.frame_start = OM_FRAME_DELIMITER;
    ota_header.header.func_type = OM_MSG_TYPE_OTA;
    ota_header.header.prime_id = mode_id;
    ota_header.header.frame_len = (uint16_t)sizeof(om_ota_header_t) + length + (uint16_t)sizeof(tail);
    ota_header.header.sn = get_log_sn_number();
    ota_header.msg_id = msg_id;
    ota_header.data_len = length;

    non_os_enter_critical();
#ifdef SDT_LOG_BY_UART
    oml_write_uart_fifo((uint8_t*)&ota_header, sizeof(om_ota_header_t), LOGUART_BASE);
    oml_write_uart_fifo((uint8_t*)msg_buffer, length, LOGUART_BASE);
    oml_write_uart_fifo((uint8_t*)&tail, sizeof(tail), LOGUART_BASE);
#else
    log_buffer_get_available_for_next_message(&available);

    // Log buffer available enough to store buffer and time_us
    if (ota_header.header.frame_len - sizeof(uint32_t) < available) {
        log_event((uint8_t *)&ota_header, sizeof(om_ota_header_t));
        log_event(msg_buffer, length);
        log_event(&tail, sizeof(tail));
    }
#endif
    non_os_exit_critical();
    return;
#else
    UNUSED(mode_id);
    UNUSED(msg_id);
    UNUSED(length);
    UNUSED(msg_buffer);
#endif
}

void log_oml_ota_switch(uint8_t on)
{
    log_oml_ota_set((om_ota_config_t)on);
}

