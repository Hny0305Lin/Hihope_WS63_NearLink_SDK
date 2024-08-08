/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc queue list.
 * Author:
 * Create: 2021-05-13
 */

#ifndef HCC_QUEUE_HEADER
#define HCC_QUEUE_HEADER

#include "td_type.h"
#include "soc_osal.h"
#include "osal_list.h"
#include "hcc_types.h"
#include "hcc_cfg_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct _hcc_data_queue_ {
    struct osal_list_head  data_queue;
    td_u32        qlen;
    osal_spinlock data_queue_lock;
} hcc_data_queue;

typedef struct _hcc_unc_struc {
    td_u8 *buf;
    td_u32 length;
    td_u8 *user_param;
    td_u8  service_type : 4;
    td_u8  sub_type : 4;
    td_u8  queue_id;
    td_u16  rsv;
#ifdef CONFIG_HCC_SUPPORT_IPC_HOST
    td_u8 *addr;
#endif
    struct osal_list_head list;
} hcc_unc_struc;

typedef struct _hcc_trans_queue_ {
    hcc_data_queue queue_info;  // 队列的操作链表
    hcc_queue_ctrl *queue_ctrl; // 队列流控配置 //
    td_u8                queue_id; // 当前队列的ID
    td_u8                rsv;
    struct {
        struct {
            td_u16 flow_ctrl_open : 1; // 队列流控标记，flow type为 HCC_FLOWCTRL_DATA 时使用   hcc_flowctrl_flag
            td_u16 is_stopped : 1;  // 队列流控状态，停止业务入列
            td_u16 fc_on_cnt : 7;
            td_u16 fc_off_cnt : 7;
        } fc_back_para;

        td_u16 credit;  /* flow type为 HCC_FLOWCTRL_CREDIT 时使用, 保存credit值 */
    } fc_para;

    hcc_data_queue send_head; /* 实际需要发送的链表 */
} hcc_trans_queue;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HCC_QUEUE_HEADER */