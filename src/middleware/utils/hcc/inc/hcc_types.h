/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc types.
 * Author:
 * Create: 2021-05-13
 */

#ifndef __HCC_TYPES_H__
#define __HCC_TYPES_H__

#include "td_type.h"
#include "hcc_cfg_comm.h"

typedef enum _hcc_queue_dir_ {
    HCC_DIR_TX = 0x0,
    HCC_DIR_RX = 0x1,
    HCC_DIR_COUNT
} hcc_queue_dir;

/*
 * HCC default macro
 */
#define HCC_DEFAULT_ALIGN_LEN 1

/* hcc tx transfer flow control */
#define HCC_FC_NONE 0x0 /* 对调用者不进行流控，netbuf一直缓冲在hcc队列中,这种类型的数据包不宜过多 */
#define HCC_FC_WAIT 0x1 /* 阻塞等待，如果是在中断上下文调用，该标记被自动清除,非中断上下文生效 */
#define HCC_FC_NET  0x2 /* 对于网络层的流控 */
#define HCC_FC_DROP 0x4 /* 流控采用丢包方式,流控时返回成功 */
#define HCC_FC_ALL  (HCC_FC_WAIT | HCC_FC_NET | HCC_FC_DROP)

typedef struct _hcc_transfer_param {
    td_u8 service_type : 4;
    td_u8 sub_type : 4;
    td_u8 queue_id;    /* 期望进入的队列号 */
    td_u16 fc_flag;    /* 流控标记 */
    td_u8 *user_param;
} hcc_transfer_param;

typedef td_u32 (*hcc_msg_rx)(td_u8 *cb_data);

/*
 * 功能描述：业务数据结构内存释放函数
 * queue_id: 队列ID号。使用场景：一个业务可能使用不同内存池管理，建议不同内存池的数据使用不同队列传输，
 *           通过 queue_id 区分使用的内存池；
 * buf: 业务实际传输数据，其中包含 hcc头和payload，hcc头由调用者分配空间，由hcc模块内部填充；
 * len: buf数据的长度，为hcc头的长度+payload的总长度；
 * user_param: 业务使用数据，使用场景：业务传输数据的buf为内部结构体的一部分，buf和内部结构需要同时释放和申请，
 *             通过buf地址无法计算出其所在结构的首地址，因此增加用户数据指针，具体指针内容hcc模块不关心；
 */
typedef td_u32 (*hcc_adapt_priv_alloc)(hcc_queue_type queue_id, td_u32 len, td_u8 **buf, td_u8 **user_param);
/*
 * 功能描述：业务数据结构内存申请函数
 * queue_id: 释放内存所在队列号；
 */
typedef td_void (*hcc_adapt_priv_free)(hcc_queue_type queue_id, td_u8 *buf, td_u8 *user_param);
/*
 * 功能描述：业务开始流控函数，停止或减缓业务数据发送
 * queue_id: 需要开始流控的队列号。
 */
typedef td_void (*hcc_flowctl_start_subq)(hcc_queue_type queue_id);
/*
 * 功能描述：业务停止流控函数，恢复业务数据发送
 * queue_id: 需要停止流控的队列号。
 */
typedef td_void (*hcc_flowctl_stop_subq)(hcc_queue_type queue_id);
/*
 * 功能描述：业务数据接收处理函数
 * queue_id: 接收数据所在队列号；
 */
typedef td_u32 (*hcc_adapt_priv_rx_process)(hcc_queue_type queue_id, td_u8 sub_type,
                                            td_u8 *buf, td_u32 len, td_u8 *user_param);
typedef struct _hcc_adapt_ops_ {
    hcc_adapt_priv_free       free;
    hcc_adapt_priv_alloc      alloc;
    hcc_flowctl_stop_subq     stop_subq;
    hcc_flowctl_start_subq    start_subq;
    hcc_adapt_priv_rx_process rx_proc;
} hcc_adapt_ops;

#pragma pack(push, 1)
typedef struct _hcc_queue_ctrl_ {
    td_u32 service_type : 4; // 队列所属的业务类型
    td_u32 sub_type : 4; // 队列所属的业务子类型
    td_u32 transfer_mode : 3; // 队列发送模式： 单条发送、聚合发送   接收mode也用该字节   // hcc_transfer_mode
    td_u32 fc_enable : 1;       // 队列流控使能，初始化默认打开
    td_u32 flow_type : 4;   // 队列流控类型,   hcc_flowctrl_type
    td_u32 low_waterline : 8;  // 队列的低水线
    td_u32 high_waterline : 8;  // 队列的高水线

    td_u16 extend;          // ipc配置为队列中buff最大长度
    td_u8 burst_limit; // 队列每次处理数据包的上限，达到上限让出机会给其他队列处理；
    td_u8 credit_bottom_value; //  flow_type 为 HCC_FLOWCTRL_CREDIT 时，该字段生效，低于该值将不再发送；
} hcc_queue_ctrl;
#pragma pack(pop)

typedef struct {
    td_u8 dir; /* hcc_queue_dir */
    td_u8 queue_id; /* hcc_queue_type */
    hcc_queue_ctrl queue_ctrl;
} hcc_queue_cfg; // 配置队列信息
#endif
