/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: msgqueue
 *
 * Create: 2021-12-16
 */

#include <los_queue.h>
#include "soc_osal.h"
#include "osal_errno.h"
#include "osal_inner.h"

#define OSAL_INVALID_MSG_NUM 0xFFFFFFFF

#ifndef TINY_KERNEL
int osal_msg_queue_create(const char *name, unsigned short queue_len, unsigned long *queue_id,
    unsigned int flags, unsigned short max_msgsize)
{
    unsigned int ret = LOS_QueueCreate(name, queue_len, (unsigned int *)queue_id, flags, max_msgsize);
    if (ret != LOS_OK) {
        osal_log("LOS_QueueCreate failed! ret = %#x.\n", ret);
        return OSAL_FAILURE;
    }
    if (name != NULL && queue_id != NULL) {
        osal_log("qName:%s qID=0x%x \r\n", name, *queue_id);
    }
    return (int)ret;
}
#endif

int osal_msg_queue_write_copy(unsigned long queue_id, void *buffer_addr, unsigned int buffer_size, unsigned int timeout)
{
    unsigned int ret = LOS_QueueWriteCopy(queue_id, buffer_addr, buffer_size, timeout);
    if (ret != LOS_OK) {
        osal_log("LOS_QueueWriteCopy failed! ret = %#x.qID=0x%x\n", ret, queue_id);
        return OSAL_FAILURE;
    }
    return (int)ret;
}

int osal_msg_queue_write_head_copy(unsigned long queue_id, void *buffer_addr, unsigned int buffer_size,
    unsigned int timeout)
{
    unsigned int ret = LOS_QueueWriteHeadCopy(queue_id, buffer_addr, buffer_size, timeout);
    if (ret != LOS_OK) {
        osal_log("LOS_QueueWriteHeadCopy failed! ret = %#x.qID=0x%x\n", ret, queue_id);
        return OSAL_FAILURE;
    }
    return (int)ret;
}

int osal_msg_queue_read_copy(unsigned long queue_id, void *buffer_addr, unsigned int *buffer_size, unsigned int timeout)
{
    unsigned int ret = LOS_QueueReadCopy(queue_id, buffer_addr, buffer_size, timeout);
    if (ret != LOS_OK) {
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
}

void osal_msg_queue_delete(unsigned long queue_id)
{
    unsigned int ret = LOS_QueueDelete(queue_id);
    if (ret != LOS_OK) {
        osal_log("LOS_QueueDelete failed! ret = %#x.\n", ret);
    }
}

int osal_msg_queue_is_full(unsigned long queue_id)
{
    QUEUE_INFO_S q_inf = { 0 };
    unsigned int ret;

    ret = LOS_QueueInfoGet(queue_id, &q_inf);
    if (ret != OSAL_SUCCESS) {
        return TRUE;
    }

#ifndef CONFIG_SEC_CORE
    return q_inf.usReadableCnt == q_inf.usQueueLen;
#else
    return q_inf.info.readWriteableCnt[0] == q_inf.info.queueLen;
#endif
}

unsigned int osal_msg_queue_get_msg_num(unsigned long queue_id)
{
    QUEUE_INFO_S q_inf = { 0 };
    unsigned int ret;

    ret = LOS_QueueInfoGet(queue_id, &q_inf);
    if (ret != OSAL_SUCCESS) {
        return OSAL_INVALID_MSG_NUM;
    }

#ifndef CONFIG_SEC_CORE
    return q_inf.usReadableCnt;
#else
    return q_inf.info.readWriteableCnt[0];
#endif
}
