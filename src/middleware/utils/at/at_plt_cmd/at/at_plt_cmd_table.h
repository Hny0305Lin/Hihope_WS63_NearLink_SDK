/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: At plt cmd table \n
 */

#if !defined(AT_PLT_CMD_TALBE_H)
#define AT_PLT_CMD_TALBE_H

#include "at.h"
typedef struct {
    uint32_t                para_map;
    int32_t                 key_id; /* Range: 0..65535 */
} nvread_args_t;

typedef struct {
    uint32_t                para_map;
    uint32_t                key_id; /* Range: 0..65535 */
    int32_t                 attr;
    uint32_t                length; /* Range: 0..32 */
    const uint8_t           *value; /*    Length: 32 */
} nvwrite_args_t;

typedef struct {
    uint32_t                para_map;
    uint32_t                run_region;
} absetrun_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *mac_addr; /* Optional.   Length: 17 */
    int32_t                 mac_type;
} efusemac_args_t;

typedef struct {
    uint32_t                para_map;
} reboot_args_t;

typedef struct {
    uint32_t                para_map;
} mfgflag_args_t;

typedef struct {
    uint32_t                para_map;
} dieid_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* 每个项目存在差异 */
} customer_rsvd_efuse_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1;
} ssijtag_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 66 */
} pubkey_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 para1;
} secverify_args_t;

typedef struct {
    uint32_t                para_map;
    uint32_t                 addr; /* Range: 0.. */
    uint32_t                 length; /* Range: 0..4096 */
} flashread_args_t;

typedef struct {
    uint32_t                para_map;
    uint32_t                 addr; /* Range: 0.. */
    uint32_t                 length; /* Range: 0..1024 */
    const uint8_t           *data; /* Optional.   Length: 2048 */
} flashwrite_args_t;

typedef union {
    nvread_args_t   nvread;
    nvwrite_args_t  nvwrite;
    absetrun_args_t absetrun;
    efusemac_args_t efusemac;
    reboot_args_t   reboot;
    mfgflag_args_t  mfgflag;
    dieid_args_t    dieid;
    customer_rsvd_efuse_args_t  customer_rsvd_efuse;
    ssijtag_args_t ssijtag;
    pubkey_args_t pubkey;
    secverify_args_t secverify;
    flashread_args_t flashread;
    flashwrite_args_t flashwrite;
} at_plt_cmd_args_t;

// factory mode at cmd args
typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~7 */
} factory_mode_args_t;

// uart log level at cmd args
typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~7 */
} loglevel_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~2 */
} sleepmode_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0..128 */
} pm_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~2 */
    int32_t para2; /* Range: 0~2 */
    int32_t para3; /* Range: 0~2 */
} uartport_args_t;

// gpio at cmd args
typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~14 */
} getiomode_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~14 */
    int32_t para2; /* Range: 0~14 */
    int32_t para3; /* Range: 0~14 */
    int32_t para4; /* Range: 0~14 */
} setiomode_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~14 */
    int32_t para2;
} gpiodir_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~14 */
    int32_t para2;
} wrgpio_args_t;

typedef struct {
    uint32_t para_map;
    int32_t para1; /* Range: 0~14 */
} rdgpio_args_t;

typedef struct {
    uint32_t para_map;
    const char *para1; /* Length: 32 */
} date_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *license; /*    Length: 4096 */
} license_args_t;

/* AT Command */
at_ret_t plt_nv_read(const nvread_args_t *args);

/* AT Command */
at_ret_t plt_nv_write(const nvwrite_args_t *args);

/* SET AB RUN REGION */
at_ret_t upg_ab_set_run_region(const absetrun_args_t *args);

/* SET MAC ADDR */
at_ret_t get_efuse_mac_addr(void);
at_ret_t set_efuse_mac_addr(const efusemac_args_t *args);

/* AT Command */
at_ret_t plt_reboot(void);

/* AT+CSV */
at_ret_t at_query_ver_cmd(void);

