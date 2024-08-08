/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: no.
 * Create: 2022-3-8
 */
#ifndef __SENDTEST_H__
#define __SENDTEST_H__

typedef struct {
    td_u32 flag;
    ip_addr_t dip;
    ip_addr_t sip;
    td_u16 dport;
    td_u16 sport;
    td_u32 interval;
    td_u32 time;
    td_u32 delay;
} iperf_cfg_t;

#define ARGS_SIZE          20
#define ERR_OK             0
#define ESP_OK             0
#define ESP_FAIL           (-1)
#define INVALID_SOCKET     (-1)
#define SENDTEST_GBIT      1000000000
#define SENDTEST_MBIT      1000000
#define SENDTEST_KBIT      1000
#define USEC_PER_TICK      10000
#define BYTE2BIT           8
#define HEADER_VERSION1    0x80000000
#define RUN_NOW            0x00000001
#define SOCKETS_MAX_NUM    8
#define SENDTEST_TCP_MSS   1460
#define SENDTEST_IP_DEBUG  0
#define SENDTEST_DEBUG_PARA 1

#define SENDTEST_FLAG_CLIENT 1
#define SENDTEST_FLAG_SERVER (1 << 1)
#define SENDTEST_FLAG_TCP (1 << 2)
#define SENDTEST_FLAG_UDP (1 << 3)
#define SENDTEST_FLAG_IP6 (1 << 31)

#define SENDTEST_DEFAULT_PORT 5001
#define SENDTEST_DEFAULT_INTERVAL 1
#define SENDTEST_DEFAULT_TIME 30
#define SENDTEST_CYCLE_TIMES_ONCE_SCHEDULE 500

#define SENDTEST_TRAFFIC_TASK_NAME "send_traffic"
#define SENDTEST_TRAFFIC_TASK_PRIORITY 20
#define SENDTEST_TRAFFIC_TASK_STACK 0x1000
#define SENDTEST_REPORT_TASK_NAME "send_report"
#define SENDTEST_REPORT_TASK_PRIORITY 2
#define SENDTEST_REPORT_TASK_STACK 0x1000

#define SENDTEST_UDP_TX_LEN 1470
#define SENDTEST_UDP_RX_LEN (8 << 10)
#define SENDTEST_TCP_TX_LEN (8 << 10)
#define SENDTEST_TCP_RX_LEN (8 << 10)

#define SENDTEST_MAX_DELAY 64

#define SENDTEST_SOCKET_RX_TIMEOUT 10
#define SENDTEST_SOCKET_TX_TIMEOUT 10
#define SENDTEST_SOCKET_ACCEPT_TIMEOUT 5
#define SENDTEST_UDP_LAST_DATA_TRY 2

unsigned int cmd_sendtest(int argc, const char *argv[]);

#endif

