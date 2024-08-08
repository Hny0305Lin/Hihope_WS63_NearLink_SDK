/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: header file of alg_autorate_rom.h
 */

#ifndef ALG_AUTORATE_ROM_H
#define ALG_AUTORATE_ROM_H

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "alg_main_rom.h"
#include "hal_device_rom.h"
#include "dmac_alg_if_part_rom.h"
#include "alg_autorate_common_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_AUTORATE

/*****************************************************************************
  1 宏定义
*****************************************************************************/
#define BIT_0  ((osal_u32)(1 << 0))
#define BIT_3  ((osal_u32)(1 << 3))
/* 比较宏 */
#define ar_min(_A, _B) (((_A) < (_B)) ? (_A) : (_B))

/* 比较宏 */
#define ar_max(_A, _B) (((_A) > (_B)) ? (_A) : (_B))
#define ar_sub(_A, _B) (((_A) > (_B)) ? ((_A) - (_B)) : (0))

#define rssi_mcs_tbl(_rate_index, _rssi) \
    {                                    \
        _rate_index, _rssi, { 0, 0 }       \
    }

#define rssi_mcs_group_tbl(_item, _cnt)  \
    {                                    \
        _item, _cnt, { 0, 0, 0 }           \
    }
#define alg_array_size(_ast_array) (sizeof(_ast_array) / sizeof((_ast_array)[0]))

#define RATE_TBL_11B_RATE_NUM  4 /* 11b速率数目 */
#define RATE_TBL_11AG_RATE_NUM 8 /* 11a/g速率数目 */
#define ALG_AUTORATE_MAX_AGGR_NUM (WLAN_AMPDU_TX_MAX_NUM) /* 默认的最大聚合长度 */
#define ALG_AUTORATE_CFG_RANK_SHIFT  22  /* 用alg_autorate_tx_rate_rank_stru的resv字段保存rank */
#define ALG_AUTORATE_CFG_RANK_BIT    0x3 /* 2bit */
#define ALG_AUTORATE_CFG_FRAME_TYPE_SHIFT  24  /* 用alg_autorate_tx_rate_rank_stru的resv字段保存frame type */
#define ALG_AUTORATE_CFG_FRAME_TYPE_BIT    0x3 /* 2bit */

#define ALG_RATE_TABLE_INVALID_RATE_INDEX 0xFF
#define ALG_AUTORATE_INVALID_RATE  0xff   /* 无效速率索引 */
#define ALG_AUTORATE_VHT_NSS_MCS_NONE 0xff              /* vht该空间流不支持任何速率 */
#define ALG_AUTORATE_HE_NSS_MCS_NONE 0xff
#define ALG_AUTORATE_INVALID_PKTNUM         0xFFFFFFFF /* 无效的探测包pktnum */
#define ALG_AUTORATE_PROBE_RATE_TX_COUNT    1     /* 探测速率发送次数 */
#define ALG_RETRY_NUM_DCM                   2     /* DCM可使用的rank个数 */
#define ALG_AUTORATE_PER_STAT_NORMAL_RSSI_THR (-60)             /* 更新PER时判定当前是否为强信号的门限 */
#define ALG_RSSI_INIT_MARKER      127    /* RSSI平滑值初始值,单位db,对应127dBm */

/* 用户gi ltf能力bitmap */
#define ALG_AUTORATE_HE_SU_PPDU_1XLTF_GI1_BIT_MASK    0x1   /* BIT0 */
#define ALG_HE_SU_PPDU_1XLTF_GI1_SHIFT       0
#define ALG_AUTORATE_HE_ER_SU_PPDU_1XLTF_GI1_BIT_MASK 0x2   /* BIT1 */
#define ALG_HE_ER_SU_PPDU_1XLTF_GI1_SHIFT    1
#define ALG_AUTORATE_HE_ER_SU_PPDU_4XLTF_GI1_BIT_MASK 0x4   /* BIT2 */
#define ALG_HE_ER_SU_PPDU_4XLTF_GI1_SHIFT    2
#define ALG_AUTORATE_HE_SU_MU_PPDU_4XLTF_GI1_BIT_MASK 0x8   /* BIT3 */
#define ALG_HE_SU_MU_PPDU_4XLTF_GI1_SHIFT    3
#define ALG_AUTORATE_NDP_4XLTF_GI4_BIT_MASK 0x10            /* BIT4 */
#define ALG_NDP_4XLTF_GI4_SHIFT              4
/* 速率统计个数 ar_max(ar_max(WLAN_11B_BUTT, WLAN_11AG_BUTT), ar_max(WLAN_HT_SINGLE_NSS_END, WLAN_HE_MCS_BUTT)) */
#define ALG_AUTORATE_USER_RATE_STAT_MAX_NUM           (WLAN_HE_MCS_BUTT)
#define ALG_AUTORATE_RATE_CAP_HT_BW_CNT               2     /* HT支持的带宽个数 */
#define ALG_AUTORATE_RATE_CAP_VHT_BW_CNT_2G           2     /* VHT 2G支持的带宽个数 */
#define ALG_AUTORATE_RATE_CAP_VHT_BW_CNT              4     /* VHT 5G支持的带宽个数 */
#define ALG_AUTORATE_BYTE_TO_BIT_SHIFT                3    /* Byte->bit移位值 */
#define ALG_AUTORATE_RATE_MBPS_TO_KBPS                1000 /* 速率扩大倍数(Mbps->kbps) */
/* 各协议模式初始速率建议值 */
#define ALG_RATE_STAT_INIT_START_INDEX_11B            WLAN_11B_1_M_BPS
#define ALG_RATE_STAT_INIT_START_INDEX_11AG           WLAN_11AG_6M_BPS
#define ALG_RATE_STAT_INIT_START_INDEX_11N            WLAN_HT_MCS1
#define ALG_RATE_STAT_INIT_START_INDEX_11AC           WLAN_VHT_MCS1
#define ALG_RATE_STAT_INIT_START_INDEX_11AX           WLAN_HE_MCS1
#define ALG_AUTORATE_HT_MAX_BITMAP_LEN                (WLAN_HT_MAX_BITMAP_LEN)    /* ht速率的bitmap最大位数 */
#define ALG_AUTORATE_HT_MAX_BITMAP_SHIFT              3 /* ht速率的bitmap最大位数移位值 */

