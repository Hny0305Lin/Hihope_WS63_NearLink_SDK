/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: ohos cmsis adapt file.
 */
#include "los_task_pri.h"
#include "los_hwi.h"
#include "cmsis_os2.h"

void *osThreadGetArgument(void)
{
    if (OS_INT_ACTIVE) {
        return NULL;
    }
    LosTaskCB *taskCb = (LosTaskCB *)osThreadGetId();
    if (taskCb == NULL) {
        return NULL;
    }
#ifdef LOSCFG_OBSOLETE_API
    return (void *)(taskCb->args[0]);
#else
    return (void *)(taskCb->args);
#endif
}

void osThreadExit(void)
{
}