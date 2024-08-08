/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: RISCV31 idle task config interface for LiteOS
 *
 * Create:  2021-10-20
 */
#ifndef IDLE_CONFIG_H
#define IDLE_CONFIG_H

/**
 * @brief  los idle task config
 */
void idle_task_config(void);
typedef void (*watchdog_port_idle_kick_callback)(void);
void watchdog_port_idle_kick_register(watchdog_port_idle_kick_callback callback);
#endif