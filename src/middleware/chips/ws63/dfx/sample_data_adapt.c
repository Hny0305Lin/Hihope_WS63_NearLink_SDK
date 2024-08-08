/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: sample data
 * This file should be changed only infrequently and with great care.
 */

#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "diag_adapt_layer.h"
#include "log_oam_pcm.h"
#include "diag_bt_sample_data.h"
#include "diag_sample_data_st.h"
#include "sample_data_adapt.h"

errcode_t diag_cmd_sample_data(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    uint32_t ret;
    diag_sample_data_ind_t sample_data_ind;
    diag_sample_data_cmd_t *sample_data_cmd = cmd_param;
    uint32_t transmit_id = sample_data_cmd->transmit_id;

    unused(cmd_param_size);

    switch (transmit_id) {
        case DIAG_SAMPLE_DATA_TRANSMIT_ID_SCO_IN:
        case DIAG_SAMPLE_DATA_TRANSMIT_ID_SCO_OUT:
        case DIAG_SAMPLE_DATA_TRANSMIT_ID_SNOOP:
            if (sample_data_cmd->flag == 1) {
                ret = diag_sample_data_report_start(transmit_id);
            } else {
                ret = diag_sample_data_report_stop(transmit_id);
            }
            break;
        default:
            dfx_log_err("transmit_id(0x%x) not support\r\n", transmit_id);
            ret = ERRCODE_FAIL;
            break;
    }

    sample_data_ind.ret = ret;
    sample_data_ind.flag = sample_data_cmd->flag;
    sample_data_ind.transmit_id = sample_data_cmd->transmit_id;

    uapi_diag_report_packet(cmd_id, option, (uint8_t *)(&sample_data_ind),
                            (uint16_t)sizeof(diag_sample_data_ind_t), true);

    return ERRCODE_SUCC;
}

void zdiag_adapt_sample_data_report(uint8_t *sdt_buf, uint16_t sdt_buf_size)
{
    om_pcm_header_t *om_pcm = (om_pcm_header_t*)sdt_buf;
    uint32_t prime_id = om_pcm->header.prime_id;
    uint32_t transmit_id;

    switch (prime_id) {
        case OM_PCM_SINK:
            transmit_id = DIAG_SAMPLE_DATA_TRANSMIT_ID_SCO_IN;
            break;
        case OM_PCM_SOURCE:
            transmit_id = DIAG_SAMPLE_DATA_TRANSMIT_ID_SCO_OUT;
            break;
        case OM_SNOOP_BUF:
            transmit_id = DIAG_SAMPLE_DATA_TRANSMIT_ID_SNOOP;
            break;
        default:
            dfx_log_err("sample_data prime_id(0x%x) not support\r\n", prime_id);
            return;
    }

    diag_sample_data_report(transmit_id, sdt_buf + sizeof(om_pcm_header_t),
                            sdt_buf_size - sizeof(om_pcm_header_t) - sizeof(uint8_t));
}
