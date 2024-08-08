/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: HCC TEST
 * Author:
 */

#ifndef HCC_TEST_HEADER
#define HCC_TEST_HEADER

#include "td_type.h"
#include "hcc_cfg_comm.h"
#ifndef CONFIG_HCC_SUPPORT_NON_OS
#include "soc_osal.h"
#endif

#define HCC_TEST_ARGC_MAX 5
#define HCC_TEST_REMOTE_CMD_ARGC_LEN 4  /* argc 1Byte + reserve 2Bytes */
#define HCC_TEST_SUBTYPE_DATA 0         /* test业务子类型：测试数据 */
#define HCC_TEST_SUBTYPE_COMMAND 5      /* test业务子类型：测试命令 */
#define HCC_TEST_CMD_STR "hcc_test_cmd"

typedef struct _hcc_test_result_ {
    td_u32 trans_rate; /* 测试速率， 单位：Mbps */
    td_u32 trans_bits; /* 测试传输总bit */
    td_u64 trans_time; /* 测试耗时， 单位：us */
} hcc_test_result;

typedef struct _hcc_test_para_ {
    td_u32 loop_cnt;
    td_u32 expect_rx_cnt;
    td_u32 test_data;
    td_u16 frame_size;
    td_u8 channel_id;
    td_u8 test_service;
    td_u8 master_to_slave;
    hcc_queue_type test_queue_id;
} hcc_test_para;

typedef struct _hcc_test_stat_ {
    td_bool test_stop;  /* 开始进行流控，测试停止 */
} hcc_test_stat;

#define HCC_TEST_TASK_STOP 0
#define HCC_TEST_TASK_START 1
typedef struct _hcc_test_info_ {
    /* reference to hcc test task */
#ifndef CONFIG_HCC_SUPPORT_NON_OS
    osal_atomic hcc_test_start;
    osal_wait hcc_test_wq;
    osal_task *hcc_test_thread_handler;
#endif
    hcc_test_para test_para;
    hcc_test_stat test_stat;
    hcc_test_result test_data;
    td_u64 start_time;
    td_u64 stop_time;
} hcc_test_info;

typedef struct _hcc_test_cmd_args_ {
    td_u32 argv[HCC_TEST_ARGC_MAX];
    td_u8  argc;
} hcc_test_cmd_args;

typedef enum {
    HCC_TEST_CMD_TEST_INIT = 0,           /* 测试模块初始化 */
    HCC_TEST_CMD_INIT_ALL_PARAM_AND_START_TEST = 1, /* 设置收发必须的参数后启动测试,参数同HCC_TEST_CMD_INIT_ALL_PARAM */
    HCC_TEST_CMD_TEST_MSG = 2,            /* 发送 message */
    HCC_TEST_CMD_SET_QUEUE = 3,            /* 设置模式 */
    HCC_TEST_CMD_SET_TX_CHANNEL = 4,     /* 设置测试发送的逻辑通道 */
    HCC_TEST_CMD_TEST_REG_OPT = 5,       /* 测试寄存器读写接口 */
    HCC_TEST_CMD_START_TEST = 6,          /* 开始测试 */
    HCC_TEST_CMD_INIT_ALL_PARAM = 7,      /* 初始化所有测试参数,顺序依次为: 发送次数、帧长度、期望接收数量 */
    HCC_TEST_CMD_PRINT_RESULT = 8,        /* 打印测试统计量 */
    HCC_TEST_CMD_M2S = 9,                 /* test host to slave */

    HCC_TEST_CMD_ENABLE_TX_THREAD = 10,   /* 发送使能/禁能线程 */
    HCC_TEST_CMD_ENABLE_RX_THREAD = 11,    /* 接收使能/禁能线程 */
    HCC_TEST_UPDATE_CREDIT = 12,
    HCC_TEST_GET_CREDIT = 13,
    HCC_TEST_CMD_SET_TX_DATA = 14,        /* 设置测试发送的数据内容 */
    HCC_TEST_CMD_SET_REG_ADDR = 15,       /* 设置寄存器地址 */
    HCC_TEST_CMD_SET_MODE = 16,           /* 设置模式 */

    HCC_TEST_PRINT_QUEUE_INFO = 20,       /* 队列信息打印 */
    HCC_TEST_PRINT_SERVICE_INFO = 21,     /* 业务信息打印 */
    HCC_TEST_PRINT_BUS_INFO = 22,       /* bus层维测信息打印 */
    HCC_TEST_CMD_REMOTE = 31,             /* 发给远端处理 */
    HCC_TEST_CMD_MAX = 32
} hcc_test_cmd;

