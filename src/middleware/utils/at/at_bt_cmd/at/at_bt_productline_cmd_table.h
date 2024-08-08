/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: At bt productline cmd tab \n
 */

#ifndef AT_BT_PRODUCTLINE_CMD_TALBE_H
#define AT_BT_PRODUCTLINE_CMD_TALBE_H
#include "at.h"
#include "bts_factory.h"

/* 产线校准 */
typedef struct {
    uint32_t para_map;
    int32_t coarse_val;
    int32_t fine_val;
} bt_pdl_xo_trim_t; // 频偏粗调

typedef struct {
    uint32_t para_map;
    int16_t temp;
} bt_pdl_temp_wr_efuse_t; // 写温度efuse

typedef struct {
    uint32_t para_map;
    int16_t target_pwr;      // 目标功率，单位：0.1 dBm
    int16_t msrd_pwr;        // 测量到的功率，单位：0.1 dBm
} bt_pdl_pwr_cali_set_pwr_t; // 设置目标功率和实测功率

/* 产线RF测试 */
typedef struct {
    uint32_t para_map;
    int32_t tx_freq;
    int32_t test_data_len;
    int32_t pk_payload_type;
    int32_t tx_phy;
} ble_rf_tx_param_args_t; // BLE TX

typedef struct {
    uint32_t para_map;
    int32_t rx_freq;
    int32_t rx_phy;
    int32_t modulation_index;
} ble_rf_rx_param_args_t; // BLE RX

typedef struct {
    uint32_t para_map;
    int32_t tx_freq;
    int32_t tx_power;
    int32_t test_data_len;
    int32_t pk_payload_type;
    int32_t tx_phy;
    int32_t tx_format;
    int32_t tx_rate;
    int32_t tx_pilot_ratio;
    int32_t tx_polar_r;
    int32_t tx_interval;
} sle_rf_tx_param_args_t; // SLE TX

typedef struct {
    uint32_t para_map;
    int32_t rx_freq;
    int32_t rx_phy;
    int32_t rx_format;
    int32_t rx_pilot_ratio;
    int32_t rx_interval;
} sle_rf_rx_param_args_t; // SLE RX

typedef struct {
    uint32_t para_map;
    int32_t rf_freq;
    int32_t rf_mode;
} rf_single_tone_param_args_t; // 单音

/* AT命令参数格式 */
const at_para_parse_syntax_t pdl_xo_trim_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0x0,
        .entry.int_range.max_val = 0xF,
        .offset = offsetof(bt_pdl_xo_trim_t, coarse_val)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0x0,
        .entry.int_range.max_val = 0xFF,
        .offset = offsetof(bt_pdl_xo_trim_t, fine_val)
    },
};

const at_para_parse_syntax_t pdl_set_temp_efuse_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = -40, // 最低-40 ℃
        .entry.int_range.max_val = 120, // 最高120 ℃
        .offset = offsetof(bt_pdl_temp_wr_efuse_t, temp)
    },
};

const at_para_parse_syntax_t pdl_pwr_cali_set_pwr_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 300, // 300：不超过30dBm
        .offset = offsetof(bt_pdl_pwr_cali_set_pwr_t, target_pwr)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 300, // 300：不超过30dBm
        .offset = offsetof(bt_pdl_pwr_cali_set_pwr_t, msrd_pwr)
    },
};

const int32_t ble_pk_payload_type_values[] = {
    0, 1, 2, 3, 4, 5, 6, 7
};
const int32_t ble_rf_tx_phy_values[] = {
    1, 2, 3, 4
};
const at_para_parse_syntax_t ble_rf_tx_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 39,
        .offset = offsetof(ble_rf_tx_param_args_t, tx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 37,
        .entry.int_range.max_val = 255,
        .offset = offsetof(ble_rf_tx_param_args_t, test_data_len)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {8, ble_pk_payload_type_values},
        .offset = offsetof(ble_rf_tx_param_args_t, pk_payload_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, ble_rf_tx_phy_values},
        .offset = offsetof(ble_rf_tx_param_args_t, tx_phy)
    },
};

const int32_t ble_rf_rx_phy_values[] = {
    1, 2, 3, 4
};
const int32_t ble_modulation_index_values[] = {
    0, 1
};
const at_para_parse_syntax_t ble_rf_rx_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 39,
        .offset = offsetof(ble_rf_rx_param_args_t, rx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, ble_rf_rx_phy_values},
        .offset = offsetof(ble_rf_rx_param_args_t, rx_phy)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, ble_modulation_index_values},
        .offset = offsetof(ble_rf_rx_param_args_t, modulation_index)
    },
};

