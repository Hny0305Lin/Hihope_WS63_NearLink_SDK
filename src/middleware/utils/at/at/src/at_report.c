/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT report source \n
 */

#include "at_product.h"
#include "at_channel.h"
#include "at_process.h"
#include "at_base.h"

void uapi_at_report(const char* str)
{
    if (str == NULL) {
        return;
    }

    uint16_t channel_id = at_proc_get_current_channel_id();
    at_write_func_t func = at_channel_get_write_func(channel_id);
    if (func != NULL) {
        func((char*)str);
    }

    at_log_normal(str, strlen(str), 0);
}

void uapi_at_report_to_single_channel(at_channel_id_t channel_id, const char* str)
{
    if (str == NULL) {
        return;
    }

    at_write_func_t func = at_channel_get_write_func(channel_id);
    if (func != NULL) {
        func((char*)str);
    }

    at_log_normal(str, strlen(str), 0);
}
