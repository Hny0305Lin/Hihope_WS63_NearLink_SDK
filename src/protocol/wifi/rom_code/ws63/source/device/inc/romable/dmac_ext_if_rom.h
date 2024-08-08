/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Header file of the DMAC external public interface.
 */

#ifndef DMAC_EXT_IF_ROM_H
#define DMAC_EXT_IF_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "wlan_types_rom.h"
#include "dmac_ext_if_type_rom.h"
#include "hal_ext_if_rom.h"
#include "wlan_mib_rom.h"
#include "oal_ext_if_rom.h"
#include "mac_frame_rom.h"
#include "dmac_ext_if_device_rom.h"
#include "wlan_resource_common_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_DMAC_EXT_IF_ROM_H

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MAC_INVALID_RX_BA_LUT_INDEX HAL_MAX_RX_BA_LUT_SIZE

/* 安全加密 :  bss_info 中记录AP 能力标识， WPA or WPA2 */
#define DMAC_WPA_802_11I BIT0
#define DMAC_RSNA_802_11I BIT1

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#define CUSTOM_MSG_DATA_HDR_LEN (sizeof(custom_cfgid_enum_uint32) + sizeof(osal_u32)) /* 抛往dmac侧消息头的长度 */
#endif // #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE

/* DMAC CB中用于不同算法对帧进行标记 */
#define DMAC_SCAN_MAX_AP_NUM_TO_GNSS 32
#define GNSS_DMAC_SCAN_RESULTS_VALID_MS 5000
#define DMAC_TX_ONE_MPDU_AMSDU_NUM_MAX 4
#define DMAC_DEF_AP_TBTT_OFFSET             10000   /* AP的tbtt提前量(单位us) */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
/* frame control字段结构体 */
typedef struct {
    osal_s16 signal;            /* 驱动接收包记录的RSSI值 */
    osal_u8  resv[2];
} dmac_vap_query_stats_stru;

/* Update join req 需要配置的速率集参数 */
typedef struct {
    union {
        osal_u8 value;
        struct {
            osal_u8 support_11b : 1;  /* 该AP是否支持11b */
            osal_u8 support_11ag : 1; /* 该AP是否支持11ag */
            osal_u8 ht_capable : 1;   /* 是否支持ht */
            osal_u8 vht_capable : 1;  /* 是否支持vht */
            osal_u8 reserved : 4;
        } capable; /* 与dmac层wlan_phy_protocol_enum对应 */
    } capable_flag;
    osal_u8 min_rate[2]; /* 第1个存储11b协议对应的速率，第2个存储11ag协议对应的速率 */
    osal_u8 reserved1;
} dmac_set_rate_stru;

typedef struct {
    mac_twt_update_source_enum_uint8 update_source;
    osal_u16 user_idx;
    osal_u8 resv;
    mac_cfg_twt_stru twt_cfg;
} dmac_ctx_update_twt_stru;

#define MAC_VAP_INVAILD (0x0) /* 0为vap无效 */
#define MAC_VAP_VAILD (0x2b)

#ifdef _PRE_WLAN_FEATURE_BTCOEX
typedef struct {
    /* 字段参考hal_one_packet_cfg_stru 部分字段需要在device侧自行获取 */
    hal_fcs_protect_type_enum_uint8   protect_type;
    hal_fcs_protect_cnt_enum_uint8    protect_cnt;
    osal_u16                          resv : 2;     /* 对应原protect_coex_pri移到device */
    osal_u16                          cfg_one_pkt_tx_vap_index : 4;
    osal_u16                          cfg_one_pkt_tx_peer_index : 5;
    osal_u16                          bit_rsv : 5;
    osal_u32                          tx_mode;
    osal_u32                          tx_data_rate;

    osal_u8                           is_valid;
    osal_u8                           rsv[3];      /* 字节对齐 */
} dmac_vap_btcoex_info_stru;
#endif

