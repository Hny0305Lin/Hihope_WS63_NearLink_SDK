/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: only use for init befor drv layer test.
 *
 * Create: 2023-02-08
*/

#ifndef CRYPTO_DRV_INIT_H
#define CRYPTO_DRV_INIT_H

#include "crypto_type.h"

td_s32 crypto_drv_mod_init(td_void);
td_void crypto_drv_mod_deinit(td_void);

#endif