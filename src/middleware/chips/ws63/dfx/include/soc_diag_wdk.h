/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: define file id and module id
 */

#ifndef SOC_DIAG_WDK_H
#define SOC_DIAG_WDK_H

#include "soc_module.h"
#include "log_def_wifi.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @ingroup  hct_diag
 * enumeration for file id
 */
typedef enum {
    /* DIAG */
    DIAG_FILE_ID_DIAG_MSG_C                     = 10000,
    DIAG_FILE_ID_DIAG_CMD_C                     = 10001,

    /* TEST */
    DIAG_FILE_ID_TEST_DIAG_C                    = 16300,
    DIAG_FILE_ID_TEST_DIAG_D                    = 16301,
    /* THE MAX VALUE OF FILE_ID */
    DIAG_FILE_ID_MAX                            = 16383,
} diag_file_id_t;

typedef enum {
    /* 受全局帧开关限制 */
    SOC_DIAG_MSG_ID_WIFI_START = 0x500, /* 起始值 0x500 */
    SOC_DIAG_MSG_ID_WIFI_RX_DSCR = SOC_DIAG_MSG_ID_WIFI_START,     /* 接收描述符 */
    SOC_DIAG_MSG_ID_WIFI_TX_DSCR,     /* 发送描述符 */
    SOC_DIAG_MSG_ID_WIFI_RX_80211_FRAME, /* 80211帧(不包括beacon) */
    SOC_DIAG_MSG_ID_WIFI_TX_80211_FRAME,
    SOC_DIAG_MSG_ID_WIFI_RX_BEACON, /* beacon */
    SOC_DIAG_MSG_ID_WIFI_TX_BEACON,
    SOC_DIAG_MSG_ID_WIFI_RX_CB,
    SOC_DIAG_MSG_ID_WIFI_TX_CB,
    SOC_DIAG_MSG_ID_WIFI_RX_HMAC_CB,
    SOC_DIAG_MSG_ID_WIFI_RX_ETH_FRAME, /* 以太网帧 */
    SOC_DIAG_MSG_ID_WIFI_TX_ETH_FRAME,
    SOC_DIAG_MSG_ID_WIFI_RX_CSI, /* csi信息 */

    SOC_DIAG_MSG_ID_WIFI_FRAME_MAX = 0x520, /* 预留帧类最大值，以上受全局帧开关限制 */

    /* 默认打开上报，无开关限制 */
    SOC_DIAG_MSG_ID_WIFI_IRQ = 0x521,         /* 中断信息 */
    SOC_DIAG_MSG_ID_WIFI_TIMER,       /* 软件定时器信息 */
    SOC_DIAG_MSG_ID_WIFI_MEMPOOL,     /* 某一内存池及所有子池使用信息 */
    SOC_DIAG_MSG_ID_WIFI_MEMBLOCK,    /* 某一内存池的所有内存块使用信息及内容，或者任意连续内存块的内容 */
    SOC_DIAG_MSG_ID_WIFI_EVENT_QUEUE, /* 当前所有存在事件的事件队列中的事件个数和事件头信息 */
    SOC_DIAG_MSG_ID_WIFI_MPDU_NUM,
    SOC_DIAG_MSG_ID_WIFI_PHY_STAT,          /* phy收发包统计值 */
    SOC_DIAG_MSG_ID_WIFI_MACHW_STAT,        /* mac收发包统计值 */
    SOC_DIAG_MSG_ID_WIFI_MGMT_STAT,         /* 管理帧统计 */
    SOC_DIAG_MSG_ID_WIFI_DBB_ENV_PARAM,     /* 空口环境类维测参数 */
    SOC_DIAG_MSG_ID_WIFI_USR_QUEUE_STAT,    /* 用户队列统计信息 */
    SOC_DIAG_MSG_ID_WIFI_VAP_STAT,          /* vap吞吐相关统计信息 */
    SOC_DIAG_MSG_ID_WIFI_USER_THRPUT_PARAM, /* 影响用户实时吞吐相关的统计信息 */
    SOC_DIAG_MSG_ID_WIFI_AMPDU_STAT,        /* ampdu业务流程统计信息 */
    SOC_DIAG_MSG_ID_WIFI_HARDWARE_INFO,     /* hal mac相关信息 */
    SOC_DIAG_MSG_ID_WIFI_USER_QUEUE_INFO,   /* 用户队列信息 */

    /* VAP与USER等结构体的信息，上报整个结构体内存 */
    SOC_DIAG_MSG_ID_WIFI_HMAC_VAP = 0x531,
    SOC_DIAG_MSG_ID_WIFI_DMAC_VAP,
    SOC_DIAG_MSG_ID_WIFI_HMAC_USER,
    SOC_DIAG_MSG_ID_WIFI_MAC_USER,
    SOC_DIAG_MSG_ID_WIFI_DMAC_USER,

    /* hmac 与 dmac vap中部分成员的大小 */
    SOC_DIAG_MSG_ID_WIFI_HMAC_VAP_MEMBER_SIZE = 0x53a,
    SOC_DIAG_MSG_ID_WIFI_DMAC_VAP_MEMBER_SIZE,

    /* 03PILOT寄存器 */
    SOC_DIAG_MSG_ID_WIFI_PHY_BANK1_PILOT_INFO = 0x540, /* PHY寄存器 BANK1 信息 */
    SOC_DIAG_MSG_ID_WIFI_PHY_BANK2_PILOT_INFO, /* PHY寄存器 BANK2 信息 */
    SOC_DIAG_MSG_ID_WIFI_PHY_BANK3_PILOT_INFO, /* PHY寄存器 BANK3 信息 */
    SOC_DIAG_MSG_ID_WIFI_PHY_BANK4_PILOT_INFO, /* PHY寄存器 BANK4 信息 */
    SOC_DIAG_MSG_ID_WIFI_PHY_BANK5_PILOT_INFO, /* PHY寄存器 BANK5 信息 */
    SOC_DIAG_MSG_ID_WIFI_PHY_BANK6_PILOT_INFO, /* PHY寄存器 BANK6 信息 */
    SOC_DIAG_MSG_ID_WIFI_PHY0_CTRL_PILOT_INFO, /* PHY寄存器 PHY0_CTRL 信息 */
    SOC_DIAG_MSG_ID_WIFI_PHY1_CTRL_PILOT_INFO, /* PHY寄存器 PHY1_CTRL 信息 */

    SOC_DIAG_MSG_ID_WIFI_MAC_CTRL0_BANK_PILOT_INFO = 0x54a, /* MAC寄存器 CTRL0 信息 */
    SOC_DIAG_MSG_ID_WIFI_MAC_CTRL1_BANK_PILOT_INFO, /* MAC寄存器 CTRL1 信息 */
    SOC_DIAG_MSG_ID_WIFI_MAC_CTRL2_BANK_PILOT_INFO, /* MAC寄存器 CTRL2 信息 */
    SOC_DIAG_MSG_ID_WIFI_MAC_RD0_BANK_PILOT_INFO,   /* MAC寄存器 RD0 信息（存在几字节信息不能读取） */
    SOC_DIAG_MSG_ID_WIFI_MAC_RD1_BANK_PILOT_INFO,   /* MAC寄存器 RD1 信息 */
    SOC_DIAG_MSG_ID_WIFI_MAC_LUT0_BANK_PILOT_INFO,  /* MAC寄存器 LUT0 信息 */
    SOC_DIAG_MSG_ID_WIFI_MAC_WLMAC_CTRL_PILOT_INFO, /* MAC寄存器 wlmac_ctrl 信息 */

    SOC_DIAG_MSG_ID_WIFI_ABB_CALI_WL_CTRL0_PILOT_INFO = 0x555, /* PHY寄存器 ABB_CALI_WL_CTRL0 信息 */
    SOC_DIAG_MSG_ID_WIFI_ABB_CALI_WL_CTRL1_PILOT_INFO, /* PHY寄存器 ABB_CALI_WL_CTRL1 信息 */

    SOC_DIAG_MSG_ID_WIFI_RF0_REG_INFO = 0x55a,        /* 03 RF0寄存器 */
    SOC_DIAG_MSG_ID_WIFI_RF1_REG_INFO,        /* 03 RF1寄存器 */

    SOC_DIAG_MSG_ID_WIFI_SOC_RF_W_C0_CTL_PILOT_INFO = 0x560, /* SOC寄存器 RF_W_C0_CTL 信息 */
    SOC_DIAG_MSG_ID_WIFI_SOC_RF_W_C1_CTL_PILOT_INFO, /* SOC寄存器 RF_W_C1_CTL 信息 */
    SOC_DIAG_MSG_ID_WIFI_SOC_W_CTL_PILOT_INFO,       /* SOC寄存器 W_CTL 信息 */
    SOC_DIAG_MSG_ID_WIFI_SOC_COEX_CTL_PILOT_INFO,    /* SOC寄存器 COEX_CTL 信息 */

    SOC_DIAG_MSG_ID_WIFI_BUTT = 0x600
} soc_diag_msg_id_wifi;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of soc_diag_wdk.h */
