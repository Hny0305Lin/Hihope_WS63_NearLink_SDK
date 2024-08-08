/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: completion
 *
 * Create: 2021-12-16
 */

#include <los_memory.h>
#include <linux/completion.h>
#include "soc_osal.h"
#include "osal_inner.h"

#ifdef __FREERTOS__
int osal_completion_init(osal_completion *com)
{
    if (com == NULL || com->completion != NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    com->completion = (osal_completion *)LOS_MemAlloc((void*)m_aucSysMem0, sizeof(completion_t));
    if (com->completion == NULL) {
        osal_log("LOS_MemAlloc failed!\n");
        return OSAL_FAILURE;
    }
    init_completion(com->completion);
    return OSAL_SUCCESS;
}

void osal_complete(osal_completion *com)
{
    if (com == NULL || com->completion == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    complete(com->completion);
}

void osal_wait_for_completion(osal_completion *com)
{
    if (com == NULL || com->completion == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    wait_for_completion(com->completion);
}

/*
 * Timeout interval for waiting on the completion (unit: Tick).
*/
unsigned long osal_wait_for_completion_timeout(osal_completion *com, unsigned long timeout)
{
    if (com == NULL || com->completion == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    return wait_for_completion_timeout(com->completion, timeout);
}

void osal_complete_all(osal_completion *com)
{
    if (com == NULL || com->completion == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    complete_all(com->completion);
}

void osal_complete_destory(osal_completion *com)
{
    if (com == NULL || com->completion == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    LOS_MemFree((void*)m_aucSysMem0, com->completion);
    com->completion = NULL;
}
#endif
