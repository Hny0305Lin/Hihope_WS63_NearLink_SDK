/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sample data
 * This file should be changed only infrequently and with great care.
 */
#include "errcode.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "dfx_adapt_layer.h"
#include "diag_adapt_layer.h"
#include "diag_sample_data_st.h"
#include "diag_bt_sample_data.h"

typedef struct {
    uint32_t transmit_id;
    uint16_t offset;
} diag_sample_data_transmit_info_t;

diag_sample_data_transmit_info_t g_sample_data_transmit_ctrl[DIAG_SAMPLE_DATA_TRANSMIT_ID_COUNT];

static diag_sample_data_transmit_info_t* sample_data_get_transmit_info(uint32_t transmit_id)
{
    uint32_t index;
    for (index = 0; index < DIAG_SAMPLE_DATA_TRANSMIT_ID_COUNT; index++) {
        if (g_sample_data_transmit_ctrl[index].transmit_id == transmit_id) {
            return &g_sample_data_transmit_ctrl[index];
        }
    }
    return NULL;
}

static diag_sample_data_transmit_info_t* sample_data_add_transmit_info(uint32_t transmit_id)
{
    uint32_t index;
    for (index = 0; index < DIAG_SAMPLE_DATA_TRANSMIT_ID_COUNT; index++) {
        if (g_sample_data_transmit_ctrl[index].transmit_id == 0) {
            break;
        }
    }
    if (index < DIAG_SAMPLE_DATA_TRANSMIT_ID_COUNT) {
        g_sample_data_transmit_ctrl[index].transmit_id = transmit_id;
        g_sample_data_transmit_ctrl[index].offset = 0;
        return &g_sample_data_transmit_ctrl[index];
    }
    return NULL;
}

errcode_t diag_sample_data_report_start(uint32_t transmit_id)
{
    diag_sample_data_transmit_info_t *transmit_info = sample_data_get_transmit_info(transmit_id);
    if (transmit_info != NULL) {
        transmit_info->offset = 0;
        return ERRCODE_SUCC;
    } else {
        transmit_info = sample_data_add_transmit_info(transmit_id);
        if (transmit_info != NULL) {
            return ERRCODE_SUCC;
        } else {
            dfx_log_err("transmit_id(0x%x) start error \r\n", transmit_id);
            return ERRCODE_FAIL;
        }
    }
}

errcode_t diag_sample_data_report_stop(uint32_t transmit_id)
{
    diag_sample_data_transmit_info_t *transmit_info = sample_data_get_transmit_info(transmit_id);
    if (transmit_info != NULL) {
        transmit_info->transmit_id = 0;
        transmit_info->offset = 0;
    }
    return ERRCODE_SUCC;
}

errcode_t diag_sample_data_report(uint32_t transmit_id, uint8_t *buf, uint32_t size)
{
    uint8_t *data[2];
    uint16_t len[2];
    diag_sample_data_reply_pkt_t ind;
    diag_sample_data_transmit_info_t *transmit_info = sample_data_get_transmit_info(transmit_id);

    if (transmit_info == NULL) {
        dfx_log_debug("sample_data_report transmit_id(0x%x) not start \r\n", transmit_id);
        return ERRCODE_FAIL;
    }

    data[0] = (uint8_t*)&ind;
    len[0] = (uint16_t)sizeof(diag_sample_data_reply_pkt_t);
    data[1] = buf;
    len[1] = (uint16_t)size;

    ind.offset = transmit_info->offset;
    ind.size = size;
    ind.ret = ERRCODE_SUCC;
    ind.transmit_id = transmit_id;
    ind.crc = 0;
    uapi_diag_report_packets_normal(DIAG_CMD_ID_SAMPLE_DATA, NULL, data, len, 2); /* pkt_cnt 为2 */
    transmit_info->offset += (uint16_t)size;
    return ERRCODE_SUCC;
}