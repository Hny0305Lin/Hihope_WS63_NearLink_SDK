/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:  log oml ota module to rom implementation
 * Author:
 * Create: 2020-4-17
 */

#include "non_os.h"
#include "log_buffer.h"
#include "log_oam_ota.h"
#include "log_oam_status.h"
#include "log_oam_pcm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define BUFFER_ARRAY_LEN 4

static uint32_t g_ota_start = OM_OTA_OPEN;

static log_oml_ota_write_handler g_log_oml_ota_write_deal = log_oml_ota_write_deal;

static log_oam_status_store_handler g_log_oam_status_store_deal = log_oam_status_store_deal;

static log_oml_bt_sdw_handler g_log_oml_bt_sdw_deal = log_oml_bt_sample_data_write_deal;

void log_oml_ota_set(om_ota_config_t value)
{
    if (value > OM_OTA_CLOSED) {
        return;
    }
    non_os_enter_critical();
    g_ota_start = (uint32_t)value;
    non_os_exit_critical();
}

uint32_t log_oml_ota_get(void)
{
    return g_ota_start;
}

void log_oml_ota_write(uint8_t mode_id, uint16_t msg_id, uint16_t length, const uint8_t *msg_buffer)
{
    log_oml_ota_write_trigger_callback(mode_id, msg_id, length, msg_buffer);
}

void log_oml_ota_write_register_callback(log_oml_ota_write_handler callback)
{
    if (callback != NULL) {
        g_log_oml_ota_write_deal = callback;
    }
}

void log_oml_ota_write_unregister_callback(void)
{
    g_log_oml_ota_write_deal = NULL;
}

void log_oml_ota_write_trigger_callback(uint8_t mode_id, uint16_t msg_id, uint16_t length, const uint8_t *msg_buffer)
{
    if (g_log_oml_ota_write_deal != NULL) {
        g_log_oml_ota_write_deal(mode_id, msg_id, length, msg_buffer);
    }
}

void log_oam_status_store(uint8_t prime_id, uint16_t msg_id, uint16_t mode, uint32_t length, ...)
{
    uint32_t buffer[BUFFER_ARRAY_LEN] = {0};
    uint8_t len;
    va_list args;

    if (length > BUFFER_ARRAY_LEN) {
        return;
    }

    va_start(args, length);
    for (len = 0; len < length; len++) {
        buffer[len] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);

    log_oam_status_store_trigger_callback(prime_id, msg_id, mode, (uint16_t)length, buffer);
}

void log_oam_status_store_register_callback(log_oam_status_store_handler callback)
{
    if (callback != NULL) {
        g_log_oam_status_store_deal = callback;
    }
}

void log_oam_status_store_unregister_callback(void)
{
    g_log_oam_status_store_deal = NULL;
}

void log_oam_status_store_trigger_callback(uint8_t prime_id, uint16_t msg_id, uint16_t mode,
                                           uint16_t length, const uint32_t *param)
{
    if (g_log_oam_status_store_deal != NULL) {
        g_log_oam_status_store_deal(prime_id, msg_id, mode, length, param);
    }
}

void log_oml_bt_sample_data_write(uint8_t mode_id, uint16_t msg_id, uint16_t length, const uint8_t *buffer)
{
    log_oml_bt_sample_data_write_trigger_callback(mode_id, msg_id, length, buffer);
}

void log_oml_bt_sample_data_write_register_callback(log_oml_bt_sdw_handler callback)
{
    if (callback != NULL) {
        g_log_oml_bt_sdw_deal = callback;
    }
}

void log_oml_bt_sample_data_write_unregister_callback(void)
{
    g_log_oml_bt_sdw_deal = NULL;
}

void log_oml_bt_sample_data_write_trigger_callback(uint8_t mode_id, uint16_t msg_id, uint16_t length,
                                                   const uint8_t *buffer)
{
    if (g_log_oml_bt_sdw_deal != NULL) {
        g_log_oml_bt_sdw_deal(mode_id, msg_id, length, buffer);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
