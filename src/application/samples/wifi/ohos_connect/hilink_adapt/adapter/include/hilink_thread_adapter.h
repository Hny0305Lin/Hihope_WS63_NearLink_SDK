/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 线程适配层接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_THREAD_ADAPTER_H
#define HILINK_THREAD_ADAPTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HILINK_WAIT_FOREVER 0xFFFFFFFFU

typedef void* HiLinkTaskId;

typedef void* HiLinkMutexId;

typedef void* HiLinkSemId;

typedef void (*HiLinkTaskEntryFunc)(void *arg);

typedef enum HiLinkTaskPrio {
    HILINK_TASK_PRIORITY_MIN = 0,
    HILINK_TASK_PRIORITY_LOW,
    HILINK_TASK_PRIORITY_MID,
    HILINK_TASK_PRIORITY_HIGH,
    HILINK_TASK_PRIORITY_MAX,
    /* 避免编译器优化，限制该枚举值为32-bit */
    HILINK_TASK_PRIORITY_REVERSED = 0x7FFFFFFF
} HiLinkTaskPrio;

typedef struct HiLinkTaskParam {
    HiLinkTaskEntryFunc func;
    HiLinkTaskPrio prio;
    unsigned int stackSize;
    void *arg;
    const char *name;
} HiLinkTaskParam;

/*
 * 描述：创建线程接口
 * 参数：param，线程参数
 * 返回: NULL创建线程失败，其他线程句柄
 */
HiLinkTaskId HILINK_CreateTask(HiLinkTaskParam *param);

/*
 * 描述：挂起线程接口
 * 参数：handle，挂起线程句柄
 * 返回：0成功, 其他失败
 */
int HILINK_ThreadSuspend(HiLinkTaskId handle);

/*
 * 描述：恢复线程接口
 * 参数：handle，恢复线程句柄
 * 返回：0成功，其他失败
 */
int HILINK_ThreadResume(HiLinkTaskId handle);

/*
 * 描述：删除线程接口
 * 参数：handle，删除线程句柄
 */
void HILINK_DeleteTask(HiLinkTaskId handle);

/*
 * 描述：获取当前运行线程的句柄
 * 返回：当前运行线程的句柄
 */
HiLinkTaskId HILINK_GetCurrentTaskId(void);

/*
 * 描述：创建互斥锁
 * 返回：NULL失败, 其他互斥锁句柄
 */
HiLinkMutexId HILINK_MutexCreate(void);

/*
 * 描述：互斥锁加锁
 * 参数：mutex，互斥锁
 *       ms，超时时间，为HILINK_WAIT_FOREVER不会超时
 * 返回：0成功, 其他失败
 */
int HILINK_MutexLock(HiLinkMutexId mutex, unsigned int ms);

/*
 * 描述：互斥锁解锁
 * 参数：mutex，互斥锁句柄
 * 返回：0成功，其他失败
 */
int HILINK_MutexUnlock(HiLinkMutexId mutex);

/*
 * 描述：互斥锁释放
 * 参数：mutex，互斥锁句柄
 */
void HILINK_MutexDestroy(HiLinkMutexId mutex);

/*
 * 描述：创建信号量
 * 参数：count，指定信号量值的初始值大小
 * 返回：NULL失败, 其他信号量句柄
 */
HiLinkSemId HILINK_SemCreate(unsigned int count);

/*
 * 描述：信号量的值减1
 * 参数：handle，信号量句柄
 *       ms，阻塞超时时间，为HILINK_WAIT_FOREVER不会超时
 * 返回：0表示成功, 其他值表示失败
 */
int HILINK_SemWait(HiLinkSemId handle, unsigned int ms);

/*
 * 描述：信号量的值加1
 * 参数：handle，信号量句柄
 * 返回：0表示成功, 其他值表示失败
 */
int HILINK_SemPost(HiLinkSemId handle);

/*
 * 描述：销毁信号量
 * 参数：handle，信号量句柄
 */
void HILINK_SemDestroy(HiLinkSemId handle);

/*
 * 描述：将进程休眠指定的时间
 * 参数：ms，休眠的毫秒数
 * 返回：0成功，其他失败
 */
int HILINK_MilliSleep(unsigned int ms);

/**
 * @brief 让出系统调度以使其他等待线程可以执行
 */
void HILINK_SchedYield(void);

#ifdef __cplusplus
}
#endif

/* 打开互斥锁检测调试功能 */
#ifdef SUPPORT_MUTEX_DEBUG
#include "mutex_debug_utils.h"
#endif

#endif /* HILINK_THREAD_ADAPTER_H */