/* AT+RDTEMP */
at_ret_t at_query_tsensor_temp(void);

/* AT+XTALCOM */
at_ret_t at_query_xtal_compesation(void);

/* AT+FTM */
at_ret_t at_factory_erase(void);
at_ret_t at_factory_mode_switch(const factory_mode_args_t *args);
at_ret_t at_factory_mode_read(void);

/* AT+LOGL */
at_ret_t at_get_log_level(void);
at_ret_t at_set_log_level(const loglevel_args_t *args);

/* AT+SLP */
at_ret_t at_get_sleep_mode(void);
at_ret_t at_set_sleep_mode(const sleepmode_args_t *args);

/* AT+SETUART */
at_ret_t at_set_uart_port(const uartport_args_t *args);

/* AT+GPIODIR */
at_ret_t at_set_gpio_dir(const gpiodir_args_t *args);

/* AT+GETIOMODE */
at_ret_t at_get_iomode(const getiomode_args_t *args);

/* AT+SETIOMODE */
at_ret_t at_set_iomode(const setiomode_args_t *args);

/* AT+WTGPIO */
at_ret_t at_wrgpio(const wrgpio_args_t *args);

/* AT+RDGPIO */
at_ret_t at_rdgpio(const rdgpio_args_t *args);

/* AT+DUMP */
at_ret_t at_get_dump(void);

/* AT+RST */
at_ret_t at_exe_reset_cmd(void);

/* AT+DATE */
at_ret_t at_date_cmd(void);
at_ret_t at_date_set_cmd(const date_args_t *args);

/* AT+HELP */
at_ret_t at_help(void);

/* AT+PM */
at_ret_t cmd_set_pm(const pm_args_t *args);

/* AT+MFGFLAG */
at_ret_t cmd_write_mfg_flag(void);

/* AT+DIEID */
at_ret_t cmd_get_dieid(void);

/* AT+CUSTOMEFUSE */
at_ret_t cmd_set_customer_rsvd_efuse(const customer_rsvd_efuse_args_t *args);
at_ret_t cmd_get_customer_rsvd_efuse(void);

/* AT+SSIJTAG */
at_ret_t cmd_disable_ssi_jtag(void);
at_ret_t cmd_get_ssi_jtag_status(void);

/* AT+PUBKEY */
at_ret_t cmd_set_hash_root_public_key(const pubkey_args_t *args);
at_ret_t cmd_get_hash_root_public_key(void);

/* AT+SECVERIFY */
at_ret_t cmd_sec_verify_enable(void);
at_ret_t cmd_get_sec_verify_status(void);

/* AT Command */
at_ret_t plt_flash_read(const flashread_args_t *args);

/* AT Command */
at_ret_t plt_flash_write(const flashwrite_args_t *args);

/* AT+LICENSE */
at_ret_t save_license(const license_args_t *args);

const at_para_parse_syntax_t nvread_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(nvread_args_t, key_id)
    },
};

const int32_t nvwrite_para2_values[] = {
    0, 1, 2, 4
};
const at_para_parse_syntax_t nvwrite_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(nvwrite_args_t, key_id)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, nvwrite_para2_values},
        .offset = offsetof(nvwrite_args_t, attr)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1024,
        .offset = offsetof(nvwrite_args_t, length)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 2048,
        .offset = offsetof(nvwrite_args_t, value)
    },
};

const int32_t absetrun_run_region_values[] = {
    0, 1, 2
};

const at_para_parse_syntax_t absetrun_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, absetrun_run_region_values},
        .offset = offsetof(absetrun_args_t, run_region)
    },
};

const int32_t efusemac_mac_type_values[] = {
    0, 1, 2
};
const at_para_parse_syntax_t efusemac_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 17,
        .offset = offsetof(efusemac_args_t, mac_addr)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {(sizeof(efusemac_mac_type_values) / sizeof(int32_t)), efusemac_mac_type_values},
        .offset = offsetof(efusemac_args_t, mac_type)
    },
};

