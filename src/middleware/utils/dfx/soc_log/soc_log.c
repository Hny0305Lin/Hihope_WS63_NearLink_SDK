/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: SOC lOG
 */

#include "soc_log.h"
#include "soc_log_strategy.h"
#include "dfx_adapt_layer.h"

#if (CONFIG_DFX_SUPPORT_SOC_LOG == DFX_YES)

#define MAX_PRINT_STR_SIZE 256

static dfx_write_data_interface_t g_write_impl = {0};

STATIC void soc_param_init(soc_log_param_t *param)
{
    param->level = SOC_LOG_DEFAULT_LEVEL;
    param->fn_name = NULL;
    param->line_num = SOC_LOG_INVALID_LINE;
    param->module_id = SOC_LOG_DEFAULT_MODULE_ID;
    param->type = SOC_LOG_TYPE_INVALID;
}

void soc_log_print(uint32_t level, uint32_t module_id, const char *fn_name, uint32_t line_num, const char *format, ...)
{
    va_list args;
    uint32_t lock_state;
    int format_len;
    uint8_t *buf  = NULL;
    soc_log_param_t param;
    dfx_write_data_interface_t impl;
    uint8_t *data[2];
    uint16_t len[2];

    soc_param_init(&param);
    param.type = SOC_LOG_TYPE_PRINT;
    param.level = (uint8_t)level;
    param.module_id = (uint16_t)module_id;
    param.fn_name = fn_name;
    param.line_num = line_num;

    buf = dfx_malloc(0, MAX_PRINT_STR_SIZE);
    if (buf == NULL) {
        goto end;
    }

    va_start(args, format);
    format_len = vsprintf_s((char*)buf, MAX_PRINT_STR_SIZE, format, args);
    if (format_len < 0) {
        va_end(args);
        goto end;
    }
    va_end(args);

    lock_state = dfx_int_lock();
    impl.write = g_write_impl.write;
    impl.fd = g_write_impl.fd;
    dfx_int_restore(lock_state);

    data[0] = (uint8_t*)&param;
    len[0] = (uint16_t)sizeof(soc_log_param_t);
    data[1] = (uint8_t*)buf;
    len[1] = (uint16_t)format_len;
    if (impl.write) {
        impl.write(impl.fd, DFX_DATA_TYPE_UAPI_LOG, data, len, 2); /* 2 是 data 和 len 的数组大小 */
    }

end:
    if (buf) {
        dfx_free(0, buf);
    }
}

void soc_log_simple_print(const char *format, ...)
{
    uint8_t *buf  = NULL;
    va_list args;
    int format_len;
    uint32_t lock_state;
    soc_log_param_t param = { 0 };
    dfx_write_data_interface_t impl;
    uint8_t *data[2];
    uint16_t len[2];

    soc_param_init(&param);
    param.type = SOC_LOG_TYPE_SIMPLE_PRINT;

    buf = dfx_malloc(0, MAX_PRINT_STR_SIZE);
    if (buf == NULL) {
        goto end;
    }

    va_start(args, format);
    format_len = vsprintf_s((char*)buf, MAX_PRINT_STR_SIZE, format, args);
    if (format_len < 0) {
        va_end(args);
        goto end;
    }
    va_end(args);

    lock_state = dfx_int_lock();
    impl.write = g_write_impl.write;
    impl.fd = g_write_impl.fd;
    dfx_int_restore(lock_state);

    data[0] = (uint8_t *)&param;
    len[0] = (uint16_t)sizeof(soc_log_param_t);
    data[1] = buf;
    len[1] = (uint16_t)format_len;
    if (impl.write) {
        impl.write(impl.fd, DFX_DATA_TYPE_UAPI_LOG, data, len, 2); /* 2 是 data 和 len 的数组大小 */
    }
end:
    if (buf) {
        dfx_free(0, buf);
    }
}

errcode_t soc_log_register_write_impl(dfx_write_data_interface_t *impl)
{
    uint32_t lock_state = dfx_int_lock();
    g_write_impl.write = impl->write;
    g_write_impl.fd = impl->fd;
    dfx_int_restore(lock_state);
    return ERRCODE_SUCC;
}

#endif /* CONFIG_DFX_SUPPORT_SOC_LOG */