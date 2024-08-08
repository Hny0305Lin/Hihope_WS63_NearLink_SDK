/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides at notify api header. Only for AT module. \n
 */
 
#ifndef    AT_NOTIFY_H
#define    AT_NOTIFY_H
 
#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
#include "at_product.h"
 
void at_notify_process_list(void);
 
void at_notify_init(void);
#endif
#endif
