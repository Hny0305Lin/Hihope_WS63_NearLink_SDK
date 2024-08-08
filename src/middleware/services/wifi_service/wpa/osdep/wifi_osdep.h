/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: header file for wifi os api
 */

#ifndef _WIFI_OSDEP_H_
#define _WIFI_OSDEP_H_

#if defined(CONFIG_OS_OSAL)
#include <soc_osal.h>

#elif defined(__LITEOS__)
#include "los_sys.h"
#include "los_task.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define WIFI_OS_OK 0
#define WIFI_OS_FAIL (-1)

#define WIFI_ERR_LOG printf

#if defined(CONFIG_OS_OSAL)
#define WIFI_WAIT_FOREVER OSAL_WAIT_FOREVER
#define WIFI_WAITMODE_OR OSAL_WAITMODE_OR
#define WIFI_WAITMODE_CLR OSAL_WAITMODE_CLR
/*
 * OSAL API cant distinguish between normal error and read timeout
 */
#define WIFI_ERRNO_EVENT_READ_TIMEOUT ((unsigned int)WIFI_OS_FAIL)
#define WIFI_TASK_STATUS_DETACHED 0
#elif defined(__LITEOS__)
#define WIFI_WAIT_FOREVER LOS_WAIT_FOREVER
#define WIFI_WAITMODE_OR LOS_WAITMODE_OR
#define WIFI_WAITMODE_CLR LOS_WAITMODE_CLR
#define WIFI_ERRNO_EVENT_READ_TIMEOUT LOS_ERRNO_EVENT_READ_TIMEOUT
#define WIFI_TASK_STATUS_DETACHED LOS_TASK_STATUS_DETACHED
#endif

typedef void *(*wifi_tsk_entry_func)(void *param);

typedef struct {
    unsigned int stack_size;   /* Task stack size.CNcomment:栈大小 CNend */
    char *task_name;           /* Task name.CNcomment:任务名 CNend */
    wifi_tsk_entry_func task_entry; /* Task entry.CNcomment:任务入口。CNend */
    const void *arg;           /* cbk paramter.CNcomment:回调函数入参 CNend */
    unsigned int task_policy;  /* Task schedule policy. Not used by default.CNcomment:任务调度策略。默认没有使用。CNend */
    unsigned int task_cpuid;   /* CPUID to which the task belongs. Not used by default.
                                  CNcomment:任务所属的CPUID。默认没有使用。CNend */
    unsigned short task_prio;  /* Task priority.CNcomment:任务优先级 CNend */
    unsigned short res;         /* Reserved. Not used by default.CNcomment:预留字段。默认没有使用。CNend */
} wifi_task_attr;

typedef enum {
    WIFI_EVENT_WPA    = 0,
    WIFI_EVENT_ELOOP  = 1,
    WIFI_EVENT_SOFTAP = 2,
    WIFI_EVENT_P2P    = 3,
    WIFI_EVENT_MAX
} wifi_event_enum;

unsigned long long os_tick_cnt_get(void);

void os_intlock(unsigned int *int_save);
void os_intrestore(unsigned int int_save);

void os_task_lock(unsigned int *int_save);
void os_task_unlock(unsigned int int_save);
void os_task_delay(unsigned int ms);
int os_task_create(unsigned int *task_id, wifi_task_attr *attr);
void os_task_delete(unsigned int task_id);

int os_event_init(unsigned int *id, wifi_event_enum num);
int os_event_read(unsigned int id, unsigned int mask, unsigned int *event_bits,
                  unsigned int flag, unsigned int timeout_ms);
int os_event_poll(unsigned int id, unsigned int mask, unsigned int *event_bits, unsigned int flag);
int os_event_write(unsigned int id, unsigned int events);
int os_event_clear(unsigned int id, unsigned int events);
int os_event_del(unsigned int id);

int os_mux_create(unsigned int *mux_id);
int os_mux_pend(unsigned int mux_id, unsigned int timeout_ms);
int os_mux_post(unsigned int mux_id);
int os_mux_delete(unsigned int mux_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* _WIFI_OSDEP_H_ */