const at_para_parse_syntax_t factory_mode_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(factory_mode_args_t, para1)
    },
};

const at_para_parse_syntax_t loglevel_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7,
        .offset = offsetof(loglevel_args_t, para1)
    },
};

const at_para_parse_syntax_t sleepmode_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(sleepmode_args_t, para1)
    },
};

const at_para_parse_syntax_t uartport_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(uartport_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(uartport_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(uartport_args_t, para3)
    },
};

const at_para_parse_syntax_t getiomode_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(getiomode_args_t, para1)
    },
};

const at_para_parse_syntax_t setiomode_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(setiomode_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(setiomode_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(setiomode_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(setiomode_args_t, para4)
    },
};

const int32_t gpiodir_para2_values[] = {
    0, 1
};

const at_para_parse_syntax_t gpiodir_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(gpiodir_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, gpiodir_para2_values},
        .offset = offsetof(gpiodir_args_t, para2)
    },
};

const int32_t wrgpio_para2_values[] = {
    0, 1
};

const at_para_parse_syntax_t wrgpio_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(wrgpio_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, wrgpio_para2_values},
        .offset = offsetof(wrgpio_args_t, para2)
    },
};

const at_para_parse_syntax_t pm_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 128,
        .offset = offsetof(pm_args_t, para1)
    },
};

const at_para_parse_syntax_t rdgpio_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(rdgpio_args_t, para1)
    },
};

const at_para_parse_syntax_t date_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(date_args_t, para1)
    },
};

const at_para_parse_syntax_t customer_rsvd_efuse_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 130,
        .offset = offsetof(customer_rsvd_efuse_args_t, para1)
    },
};

const at_para_parse_syntax_t pubkey_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 66,
        .offset = offsetof(pubkey_args_t, para1)
    },
};

const at_para_parse_syntax_t flashread_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE,
        .entry.int_range.min_val = 0,
        .offset = offsetof(flashread_args_t, addr)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 4096,
        .offset = offsetof(flashread_args_t, length)
    },
};

const at_para_parse_syntax_t flashwrite_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE,
        .entry.int_range.min_val = 0,
        .offset = offsetof(flashwrite_args_t, addr)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1024,
        .offset = offsetof(flashwrite_args_t, length)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 2048,
        .offset = offsetof(flashwrite_args_t, data)
    },
};

const at_para_parse_syntax_t license_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4096,
        .offset = offsetof(license_args_t, license)
    },
};

