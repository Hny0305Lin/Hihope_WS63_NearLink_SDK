/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: tick timer for liteos
 *
 */
#ifndef __TICK_TIMER_H__
#define __TICK_TIMER_H__
typedef void (*tick_handle_hook)(void);
void hal_tick_handle_register(tick_handle_hook hook);
#endif