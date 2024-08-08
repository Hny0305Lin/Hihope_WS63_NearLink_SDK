/**
 * @defgroup jiffies Jiffies
 * @ingroup linux
 */

#ifndef _LINUX_JIFFIES_H
#define _LINUX_JIFFIES_H

#include "los_sys.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_JIFFY_OFFSET ((LONG_MAX >> 1) - 1)

/**
 * @ingroup jiffies
 * @brief It is same with #LOS_TickCountGet.
 */
extern unsigned long long get_jiffies_64(void);

/**
 * @ingroup jiffies
 * @brief It is same with #LOS_Tick2MS.
 *
 * @attention
 * <ul>
 * <li>This API is not fully compatible with linux.</li>
 * <li>The calculation result is rounded down.</li>
 * </ul>
 */
extern unsigned int jiffies_to_msecs(const unsigned long j);

/**
 * @ingroup jiffies
 * @brief Convert milliseconds to jiffies.
 *
 * @par Description:
 * This API is used to convert milliseconds to jiffies.
 * @attention
 * <ul>
 * <li>This API is not fully compatible with linux.</li>
 * <li>IF (HZ > OS_SYS_MS_PER_SECOND) || (OS_SYS_MS_PER_SECOND % HZ), #ENOSUPP will be returned.</li>
 * </ul>
 *
 * @param m  [IN] The milliseconds need to be converted to jiffies.
 *
 * @retval   #unsigned long Number of jiffies obtained through the conversion.
 * @par Dependency:
 * <ul>
 * <li>jiffies.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern unsigned long msecs_to_jiffies(const unsigned int m);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_JIFFIES_H */
