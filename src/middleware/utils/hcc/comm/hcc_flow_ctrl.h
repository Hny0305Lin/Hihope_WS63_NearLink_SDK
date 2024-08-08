/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc flow ctrl module completion.
 * Author:
 * Create: 2021-06-30
 */

#ifndef HCC_FLOW_CTRL_HEADER
#define HCC_FLOW_CTRL_HEADER

#include "td_type.h"
#include "hcc.h"
#include "hcc_queue.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HCC_FC_PRE_PROC_WAIT_TIME 1000

typedef enum _hcc_flowctrl_type_ {
    HCC_FLOWCTRL_DATA,     // 被动接收  接收端的状态，（msg方式）
    HCC_FLOWCTRL_CREDIT,   // 发送前主动获取接收端的剩余包数量
    HCC_FLOWCTRL_BUTT,
    HCC_FLOWCTRL_TYPE_INVALID = 0xF
} hcc_flowctrl_type;

typedef enum _hcc_flowctrl_flag_ {
    HCC_FLOWCTRL_FLAG_OFF = 0,
    HCC_FLOWCTRL_FLAG_ON = 1
} hcc_flowctrl_flag;

#ifdef CONFIG_HCC_SUPPORT_FLOW_CONTRL
td_bool hcc_flow_ctrl_sched_check(hcc_handler *hcc, hcc_trans_queue *queue);
ext_errno hcc_flow_ctrl_process(hcc_handler *hcc, hcc_trans_queue *queue);
td_u32 hcc_flowctrl_on_proc(td_u8 *data);
td_u32 hcc_flowctrl_off_proc(td_u8 *data);
td_u32 hcc_flowctrl_check_proc(td_u8 *data);
#if (defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION))
ext_errno hcc_flow_ctrl_pre_proc(hcc_handler *hcc, hcc_transfer_param *param, hcc_trans_queue *hcc_queue);
#else
ext_errno hcc_flow_ctrl_pre_proc(hcc_handler *hcc, TD_CONST hcc_transfer_param *param,
    hcc_trans_queue *hcc_queue);
#endif
td_void hcc_adapt_tx_start_subq(hcc_handler *hcc, hcc_trans_queue *hcc_queue);
ext_errno hcc_flow_ctrl_module_init(hcc_handler *hcc);
td_void hcc_flow_ctrl_module_deinit(hcc_handler *hcc);
td_void fc_msg_init(td_u8 chl);
#endif
td_bool fc_below_low_waterline(hcc_trans_queue *hcc_queue);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HCC_FLOW_CTRL_HEADER */