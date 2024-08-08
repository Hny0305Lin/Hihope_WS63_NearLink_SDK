/**
 * @defgroup clock
 * @ingroup linux
 */
#ifndef _LINUX_CLOCK_H
#define _LINUX_CLOCK_H

#include "los_typedef.h"

/**
 * @ingroup clock
 * @brief get the system time(ns)
 *
 * @par Description:
 * This API is used to get the system time by read the value of system timer.
 * @attention
 * <ul>
 * <li>system should init the system timer first.</li>
 * </ul>
 *
 * @param None
 *
 * @retval unsigned long long   the system time(ns).
 * @par Dependency:
 * <ul><li>clock.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R003C00
 */
extern unsigned long long sched_clock(void);

#endif
