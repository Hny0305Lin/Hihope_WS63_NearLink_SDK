/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag get cpup
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_get_cpup.h"
#include "dfx_adapt_layer.h"
#include "soc_diag_cmd_id.h"
#include "dfx_cpup.h"
#include "dfx_task.h"
#include "debug_print.h"
#include "errcode.h"

#define DFX_CPUP_MAX_OUT_LEN    512
#define DFX_MAX_CPUP_MODE       3
#define DFX_CPUP_LAST_MULIT     0
#define DFX_CPUP_LAST_ONE       1
#define DFX_CPUP_ALL_TIME       0xffff

#define DFX_CPUP_PRECISION_MULT 10

typedef struct {
    dfx_cpup_item_usage_info_t *cpup_info[DFX_MAX_CPUP_MODE];
    task_info_t                *task_info;
    uint32_t                    display_mode;
} dfx_cpup_output_info_t;

static void diag_cpup_output_title(diag_option_t *option, uint32_t display_mode, uint8_t *out_buffer,
    uint32_t buffer_len)
{
    int32_t str_len = 0;
    if (display_mode == 0 || out_buffer == NULL || buffer_len == 0) {
        return;
    }

    (void)memset_s(out_buffer, buffer_len, 0, buffer_len);

    str_len = sprintf_s((char *)out_buffer, buffer_len, "\r\nName                   TID    ");
    if (buffer_len > (uint32_t)str_len) {
        str_len += sprintf_s((char *)&out_buffer[str_len], buffer_len - (uint32_t)str_len, "Priority   Status       "
            "StackSize    StackPoint             TopOfStack             BottomOfStack");
    }

    if (buffer_len > (uint32_t)str_len) {
        str_len += sprintf_s((char *)&out_buffer[str_len], buffer_len - (uint32_t)str_len,
            "  CPUP  CPUP 10s  CPUP 1s  ");
    }

    if (buffer_len > (uint32_t)str_len) {
        str_len += sprintf_s((char *)&out_buffer[str_len], buffer_len - (uint32_t)str_len,
            "\r\n----                   ---    ");
    }

    if (buffer_len > (uint32_t)str_len) {
        str_len += sprintf_s((char *)&out_buffer[str_len], buffer_len - (uint32_t)str_len, "--------   --------     "
            "---------    ----------             ----------             -------------");
    }

    if (buffer_len > (uint32_t)str_len) {
        str_len += sprintf_s((char *)&out_buffer[str_len], buffer_len - (uint32_t)str_len,
            "  ----  ----------  ----------  \n");
    }

    (void)uapi_diag_report_packet(DIAG_CMD_MOCKED_SHELL_IND, option, (const uint8_t *)out_buffer, (uint16_t)str_len,
        true);
}

static errcode_t diag_cpup_output(uint16_t cmd_id, diag_option_t *option, dfx_cpup_output_info_t *info,
    uint8_t *out_buffer, uint32_t buffer_len)
{
    errcode_t ret = ERRCODE_SUCC;

    if (info->display_mode == 0) {
        dfx_cpup_item_ind_t cpup_str;

        for (uint32_t i = 0; i < DFX_MAX_CPUP_MODE; i++) {
            memset_s(&cpup_str, sizeof(dfx_cpup_item_ind_t), 0, sizeof(dfx_cpup_item_ind_t));

            cpup_str.id = info->task_info->id;
            cpup_str.usage = info->cpup_info[i]->usage;
            if (memcpy_s(cpup_str.name, DFX_TASK_NAME_LEN, info->task_info->name, DFX_TASK_NAME_LEN - 1) != EOK) {
                continue;
            }

            ret = uapi_diag_report_packet(cmd_id, option, (const uint8_t *)(&cpup_str), sizeof(cpup_str), true);
        }
    } else {
        if (out_buffer == NULL || buffer_len == 0) {
            return ERRCODE_FAIL;
        }
        (void)memset_s(out_buffer, buffer_len, 0, buffer_len);

        int32_t str_len = 0;
        str_len = sprintf_s((char *)out_buffer, buffer_len, "%-23s0x%-5x", info->task_info->name,
            info->task_info->id);
        if (buffer_len > (uint32_t)str_len) {
            str_len += sprintf_s((char *)&out_buffer[str_len], buffer_len - (uint32_t)str_len,
                "%-11u%-13s0x%-11x0x%-18lx   0x%-18lx   0x%-11x",
                info->task_info->priority, dfx_os_task_status_convert_str(info->task_info->status),
                info->task_info->stack_size, info->task_info->sp,
                info->task_info->top_of_stack, info->task_info->bottom_of_stack);
        }

        if (buffer_len > (uint32_t)str_len) {
            str_len += sprintf_s((char *)&out_buffer[str_len], buffer_len - (uint32_t)str_len,
                " %4u.%1u%7u.%1u%9u.%1u   \n",
                info->cpup_info[0]->usage / DFX_CPUP_PRECISION_MULT,  /* the cpup_info[0] is all cpup */
                info->cpup_info[0]->usage % DFX_CPUP_PRECISION_MULT,  /* the cpup_info[0] is all cpup */
                info->cpup_info[1]->usage / DFX_CPUP_PRECISION_MULT,  /* the cpup_info[1] is last multi cpup */
                info->cpup_info[1]->usage % DFX_CPUP_PRECISION_MULT,  /* the cpup_info[1] is last multi cpup */
                info->cpup_info[2]->usage / DFX_CPUP_PRECISION_MULT,  /* the cpup_info[2] is last one second cpup */
                info->cpup_info[2]->usage % DFX_CPUP_PRECISION_MULT); /* the cpup_info[2] is last one second cpup */
        }
        ret = uapi_diag_report_packet(DIAG_CMD_MOCKED_SHELL_IND, option, (const uint8_t *)out_buffer,
            (uint16_t)str_len, true);
    }
    return ret;
}

