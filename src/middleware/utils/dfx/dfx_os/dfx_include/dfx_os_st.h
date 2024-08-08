/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx os st
 * This file should be changed only infrequently and with great care.
 */

#ifndef DFX_OS_ST_H
#define DFX_OS_ST_H
#include <stdint.h>
#include <stdbool.h>

#define EXT_OS_STAT_MSG_WAIT_FAIL     0x1   /* Used in the interrupt context.CNcomment:中断上下文使用CNend */
#define EXT_OS_STAT_MSG_SEND_FAIL     0x2   /* The timeout period of the interrupt context configuration is not 0,
                                              and the queue is full.CNcomment:中断上下文配置超时时间不为0, 队列满 CNend */
#define EXT_OS_STAT_SEM_WAIT_FAIL     0x4   /* Used in the interrupt context.CNcomment:中断上下文使用CNend */
#define EXT_OS_STAT_SEM_SIGNAL_FAIL   0x8   /* Repeated release.CNcomment:重复释放CNend */
#define EXT_OS_STAT_MUX_PEND_FAIL     0x10  /* Used in the interrupt context.CNcomment:中断上下文使用CNend  */
#define EXT_OS_STAT_MUX_POST_FAIL     0x20  /* Cross-task use, not created.CNcomment:跨任务使用，未创建CNend  */
#define EXT_OS_STAT_EVENT_WAIT_FAIL   0x40  /* Used in the interrupt context.CNcomment:中断上下文使用 CNend */
#define EXT_OS_STAT_EVENT_SEND_FAIL   0x80  /* Initialized EVT resources used up.
                                              CNcomment:初始化EVT资源已经用完 CNend */
#define EXT_OS_STAT_EVENT_CLR_FAIL    0x100 /* Invalid input argument.CNcomment:入参错误 CNend */
#define EXT_OS_STAT_SLEEP_FAIL        0x200 /* Used in the interrupt context.CNcomment:中断上下文使用CNend  */
#define EXT_OS_STAT_START_TIMER_FAIL  0x400 /* Invalid input argument.CNcomment:入参错误 CNend */
#define EXT_OS_STAT_CREATE_TIMER_FAIL 0x800 /* WorkQueue used up.CNcomment:创建定时器句柄失败 CNend */

#define DFX_TASK_NAME_LEN     32
typedef struct {
    char name[DFX_TASK_NAME_LEN]; /* Task entrance function.CNcomment:入口函数CNend */
    bool valid;
    uint32_t id;                      /* Task ID.CNcomment:任务ID CNend */
    uint16_t status;                  /* Task status. Status detail see los_task_pri.h.CNcomment:任务状态。
                                         详细状态码请参考los_task_pri.h CNend */
    uint16_t priority;                /* Task priority.CNcomment:任务优先级 CNend */
    void *task_sem;                   /* Semaphore pointer.CNcomment:信号量指针CNend */
    void *task_mutex;                 /* Mutex pointer.CNcomment:互斥锁指针CNend */
    uint32_t event_stru[3];           /* Event: 3 nums.CNcomment:3个事件CNend */
    uint32_t event_mask;              /* Event mask.CNcomment:事件掩码CNend */
    uint32_t stack_size;              /* Task stack size.CNcomment:栈大小CNend */
    uint32_t top_of_stack;            /* Task stack top.CNcomment:栈顶CNend */
    uint32_t bottom_of_stack;         /* Task stack bottom.CNcomment:栈底CNend */
    uint32_t sp;                      /* Task SP pointer.CNcomment:当前SP.CNend */
    uint32_t curr_used;               /* Current task stack usage.CNcomment:当前任务栈使用率CNend */
    uint32_t peak_used;               /* Task stack usage peak.CNcomment:栈使用峰值CNend */
    uint32_t overflow_flag;           /* Flag that indicates whether a task stack overflow occurs.
                                       CNcomment:栈溢出标记位CNend */
} task_info_t;

/**
 * @ingroup os_stat
 * System resource usage statistic.CNcomment:系统资源使用统计量。CNend
 */
typedef struct {
    uint8_t timer_usage;  /* Number of used system timers.CNcomment:当前使用系统定时器个数 CNend */
    uint8_t task_usage;   /* Number of used tasks.CNcomment:当前使用任务个数 CNend */
    uint8_t sem_usage;    /* Number of used semaphores.CNcomment:当前使用信号量个数 CNend */
    uint8_t queue_usage;  /* Number of used message queues.CNcomment:当前使用消息队列个数 CNend */
    uint8_t mux_usage;    /* Number of used mutexes.CNcomment:当前使用互斥锁个数 CNend */
    uint8_t event_usage;  /* Number of used events.CNcomment:当前使用事件个数 CNend */
    uint16_t err_info;    /* Error statistic EXT_OS_STAT_XXX, used to log occurred errors.
                           CNcomment:错误统计量EXT_OS_STAT_XXX，用于记录曾经出现过的错误 CNend */
} os_resource_use_stat_t;

typedef struct {
    uint32_t total;                /* Total space of the memory pool (unit: byte).
                                    CNcomment:内存池总大小（单位：byte）CNend */
    uint32_t used;                 /* Used space of the memory pool (unit: byte).
                                    CNcomment:内存池已经使用大小（单位：byte）CNend */
    uint32_t free;                 /* Free space of the memory pool (unit: byte).
                                    CNcomment:内存池剩余空间（单位：byte）CNend */
    uint32_t free_node_num;        /* Number of free nodes in the memory pool.
                                    CNcomment:内存池剩余空间节点个数 CNend */
    uint32_t used_node_num;        /* Number of used nodes in the memory pool.
                                    CNcomment:内存池已经使用的节点个数 CNend */
    uint32_t max_free_node_size;   /* Maximum size of the node in the free space of the memory pool (unit: byte).
                                    CNcomment:内存池剩余空间节点中最大节点的大小（单位：byte）CNend */
    uint32_t peek_size;            /* Peak memory usage of the memory pool.CNcomment:内存池使用峰值CNend */
} mdm_mem_info_t;

typedef struct {
    uint32_t display_mode;
    uint32_t clear_flag;
    uint32_t int_flag;
} diag_cpup_cmd_t;

typedef struct {
    char name[DFX_TASK_NAME_LEN];
    uint32_t id;
    uint32_t usage;
} dfx_cpup_item_ind_t;

typedef struct {
    uint8_t valid : 1;   /**< Whether the information is valid. true: yes; false: no. */
    uint8_t task : 1;    /**< Whether the message is a task or an interrupt. true: task; false: interrupt. */
    uint8_t pad0 : 6;    /**< Reserved. */
    uint8_t id;          /**< Task ID/Interrupt number. */
    uint16_t pad1;       /**< Reserved. */
    uint32_t permillage; /**< CPU usage (per mil). */
} cpup_item_t;

typedef struct {
    uint16_t status;
    uint32_t usage;
} dfx_cpup_item_usage_info_t;

typedef struct {
    uint8_t timer_usage;  /* Number of used system timers. */
    uint8_t task_usage;   /* Number of used tasks. */
    uint8_t sem_usage;    /* Number of used semaphores. */
    uint8_t queue_usage;  /* Number of used message queues. */
    uint8_t mux_usage;    /* Number of used mutexes. */
} osal_os_resource_use_stat_t;

#endif
