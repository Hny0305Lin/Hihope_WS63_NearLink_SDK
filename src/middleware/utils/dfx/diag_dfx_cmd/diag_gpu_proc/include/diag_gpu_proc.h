/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: gpu proc by diag
 */

#ifndef __DIAG_GPU_PROC_H__
#define __DIAG_GPU_PROC_H__

#include "errcode.h"

typedef struct {
    uint32_t proc_en;
    uint32_t lut_en;
} gpu_gmmu_proc_info_t;

errcode_t diag_gpu_proc_init(void);

#endif /* __DIAG_GPU_PROC_H__ */