/********************** TEST SERVICE *****************************/
td_void hcc_test_init(td_void);
td_void hcc_test_deinit(td_void);
td_void hcc_test(td_void);
td_void hcc_adapt_test_free(hcc_queue_type queue_id, td_u8 *buf, td_u8 *user_param);

/********************** TEST *****************************/
td_s32 hcc_test_proc_local_cmd(td_u32 *argv, td_u32 argc);
td_void hcc_test_start_test_after_init_paras(td_void);
td_void hcc_test_print_test_result(td_void);

hcc_test_info *hcc_test_get_test_info(td_void);

td_void hcc_test_set_trans_rate(td_u32 rate);
td_void hcc_test_set_trans_time_us(td_u64 us);
td_u32 hcc_test_get_exp_rx_cnt(td_void);
td_u64 hcc_test_get_start_time(td_void);
td_void hcc_test_set_start_time(td_u64 us);
td_u64 hcc_test_get_stop_time(td_void);
td_void hcc_test_set_stop_time(td_u64 us);
td_u64 hcc_test_get_trans_time_us(td_void);
td_u32 hcc_test_get_trans_rate(td_void);
td_u32 hcc_test_get_trans_bits(td_void);
td_void hcc_test_set_trans_bits(td_u32 bits);
td_bool hcc_test_get_m2s_status(td_void);
td_u32 hcc_test_get_loop_cnt(td_void);
td_u16 hcc_test_get_frame_size(td_void);
td_u32 hcc_test_get_test_service(td_void);
td_bool hcc_test_is_test_stop(td_void);
td_void hcc_test_set_test_stop(td_bool stop);
hcc_queue_type hcc_test_get_test_queue_id(td_void);
td_void hcc_test_set_test_queue_id(td_u32 queue_id);
td_u32 hcc_test_get_test_data(td_void);
td_u8 hcc_test_get_test_chan(td_void);
/********************** TEST host device *****************************/
td_void hcc_test_get_credit(td_void);
td_void hcc_test_msg_init(td_void);
td_void hcc_test_register_opt(td_u32 cmd);
td_void hcc_test_set_register_addr(td_u32 addr, td_u32 offset, td_u32 value);
td_void hcc_test_reset_rx_stat(td_void);
td_void hcc_test_print_test_result(td_void);
td_u32 hcc_test_rx_proc(hcc_queue_type queue_id, td_u8 sub_type, td_u8 *buf, td_u32 len, td_u8 *user_param);
td_void hcc_test_start(td_void);
td_void hcc_test_start_test_after_init_paras(td_void);
int hcc_test_cmd_proc(int argc, char *argv[]);
td_s32 hcc_test_proc_remote_cmd(td_u32 *argv, td_u32 argc);
td_s32 hcc_test_cmd_ctrl_init(void);

static inline td_u64 hcc_get_timestamp(td_void)
{
#ifndef CONFIG_HCC_SUPPORT_NON_OS
    osal_timeval tv;
    osal_gettimeofday(&tv);
    return (td_u64)(tv.tv_sec * 1000000 + tv.tv_usec); /* ms转化 us * 1000000 */
#else
    return 0;
#endif
}

typedef td_s32 (*hcc_test_proc)(td_u32 *argv, td_u32 argc);
td_void hcc_test_cmd_register(hcc_test_proc test_proc);
#endif /* HCC_TEST_HEADER */
