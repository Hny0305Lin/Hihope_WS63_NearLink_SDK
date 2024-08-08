/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: event
 *
 * Create: 2021-12-16
 */

#include <los_event.h>
#include <los_memory.h>
#include "soc_osal.h"
#include "osal_inner.h"

#define BIT_31 (1 << 31)

int osal_event_init(osal_event *event_obj)
{
    unsigned int ret;
    if (event_obj == NULL || event_obj->event != NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    event_obj->event = (EVENT_CB_S *)LOS_MemAlloc((void*)m_aucSysMem0, sizeof(EVENT_CB_S));
    if (event_obj->event == NULL) {
        osal_log("LOS_MemAlloc failed!\n");
        return OSAL_FAILURE;
    }
    ret = LOS_EventInit(event_obj->event);
    if (ret != LOS_OK) {
        LOS_MemFree((void*)m_aucSysMem0, event_obj->event);
        event_obj->event = NULL;
        osal_log("LOS_EventInit failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}
int osal_event_write(osal_event *event_obj, unsigned int mask)
{
    unsigned int ret;
    if (event_obj == NULL || (mask & BIT_31) == 1) {
        osal_log("parameter invalid! mask=%#x.\n", mask);
        return OSAL_FAILURE;
    }
    ret = LOS_EventWrite(event_obj->event, mask);
    if (ret != OSAL_SUCCESS) {
        osal_log("LOS_EventWrite failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}
int osal_event_read(osal_event *event_obj, unsigned int mask, unsigned int timeout_ms, unsigned int mode)
{
    unsigned int ret;
    unsigned int timeout;
    if (event_obj == NULL || (mask & BIT_31) == 1) {
        osal_log("parameter invalid! mask=%#x.\n", mask);
        return OSAL_FAILURE;
    }
    timeout = (timeout_ms == OSAL_EVENT_FOREVER) ? LOS_WAIT_FOREVER : LOS_MS2Tick(timeout_ms);
    ret = LOS_EventRead(event_obj->event, mask, mode, timeout);
    if (!ret || (ret & LOS_ERRTYPE_ERROR)) {
        osal_log("LOS_EventRead failed! ret=%#x. mask=%#x. mode=%#x\n", ret, mask, mode);
        return OSAL_FAILURE;
    } else {
        return (int)ret;
    }
}
int osal_event_clear(osal_event *event_obj, unsigned int mask)
{
    unsigned int ret;
    if (event_obj == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    ret = LOS_EventClear(event_obj->event, ~mask);
    if (ret != OSAL_SUCCESS) {
        osal_log("LOS_EventClear failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

int osal_event_destroy(osal_event *event_obj)
{
    unsigned int ret;
    if (event_obj == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    ret = LOS_EventDestroy(event_obj->event);
    LOS_MemFree((void*)m_aucSysMem0, event_obj->event);
    event_obj->event = NULL;
    if (ret != OSAL_SUCCESS) {
        osal_log("LOS_EventDestroy failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}