/* dmac vap */
typedef struct dmac_vap_tag {
    /* -->> Public Start */
    osal_u8 bssid[WLAN_MAC_ADDR_LEN]; /* BSSID，非MAC地址，MAC地址是mib中的auc_dot11StationID  */

    osal_u8 init_flag; /* vap为静态资源，标记VAP有没有被申请。DMAC OFFLOAD模式VAP被删除后过滤缓冲的帧 */
    osal_u8 vap_id; /* 即资源池索引值 */

    wlan_dmac_mib_stru *mib_info; /* mib信息(当时配置vap时，可以直接将指针值为NULL，节省空间) */
    mac_scanned_all_bss_info all_bss_info;
    mac_channel_stru channel; /* vap所在的信道 */

    osal_u16 user_nums; /* VAP下已挂接的用户个数 */
    osal_u8 protocol             : 5;
    osal_u8 voice_aggr           : 1; /* 该vap是否针对VO业务支持聚合 */
    osal_u8 use_rts_threshold    : 1; /* 该vap的RTS策略是否使用协议规定的RTS门限 */
    osal_u8 hal_cap_11ax         : 1;
    osal_u8 tim_bitmap_len;
    osal_u8 *tim_bitmap;

    /* --------- Public STA成员 Start ----------------- */
    osal_u32 tb_aggr_bytes;
    osal_u16 sta_aid; /* VAP为STA模式时保存AP分配给STA的AID(从响应帧获取), 取值范围1~2007;VAP为AP模式时,不用此成员变量 */
    osal_u16 assoc_vap_id; /* VAP为STA模式时保存user(ap)的资源池索引；VAP为AP模式时，不用此成员变量 */
#ifdef _PRE_WLAN_FEATURE_MAC_PARSE_TIM
    osal_u16 tim_pos;
#endif
    /* --------- Public STA成员 End ------------------- */

    /* --------- Public AP成员 Start ------------------ */
    osal_u16 multi_user_idx; /* 组播用户ID */
    struct osal_list_head mac_user_list_head; /* 关联用户节点双向链表,使用USER结构内的DLIST */
     /* --------- Public AP成员 End ------------------ */

    /* -->> Public End */

    /* -->> Private Start */
    /* --------- Private STA成员 Start ------------------ */
    mac_vap_state_enum_uint8 vap_state; /* VAP状态 */
    osal_u8 beacon_timeout_times : 7; /* sta等待beacon超时计数 */
    osal_u8 auth_received        : 1; /* 接收了auth */
    osal_u8 assoc_rsp_received   : 1; /* 接收了assoc */
    osal_u8 keepalive_en         : 1; /* vap KeepAlive功能开关: 0-关闭, 1-开启 */
    osal_u8 is_erp               : 1; /* 从hmac同步当前是否处在erp保护模式中 */
    osal_u8 is_sta_non_erp_num   : 1; /* 从hmac同步是否sta_non_erp_num，1表示大于0 */
    osal_u8 resv1                : 4;
    osal_u8 linkloss_rate; /* linkloss比例信息，同步至device侧供算法使用 */
    osal_u64 sta_tbtt_tsf_time;
    /* --------- Private STA成员 End ------------------ */

    /* -->> Private End */

    /* -->> Features Start */
    osal_u8 sta_pm_close_status;  /* sta 低功耗状态, 包含多个模块的低功耗控制信息 */
    osal_u8 bcn_tout_max_cnt; /* beacon连续收不到最大睡眠次数 */
    osal_u16 er_su_disable            : 1;
    osal_u16 dcm_constellation_tx     : 2;
    osal_u16 bandwidth_extended_range : 1;
    osal_u16 resv4                    : 12;
    osal_u16 beacon_timeout; /* beacon timeout配置值 */
    osal_u8 psm_dtim_period; /* 实际采用的dtim period */
    osal_u16 psm_listen_interval; /* 实际采用的listen_interval */
    osal_u8 uapsd_cap   : 1; /* 保存与STA关联的AP是否支持uapsd能力信息 */
    osal_u8 al_tx_flag  : 1; /* 常发标志 */
    osal_u8 mu_spacing_factor_flag : 1;
    osal_u8 cfg_pm_mode : 3; /* 手动挡保存的低功耗模式 */
    osal_u8 erp_mode    : 1; /* 是否处于erp保护模式 */
    osal_u8 nodata_retransmit_flag : 1; /* null_data报文重传标志位 */
    osal_u8 resv_feature0;
    mac_cfg_uapsd_sta_stru sta_uapsd_cfg; /* UAPSD的配置信息 */
    osal_s16 signal;            /* 驱动接收包记录的RSSI值 */
    osal_u16 in_tbtt_offset; /* 内部tbtt offset配置值 */
    mac_cfg_twt_stru twt_cfg;
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    dmac_vap_btcoex_info_stru vap_btcoex;
#endif
    /* -->> Features End */

    mac_sta_pm_handler_stru sta_pm_handler; /* sta侧pm管理结构定义 */

    mac_cfg_p2p_noa_param_stru p2p_noa_param;
#ifdef _PRE_WLAN_FEATURE_CHBA
    osal_u8 chba_mode;
    osal_u8 resv_chba[3];
#endif
    /* -->> HAL Start */
    hal_tx_txop_alg_stru txop; /* 单播数据帧发送参数 */
    hal_to_dmac_vap_stru *hal_vap; /* hal vap结构 */
    hal_to_dmac_device_stru *hal_device; /* hal device结构体以免二次获取 */
    /* -->> HAL End */

    /* -->> Forward Start */
    hal_tx_dscr_queue_header_stru tx_dscr_queue_fake[HAL_TX_QUEUE_BUTT];
    hal_tx_dscr_queue_header_stru tx_dscr_queue[HAL_TX_QUEUE_BUTT]; /* 发送描述符链表, 不区分用户，只分AC队列 */
    /* -->> Forward End */

    uintptr_t *_rom; /* ROM化后资源扩展指针 */
} dmac_vap_stru;

