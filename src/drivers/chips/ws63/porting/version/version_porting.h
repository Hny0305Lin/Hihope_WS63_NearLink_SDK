/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides version port header. \n
 *
 * History: \n
 * 2023-11-08， Create file. \n
 */

#ifndef VERSION_PORTING_H
#define VERSION_PORTING_H

#include <stdint.h>
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    uint32_t c : 8;
    uint32_t r : 8;
    uint32_t v : 16;
} plat_version_t;

typedef struct {
    uint32_t c : 8;
    uint32_t r : 8;
    uint32_t v : 8;
    uint32_t b : 8;
} plat_version_mod_t;

char *plat_get_sw_version_str(void);

void print_version(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

