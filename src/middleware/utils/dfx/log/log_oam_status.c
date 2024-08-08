/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   LOG OAM STATUS MODULE
 * Author:
 * Create:
 */

#include "securec.h"
#include "error_types.h"
#include "log_common.h"
#include "log_printf.h"
#include "log_oam_status.h"

#define BUFFER_ARRAY_LEN 4
#define PRESS_PARAMS_BUFFER0_INDEX 0
#define PRESS_PARAMS_BUFFER1_INDEX 1
#define PRESS_PARAMS_BUFFER2_INDEX 2
#define PRESS_PARAMS_BUFFER3_INDEX 3


int32_t pf_feature_set(uint32_t feature, uint8_t set)
{
    UNUSED(feature);
    UNUSED(set);

    return SUCC;
}

int32_t pf_feature_get(uint32_t feature)
{
    UNUSED(feature);

    return FEATURE_OFF;
}

void log_oml_status_packet(om_status_data_stru_t *status_entry, uint8_t prime_id,
                           uint16_t msg_id, uint16_t length, const uint8_t *buffer)
{
    if (status_entry == NULL) {
        return;
    }
    /* Strutc */
    status_entry->header.frame_start = OM_FRAME_DELIMITER;
    status_entry->header.func_type = OM_MSG_TYPE_STATUS;
    status_entry->header.prime_id = prime_id;
    status_entry->header.arr_reserver[0] = 0;
    status_entry->header.frame_len = length + OML_STATUS_ADD_LENGTH;
    status_entry->header.sn = get_log_sn_number();
    status_entry->msg_id = msg_id;
    status_entry->data_len = length;

    errno_t sec_ret;
    sec_ret = memcpy_s(status_entry->data, OM_STATUS_DATA_MAX_SIZE, buffer, length);
    if (sec_ret != EOK) {
        return;
    }

    *(status_entry->data + length) = OM_FRAME_DELIMITER;
}

uint32_t log_oml_status_write(uint8_t prime_id, uint16_t msg_id, uint16_t mode, uint16_t length, const uint8_t *buffer)
{
    if (log_get_local_log_level() == LOG_LEVEL_NONE) {
        return SUCC;
    }

    om_status_data_stru_t om_status_entry;

    /* Check parameters */
    if (length >= OM_STATUS_DATA_MAX_SIZE || buffer == NULL) {
        return RET_TYPE_ERROR_IN_PARAMETERS;
    }

    log_oml_status_packet(&om_status_entry, (uint8_t)(mode | prime_id), msg_id, length, buffer);

    log_event((uint8_t *)&om_status_entry, length + OML_STATUS_ADD_LENGTH);

    return SUCC;
}

void log_oam_status_store_deal(uint8_t prime_id, uint16_t msg_id, uint16_t mode, uint16_t length, const uint32_t *param)
{
    uint32_t buffer[BUFFER_ARRAY_LEN] = {0};
    uint8_t len;
    uint32_t *param_data = (uint32_t *)param;
    if (length > BUFFER_ARRAY_LEN) {
        return;
    }

    for (len = 0; len < length; len++) {
        if (param_data != NULL) {
            buffer[len] = *param_data;
            param_data++;
        }
    }

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    if (log_oml_status_write(prime_id, msg_id, mode, (uint16_t)(length * sizeof(uint32_t)), (uint8_t *)buffer) !=
        SUCC) {
        return;
    }
#else
    UNUSED(prime_id);
    UNUSED(mode);
    compress_printf(msg_id, press_params(BTC_MAGIC_LOG_CODE, LOG_LEVEL_INFO, length),
                    buffer[PRESS_PARAMS_BUFFER0_INDEX],
                    buffer[PRESS_PARAMS_BUFFER1_INDEX],
                    buffer[PRESS_PARAMS_BUFFER2_INDEX],
                    buffer[PRESS_PARAMS_BUFFER3_INDEX]);
#endif
}

void log_oam_status_store_init(void)
{
    log_oam_status_store_register_callback(log_oam_status_store_deal);
}
