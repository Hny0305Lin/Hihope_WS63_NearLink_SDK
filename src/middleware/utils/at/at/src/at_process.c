/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT process source \n
 */

#include "at_base.h"
#include "at_channel.h"
#include "at_cmd.h"
#include "at_parse.h"
#include "at_msg.h"
#include "at_notify.h"
#include "at_process.h"

typedef struct {
#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
    bool in_progress;
    bool block_urc;
    bool allowed_abort;
#endif
    uint16_t channel_id;
    const at_cmd_entry_t *entry;
    at_format_t format;
    at_cmd_type_t type;
    void *args;
} at_control_block_t;

at_control_block_t g_at_control_block = {0};

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
static at_abort_func_t g_at_default_abort_func = NULL;
static at_abort_func_t g_at_abort_func = NULL;
static void *g_at_abort_func_arg = NULL;
static at_interactivity_func_t g_at_interactivity_func = NULL;
#endif

uint16_t at_proc_get_current_channel_id(void)
{
    return g_at_control_block.channel_id;
}

static at_ret_t at_proc_perform_command_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->cmd) {
        ret = g_at_control_block.entry->cmd();
    } else {
        return AT_RET_PROC_CMD_FUNC_MISSING;
    }
    return ret;
}

static at_ret_t at_proc_perform_read_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->read) {
        ret = g_at_control_block.entry->read();
    } else {
        return AT_RET_PROC_READ_FUNC_MISSING;
    }
    return ret;
}

static at_ret_t at_proc_perform_test_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->test) {
        ret = g_at_control_block.entry->test();
    } else {
        return AT_RET_PROC_TEST_FUNC_MISSING;
    }
    return ret;
}

static at_ret_t at_proc_perform_set_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->set) {
        ret = g_at_control_block.entry->set(g_at_control_block.args);
    } else {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    return ret;
}

#ifdef CONFIG_AT_SUPPORT_QUERY
static at_ret_t at_proc_perform_query_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->query) {
        ret = g_at_control_block.entry->query(g_at_control_block.args);
    } else {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    return ret;
}
#endif

static at_ret_t at_proc_perform_current_cmd(void)
{
    at_ret_t ret;

    switch (g_at_control_block.type) {
        case AT_CMD_TYPE_CMD:
            ret = at_proc_perform_command_cmd();
            break;
        case AT_CMD_TYPE_READ:
            ret = at_proc_perform_read_cmd();
            break;
        case AT_CMD_TYPE_TEST:
            ret = at_proc_perform_test_cmd();
            break;
        case AT_CMD_TYPE_SET:
            ret = at_proc_perform_set_cmd();
            break;
#ifdef CONFIG_AT_SUPPORT_QUERY
        case AT_CMD_TYPE_QUERY:
            ret = at_proc_perform_query_cmd();
            break;
#endif
        default:
            return AT_RET_CMD_TYPE_ERROR;
    }

    return ret;
}

static void at_proc_para_arguments_finish(void)
{
    if (g_at_control_block.args == NULL) {
        return;
    }

    at_parse_free_arguments(g_at_control_block.args, g_at_control_block.entry->syntax);
    at_free(g_at_control_block.args);
    g_at_control_block.args = NULL;
}

