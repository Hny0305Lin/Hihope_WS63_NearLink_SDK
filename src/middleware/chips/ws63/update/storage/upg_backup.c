/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: UPG backup image functions for different chip
 */

#include <stddef.h>
#include <stdint.h>
#include "errcode.h"
#include "securec.h"
#include "common_def.h"
#include "partition.h"
#include "main.h"

#include "upg_definitions.h"
#include "upg_porting.h"
#include "upg_alloc.h"
#include "upg_debug.h"
#include "upg_common_porting.h"

errcode_t upg_image_backups_update(void)
{
    return ERRCODE_SUCC;
}
