  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: hilink bt function \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */
#include "hilink_call.h"
#include "hilink_bt_function.h"

int HILINK_BT_SetSdkEventCallback(HILINK_BT_SdkEventCallBack callback)
{
    return hilink_call1(HILINK_CALL_HILINK_BT_SET_SDK_EVENT_CALLBACK, int,
        HILINK_BT_SdkEventCallBack, callback);
}