const int32_t sle_rf_tx_power_values[] = {
    0, 1, 2, 3, 4, 5, 6, 7
};
const int32_t sle_rf_tx_payload_type_values[] = {
    0, 1, 2, 3, 4, 5, 6, 7
};
const int32_t sle_rf_tx_phy_values[] = {
    0, 1, 4
};
const int32_t sle_rf_tx_format_values[] = {
    0, 2, 3
};
const int32_t sle_rf_tx_rate_values[] = {
    0, 2, 3
};
const int32_t sle_rf_tx_pilot_ratio_values[] = {
    0, 1, 2, 3
};
const int32_t sle_rf_tx_polar_r_values[] = {
    0, 1, 2, 3
};
const at_para_parse_syntax_t sle_rf_tx_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 78,
        .offset = offsetof(sle_rf_tx_param_args_t, tx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {8, sle_rf_tx_power_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_power)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 37,
        .entry.int_range.max_val = 256,
        .offset = offsetof(sle_rf_tx_param_args_t, test_data_len)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {8, sle_rf_tx_payload_type_values},
        .offset = offsetof(sle_rf_tx_param_args_t, pk_payload_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, sle_rf_tx_phy_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_phy)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, sle_rf_tx_format_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_format)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, sle_rf_tx_rate_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_rate)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, sle_rf_tx_pilot_ratio_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_pilot_ratio)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, sle_rf_tx_polar_r_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_polar_r)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 7,
        .entry.int_range.max_val = 255,
        .offset = offsetof(sle_rf_tx_param_args_t, tx_interval)
    },
};

const int32_t sle_rf_rx_phy_values[] = {
    0, 1, 4
};
const int32_t sle_rf_rx_format_values[] = {
    0, 2
};
const int32_t sle_rf_rx_pilot_ratio_values[] = {
    0, 1, 2, 3
};
const at_para_parse_syntax_t sle_rf_rx_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 79,
        .offset = offsetof(sle_rf_rx_param_args_t, rx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, sle_rf_rx_phy_values},
        .offset = offsetof(sle_rf_rx_param_args_t, rx_phy)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, sle_rf_rx_format_values},
        .offset = offsetof(sle_rf_rx_param_args_t, rx_format)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, sle_rf_rx_pilot_ratio_values},
        .offset = offsetof(sle_rf_rx_param_args_t, rx_pilot_ratio)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 7,
        .entry.int_range.max_val = 255,
        .offset = offsetof(sle_rf_rx_param_args_t, rx_interval)
    },
};

const int32_t rf_single_tone_mode[] = {
    0, 1, 255
};
const at_para_parse_syntax_t rf_single_tone_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 79,
        .offset = offsetof(rf_single_tone_param_args_t, rf_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, rf_single_tone_mode},
        .offset = offsetof(rf_single_tone_param_args_t, rf_mode)
    },
};

/* 注册回调 */
/* CALLBACK */
at_ret_t bt_at_enable_sle_cmd(void);
at_ret_t bt_at_ble_register_callback_cmd(void);
at_ret_t bt_at_sle_register_callback_cmd(void);

/* 单音 */
at_ret_t bt_at_rf_single_tone_cmd(const rf_single_tone_param_args_t *args);

/* 频偏校准 */
at_ret_t bt_at_pdl_xo_trim_cmd(bt_pdl_xo_trim_t *args);
at_ret_t bt_at_pdl_xo_trim_rd_cmd(void);
at_ret_t bt_at_pdl_xo_trim_get_temp_cmd(void);
at_ret_t bt_at_pdl_xo_trim_wr_efuse_cmd(void);
at_ret_t bt_at_pdl_xo_trim_rd_efuse_cmd(void);
at_ret_t bt_at_pdl_temp_wr_efuse_cmd(bt_pdl_temp_wr_efuse_t *args);
at_ret_t bt_at_pdl_temp_rd_efuse_cmd(void);

/* 功率校准 */
at_ret_t bt_at_pdl_pwr_cali_set_pwr_cmd(bt_pdl_pwr_cali_set_pwr_t *args);
at_ret_t bt_at_pdl_pwr_cali_get_val_cmd(void);
at_ret_t bt_at_pdl_pwr_cali_apply_cmd(void);
at_ret_t bt_at_pdl_pwr_cali_wr_efuse_cmd(void);
at_ret_t bt_at_pdl_pwr_cali_rd_efuse_cmd(void);

/* BLE TESTMODE */
at_ret_t bt_at_ble_reset_cmd(void);
at_ret_t bt_at_ble_rf_tx_cmd(const ble_rf_tx_param_args_t *args);
at_ret_t bt_at_ble_rf_rx_cmd(const ble_rf_rx_param_args_t *args);
at_ret_t bt_at_ble_rf_trxend_cmd(void);

/* SLE TESTMODE */
at_ret_t bt_at_sle_reset_cmd(void);
at_ret_t bt_at_sle_rf_tx_cmd(const sle_rf_tx_param_args_t *args);
at_ret_t bt_at_sle_rf_rx_cmd(const sle_rf_rx_param_args_t *args);
at_ret_t bt_at_sle_rf_trxend_cmd(void);