/* DMAC与HMAC模块共用的WLAN DRX事件结构体 */
typedef struct {
    oal_dmac_netbuf_stru *netbuf; /* netbuf链表一个元素 */
    osal_u16 netbuf_num;        /* netbuf链表的个数 */
    osal_u8 resv[2];           /* 2字节对齐 */
} dmac_wlan_drx_event_stru;

/* DMAC与HMAC模块共用的WLAN CRX事件结构体 */
typedef struct {
    oal_dmac_netbuf_stru *netbuf; /* 指向管理帧对应的netbuf */
    //    osal_u8              *chtxt;          /* Shared Key认证用的challenge text */
} dmac_wlan_crx_event_stru;

/*
    (1)DMAC与HMAC模块共用的CTX子类型ACTION对应的事件的结构体
    (2)当DMAC自身产生DELBA帧时，使用该结构体向HMAC模块抛事件
*/

/* WUR更新事件参数 */
#ifdef _PRE_WLAN_FEATURE_WUR_TX
typedef struct {
    mac_wur_update_source_enum_uint8 update_source;
    osal_u8 resv;
    osal_u16 user_idx;
    mac_wur_param_htd_stru wur_param_htd;
    mac_user_wur_cap_ie_stru wur_cap_ie;
} dmac_ctx_update_wur_stru;
#endif

/* HMAC到DMAC配置同步操作 */
typedef struct {
    wlan_dmac_cfgid_enum_uint16 cfgid;
    osal_u8 resv[2];          /* 预留2字节对齐 */
    osal_u32 (*set_func)(dmac_vap_stru *dmac_vap, osal_u8 len, const osal_u8 *param);
} dmac_config_syn_stru;

typedef struct {
    oal_bool_enum_uint8 in_use;          /* 缓存BUF是否被使用 */
    osal_u8 num_buf;                    /* MPDU对应的描述符的个数 */
    osal_u16 seq_num;                   /* MPDU对应的序列号 */
    oal_dmac_netbuf_head_stru netbuf_head; /* MPDU对应的描述符首地址 */
    osal_u32 rx_time;                   /* 报文被缓存的时间戳 */
} dmac_rx_buf_stru;

/* 处理MPDU的MSDU的处理状态的结构体的定义 */
typedef struct {
    oal_dmac_netbuf_stru *curr_netbuf; /* 当前处理的netbuf指针 */
    osal_u8 *curr_netbuf_data;      /* 当前处理的netbuf的data指针 */
    osal_u16 submsdu_offset;         /* 当前处理的submsdu的偏移量,   */
    osal_u8 msdu_nums_in_netbuf;     /* 当前netbuf包含的总的msdu数目 */
    osal_u8 procd_msdu_in_netbuf;    /* 当前netbuf中已处理的msdu数目 */
    osal_u8 netbuf_nums_in_mpdu;     /* 当前MPDU的中的总的netbuf的数目 */
    osal_u8 procd_netbuf_nums;       /* 当前MPDU中已处理的netbuf的数目 */
    osal_u8 procd_msdu_nums_in_mpdu; /* 当前MPDU中已处理的MSDU数目 */

    osal_u8 flag;
} dmac_msdu_proc_state_stru;

