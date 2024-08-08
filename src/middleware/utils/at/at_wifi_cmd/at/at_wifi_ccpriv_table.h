/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: struction of AT command and public function
 */

#if ! defined (AT_WIFI_CCPRIV_AT_CMD_TALBE_H)
#define AT_WIFI_CCPRIV_AT_CMD_TALBE_H

#include "at.h"
typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1;
    const uint8_t           *para2; /* Optional. */
    const uint8_t           *para3; /* Optional.   Length: 32 */
    const uint8_t           *para4; /* Optional.   Length: 32 */
    const uint8_t           *para5; /* Optional. */
} alrx_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1;
    const uint8_t           *para2; /* Optional. */
    const uint8_t           *para3; /* Optional.   Length: 32 */
    const uint8_t           *para4; /* Optional.   Length: 32 */
} altx_args_t;

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
    const uint8_t           *para11; /* Optional.   Length: 32 */
    const uint8_t           *para12; /* Optional.   Length: 32 */
    const uint8_t           *para13; /* Optional.   Length: 32 */
    const uint8_t           *para14; /* Optional.   Length: 32 */
    const uint8_t           *para15; /* Optional.   Length: 32 */
} ccpriv_args_t;

typedef struct {
    uint32_t                para_map;
} rxinfo_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..1 */
} arlog_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    const uint8_t           *para2; /*    Length: 32 */
} usrinfo_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
} vapinfo_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 5 */
    const uint8_t           *para2; /*    Length: 5 */
    const uint8_t           *para3; /*    Length: 5 */
} setrpwr_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 1 */
    const uint8_t           *para2; /*    Length: 6 */
} caltone_args_t;

typedef union {
    alrx_args_t     alrx;
    altx_args_t     altx;
    ccpriv_args_t   ccpriv;
    rxinfo_args_t   rxinfo;
    arlog_args_t    arlog;
    usrinfo_args_t  usrinfo;
    vapinfo_args_t  vapinfo;
} at_wifi_ccpriv_args_t;

/* AT Command ALRX */
at_ret_t at_los_wifi_al_rx(const alrx_args_t *args);

/* AT Command ALTX */
at_ret_t at_los_wifi_al_tx(const altx_args_t *args);

/* AT Command CCPRIV */
at_ret_t at_los_wifi_ccpriv(const ccpriv_args_t *args);

/* AT Command RXINFO */
at_ret_t at_los_wifi_rx_info(void);
/* AT+ARLOG */
at_ret_t at_los_wifi_set_arlog(const arlog_args_t *args);

/* AT+USRINFO */
at_ret_t at_los_wifi_get_usr_info(const usrinfo_args_t *args);

/* AT+VAPINFO */
at_ret_t at_los_wifi_get_vap_info(const vapinfo_args_t *args);

/* AT+SETRPWR */
at_ret_t at_los_wifi_set_rate_power(const setrpwr_args_t *args);

/* AT+CALTONE */
at_ret_t at_los_wifi_cal_tone(const caltone_args_t *args);

const uint8_t * const alrx_para1_values[] = {
    (uint8_t *)"0",
    (uint8_t *)"1"
};
const uint8_t * const alrx_para2_values[] = {
    (uint8_t *)"0",
    (uint8_t *)"1",
    (uint8_t *)"2",
    (uint8_t *)"3",
    (uint8_t *)"4",
    (uint8_t *)"5",
    (uint8_t *)"6"
};
const uint8_t * const alrx_para5_values[] = {
    (uint8_t *)"0",
    (uint8_t *)"1"
};
const at_para_parse_syntax_t alrx_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.string_list = {2, alrx_para1_values},
        .offset = offsetof(alrx_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.string_list = {7, alrx_para2_values},
        .offset = offsetof(alrx_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(alrx_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(alrx_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.string_list = {2, alrx_para5_values},
        .offset = offsetof(alrx_args_t, para5)
    },
};

const uint8_t * const altx_para1_values[] = {
    (uint8_t *)"0",
    (uint8_t *)"1",
    (uint8_t *)"2"
};
const uint8_t * const altx_para2_values[] = {
    (uint8_t *)"0",
    (uint8_t *)"1",
    (uint8_t *)"2",
    (uint8_t *)"3",
    (uint8_t *)"4",
    (uint8_t *)"5",
    (uint8_t *)"6"
};
const at_para_parse_syntax_t altx_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.string_list = {3, altx_para1_values},
        .offset = offsetof(altx_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.string_list = {7, altx_para2_values},
        .offset = offsetof(altx_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(altx_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(altx_args_t, para4)
    },
};

const at_para_parse_syntax_t ccpriv_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para10)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para11)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para12)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para13)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para14)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ccpriv_args_t, para15)
    },
};

const at_para_parse_syntax_t arlog_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(arlog_args_t, para1)
    },
};

const at_para_parse_syntax_t usrinfo_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(usrinfo_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(usrinfo_args_t, para2)
    },
};

const at_para_parse_syntax_t vapinfo_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(vapinfo_args_t, para1)
    },
};
const at_para_parse_syntax_t setrpwr_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(setrpwr_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(setrpwr_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(setrpwr_args_t, para3)
    },
};
const at_para_parse_syntax_t caltone_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 1,
        .offset = offsetof(caltone_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 6,
        .offset = offsetof(caltone_args_t, para2)
    },
};

const at_cmd_entry_t at_wifi_mfg_parse_table[] = {
    {
        "ALRX",
        6,
        0,
        alrx_syntax,
        NULL,
        (at_set_func_t)at_los_wifi_al_rx,
        NULL,
        NULL,
    },
    {
        "ALTX",
        7,
        0,
        altx_syntax,
        NULL,
        (at_set_func_t)at_los_wifi_al_tx,
        NULL,
        NULL,
    },
    {
        "RXINFO",
        9,
        0,
        NULL,
        at_los_wifi_rx_info,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "ARLOG",
        16,
        0,
        arlog_syntax,
        NULL,
        (at_set_func_t)at_los_wifi_set_arlog,
        NULL,
        NULL,
    },
    {
        "USRINFO",
        15,
        0,
        usrinfo_syntax,
        NULL,
        (at_set_func_t)at_los_wifi_get_usr_info,
        NULL,
        NULL,
    },
    {
        "VAPINFO",
        14,
        0,
        vapinfo_syntax,
        NULL,
        (at_set_func_t)at_los_wifi_get_vap_info,
        NULL,
        NULL,
    },
    {
        "SETRPWR",
        18,
        0,
        setrpwr_syntax,
        NULL,
        (at_set_func_t)at_los_wifi_set_rate_power,
        NULL,
        NULL,
    },
    {
        "CALTONE",
        19,
        0,
        caltone_syntax,
        NULL,
        (at_set_func_t)at_los_wifi_cal_tone,
        NULL,
        NULL,
    },
};

const at_cmd_entry_t at_wifi_ccpriv_parse_table[] = {
    {
        "CCPRIV",
        8,
        0,
        ccpriv_syntax,
        NULL,
        (at_set_func_t)at_los_wifi_ccpriv,
        NULL,
        NULL,
    }
};
#endif  /* AT_WIFI_CCPRIV_AT_CMD_TALBE_H */

