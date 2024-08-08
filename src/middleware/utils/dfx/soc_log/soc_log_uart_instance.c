/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: SOC lOG implement using uart output
 */

#include "soc_log_uart_instance.h"
#include "soc_log_impl_str.h"
#include "dfx_adapt_layer.h"

#if (CONFIG_DFX_SUPPORT_SOC_LOG == DFX_YES)

#define MAX_PRINT_SIZE 128

static dfx_soc_log_output_handler g_dfx_soc_log_output_handler = NULL;

STATIC char *level_2_string(uint8_t level)
{
    switch (level) {
        case SOC_LOG_LEVEL_ALERT:
            return "ALERT";
        case SOC_LOG_LEVEL_FATAL:
            return "FATAL";
        case SOC_LOG_LEVEL_ERROR:
            return "ERROR";
        case SOC_LOG_LEVEL_WARNING:
            return "WARNING";
        case SOC_LOG_LEVEL_NOTICE:
            return "NOTICE";
        case SOC_LOG_LEVEL_INFO:
            return "INFO";
        case SOC_LOG_LEVEL_DBG:
            return "DBG";
        case SOC_LOG_LEVEL_TRACE:
            return "TRACE";
        default:
            return "UNKNOWN";
    }
}

STATIC void log_outputs(uint8_t *data, uint32_t len)
{
    if (g_dfx_soc_log_output_handler) {
        g_dfx_soc_log_output_handler(data, len);
    }
}

/* dfx_write_data_serial.c */
STATIC int32_t dfx_serial_write_str(uint8_t *data[], uint16_t len[], uint8_t cnt)
{
    int32_t total_len = 0;
    uint32_t lock_stat = dfx_int_lock();
    for (int i = 0; i < cnt; i++) {
        log_outputs(data[i], len[i]);
        total_len += len[i];
    }
    dfx_int_restore(lock_stat);
    return total_len;
}

STATIC void dfx_serial_write_uapi_log_head(soc_log_param_t *param)
{
    int32_t len;
    uint32_t lock_stat;
    char *level_string = NULL;
    uint8_t *buff_out = NULL;
    char *machine_name = dfx_machine_get_name();
    level_string = level_2_string(param->level);
    if (param->type == SOC_LOG_TYPE_KEY_TRACE) {
        level_string = "KEY_TRACE";
    }

    buff_out = dfx_malloc(0, MAX_PRINT_SIZE);
    if (buff_out == NULL) {
        return;
    }

    if (param->fn_name) {
        len = snprintf_s((char*)buff_out, MAX_PRINT_SIZE, MAX_PRINT_SIZE - 1, "[%s]level=%s module=%d %s %d ",
                         machine_name, level_string, (int32_t)param->module_id,
                         param->fn_name, (int32_t)param->line_num);
    } else {
        len = snprintf_s((char*)buff_out, MAX_PRINT_SIZE, MAX_PRINT_SIZE - 1, "[%s]level=%s module=%d ",
                         machine_name, level_string, (int32_t)param->module_id);
    }

    if (len < 0) {
        dfx_free(0, buff_out);
        return;
    }
    lock_stat = dfx_int_lock();
    log_outputs(buff_out, (uint32_t)len);
    dfx_int_restore(lock_stat);

    dfx_free(0, buff_out);
}

STATIC int32_t dfx_serial_write_uapi_log(uint8_t *data[], uint16_t len[], uint8_t cnt)
{
    /* 输出模块ID, 输出打印等级, 输出function_name */
    int32_t total_len = len[0];
    soc_log_param_t *param = (soc_log_param_t *)data[0];

    if (total_len != 0 && param != NULL) {
        if (param->type != SOC_LOG_TYPE_PRINT && param->type != SOC_LOG_TYPE_SIMPLE_PRINT &&
            param->type != SOC_LOG_TYPE_KEY_TRACE) {
            return 0;
        }

        if (param->type != SOC_LOG_TYPE_SIMPLE_PRINT) {
            dfx_serial_write_uapi_log_head(param);
        }
    }

    total_len += dfx_serial_write_str(&data[1], &len[1], cnt - 1);
    return total_len;
}

STATIC int32_t dfx_serial_write_data(void *fd, dfx_data_type_t data_type, uint8_t *data[], uint16_t len[], uint8_t cnt)
{
    unused(fd);

    switch (data_type) {
        case DFX_DATA_TYPE_STRING:
            return dfx_serial_write_str(data, len, cnt);
        case DFX_DATA_TYPE_UAPI_LOG:
            return dfx_serial_write_uapi_log(data, len, cnt);
        default:
            return ERRCODE_FAIL;
    }
}

errcode_t dfx_serial_get_write_data_impl(dfx_write_data_interface_t *impl)
{
    impl->write = dfx_serial_write_data;
    impl->fd = 0;
    return ERRCODE_SUCC;
}

void dfx_register_soc_log_output_handler(dfx_soc_log_output_handler handler)
{
    g_dfx_soc_log_output_handler = handler;
}

#endif /* CONFIG_DFX_SUPPORT_SOC_LOG */