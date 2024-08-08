/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: driver initialize processes.
 *
 * Create: 2023-06-01
*/

#ifndef CRYPTO_DRV_INIT_H
#define CRYPTO_DRV_INIT_H

#include "crypto_type.h"

td_s32 crypto_drv_mod_init(td_void);
td_void crypto_drv_mod_deinit(td_void);
td_s32 crypto_drv_mod_resume(td_void);
td_void crypto_drv_mod_suspend(td_void);

#endif