/* rx 描述符bw枚举转算法bw枚举 */
#define rx_desc_freq_bw_2_alg_ar_bw_cap(_rx_desc_freq_bw) \
    (((_rx_desc_freq_bw) == WLAN_BAND_ASSEMBLE_20M) ? ALG_AR_BW_CAP_20M :  \
     ((_rx_desc_freq_bw) == WLAN_BAND_ASSEMBLE_SU_ER_106_TONE) ? ALG_AR_BW_HE_RU_106_TONE :  \
     ((_rx_desc_freq_bw) == WLAN_BAND_ASSEMBLE_40M) ? ALG_AR_BW_CAP_40M :  \
     ((_rx_desc_freq_bw) == WLAN_BAND_ASSEMBLE_40M_DUP) ? ALG_AR_BW_CAP_20M :  ALG_AR_BW_CAP_BUTT)

#define alg_autorate_get_vht_max_support_mcs(_nss_mcs_bit) \
    ((3 == (_nss_mcs_bit)) ? ALG_AUTORATE_VHT_NSS_MCS_NONE : \
     (2 == (_nss_mcs_bit)) ? WLAN_VHT_MCS9 : \
     (1 == (_nss_mcs_bit)) ? WLAN_VHT_MCS8 : WLAN_VHT_MCS7)

#define alg_autorate_get_he_max_support_mcs(_nss_mcs_bit) \
    ((3 == (_nss_mcs_bit)) ? ALG_AUTORATE_HE_NSS_MCS_NONE : \
     (2 == (_nss_mcs_bit)) ? WLAN_HE_MCS9 + 2 : \
     (1 == (_nss_mcs_bit)) ? WLAN_HE_MCS9 : WLAN_HE_MCS7)

/* 计算goodput需要的参数 */
#define ALG_AGGR_11N_AGGR_BYTES_LIMIT       65535   /* 11n的最大聚合字节数限制 */
#define ALG_AGGR_11AC_AGGR_BYTES_LIMIT      1048575 /* 11ac的最大聚合字节数限制 */
#define ALG_AGGR_11AX_AGGR_BYTES_LIMIT      6500631 /* 11ax的最大聚合字节数限制 */
#define ALG_TYPICAL_FRAME_LEN               1544    /* 典型包长 */
#define ALG_AUTORATE_RATE_MBPS_TO_KBPS      1000    /* 速率扩大倍数(Mbps->kbps) */
#define ALG_AUTORATE_TIME_KBPS_TO_BPS       1000    /* 速率扩大倍数(kbps->bps) */
#define ALG_AUTORATE_TIME_MS_TO_US          1000    /* 时间扩大倍数(us->ns) */
#define ALG_AUTORATE_TIME_NS_TO_US          1000    /* 时间缩小倍数(NS->us) */
#define ALG_AUTORATE_DIFS_TIME_US           28      /* DIFS时间开销(单位:us) */
#define ALG_AUTORATE_SIFS_TIME_US           10      /* SIFS时间开销(单位:us) */
#define ALG_AUTORATE_BA_TIME_US             32      /* BA时间开销(单位:us) */
#define ALG_AUTORATE_11AG_ACK_TIME_US       40      /* 11a/g ACK时间开销(单位:us) */
#define ALG_AUTORATE_11B_ACK_TIME_US        208     /* 11b ACK时间开销(单位:us) */
#define ALG_AUTORATE_11N_PHY_HEAD_TIME_US   32      /* 11n的phy头所占时间(单位:us)--HT-LTF与NSS相关,另外计算 */
#define ALG_AUTORATE_11AC_PHY_HEAD_TIME_US  36      /* 11ac的phy头所占时间(单位:us)--VHT-LTF与NSS相关,另外计算 */
#define ALG_AUTORATE_11AG_PHY_HEAD_TIME_US  20      /* 11a/g的phy头所占时间(单位:us) */
#define ALG_AUTORATE_11B_PHY_HEAD_TIME_US   96      /* 11b的PHY头所占时间(单位:us) */
#define ALG_AUTORATE_HE_LTF_4X_TIME_NS      1280    /* HE-LTF时间开销(单位:ns) */
#define ALG_AUTORATE_HT_VHT_LTF_TIME_US     4       /* HT-LTF/VHT-LTF时间开销(单位:us) */
#define ALG_AUTORATE_SUBFRAME_DELIMITER     4       /* 子帧delimiter字节数 */
#define ALG_AUTORATE_11AX_HE_SU_PPDU_PHY_HEAD_TIME_US 36 /* 11ax的HESUPPDU PHY头所占时间(us) HE-LTF与NSS相关另外计算 */
/* 描述符相关 */
#define ALG_RATE_VALUE_MCS_SHIFT           4
#define alg_ar_build_nss_mcs(_nss, _mcs)   ((osal_u8)(((_nss) << ALG_RATE_VALUE_MCS_SHIFT) | (_mcs)))
#define alg_ar_other_alg_probe_ppdu(_type)  \
    ((_type) != DMAC_USER_ALG_NON_PROBE && (_type) != DMAC_USER_ALG_AUOTRATE_PROBE)
