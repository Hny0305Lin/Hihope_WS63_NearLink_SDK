/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   log OAM pcm producer interface
 * Author:
 * Create:
 */
#ifndef LOG_OAM_DSCR_CB_H
#define LOG_OAM_DSCR_CB_H

#include "log_oam_logger.h"
#include "log_printf.h"

#define OML_DSCR_CB_ADD_LENGTH   (19)
#define OM_DSCR_CB_DATA_MAX_SIZE (1600)

typedef struct _om_dscr_cb_data_stru_t {
    om_msg_header_stru_t header;
    uint32_t msg_id;
    uint32_t module_id;
    uint16_t data_len;
    uint8_t data[OM_DSCR_CB_DATA_MAX_SIZE];
    uint8_t frame_tail;
} om_dscr_cb_data_stru_t;

uint32_t log_oml_dscr_cb_write(uint32_t module_id, uint32_t msg_id, const uint8_t *buffer, uint16_t length,
    uint8_t level);

#if ((defined SLAVE_BY_WS53_ONLY) && (SLAVE_BY_WS53_ONLY == 1))     // 1 在WS53方案中host有另外的接口定义
uint32_t uapi_diag_report_sys_msg(uint32_t module_id, uint32_t msg_id, const uint8_t *buf,
    uint16_t buf_size, uint8_t level);
#endif

#endif
