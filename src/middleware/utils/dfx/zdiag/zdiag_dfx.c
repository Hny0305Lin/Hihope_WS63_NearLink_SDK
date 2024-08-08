/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: zdiag dfx
 * This file should be changed only infrequently and with great care.
 */
#include "diag_dfx.h"
#include "soc_module.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "errcode.h"
#if !defined __KERNEL__
#include "soc_diag_util.h"
#endif
#include "soc_diag_msg_id.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_TEST_DIAG_D

#undef THIS_MOD_ID
#define THIS_MOD_ID SOC_ID_BSP

typedef enum {
    DIAG_DFX_CMD_CASE_GET_STAT,
    DIAG_DFX_CMD_CASE_REPORT_MSG,
    DIAG_DFX_CMD_CASE_REPORT_FIX_MSG,
    DIAG_DFX_CMD_CASE_LAST_DUMP,
    DIAG_DFX_CMD_CASE_SYS_MSG,
    DIAG_DFX_CMD_CASE_FAULT_MOCKED,
} diag_dfx_cmd_case_id_t;

zdiag_dfx_stat_t g_zdiag_dfx_stat;

void dfx_last_dump_data(uint32_t dump_id, uintptr_t addr, uint32_t size);

zdiag_dfx_stat_t *uapi_zdiag_get_dfx_stat(void)
{
    return &g_zdiag_dfx_stat;
}

STATIC errcode_t diag_dfx_report_sys_msg(uint16_t cmd_id, const  void *cmd_param, uint16_t cmd_param_size,
    const diag_option_t *option)
{
    uint32_t data[] = {1, 2, 3, 4}; /* test val 1 2 3 4 */
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);

    uapi_diag_report_sys_msg(0, 0x1, (uint8_t*)data, sizeof(data), 1);  /* 0 0x1 1 test val */
    uapi_diag_report_sys_msg(1, 0xfeb40645, (uint8_t*)data, sizeof(data), 2); /* 1 0xfeb40645 2 test val */
    return ERRCODE_SUCC;
}


STATIC errcode_t diag_dfx_last_dump(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                    const diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);

    diag_dfx_cmd_req_st_t *req = cmd_param;

    dfx_last_dump_data(req->data[0], req->data[1], req->data[2]); /* 0 1 2 为data 下标 */
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_dfx_cmd_get_stat(uint16_t cmd_id, const void *cmd_param, uint16_t cmd_param_size,
                                       diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    return uapi_diag_report_packet(DIAG_CMD_ID_DIAG_DFX_START, option, (uint8_t *)&g_zdiag_dfx_stat,
        sizeof(zdiag_dfx_stat_t), true);
}

STATIC errcode_t diag_dfx_cmd_report_msg(uint16_t cmd_id, const void *cmd_param, uint16_t cmd_param_size,
    diag_option_t *option)
{
#if !defined __KERNEL__
    uapi_diag_info_log1(100, "test_1_log1", 1);          /* 100 1  */
    uapi_diag_warning_log1(101, "test_1_warning1", 2);   /* 101 2 test val */
    uapi_diag_error_log1(102, "test_1_error1", 3);       /* 102 3 test val */
    uapi_diag_info_log1(0x100, "test_2_log1", 4);        /* 0x100 4 test val */
    uapi_diag_warning_log1(0x101, "test_2_warning1", 5); /* 0x101 5 test val */
    uapi_diag_error_log1(0x102, "test_2_error1", 6);     /* 0x102 6 test val */
#endif
    unused(cmd_id);
    unused(cmd_param_size);
    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)cmd_param, sizeof(diag_dfx_cmd_req_st_t), false);
}

STATIC errcode_t diag_dfx_cmd_report_fix_msg(uint16_t cmd_id, const  void *cmd_param, uint16_t cmd_param_size,
                                             const diag_option_t *option)
{
    uint32_t data[] = {1, 2, 3, 4}; /* test val 1 2 3 4 */
    uapi_diag_report_sys_msg(THIS_MOD_ID, SOC_DIAG_MSG_ID_DIAG_TEST_U8_ARRAY, (uint8_t *)data, sizeof(data), 1);
    uapi_diag_report_sys_msg(THIS_MOD_ID, SOC_DIAG_MSG_ID_DIAG_TEST_U32_ARRAY, (uint8_t *)data, sizeof(data), 1);

    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);
    return ERRCODE_SUCC;
}

#ifndef NDEBUG
STATIC errcode_t diag_dfx_cmd_fault_mocked(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                           const diag_option_t *option)
{
    diag_dfx_cmd_req_st_t *req = cmd_param;

    if (req->data[0] == 0) {
        if (req->data[1] == 0) {
            uint32_t *null_pointer = NULL;
            *null_pointer = req->case_id;
        } else {
            while (true) {}
        }
    }
    unused(cmd_id);
    unused(cmd_param_size);
    unused(option);
    return ERRCODE_SUCC;
}
#endif

errcode_t diag_dfx_cmd(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    diag_dfx_cmd_req_st_t *req = cmd_param;
    unused(cmd_param_size);
    switch (req->case_id) {
        case DIAG_DFX_CMD_CASE_GET_STAT:
            return diag_dfx_cmd_get_stat(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_REPORT_MSG:
            return diag_dfx_cmd_report_msg(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_REPORT_FIX_MSG:
            return diag_dfx_cmd_report_fix_msg(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_LAST_DUMP:
            return diag_dfx_last_dump(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_SYS_MSG:
            return diag_dfx_report_sys_msg(cmd_id, cmd_param, cmd_param_size, option);
#ifndef NDEBUG
        case DIAG_DFX_CMD_CASE_FAULT_MOCKED:
            return diag_dfx_cmd_fault_mocked(cmd_id, cmd_param, cmd_param_size, option);
#endif
        default:
            return uapi_diag_report_packet(cmd_id, option, (uint8_t *)req, sizeof(diag_dfx_cmd_req_st_t), false);
    }
    return ERRCODE_SUCC;
}
