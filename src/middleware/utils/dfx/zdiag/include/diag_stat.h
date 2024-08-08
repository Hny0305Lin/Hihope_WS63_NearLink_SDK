/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 */

#ifndef __ZDIAG_STAT_H__
#define __ZDIAG_STAT_H__

#include "diag_common.h"
#include "diag_config.h"
#include "diag.h"

typedef struct {
    const diag_sys_stat_obj_t *stat_cmd_list[CONFIG_STAT_CMD_LIST_NUM];  /* Stat. cmd obj list */
    uint16_t aus_stat_cmd_num[CONFIG_STAT_CMD_LIST_NUM];
} zdiag_stat_ctrl_t;

typedef struct {
    uint16_t obj_id;
    uint8_t *object;
    uint16_t obj_size;
    uint16_t obj_cnt;
    diag_option_t *option;
    bool sync;
    uint8_t pad[2]; /* pad(2) */
} zdiag_report_stat_obj_stru_t;

errcode_t zdiag_report_stat_obj(zdiag_report_stat_obj_stru_t pkt);
errcode_t zdiag_query_stat_obj(uint32_t id, uint32_t *obj, uint16_t *obj_len, uint16_t *obj_cnt);

#endif /* __ZDIAG_STAT_H__ */
