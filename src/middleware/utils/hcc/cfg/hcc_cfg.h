/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hcc product configuration.
 * Author:
 * Create: 2023-02-13
 */

#ifndef __HCC_CFG_H__
#define __HCC_CFG_H__

#include "td_type.h"
#include "hcc_types.h"
#include "hcc_bus_types.h"
#include "hcc_cfg_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HCC_TASK_PRIORITY 5
#define HCC_FLOWCTRL_DEFAULT_LO_WATER_LINE 4
#define HCC_FLOWCTRL_DEFAULT_HI_WATER_LINE 8
#define HCC_FLOWCTRL_DEFAULT_CREDIT_BOTTOM_VALUE 12
#define HCC_DEFAULT_QUEUE_TRANSFER_BURST_LIMIT 8
#define HCC_MAX_QUEUE_TRANSFER_BURST_LIMIT 8
#define HCC_DATA_QUEUE_BUF_LEN 1680
#define HCC_UNC_POOL_SIZE_CCORE 48
#define HCC_UNC_POOL_SIZE_ACORE 96
#define HCC_UNC_POOL_SIZE_LOW_LIMIT 8
#define HCC_IPC_RX_SCH_WATER_LINE 4

#define HCC_TX_PROC_FAILED_MAX_NUM 100

typedef enum _d2h_srv_msg_type {
    D2H_MSG_WLAN_WAKEUP_SUCC = D2H_MSG_PLAT_END,       /* wlan device唤醒成功 */
    D2H_MSG_WLAN_ALLOW_SLEEP = 11,       /* wlan device回复host允许睡眠 */
    D2H_MSG_WLAN_DISALLOW_SLEEP = 12,    /* wlan device回复host不允许睡眠 */
    D2H_MSG_WLAN_WAKEUP_REQ = 13,        /* wlan device请求唤醒host */
    D2H_MSG_HCC_RX_MAX = HCC_RX_MAX_MESSAGE, /* max support msg count */
} d2h_srv_msg_type;

/* Host to device sdio message type */
typedef enum _h2d_srv_msg_type {
    H2D_MSG_WLAN_WAKE_UP_REQ = H2D_MSG_PLAT_END,  /* wlan host唤醒devcie请求 */
    H2D_MSG_WLAN_SLEEP_REQ = 11,    /* wlan host请求device睡眠 */
    H2D_MSG_HCC_HCC_TX_MAX = HCC_TX_MAX_MESSAGE, /* max support msg count */
} h2d_srv_msg_type;

hcc_queue_cfg *hcc_get_queue_cfg(uint8_t *arr_len);
bool hcc_check_bt_queue(uint8_t queue_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HCC_CFG_H__ */
