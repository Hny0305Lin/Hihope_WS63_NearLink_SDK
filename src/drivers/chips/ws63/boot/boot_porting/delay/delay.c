/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: delay
 *
 * Create: 2023-03-09
 */

#include "tcxo.h"

void udelay(uint32_t us)
{
    (void)uapi_tcxo_delay_us(us);
}

void mdelay(uint32_t ms)
{
    (void)uapi_tcxo_delay_ms(ms);
}
