/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: the header file of hcc public interfacer.
 * Author:
 * Create: 2021-05-13
 */

#ifndef HCC_INTERFACE_H
#define HCC_INTERFACE_H
#include "td_type.h"
#include "hcc_types.h"
#include "hcc.h"
#include "hcc_bus.h"
#include "hcc_test.h"
#include "hcc_cfg_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HCC_TASK_NAME_MAX_LEN 20
#define HCC_CHANNEL_INVALID TD_U8_MAX

typedef struct {
    hcc_bus_type bus_type;
    hcc_queue_cfg *queue_cfg;
    td_char *task_name;
    td_u8 task_pri;
    td_u8 service_max_cnt;
    td_u16 tx_fail_num_limit;
    td_u8 queue_len;
    td_u8 unc_pool_low_limit;
    td_u16 unc_pool_size;
    td_u16 max_proc_packets_per_loop;
    td_u16 rsv;
} hcc_channel_param;

/* HCC通道初始化/去初始化 */
td_u8 hcc_init(hcc_channel_param *init);
td_void hcc_deinit(td_u8 chl);

/* 获取逻辑通道句柄 */
hcc_handler *hcc_get_handler(td_u8 chl);

/*
 * 使能/禁能调度接收线程，禁能后将使用直调方式
 */
td_void hcc_enable_rx_thread(td_u8 chl, td_bool enable);

/*
 * 使能/禁能调度发送线程，禁能后将使用直调方式
 */
td_void hcc_enable_tx_thread(td_u8 chl, td_bool enable);

/*
 * 消息机制: 该机制只支持通知 msg id, 不支持传数据，若需要传输数据请使用 hcc_tx_data 接口;
 * 参数service_type: 业务类型，由于每个业务使用的bus可能不同，因此需要传入 service type, hcc才能用对应的bus传输.
 */
ext_errno hcc_message_register(td_u8 chl, td_u8 rsv, hcc_rx_msg_type msg_id, hcc_msg_rx cb, td_u8 *cb_data);
ext_errno hcc_message_unregister(td_u8 chl, td_u8 rsv, hcc_rx_msg_type msg_id);

/*
 * buf是hcc head + payloadl; hcc head由调用者分配空间，由hcc填充；
 * len:hcc head + payload的总长度; len一定要大于hcc头的长度
 * 返回值: 成功:EXT_ERR_SUCCESS， 失败:其他值
 * 若返回失败需要接口调用者释放内存；
 * 注:所有hcc收到的pkt长度都是包含了hcc头结构的长度。
 */
ext_errno hcc_tx_data(td_u8 chl, td_u8 *buf, td_u16 len, hcc_transfer_param *param);
ext_errno hcc_bt_tx_data(td_u8 chl, td_u8 *data_buf, td_u16 len, hcc_transfer_param *param);

/*
 * 可使用该接口获取 hcc 头结构的长度, hcc_tx_data 接口需要预留 hcc头结构的空间;
 * Rx callback 中业务也可以使用该接口获取hcc头长度，以偏移到实际 payload 所在位置;
 */
td_u16 hcc_get_head_len(td_void);

td_bool hcc_get_state(td_u8 chl);

/*************************************************bus layer interface*******************************************/
ext_errno hcc_send_message(td_u8 chl, hcc_tx_msg_type msg_id, td_u8 rsv);

/*
 * 读寄存器
 * chl: 指定通道的寄存器
 * addr: 寄存器地址数组（sdio:建议高16bit配置为0x4001，低2bit配置为0，
 *                       sdio只取addr中第2-15bit的值，其他bit的值忽略，
 *                      因此即使不按照建议配置addr，读取的也依然是0x4001XXXX寄存器。）
 * value: 保存寄存器值数组
 */
ext_errno hcc_read_reg(td_u8 chl, td_u32 addr, td_u32 *value);

/*
 * 写寄存器
 * chl: 指定通道的寄存器
 * addr: 寄存器地址数组（sdio:建议高16bit配置为0x4001，低2bit配置为0，
 *                      sdio只取addr中第2-15bit的值，其他bit的值忽略，
 *                      因此即使不按照建议配置addr，被写入的也依然是0x4001XXXX寄存器。）
 * value: 保存寄存器值数组
 */
ext_errno hcc_write_reg(td_u8 chl, td_u32 addr, td_u32 value);

/*
 * 获取通道传输的数据对齐长度
 */
td_u32 hcc_get_channel_align_len(td_u8 chl);

/*
 * 设置tx总线聚合层个数.
 */
td_void hcc_set_tx_sched_count(td_u8 chl, td_u8 count);

/*
 * 检查并关hcc流控.
 */
td_u32 hcc_flowctrl_check_with_off(td_u8 *data);

/*************************************************service layer interface*******************************************/

/*
 * hcc_service_type :  业务枚举类型
 * chl       :  通道ID
 * hcc_adapt_ops    :  业务回调接口
 */
/*
 * 业务初始化，业务类型对应的adapt结构，包括业务的内存分配、流控处理、Rx calbback;
 */
ext_errno hcc_service_init(td_u8 chl, hcc_service_type service_type, hcc_adapt_ops *adapt);
ext_errno hcc_service_deinit(td_u8 chl, hcc_service_type service_type);
/*
 * 更新流控credit值
 * 对流控方式为  HCC_FLOWCTRL_CREDIT  的队列才有效；
 * RX端在内存管理中更新credit值
 * TX端在发送时获取credit值，根据credit的水线决定是否发送；
 * 注意: 如果要使用该流控方式，需要在 hcc_service_init 之后使用 hcc_service_update_credit 更新下初始值，否则为0
 */
td_void hcc_service_update_credit(td_u8 chl, hcc_service_type serv, td_u32 credit);

/*************************************************flow ctrl interface*******************************************/
/*
 * 获取/设置 流控水线值
 * chl: 逻辑通道
 * direction: 队列方向
 * q_id: 队列ID
 * low_line: 低水线值
 * high_line: 高水线值
 */
ext_errno hcc_flow_ctrl_set_water_line(td_u8 chl, hcc_queue_dir direction, td_u8 q_id,
                                       td_u8 low_line, td_u8 high_line);
ext_errno hcc_flow_ctrl_get_water_line(td_u8 chl, hcc_queue_dir direction, td_u8 q_id,
                                       td_u8 *low_line, td_u8 *high_line);
td_void hcc_ipc_complement_mem(td_void);

#ifdef CONFIG_HCC_SUPPORT_IPC
td_u32 hcc_ipc_prequeue_remaining_packets(td_u8 queue_id);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* HCC_INTERFACE_H */
