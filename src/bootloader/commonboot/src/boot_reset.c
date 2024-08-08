/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: reset
 *
 * Create: 2023-01-09
 */

#include "boot_reset.h"
#include "boot_def.h"
#include "boot_init.h"
#include "non_os.h"

#ifdef SUPPORT_GPIO
#include "hal_gpio.h"
#endif
void reset(void)
{
    update_reset_count();
    uapi_reg_write16(BOOT_PORTING_RESET_REG, BOOT_PORTING_RESET_VALUE);
    for (;;) { }
}

void boot_fail(void)
{
    /* wait for it... */
    for (;;) { }
}