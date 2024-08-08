/**
 * @defgroup rwsem Read write semaphore
 * @ingroup linux
 */

#ifndef _LINUX_RWSEM_H
#define _LINUX_RWSEM_H

#include "los_list.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RWSEM_UNINIT_VALUE 0xFFFFFFFF

/**
 * @ingroup rwsem
 * rw semaphore structure.
 */
struct rw_semaphore {
    UINT32 rwsemHandle;
};

extern void InitRwsem(struct rw_semaphore *rwsem);

/**
 * @ingroup  rwsem
 * @brief Statically defines and initializes a read/write semaphore.
 *
 * @par Description:
 * This API is used to statically define and initialize a read/write semaphore.
 *
 * @attention
 * None.
 *
 * @param  semName     [IN] the name of the read/write semaphore to be defined.
 *
 * @retval #None
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define DECLARE_RWSEM(semName)  \
        struct rw_semaphore (semName) = {RWSEM_UNINIT_VALUE}

/**
 * @ingroup  rwsem
 * @brief Dynamically initializes a read/write semaphore.
 *
 * @par Description:
 * This API is used to dynamically initialize a read/write semaphore.
 *
 * @attention
 * None.
 *
 * @param  rwsem     [IN] the semaphore to be initialized.
 *
 * @retval #None
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define init_rwsem(rwsem)   InitRwsem(rwsem)

/**
 * @ingroup  rwsem
 * @brief acquire the semaphore for reading.
 *
 * @par Description:
 * This API is used to acquire the semaphore for reading. If no more tasks are allowed to acquire the semaphore,
 * calling this function will put the task to sleep until the semaphore is released.
 *
 * @attention
 * <ul>
 * <li>Blocking interface, disallowed in interrupt isr handler. </li>
 * </ul>
 *
 * @param  rwsem     [IN] the semaphore to be acquired.
 *
 * @retval #None
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
void down_read(struct rw_semaphore *rwsem);

/**
 * @ingroup  rwsem
 * @brief try to acquire the semaphore for reading, without waiting.
 *
 * @par Description:
 * This API is used to try to acquire the semaphore for reading, without waiting.
 *
 * @attention
 * <ul>
 * <li> The return value is inconsistent with the Linux standard interface definition. </li>
 * </ul>
 *
 * @param  rwsem     [IN] the semaphore to be acquired.
 *
 * @retval # 1    the semaphore has been acquired successfully.
 * @retval # 0    it cannot be acquired.
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
int down_read_trylock(struct rw_semaphore *rwsem);

/**
 * @ingroup  rwsem
 * @brief acquire the semaphore for writing.
 *
 * @par Description:
 * This API is used to acquire the semaphore for writing. If no more tasks are allowed to acquire the semaphore,
 * calling this function will put the task to sleep until the semaphore is released.
 *
 * @attention
 * <ul>
 * <li>Blocking interface, disallowed in interrupt isr handler. </li>
 * </ul>
 *
 * @param  rwsem     [IN] the semaphore to be acquired.
 *
 * @retval #None
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
void down_write(struct rw_semaphore *rwsem);

/**
 * @ingroup  rwsem
 * @brief try to acquire the semaphore for writing, without waiting.
 *
 * @par Description:
 * This API is used to try to acquire the semaphore for writing, without waiting.
 *
 * @attention
 * <ul>
 * <li> The return value is inconsistent with the Linux standard interface definition. </li>
 * </ul>
 *
 * @param  rwsem     [IN] the semaphore to be acquired.
 *
 * @retval # 1     the semaphore has been acquired successfully.
 * @retval # 0     it cannot be acquired.
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
int down_write_trylock(struct rw_semaphore *rwsem);

/**
 * @ingroup  rwsem
 * @brief release the read semaphore.
 *
 * @par Description:
 * This API is used to release the read semaphore.
 *
 * @attention
 * None.
 *
 * @param  rwsem     [IN] the semaphore to be acquired.
 *
 * @retval # None.
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
void up_read(struct rw_semaphore *rwsem);

/**
 * @ingroup  rwsem
 * @brief release the write semaphore.
 *
 * @par Description:
 * This API is used to release the write semaphore.
 *
 * @attention
 * None.
 *
 * @param  rwsem     [IN] the semaphore to be acquired.
 *
 * @retval # None.
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
void up_write(struct rw_semaphore *rwsem);

/**
 * @ingroup  rwsem
 * @brief Downgrades the write semaphore to the read semaphore.
 *
 * @par Description:
 * This API is used to downgrade the write semaphore to the read semaphore.
 *
 * @attention
 * None.
 *
 * @param  rwsem     [IN] the semaphore to be acquired.
 *
 * @retval # None.
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
void downgrade_write(struct rw_semaphore *rwsem);

/**
 * @ingroup  rwsem
 * @brief destroy the semaphore at the address pointed to by rwsem.
 *
 * @par Description:
 * This API is used to destroy the read and write semaphore.
 *
 * @attention
 * None.
 *
 * @param  rwsem     [IN] the semaphore to be acquired.
 *
 * @retval # 1     the semaphore has been destoryed successfully.
 * @retval # 0     it cannot be destoryed.
 * @par Dependency:
 * <ul>
 * <li>rwsem.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
int rwsem_destory(struct rw_semaphore *rwsem);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_RWSEM_H */
