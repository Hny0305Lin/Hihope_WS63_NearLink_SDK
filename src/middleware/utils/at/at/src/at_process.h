/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides at process service api header. Only for AT module. \n
 */

#ifndef    AT_PROCESS_H
#define    AT_PROCESS_H

#include "at_product.h"

at_ret_t at_proc_cmd_handle(uint16_t channel_id);

uint16_t at_proc_get_current_channel_id(void);

#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
void at_proc_cmd_urc_handle(void);
#endif
 
#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
at_ret_t at_proc_interactivity_handle(uint16_t channel_id);
 
at_ret_t at_proc_timeout_handle(void);
 
at_ret_t at_proc_cmd_result_handle(uint16_t result);
#endif

#endif
