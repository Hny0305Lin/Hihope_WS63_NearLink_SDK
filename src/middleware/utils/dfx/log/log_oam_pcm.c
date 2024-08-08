/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description:  LOG OAM PCM MODULE
 */
#include "non_os.h"
#include "log_common.h"
#include "log_buffer.h"
#include "log_oam_pcm.h"

static uint16_t g_log_bt_sampledata_sn_number = 0;
static uint8_t g_bt_sample_datastart = OM_BT_SAMPLE_DATA_OPEN;

void log_oml_bt_sample_data_init(void)
{
    /* Set initialization success flag */
    g_bt_sample_datastart = OM_BT_SAMPLE_DATA_OPEN;
}

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
void log_oml_bt_sample_data_write_deal(uint8_t mode_id, uint16_t msg_id, uint16_t length, const uint8_t *msg_buffer)
{
    om_pcm_header_t pcm_header;
    uint8_t tail = OM_FRAME_DELIMITER;
    uint32_t lb_available;

    /* Check if initialization or OTA can output */
    if ((g_bt_sample_datastart == OM_BT_SAMPLE_DATA_CLOSED) || (log_get_local_log_level() == LOG_LEVEL_NONE)) {
        return;
    }

    /* Check parameter */
    if ((length > BT_SAMPLE_DATA_MAX_SIZE) || (msg_buffer == NULL)) {
        return;
    }

    /* Fill in the structure */
    pcm_header.header.frame_start = OM_FRAME_DELIMITER;
    pcm_header.header.func_type = OM_BT_SAMPLE_DATA;
    pcm_header.header.prime_id = mode_id;
    pcm_header.header.frame_len = (uint16_t)sizeof(om_pcm_header_t) + length + (uint16_t)sizeof(tail);
    pcm_header.header.sn = g_log_bt_sampledata_sn_number++;
    pcm_header.msg_id = msg_id;
    pcm_header.data_len = length;

    lb_available = 0;
    non_os_enter_critical();
    log_buffer_get_available_for_next_message(&lb_available);

    // Log buffer available enough to store buffer and time_us
    if (pcm_header.header.frame_len - sizeof(uint32_t) >= lb_available) {
        non_os_exit_critical();
        return;
    } else {
        log_event((uint8_t *)&pcm_header, sizeof(om_pcm_header_t));
        log_event(msg_buffer, length);
        log_event(&tail, sizeof(tail));
        non_os_exit_critical();
    }
}
#else
void log_oml_bt_sample_data_write_deal(uint8_t mode_id, uint16_t msg_id, uint16_t length, const uint8_t *buffer)
{
    om_pcm_header_t pcm_header;

    /* Check if initialization or OTA can output */
    if ((g_bt_sample_datastart == OM_BT_SAMPLE_DATA_CLOSED) || (log_get_local_log_level() == LOG_LEVEL_NONE)) {
        return;
    }

    /* Check parameter */
    if ((length > BT_SAMPLE_DATA_MAX_SIZE) || (buffer == NULL)) {
        return;
    }

    pcm_header.magic = OM_SNOOP_MAGIC_NUM;
    pcm_header.primeid = mode_id;
    pcm_header.sn = g_log_bt_sampledata_sn_number++;
    pcm_header.msgid = msg_id;
    pcm_header.datalen = length;

    compress_log_write((const uint8_t *)&pcm_header, sizeof(om_pcm_header_t));
    compress_log_write((const uint8_t *)buffer, length);
}
#endif

void log_oml_bt_sample_data_switch(uint8_t on)
{
    g_bt_sample_datastart = on;
}
