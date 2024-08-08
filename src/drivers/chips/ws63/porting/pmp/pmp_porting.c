/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides pmp porting template \n
 *
 * History: \n
 * 2022-09-26， Create file. \n
 */

#include "hal_pmp_riscv31.h"
#include "pmp_porting.h"

void pmp_port_register_hal_funcs(void)
{
    hal_pmp_register_funcs(hal_pmp_riscv31_funcs_get());
}

void pmp_port_unregister_hal_funcs(void)
{
    hal_pmp_unregister_funcs();
}