/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 HAL tcxo \n
 *
 * History: \n
 * 2022-08-16， Create file. \n
 */

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "tcxo_porting.h"
#include "hal_tcxo.h"
#include "hal_tcxo_v150.h"

#define HAL_TCXO_COUNT_VALUE_B_VALID_TRUE 1

errcode_t hal_tcxo_init(void)
{
    if (hal_tcxo_v150_regs_init() != ERRCODE_SUCC) {
        return ERRCODE_TCXO_REG_ADDR_INVALID;
    }

    /* set the tcxo cnt enable */
    hal_tcxo_status_set_enable();
    /* clear the tcxo cnt */
    hal_tcxo_status_set_clear();
    return ERRCODE_SUCC;
}

errcode_t hal_tcxo_deinit(void)
{
    hal_tcxo_v150_regs_deinit();
    return ERRCODE_SUCC;
}

uint64_t hal_tcxo_get(void)
{
    bool f_cnt_val = false;
    /* refresh the data to tcxo cnt value */
    hal_tcxo_status_set_refresh();

    for (uint32_t i = 0; i < TCXO_LOCK_GET_ATTE; i++) {
        if (hal_tcxo_status_get_valid() == HAL_TCXO_COUNT_VALUE_B_VALID_TRUE) {
            f_cnt_val = true;
            break;  /* wait until the value in tcxo cnt value is available */
        }
    }

    if (f_cnt_val == true) {
        return hal_tcxo_reg_count_get();
    } else {
        return 0;
    }
}