
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2022. All rights reserved.
 * Description: osal adapt atomic event file.
 */
#include "osal_adapt.h"

/************************ osal_event ************************/
int osal_adapt_event_init(osal_event *event_obj)
{
    return osal_event_init(event_obj);
}

int osal_adapt_event_write(osal_event *event_obj, unsigned int mask)
{
    return osal_event_write(event_obj, mask);
}

int osal_adapt_event_read(osal_event *event_obj, unsigned int mask, unsigned int timeout_ms, unsigned int mode)
{
    return osal_event_read(event_obj, mask, timeout_ms, mode);
}

int osal_adapt_event_clear(osal_event *event_obj, unsigned int mask)
{
    return osal_event_clear(event_obj, mask);
}

int osal_adapt_event_destroy(osal_event *event_obj)
{
    return osal_event_destroy(event_obj);
}