#define alg_ar_probe_ppdu(_type)           (DMAC_USER_ALG_AUOTRATE_PROBE == (_type))
#define alg_ar_lock_user(_lock)            ((_lock) == ALG_LOCK_AUTORATE_PROBE)
#define alg_ar_other_alg_lock_user(_lock)  (((_lock) != ALG_LOCK_NON_PROBE) && ((_lock) != ALG_LOCK_AUTORATE_PROBE))

/* 发送异常，数据帧没有真正发送到空口 */
#define alg_autorate_tx_status_is_abandon_tx(_status, _data_send_cnt) \
    ((HAL_TX_INVALID == (_status)) || (HAL_TX_KEY_SEARCH_FAIL == (_status)) || (HAL_TX_AMPDU_MISMATCH == (_status)) || \
     (HAL_TX_FAIL_ABORT == (_status)) || (HAL_TX_TIMEOUT == (_status)) || ((_data_send_cnt) == 0))

#define alg_rate_stat_smooth_short_per(_avg_per, _instant_per, _scale)                                     \
    ((((osal_u16)((_avg_per) * ((1ul << (ALG_AUTORATE_SHORT_STAT_SHIFT + (osal_u8)(_scale))) - 1))) >> \
         (ALG_AUTORATE_SHORT_STAT_SHIFT + (osal_u8)(_scale))) + \
         ((_instant_per) >> (ALG_AUTORATE_SHORT_STAT_SHIFT + (osal_u8)(_scale))))
#define alg_ar_is_probe_rate_good(_rate1, _rate2) (((_rate1)->short_per <= ALG_AR_RATE_BAD_THRD) || \
                ((_rate2)->short_per <= ALG_AR_RATE_BAD_THRD))

#define is_rate_pro_bw_nss_diff(_rate_record_node, _probe_node) \
        (((_rate_record_node)->en_protocol != (_probe_node)->en_protocol) || \
         ((_rate_record_node)->en_bw != (_probe_node)->en_bw) || \
         ((_rate_record_node)->en_nss != (_probe_node)->en_nss) || \
         ((_rate_record_node)->en_gi_type != (_probe_node)->en_gi_type))

/* 不同bw, rssi差值 */
#define alg_ar_cali_rssi_based_bw(_bw, _ori_rssi)  \
    (((_bw) == ALG_AR_BW_CAP_20M) ? (_ori_rssi) : \
    ((_bw) == ALG_AR_BW_CAP_40M) ? ((_ori_rssi) - 3) : \
    ((_bw) == ALG_AR_BW_CAP_80M) ? ((_ori_rssi) - 6) : ((_ori_rssi) - 9))

#define alg_ar_judge_is_same_stat_info(_pbest_rate, _best_rate) \
    ((((_pbest_rate)->en_protocol) == ((_best_rate)->en_protocol)) &&  \
     (((_pbest_rate)->en_bw) == ((_best_rate)->en_bw)) &&  \
     (((_pbest_rate)->en_nss) == ((_best_rate)->en_nss)) &&  \
     (((_pbest_rate)->en_gi_type) == ((_best_rate)->en_gi_type)) &&   \
     (((_pbest_rate)->en_dcm_used) == ((_best_rate)->en_dcm_used)))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* AUTORATE 专用带宽能力枚举 */