static at_ret_t at_proc_para_arguments_prepare(const char *str)
{
    at_ret_t ret;
    uint32_t struct_max_size;

#ifdef CONFIG_AT_SUPPORT_QUERY
    if (((g_at_control_block.type != AT_CMD_TYPE_SET) && (g_at_control_block.type != AT_CMD_TYPE_QUERY))
        || (g_at_control_block.entry->syntax == NULL)) {
#else
    if ((g_at_control_block.type != AT_CMD_TYPE_SET) || (g_at_control_block.entry->syntax == NULL)) {
#endif
        return AT_RET_OK;
    }

    struct_max_size = at_cmd_get_max_struct_size();
    g_at_control_block.args = at_malloc(struct_max_size);
    if (g_at_control_block.args == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    memset_s(g_at_control_block.args, struct_max_size, 0, struct_max_size);

    ret = at_parse_para_arguments(str, g_at_control_block.args,
                                  g_at_control_block.entry->syntax);
    if (ret != AT_RET_OK) {
        at_proc_para_arguments_finish();
        return ret;
    }
    return AT_RET_OK;
}

static at_ret_t at_proc_exec_cmd(const at_cmd_info_t *cmd_info)
{
    at_ret_t ret;
    uint16_t str_offset = 0;

    g_at_control_block.channel_id = cmd_info->channel_id;

    g_at_control_block.format = at_parse_format_of_cmd(cmd_info, &str_offset);
    if (g_at_control_block.format == AT_FORMAT_ERROR) {
        return AT_RET_CMD_FORMAT_ERROR;
    }

    g_at_control_block.entry = at_cmd_find_entry(cmd_info->cmd_str, &str_offset);
    if (g_at_control_block.entry == NULL) {
        return AT_RET_CMD_NO_MATCH;
    }

#ifdef CONFIG_AT_SUPPORT_CMD_ATTR
    if (g_at_control_block.entry->attribute != 0) {
        if (at_cmd_attr(g_at_control_block.entry->attribute) == false) {
            return AT_RET_CMD_ATTR_NOT_ALLOW;
        }
    }
#endif

    g_at_control_block.type = at_parse_cmd_type(cmd_info->cmd_str, &str_offset);

    /* The AT command parameters are transferred by filling the corresponding fields in the parameter structure. */
    ret = at_proc_para_arguments_prepare(cmd_info->cmd_str + str_offset);
    if (ret != AT_RET_OK) {
        return ret;
    }

    ret = at_proc_perform_current_cmd();
    at_proc_para_arguments_finish();
    return ret;
}

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
static void at_timer_callback(void *arg)
{
    unused(arg);
    at_msg_block_t msg;
    msg.type = AT_CMD_TIMEOUT_MSG;
    at_msg_send(&msg);
}

static void at_proc_progress_block(uint16_t attribute)
{
    g_at_control_block.in_progress = true;
    if ((attribute & AT_FLAG_ABORTABLE) != 0) {
        g_at_control_block.allowed_abort = false;
        at_timer_start(AT_MAX_TIME_OUT, at_timer_callback, NULL);
    }
 
    if ((attribute & AT_FLAG_NOT_BLOCK_URC) == 0) {
        g_at_control_block.block_urc = true;
    }
}

errcode_t uapi_at_cmd_default_abort_register(at_abort_func_t func)
{
    if (func == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_at_default_abort_func = func;
    return ERRCODE_SUCC;
}

errcode_t uapi_at_cmd_abort_register(at_abort_func_t func, void *arg)
{
    if (func == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_at_abort_func = func;
    g_at_abort_func_arg = arg;
    return ERRCODE_SUCC;
}

static at_ret_t at_abort_current_cmd(void)
{
    if ((g_at_default_abort_func == NULL) && (g_at_abort_func == NULL)) {
        return AT_RET_CMD_IN_PROGRESS_BLOCK;
    }
    
    at_ret_t ret;
    if (g_at_abort_func != NULL) {
        ret = g_at_abort_func(g_at_abort_func_arg);
    } else {
        ret = g_at_default_abort_func(NULL);
    }
 
    if (ret != AT_RET_OK) {
        return ret;
    }
 
    g_at_control_block.in_progress = false;
    g_at_control_block.allowed_abort = false;
    g_at_control_block.block_urc = false;
    return AT_RET_PROC_ABORT_CURRENT_COMMAND;
}

static at_ret_t at_abort(uint16_t channel_id)
{
    if (g_at_control_block.in_progress == false) {
        return AT_RET_OK;
    }
 
    if (g_at_control_block.allowed_abort == false) {
        uapi_at_report_to_single_channel(channel_id, AT_RESPONSE_BUSY);
        return AT_RET_CMD_IN_PROGRESS_BLOCK;
    }
 
    uapi_at_report_to_single_channel(channel_id, AT_RESPONSE_ABORTING);
    return at_abort_current_cmd();
}

errcode_t uapi_at_interactivity_func_register(at_interactivity_func_t func)
{
    if (func == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_at_interactivity_func = func;
    return ERRCODE_SUCC;
}

static void at_proc_wait_interactivity(void)
{
    uint16_t id = at_proc_get_current_channel_id();
    g_at_control_block.in_progress = true;
    g_at_control_block.block_urc = true;
    at_channel_data_reset(id);
    at_channel_data_wait_interactivity(id);
}

static void at_proc_release_interactivity(void)
{
    uint16_t id = at_proc_get_current_channel_id();
    g_at_control_block.in_progress = false;
    g_at_control_block.block_urc = false;
    g_at_interactivity_func = NULL;
    at_channel_data_reset(id);
}
#endif

static bool at_proc_need_exit(at_ret_t result)
{
    switch (result) {
        case AT_RET_OK:
            uapi_at_report(AT_RESPONSE_OK);
            break;
#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
        case AT_RET_CMD_IN_PROGRESS_BLOCK:
            at_proc_progress_block(g_at_control_block.entry->attribute);
            return true;
        case AT_RET_PROC_WAIT_INTERACTIVITY:
            at_proc_wait_interactivity();
            return true;
#endif
        default:
            uapi_at_report(AT_RESPONSE_ERROR);
            break;
    }
    return false;
}

static void at_proc_cmd_process(void)
{
    at_ret_t ret = AT_RET_OK;
    at_cmd_info_t *cmd_info;

    cmd_info = at_parse_get_next_remain_cmd();
    while (cmd_info != NULL) {
        ret = at_proc_exec_cmd(cmd_info);
        at_parse_del_one_remain_cmd(cmd_info);
        if (at_proc_need_exit(ret) == true) {
            break;
        }
        cmd_info = at_parse_get_next_remain_cmd();
    }
}

at_ret_t at_proc_cmd_handle(uint16_t channel_id)
{
    at_ret_t ret = AT_RET_OK;
    char *str = NULL;

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
    ret = at_abort(channel_id);
    if (ret != AT_RET_OK) {
        at_channel_data_reset(channel_id);
        return ret;
    }
#endif

    str = (char*)at_channel_get_data(channel_id);
    if (str == NULL) {
        at_channel_data_reset(channel_id);
        return AT_RET_CHANNEL_DATA_NULL;
    }
    at_log_normal(str, strlen(str), 0);

    ret = at_parse_split_combine_cmd(str, (uint32_t)strlen(str), channel_id);
    at_channel_data_reset(channel_id);
    if (ret != AT_RET_OK) {
        return ret;
    }

    at_proc_cmd_process();
    return ret;
}

#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
void at_proc_cmd_urc_handle(void)
{
#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
    if (g_at_control_block.block_urc == true) {
        return;
    }
#endif
    at_notify_process_list();
}
#endif

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
static at_ret_t at_proc_interactivity_process(uint16_t channel_id, char *str, uint32_t len)
{
    if (g_at_interactivity_func == NULL) {
        at_proc_release_interactivity();
        return AT_RET_OK;
    }
 
    at_ret_t ret = g_at_interactivity_func(str, len);
    if (ret == AT_RET_PROC_WAIT_INTERACTIVITY) {
        at_proc_wait_interactivity();
        return ret;
    }
 
    if (ret != AT_RET_OK) {
        uapi_at_report_to_single_channel(channel_id, AT_RESPONSE_ERROR);
    }
    at_proc_release_interactivity();
    return ret;
}

at_ret_t at_proc_interactivity_handle(uint16_t channel_id)
{
    at_ret_t ret = AT_RET_OK;
    char *str = NULL;
 
    str = (char*)at_channel_get_data(channel_id);
    if (str == NULL) {
        at_proc_release_interactivity();
        return AT_RET_CHANNEL_DATA_NULL;
    }
 
    ret = at_proc_interactivity_process(channel_id, str, strlen(str));
    return ret;
}

static void at_proc_at_remain_task_process(void)
{
    at_proc_cmd_process();
#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
    at_proc_cmd_urc_handle();
#endif
}

at_ret_t at_proc_cmd_result_handle(uint16_t result)
{
    if (g_at_control_block.entry == NULL) {
        return AT_RET_OK;
    }
    at_timer_delete();
    g_at_control_block.in_progress = false;
    g_at_control_block.block_urc = false;
 
    if (result == 0) {
        uapi_at_report(AT_RESPONSE_OK);
    } else {
        uapi_at_report(AT_RESPONSE_ERROR);
    }
 
    at_proc_at_remain_task_process();
    return AT_RET_OK;
}

at_ret_t at_proc_timeout_handle(void)
{
    at_timer_delete();
    /* This means that even though the AT command execution times out,
       it will not be interrupted when there are no other AT commands. */
    if (at_parse_has_remain_cmd() != true) {
        g_at_control_block.allowed_abort = true;
        return AT_RET_ABORT_DELAY;
    }
 
    at_ret_t ret = at_abort_current_cmd();
    if (ret != AT_RET_PROC_ABORT_CURRENT_COMMAND) {
        return ret;
    }
 
    at_proc_at_remain_task_process();
    return ret;
}
#endif
