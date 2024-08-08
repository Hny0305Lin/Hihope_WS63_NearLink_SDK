/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: struction of AT command and public function
 */

#if ! defined (AT_WIFI_MFG_CMD_TALBE_H)
#define AT_WIFI_MFG_CMD_TALBE_H
#ifdef _PRE_WLAN_FEATURE_MFG_TEST

#include "at.h"
#include "td_base.h"
typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    const uint8_t           *para2; /* Optional.   Length: 32 */
} calipower_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 30 */
} coarse_args_t;

typedef struct {
    uint32_t                para_map;
} efusepower_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    const uint8_t           *para2; /*    Length: 32 */
    const uint8_t           *para3; /*    Length: 32 */
} efuserssi_args_t;

typedef struct {
    uint32_t                para_map;
} rcaldata_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
} efusetemp_args_t;

typedef struct {
    uint32_t                para_map;
} efusexotrim_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    const uint8_t           *para2; /* Optional.   Length: 32 */
    const uint8_t           *para3; /* Optional.   Length: 32 */
    const uint8_t           *para4; /* Optional.   Length: 32 */
} factor_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 30 */
} fine_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
} getrssicomp_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    const uint8_t           *para2; /* Optional.   Length: 32 */
    const uint8_t           *para3; /* Optional.   Length: 32 */
    const uint8_t           *para4; /* Optional.   Length: 32 */
    const uint8_t           *para5; /* Optional.   Length: 32 */
    const uint8_t           *para6; /* Optional.   Length: 32 */
    const uint8_t           *para7; /* Optional.   Length: 32 */
    const uint8_t           *para8; /* Optional.   Length: 32 */
    const uint8_t           *para9; /* Optional.   Length: 32 */
    const uint8_t           *para10; /* Optional.   Length: 32 */
} highcurve_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    const uint8_t           *para2; /* Optional.   Length: 32 */
    const uint8_t           *para3; /* Optional.   Length: 32 */
    const uint8_t           *para4; /* Optional.   Length: 32 */
    const uint8_t           *para5; /* Optional.   Length: 32 */
    const uint8_t           *para6; /* Optional.   Length: 32 */
    const uint8_t           *para7; /* Optional.   Length: 32 */
    const uint8_t           *para8; /* Optional.   Length: 32 */
    const uint8_t           *para9; /* Optional.   Length: 32 */
    const uint8_t           *para10; /* Optional.   Length: 32 */
} lowcurve_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
} remain_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    const uint8_t           *para2; /*    Length: 32 */
} rssicomp_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    const uint8_t           *para2; /* Optional.   Length: 32 */
} tarpower_args_t;

typedef struct {
    uint32_t                para_map;
} temp_args_t;

typedef struct {
    uint32_t                para_map;
} xotrime_args_t;

typedef union {
    calipower_args_t calipower;
    coarse_args_t   coarse;
    efusepower_args_t efusepower;
    efuserssi_args_t efuserssi;
    rcaldata_args_t rcaldata;
    efusetemp_args_t efusetemp;
    efusexotrim_args_t efusexotrim;
    factor_args_t   factor;
    fine_args_t     fine;
    getrssicomp_args_t getrssicomp;
    highcurve_args_t highcurve;
    lowcurve_args_t lowcurve;
    remain_args_t   remain;
    rssicomp_args_t rssicomp;
    tarpower_args_t tarpower;
    temp_args_t     temp;
    xotrime_args_t  xotrime;
} at_wifi_args_t;

/* AT Command */
at_ret_t cmd_set_cali_power(const calipower_args_t *args);

/* AT Command */
at_ret_t cmd_set_cmu_xo_trim_coarse(const coarse_args_t *args);

/* AT Command */
at_ret_t cmd_set_efuse_power_info(void);
at_ret_t cmd_get_efuse_power_info(void);

/* AT Command */
at_ret_t cmd_set_efuse_rssi_offset(const efuserssi_args_t *args);
at_ret_t cmd_get_efuse_rssi_offset(void);

/* AT Command */
at_ret_t cmd_get_rcaldata(void);

/* AT Command */
at_ret_t cmd_set_efuse_temp(const efusetemp_args_t *args);
at_ret_t cmd_get_efuse_temp(void);

/* AT Command */
at_ret_t cmd_set_efuse_cmu_xo_trim(void);
at_ret_t cmd_get_efuse_cmu_xo_trim(void);

/* AT Command */
at_ret_t cmd_set_curve_factor(const factor_args_t *args);
at_ret_t cmd_get_curve_factor(void);

/* AT Command */
at_ret_t cmd_set_cmu_xo_trim_fine(const fine_args_t *args);

/* AT Command */
at_ret_t cmd_get_rssi_offset(const getrssicomp_args_t *args);

/* AT Command */
at_ret_t cmd_set_curve_param(const highcurve_args_t *args);
at_ret_t cmd_get_curve_param(void);

/* AT Command */
at_ret_t cmd_set_low_curve_param(const lowcurve_args_t *args);
at_ret_t cmd_get_low_curve_param(void);

/* AT Command */
at_ret_t cmd_efuse_remain(const remain_args_t *args);