typedef enum {
    ALG_AR_BW_HE_RU_26_TONE,
    ALG_AR_BW_HE_RU_52_TONE,
    ALG_AR_BW_HE_RU_106_TONE,
    ALG_AR_BW_CAP_20M,
    ALG_AR_BW_CAP_40M,
    ALG_AR_BW_CAP_2G_BUTT,
    ALG_AR_BW_CAP_80M = ALG_AR_BW_CAP_2G_BUTT,
    ALG_AR_BW_CAP_160M,

    ALG_AR_BW_CAP_BUTT
} alg_ar_bw_cap_enum;
typedef osal_u8 alg_ar_bw_cap_enum_uint8;
/* 用于标识各协议模式rate table包含的带宽，rate table新增带宽则需要修改以下枚举 */
typedef enum {
    ALG_AR_RATE_TABLE_HT_20M,
    ALG_AR_RATE_TABLE_HT_40M,
    ALG_AR_RATE_TABLE_HT_BUTT,
} alg_rate_table_ht_bw_type_enum;
typedef osal_u8 alg_rate_table_ht_bw_type_enum_uint8;
typedef enum {
    ALG_AR_RATE_TABLE_VHT_20M,
    ALG_AR_RATE_TABLE_VHT_40M,
    ALG_AR_RATE_TABLE_VHT_BUTT,
} alg_rate_table_vht_bw_type_enum;
typedef osal_u8 alg_rate_table_vht_bw_type_enum_uint8;
typedef enum {
    ALG_AR_RATE_TABLE_HE_RU_26_TONE,
    ALG_AR_RATE_TABLE_HE_RU_52_TONE,
    ALG_AR_RATE_TABLE_HE_RU_106_TONE,
    ALG_AR_RATE_TABLE_HE_20M,
    ALG_AR_RATE_TABLE_HE_BUTT,
} alg_rate_table_he_bw_type_enum;
typedef osal_u8 alg_rate_table_he_bw_type_enum_uint8;

/* he用户速率能力索引 */
typedef enum {
    ALG_AUTORATE_HE_RATE_SET_CAP_LE_80M_INDEX = 0, /* 带宽<=80M */
    ALG_AUTORATE_HE_RATE_SET_CAP_160M_INDEX = 1,   /* 带宽为160M */

    ALG_AUTORATE_HE_RATE_SET_CAP_INDEX_BUTT
} alg_autorate_he_rate_set_cap_index_enum;
typedef osal_u8 alg_autorate_he_rate_set_cap_index_enum_uint8;

/* AR探测模式索引 */
typedef enum {
    ALG_AUTORATE_PROBE_MCS,
    ALG_AUTORATE_PROBE_BW,
    ALG_AUTORATE_PROBE_PROTOCOL,

    ALG_AUTORATE_PROBE_PHASE_BUTT
} alg_autorate_probe_phase_enum;
typedef osal_u8 alg_autorate_probe_phase_enum_uint8;

/* 探测方向 */
typedef enum {
    ALG_AUTORATE_PROBE_UP = 0,      /* mcs up/ascend bw/ascend protocol */
    ALG_AUTORATE_PROBE_DOWN = 1,    /* mcs down/descend bw/descend protocol */
    ALG_AUTORATE_PROBE_DIRECTION_BUTT
} alg_autorate_probe_direction_enum;
typedef osal_u8 alg_autorate_probe_direction_enum_uint8;

typedef enum {
    ALG_AUTORATE_DESCEND_PROTOCOL,
    ALG_AUTORATE_ASCEND_PROTOCOL,

    ALG_AUTORATE_CROSS_PROTOCOL_PROBE_DIRECTION_BUTT
} alg_autorate_cross_pro_probe_direction_enum;
typedef osal_u8 alg_autorate_cross_pro_probe_direction_enum_uint8;

typedef enum {
    ALG_AR_CFG_FIX_RATE_UCAST_DATA,
    ALG_AR_CFG_FIX_RATE_BCAST_DATA,
    ALG_AR_CFG_FIX_RATE_UCAST_MGMT,
    ALG_AR_CFG_FIX_RATE_BCAST_MGMT,

    ALG_AR_CFG_FIX_RATE_BUTT
} alg_ar_cfg_fix_rate_frame_type_enum;
typedef osal_u8 alg_ar_cfg_fix_rate_frame_type_enum_uint8;
/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/
typedef struct {
    osal_u32 rate_kbps;         /* WLAN_LONG_GI */
    osal_u32 sgi_rate_kbps;     /* WLAN_SHORT_GI */
    osal_u32 aggr_bytes_limit_4ms;
} alg_rate_table_stru;

/* 每个rank的速率信息，注意每元素要定义为4字节，只用于临时计算 */
typedef struct {
    osal_u32   protocol_mode;    /* wlan_phy_protocol_enum */
    osal_u32   freq_bw;          /* hal_channel_assemble_enum */
    osal_u32   preamble;         /* wlan_phy_preamble_type */
    osal_u32   fec_coding;       /* wlan_channel_code_enum */
    osal_u32   gi_type;          /* wlan_phy_he_gi_type_enum_uint8 */
    osal_u32   ltf_type;         /* wlan_phy_he_ltf_type_enum */
    osal_u32   tx_count;         /* 发送次数 */
    osal_u32   ppdu_dcm;         /* wlan_phy_dcm_status */
    osal_u32   nss_rate;         /* wlan_phy_rate_enum */
} alg_autorate_tx_rate_rank_stru;

