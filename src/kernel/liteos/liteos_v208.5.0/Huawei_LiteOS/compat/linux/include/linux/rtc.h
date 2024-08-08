/**
 * @defgroup rtc Rtc
 * @ingroup linux
 */

#ifndef _LINUX_RTC_H
#define _LINUX_RTC_H

#include "los_typedef.h"
#include "uapi/linux/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup rtc
 * @brief Check whether it is a leap year.
 *
 * @par Description:
 * This API is used to check whether it is a leap year.
 *
 * @attention
 * None.
 *
 * @param  year     [IN] year.
 *
 * @retval # 0      #year is not a leap year.
 * @retval # 1      #year is a leap year.
 * @par Dependency:
 * <ul>
 * <li>rtc.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see
 */
static inline bool is_leap_year(unsigned int year)
{
    /* 4, 100, 400: leap year can be divided by 4 and cannot be divided by 100, or it can be divided by 400 */
    return ((!(year % 4) && (year % 100)) || !(year % 400));
}

/**
 * @ingroup rtc
 * @brief Converts the RTC time to seconds.
 *
 * @par Description:
 * This API is used to convert the current time to the number of seconds that have elapsed since 1970.
 *
 * @attention
 * None.
 *
 * @param  tm     [IN]  RTC time.
 * @param  time   [OUT] Number of seconds after conversion.
 *
 * @retval # 0      Always returns 0.
 * @par Dependency:
 * <ul>
 * <li>rtc.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see
 */
int rtc_tm_to_time(struct rtc_time *tm, unsigned long *time);

/**
 * @ingroup rtc
 * @brief Converts the seconds to RTC time.
 *
 * @par Description:
 * This API is used to convert the seconds to RTC time.
 *
 * @attention
 * None.
 *
 * @param  time   [IN] seconds.
 * @param  tm     [OUT] RTC time.
 *
 * @retval # None.
 * @par Dependency:
 * <ul>
 * <li>rtc.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see
 */
void rtc_time_to_tm(unsigned long time, struct rtc_time *tm);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_RTC_H */