/* AT Command */
at_ret_t cmd_set_rssi_offset(const rssicomp_args_t *args);

/* AT Command */
at_ret_t cmd_set_tar_power(const tarpower_args_t *args);

/* AT Command */
at_ret_t cmd_get_temp(void);

/* AT Command */
at_ret_t cmd_get_cmu_xo_trim(void);

const at_para_parse_syntax_t calipower_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(calipower_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(calipower_args_t, para2)
    },
};

const at_para_parse_syntax_t coarse_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 30,
        .offset = offsetof(coarse_args_t, para1)
    },
};

const at_para_parse_syntax_t efuserssi_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(efuserssi_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(efuserssi_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(efuserssi_args_t, para3)
    },
};

const at_para_parse_syntax_t efusetemp_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(efusetemp_args_t, para1)
    },
};

const at_para_parse_syntax_t factor_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(factor_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(factor_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(factor_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(factor_args_t, para4)
    },
};

const at_para_parse_syntax_t fine_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 30,
        .offset = offsetof(fine_args_t, para1)
    },
};

const at_para_parse_syntax_t getrssicomp_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(getrssicomp_args_t, para1)
    },
};

const at_para_parse_syntax_t highcurve_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(highcurve_args_t, para10)
    },
};

const at_para_parse_syntax_t lowcurve_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(lowcurve_args_t, para10)
    },
};

const at_para_parse_syntax_t remain_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(remain_args_t, para1)
    },
};

const at_para_parse_syntax_t rssicomp_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(rssicomp_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(rssicomp_args_t, para2)
    },
};

const at_para_parse_syntax_t tarpower_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(tarpower_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(tarpower_args_t, para2)
    },
};

const at_cmd_entry_t at_wifi_mfg_cmd_parse_table[] = {
    {
        "CALIPOWER",
        10,
        0,
        calipower_syntax,
        NULL,
        (at_set_func_t)cmd_set_cali_power,
        NULL,
        NULL,
    },
    {
        "COARSE",
        7,
        0,
        coarse_syntax,
        NULL,
        (at_set_func_t)cmd_set_cmu_xo_trim_coarse,
        NULL,
        NULL,
    },
    {
        "EFUSEPOWER",
        11,
        0,
        NULL,
        cmd_set_efuse_power_info,
        (at_set_func_t)NULL,
        cmd_get_efuse_power_info,
        NULL,
    },
    {
        "EFUSERSSI",
        10,
        0,
        efuserssi_syntax,
        NULL,
        (at_set_func_t)cmd_set_efuse_rssi_offset,
        cmd_get_efuse_rssi_offset,
        NULL,
    },
    {
        "RCALDATA",
        12,
        0,
        NULL,
        cmd_get_rcaldata,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "EFUSETEMP",
        10,
        0,
        efusetemp_syntax,
        NULL,
        (at_set_func_t)cmd_set_efuse_temp,
        cmd_get_efuse_temp,
        NULL,
    },
    {
        "EFUSEXOTRIM",
        12,
        0,
        NULL,
        cmd_set_efuse_cmu_xo_trim,
        (at_set_func_t)NULL,
        cmd_get_efuse_cmu_xo_trim,
        NULL,
    },
    {
        "FACTOR",
        7,
        0,
        factor_syntax,
        NULL,
        (at_set_func_t)cmd_set_curve_factor,
        cmd_get_curve_factor,
        NULL,
    },
    {
        "FINE",
        5,
        0,
        fine_syntax,
        NULL,
        (at_set_func_t)cmd_set_cmu_xo_trim_fine,
        NULL,
        NULL,
    },
    {
        "GETRSSICOMP",
        12,
        0,
        getrssicomp_syntax,
        NULL,
        (at_set_func_t)cmd_get_rssi_offset,
        NULL,
        NULL,
    },
    {
        "HIGHCURVE",
        10,
        0,
        highcurve_syntax,
        NULL,
        (at_set_func_t)cmd_set_curve_param,
        cmd_get_curve_param,
        NULL,
    },
    {
        "LOWCURVE",
        9,
        0,
        lowcurve_syntax,
        NULL,
        (at_set_func_t)cmd_set_low_curve_param,
        cmd_get_low_curve_param,
        NULL,
    },
    {
        "REMAIN",
        10,
        0,
        remain_syntax,
        NULL,
        (at_set_func_t)cmd_efuse_remain,
        NULL,
        NULL,
    },
    {
        "RSSICOMP",
        9,
        0,
        rssicomp_syntax,
        NULL,
        (at_set_func_t)cmd_set_rssi_offset,
        NULL,
        NULL,
    },
    {
        "TARPOWER",
        9,
        0,
        tarpower_syntax,
        NULL,
        (at_set_func_t)cmd_set_tar_power,
        NULL,
        NULL,
    },
    {
        "TEMP",
        5,
        0,
        NULL,
        cmd_get_temp,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "XOTRIME",
        8,
        0,
        NULL,
        NULL,
        (at_set_func_t)NULL,
        cmd_get_cmu_xo_trim,
        NULL,
    },
};

#endif
#endif  /* AT_WIFI_MFG_CMD_TALBE_H */

