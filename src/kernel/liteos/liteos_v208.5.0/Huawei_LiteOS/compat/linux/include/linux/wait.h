/**
 * @defgroup wait Wait
 * @ingroup linux
 */
#ifndef _LINUX_WAIT_H
#define _LINUX_WAIT_H

#include <linux/spinlock.h>
#include "los_event.h"
#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Notice about wait_queue_head_t:
 * 1.The stEvent is used for task-synchronization and has the same function as wait_event_head in Linux.
 *   In LiteOS, when wait_event is called, if the condition is not true, the task will be blocked and
 *   mounted on stEvent.stEventList. In Linux, the blocked task will be mounted on wait_queue_head.task_list.
 * 2.The lock and poll_queue are only used for poll operation: poll_queue is used to link poll_wait_node,
 *   and lock is used to protect this poll_queue.
 */
typedef struct wait_queue_head {
    EVENT_CB_S     stEvent;
    spinlock_t     lock;
    LOS_DL_LIST    poll_queue;
} wait_queue_head_t;

#define osWaitForever     0xFFFFFFFFU     /* wait forever timeout value */
#define DECLARE_WAIT_QUEUE_HEAD(wq) wait_queue_head_t wq = { { 0, &wq.stEvent.stEventList, &wq.stEvent.stEventList },  \
                                                             SPIN_LOCK_INITIALIZER("wait_queue_spinlock"), \
                                                             { &wq.poll_queue, &wq.poll_queue } }

void __init_waitqueue_head(wait_queue_head_t *wait);

/**
 * @ingroup  wait
 * @brief Initialize the waitqueue head.
 *
 * @par Description:
 * This API is used to initialize the waitqueue head.
 *
 * @attention
 * <ul>
 * <li>Please make sure the input parameter wait is valid, otherwise, the system would crash.</li>
 * </ul>
 *
 * @param  wait [IN]  Pointer to the waiting queue.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>Wait.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
#define init_waitqueue_head(wait) __init_waitqueue_head(wait)

void __wake_up_interruptible(wait_queue_head_t *wait);

/**
 * @ingroup  wait
 * @brief wakeup the process that registered on the wait queue.
 *
 * @par Description:
 * This API is used to wakeup the process that registered on the wait queue.
 *
 * @attention
 * <ul>
 * <li>Please make sure the input parameter wait is valid, otherwise, the system would crash.</li>
 * </ul>
 *
 * @param  wait [IN]  Pointer to the waiting queue.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>Wait.h: the header file that contains the API declaration.</li></ul>
 * @see wake_up.
 */
#define wake_up_interruptible(wait) __wake_up_interruptible(wait)

void __wake_up_interruptible_poll(wait_queue_head_t *wait, unsigned int key);

/**
 * @ingroup  wait
 * @brief wakeup the process that registered on the wait queue.
 *
 * @par Description:
 * This API is used to wakeup the process that registered on the wait queue.
 *
 * @attention
 * <ul>
 * <li>Please make sure the input parameter wait is valid, otherwise, the system would crash.</li>
 * <li>This function is used to wake up the queue head with multiple events waiting.</li>
 * </ul>
 *
 * @param  wait [IN]  Pointer to the waiting queue.
 * @param  key  [IN]  Mask for waiting events.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>Wait.h: the header file that contains the API declaration.</li></ul>
 * @see wake_up_interruptible.
 */
#define wake_up_interruptible_poll(wait, key) __wake_up_interruptible_poll(wait, key)

/**
 * @ingroup  wait
 * @brief wakeup the process that registered on the wait queue.
 *
 * @par Description:
 * This API is used to wake up the process that registered on the wait queue.
 *
 * @attention
 * <ul>
 * <li>Please make sure the input parameter wait is valid, otherwise, the system would crash.</li>
 * </ul>
 *
 * @param  wait [IN]  Pointer to the waiting queue.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>Wait.h: the header file that contains the API declaration.</li></ul>
 * @see wake_up_interruptible
 */
#define wake_up(wait)        wake_up_interruptible(wait)

/**
 * @ingroup wait
 * @brief sleep until a condition gets true.
 *
 * @attention
 * <ul>
 * <li>The OS does not support signal interrupts.</li>
 * <li>This event does not support interrupting by a signal.</li>
 * </ul>
 * @param  wait         [IN] the waitqueue to wait on.
 * @param  condition    [IN] a condition evaluates to true or false.
 *
 * @retval #0 always return 0
 *
 * @par Dependency:
 * <ul><li>wait.h: the header file that contains the API declaration.</li></ul>
 * @see wait_event.
 */
