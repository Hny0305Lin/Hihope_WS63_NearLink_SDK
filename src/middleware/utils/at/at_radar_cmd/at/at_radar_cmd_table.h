/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: At plt cmd table \n
 * Author:
 */

#if !defined(AT_RADAR_CMD_TALBE_H)
#define AT_RADAR_CMD_TALBE_H

#include "at.h"

typedef struct {
    uint32_t            para_map;
    int32_t             status; /* Range: 0..4 */
} radarsetst_args_t;

typedef struct {
    uint32_t            para_map;
    int32_t             times; /* Range: 0..20 */
    int32_t             loop; /* Range: 2..32 */
    int32_t             ant; /* Range: 0..3 */
    int32_t             wave; /* Range: 0..10 */
    int32_t             dbg_type; /* Range: 0..4 */
    int32_t             period; /* Range: 3000..20000 */
} radarsetpara_args_t;

typedef struct {
    uint32_t            para_map;
    int32_t             height; /* Range: 0..2 */
    int32_t             material; /* Range: 0..2 */
    int32_t             scenario; /* Range: 0..1 */
    int32_t             fusion_track; /* Range: 0..1 */
    int32_t             fusion_ai; /* Range: 0..1 */
} radaralgctrl_args_t;

typedef struct {
    uint32_t            para_map;
    uint32_t            d_th_1m; /* Range: 0..99 */
    uint32_t            d_th_2m; /* Range: 0..99 */
    uint32_t            p_th; /* Range: 0..99 */
    uint32_t            t_th_1m; /* Range: 0..99 */
    uint32_t            t_th_2m; /* Range: 0..99 */
    uint32_t            b_th_ratio; /* Range: 0..100 */
    uint32_t            b_th_cnt; /* Range: 0..99 */
    uint32_t            a_th; /* Range: 0..100 */
    uint32_t            write_to_nv; /* Range: 0..1 */
} radaralgpara_args_t;

typedef struct {
    uint32_t            para_map;
    uint32_t            dly_time; /* Range: 1..43200 */
} radarsetdlytime_args_t;

typedef struct {
    uint32_t            para_map;
    int32_t             gear; /* Range: 0..2 */
} radarsetledgear_args_t;

// 以下为产测命令
#ifdef CONFIG_RADAR_MFG
typedef struct {
    uint32_t            para_map;
    uint16_t            target_iso; /* Range: 0..40 */
    uint16_t            win; /* Range: 0..20 */
} mfgradartestiso_args_t;

typedef struct {
    uint32_t            para_map;
    int16_t             noise_thres; /* Range: 0..40 */
} mfgradartestnoise_args_t;
#endif

/* AT+RADARSETST=status */
at_ret_t at_radar_set_sts(const radarsetst_args_t *args);

/* AT+RADARSETPARA=times,loop,ant,wave,period */
at_ret_t at_radar_set_para(const radarsetpara_args_t *args);

/* AT+RADARALGCTRL=height,material,scenario,fusion_track,fusion_ai */
at_ret_t at_radar_alg_ctrl(const radaralgctrl_args_t *args);

/* AT+RADARALGPARA=参照参数列表 */
at_ret_t at_radar_alg_para(const radaralgpara_args_t *args);

/* AT+RADARGETST */
at_ret_t at_radar_get_sts(void);

/* AT+RADARGETISO */
at_ret_t at_radar_get_isolation(void);

/* AT+RADARSETDLY=dly_time */
at_ret_t at_radar_set_dly_time(const radarsetdlytime_args_t *args);

/* AT+RADARGETDLY */
at_ret_t at_radar_get_dly_time(void);

/* AT+RADARSETLED=gear */
at_ret_t at_radar_set_led_gear(const radarsetledgear_args_t *args);

// 以下为产测命令
#ifdef CONFIG_RADAR_MFG
/* AT+RADARTESTISO=target_iso,win */
at_ret_t at_mfg_radar_test_iso(const mfgradartestiso_args_t *args);

/* AT+RADARTESTNOISE=noise_thres */
at_ret_t at_mfg_radar_test_noise(const mfgradartestnoise_args_t *args);
#endif

const at_para_parse_syntax_t radarsetst_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 4,
        .offset = offsetof(radarsetst_args_t, status)
    },
};

