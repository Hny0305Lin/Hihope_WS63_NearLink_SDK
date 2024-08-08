/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  zdiag for sample data.
 */

#ifndef __DIAG_SAMPLE_DATA_H__
#define __DIAG_SAMPLE_DATA_H__

#include "diag.h"

#define ZDIAG_SAMPLE_START 1
#define ZDIAG_SAMPLE_STOP 0

typedef enum {
    ZDIAG_BGLE_SAMPLE_CB = 0,
    ZDIAG_WLAN_SAMPLE_CB,
    ZDIAG_SOC_SAMPLE_CB,
    ZDIAG_WLAN_PHY_SAMPLE_CB,
    ZDIAG_SAMPLE_CB_END,
}diag_sample_cb_enum;

/*********************bgle/soc sample data******************/

typedef struct {
    uint32_t flag;        /* 1: start; 0: stop; */
    uint32_t transmit_id;
    uint32_t sample_size; /* ram size: 0-7 */
    uint32_t sample_type; /* full stop, cyclical */
} diag_btsoc_sample_cmd;

/* report end */
typedef struct {
    uint32_t transmit_id;
    uint32_t state_code;  /* end code: 11 */
    uint32_t len;         /* reserved */
    uint32_t data[0];     /* reserved */
} diag_sample_notify;

/***********************wifi sample data***********************/

typedef struct {
    uint32_t flag;        /* 1: start; 0: stop; */
    uint32_t mode;        /* mode/transmitid: 1,4,8; */
    uint32_t sample_size; /* ram size: 0-7 */
    uint32_t sample_type; /* full stop, cyclical */
    uint32_t sub_mode;    /* 0-31 */
} diag_wlan_sample_cmd;

/***********************wifi PHY数采***********************/
/* hso配置参数下发结构体 */
typedef struct {
    uint32_t flag;            /* 1：启动数采; 0：停止数采; */
    uint32_t node;            /* phy 数采节点 mode */
    uint32_t sample_size;     /* 存储数采ram大小 2k 4k 6k 8k 10k 12k 14k 16k 32k */
    uint32_t sample_type;     /* 数采类型：存满停止 0、循环数采 1 */
    uint32_t trigger_start;   /* 触发起始条件 */
    uint32_t trigger_end;     /* 触发结束条件 */
    uint32_t event_rpt_addr;  /* 事件上报寄存器 */
} diag_wlan_phy_sample_cmd;

typedef struct {
    uint32_t transmit_id;
    uint32_t cmd_id;       /* wifi/soc/bgle */
    uint32_t offset;
    uint32_t running;
    uint32_t msg_cnt;
} diag_sample_record;

typedef uint32_t (*diag_sample_func_cb)(void *param, uint32_t len);
void diag_sample_data_register(diag_sample_cb_enum idx, diag_sample_func_cb func);
errcode_t diag_cmd_report_sample_data(uint8_t *buf, uint32_t len);
errcode_t diag_report_wlan_sample_data(const uint8_t *buf, uint32_t len, uint32_t msg_id);
errcode_t diag_cmd_wlan_module_sample_data(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                           diag_option_t *option);

#endif