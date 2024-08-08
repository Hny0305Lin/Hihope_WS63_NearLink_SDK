 /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: HiLink register to get function ACkeyV2 \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "hilink_call.h"
#include "hilink_device.h"

void HILINK_RegisterGetAcV2Func(HILINK_GetAcKeyFunc func)
{
    hilink_call1(HILINK_CALL_HILINK_REGISTER_GET_AC_V2_FUNC, void, HILINK_GetAcKeyFunc, func);
}