static void diag_cpup_fail_report(uint16_t cmd_id, diag_option_t *option)
{
    dfx_cpup_item_ind_t cpup_str = {"Fail, Please Cheack Cpup Status", 0, 0};
    uapi_diag_report_packet(cmd_id, option, (const uint8_t *)(&cpup_str), sizeof(cpup_str), true);
}

errcode_t diag_cmd_get_cpup(uint16_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param_size);

    uint32_t ret = ERRCODE_SUCC;
    uint32_t cpup_mode[DFX_MAX_CPUP_MODE] = {DFX_CPUP_ALL_TIME, DFX_CPUP_LAST_MULIT, DFX_CPUP_LAST_ONE};
    uint32_t cpup_info_len = (uint32_t)sizeof(dfx_cpup_item_usage_info_t) * CPUP_DIAG_REPORT_CNT * DFX_MAX_CPUP_MODE;
    diag_cpup_cmd_t *cmd = (diag_cpup_cmd_t *)cmd_param;
    uint8_t *out_buffer = NULL;

    dfx_cpup_item_usage_info_t *cpup_info = (dfx_cpup_item_usage_info_t *)dfx_malloc(0, cpup_info_len);
    if (cpup_info == NULL) {
        return ERRCODE_MALLOC;
    }
    memset_s(cpup_info, cpup_info_len, 0, cpup_info_len);

    if (cmd->clear_flag == 1) {
        dfx_cpup_reset();
    }

    if (cmd->display_mode == 1) {
        out_buffer = (uint8_t *)dfx_malloc(0, DFX_CPUP_MAX_OUT_LEN);
    }

    for (uint32_t j = 0; j < DFX_MAX_CPUP_MODE; j++) {
        ret += dfx_cpup_get_all_usage(CPUP_DIAG_REPORT_CNT, cpup_info + (j * CPUP_DIAG_REPORT_CNT), cpup_mode[j], true);
    }
    if (ret != ERRCODE_SUCC) {
        diag_cpup_fail_report(cmd_id, option);
        goto err;
    }

    diag_cpup_output_title(option, cmd->display_mode, out_buffer, DFX_CPUP_MAX_OUT_LEN);

    for (uint32_t i = 0; i < CPUP_DIAG_REPORT_CNT; i++) {
        task_info_t task_info = {0};
        dfx_cpup_output_info_t out_info;

        if (cpup_info[i].status == 0) {
            continue;
        }

        ret = dfx_os_get_task_info(i, &task_info);
        if (ret != ERRCODE_SUCC) {
            continue;
        }

        for (uint32_t j = 0; j < DFX_MAX_CPUP_MODE; j++) {
            out_info.cpup_info[j] = cpup_info + (j * CPUP_DIAG_REPORT_CNT) + i;
        }
        out_info.task_info = &task_info;
        out_info.display_mode = cmd->display_mode;
        ret = diag_cpup_output(cmd_id, option, &out_info, out_buffer, DFX_CPUP_MAX_OUT_LEN);
    }

err:
    dfx_free(0, cpup_info);
    if (out_buffer != NULL) {
        dfx_free(0, out_buffer);
    }
    return ret;
}

errcode_t diag_cmd_set_cpup_enable(uint16_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size,
    diag_option_t *option)
{
    unused(cmd_param_size);

    uint32_t cpup_status = *(uint32_t *)cmd_param;

    if (cpup_status == 0) {
        dfx_cpup_stop();
        return uapi_diag_report_packet(cmd_id, option, (uint8_t *)("Cpup Stop!\r\n"),
            strlen("Cpup Stop!\r\n") + 1, true);
    } else if (cpup_status == 1) {
        dfx_cpup_start();
        return uapi_diag_report_packet(cmd_id, option, (uint8_t *)("Cpup Start!\r\n"),
            strlen("Cpup Start!\r\n") + 1, true);
    } else {
        return uapi_diag_report_packet(cmd_id, option, (uint8_t *)("Invalid Para!\r\n"),
            strlen("Invalid Para!\r\n") + 1, true);
    }
}