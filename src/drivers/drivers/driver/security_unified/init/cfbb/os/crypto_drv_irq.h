/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: drivers of irq.
 *
 * Create: 2023-06-16
*/

#ifndef CRYPTO_IRQ_H
#define CRYPTO_IRQ_H

#include "crypto_type.h"

td_s32 crypto_irq_init(td_void);

td_void crypto_irq_deinit(td_void);

#endif