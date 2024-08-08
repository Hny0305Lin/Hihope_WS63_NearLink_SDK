/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides tcxo trim port \n
 *
 * History: \n
 * 2024-01-31， Create file. \n
 */

#ifndef XO_TRIM_PORTING_H
#define XO_TRIM_PORTING_H

#include <stdint.h>
#include "std_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void cmu_xo_trim_init(void);
#if defined(CONFIG_MIDDLEWARE_SUPPORT_NV)
void cmu_xo_trim_temp_comp_init(void);
void cmu_xo_trim_temp_comp_print(void);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
