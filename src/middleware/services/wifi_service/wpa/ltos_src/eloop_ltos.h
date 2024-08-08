/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: eloop API
 */


#ifndef ELOOP_LTOS_H
#define ELOOP_LTOS_H

#include "eloop.h"

#define ELOOP_ALREADY_RUNNING          1
#define ELOOP_NOT_RUNNING              0
#define ELOOP_MAX_EVENTS               16

int eloop_terminate_wpa_process(void);
void eloop_run_process_normal_event(unsigned int *ret_flags, struct eloop_data *eloop);
int eloop_start_running(eloop_task_type eloop_task);
int eloop_run_check(eloop_task_type eloop_task);
int global_eloop_is_running(void);
#endif /* ELOOP_LTOS_H */
