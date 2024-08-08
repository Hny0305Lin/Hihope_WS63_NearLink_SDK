/**
 * @defgroup spinlock SpinLock
 * @ingroup linux
 */
#ifndef _LINUX_SPINLOCK_H
#define _LINUX_SPINLOCK_H

#include "los_typedef.h"
#include "los_spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct Spinlock spinlock_t;

#define DEFINE_SPINLOCK(x) spinlock_t x = SPIN_LOCK_INITIALIZER(x)

#define SPIN_UNUSED_PARAM(type, name) do { \
    type tmp1 = (name);                    \
    type tmp2 = tmp1;                      \
    tmp1 = tmp2;                           \
} while (0)

#define spin_lock_bh(lock) SPIN_UNUSED_PARAM(spinlock_t *, lock)

#define spin_unlock_bh(lock) SPIN_UNUSED_PARAM(spinlock_t *, lock)

#ifdef LOSCFG_KERNEL_SMP

/**
 * @ingroup  spinlock
 * @brief initialize the spinlock.
 *
 * @par Description:
 * This API is used to initialize the spinlock.
 *
 * @attention
 * <ul>
 * <li> This interface is implemented as stub function in single core
 * scenario. </li>
 * </ul>

 * @param  lock     [IN/OUT] the spinlock to be initialized.
 *
 * @retval  None.
 * @par Dependency:
 * <ul>
 * <li>spinlock.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define spin_lock_init(lock) LOS_SpinInit(lock)

/**
 * @ingroup  spinlock
 * @brief Get the spin lock.
 *
 * @par Description:
 * This API is used to get the spin lock.
 *
 * @attention
 * <ul>
 * <li> This interface is implemented as LOS_TaskLock function in single core
 * scenario. </li>
 * </ul>

 * @param  lock     [IN] the spinlock.
 *
 * @retval  None.
 * @par Dependency:
 * <ul>
 * <li>spinlock.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define spin_lock(lock) LOS_SpinLock(lock)

/**
 * @ingroup  spinlock
 * @brief release the spinlock.
 *
 * @par Description:
 * This API is used to release the spinlock.
 *
 * @attention
 * <ul>
 * <li> This interface is implemented as LOS_TaskUnlock function in single core
 * scenario. </li>
 * </ul>

 * @param  lock     [IN] the spinlock.
 *
 * @retval  None.
 * @par Dependency:
 * <ul>
 * <li>spinlock.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define spin_unlock(lock) LOS_SpinUnlock(lock)

/**
 * @ingroup  spinlock
 * @brief get the spinlock and disable local core interrupt.
 *
 * @par Description:
 * This API is used to get the spinlock and disable intterrupt.
 *
 * @attention
 * <ul>
 * <li> LOS_TaskUnlock </li>
 * </ul>

 * @param  lock     [IN] the spinlock.
 * @param  flags    [OUT] the interrupt status.
 *
 * @retval  None.
 * @par Dependency:
 * <ul>
 * <li>spinlock.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define spin_lock_irqsave(lock, flags) LOS_SpinLockSave(lock, (UINT32 *)&flags)

/**
 * @ingroup  spinlock
 * @brief release the spinlock and enable local core interrupt.
 *
 * @par Description:
 * This API is used to release the spinlock and enable local core interrupt.
 *
 * @attention
 * <ul>
 * <li>  </li>
 * </ul>

 * @param  lock     [IN] the spinlock.
 * @param  flags    [IN] the interrupt status.
 *
 * @retval  None.
 * @par Dependency:
 * <ul>
 * <li>spinlock.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define spin_unlock_irqrestore(lock, flags) LOS_SpinUnlockRestore(lock, (UINT32)flags)

#else
#define spin_lock_init(lock)  do {} while (0)
#define spin_lock(lock) LOS_TaskLock()
#define spin_unlock(lock) LOS_TaskUnlock()
#define spin_lock_irqsave(lock, flags)  do { \
    flags = LOS_IntLock();                   \
    spin_lock(lock);                         \
} while (0)

#define spin_unlock_irqrestore(lock, flags)  do { \
    spin_unlock(lock);                            \
    LOS_IntRestore(flags);                        \
} while (0)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_SPINLOCK_H */