/* 每一个MSDU包含的内容的结构体的定义 */
typedef struct {
    osal_u8 sa[WLAN_MAC_ADDR_LEN]; /* MSDU发送的源地址 */
    osal_u8 da[WLAN_MAC_ADDR_LEN]; /* MSDU接收的目的地址 */
    osal_u8 ta[WLAN_MAC_ADDR_LEN]; /* MSDU接收的发送地址 */
    osal_u8 priority;
    osal_u8 resv[1];

    oal_dmac_netbuf_stru *netbuf; /* MSDU对应的netbuf指针(可以使clone的netbuf) */
} dmac_msdu_stru;

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
#define TIME_ONE_SEC_IN_US 1000000

/*****************************************************************************
 函 数 名  : dmac_get_cb_seq_num
 功能描述  : 根据tx control block获取对应的序列号
*****************************************************************************/
static INLINE__ osal_u16 dmac_get_cb_seq_num(const mac_tx_ctl_stru *tx_ctl)
{
    return ((mac_ieee80211_frame_stru *)((osal_u8 *)tx_ctl + OAL_MAX_CB_LEN))->seq_num;
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_wlan_frame_type
 功能描述  : 根据tx control block获取帧类型
*****************************************************************************/
static INLINE__ osal_u16 dmac_get_cb_wlan_frame_type(const mac_tx_ctl_stru *tx_ctl)
{
    return ((mac_ieee80211_frame_stru *)((osal_u8 *)tx_ctl + OAL_MAX_CB_LEN))->frame_control.type;
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_wlan_frame_subtype
 功能描述  : 根据tx control block获取帧子类型
*****************************************************************************/
static INLINE__ osal_u16 dmac_get_cb_wlan_frame_subtype(const mac_tx_ctl_stru *tx_ctl)
{
    return ((mac_ieee80211_frame_stru *)((osal_u8 *)tx_ctl + OAL_MAX_CB_LEN))->frame_control.sub_type;
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_frame_header_addr
 功能描述  : 根据tx control block获取对应的帧头地址
*****************************************************************************/
static INLINE__ mac_ieee80211_frame_stru *dmac_get_cb_frame_header_addr(const mac_tx_ctl_stru *tx_ctl)
{
    return (mac_ieee80211_frame_stru *)((osal_u8 *)tx_ctl + OAL_MAX_CB_LEN);
}

/*****************************************************************************
 函 数 名  : dmac_get_rx_cb_mac_hdr
 功能描述  : 获取mac头的值

*****************************************************************************/
static INLINE__ osal_u32 *dmac_get_rx_cb_mac_hdr(mac_rx_ctl_stru *cb_ctrl)
{
    return (osal_u32 *)((osal_u8 *)cb_ctrl + OAL_MAX_CB_LEN);
}
/*****************************************************************************
 函 数 名  : dmac_ba_seq_add
 功能描述  : 获取两数之和跟 DMAC_BA_SEQNO_MASK 的逻辑与的结果
*****************************************************************************/
static INLINE__ osal_u16 dmac_ba_seq_add(osal_u16 seq1, osal_u16 seq2)
{
    return ((seq1 + seq2) & DMAC_BA_SEQNO_MASK);
}

/*****************************************************************************
 函 数 名  : dmac_ba_seq_sub
 功能描述  : 获取两数之差跟 DMAC_BA_SEQNO_MASK 的逻辑与的结果
*****************************************************************************/
static INLINE__ osal_u16 dmac_ba_seq_sub(osal_u16 seq1, osal_u16 seq2)
{
    return ((seq1 - seq2) & DMAC_BA_SEQNO_MASK);
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_is_vipframe
 功能描述  : 判断一个帧是否为数据帧中的VIP帧
*****************************************************************************/
static INLINE__ osal_u8 dmac_get_cb_is_vipframe(const mac_tx_ctl_stru *tx_ctrl)
{
    if ((tx_ctrl->frame_type == WLAN_CB_FRAME_TYPE_DATA) && (tx_ctrl->frame_subtype <= MAC_DATA_VIP_FRAME)) {
        return OSAL_TRUE;
    }
    return OSAL_FALSE;
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_is_smps_frame
 功能描述  : 判断一个帧是否为Action帧中的smps帧
*****************************************************************************/
static INLINE__ osal_u8 dmac_get_cb_is_smps_frame(const mac_tx_ctl_stru *tx_ctrl)
{
    if ((tx_ctrl->frame_type == WLAN_CB_FRAME_TYPE_ACTION) &&
        (tx_ctrl->frame_subtype == WLAN_ACTION_SMPS_FRAME_SUBTYPE)) {
        return OSAL_TRUE;
    }
    return OSAL_FALSE;
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_is_opmode_frame
 功能描述  : 判断一个帧是否为Action帧中的opmode帧
*****************************************************************************/
static INLINE__ osal_u8 dmac_get_cb_is_opmode_frame(const mac_tx_ctl_stru *tx_ctrl)
{
    if ((tx_ctrl->frame_type == WLAN_CB_FRAME_TYPE_ACTION) &&
        (tx_ctrl->frame_subtype == WLAN_ACTION_OPMODE_FRAME_SUBTYPE)) {
        return OSAL_TRUE;
    }
    return OSAL_FALSE;
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_is_p2pgo_frame
 功能描述  : 判断一个帧是否为管理帧中的p2pgo帧
*****************************************************************************/
static INLINE__ osal_u8 dmac_get_cb_is_p2pgo_frame(const mac_tx_ctl_stru *tx_ctrl)
{
    if ((tx_ctrl->frame_type == WLAN_CB_FRAME_TYPE_MGMT) &&
        (tx_ctrl->frame_subtype == WLAN_ACTION_P2PGO_FRAME_SUBTYPE)) {
        return OSAL_TRUE;
    }
    return OSAL_FALSE;
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_is_data_frame
 功能描述  : 判断一个帧是否为数据帧中的data帧或qos data帧
*****************************************************************************/
static INLINE__ osal_u8 dmac_get_cb_is_data_frame(const mac_tx_ctl_stru *tx_ctrl)
{
    if ((dmac_get_cb_wlan_frame_type(tx_ctrl) == WLAN_DATA_BASICTYPE) &&
        (dmac_get_cb_wlan_frame_subtype(tx_ctrl) == WLAN_DATA ||
        dmac_get_cb_wlan_frame_subtype(tx_ctrl) == WLAN_QOS_DATA)) {
        return OSAL_TRUE;
    }
    return OSAL_FALSE;
}

/*****************************************************************************
 函 数 名  : mac_get_cb_is_udata_frame
 功能描述  : 判断一个帧是否为数据帧中单播的data帧或qos data帧
*****************************************************************************/
static INLINE__ osal_u8 mac_get_cb_is_udata_frame(const mac_tx_ctl_stru *tx_ctrl)
{
    if ((dmac_get_cb_is_data_frame(tx_ctrl) == OSAL_TRUE) && (dmac_get_cb_is_vipframe(tx_ctrl) == OSAL_FALSE) &&
        (tx_ctrl->ismcast == OSAL_FALSE)) {
        return OSAL_TRUE;
    }
    return OSAL_FALSE;
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_is_qos_data
 功能描述  : 判断一个帧是否为数据帧中的qos data帧或qos null data帧
*****************************************************************************/
static INLINE__ osal_u8 dmac_get_cb_is_qos_data(const mac_tx_ctl_stru *tx_ctrl)
{
    if ((dmac_get_cb_wlan_frame_type(tx_ctrl) == WLAN_DATA_BASICTYPE) &&
        (dmac_get_cb_wlan_frame_subtype(tx_ctrl) == WLAN_QOS_DATA ||
        dmac_get_cb_wlan_frame_subtype(tx_ctrl) == WLAN_QOS_NULL_FRAME)) {
        return OSAL_TRUE;
    }
    return OSAL_FALSE;
}

/*****************************************************************************
 函 数 名  : dmac_get_cb_is_bar
 功能描述  : 判断一个帧是否为控制帧中的block ack req帧
*****************************************************************************/
static INLINE__ osal_u8 dmac_get_cb_is_bar(const mac_tx_ctl_stru *tx_ctrl)
{
    if (dmac_get_cb_wlan_frame_type(tx_ctrl) == WLAN_CONTROL &&
        dmac_get_cb_wlan_frame_subtype(tx_ctrl) == WLAN_BLOCKACK_REQ) {
        return OSAL_TRUE;
    }
    return OSAL_FALSE;
}

/*****************************************************************************
 函 数 名  : dmac_mib_set_dot11dtimperiod
 功能描述  : 设置MIB项 dot11DTIMPeriod 的值

*****************************************************************************/
static INLINE__ osal_void dmac_mib_set_dot11dtimperiod(dmac_vap_stru *dmac_vap, osal_u32 val)
{
    if (val != 0) {
        dmac_vap->mib_info->dot11_dtim_period = val;
    }
}

/*****************************************************************************
 函 数 名  : mac_mib_get_dot11dtimperiod
 功能描述  : 获取MIB项 dot11DTIMPeriod 的值

*****************************************************************************/
static INLINE__ osal_u32 dmac_mib_get_dot11_dtim_period(const dmac_vap_stru *dmac_vap)
{
    return dmac_vap->mib_info->dot11_dtim_period;
}

/*****************************************************************************
 函 数 名  : dmac_mib_set_powermanagementmode
 功能描述  : 设置MIB项 dot11DTIMPeriod 的值

*****************************************************************************/
static INLINE__ osal_void dmac_mib_set_powermanagementmode(dmac_vap_stru *dmac_vap, osal_u8 val)
{
    if (val != 0) {
        dmac_vap->mib_info->dot11_power_management_mode = val;
    }
}

/*****************************************************************************
 函 数 名  : dmac_mib_get_powermanagementmode
 功能描述  : 获取MIB项 dot11DTIMPeriod 的值

*****************************************************************************/
static INLINE__ osal_u32 dmac_mib_get_powermanagementmode(const dmac_vap_stru *dmac_vap)
{
    return dmac_vap->mib_info->dot11_power_management_mode;
}

static INLINE__ osal_void dmac_mib_set_beacon_period(dmac_vap_stru *dmac_vap, osal_u32 val)
{
    if (val != 0) {
        dmac_vap->mib_info->dot11_beacon_period = val;
    }
}

static INLINE__ osal_u32 dmac_mib_get_beacon_period(const dmac_vap_stru *dmac_vap)
{
    return dmac_vap->mib_info->dot11_beacon_period;
}

/*****************************************************************************
 功能描述  : 获取MIB项 dot11MgmtOptionMultiBSSIDImplemented 的值
*****************************************************************************/
static INLINE__ osal_u8 dmac_mib_get_multi_bssid_implement(dmac_vap_stru *dmac_vap)
{
    return dmac_vap->mib_info->dot11_mgmt_option_multi_bssid_implemented;
}

/*****************************************************************************
 功能描述  : 设置MIB项 dot11MgmtOptionMultiBSSIDImplemented 的值
*****************************************************************************/
static INLINE__ osal_void dmac_mib_set_multi_bssid_implement(dmac_vap_stru *dmac_vap, osal_u8 val)
{
    dmac_vap->mib_info->dot11_mgmt_option_multi_bssid_implemented = val;
}

/*****************************************************************************
 功能描述  : 获取MIB项 dot11_heldpc_coding_in_payload 的值
*****************************************************************************/
static INLINE__ osal_u8 dmac_mib_get_heldpc_coding_in_payload(const dmac_vap_stru *dmac_vap)
{
    return dmac_vap->mib_info->dot11_heldpc_coding_in_payload;
}

/*****************************************************************************
 功能描述  : 设置MIB项 dot11_heldpc_coding_in_payload 的值
*****************************************************************************/
static INLINE__ osal_void dmac_mib_set_heldpc_coding_in_payload(dmac_vap_stru *dmac_vap, osal_u8 val)
{
    dmac_vap->mib_info->dot11_heldpc_coding_in_payload = val;
}

/*****************************************************************************
 功能描述  : 获取MIB项 dot11_ldpc_coding_option_implemented 的值
*****************************************************************************/
static INLINE__ osal_u8 dmac_mib_get_ldpc_coding_option_implemented(const dmac_vap_stru *dmac_vap)
{
    return dmac_vap->mib_info->dot11_ldpc_coding_option_implemented;
}

/*****************************************************************************
 功能描述  : 设置MIB项 dot11_ldpc_coding_option_implemented 的值
*****************************************************************************/
static INLINE__ osal_void dmac_mib_set_ldpc_coding_option_implemented(dmac_vap_stru *dmac_vap, osal_u8 val)
{
    dmac_vap->mib_info->dot11_ldpc_coding_option_implemented = val;
}

/*****************************************************************************
 功能描述  : 获取MIB项 dot11_ldpc_coding_option_activated 的值
*****************************************************************************/
static INLINE__ osal_u8 dmac_mib_get_ldpc_coding_option_activated(const dmac_vap_stru *dmac_vap)
{
    return dmac_vap->mib_info->dot11_ldpc_coding_option_activated;
}

/*****************************************************************************
 功能描述  : 设置MIB项 dot11_ldpc_coding_option_activated 的值
*****************************************************************************/
static INLINE__ osal_void dmac_mib_set_ldpc_coding_option_activated(dmac_vap_stru *dmac_vap, osal_u8 val)
{
    dmac_vap->mib_info->dot11_ldpc_coding_option_activated = val;
}

/*****************************************************************************
 功能描述  : 获取MIB项 dot11_vhtldpc_coding_option_implemented 的值
*****************************************************************************/
static INLINE__ osal_u8 dmac_mib_get_vhtldpc_coding_option_implemented(const dmac_vap_stru *dmac_vap)
{
    return dmac_vap->mib_info->dot11_vhtldpc_coding_option_implemented;
}

/*****************************************************************************
 功能描述  : 设置MIB项 dot11_vhtldpc_coding_option_implemented 的值
*****************************************************************************/
static INLINE__ osal_void dmac_mib_set_vhtldpc_coding_option_implemented(dmac_vap_stru *dmac_vap, osal_u8 val)
{
    dmac_vap->mib_info->dot11_vhtldpc_coding_option_implemented = val;
}

/*****************************************************************************
 函 数 名  : dmac_mib_get_station_id
 功能描述  : 获取MIB项 dot11StationID 的值

*****************************************************************************/
static INLINE__ osal_u8 *dmac_mib_get_station_id(const dmac_vap_stru *dmac_vap)
{
    return dmac_vap->mib_info->dot11_station_id;
}

static INLINE__ osal_u8 dmac_is_p2p_go(const dmac_vap_stru *dmac_vap)
{
    return (dmac_vap->hal_vap->p2p_mode == WLAN_P2P_GO_MODE);
}

static INLINE__ osal_u8 dmac_is_p2p_noa_enabled(const dmac_vap_stru *dmac_vap)
{
    return (dmac_vap->p2p_noa_param.count != 0);
}

static INLINE__ osal_u8 dmac_is_p2p_cl(const dmac_vap_stru *dmac_vap)
{
    return (dmac_vap->hal_vap->p2p_mode == WLAN_P2P_CL_MODE);
}

static INLINE__ osal_u8 dmac_is_p2p_dev(const dmac_vap_stru *dmac_vap)
{
    return (dmac_vap->hal_vap->p2p_mode == WLAN_P2P_DEV_MODE);
}

static INLINE__ osal_u8 get_p2p_mode(const dmac_vap_stru *dmac_vap)
{
    return (dmac_vap->hal_vap->p2p_mode);
}

static INLINE__ osal_u8 dmac_is_legacy_vap(const dmac_vap_stru *dmac_vap)
{
    return (dmac_vap->hal_vap->p2p_mode == WLAN_LEGACY_VAP_MODE);
}

static INLINE__ osal_u8 dmac_is_legacy_sta(const dmac_vap_stru *dmac_vap)
{
    return ((dmac_vap->hal_vap->vap_mode == WLAN_VAP_MODE_BSS_STA) &&
        (dmac_vap->hal_vap->p2p_mode == WLAN_LEGACY_VAP_MODE));
}

static INLINE__ osal_u8 dmac_is_legacy_ap(const dmac_vap_stru *dmac_vap)
{
    return ((dmac_vap->hal_vap->vap_mode == WLAN_VAP_MODE_BSS_AP) &&
        (dmac_vap->hal_vap->p2p_mode == WLAN_LEGACY_VAP_MODE));
}

static INLINE__ osal_u8 dmac_is_up_vap(const dmac_vap_stru *dmac_vap)
{
    return ((dmac_vap->vap_state == MAC_VAP_STATE_UP) || (dmac_vap->vap_state == MAC_VAP_STATE_PAUSE) ||
        (dmac_vap->vap_state == MAC_VAP_STATE_ROAMING));
}

osal_u32 *dmac_get_rx_cb_mac_hdr(mac_rx_ctl_stru *cb_ctrl);
osal_u32 mac_vap_set_cb_tx_user_idx(dmac_vap_stru *dmac_vap, mac_tx_ctl_stru *tx_ctl, const osal_u8 *data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of dmac_ext_if_rom.h */
