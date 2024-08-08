/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides tcxo port \n
 *
 * History: \n
 * 2023-09-14， Create file. \n
 */

#ifndef SOC_PORTING_H
#define SOC_PORTING_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CLK24M_TCXO 1
#define CLK40M_TCXO 0
uint32_t get_tcxo_freq(void);
void boot_clock_adapt(void);
void switch_flash_clock_to_pll(void);
void config_sfc_ctrl_ds(void);
bool is_tcxo_24mhz(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