#ifdef _PRE_WLAN_FEATURE_INTRF_MODE
/* 干扰场景优化配置信息结构体 */
typedef struct {
    osal_u8 fix_11b_switch;          /* 固定11b模式下的发送速率 */
    osal_u8 fix_11n_switch;          /* 设置11n模式下的速率档位 */
    osal_u8 no_11b_switch;           /* 设置Autorate算法为不降协议至11b */
    osal_u8 resv;
} alg_autorate_intrf_mode_cfg_stru;
#endif

/* 速率自适应算法 VAP级别 结构体 */
typedef struct {
    dmac_vap_stru *dmac_vap;
    alg_autorate_tx_fix_rate_rank_stru mcast_data_tx_param[HAL_TX_RATE_MAX_NUM];
    alg_autorate_tx_fix_rate_rank_stru ucast_mgmt_tx_param[HAL_TX_RATE_MAX_NUM];
    alg_autorate_tx_fix_rate_rank_stru mcast_mgmt_tx_param[HAL_TX_RATE_MAX_NUM];

#ifdef _PRE_WLAN_FEATURE_INTRF_MODE
    alg_autorate_intrf_mode_cfg_stru intrf_mode_cfg; /* 干扰场景优化配置信息 */
#endif

    osal_u8   en_registered;
    osal_u8   resv[3];
    osal_u8   *rom;
} alg_autorate_vap_stru;

typedef struct {
    osal_u32   ht_rate_set_bitmap;   /* 11n 20/40M 下各速率的 bitmap,bit0~bit15: MCS0~MCS15 */
    oal_bool_enum_uint8 en_supp_short_gi[ALG_AR_BW_CAP_2G_BUTT]; /* 11n 20/40M 下是否支持 short_gi */
    osal_u8    rev;
} alg_autorate_ht_rate_set_stru;

typedef struct {
    osal_u16   vht_rate_set_bitmap[WLAN_NSS_MAX_NUM_LIMIT];    /* 11ac 各nss 速率的bitmap: MCS0~MCS11 */
    oal_bool_enum_uint8 supp_short_gi[ALG_AR_BW_CAP_BUTT]; /* 11ac 20/40/80/160 下是否支持 short_gi */
    osal_u8    rev;
} alg_autorate_vht_rate_set_stru;

typedef struct {
    /* 11ax le80M/160M & 各nss 速率的bitmap: MCS0~MCS11 */
    osal_u16   he_rate_set_bitmap[ALG_AUTORATE_HE_RATE_SET_CAP_INDEX_BUTT][WLAN_NSS_MAX_NUM_LIMIT];

   /* ---------- optional HE-LTF + GIx capability ----------
    *
    * BIT0: HE_SU_PPDU    1 x HE-LTF + 0.8us
    * BIT1: HE_ER_SU_PPDU 1 x HE-LTF + 0.8us
    * BIT2: HE_ER_SU_PPDU 4 x HE-LTF + 0.8us
    * BIT3: HE_SU/MU_PPDU 4 x HE-LTF + 0.8us
    * BIT4: NDP           4 x HE-LTF + 3.2us
    * BIT5: HE_SU/MU_PPDU 4 x HE-LTF + 0.4us
*/
    osal_u8   he_ltf_gi_cap_bitmap;
    osal_u8   rev[3];
} alg_autorate_he_rate_set_stru;

/* 最底层 basic Rate Info结构 */
typedef struct {
    wlan_protocol_enum_uint8 en_protocol;   /* !!! wlan_protocol_enum_uint8, tx param trans to wlan_phy_protocol_enum */
    alg_ar_bw_cap_enum_uint8 en_bw;      /* !!! alg_ar_bw_cap_enum_uint8, tx param trans to hal_channel_assemble_enum */
    wlan_nss_enum_uint8 en_nss;
    wlan_gi_type_enum_uint8 en_gi_type;   /* !!! wlan_gi_type_enum, tx param [he]  trans to wlan_phy_he_gi_type_enum */

    oal_bool_enum_uint8 en_dcm_used;
    osal_u8    rate_index;           /* !!! mcs: wlan_11b_rate_index_enum_uint8/wlan_legacy_ofdm_mcs_enum_uint8/
                                            wlan_ht_mcs_enum_uint8/wlan_vht_mcs_enum/wlan_he_mcs_enum,
                                            tx param need trans to wlan_phy_rate_enum */
    osal_u16   short_per;
    osal_u16   long_per;        /* best rate用,探测node不用 */
    osal_u16   rev;
    osal_u32   theory_goodput;                     /* 对应带宽下该速率的理论goodput */
} alg_autorate_basic_rate_info_stru;

typedef struct {
    osal_u8   probe_intvl_pktcnt;        /* 探测间隔包计数 */
    osal_u8   probe_pktcnt_thrd;         /* 探测间隔包门限 */
    osal_u8   probe_cur_intvl_keep_cnt;  /* 探测当前包间隔门限保持计数 */
    osal_u8   probe_intvl_keep_cnt_thrd; /* 探测当前包间隔门限保持计数的门限 */
} alg_autorate_probe_time_trigger_info_stru;

