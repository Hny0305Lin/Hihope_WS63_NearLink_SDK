/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides reset driver source \n
 *
 * History: \n
 * 2023-10-18， Create file. \n
 */
#include "reset.h"
#include "hal_reboot.h"
#include "cpu_utils.h"

void RebootDevice(unsigned int cause)
{
    cpu_utils_set_system_status_by_cause(cause);
    hal_reboot_chip();
}