#define wait_event_interruptible(wait, condition) ({                                                       \
    int ret = 0;                                                                                           \
                                                                                                           \
    while (!(condition)) {                                                                                 \
        (void)LOS_EventRead(&(wait).stEvent, 0x1U, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER); \
    }                                                                                                      \
    ret;                                                                                                   \
})

/**
 * @ingroup wait
 * @brief sleep until a condition gets true.
 *
 * @par Description:
 * This API is used to sleep  a process until the condition evaluates to true.
 * The condition is checked each time when the waitqueue wait is woken up.
 *
 * @attention
 * None.
 *
 * @param  wait         [IN] the waitqueue to wait on.
 * @param  condition    [IN] a condition evaluates to true or false.
 *
 * @retval None.
 *
 * @par Dependency:
 * <ul><li>wait.h: the header file that contains the API declaration.</li></ul>
 * @see wait_event_interruptible.
 */
#define wait_event(wait, condition)  (VOID)wait_event_interruptible(wait, condition)

/**
 * @ingroup wait
 * @brief sleep until a condition gets true or a timeout elapses.
 *
 * @par Description:
 * This API is used to sleep  a process until the condition evaluates to true or a timeout elapses.
 * The condition is checked each time when the waitqueue wait is woken up.
 *
 * @attention
 * <ul>
 * <li>This API does not fully adapt to Linux, and the returned values are different.</li>
 * <li>The value range of parameter timeout is [0, 0xFFFFFFFF], and 0xFFFFFFFF means waiting forever.</li>
 * <li>This event does not support interrupting by a signal.</li>
 * </ul>
 *
 * @param  wait         [IN] the waitqueue to wait on.
 * @param  condition    [IN] a condition evaluates to true or false.
 * @param  timeout      [IN] the max sleep time (unit : Tick). it is jiffies in linux.
 *
 * @retval #0 return 0 if the condition evaluated to false after the timeout elapsed
 * @retval #1 return 1 if the condition evaluated to true after the timeout elapsed
 * @retval #2 return 2 if the condition evaluated to true and the timeout is osWaitForever
 * @retval #others return the remaining ticks if the condition evaluated to true before the timeout elapsed
 *
 * @par Dependency:
 * <ul><li>linux\wait.h: the header file that contains the API declaration.</li></ul>
 * @see wait_event_interruptible.
 */
#define wait_event_interruptible_timeout(wait, condition, timeout)  ({                               \
    INT32 tmpTimeout;                                                                                \
    UINT32 ret = (timeout);                                                                          \
    UINT64 ticksnow;                                                                                 \
                                                                                                     \
    ticksnow = LOS_TickCountGet();                                                                   \
    tmpTimeout = (timeout);                                                                          \
    if ((condition) && ((timeout) == 0)) {                                                           \
        ret = 1;                                                                                     \
    }                                                                                                \
    while (!(condition)) {                                                                           \
        ret = LOS_EventRead(&(wait).stEvent, 0x1U, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, tmpTimeout); \
        if ((timeout) == osWaitForever) {                                                            \
            if (!(condition)) {                                                                      \
                continue;                                                                            \
            }                                                                                        \
            ret = 2;                                                                                 \
            break;                                                                                   \
        }                                                                                            \
        tmpTimeout = (INT32)((timeout) - (UINT32)(LOS_TickCountGet() - ticksnow));                   \
                                                                                                     \
        /* If the timeout elapsed or read event times out, the wait ends. */                         \
        if ((tmpTimeout <= 0) || ((tmpTimeout > 0) && (ret == LOS_ERRNO_EVENT_READ_TIMEOUT))) {      \
            ret = (condition) ? 1 : 0;                                                               \
            break;                                                                                   \
        }                                                                                            \
                                                                                                     \
        if (condition) {                                                                             \
            ret = (UINT32)tmpTimeout;                                                                \
            break;                                                                                   \
        }                                                                                            \
    }                                                                                                \
    ret;                                                                                             \
})

#define add_wait_queue(wait, newWait) do {} while (0)
#define remove_wait_queue(wait, oldWait) do {} while (0)
#define DECLARE_WAITQUEUE(wait, current) do {} while (0)

/**
 * @ingroup wait
 * @brief Check whether the waitqueue is active.
 *
 * @par Description:
 * This API is used to check whether the waitqueue is active.
 *
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param  q    [IN] The waitqueue to be checked.
 *
 * @retval #0 The queue is not active.
 * @retval #1 The queue is active.
 *
 * @par Dependency:
 * <ul><li>linux\wait.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
static inline int waitqueue_active(wait_queue_head_t *q)
{
    return !LOS_ListEmpty(&(q->stEvent.stEventList));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_WAIT_H */