typedef struct {
    osal_u8 need_continue_probe;    /* 当前探测速率是否需要再次探测 */
    osal_u8 cur_probe_succ;    /* 当前探测报文是否发送成功，作为继续探测的前提 */
    osal_u8 rssi_sudden_flag;    /* 用户RSSI突变标志 */
    osal_u8 is_probe_up_reserve_rate_better; /* 约束探测间隔不要增大,探测间隔计数不累加 */
    osal_u8 resv_rate_better_probe;  /* 探测过程中最优速率per变化明显，说明环境变化明显, 触发向上探测 */
    osal_u8 is_probe_other_gi;       /* 标记当前是否在探测other gi */
    osal_u16 probe_mpdu_cnt;         /* 探测mpdu个数 */
    osal_u16 rate_record_org_short_per; /* 探测前记录的short per */
    osal_u16 rev;

    alg_autorate_basic_rate_info_stru last_stage_best_probe_rate_info;
    alg_autorate_probe_time_trigger_info_stru up_time_trig_info;
    alg_autorate_probe_time_trigger_info_stru down_time_trig_info;
} alg_autorate_probe_mcs_stru;
typedef struct {
    osal_u8   ascend_bw_rate_better_keep_cnt;  /* 待升带宽速率比当前带宽最优速率gdpt高的持续计数,超过门限才能切换 */
    osal_u8   descend_bw_rate_better_keep_cnt; /* 降带宽速率比最优速率gp高的计数,超过门限才能切换 */
    osal_u8   en_enable_event_trig;        /* 当前是否使能 event trig 降带宽探测 */
    osal_u8   consecutive_event_trig_cnt;  /* 记录event trig 连续降带宽探测未探测到最优速率的次数,超过上限去使能 */

    alg_autorate_probe_time_trigger_info_stru ascend_bw_time_trig_info;  /* 升带宽计数触发，降带宽事件触发 */
    alg_autorate_probe_time_trigger_info_stru descend_bw_time_trig_info; /* 降带宽计数触发，降带宽事件触发 */
} alg_autorate_probe_bw_stru;

typedef struct {
    /* probe mgmt info */
    osal_u8 is_probing;    /* 用户是否正在进行ar探测 */
    osal_u8 is_event_trig;    /* 是否事件触发本次探测 */
    alg_autorate_probe_phase_enum_uint8 cur_probe_phase; /* 探测模式 mcs/bw/protocol */
    alg_autorate_probe_direction_enum_uint8 cur_probe_direction;
    osal_u8   probe_pktno_wait_cnt;         /* 发送方式乒乓操作, 下次tx_complete获得的pktno不一定
                                               为本次tx_process设置的probe_pktno, 设置wait_cnt避免卡死 */
    osal_u8    rev[3];
    osal_u32   probe_pktno;                 /* used to identify probe pkt */
    osal_u32   last_probe_node_time_ms;     /* 探测阶段内，上一次报文发送完成的时间戳 ms,距离当前时间太久则结束探测 */

    alg_autorate_probe_mcs_stru             probe_mcs_info;
    alg_autorate_probe_bw_stru              probe_bw_info;
    alg_autorate_basic_rate_info_stru       best_probe_rate_info;
    alg_autorate_basic_rate_info_stru       next_probe_rate_info;
    osal_u8 *rom;
} alg_autorate_probe_info_stru;

/* 速率自适应算法 USER级别 结构体 */
typedef struct {
    /***** 基本能力信息 *****/
    osal_u8 user_valid;                      /* 用户初始化成功标志: 初始化成功为True */
    osal_u8 enable_ampdu;                    /* 是否支持AMPDU */
    osal_u8 supp_ldpc;                       /* 是否支持LDPC */
    osal_u8 is_initial_stage;                /* 起步阶段识别状态量 */
    osal_u8 initial_stage_pkt_cnt;           /* 起步阶段历经包个数 */
    alg_ar_bw_cap_enum_uint8  avail_bandwidth;           /* 用户支持带宽上限 */
    alg_ar_bw_cap_enum_uint8  cur_bandwidth;             /* 用户当前工作带宽 */
    wlan_protocol_enum_uint8  avail_protocol;            /* 用户支持协议上限 */
    wlan_protocol_enum_uint8  cur_protocol;              /* 用户当前使用的协议 */
    osal_u8   cur_max_nss;                      /* wlan_nss_enum_uint8 用户当前使用的最大空间流数目 */

    /***** 碰撞检测相关 *****/
    osal_u8   consecutive_all_fail_cnt;         /* 连续所有rank发送失败的PPDU个数 */
    osal_u8   collision_tolerant_ppdu_cnt;      /* 疑似碰撞的PPDU个数 */
    osal_s16  ack_rssi;       /* 16 bit: ACK_RSSI平滑结果 */
    /***** user支持的速率集 *****/
    osal_u8   user_11b_rate_set_bitmap;    /* bit0~bit3: 1M, 2M, 5.5M, 11M */
    osal_u8   legacy_rate_set_bitmap; /* bit0~bit7: wlan_11ag_rate_value_enum */
    union {
        alg_autorate_he_rate_set_stru  he_rate_set;
        alg_autorate_vht_rate_set_stru vht_rate_set;
        alg_autorate_ht_rate_set_stru  ht_rate_set;
    } un_rate_set;

    /***** 速率统计信息 *****/
    alg_autorate_basic_rate_info_stru best_rate_info;
    alg_autorate_rate_stat_info_stru  rate_stat_info[ALG_AUTORATE_USER_RATE_STAT_MAX_NUM]; /* 用户速率统计信息 */

    /***** 自动速率探测相关信息 *****/
    alg_autorate_probe_info_stru      user_probe_info;

    dmac_user_stru *user;             /* 用户结构体指针 */
    dmac_vap_stru  *dmac_vap;
    alg_autorate_dev_stru  *autorate_dev;
    osal_u8 *rom;
} alg_autorate_user_stru;