const at_para_parse_syntax_t radarsetpara_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 20,
        .offset = offsetof(radarsetpara_args_t, times)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 2,
        .entry.int_range.max_val = 32,
        .offset = offsetof(radarsetpara_args_t, loop)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 3,
        .offset = offsetof(radarsetpara_args_t, ant)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10,
        .offset = offsetof(radarsetpara_args_t, wave)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 4,
        .offset = offsetof(radarsetpara_args_t, dbg_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 3000,
        .entry.int_range.max_val = 100000,
        .offset = offsetof(radarsetpara_args_t, period)
    },
};

const at_para_parse_syntax_t radaralgctrl_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(radaralgctrl_args_t, height)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(radaralgctrl_args_t, material)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(radaralgctrl_args_t, scenario)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(radaralgctrl_args_t, fusion_track)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(radaralgctrl_args_t, fusion_ai)
    },
};

const at_para_parse_syntax_t radaralgpara_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 99,
        .offset = offsetof(radaralgpara_args_t, d_th_1m)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 99,
        .offset = offsetof(radaralgpara_args_t, d_th_2m)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 99,
        .offset = offsetof(radaralgpara_args_t, p_th)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 99,
        .offset = offsetof(radaralgpara_args_t, t_th_1m)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 99,
        .offset = offsetof(radaralgpara_args_t, t_th_2m)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 100,
        .offset = offsetof(radaralgpara_args_t, b_th_ratio)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 99,
        .offset = offsetof(radaralgpara_args_t, b_th_cnt)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 100,
        .offset = offsetof(radaralgpara_args_t, a_th)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(radaralgpara_args_t, write_to_nv)
    },
};

const at_para_parse_syntax_t radarsetdlytime_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 43200,
        .offset = offsetof(radarsetdlytime_args_t, dly_time)
    },
};

const at_para_parse_syntax_t radarsetledgear_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(radarsetledgear_args_t, gear)
    },
};

// 以下为产测命令
#ifdef CONFIG_RADAR_MFG
const at_para_parse_syntax_t mfgradartestiso_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 40,
        .offset = offsetof(mfgradartestiso_args_t, target_iso)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 20,
        .offset = offsetof(mfgradartestiso_args_t, win)
    },
};

const at_para_parse_syntax_t mfgradartestnoise_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 40,
        .offset = offsetof(mfgradartestnoise_args_t, noise_thres)
    },
};
#endif

const at_cmd_entry_t at_radar_cmd_parse_table[] = {
    {
        "RADARSETST",
        10001,
        0,
        radarsetst_syntax,
        NULL,
        (at_set_func_t)at_radar_set_sts,
        NULL,
        NULL,
    },
    {
        "RADARSETPARA",
        10002,
        0,
        radarsetpara_syntax,
        NULL,
        (at_set_func_t)at_radar_set_para,
        NULL,
        NULL,
    },
    {
        "RADARALGCTRL",
        10003,
        0,
        radaralgctrl_syntax,
        NULL,
        (at_set_func_t)at_radar_alg_ctrl,
        NULL,
        NULL,
    },
    {
        "RADARGETST",
        10004,
        0,
        NULL,
        at_radar_get_sts,
        NULL,
        NULL,
        NULL,
    },
    {
        "RADARGETISO",
        10005,
        0,
        NULL,
        at_radar_get_isolation,
        NULL,
        NULL,
        NULL,
    },
    {
        "RADARSETDLY",
        10006,
        0,
        radarsetdlytime_syntax,
        NULL,
        (at_set_func_t)at_radar_set_dly_time,
        NULL,
        NULL,
    },
    {
        "RADARGETDLY",
        10007,
        0,
        NULL,
        at_radar_get_dly_time,
        NULL,
        NULL,
        NULL,
    },
    {
        "RADARSETLED",
        10008,
        0,
        radarsetledgear_syntax,
        NULL,
        (at_set_func_t)at_radar_set_led_gear,
        NULL,
        NULL,
    },
    // 以下为产测命令
#ifdef CONFIG_RADAR_MFG
    {
        "RADARTESTISO",
        10009,
        0,
        mfgradartestiso_syntax,
        NULL,
        (at_set_func_t)at_mfg_radar_test_iso,
        NULL,
        NULL,
    },
    {
        "RADARTESTNOISE",
        10010,
        0,
        mfgradartestnoise_syntax,
        NULL,
        (at_set_func_t)at_mfg_radar_test_noise,
        NULL,
        NULL,
    },
#endif
    {
        "RADARALGPARA",
        10011,
        0,
        radaralgpara_syntax,
        NULL,
        (at_set_func_t)at_radar_alg_para,
        NULL,
        NULL,
    },
};

#endif  /* AT_RADAR_CMD_AT_CMD_TALBE_H */