/* AT+BLEPWR? */
at_ret_t cmd_get_bsle_pwr_cali_flag(void);

/* AT BT PRODUCTLINE CMD TABLE */
const at_cmd_entry_t at_bt_productline_cmd_parse_table[] = {
    {
        "SLEENABLE",
        1, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_enable_sle_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BLEFACCALLBACK",
        2, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_ble_register_callback_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "SLEFACCALLBACK",
        3, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_sle_register_callback_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BLETX",                            // NAME
        4,                                  // ID
        0,                                  // ATTRIBUTE
        ble_rf_tx_param_syntax,             // SYNTAX
        NULL,                               // cmd with no args, eg: AT+TEST
        (at_set_func_t)bt_at_ble_rf_tx_cmd, // cmd with args, eg: AT+TEST=XX,XX,XX,XX
        NULL,                               // read cmd, eg: AT+TEST?
        NULL,                               // test cmd, eg: AT+TEST=?
    },
    {
        "BLERX",                            // NAME
        5,                                  // ID
        0,                                  // ATTRIBUTE
        ble_rf_rx_param_syntax,             // SYNTAX
        NULL,                               // cmd with no args, eg: AT+TEST
        (at_set_func_t)bt_at_ble_rf_rx_cmd, // cmd with args, eg: AT+TEST=XX,XX,XX,XX
        NULL,                               // read cmd, eg: AT+TEST?
        NULL,                               // test cmd, eg: AT+TEST=?
    },
    {
        "BLETRXEND",
        6, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_ble_rf_trxend_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BLERST",
        7, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_ble_reset_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "SLETX",
        8, // ID
        0, // Attribute
        sle_rf_tx_param_syntax,
        NULL,
        (at_set_func_t)bt_at_sle_rf_tx_cmd,
        NULL,
        NULL,
    },
    {
        "SLERX",
        9, // ID
        0, // Attribute
        sle_rf_rx_param_syntax,
        NULL,
        (at_set_func_t)bt_at_sle_rf_rx_cmd,
        NULL,
        NULL,
    },
    {
        "SLETRXEND",
        10, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_sle_rf_trxend_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "SLERST",
        11, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_sle_reset_cmd,
        NULL,
        NULL,
        NULL,
    },
#if defined(BGLE_FEATURE_MFG_TEST)
    {
        "XOTRIM",
        12, // ID
        0,  // Attribute
        pdl_xo_trim_param_syntax,
        NULL,
        (at_set_func_t)bt_at_pdl_xo_trim_cmd,
        (at_read_func_t)bt_at_pdl_xo_trim_rd_cmd, // 读XO
        NULL,
    },
    {
        "READTEMP",
        13, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_pdl_xo_trim_get_temp_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "XOEFUSE",
        14, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_pdl_xo_trim_wr_efuse_cmd, // 写XO efuse
        NULL,
        (at_read_func_t)bt_at_pdl_xo_trim_rd_efuse_cmd, // 读XO efuse
        NULL,
    },
    {
        "TEMPEFUSE",
        15, // ID
        0,  // Attribute
        pdl_set_temp_efuse_param_syntax,
        NULL,
        (at_set_func_t)bt_at_pdl_temp_wr_efuse_cmd,  // 写温度 efuse
        (at_read_func_t)bt_at_pdl_temp_rd_efuse_cmd, // 读温度 efuse
        NULL,
    },
    {
        "PWRCALI",
        16, // ID
        0,  // Attribute
        pdl_pwr_cali_set_pwr_param_syntax,
        (at_cmd_func_t)bt_at_pdl_pwr_cali_apply_cmd,    // 应用校准值
        (at_set_func_t)bt_at_pdl_pwr_cali_set_pwr_cmd,  // 设置参数，目标功率和实测功率
        (at_read_func_t)bt_at_pdl_pwr_cali_get_val_cmd, // 读功率校准补偿值
        NULL,
    },
    {
        "PWRCALIEFUSE",
        17, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_pdl_pwr_cali_wr_efuse_cmd, // 写功率校准efuse
        NULL,
        (at_read_func_t)bt_at_pdl_pwr_cali_rd_efuse_cmd, // 读功率校准efuse
        NULL,
    },
#endif
    {
        "BTTXLO",
        18, // ID
        0,  // Attribute
        rf_single_tone_param_syntax,
        NULL,
        (at_set_func_t)bt_at_rf_single_tone_cmd,
        NULL,
        NULL,
    },
    {
        "BLEPWR",
        19,
        0,
        NULL,
        (at_cmd_func_t)cmd_get_bsle_pwr_cali_flag,
        NULL,
        NULL,
        NULL,
    },
};
#endif // AT_BT_PRODUCTLINE_CMD_TALBE_H