const at_cmd_entry_t at_plt_cmd_parse_table[] = {
    {
        "NVREAD",
        2,
        0,
        nvread_syntax,
        NULL,
        (at_set_func_t)plt_nv_read,
        NULL,
        NULL,
    },
    {
        "NVWRITE",
        1,
        0,
        nvwrite_syntax,
        NULL,
        (at_set_func_t)plt_nv_write,
        NULL,
        NULL,
    },
    {
        "ABSETRUN",
        2,
        0,
        absetrun_syntax,
        NULL,
        (at_set_func_t)upg_ab_set_run_region,
        NULL,
        NULL,
    },
    {
        "EFUSEMAC",
        2,
        0,
        efusemac_syntax,
        NULL,
        (at_set_func_t)set_efuse_mac_addr,
        get_efuse_mac_addr,
        NULL,
    },
    {
        "REBOOT",
        2,
        0,
        NULL,
        plt_reboot,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "CSV",
        8,
        0,
        NULL,
        at_query_ver_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "RDTEMP",
        8,
        0,
        NULL,
        at_query_tsensor_temp,
        NULL,
        NULL,
        NULL,
    },
    {
        "XTALCOM",
        8,
        0,
        NULL,
        at_query_xtal_compesation,
        NULL,
        NULL,
        NULL,
    },
    {
        "FTMERASE",
        8,
        0,
        NULL,
        at_factory_erase,
        NULL,
        NULL,
        NULL,
    },
    {
        "FTM",
        8,
        0,
        factory_mode_syntax,
        NULL,
        (at_set_func_t)at_factory_mode_switch,
        NULL,
        (at_read_func_t)at_factory_mode_read,
    },
    {
        "LOGL",
        8,
        0,
        loglevel_syntax,
        at_get_log_level,
        (at_set_func_t)at_set_log_level,
        NULL,
        NULL,
    },
    {
        "PM",
        8,
        0,
        pm_syntax,
        NULL,
        (at_set_func_t)cmd_set_pm,
        NULL,
        NULL,
    },
    {
        "SLP",
        8,
        0,
        sleepmode_syntax,
        at_get_sleep_mode,
        (at_set_func_t)at_set_sleep_mode,
        NULL,
        NULL,
    },
    {
        "SETUART",
        8,
        0,
        uartport_syntax,
        NULL,
        (at_set_func_t)at_set_uart_port,
        NULL,
        NULL,
    },
    {
        "GPIODIR",
        8,
        0,
        gpiodir_syntax,
        NULL,
        (at_set_func_t)at_set_gpio_dir,
        NULL,
        NULL,
    },
    {
        "GETIOMODE",
        8,
        0,
        getiomode_syntax,
        NULL,
        (at_set_func_t)at_get_iomode,
        NULL,
        NULL,
    },
    {
        "SETIOMODE",
        8,
        0,
        setiomode_syntax,
        NULL,
        (at_set_func_t)at_set_iomode,
        NULL,
        NULL,
    },

    {
        "RDGPIO",
        8,
        0,
        rdgpio_syntax,
        NULL,
        (at_set_func_t)at_rdgpio,
        NULL,
        NULL,
    },
    {
        "WTGPIO",
        8,
        0,
        wrgpio_syntax,
        NULL,
        (at_set_func_t)at_wrgpio,
        NULL,
        NULL,
    },
    {
        "HELP",
        4,
        0,
        NULL,
        at_help,
        NULL,
        NULL,
        NULL,
    },
    {
        "DUMP",
        8,
        0,
        NULL,
        at_get_dump,
        NULL,
        NULL,
        NULL,
    },
    {
        "RST",
        8,
        0,
        NULL,
        at_exe_reset_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "DATE",
        8,
        0,
        date_syntax,
        at_date_cmd,
        (at_set_func_t)at_date_set_cmd,
        NULL,
        NULL,
    },
    {
        "MFGFLAG",
        9,
        0,
        NULL,
        cmd_write_mfg_flag,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "DIEID",
        6,
        0,
        NULL,
        cmd_get_dieid,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "CUSTOMEFUSE",
        7,
        0,
        customer_rsvd_efuse_syntax,
        NULL,
        (at_set_func_t)cmd_set_customer_rsvd_efuse,
        cmd_get_customer_rsvd_efuse,
        NULL,
    },
    {
        "JTAG",
        7,
        0,
        NULL,
        cmd_disable_ssi_jtag,
        (at_set_func_t)NULL,
        cmd_get_ssi_jtag_status,
        NULL,
    },
    {
        "PUBKEY",
        7,
        0,
        pubkey_syntax,
        NULL,
        (at_set_func_t)cmd_set_hash_root_public_key,
        cmd_get_hash_root_public_key,
        NULL,
    },
    {
        "SECVERIFY",
        10,
        0,
        NULL,
        cmd_sec_verify_enable,
        (at_set_func_t)NULL,
        cmd_get_sec_verify_status,
        NULL,
    },
    {
        "FLASHREAD",
        10,
        0,
        flashread_syntax,
        NULL,
        (at_set_func_t)plt_flash_read,
        NULL,
        NULL,
    },
    {
        "FLASHWRITE",
        11,
        0,
        flashwrite_syntax,
        NULL,
        (at_set_func_t)plt_flash_write,
        NULL,
        NULL,
    },
    {
        "LICENSE",
        2,
        0,
        license_syntax,
        NULL,
        (at_set_func_t)save_license,
        NULL,
        NULL,
    },
};

#endif  /* AT_PLT_CMD_AT_CMD_TALBE_H */

