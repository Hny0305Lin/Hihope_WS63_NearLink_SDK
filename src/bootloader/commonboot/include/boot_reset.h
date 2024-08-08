/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_RESET_H
#define BOOT_RESET_H

void reset(void);
void boot_fail(void);
void update_reset_count(void);

#endif