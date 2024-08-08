/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc product configuration.
 * Author:
 * Create: 2021-09-18
 */

#ifndef __HCC_CFG_COMM_H__
#define __HCC_CFG_COMM_H__

#include "osal_types.h"
#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HCC_TRANS_THREAD_TASK_STACK_SIZE 4096

/*
 * 各个bus使用差异说明：
 * SDIO: h->D:  无论是否聚合都带描述符， && 发送之前需要发送控制帧标识队列
 *       D->h:  不论是否聚合都不带描述符，使用64个寄存器存储，第0个标识队列ID， && 发送之前不需要发送控制帧
 */
typedef enum _hcc_transfer_mode_ {
    HCC_SINGLE_MODE,        /* 单帧发送 */
    HCC_ASSEMBLE_MODE,      /* 聚合发送 */
    HCC_TRANS_MODE_BUTT,
    HCC_TRANS_MODE_INVALID = TD_U8_MAX
} hcc_transfer_mode;

typedef enum _hcc_ipc_transfer_mode_ {
    HCC_IPC_TRANS_POSTMEM,        /* 内存后申请模式 */
    HCC_IPC_TRANS_PREMEM,         /* 内存预申请模式 */
    HCC_IPC_TRANS_MODE_BUTT,
    HCC_IPC_TRANS_MODE_INVALID = TD_U8_MAX
} hcc_ipc_transfer_mode;

typedef enum _hcc_bus_type_ {
    HCC_BUS_MIN = 0x0,
    HCC_BUS_USB = 0x0,
    HCC_BUS_SDIO = 0x1,
    HCC_BUS_IPC = 0x3,
    HCC_BUS_PCIE = 0x4,
    HCC_BUS_VIRTUAL = 0x5,
    HCC_BUS_UART = 0x6,
    HCC_BUS_BUTT,
    HCC_BUS_MAX  = 0xFF
} hcc_bus_type;

#define HCC_CHANNEL_AP HCC_BUS_IPC

typedef enum _hcc_serv_main_type {
    HCC_ACTION_TYPE_BT = 0,        /* data from bt */
    HCC_ACTION_TYPE_OAM = 1,       /* data from oam,plat etc. */
    HCC_ACTION_TYPE_TEST = 2,      /* used for hcc transfer test,msg,dfx etc */
    HCC_ACTION_TYPE_GLP = 3,       /* used for device chr exception, discarded by hso now */
    HCC_ACTION_TYPE_CUSTOMIZE = 4, /* used for wifi control message */
    HCC_ACTION_TYPE_BTC_MSG = 5,   /* used for btc msg */
    HCC_ACTION_TYPE_WIFI = 6,      /* data from wifi data transfer */
    HCC_SERVICE_VIRTUAL_HMAC = 7,      /* test for resv1 */
    HCC_SERVICE_VIRTUAL_DMAC = 8,      /* test for resv2 */
    HCC_SERVICE_FOR_HCC_INTERNAL = 9,  /* test for resv3 */
    HCC_ACTION_TYPE_SLE = 0xA,         /* data from sle */
    HCC_SERVICE_TYPE_MAX,
} hcc_service_type;

typedef enum _hcc_queue_type {
    CTRL_QUEUE = 0,                /* used for wifi control message */
    DATA_HI_QUEUE = 1,             /* used for wifi mgmt frame */
    DATA_LO_QUEUE = 2,             /* used for wifi data frame */
    BT_DATA_QUEUE = 3,             /* used for bt data */
    SLE_DATA_QUEUE = 4,            /* used for gle data */
    HCC_QUEUE_INTERNAL = 5,        /* used for oam,plat etc */
    BTC_MSG_QUEUE = 6,             /* used for btc msg */
    HCC_TEST_QUEUE = 7,            /* used for hcc test */
    HCC_QUEUE_COUNT
} hcc_queue_type;

typedef enum _syschannel_queue_type {
    SYSCH_MSG_QUEUE = 0,                /* used for syschannel message */
    SYSCH_DATA_QUEUE = 1,               /* used for syschannel data */
    SYSCH_TEST_QUEUE = 2,               /* used for syschannel test */
} syschannel_queue_type;

typedef enum _d2h_msg_type {
    D2H_MSG_FLOWCTRL_ON = 0,       /* can't send data, now discard, wifi flowctrl self */
    D2H_MSG_FLOWCTRL_OFF = 1,      /* can send data, now discard, wifi flowctrl self */
    D2H_MSG_FLOWCTRL_CHECK = 2,
    D2H_MSG_TEST = 3,
    D2H_MSG_TEST_START = 4,        /* hcc test 开始测试，时间在Host侧记录 */
    D2H_MSG_TEST_STOP = 5,         /* hcc test 结束测试，时间在Host侧记录 */
    D2H_MSG_CREDIT_UPDATE = 6,     /* update high priority buffer credit value, not used */
    D2H_MSG_DEVICE_PANIC = 7,
    D2H_MSG_BSP_READY = 8,         /* device boot msg */
    D2H_MSG_WLAN_READY = 9,        /* wlan ready msg */
    D2H_MSG_PLAT_END = 10,         /* 保留前十个 */
} d2h_msg_type;

/* Host to device sdio message type */
typedef enum _h2d_msg_type {
    H2D_MSG_FLOWCTRL_ON = 0,
    H2D_MSG_FLOWCTRL_OFF = 1,  /* can send data, force to open */
    H2D_MSG_FLOWCTRL_CHECK = 2,
    H2D_MSG_TEST_INIT = 3,
    H2D_MSG_TEST = 4,       /* msg for hcc test */
    H2D_MSG_TEST_LOOP = 5,  /* 双向测试使用msg机制触发 */
    H2D_MSG_WLAN_OPEN = 6,          /* wlan open msg */
    H2D_MSG_WLAN_CLOSE = 7,         /* wlan close msg */
    H2D_MSG_BT_OPEN = 8,
    H2D_MSG_BT_CLOSE = 9,
    H2D_MSG_PLAT_END = 10,  /* 保留前十个 */
} h2d_msg_type;

#define FC_MSG_FLOWCTRL_ON  D2H_MSG_FLOWCTRL_ON
#define FC_MSG_FLOWCTRL_OFF D2H_MSG_FLOWCTRL_OFF
#define FC_MSG_FLOWCTRL_CHECK  D2H_MSG_FLOWCTRL_CHECK

typedef td_u32 hcc_tx_msg_type;
typedef td_u32 hcc_rx_msg_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HCC_CFG_COMM_H__ */
