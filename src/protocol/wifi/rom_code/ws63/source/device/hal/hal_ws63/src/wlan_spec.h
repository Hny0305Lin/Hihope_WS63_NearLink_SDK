/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of WLAN product specification macro definition.
 */

#ifndef __WLAN_SPEC_H__
#define __WLAN_SPEC_H__

/*****************************************************************************
    其他头文件包含
*****************************************************************************/
#include "wlan_types_common.h"
#include "wlan_spec_type.h"
#include "wlan_spec_hh503.h"
#include "oal_types.h"
#include "oal_mem_pool.h"
#include "wlan_spec_rom.h"
#include "wlan_phy_common.h"

/*****************************************************************************
  alg相关的spec
*****************************************************************************/
#define ALG_TRAFFIC_CTL_ENABLE_DEFAULT                OSAL_TRUE
#define ALG_TRAFFIC_CTL_RX_ENABLE_DEFAULT             OSAL_TRUE
#define ALG_TRAFFIC_CTL_TX_ENABLE_DEFAULT             OSAL_TRUE

/* 接收方向大包队列阈值 */
#define ALG_TRAFFIC_CTL_RX_PKT_LOW_THRES              42  /* > 5Mbps */
#define ALG_TRAFFIC_CTL_RX_PKT_HIGH_THRES             84  /* > 10Mbps */
#define ALG_TRAFFIC_CTL_RX_PKT_BUSY_THRES             168 /* > 20Mbps */
#define ALG_TRAFFIC_CTL_NETBUF_WINDOW_SIZE            2   /* 回滞区间 */
#define ALG_TRAFFIC_CTL_NETBUF_RESERVED_SIZE          0   /* 其他业务保留 */

#define ALG_TRAFFIC_CTL_RX_DSCR_DEFAULT_THRES         WLAN_MEM_NORMAL_RX_MAX_CNT
#define ALG_TRAFFIC_CTL_RX_DSCR_MIN_THRES             4  /* 调低阈值，至少满足一次中断收包所需要的描述符个数即可 */
#define ALG_TRAFFIC_CTL_RX_DSCR_LOW_THRES             8
#define ALG_TRAFFIC_CTL_RX_DSCR_HIGH_THRES            12
#define ALG_TRAFFIC_CTL_RX_DSCR_BUSY_THRES            16
#define ALG_TRAFFIC_CTL_RX_DSCR_BUSY_THRES_LEGACY     4

/* 接收方向小包队列阈值 */
#define ALG_TRAFFIC_CTL_RX_SMALL_Q_PKT_LOW_THRES      2
#define ALG_TRAFFIC_CTL_RX_SMALL_Q_PKT_HIGH_THRES     12
#define ALG_TRAFFIC_CTL_RX_SMALL_Q_PKT_BUSY_THRES     18

#define ALG_TRAFFIC_CTL_RX_SMALL_Q_DSCR_DEFAULT_THRES WLAN_MEM_SMALL_RX_MAX_CNT
#define ALG_TRAFFIC_CTL_RX_SMALL_Q_DSCR_MIN_THRES     8  /* 需要至少满足一次中断收包所需要的描述符个数 */
#define ALG_TRAFFIC_CTL_RX_SMALL_Q_DSCR_LOW_THRES     8
#define ALG_TRAFFIC_CTL_RX_SMALL_Q_DSCR_HIGH_THRES    8
#define ALG_TRAFFIC_CTL_RX_SMALL_Q_DSCR_BUSY_THRES    8
#define ALG_DEBUG_FLAG                                1
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of file */

