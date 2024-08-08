/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc module completion.
 * Author:
 * Create: 2021-05-13
 */

#ifndef HCC_MODULE_HEADER
#define HCC_MODULE_HEADER

#include "td_type.h"
#include "soc_errno.h"
#include "hcc_queue.h"
#include "hcc_bus_types.h"
#include "hcc_dfx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct _hcc_handler_ hcc_handler;
#define HCC_HDR_LEN hcc_get_head_len()

typedef enum _hcc_module_init_errno_ {
    HCC_MODULE_INIT_FAILED_INIT_STATE,
    HCC_MODULE_INIT_FAILED_SET_STATE,
    HCC_MODULE_INIT_FAILED_INIT_UNC_POOL,
    HCC_MODULE_INIT_FAILED_INIT_QUEUE,
    HCC_MODULE_INIT_FAILED_INIT_FLOW_CTRL,
    HCC_MODULE_INIT_FAILED_INIT_TASK,
    HCC_MODULE_INIT_FAILED_ADD_HANDLER,
} hcc_module_init_errno;

#define HCC_DISABLE TD_FALSE
#define HCC_ENABLE  TD_TRUE

#pragma pack(push, 1)
/* 4bytes */
typedef struct _hcc_header_ {
    td_u8 sub_type : 4;  /* sub type to hcc type,refer to hcc_action_type */
    td_u8 service_type : 4; /* main type to hcc type,refer to hcc_action_type */
    td_u8 queue_id;  /* 队列ID, TX-RX队列为对应关系 */
    td_u16 pay_len; /* payload的长度 */
} hcc_header;
#pragma pack(pop)

typedef struct _hcc_serv_info_ {
    td_u32 service_type : 4;
    td_u32 bus_type : 4;
    td_u32 rsv : 24;
    osal_atomic service_enable;  /* enable after hcc_service_init */
    hcc_adapt_ops *adapt;
} hcc_serv_info;

typedef struct _hcc_service_list_ {
    struct osal_list_head  service_list;
    hcc_serv_info *service_info;
} hcc_service_list;

typedef struct _hcc_transfer_resource_ {
#ifndef CONFIG_HCC_SUPPORT_NON_OS
    osal_wait hcc_transfer_wq;
    osal_wait hcc_fc_wq;
    osal_task *hcc_transfer_thread_handler; // 主循环task
#endif

    td_u32 cur_trans_pkts : 12;
    td_u32 max_proc_packets_per_loop : 12;
    td_u32 rx_thread_enable : 1;
    td_u32 tx_thread_enable : 1;
    td_u32 task_run : 2;
    td_u32 rsv : 4;

    td_u32 tx_fail_num_limit : 12;
    td_u32 tx_fail_cnt : 12;
    td_u32 rsv1 : 8;

    hcc_trans_queue *hcc_queues[HCC_DIR_COUNT];
} hcc_trans_resource;

struct _hcc_handler_ {
    hcc_data_queue *unc_pool_head;
    hcc_trans_resource hcc_resource;
    hcc_service_list hcc_serv; /* hcc注册的服务 */
    hcc_bus *bus; /* 与当前逻辑通道对应的物理通道的结构 */
    hcc_queue_cfg *que_cfg;
    hcc_dfx *dfx_handle;
    td_u8 que_max_cnt;
    td_u8 srv_max_cnt;
    td_u8 channel_id;
    td_u8 unc_low_limit;
    td_u8 hcc_state;  // HCC状态，使能、禁能、异常使用
};

td_u32 hcc_check_overrun(hcc_handler *hcc);
td_void hcc_handle_tx_fail(hcc_handler *hcc);
td_u32 hcc_get_transfer_packet_num(hcc_handler *hcc);
td_s32 hcc_transfer_thread(td_void *data);
td_u16 hcc_tx_queue_proc(hcc_handler *hcc, hcc_trans_queue *queue);
hcc_service_type hcc_fuzzy_trans_queue_2_service(hcc_handler *hcc, hcc_queue_type queue_id);
hcc_service_type hcc_queue_id_2_service_type(hcc_handler *hcc, hcc_queue_dir dir, hcc_queue_type queue_id);
ext_errno hcc_sched_transfer(hcc_handler *hcc);
td_u16 hcc_rx_queue_proc(hcc_handler *hcc, hcc_trans_queue *queue);
td_void hcc_change_state(osal_atomic *atomic, td_u32 state);
td_bool hcc_check_header_vaild(TD_CONST hcc_handler *hcc, TD_CONST hcc_header *hdr);
td_void hcc_init_unc_buf(hcc_unc_struc *unc_buf, td_u8* buf, td_u32 len, hcc_transfer_param *param);

/* HCC state */
typedef enum {
    HCC_OFF = 0,    /* 关闭状态，不允许调用HCC接口发送数据 */
    HCC_ON,         /* 打开状态，允许调用HCC接口收发数据 */
    HCC_EXCEPTION,  /* 异常状态 */
    HCC_BUS_FORBID, /* 禁止hcc bus reg 操作 */
    HCC_STATE_END
} hcc_state_enum;

td_void hcc_enable_switch(td_u8 chl, td_bool enable);

td_s32 hcc_resume_xfer(td_u8 chl);
td_s32 hcc_stop_xfer(td_u8 chl);

ext_errno hcc_dfx_init(hcc_handler *hcc);
td_void hcc_dfx_queue_total_pkt_increase(hcc_handler *hcc, hcc_service_type serv_type, hcc_queue_dir dir,
                                         hcc_queue_type q_id, td_u8 cnt);
td_void hcc_dfx_queue_loss_pkt_increase(hcc_handler *hcc, hcc_queue_dir dir, hcc_queue_type q_id);

/*
 * 一个service对应多个队列，记录service的同时记录对应的queue 申请内存成功失败的次数
 */
/* 回调业务alloc次数 */
td_void hcc_dfx_service_alloc_cb_cnt_increase(hcc_handler *hcc, hcc_service_type service_type);
/* 业务内存alloc返回成功或失败次数，异常时可能会有参数不正确问题 */
td_void hcc_dfx_service_alloc_cnt_increase(hcc_handler *hcc, hcc_service_type service_type,
                                           hcc_queue_type queue_id, td_bool success);
/* 回调业务free次数 */
td_void hcc_dfx_service_free_cnt_increase(hcc_handler *hcc, hcc_service_type service_type);

/* hcc链表、业务内存释放次数 */
td_void hcc_dfx_mem_free_cnt_increase(hcc_handler *hcc);
td_void hcc_dfx_unc_free_cnt_increase(hcc_handler *hcc);
/* hcc链表TX/RX方向申请成功或失败次数 */
td_void hcc_dfx_unc_alloc_cnt_increase(hcc_handler *hcc, hcc_queue_dir direction, td_bool success);
td_void hcc_dfx_service_total_increase(hcc_handler *hcc);

td_void hcc_dfx_service_startsubq_cnt_increase(hcc_handler *hcc, hcc_service_type service_type);
td_void hcc_dfx_service_stopsubq_cnt_increase(hcc_handler *hcc, hcc_service_type service_type);
td_void hcc_dfx_service_rx_cnt_increase(hcc_handler *hcc, hcc_service_type service_type, hcc_queue_type queue_id);
td_void hcc_dfx_service_exp_rx_cnt_increase(hcc_handler *hcc, hcc_service_type service_type);

td_void hcc_dfx_service_rx_err_cnt_increase(hcc_handler *hcc, hcc_service_type service_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HCC_MODULE_HEADER */
