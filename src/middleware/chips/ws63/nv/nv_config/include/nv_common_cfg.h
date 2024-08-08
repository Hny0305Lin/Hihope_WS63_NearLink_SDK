/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NV common header file.
 */

#ifndef __NV_COMMON_CFG_H__
#define __NV_COMMON_CFG_H__

#include "stdint.h"
/* 修改此文件后需要先编译A核任意版本生成中间文件application.etypes后才能在编译nv.bin时生效 */
#define WLAN_MAC_ADDR_LEN 6
#define WLAN_COUNTRY_CODE_LEN 2
#define WLAN_XO_TRIM_TEMP_LEN 8
#define WLAN_RF_FE_RX_LOSS_NUM 3
#define WLAN_RF_FE_MAX_POWER_NUM 1
#define WLAN_RF_FE_TARGET_POWER_NUM 33
#define WLAN_RF_FE_LIMIT_POWER_NUM 56
#define WLAN_RF_FE_SAR_POWER_NUM 3
#define WLAN_RF_FE_RSSI_COMP_NUM 3
#define WLAN_RF_FE_REF_POWER_NUM 6
#define WLAN_RF_FE_POWER_CURVE_NUM 9
#define WLAN_RF_FE_CURVE_FACTOR_NUM 6
#define WLAN_RF_FE_CALI_DATA_LEN 1021   // 校准数据结构总共1024长度 留给数据的1021 目前够用
#define WLAN_RF_FE_CALI_DATA_STRUCT_SIZE 1024
#define WLAN_HILINK_SSID_LEN 33
#define WLAN_HILINK_PWD_LEN 65
#define WLAN_HILINK_MAC_LEN 6
/* 基础类型无需在此文件中定义，直接引用即可，对应app.json中的sample0 */

/* 普通结构体，对应app.json中的sample1 */
typedef struct {
    int8_t param1;
    int8_t param2;
    uint32_t param3;
    uint32_t param4;
    uint8_t param5;
} sample1_type_t;

/* 普通数组，对应app.json中的sample2 */
typedef struct {
    uint8_t mac[WLAN_MAC_ADDR_LEN];
} mac_type_t;

typedef struct {
    uint8_t country[WLAN_COUNTRY_CODE_LEN];
} country_type_t;

typedef struct {
    int8_t xo_trim_temp_fine_code[WLAN_XO_TRIM_TEMP_LEN];
} xo_trim_temp_type_t;

/* 多类型结构嵌套，对应app.json中的sample3 */
typedef struct {
    uint32_t param1[WLAN_MAC_ADDR_LEN];
    uint32_t param2;
    uint8_t param3;
    sample1_type_t param4;
} sample3_type_t;

/* 更为复杂的结构体嵌套，对应app.json中的sample4 */
typedef struct {
    sample1_type_t param1;
    mac_type_t param2;
    sample3_type_t param3;
} sample4_type_t;

typedef struct {
    uint8_t rx_insert_loss[WLAN_RF_FE_RX_LOSS_NUM];
} fe_rx_insert_loss_type_t;

typedef struct {
    uint8_t chip_max_power[WLAN_RF_FE_MAX_POWER_NUM];
    uint8_t target_power[WLAN_RF_FE_TARGET_POWER_NUM];
    uint8_t limit_power[WLAN_RF_FE_LIMIT_POWER_NUM];
    uint8_t sar_power[WLAN_RF_FE_SAR_POWER_NUM];
} fe_tx_power_type_t;

typedef struct {
    uint8_t rx_rssi_comp[WLAN_RF_FE_RSSI_COMP_NUM];
} fe_rx_rssi_comp_type_t;

typedef struct {
    uint16_t ref_power[WLAN_RF_FE_REF_POWER_NUM];
} fe_tx_ref_power_type_t;

typedef struct {
    uint16_t power_curve[WLAN_RF_FE_POWER_CURVE_NUM];
} fe_tx_power_curve_type_t;

typedef struct {
    uint16_t curve_factor[WLAN_RF_FE_CURVE_FACTOR_NUM];
} fe_tx_curve_factor_type_t;

typedef struct {
    uint16_t cali_data_len;
    uint8_t cali_data_received;
    uint8_t data[WLAN_RF_FE_CALI_DATA_LEN];
} fe_cali_data_type_t;

/* btc功率相关在此结构体中添加 */
typedef struct {
    uint8_t btc_max_txpower;
} btc_power_type_t;

/* 雷达算法相关在此结构体中添加 */
typedef struct {
    uint8_t d_th_1m;              // dispersion
    uint8_t d_th_2m;
    uint8_t d_buf_cnt_1m;
    uint8_t d_buf_cnt_2m;
    uint8_t d_use_2md1m;
    uint8_t d_use_1m_dfa;
    uint8_t t_only_enable;
    uint8_t d_th_1m_dfa;
    uint8_t a_always;
    uint8_t d_buf_cnt_1m_dfa;
    uint8_t d_buf_cnt_2m_dfa;
    uint8_t p_buf_cnt;           // presence
    uint8_t p_th;
    uint8_t t_th_1m;             // track
    uint8_t t_th_2m;
    uint8_t b_th_ratio;          // bitmap
    uint8_t b_th_cnt;
    uint8_t a_th;                // ai
    uint8_t a_buf_cnt;
    uint8_t res_0;
} radar_alg_param_t;

typedef struct {
    uint8_t height;               // 模组安装架高信息: 1/2/3米
    uint8_t scenario;             // 场景: 家居/空旷
    uint8_t wifi_mode;            // WIFI模式: STA/SOFTAP
    uint8_t material;             // 模组视距方向遮挡材料: 塑料/金属
    uint8_t fusion_track;         // 是否融合距离跟踪结果
    uint8_t fusion_ai;            // 是否融合AI结果
    uint16_t quit_dly_time;       // 退出时间
} radar_alg_ctrl_t;

/* hilink配网 ssid与密码 */
typedef struct {
    uint8_t ssid[WLAN_HILINK_SSID_LEN];
    uint8_t pwd[WLAN_HILINK_PWD_LEN];
    uint8_t bssid[WLAN_HILINK_MAC_LEN];
} hilink_connect_info_t;
#endif /* __NV_COMMON_CFG_H__ */
