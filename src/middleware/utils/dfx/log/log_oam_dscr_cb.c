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
#include "log_oam_dscr_cb.h"

static void log_oml_dscr_cb_packet(om_dscr_cb_data_stru_t *dscr_entry, uint32_t module_id,
    uint32_t msg_id, const uint8_t *buffer, uint16_t length)
{
    errno_t sec_ret;

    if (dscr_entry == NULL) {
        return;
    }
    /* Strutc */
    dscr_entry->header.frame_start = OM_FRAME_DELIMITER;
    dscr_entry->header.func_type = OM_MSG_TYPE_DSCR;
    dscr_entry->header.prime_id = 0;    // 待适配，能用于trigger
    dscr_entry->header.arr_reserver[0] = 0;
    dscr_entry->header.frame_len = length + OML_DSCR_CB_ADD_LENGTH;
    dscr_entry->header.sn = get_log_sn_number();
    dscr_entry->msg_id = msg_id;
    dscr_entry->module_id = module_id;
    dscr_entry->data_len = length;

    sec_ret = memcpy_s(dscr_entry->data, OM_DSCR_CB_DATA_MAX_SIZE, buffer, length);
    if (sec_ret != EOK) {
        return;
    }

    *(dscr_entry->data + length) = OM_FRAME_DELIMITER;
}

uint32_t log_oml_dscr_cb_write(uint32_t module_id, uint32_t msg_id, const uint8_t *buffer, uint16_t length,
    uint8_t level)
{
    om_dscr_cb_data_stru_t om_dscr_entry;
    UNUSED(level);

    if (log_get_local_log_level() == LOG_LEVEL_NONE) {
        return SUCC;
    }
    /* Check parameters */
    if (length >= OM_DSCR_CB_DATA_MAX_SIZE || buffer == NULL) {
        return RET_TYPE_ERROR_IN_PARAMETERS;
    }

    log_oml_dscr_cb_packet(&om_dscr_entry, module_id, msg_id, buffer, length);

    log_event((uint8_t *)&om_dscr_entry, length + OML_DSCR_CB_ADD_LENGTH);
    return SUCC;
}

#if ((defined SLAVE_BY_WS53_ONLY) && (SLAVE_BY_WS53_ONLY == 1))   // 1 在WS53方案中host有另外的接口
uint32_t uapi_diag_report_sys_msg(uint32_t module_id, uint32_t msg_id, const uint8_t *buf,
    uint16_t buf_size, uint8_t level)
{
    uint32_t ret;
    ret = log_oml_dscr_cb_write(module_id, msg_id, buf, buf_size, level);
    if (ret != SUCC) {
        return RET_TYPE_ERROR_IN_PARAMETERS;
    }
    return SUCC;
}
#endif