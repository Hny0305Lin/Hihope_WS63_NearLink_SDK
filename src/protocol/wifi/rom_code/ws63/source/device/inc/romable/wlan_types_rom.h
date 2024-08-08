/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of commom message.
 */

#ifndef WLAN_TYPES_ROM_H
#define WLAN_TYPES_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if_rom.h"
#include "oal_wlan_util_rom.h"
#include "wlan_types_base_rom.h"
#include "wlan_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define WLAN_MAX_BAR_LEN                    20  /* BAR帧的最大长度,  包含head和payload */
#define WLAN_MAX_BAR_HEAD_LEN               16  /* BAR帧的头最大长度 */

#define WLAN_WITP_CAPABILITY_PRIVACY BIT4
/*****************************************************************************
  2.5 过滤命令宏定义
*****************************************************************************/
#define  WLAN_BIP_REPLAY_FAIL_FLT  BIT0               /* BIP重放攻击过滤 */
#define  WLAN_CCMP_REPLAY_FAIL_FLT  BIT1              /* CCMP重放攻击过滤 */
#define  WLAN_OTHER_CTRL_FRAME_FLT BIT2               /* direct控制帧过滤 */
#define  WLAN_BCMC_MGMT_OTHER_BSS_FLT BIT3            /* 其他BSS网络的组播管理帧过滤 */
#define  WLAN_UCAST_MGMT_OTHER_BSS_FLT BIT4           /* 其他BSS网络的单播管理帧过滤 */

#define  WLAN_UCAST_DATA_OTHER_BSS_FLT BIT5           /* 其他BSS网络的单播数据帧过滤 */
#define  WLAN_CFG_OTHER_BSS_BEACON_FLT_EN BIT25       /* 其他BSS网络的Beacon帧过滤 */
#define  WLAN_CFG_WDS_FLT_EN BIT24                    /* WDS帧是否进行过滤 */
#define  WLAN_CFG_WDS_BCMC_FLT_EN BIT23               /* 广播WDS帧是否过滤开关 */
#define  WLAN_CFG_WDS_NON_DIRECT_FLT_EN BIT22         /* 非直接单播WDS帧是否过滤开关 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wlan_types_rom.h */