/* goodput计算的内部参数 */
typedef struct {
    osal_u16 mpdu_bytes;
    osal_u16 aggr_subfrm;
    osal_u32 rate_kbps;
} alg_autorate_calc_goodput_param_stru;

/*****************************************************************************
  6 函数声明
*****************************************************************************/
typedef osal_u32 (*alg_autorate_init_cb)(osal_void);
typedef osal_u32 (*alg_autorate_dev_init_cb)(alg_autorate_dev_stru *autorate_dev);
typedef osal_u32 (*alg_autorate_add_vap_cb)(alg_autorate_vap_stru *autorate_vap);
typedef osal_u32 (*alg_autorate_start_vap_cb)(alg_autorate_vap_stru *autorate_vap);
typedef osal_u32 (*alg_autorate_del_vap_cb)(alg_autorate_vap_stru *autorate_vap);
typedef osal_u32 (*alg_autorate_init_user_cb)(alg_autorate_user_stru *user_info);
typedef osal_u32 (*alg_autorate_exit_user_cb)(alg_autorate_user_stru *user_info);
typedef osal_u32 (*alg_autorate_tx_process_cb)(dmac_user_stru *user, mac_tx_ctl_stru *tx_ctl,
    hal_tx_txop_alg_stru *txop_param, osal_u32 *ret);
typedef osal_u32 (*alg_autorate_tx_process_set_fix_rate_cb)(dmac_vap_stru *dmac_vap, dmac_user_stru *user,
    mac_tx_ctl_stru *tx_ctl, hal_tx_txop_alg_stru *txop_param, osal_u32 *ret);
typedef osal_u32 (*alg_autorate_rx_process_cb)(dmac_vap_stru *vap, dmac_user_stru *user, oal_dmac_netbuf_stru *buf,
    hal_rx_statistic_stru *rx_stats, osal_u32 *ret);
typedef osal_u32 (*alg_ar_intf_change_proc_cb)(hal_to_dmac_device_stru *hal_device,
    alg_intf_det_notify_info_stru *intf_det_notify);
typedef osal_u32 (*alg_ar_tx_comp_process_abnormal_cb)(dmac_user_stru *user,
    hal_tx_dscr_ctrl_one_param *tx_dscr, mac_tx_ctl_stru *cb, alg_autorate_user_stru *user_info);
typedef osal_u32 (*alg_ar_tx_comp_process_cb)(dmac_user_stru *user,
    hal_tx_dscr_ctrl_one_param *tx_dscr, mac_tx_ctl_stru *cb, alg_autorate_user_stru *user_info, osal_u32 *ret);
typedef osal_u32 (*alg_ar_update_per_cb)(alg_autorate_dev_stru *autorate_dev,
    dmac_user_stru *user, hal_tx_dscr_ctrl_one_param *tx_dscr, alg_autorate_user_stru *user_info);
typedef osal_u32 (*alg_ar_event_trig_probe_cb)(alg_autorate_user_stru *user_info,
    alg_autorate_probe_phase_enum_uint8 *probe_phase, alg_autorate_probe_direction_enum_uint8 *probe_direction);
typedef osal_u32 (*alg_ar_probe_mcs_prepare_cb)(alg_autorate_user_stru *user_info,
    oal_bool_enum_uint8 *continue_cur_stage_probe, alg_autorate_probe_direction_enum_uint8 probe_direction,
    osal_u32 *ret);
typedef osal_u32 (*alg_ar_probe_init_cb)(alg_autorate_user_stru *user_info, hal_tx_dscr_ctrl_one_param *tx_dscr,
    alg_autorate_probe_phase_enum_uint8 probe_phase, alg_autorate_probe_direction_enum_uint8 probe_direction,
    osal_u32 *ret);
typedef osal_u32 (*alg_ar_probe_finish_cb)(alg_autorate_user_stru *user_info, oal_bool_enum_uint8 *update_bestrate,
    osal_u32 *ret);
