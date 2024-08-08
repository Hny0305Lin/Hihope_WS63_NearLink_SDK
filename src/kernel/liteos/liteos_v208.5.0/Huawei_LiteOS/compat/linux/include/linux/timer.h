/**
 *  @defgroup timer Timer
 *  @ingroup linux
 */

#ifndef _LINUX_TIMER_H
#define _LINUX_TIMER_H

#include "linux/kernel.h"
#include "los_spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef unsigned long ULONG;

typedef struct timer_list {
    ULONG expires;                              /**< Timing duration of the timer. */
    VOID (*function)(struct timer_list *);      /**< Callback function that handles timer timeout. */
    UINT16 timerid;                             /**< Timer ID. */
    UINT32 flag;                                /**< Indicates whether the timer is valid. */
#define TIMER_VALID             0xABCDDCBA
#define TIMER_UNVALID           0xDCBAABCD
    BOOL created;                               /**< Not used. */
    SPIN_LOCK_S lock;                           /**< Private spinlock. */
} timer_list_t;

/* This API is not provided externally. */
static inline int timer_pending(const timer_list_t *timer)
{
    return 0;
}

/**
 * @ingroup  timer
 * @brief Initialize a timer.
 *
 * @par Description:
 * This API is used to initialize a timer.
 *
 * @attention
 * <ul>
 * <li>The parameter timer must be valid, otherwise, the system would be abnormal.</li>
 * </ul>
 *
 * @param  timer   [IN] timer handle.
 *
 * @retval  None.
 * @par Dependency:
 * <ul><li>timer.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern void init_timer(timer_list_t *timer);

/**
 * @ingroup  timer
 * @brief Statically defines and initializes a timer.
 *
 * @par Description:
 * This API is used to statically define and initialize a timer.
 *
 * @attention
 * <ul>
 * <li>The parameter timer must be valid, otherwise, the system would be abnormal.</li>
 * </ul>
 *
 * @param  timerName   [IN] the name of the rtimer to be defined.
 * @param  func        [IN] callback function that handles timer timeout.
 *
 * @retval  None.
 * @par Dependency:
 * <ul><li>timer.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
#define DEFINE_TIMER(timerName, func)                                           \
    timer_list_t timerName = {.expires = 0,                                     \
                              .function = (func),                               \
                              .timerid = 0,                                     \
                              .flag = TIMER_UNVALID,                            \
                              .created = FALSE,                                 \
                              SPIN_LOCK_INITIALIZER("linux_timer_spinlock")}

/**
 * @ingroup  timer
 * @brief Initialize a timer.
 *
 * @par Description:
 * This API is used to initialize a timer.
 *
 * @attention
 * <ul>
 * <li>The parameter timer must be valid, otherwise, the system would be abnormal.</li>
 * </ul>
 *
 * @param  timer       [IN] timer handle.
 * @param  function    [IN] callback function that handles timer timeout.
 * @param  flags       [IN] Indicates whether the timer is valid.
 *
 * @retval  None.
 * @par Dependency:
 * <ul><li>timer.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern void timer_setup(struct timer_list *timer, VOID (*function)(struct timer_list *), UINT32 flags);

/**
 * @ingroup  timer
 * @brief create a timer and start it.
 *
 * @par Description:
 * This API is used to create a timer and start it.
 *
 * @attention
 * <ul>
 * <li>The parameter timer must be valid, otherwise, the system would be abnormal. </li>
 * <li>Please make sure the domain expires, function, data, timerid inside of timer is valid, otherwise,
 * create timer would failure. Please refer to LOS_SwtmrCreate() for details.</li>
 * <li>Do not forget to initialize the structure with 'init_timer()'
 * before calling this function at the first time.</li>
 * </ul>
 *
 * @param  timer   [IN] timer handle.
 * The value range of timer->expires is [0, 0xFFFFFFFF].
 *
 * @retval  None.
 * @par Dependency:
 * <ul><li>timer.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern void add_timer(timer_list_t *timer);

/**
 * @ingroup  timer
 * @brief delete a timer.
 *
 * @par Description:
 * This API is used to delete a timer.
 *
 * @attention
 * <ul>
 * <li>The parameter timer must be valid, otherwise, the system would be abnormal. </li>
 * <li>Please make sure the domain timerid is valid, otherwise, delete timer would be failed.</li>
 * </ul>
 *
 * @param  timer   [IN] timer handle.
 *
 * @retval  #0 Delete an inactive timer or delete an active timer failure.
 * @retval  #1 Delete an active timer successful.
 * @par Dependency:
 * <ul><li>timer.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern int del_timer(timer_list_t *timer);

/**
 * @ingroup  timer
 * @brief delete a timer(as same as del_timer()).
 *
 * @par Description:
 * This API is used to delete a timer.
 *
 * @attention
 * <ul>
 * <li>The parameter t must be valid, otherwise, the system would be abnormal. </li>
 * <li>Please make sure the domain timerid is valid, otherwise, delete timer would be failed.</li>
 * </ul>
 *
 * @param  timer   [IN] timer handle.
 *
 * @retval  #0 Delete an inactive timer or delete an active timer failure.
 * @retval  #1 Delete an active timer successful.
 * @par Dependency:
 * <ul><li>timer.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
#define del_timer_sync(t)        del_timer(t)

/**
 * @ingroup  timer
 * @brief modifity a timer.
 *
 * @par Description:
 * This API is used to modifity a timer. It takes effect only if the timer is in using.
 * Actually, the specified timer would be deleted and then added again,
 * so you can refer to add_timer() for details about the input parameters.
 *
 * @attention
 * <ul>
 * <li>The parameter timer must be valid, otherwise, the system would be abnormal. </li>
 * <li>The parameter expires must not be 0, otherwise, modify timer would be failed. </li>
 * <li>Please make sure the domain expires, function, data, timerid inside of timer is valid,
 * otherwise, create timer would fail. Please refer to LOS_SwtmrCreate() for details.</li>
 * </ul>
 *
 * @param  timer   [IN] timer handle.
 * @param  expires [IN] Timing duration of the software timer to be created (unit: milisecond).
 *
 * @retval  None.
 * @par Dependency:
 * <ul><li>timer.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern int mod_timer(timer_list_t *timer, ULONG expires);

/**
 * @ingroup  timer
 * @brief Gets the address of the container structure.
 *
 * @par Description:
 * This API gets the address of the structure.
 * @attention
 * None.
 *
 * @param ptr           [IN] The pointer to the structure.
 * @param paramTimer    [IN] Input parameter of the callback function.
 * @param structTimer   [IN] The name of the #paramTimer in the structure.
 *
 * @retval The pointer to the address of the container structure.
 * @par Dependency:
 * <ul><li>timer.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
#define from_timer(retPointer, paramTimer, structTimer) container_of(paramTimer, typeof(*retPointer), structTimer)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_TIMER_H */
