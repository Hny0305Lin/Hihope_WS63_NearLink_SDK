/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: log OAM pcm producer interface
 * Author:
 * Create:
 */

#ifndef LOG_OAM_PCM_H
#define LOG_OAM_PCM_H

#include "log_oam_logger.h"
// for IAR compiler
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

#define PCM_REPORT_DATA_MAX_SIZE 1200
#define BT_SAMPLE_DATA_MAX_SIZE  1200
#define OM_PCM_SINK              0x00
#define OM_PCM_SOURCE            0x10
#define OM_DBG_BUF               0x20
#define OM_SNOOP_BUF             0X30
#define OM_SNOOP_MAGIC_NUM       0x50

#define bt_pcm_out_write(msg_id, length, buffer) \
    log_oml_bt_sample_data_write(OM_PCM_SOURCE, msg_id, length, buffer)
#define bt_pcm_in_write(msg_id, length, buffer) \
    log_oml_bt_sample_data_write(OM_PCM_SINK, msg_id, length, buffer)
#define bt_pcm_buffer_write(msg_id, length, buffer) \
    log_oml_bt_sample_data_write(OM_DBG_BUF, msg_id, length, buffer)
#define bt_snoop_buffer_write(msg_id, length, buffer) \
    log_oml_bt_sample_data_write(OM_SNOOP_BUF, msg_id, length, buffer)

typedef enum {
    OM_BT_SAMPLE_DATA_OPEN,
    OM_BT_SAMPLE_DATA_CLOSED,
} log_om_pcm_report_config_t;

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
typedef struct {
    om_msg_header_stru_t header;
    uint16_t msg_id;
    uint16_t data_len;
} om_pcm_header_t;
#else
typedef struct {
    uint32_t magic : 8;
    uint32_t primeid : 8;
    uint32_t sn : 16;
    uint32_t msgid : 16;
    uint32_t datalen : 16;
} om_pcm_header_t;
#endif

typedef enum {
    SAMPLE_DATA_SNOOP = 00,
} om_sample_data_msgid_t;

typedef void(*log_oml_bt_sdw_handler)(uint8_t mode_id, uint16_t msg_id, uint16_t length, const uint8_t *buffer);
void log_oml_bt_sample_data_init(void);
void log_oml_bt_sample_data_write(uint8_t mode_id, uint16_t msg_id, uint16_t length, const uint8_t *buffer);
void log_oml_bt_sample_data_switch(uint8_t on);
void log_oml_bt_sample_data_write_deal(uint8_t mode_id, uint16_t msg_id, uint16_t length, const uint8_t *msg_buffer);
void log_oml_bt_sample_data_write_register_callback(log_oml_bt_sdw_handler callback);
void log_oml_bt_sample_data_write_unregister_callback(void);
void log_oml_bt_sample_data_write_trigger_callback(uint8_t mode_id, uint16_t msg_id, uint16_t length,
    const uint8_t *buffer);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif
