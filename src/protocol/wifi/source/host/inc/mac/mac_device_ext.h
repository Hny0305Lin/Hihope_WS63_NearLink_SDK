/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2023. All rights reserved.
 * Description: Header file of mac_device_etc.c.
 * Create: 2020-7-8
 */
#ifndef __MAC_DEVICE_EXT_H__
#define __MAC_DEVICE_EXT_H__

#include "mac_frame.h"

#include "mac_regdomain.h"
#include "wlan_spec.h"
#include "wlan_mib_hcm.h"
#include "wlan_types_common.h"
#include "frw_osal.h"

#include "oam_ext_if.h"

#include "frw_ext_if.h"
#include "hal_common_ops.h"

#include "osal_types.h"
#include "osal_adapt.h"
#include "oal_ext_if.h"
#include "hal_ext_if.h"
#include "wlan_resource_common.h"
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    宏定义
*****************************************************************************/
#define MAC_NET_DEVICE_NAME_LENGTH          16
#define MAC_BAND_CAP_NAME_LENGTH            16
#define MAC_DATARATES_PHY_80211G_NUM        12

#define MAC_EXCEPTION_TIME_OUT              10000
#define MULTI_BSSID_BITMAP                  32
#define WLAN_USER_MAX_SUPP_RATES            16        /* 用于记录对端设备支持的速率最大个数 */

/* 复位状态 */
#define mac_device_get_cap_bw(_pst_device)      ((_pst_device)->device_cap.channel_width)
#define mac_device_get_nss_num(_pst_device)     ((_pst_device)->device_cap.nss_num)
#define mac_device_get_cap_ldpc(_pst_device)    ((_pst_device)->device_cap.ldpc_is_supp)
#define mac_device_get_cap_txstbc(_pst_device)  ((_pst_device)->device_cap.tx_stbc_is_supp)
#define mac_device_get_cap_rxstbc(_pst_device)  ((_pst_device)->device_cap.rx_stbc_is_supp)
#define mac_device_get_cap_subfer(_pst_device)  ((_pst_device)->device_cap.su_bfmer_is_supp)
#define mac_device_get_cap_subfee(_pst_device)  ((_pst_device)->device_cap.su_bfmee_is_supp)
#define mac_device_get_cap_mubfer(_pst_device)  ((_pst_device)->device_cap.mu_bfmer_is_supp)
#define mac_device_get_cap_mubfee(_pst_device)  ((_pst_device)->device_cap.mu_bfmee_is_supp)
#define mac_device_get_cap_rx_mtid(_pst_device) ((_pst_device)->device_cap.mtid_aggregation_rx_support)
#define mac_device_get_cap_32bitmap(_pst_device)    ((_pst_device)->device_cap.ba_bitmap_support_32bit)
#ifdef _PRE_WLAN_FEATURE_SMPS
#define mac_device_get_mode_smps(_pst_device)   ((_pst_device)->mac_smps_mode)
#endif

#define MAX_PNO_SSID_COUNT              16
#define MAX_PNO_REPEAT_TIMES            4
#define PNO_SCHED_SCAN_INTERVAL         (60 * 1000)

/* DMAC SCANNER 扫描模式 */
#define MAC_SCAN_FUNC_MEAS              0x1
#define MAC_SCAN_FUNC_STATS             0x2
#define MAC_SCAN_FUNC_RADAR             0x4
#define MAC_SCAN_FUNC_BSS               0x8
#define MAC_SCAN_FUNC_P2P_LISTEN        0x10
#define MAC_SCAN_FUNC_ALL      (MAC_SCAN_FUNC_MEAS | MAC_SCAN_FUNC_STATS | MAC_SCAN_FUNC_RADAR | MAC_SCAN_FUNC_BSS)

#define MAC_SCAN_CHANNEL_INTERVAL_DEFAULT               6           /* 间隔6个信道，切回工作信道工作一段时间 */
#define MAC_WORK_TIME_ON_HOME_CHANNEL_DEFAULT           110         /* 背景扫描时，返回工作信道工作的时间 */
#define MAC_SCAN_CHANNEL_INTERVAL_PERFORMANCE           2           /* 间隔2个信道，切回工作信道工作一段时间 */
#define MAC_WORK_TIME_ON_HOME_CHANNEL_PERFORMANCE       60          /* WLAN未关联，P2P关联，返回工作信道工作的时间 */

#ifdef _PRE_WLAN_FEATURE_DBAC
#define mac_dbac_enable(_pst_device) (_pst_device->en_dbac_enabled == OAL_TRUE)
#else
#define mac_dbac_enable(_pst_device) (OAL_FALSE)
#endif

#define MAC_FCS_DBAC_IGNORE                 0   /* 不是DBAC场景 */
#define MAC_FCS_DBAC_NEED_CLOSE             1   /* DBAC需要关闭 */
#define MAC_FCS_DBAC_NEED_OPEN              2   /* DBAC需要开启 */

#define BTCOEX_BSS_NUM_IN_BLACKLIST         16

/* 特性宏END，第二阶段需要挪走 */

/*****************************************************************************
    枚举定义
*****************************************************************************/
typedef enum {
    MAC_CH_TYPE_NONE      = 0,
    MAC_CH_TYPE_PRIMARY   = 1,
    MAC_CH_TYPE_SECONDARY = 2,

    MAC_CH_TYPE__BUTT
} mac_ch_type_enum;
typedef osal_u8 mac_ch_type_enum_uint8;

typedef enum {
    MAC_CHAN_NOT_SUPPORT = 0,      /* 管制域不支持该信道 */
    MAC_CHAN_AVAILABLE_ALWAYS,     /* 信道一直可以使用 */
    MAC_CHAN_AVAILABLE_TO_OPERATE, /* 经过检测(CAC, etc...)后，该信道可以使用 */
    MAC_CHAN_DFS_REQUIRED,         /* 该信道需要进行雷达检测 */
    MAC_CHAN_BLOCK_DUE_TO_RADAR,   /* 由于检测到雷达导致该信道变的不可用 */

    MAC_CHAN_STATUS_BUTT
} mac_chan_status_enum;
typedef osal_u8 mac_chan_status_enum_uint8;

/* device reset同步子类型枚举 */
typedef enum {
    MAC_RESET_SWITCH_SET_TYPE,
    MAC_RESET_SWITCH_GET_TYPE,
    MAC_RESET_STATUS_GET_TYPE,
    MAC_RESET_STATUS_SET_TYPE,
    MAC_RESET_SWITCH_SYS_TYPE = MAC_RESET_SWITCH_SET_TYPE,
    MAC_RESET_STATUS_SYS_TYPE = MAC_RESET_STATUS_SET_TYPE,

    MAC_RESET_SYS_TYPE_BUTT
} mac_reset_sys_type_enum;
typedef osal_u8 mac_reset_sys_type_enum_uint8;

typedef enum {
    MAC_REPORT_INFO_FLAGS_HARDWARE_INFO =           BIT0,
    MAC_REPORT_INFO_FLAGS_QUEUE_INFO =              BIT1,
    MAC_REPORT_INFO_FLAGS_MEMORY_INFO =             BIT2,
    MAC_REPORT_INFO_FLAGS_EVENT_INFO =              BIT3,
    MAC_REPORT_INFO_FLAGS_VAP_INFO =                BIT4,
    MAC_REPORT_INFO_FLAGS_USER_INFO =               BIT5,
    MAC_REPORT_INFO_FLAGS_TXRX_PACKET_STATISTICS =  BIT6,
    MAC_REPORT_INFO_FLAGS_BUTT,
} mac_report_info_flags;
typedef osal_u8 mac_report_info_flags_enum_uint8;

typedef enum {
    MAC_DEVICE_DISABLE = 0,
    MAC_DEVICE_2G,
    MAC_DEVICE_5G,
    MAC_DEVICE_2G_5G,

    MAC_DEVICE_BUTT,
} mac_device_radio_cap_enum;
typedef osal_u8 mac_device_radio_cap_enum_uint8;

/* 特性枚举START，第二阶段需要挪走 */
typedef enum {
    MAC_DFR_TIMER_STEP_1 = 0,
    MAC_DFR_TIMER_STEP_2 = 1,
} mac_dfr_timer_step_enum;
typedef osal_u8 mac_dfr_timer_step_enum_uint8;

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* 设置发送描述符帧类型枚举 */
typedef enum {
    MAC_SET_DSCR_TYPE_UCAST_DATA  = 0,  /* 单播数据帧 */
    MAC_SET_DSCR_TYPE_MCAST_DATA,       /* 组播数据帧 */
    MAC_SET_DSCR_TYPE_BCAST_DATA,       /* 广播数据帧 */
    MAC_SET_DSCR_TYPE_UCAST_MGMT_2G,    /* 单播管理帧,仅2G */
    MAC_SET_DSCR_TYPE_MBCAST_MGMT_2G,   /* 组、广播管理帧,仅2G */

    MAC_SET_DSCR_TYPE_BUTT,
} mac_set_dscr_frame_type_enum;
typedef td_u8 mac_set_dscr_frame_type_enum_uint8;
#endif

typedef enum {
    MAC_SDT_MODE_WRITE = 0,
    MAC_SDT_MODE_READ,
    MAC_SDT_MODE_WRITE16,
    MAC_SDT_MODE_READ16,

    MAC_SDT_MODE_BUTT
} mac_sdt_rw_mode_enum;
typedef osal_u8 mac_sdt_rw_mode_enum_uint8;

typedef enum {
    MAC_CSA_FLAG_NORMAL = 0,
    MAC_CSA_FLAG_START_DEBUG, /* 固定csa ie 在beacon帧中 */
    MAC_CSA_FLAG_CANCLE_DEBUG,

    MAC_CSA_FLAG_BUTT
} mac_csa_flag_enum;
typedef osal_u8 mac_csa_flag_enum_uint8;

/* 配置命令使用，mimo-siso切换mode */
typedef enum {
    MAC_M2S_MODE_QUERY        = 0,  /* 参数查询模式 */
    MAC_M2S_MODE_MSS          = 1,  /* MSS下发模式 */
    MAC_M2S_MODE_DELAY_SWITCH = 2,  /* 延迟切换测试模式 */
    MAC_M2S_MODE_SW_TEST      = 3,  /* 软切换测试模式,测试siso和mimo */
    MAC_M2S_MODE_HW_TEST      = 4,  /* 硬切换测试模式,测试siso和mimo */
    MAC_M2S_MODE_RSSI         = 5,  /* rssi切换 */

    MAC_M2S_MODE_BUTT,
} mac_m2s_mode_enum;
typedef osal_u8 mac_m2s_mode_enum_uint8;

/* 配置命令使用，期望切换状态 */
typedef enum {
    MAC_M2S_COMMAND_STATE_SISO_C0   = 0,
    MAC_M2S_COMMAND_STATE_SISO_C1   = 1,
    MAC_M2S_COMMAND_STATE_MIMO      = 2,
    MAC_M2S_COMMAND_STATE_MISO_C0   = 3,
    MAC_M2S_COMMAND_STATE_MISO_C1   = 4,

    MAC_M2S_COMMAND_STATE_BUTT,
} mac_m2s_command_state_enum;
typedef osal_u8 mac_m2s_command_state_enum_uint8;

/* MSS使用时命令形式 */
typedef enum {
    MAC_M2S_COMMAND_MODE_SET_AUTO = 0,
    MAC_M2S_COMMAND_MODE_SET_SISO_C0 = 1,
    MAC_M2S_COMMAND_MODE_SET_SISO_C1 = 2,
    MAC_M2S_COMMAND_MODE_SET_MIMO = 3,
    MAC_M2S_COMMAND_MODE_GET_STATE = 4,

    MAC_M2S_COMMAND_MODE_BUTT,
} mac_m2s_command_mode_enum;
typedef osal_u8 mac_m2s_command_mode_enum_uint8;

/* 配置接收功率参数 */
typedef enum {
    HAL_M2S_RSSI_SHOW_TH,
    HAL_M2S_RSSI_SHOW_MGMT,
    HAL_M2S_RSSI_SHOW_DATA,
    HAL_M2S_RSSI_SET_MIN_TH,
    HAL_M2S_RSSI_SET_DIFF_TH,
} hal_dev_rssi_enum;
typedef osal_u8 hal_dev_rssi_enum_uint8;

typedef enum {
    MAC_PM_DEBUG_SISO_RECV_BCN = 0,
    MAC_PM_DEBUG_NO_PS_FRM_INT = 1,
    MAC_PM_DEBUG_APF = 2,
    MAC_PM_DEBUG_AO = 3,

    MAC_PM_DEBUG_CFG_BUTT
} mac_pm_debug_cfg_enum;
typedef osal_u8 mac_pm_debug_cfg_enum_uint8;

typedef enum {
    MAC_DBDC_CHANGE_HAL_DEV = 0,   /* vap change hal device hal vap */
    MAC_DBDC_SWITCH         = 1,   /* DBDC软件开关 */
    MAC_FAST_SCAN_SWITCH    = 2,   /* 并发扫描开关 */
    MAC_DBDC_STATUS         = 3,   /* DBDC状态查询 */

    MAC_DBDC_CMD_BUTT
} mac_dbdc_cmd_enum;
typedef osal_u8 mac_dbdc_cmd_enum_uint8;

typedef enum {
    MAC_ACS_RSN_INIT,
    MAC_ACS_RSN_LONG_TX_BUF,
    MAC_ACS_RSN_LARGE_PER,
    MAC_ACS_RSN_MWO_DECT,
    MAC_ACS_RSN_RADAR_DECT,

    MAC_ACS_RSN_BUTT
} mac_acs_rsn_enum;
typedef osal_u8 mac_acs_rsn_enum_uint8;

typedef enum {
    MAC_ACS_SW_NONE = 0x0,
    MAC_ACS_SW_INIT = 0x1,
    MAC_ACS_SW_DYNA = 0x2,
    MAC_ACS_SW_BOTH = 0x3,

    MAC_ACS_SW_BUTT
} mac_acs_sw_enum;
typedef osal_u8 mac_acs_sw_enum_uint8;

typedef enum {
    MAC_ACS_SET_CH_DNYA = 0x0,
    MAC_ACS_SET_CH_INIT = 0x1,

    MAC_ACS_SET_CH_BUTT
} mac_acs_set_ch_enum;
typedef osal_u8 mac_acs_set_ch_enum_uint8;

typedef enum {
    MAC_RSSI_LIMIT_SHOW_INFO,
    MAC_RSSI_LIMIT_ENABLE,
    MAC_RSSI_LIMIT_DELTA,
    MAC_RSSI_LIMIT_THRESHOLD,

    MAC_RSSI_LIMIT_TYPE_BUTT
} mac_rssi_limit_type_enum;
typedef osal_u8 mac_rssi_limit_type_enum_uint8;

typedef struct {
    oal_bool_enum_uint8     en_11k;
    oal_bool_enum_uint8     en_11v;
    oal_bool_enum_uint8     en_11r;
    oal_bool_enum_uint8     en_11r_ds;
} mac_device_voe_custom_stru;

/*****************************************************************************
    结构体定义
*****************************************************************************/
typedef struct {
    osal_u8                 rx_ba_lut_idx_table[MAC_RX_BA_LUT_BMAP_LEN];        /* 接收端BA LUT表位图 */
} mac_lut_table_stru;

/* chip结构体 */
typedef struct {
    /* CHIP下挂的DEV，仅记录对应的ID索引值 */
    osal_u8                 device_id[WLAN_SERVICE_DEVICE_MAX_NUM_PER_CHIP];
    osal_u8                 device_nums; /* chip下device的数目 */
    osal_u8                 chip_id;     /* 芯片ID */
    oal_bool_enum_uint8     chip_state;  /* 标识是否已初始化 */
    osal_u32                chip_ver;    /* 芯片版本 */
    hal_to_dmac_chip_stru   *hal_chip;   /* 硬mac结构指针，HAL提供，用于逻辑和物理chip的对应 */
    mac_lut_table_stru      lut_table;   /* 软件维护LUT表资源的结构体 */
    void                    *p_alg_priv;     /* chip级别算法私有结构体 */

    /* 用户相关成员变量 */
    frw_timeout_stru        active_user_timer;   /* 用户活跃定时器 */
} mac_chip_stru;

/* board结构体 */
typedef struct {
    mac_chip_stru           chip[WLAN_CHIP_MAX_NUM_PER_BOARD];    /* board挂接的芯片 */
    osal_u8                 chip_id_bitmap;                        /* 标识chip是否被分配的位图 */
    osal_u8                 auc_resv[3];                              /* 字节对齐 */
} mac_board_stru;

typedef struct {
    osal_u16 num_networks; /* 记录当前信道下扫描到的BSS个数 */
    osal_u8 resv[2];  /* 预留2字节对齐 */
    osal_u8 bssid_array[WLAN_MAX_SCAN_BSS_PER_CH][WLAN_MAC_ADDR_LEN]; /* 记录当前信道下扫描到的所有BSSID */
} mac_bss_id_list_stru;

/* device reset事件同步结构体 */
typedef struct {
    mac_reset_sys_type_enum_uint8 reset_sys_type;   /* 复位同步类型 */
    osal_u8 value;                                  /* 同步信息值 */
    osal_u8 resv[2];                                /* 预留2字节对齐 */
} mac_reset_sys_stru;

typedef struct {
    osal_u16 num_networks;
    mac_ch_type_enum_uint8 ch_type;
#ifdef _PRE_WLAN_FEATURE_DFS
    mac_chan_status_enum_uint8 ch_status;
#else
    osal_u8 auc_resv[1];
#endif
} mac_ap_ch_info_stru;

typedef struct {
    /* 支持2*2 */                                 /* 支持MU-MIMO */
    wlan_nss_enum_uint8 nss_num : 4;            /* device Nss 空间流最大个数 */
    wlan_bw_cap_enum_uint8 channel_width : 4;   /* 支持的带宽 */

    oal_bool_enum_uint8 nb_is_supp : 1;         /* 支持窄带 */
    oal_bool_enum_uint8 is_supp_1024qam : 1;    /* 支持1024QAM速率 */
    oal_bool_enum_uint8 mc_is_supp_80211 : 1;   /* 支持80211 mc */
    oal_bool_enum_uint8 ldpc_is_supp : 1;       /* 是否支持接收LDPC编码的包 */
    oal_bool_enum_uint8 tx_stbc_is_supp : 1;    /* 是否支持最少2x1 STBC发送 */
    oal_bool_enum_uint8 rx_stbc_is_supp : 1;    /* 是否支持stbc接收,支持2个空间流 */
    oal_bool_enum_uint8 su_bfmer_is_supp : 1;   /* 是否支持单用户beamformer */
    oal_bool_enum_uint8 su_bfmee_is_supp : 1;   /* 是否支持单用户beamformee */

    oal_bool_enum_uint8 mu_bfmer_is_supp : 1;   /* 是否支持多用户beamformer */
    oal_bool_enum_uint8 mu_bfmee_is_supp : 1;   /* 是否支持多用户beamformee */
    oal_bool_enum_uint8 switch_11ax : 1;        /* 11ax开关 */
    oal_bool_enum_uint8 ba_bitmap_support_32bit : 1;
    osal_u8 mtid_aggregation_rx_support         : 3;
    oal_bool_enum_uint8 resv                    : 1;

    oal_bool_enum_uint8 resv1;
} mac_device_capability_stru;

typedef struct {
    osal_u32            cmd_bit_map;
    osal_u8             srb_switch; /* siso收beacon开关 */
    osal_u8             dto_switch; /* 动态tbtt offset开关 */
    osal_u8             nfi_switch;
    osal_u8             apf_switch;
    osal_u8             ao_switch;
} mac_pm_debug_cfg_stru;

#ifdef _PRE_WLAN_FEATURE_11AX
/* 80211ax新增的ELEMENT结构 */
typedef struct {
    osal_u8                   ext_eid;
    osal_u8                   element_body_len;
    osal_u8                   *element_body;
} mac_11ax_element_stru;
#endif

typedef struct {
    osal_u32                         tx_seqnum;          /* 最近一次tx上报的SN号 */
    osal_u16                         seqnum_used_times;  /* 软件使用了tx_seqnum的次数 */
    osal_u16                         incr_constant;      /* 维护非Qos 分片帧的递增常量 */
} mac_tx_seqnum_struc;

/* hmac侧扫描结果超过阈值信息的结构体 */
typedef struct {
    osal_u8 exceed_count; // 当前超过阈值的次数
    osal_u8 exceed_limit; // 超过阈值的限制次数 当exceed_count >= exceed_limit时, 信道随机化
    osal_bool randomize;  // 信道是否需要随机化
} hmac_bss_list_exceed_info_stru;

/* 存储在hmac device下的扫描结果维护的结构体 */
typedef struct {
    osal_spinlock lock;
    struct osal_list_head bss_list_head;
    osal_u32          bss_num;
    hmac_bss_list_exceed_info_stru bss_list_exceed_info;
} hmac_bss_mgmt_stru;

typedef struct {
    wlan_m2s_mgr_vap_stru m2s_blacklist[WLAN_M2S_BLACKLIST_MAX_NUM];
    osal_u8 blacklist_cnt;
} mac_m2s_ie_stru;

typedef struct {
    mac_rssi_limit_type_enum_uint8 rssi_limit_type; /* 4种参数类型 */
    oal_bool_enum_uint8 rssi_limit_enable_flag;
    osal_s8 rssi;
    osal_s8 rssi_delta;
} mac_cfg_rssi_limit_stru;

#ifdef _PRE_WLAN_FEATURE_NEGTIVE_DET
typedef struct {
    oal_bool_enum_uint8 is_pk_mode;
    wlan_bw_cap_enum_uint8 curr_bw_cap;             /* 目前使用的带宽，本参数仅在lagency sta模式下生效 */
    wlan_protocol_cap_enum_uint8 curr_protocol_cap; /* 目前使用的协议模式，本参数仅在lagency sta模式下生效 */
    wlan_nss_enum_uint8 curr_num_spatial_stream;    /* 目前单双流的计数 */

    osal_u32 tx_bytes; /* WIFI 业务发送帧统计 */
    osal_u32 rx_bytes; /* WIFI 业务接收帧统计 */
    osal_u32 dur_time; /* 统计时间间隔 */
} mac_cfg_pk_mode_stru;
#endif

/* 黑名单 */
typedef struct {
    osal_u8                         auc_mac_addr[OAL_MAC_ADDR_LEN];         /* mac地址   */
    osal_u8                         reserved[2];                        /* 字节对齐 */
    osal_u32                        cfg_time;                            /* 加入黑名单的时间 */
    osal_u32                        aging_time;                          /* 老化时间 */
    osal_u32                        drop_counter;                        /* 报文丢弃统计       */
} mac_blacklist_stru;

/* 自动黑名单 */
typedef struct {
    osal_u8                         auc_mac_addr[OAL_MAC_ADDR_LEN];         /* mac地址   */
    osal_u8                         reserved[2];                        /* 字节对齐 */
    osal_u32                        cfg_time;                            /* 初始时间 */
    osal_u32                        asso_counter;                        /* 关联计数 */
} mac_autoblacklist_stru;

/* 自动黑名单信息 */
typedef struct {
    osal_u8                         enabled;                             /* 使能标志 0:未使能  1:使能 */
    osal_u8                         list_num;                               /* 有多少个自动黑名单          */
    osal_u8                         reserved[2];                        /* 字节对齐 */
    osal_u32                        ul_threshold;                           /* 门限 */
    osal_u32                        reset_time;                          /* 重置时间     */
    osal_u32                        aging_time;                          /* 老化时间     */
    mac_autoblacklist_stru          autoblack_list[WLAN_BLACKLIST_MAX]; /* 自动黑名单表 */
} mac_autoblacklist_info_stru;

/* 黑白名单信息 */
typedef struct {
    osal_u8                         mode;                                /* 黑白名单模式   */
    osal_u8                         list_num;                            /* 名单数    */
    osal_u8                         blacklist_vap_index;                 /* 黑名单vap index */
    osal_u8                         blacklist_device_index;              /* 黑名单device index */
    mac_autoblacklist_info_stru     autoblacklist_info;                  /* 自动黑名单信息 */
    mac_blacklist_stru              black_list[WLAN_BLACKLIST_MAX];     /* 有效黑白名单表 */
} mac_blacklist_info_stru;

typedef struct {
    osal_u8                    p2p_mode;            /* 当前device下的P2P_模式 0 无P2P设备 1 DEV 2 GO 3 GC */
    mac_vap_state_enum_uint8   last_vap_state;   /* P2P0/P2P_CL共用VAP结构.保存VAP进入listen前的状态 */
    osal_u8                    roc_need_switch;  /* remain on channel后需要切回原信道 */
    osal_u8                    p2p_ps_pause;        /* P2P 节能是否处于pause状态 */
    oal_net_device_stru       *p2p_net_device;  /* P2P 共存场景下主net_device(p2p0) 指针 */
    osal_u64                   ull_send_action_id;  /* P2P action id/cookie */
    osal_u64                   ull_last_roc_id;
    oal_ieee80211_channel_stru st_listen_channel;
    oal_nl80211_channel_type   listen_channel_type;
    oal_net_device_stru       *primary_net_device; /* P2P 共存场景下主net_device(wlan0) 指针 */
    oal_net_device_stru       *second_net_device;  /* 信道跟随增加,后续不使用可以删除 */
} mac_p2p_info_stru;

/* 扫描完成事件返回状态码 */
typedef enum {
    MAC_SCAN_SUCCESS = 0,       /* 扫描成功 */
    MAC_SCAN_PNO     = 1,       /* pno扫描结束 */
    MAC_SCAN_TIMEOUT = 2,       /* 扫描超时 */
    MAC_SCAN_REFUSED = 3,       /* 扫描被拒绝 */
    MAC_SCAN_ABORT   = 4,       /* 终止扫描 */
    MAC_SCAN_ABORT_SYNC = 5,    /* 扫描被终止同步状态，用于上层去关联命令时强制abort不往内核上报等dmac响应abort往上报 */
    MAC_SCAN_STATUS_BUTT,       /* 无效状态码，初始化时使用此状态码 */
} mac_scan_status_enum;
typedef osal_u8 mac_scan_status_enum_uint8;

/* 扫描结果 */
typedef struct {
    mac_scan_status_enum_uint8  scan_rsp_status;
    osal_u8                     resv[3];
    osal_u64                    cookie;
} mac_scan_rsp_stru;

/* 扫描参数结构体 */
typedef osal_void (*mac_scan_cb_fn)(osal_void *p_scan_record);

#pragma pack(push, 1)
/* 上报的扫描结果的扩展信息，放在上报host侧的管理帧netbuf的后面 */
typedef struct {
    osal_s32                            rssi;                       /* 信号强度 */
    osal_s8                             ant0_rssi;                  /* 天线0的rssi */
    osal_s8                             ant1_rssi;                  /* 天线1的rssi */
    wlan_mib_desired_bsstype_enum_uint8 bss_type;                   /* 扫描到的bss类型 */

    osal_u8                             channel;                    /* 当前扫描的信道 */
    osal_s8                             snr_ant0;                   /* ant0 SNR */
    osal_s8                             snr_ant1;                   /* ant1 SNR */

    osal_u8                             resv[2];                    /* 预留字段 */

    osal_u32                            parent_tsf;                 /* 收帧TSF Timer */

    wlan_nss_enum_uint8                 support_max_nss;            /* 该AP支持的最大空间流数 */
    oal_bool_enum_uint8                 support_opmode;             /* 该AP是否支持OPMODE */
    osal_u8                             num_sounding_dim;           /* 该AP发送txbf的天线数 */
    osal_u8                             resv1;                      /* 预留字段 */
} mac_scanned_result_extend_info_stru;
#pragma pack(pop)

/* 扫描运行结果记录 */
typedef struct {
    hmac_bss_mgmt_stru          bss_mgmt;                            /* 存储扫描BSS结果的管理结构 */
    wlan_scan_chan_stats_stru   chan_results[WLAN_MAX_CHANNEL_NUM]; /* 信道统计/测量结果 */
    osal_u8                     chan_numbers;                        /* 此次扫描总共需要扫描的信道个数 */
    osal_u8                     device_id : 4;
    osal_u8                     chip_id   : 4;
    osal_u8                     vap_id;                              /* 本次执行扫描的vap id */
    mac_scan_status_enum_uint8  scan_rsp_status;                     /* 本次扫描完成返回的状态码，是成功还是被拒绝 */

    oal_time_us_stru            scan_start_timestamp;                /* 扫描维测使用 */
    mac_scan_cb_fn              p_fn_cb;                                /* 此次扫描结束的回调函数指针 */

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    osal_u8                     is_any_scan;
    osal_u8                     n_is_mesh_sta_scan;             /* 是否是mesh sta的扫描 */
    osal_u8                     resv[2];                        /* 2 保留字节 */
#endif

    osal_u64                    ull_cookie;                     /* 保存P2P 监听结束上报的cookie 值 */
    mac_vap_state_enum_uint8    vap_last_state;              /* 保存VAP进入扫描前的状态,AP/P2P GO模式下20/40M扫描专用 */
    osal_u64                    scan_start_time;             /* 扫描起始时间戳 */
    // 增加记录扫描类型，以便识别CHBA RRM扫描的相关处理
    osal_u8 is_chan_meas_scan;
    osal_u8 rsv[3];     /* 3个保留字节 */
} hmac_scan_record_stru;

/* 扫描相关相关控制信息 */
typedef struct {
    /* scan 相关控制信息 */
    oal_bool_enum_uint8                     is_scanning;                 /* host侧的扫描请求是否正在执行 */
    osal_u8 is_random_mac_addr_scan;   /* 是否为随机mac addr扫描，0：关闭(默认) 1:条件（mac ie）开启；2：强制开启 */
    oal_bool_enum_uint8                     complete;                    /* 内核普通扫描请求是否完成标志 */
    oal_bool_enum_uint8                     sched_scan_complete;         /* 调度扫描是否正在运行标记 */

    oal_cfg80211_scan_request_stru          *request;                   /* 内核下发的扫描请求结构体 */
    oal_cfg80211_sched_scan_request_stru    *sched_scan_req;            /* 内核下发的调度扫描请求结构体 */
    osal_wait wait_queue;
    osal_mutex                              scan_request_mutexlock;     /* scan_request使用互斥锁 */

    frw_timeout_stru                        scan_timeout;                /* 扫描模块host侧的超时保护所使用的定时器 */
    osal_u8                                 random_mac[WLAN_MAC_ADDR_LEN]; /* 扫描时候用的随机MAC */
    osal_u8                                 auc_resv[2];

    /* 扫描运行记录管理信息，包括扫描结果和发起扫描者的相关信息 */
    hmac_scan_record_stru                   scan_record_mgmt;

    /* 记录请求信道列表的map,11b模式下收到非当前信道扫描帧过滤使用,5g管理帧不使用11b速率 */
    osal_u32                                scan_2g_ch_list_map;
} hmac_scan_stru;

/* 扫描到的BSS描述结构体 */
typedef struct {
    /* 基本信息 */
    wlan_mib_desired_bsstype_enum_uint8 bss_type;                        /* bss网络类型 */
    osal_u8                             dtim_period;                     /* dtime周期 */
    osal_u8                             dtim_cnt;                        /* dtime cnt */
    oal_bool_enum_uint8                 en_11ntxbf;                         /* 11n txbf */

    oal_bool_enum_uint8                 new_scan_bss;                    /* 是否是新扫描到的BSS */
    osal_u8                             multi_bssid_capa;                   /* AP的multiple bssid能力位 */
    osal_s8                             c_rssi;                             /* bss的信号强度 */
    osal_char                           ac_ssid[WLAN_SSID_MAX_LEN];         /* 网络ssid */
    osal_u16                            beacon_period;                   /* beacon周期 */
    osal_u16                            us_cap_info;                        /* 基本能力信息 */
    osal_u8                             auc_mac_addr[WLAN_MAC_ADDR_LEN];    /* 基础型网络 mac地址与bssid相同 */
    osal_u8                             bssid[WLAN_MAC_ADDR_LEN];       /* 网络bssid */
    mac_channel_stru                    st_channel;                         /* bss所在的信道 */
    osal_u8                             wmm_cap;                         /* 是否支持wmm */
    osal_u8                             uapsd_cap;                       /* 是否支持uapsd */
    oal_bool_enum_uint8                 desired;                         /* 标志位，此bss是否是期望的 */
    osal_u8                             num_supp_rates;                  /* 支持的速率集个数 */
    osal_u8                             supp_rates[WLAN_USER_MAX_SUPP_RATES]; /* 支持的速率集 */

#ifdef _PRE_WLAN_FEATURE_11D
    osal_s8                             ac_country[WLAN_COUNTRY_STR_LEN];   /* 国家字符串 */
    osal_u8                             resv2[1];
#endif

    /* 安全相关的信息 */
    osal_u8                             *rsn_ie;          /* 用于存放beacon、probe rsp的rsn ie */
    osal_u8                             *wpa_ie;          /* 用于存放beacon、probe rsp的wpa ie */
    osal_u8                             *ssid_ie;                        /* 网络ssid ie, 非字符串 */
    osal_u8                             ssid_ie_len;                     /* 网络ssid ie长度, 非字符串 */

    /* 11n 11ac信息 */
    oal_bool_enum_uint8                 ht_capable;                      /* 是否支持ht */
    oal_bool_enum_uint8                 vht_capable;                     /* 是否支持vht */
    oal_bool_enum_uint8                 vendor_vht_capable;              /* 是否支持hidden vendor vht */
    wlan_bw_cap_enum_uint8              bw_cap;                          /* 支持的带宽 0-20M 1-40M */
    wlan_channel_bandwidth_enum_uint8   channel_bandwidth;               /* 信道带宽 */
    osal_u8                             coex_mgmt_supp;                  /* 是否支持共存管理 */
    oal_bool_enum_uint8                 ht_ldpc;                         /* 是否支持ldpc */
    oal_bool_enum_uint8                 ht_stbc;                         /* 是否支持stbc */
    osal_u8                             wapi;
    osal_u8                             vendor_novht_capable;            /* 私有vendor中不需再携带 */
#ifdef _PRE_WLAN_MP13_DDC_BUGFIX
    oal_bool_enum_uint8                 ddc_whitelist_chip_oui;
#else
    osal_u8                             resv0;
#endif
    osal_u32                            timestamp;                       /* 更新此bss的时间戳 */
    osal_u8                             phy_type;
    osal_s8                             ac_rsni[2];
    osal_u8                             need_drop;                       /* 判断当前扫描结果是否需要过滤 */
    osal_u32                            parent_tsf;
#ifdef _PRE_WLAN_WIRELESS_EXT
    osal_u16                          ht_cap_info;                     /* ht capabilities 支持能力 */
    wlan_bw_cap_enum_uint8            real_bw;                         /* 只记录当前实际bw */
    osal_u8                           resv4;                           /* 表示预留字节对齐 */
    osal_u8                          *wps_ie;                          /* 用于存放beacon、probe rsp的wps ie */
    osal_u8                          *wapi_ie;                         /* 用于存放beacon、probe rsp的wapi_ie */
#endif

    oal_bool_enum_uint8                 btcoex_ps_blacklist_chip_oui;    /* ps机制one pkt帧类型需要修订为self-cts */
    wlan_nss_enum_uint8                 support_max_nss;                 /* 该AP支持的最大空间流数 */
    oal_bool_enum_uint8                 support_opmode;                  /* 该AP是否支持OPMODE */
    osal_u8                             num_sounding_dim;                /* 该AP发送txbf的天线数 */

    oal_bool_enum_uint8                 he_capable;                      /* 是否支持11ax */
#ifdef _PRE_WLAN_FEATURE_11AX
    mac_11ax_element_stru               he_capabilities;
#endif
    osal_u8                             bss_assoc_disallowed;            /* MBO IE中指示AP是否允许关联 */
    oal_bool_enum_uint8                 support_rrm;                     /* 是否支持RRM */
#ifdef _PRE_WLAN_FEATURE_1024QAM
    oal_bool_enum_uint8                 support_1024qam;
#endif
    osal_s8                             c_ant0_rssi;                        /* 天线0的rssi */
    osal_s8                             c_ant1_rssi;                        /* 天线1的rssi */

    mac_scanned_all_bss_info            all_bss_info;                       /* multiple bssid信息 */

    /* 管理帧信息 */
    osal_u32                            mgmt_len;                        /* 管理帧的长度 */
    osal_u8                             mgmt_buff[MAC_80211_QOS_HTC_4ADDR_FRAME_LEN];  /* 记录beacon帧或probe rsp帧 */
} mac_bss_dscr_stru;

typedef struct {
    wlan_mib_desired_bsstype_enum_uint8 bss_type;   /* 要扫描的bss类型 */
    wlan_scan_type_enum_uint8 scan_type;            /* 主动/被动 */
    osal_u8 bssid_num;                              /* 期望扫描的bssid个数 */
    osal_u8 ssid_num;                               /* 期望扫描的ssid个数 */

    osal_u8 sour_mac_addr[WLAN_MAC_ADDR_LEN];       /* probe req帧中携带的发送端地址 */
    osal_u8 p2p0_listen_channel;
    osal_u8 max_scan_count_per_channel;             /* 每个信道的扫描次数 */

    osal_u8 bssid[WLAN_SCAN_REQ_MAX_BSSID][WLAN_MAC_ADDR_LEN];  /* 期望的bssid */
    mac_ssid_stru mac_ssid_set[WLAN_SCAN_REQ_MAX_SSID];         /* 期望的ssid */

    osal_u8 max_send_probe_req_count_per_channel;   /* 每次信道发送扫描请求帧的个数，默认为1 */
    osal_u8 is_p2p0_scan : 1;                       /* 是否为p2p0 发起扫描 */
    osal_u8 is_radom_mac_saved : 1;                 /* 是否已经保存随机mac */
    osal_u8 radom_mac_saved_to_dev : 2;             /* 用于并发扫描 */
    osal_u8 desire_fast_scan : 1;                   /* 本次扫描期望使用并发 */
    osal_u8 bit_rsv : 3;                            /* 保留位 */

    oal_bool_enum_uint8 abort_scan_flag;            /* 终止扫描 */
    oal_bool_enum_uint8 is_random_mac_addr_scan;    /* 是否是随机mac addr扫描 */

    oal_bool_enum_uint8 need_switch_back_home_channel;  /* 背景扫描时，扫描完一个信道，判断是否需要切回工作信道工作 */
    osal_u8 scan_channel_interval;                      /* 间隔n个信道，切回工作信道工作一段时间 */
    osal_u16 work_time_on_home_channel;                 /* 背景扫描时，返回工作信道工作的时间 */

    mac_channel_stru channel_list[WLAN_MAX_CHANNEL_NUM];

    osal_u8 channel_nums; /* 信道列表中信道的个数 */
    osal_u8 probe_delay;  /* 主动扫描发probe request帧之前的等待时间 */
    osal_u16 scan_time;   /* 扫描在某一信道停留此时间后，扫描结束, ms，必须是10的整数倍 */

    wlan_scan_mode_enum_uint8 scan_mode; /* 扫描模式:前景扫描 or 背景扫描 */
    osal_u8 scan_flag;  /* 内核下发的扫描模式,每个bit意义见wlan_scan_flag_enum，暂时只解析是否为并发扫描 */
    osal_u8 scan_func;  /* DMAC SCANNER 扫描模式 */
    osal_u8 vap_id;     /* 下发扫描请求的vap id */
    osal_u64 cookie;    /* P2P 监听下发的cookie 值 */

    /* 重要:回调函数指针:函数指针必须放最后否则核间通信出问题 */
    mac_scan_cb_fn fn_cb;
} mac_scan_req_stru;

typedef struct {
    frw_timeout_stru    rx_dscr_opt_timer;     /* rx_dscr调整定时器 */
    osal_u32            rx_pkt_num;
    osal_u32            rx_pkt_opt_limit;
    osal_u32            rx_pkt_reset_limit;
    oal_bool_enum_uint8 dscr_opt_state;        /* TRUE表示已调整 */
    oal_bool_enum_uint8 dscr_opt_enable;
} hmac_rx_dscr_opt_stru;

/* m2s device信息结构体 */
typedef struct {
    wlan_nss_enum_uint8                 nss_num;           /* device的接收空间流个数 */
    wlan_mib_mimo_power_save_enum_uint8 smps_mode;         /* mac device的smps能力，用于切换后vap能力初始化 */
    osal_u8                             reserved[2];
} mac_device_m2s_stru;

typedef struct {
    mac_m2s_mode_enum_uint8 cfg_m2s_mode; /* 0:参数查询模式;
                                             1:参数配置模式;2.切换模式;3.软切换测试模式;4.硬切换测试模式 5.RSSI配置命令
                                              */
    union {
        struct {
            oal_bool_enum_uint8 m2s_type;                   /* 切换类型 */
            osal_u8 master_id;                              /* 主辅路id */
            mac_m2s_command_state_enum_uint8 m2s_state;     /* 期望切换到状态 */
            wlan_m2s_trigger_mode_enum_uint8 trigger_mode;  /* 切换触发业务模式 */
        } test_mode;

        struct {
            oal_bool_enum_uint8 mss_on;
        } mss_mode;

        struct {
            osal_u8 opt;
            osal_char value;
        } rssi_mode;
    } pri_data;
} mac_m2s_mgr_stru;

typedef struct {
    osal_u8 user_idx;                     /* CSI白名单用户index 取值范围0~3，最多4个用户 */
    osal_u8 enable;                       /* CSI白名单用户开关，0去使能，忽略以下所有参数， 1使能，校验后续参数 */
    osal_u8 cfg_match_ta_ra_sel;          /* CSI白名单地址过滤类型 0 RA 1 TA */
    osal_u8 resv;                         /* 保留1位，字节对齐 */
    osal_u8 mac_addr[WLAN_MAC_ADDR_LEN];  /* 白名单MAC地址 */
    osal_u8 frame_type_filter_bitmap;     /* 帧类型过滤具体参数 取值范围0~7，bit0管理帧 bit1控制帧 bit2数据帧 */
    osal_u8 sub_type_filter_enable;       /* 帧子类型过滤开关，0关闭 1打开，只有在1时才会校验sub_type_filter_bitmap */
    osal_u8 sub_type_filter;              /* 帧子类型过滤具体参数 4位二进制数对应的十进制结果（如 1100即为12） */
    osal_u8 ppdu_filter_bitmap;           /* ppdu format过滤具体参数 取值范围0~63
                                               bit[0]:non-HT
                                               bit[1]:HE_(ER)SU
                                               bit[2]:HE_MU_MIMO
                                               bit[3]:HE_MU_OFDMA
                                               bit[4]:HT
                                               bit[5]:VHT */
    osal_u16 period;                      /* CSI上报时间间隔 单位ms */
} mac_csi_usr_config_stru;

typedef struct {
    osal_u8 csi_data_blk_num;
    osal_u16 csi_data_max_len;
    osal_u8 resv[1];
} mac_csi_buffer_stru;

typedef struct {
    osal_u8     category;
    osal_u8     action_code;
    osal_u8     oui[3];     /* 3字节组织码 */
    osal_u8     eid;
    osal_u8     lenth;
    osal_u8     location_type;
    osal_u8     mac_server[WLAN_MAC_ADDR_LEN];
    osal_u8     mac_client[WLAN_MAC_ADDR_LEN];
    osal_u8     payload[4];     /* 4字节上层数据 */
} mac_location_event_stru;

#ifdef _PRE_WLAN_FEATURE_PNO_SCAN

/* PNO扫描信息结构体 */
typedef struct {
    osal_u8 ssid[WLAN_SSID_MAX_LEN];
    oal_bool_enum_uint8 scan_ssid;
    osal_u8 auc_resv[2];  /* 预留2字节对齐 */
} pno_match_ssid_stru;

typedef struct {
    pno_match_ssid_stru match_ssid_set[MAX_PNO_SSID_COUNT];
    osal_s32 ssid_count;                            /* 下发的需要匹配的ssid集的个数 */
    osal_s32 rssi_thold;                            /* 可上报的rssi门限 */
    osal_u32 pno_scan_interval;                     /* pno扫描间隔 */
    osal_u8 sour_mac_addr[WLAN_MAC_ADDR_LEN];       /* probe req帧中携带的发送端地址 */
    osal_u8 pno_scan_repeat;                        /* pno扫描重复次数 */
    oal_bool_enum_uint8 is_random_mac_addr_scan;    /* 是否随机mac */

    mac_scan_cb_fn fn_cb; /* 函数指针必须放最后否则核间通信出问题 */
} mac_pno_scan_stru;

typedef struct {
    wlan_mib_desired_bsstype_enum_uint8 bss_type;   /* 要扫描的bss类型 */
    wlan_scan_type_enum_uint8 scan_type;            /* 主动/被动 */
    wlan_scan_mode_enum_uint8 scan_mode;            /* 扫描模式:前景扫描 or 背景扫描 */
    osal_u8 probe_delay;                            /* 主动扫描发probe request帧之前的等待时间 */
    osal_u8 scan_func;                              /* DMAC SCANNER 扫描模式 */
    osal_u8 max_scan_count_per_channel;             /* 每个信道的扫描次数 */
    osal_u8 max_send_probe_req_count_per_channel;   /* 每次信道发送扫描请求帧的个数，默认为1 */
    osal_u8 scan_flag;
    osal_u16 scan_time;                             /* 扫描在某一信道停留此时间后，扫描结束, ms，必须是10的整数倍 */
    osal_u16 work_time_on_home_channel;             /* 背景扫描时，返回工作信道工作的时间 */
} mac_pno_scan_ctrl;

typedef struct {
    mac_pno_scan_stru sched;
    mac_pno_scan_ctrl ctrl;
} mac_pno_scan_param;


/* PNO调度扫描管理结构体 */
typedef struct {
    mac_pno_scan_param pno_sched_scan_params;   /* pno调度扫描请求的参数 */
    osal_void *hmac_vap;                        /* 保存启动pno扫描的vap handle */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    osal_timer pno_sched_scan_timer;
    osal_u32 pno_sched_scan_timer_id;
#endif
    osal_u8 curr_pno_sched_scan_times;          /* 当前pno调度扫描次数 */
    oal_bool_enum_uint8 is_found_match_ssid;    /* 是否扫描到了匹配的ssid */
    osal_u8 auc_resv[2];     /* 预留2字节对齐 */
} mac_pno_sched_scan_mgmt_stru;
/* 特性结构体END，第二阶段需要挪走 */
#endif

typedef struct {
    /********************************** HMAC&DMAC公共成员START **********************************/
    osal_u8 vap_id[WLAN_SERVICE_VAP_MAX_NUM_PER_DEVICE]; /* device下的业务vap，此处只记录VAP ID */
    osal_u8 resv;

    osal_u8 cfg_vap_id   : 4; /* 配置vap ID */
    osal_u8 vap_num      : 4; /* 当前device下的业务VAP数量(AP+STA) */
    osal_u8 up_vap_num   : 4; /* 维护当前整个device up状态vap个数 */
    osal_u8 mac_vap_id   : 4; /* 多vap共存时，保存睡眠前的mac vap id */
    osal_u8 sta_num; /* 当前device下的STA数量 */

    oal_bool_enum_uint8 device_state            : 1; /* 标识是否已经被分配，(OAL_TRUE初始化完成，OAL_FALSE未初始化 ) */
    oal_bool_enum_uint8 is_random_mac_addr_scan : 1; /* 随机mac扫描开关,从hmac下发 */
    osal_u8 resv0                               : 6;

    /* device能力 */
    wlan_protocol_cap_enum_uint8 protocol_cap        : 4; /* 协议能力 */
    wlan_band_cap_enum_uint8 band_cap                : 4; /* 频段能力 */
    wlan_channel_band_enum_uint8 max_band            : 4; /* 已配置VAP的频段，其后的VAP配置值不能与此值矛盾，仅在非DBAC时使用 */
    wlan_channel_bandwidth_enum_uint8 max_bandwidth  : 4; /* VAP的最大带宽值，其后的VAP配置不能与此矛盾，仅在非DBAC时使用 */

    osal_u8 max_channel; /* 已配置VAP的信道号，其后的VAP配置值不能与此值矛盾，仅在非DBAC时使用 */

    osal_u8 hw_addr[WLAN_MAC_ADDR_LEN];         /* 从eeprom或flash获得的mac地址，ko加载时调用hal接口赋值 */
    osal_u8 mac_oui[WLAN_RANDOM_MAC_OUI_LEN];   /* 随机mac地址OUI,由Android下发 */

    mac_data_rate_stru mac_rates_11g[MAC_DATARATES_PHY_80211G_NUM]; /* 11g速率 */

    mac_scan_req_stru scan_params; /* 最新一次的扫描参数信息 */
    frw_timeout_stru obss_scan_timer; /* obss扫描定时器，循环定时器 */

    /* linux内核中的device物理信息 */
    oal_wiphy_stru *wiphy; /* 用于存放和VAP相关的wiphy设备信息，在AP/STA模式下均要使用；可以多个VAP对应一个wiphy */

    mac_bss_id_list_stru bss_id_list; /* 当前信道下的扫描结果 */

    mac_device_capability_stru device_cap; /* device的部分能力，包括定制化 */

    mac_ap_ch_info_stru ap_channel_list[MAC_MAX_SUPP_CHANNEL];
    /********************************** HMAC&DMAC公共成员END **********************************/

    /********************************** HMAC私有成员START **********************************/
    osal_u8 device_id; /* 芯片ID */
    osal_u8 chip_id; /* 设备ID */
    osal_u8 uc_in_suspend;
    osal_u8 wapi;
    hmac_scan_stru scan_mgmt; /* 扫描管理结构体 */
    hmac_rx_dscr_opt_stru rx_dscr_opt;

    mac_p2p_info_stru st_p2p_info; /* P2P 相关信息 */
#ifdef _PRE_WLAN_FEATURE_SMPS
    /* SMPS是MAC的能力，需要device上所有的VAP都支持SMPS才会开启MAC的SMPS能力 */
    wlan_mib_mimo_power_save_enum_uint8 mac_smps_mode; /* 记录当前MAC工作的SMPS能力(放在mac侧，hmac也会用于判断) */
#endif

#ifdef _PRE_WIFI_DMT
    osal_u8 desired_bss_num; /* 扫描到的期望的bss个数 */
    osal_u8 desired_bss_idx[WLAN_MAX_SCAN_BSS_NUM]; /* 期望加入的bss在bss list中的位置 */
#endif
    osal_wait netif_change_event;

#ifdef _PRE_WLAN_TCP_OPT
    oal_bool_enum_uint8 sys_tcp_rx_ack_opt_enable;
    oal_bool_enum_uint8 sys_tcp_tx_ack_opt_enable;
#endif
    oal_bool_enum_uint8 start_via_priv : 1;
    oal_bool_enum_uint8 en_dbac_enabled : 1;
    oal_bool_enum_uint8 en_dbac_running : 1; /* DBAC是否在运行 */
    osal_u8 resv3                      : 5;

#ifdef _PRE_WLAN_FEATURE_P2P
    osal_u8 set_mode_iftype;  /* 表示在GC连接过程中是否清空hmac_device */
    osal_u8 p2p_listen_channel;  /* 在hmac_device中保存P2P的监听信道值 */
    osal_u16 beacon_interval; /* device级别beacon interval,device下所有VAP约束为同一值 */
    osal_u32 p2p_intf_status;
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend early_suspend; /* early_suspend支持 */
#endif

    osal_void **hmac_device_feature_arr;
    /********************************** HMAC私有成员END **********************************/

    /********************************** DMAC私有成员START **********************************/
    oal_bool_enum_uint8 in_suspend            : 1;
    oal_bool_enum_uint8 txop_enable           : 1; /* 发送无聚合时采用TXOP模式 */
    /* 当前扫描状态，根据此状态处理obss扫描和host侧下发的扫描请求，以及扫描结果的上报处理 */
    mac_scan_state_enum_uint8 curr_scan_state : 4;
    osal_u8 resv4                             : 2;

    osal_u16 aus_ac_mpdu_num[WLAN_WME_AC_BUTT]; /* device下各个AC对应的mpdu_num数 */
    osal_u16 total_mpdu_num; /* device下所有TID中总共的mpdu_num数量 */
    osal_u16 psm_mpdu_num;   /* 节能队列mpdu_num总数 */

    osal_u32 scan_timestamp; /* 记录最新一次扫描开始的时间 */

    mac_fcs_cfg_stru fcs_cfg;
    mac_fcs_mgr_stru fcs_mgr;

    osal_u8 ra_lut_index_table[WLAN_ACTIVE_USER_IDX_BMAP_LEN]; /* 关联用户 软件维护LUT表位图 大小2 */

    frw_timeout_stru keepalive_timer; /* keepalive定时器 */
    mac_cfg_rssi_limit_stru rssi_limit;

    hal_to_dmac_device_stru *hal_device[WLAN_DEVICE_MAX_NUM_PER_CHIP]; /* 03动态dbdc 两个hal */

    oal_bool_enum_uint8 wmm                  : 1; /* wmm使能开关 */
    oal_bool_enum_uint8 en_2040bss_switch    : 1; /* 20/40 bss检测开关 */
    oal_bool_enum_uint8 arpoffload_switch    : 1; /* arp offload功能切换开关 */
    oal_bool_enum_uint8 dbac_enabled         : 1; /* DBAC是否已经使能 */
    oal_bool_enum_uint8 dbac_running         : 1; /* DBAC是否在运行 */
    oal_bool_enum_uint8 intol_bit_recd_40mhz : 1;
    oal_bool_enum_uint8 sniffer_switch       : 1; /* 抓包功能开关 */
    oal_bool_enum_uint8 en_wmm               : 1; /* wmm使能开关 根据对端wmm能力更新 */

    oal_bool_enum_uint8 arp_offload_switch;
#ifdef _PRE_WLAN_FEATURE_PNO_SCAN
    mac_pno_sched_scan_mgmt_stru *pno_sched_scan_mgmt; /* pno调度扫描管理结构体指针，内存动态申请，从而节省内存 */
#endif
    mac_channel_stru p2p_vap_channel; /* p2p listen时记录p2p的信道，用于p2p listen结束后恢复 */

    osal_u32 vap_pause_bitmap;

#if (_PRE_OS_VERSION_LITEOS != _PRE_OS_VERSION)
    osal_spinlock lock; /* tid enqueue/dequeue lock */
#endif
    /********************************** DMAC私有成员END **********************************/
} hmac_device_stru;

typedef struct {
    hmac_device_stru *hmac_device;
} mac_wiphy_priv_stru;

/*****************************************************************************
    函数声明
*****************************************************************************/
osal_u32 mac_device_init_etc(hmac_device_stru *hmac_device, osal_u32 chip_ver,
    osal_u8 chip_id, osal_u8 device_id);
osal_u32 mac_chip_init_etc(mac_chip_stru *chip, osal_u8 device_max);
osal_u32 mac_board_init_etc(void);

osal_u32 mac_device_exit_etc(hmac_device_stru *hmac_device);
osal_u32 mac_chip_exit_etc(const mac_board_stru *board, mac_chip_stru *chip);
osal_u32 mac_board_exit_etc(const mac_board_stru *board);
osal_void hmac_chip_inc_active_user(hmac_device_stru *hmac_device);
osal_void hmac_chip_dec_active_user(hmac_device_stru *hmac_device);

wlan_mib_vht_supp_width_enum_uint8 hmac_device_trans_bandwith_to_vht_capinfo(wlan_bw_cap_enum_uint8 max_op_bd);

void mac_blacklist_get_pointer(wlan_vap_mode_enum_uint8 vap_mode, osal_u8 dev_id, osal_u8 chip_id,
    osal_u8 vap_id, mac_blacklist_info_stru **blacklist_info);

osal_void *hmac_device_get_all_rates_etc(const hmac_device_stru *hmac_device);

osal_u32 hmac_device_check_5g_enable(osal_u8 device_id);

mac_device_capability_stru *mac_device_get_capability(osal_void);
osal_u8 *mac_get_device_radio_cap(osal_void);
mac_board_stru *mac_get_pst_mac_board(osal_void);
mac_device_voe_custom_stru *mac_get_pst_mac_voe_custom_param(osal_void);
osal_void mac_device_set_random_scan_oui(osal_u32 scan_oui);
osal_void mac_set_pst_mac_voe_custom_over_ds_param(osal_u8 over_ds_en);
osal_u8 mac_get_user_res_num(osal_void);
osal_u8 *mac_get_user_res_num_addr(osal_void);
wlan_bw_cap_enum_uint8 hmac_get_device_bw_mode(const hal_to_dmac_device_stru *hal_device);

/*****************************************************************************
    函数定义
*****************************************************************************/

/* 特性函数START，第二阶段需要挪走 */
/*****************************************************************************
 功能描述  : 判断对应device上dbac功能是否使能
*****************************************************************************/
static INLINE__ oal_bool_enum_uint8 mac_is_dbac_enabled(const hmac_device_stru *hmac_device)
{
    return hmac_device->en_dbac_enabled;
}

/*****************************************************************************
 功能描述  : 判断对应device上dbac功能是否在运行
*****************************************************************************/
static INLINE__  oal_bool_enum_uint8 mac_is_dbac_running(const hmac_device_stru *hmac_device)
{
    if (hmac_device->en_dbac_enabled == OAL_FALSE) {
        return OAL_FALSE;
    }

    return hmac_device->en_dbac_running;
}

#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
static INLINE__  oal_bool_enum_uint8 mac_get_2040bss_switch(hmac_device_stru *hmac_device)
{
    return hmac_device->en_2040bss_switch;
}
static INLINE__  void mac_set_2040bss_switch(hmac_device_stru *hmac_device, oal_bool_enum_uint8 en_switch)
{
    hmac_device->en_2040bss_switch = en_switch;
}
#endif

/* 特性函数END，第二阶段需要挪走 */

/*****************************************************************************
 功能描述  : 检查芯片是否需要使能5g功能
*****************************************************************************/
static INLINE__ oal_bool_enum_uint8 mac_device_check_5g_enable_per_chip(void)
{
    osal_u8            dev_idx    = 0;

    while (dev_idx < WLAN_SERVICE_DEVICE_MAX_NUM_PER_CHIP) {
        if (hmac_device_check_5g_enable(dev_idx) != 0) {
            return OAL_TRUE;
        }
        dev_idx++;
    }

    return OAL_FALSE;
}

/*****************************************************************************
 功能描述  : 是否为隐藏ssid
*****************************************************************************/
static INLINE__ oal_bool_enum_uint8  mac_is_hide_ssid(const osal_u8 *ssid_ie, osal_u8 ssid_len)
{
    return (oal_bool_enum_uint8)((ssid_ie == OAL_PTR_NULL) || (ssid_len == 0) || (ssid_ie[0] == '\0'));
}

/*****************************************************************************
 功能描述  : 是否开启自动信道选择
*****************************************************************************/
static INLINE__ oal_bool_enum_uint8  mac_device_is_auto_chan_sel_enabled(const hmac_device_stru *hmac_device)
{
    /* BSS启动时，如果用户没有设置信道，则默认为开启自动信道选择 */
    if (hmac_device->max_channel == 0) {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}

static INLINE__ oal_bool_enum_uint8 mac_device_is_scaning(const hmac_device_stru *hmac_device)
{
    return (hmac_device->curr_scan_state == MAC_SCAN_STATE_RUNNING) ? OSAL_TRUE : OSAL_FALSE;
}

static INLINE__ oal_bool_enum_uint8 mac_device_is_listening(const hmac_device_stru *hmac_device)
{
    return ((hmac_device->curr_scan_state == MAC_SCAN_STATE_RUNNING) &&
    ((hmac_device->scan_params.scan_func & MAC_SCAN_FUNC_P2P_LISTEN) != 0)) ? OSAL_TRUE : OSAL_FALSE;
}

/*****************************************************************************
 函 数 名  : mac_m2s_cali_smps_mode
 功能描述  : 根据空间流数获取smps模式
*****************************************************************************/
static INLINE__ wlan_mib_mimo_power_save_enum_uint8 mac_m2s_cali_smps_mode(wlan_nss_enum_uint8 nss)
{
    return (nss == WLAN_SINGLE_NSS) ? WLAN_MIB_MIMO_POWER_SAVE_STATIC : WLAN_MIB_MIMO_POWER_SAVE_MIMO;
}

#ifdef _PRE_WLAN_FEATURE_M2S
/*****************************************************************************
 函 数 名  : mac_m2s_cali_nss_from_smps_mode
 功能描述  : 根据smps模式获取空间流数
*****************************************************************************/
static INLINE__ wlan_nss_enum_uint8 mac_m2s_cali_nss_from_smps_mode(wlan_mib_mimo_power_save_enum_uint8 smps_mode)
{
    return (smps_mode == WLAN_MIB_MIMO_POWER_SAVE_STATIC) ? WLAN_SINGLE_NSS : WLAN_DOUBLE_NSS;
}
#endif

#ifdef __cplusplus
#if __cplusplus
        }
#endif
#endif

#endif
