/**
 *  @defgroup linux Linux
 *  @defgroup delay Delay
 *  @ingroup linux
 */
#ifndef _ASM_DELAY_H
#define _ASM_DELAY_H

#include "los_typedef.h"
#include "los_tick.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef udelay
/**
 * @ingroup  delay
 * @brief It is same with #LOS_Udelay.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 */
#define udelay(n)   LOS_Udelay(n)
#endif

#ifndef mdelay
/**
 * @ingroup  delay
 * @brief It is same with #LOS_Mdelay.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 */
#define mdelay(n)   LOS_Mdelay(n)
#endif

/**
 * @ingroup delay
 * @brief Delay current task.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to delay current task.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The unit of param is millisecond, and the minimum is 1 tick.</li>
 * <li>This API is not an accurate delay function. The actual delay time is longer than that of parameter msecs.</li>
 * </ul>
 *
 * @param msecs     [IN] Type #unsigned int time to delay current task.
 * @retval None.
 * @par Dependency:
 * <ul><li>delay.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V100R001C00
 */
extern void msleep(unsigned int msecs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