typedef osal_u32 (*alg_ar_probe_finish_update_info_cb)(alg_autorate_user_stru *user_info, osal_u8 update_bestrate);
typedef osal_u32 (*alg_ar_probe_result_update_cb)(alg_autorate_user_stru *user_info,
    hal_tx_dscr_ctrl_one_param *tx_dscr, osal_u32 *ret);
typedef osal_u32 (*alg_ar_probe_result_analysis_cb)(alg_autorate_user_stru *user_info,
    hal_tx_dscr_ctrl_one_param *tx_dscr, oal_bool_enum_uint8 *continue_probe);
hal_channel_assemble_enum_uint8 alg_ar_bw_cap_2_hal_channel_assemble_bw(alg_ar_bw_cap_enum_uint8 bw_cap);
alg_ar_bw_cap_enum_uint8 hal_channel_assemble_bw_2_alg_ar_bw_cap(hal_channel_assemble_enum_uint8 assemble_bw);
alg_ar_bw_cap_enum_uint8 wlan_he_ru_bw_2_alg_ar_bw_cap(wlan_he_ru_enum_uint8 he_ru_bw);
alg_rate_table_he_bw_type_enum_uint8 alg_he_ar_bw_2_ar_he_rate_table_bw(alg_ar_bw_cap_enum_uint8 en_bw);
alg_ar_bw_cap_enum_uint8 wlan_bw_cap_2_alg_ar_bw_cap(wlan_bw_cap_enum_uint8 wlan_bw_cap);
wlan_bw_cap_enum_uint8 alg_ar_bw_cap_2_wlan_bw_cap(alg_ar_bw_cap_enum_uint8 ar_bw_cap);
wlan_protocol_enum_uint8 wlan_protocol_2_alg_ar_protocol(wlan_protocol_enum_uint8 wlan_protocol);
wlan_phy_he_gi_type_enum_uint8 wlan_gi_type_2_phy_gi_type(
    wlan_protocol_enum_uint8 wlan_protocol, wlan_gi_type_enum_uint8 wlan_gi);
wlan_gi_type_enum_uint8 phy_gi_type_2_wlan_gi_type(
    wlan_phy_protocol_enum_uint8 phy_protocol, wlan_phy_he_gi_type_enum_uint8 phy_gi);
osal_u16 alg_rate_stat_smooth_long_per(osal_u16 avg_per, osal_u16 instant_per, osal_u8 scale);
osal_u16 alg_autorate_fast_smooth_per(osal_u16 avg_per, osal_u16 instant_per, osal_u8 shift);
oal_bool_enum alg_ar_compare_bigger(osal_u32 _new, osal_u32 basic, osal_u16 prop);
oal_bool_enum alg_ar_probe_better(
    alg_autorate_basic_rate_info_stru *best_probe_rate_info,
    alg_autorate_basic_rate_info_stru *best_rate_info,
    osal_u16 gdpt_better_thrd);
oal_bool_enum alg_ar_is_probe_both_rate_bad(
    alg_autorate_basic_rate_info_stru *rate1, alg_autorate_basic_rate_info_stru *rate2);
oal_bool_enum alg_ar_judge_best_and_probe_rate_diff(
    alg_autorate_basic_rate_info_stru *pbest_rate, alg_autorate_basic_rate_info_stru *pprobe_rate);
osal_u16 alg_ar_tx_complete_calc_instant_per(hal_tx_dscr_ctrl_one_param *tx_dscr);
osal_u32 alg_ar_probe_mcs_get_next_probe_node(alg_autorate_user_stru *user_info,
    oal_bool_enum_uint8 *tmp_continue_cur_stage_probe, alg_autorate_probe_direction_enum_uint8 probe_direction);
void alg_ar_probe_mcs_result_update(alg_autorate_user_stru *user_info, osal_u16 instant_per,
    hal_tx_dscr_ctrl_one_param *tx_dscr);
void alg_ar_probe_result_update_stat_info(alg_autorate_user_stru *user_info,
    hal_tx_dscr_ctrl_one_param *tx_dscr);
osal_u32 alg_ar_probe_check_probe_aging(alg_autorate_user_stru *user_info);
void alg_ar_update_best_rate(alg_autorate_basic_rate_info_stru *best_node,
    alg_autorate_basic_rate_info_stru *cur_node);
void alg_ar_probe_mcs_update_best_probe(alg_autorate_user_stru *user_info);
osal_u32 alg_ar_prepare_probe_info_choose_next_probe_rate_idx(alg_autorate_user_stru *user_info,
    osal_u32 probe_gdpt, alg_autorate_basic_rate_info_stru *next_probe_node);
osal_u32 alg_ar_probe_try_find_next_bigger_valid_rate_idx(alg_autorate_user_stru *user_info,
    alg_autorate_basic_rate_info_stru *next_probe_node);
oal_bool_enum_uint8 alg_ar_probe_result_analysis_update_best_probe(alg_autorate_user_stru *user_info,
    alg_autorate_basic_rate_info_stru *next_probe_node, alg_autorate_basic_rate_info_stru *best_probe_rate);

#endif /* #ifdef _PRE_WLAN_FEATURE_AUTORATE */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of alg_autorate_rom.h */
