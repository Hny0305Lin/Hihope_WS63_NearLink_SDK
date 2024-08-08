/**
 * @defgroup semaphore Semaphore
 * @ingroup linux
 */

#ifndef _ASM_SEMAPHORE_H
#define _ASM_SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UNINIT_VALUE 0xFFFFFFFF
#define ERESTARTSYS 512 /* The return vlaue of down_interruptiable in Linux */

struct semaphore {
    unsigned int sem;
    int count;
};

typedef struct semaphore losMutexDef_t;
extern int _sema_lock(losMutexDef_t *sem);
extern int _sema_unlock(losMutexDef_t *sem);
extern int _sema_init(losMutexDef_t *sem, unsigned int value);
extern int _sema_destory(losMutexDef_t *sem);
extern int _sema_trylock(losMutexDef_t *sem);

/* The counting semaphore is initialized for use by the mutex lock. linux
 * standard adaptation macro function, do not modify. */
#define init_MUTEX(sem)         (void)_sema_init((losMutexDef_t*)(sem), 1)
#define init_MUTEX_LOCKED(sem)  (void)_sema_init((losMutexDef_t*)(sem), 0)
#define destory_MUTEX(sem)      _sema_destory((losMutexDef_t *)(sem))
#define DECLARE_MUTEX(x) struct semaphore x = { UNINIT_VALUE, 1 }
#define DECLARE_MUTEX_LOCKED(x) struct semaphore x = { UNINIT_VALUE, 0 }

/**
 * @ingroup  semaphore
 * @brief Defines and initializes a semaphore.
 *
 * @par Description:
 * This API is used to define a semaphore named #x and initialize it.
 *
 * @attention
 * None.
 *
 * @param  x     Name of the semaphore.
 * @retval #None
 * @par Dependency:
 * <ul>
 * <li>semaphore.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see sema_init.
 */
#define DEFINE_SEMAPHORE(x) DECLARE_MUTEX(x)

/**
 * @ingroup  semaphore
 * @brief Init the semaphore.
 *
 * @par Description:
 * This API is used to init the semaphore.
 *
 * @attention
 * <ul>
 * <li> Semaphore dynamic initialization interface, ONLY used as a binary semaphore.
 * </li>
 * </ul>
 *
 * @param  sem     [IN] the semaphore to be acquired.
 * @param  n       [IN] initial value.
 * @retval #None
 * @par Dependency:
 * <ul>
 * <li>semaphore.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see DEFINE_SEMAPHORE.
 */
#define sema_init(sem, n) (VOID)_sema_init((losMutexDef_t*)(sem), (n))

/**
 * @ingroup  semaphore
 * @brief acquire the semaphore.
 *
 * @par Description:
 * This API is used to acquire the semaphore. If no more tasks are allowed to
 * acquire the semaphore, calling this function will put the task to sleep until
 * the semaphore is released.
 *
 * @attention
 * <ul>
 * <li>Blocking interface, disallowed in interrupt isr handler. </li>
 * </ul>
 *
 * @param  sem     [IN] the semaphore to be acquired.
 *
 * @retval #None
 * @par Dependency:
 * <ul>
 * <li>semaphore.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see up.
 */
#define down(sem)               (void)_sema_lock((losMutexDef_t*)(sem))

/**
 * @ingroup  semaphore
 * @brief acquire the semaphore.
 *
 * @par Description:
 * This API is used to acquire the semaphore. If no more tasks are allowed to
 * acquire the semaphore, calling this function will put the task to sleep until
 * the semaphore is released.
 *
 * @attention
 * <ul>
 * <li> This interface is the same function as the down interface,
 * inconsistent with standard linux behavior. </li>
 * </ul>
 *
 * @param  sem     [IN] the semaphore to be acquired.
 *
 * @retval # 0     the semaphore has been acquired successfully.
 * @retval # -1    it cannot be acquired.
 * @par Dependency:
 * <ul>
 * <li>semaphore.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see down.
 */
#define down_interruptible(sem) _sema_lock((losMutexDef_t*)(sem))

/**
 * @ingroup  semaphore
 * @brief try to acquire the semaphore, without waiting.
 *
 * @par Description:
 * This API is used to try to acquire the semaphore, without waiting.
 *
 * @attention
 * <ul>
 * <li> The return value that cannot be obtained by the semaphore is
 * inconsistent with the Linux standard interface definition(linux standard return 1). </li>
 * </ul>
 *
 * @param  sem     [IN] the semaphore to be acquired.
 *
 * @retval # 0     the semaphore has been acquired successfully.
 * @retval # 1    it cannot be acquired.
 * @par Dependency:
 * <ul>
 * <li>semaphore.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see down.
 */
extern int down_trylock(struct semaphore *sem);

/**
 * @ingroup  semaphore
 * @brief release the semaphore.
 *
 * @par Description:
 * This API is used to release the semaphore.
 *
 * @attention
 * <ul>
 * <li> None. </li>
 * </ul>
 *
 * @param  sem     [IN] the semaphore to be acquired.
 *
 * @retval # None.
 * @par Dependency:
 * <ul>
 * <li>semaphore.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see down.
 */
#define up(sem)                 (void)_sema_unlock((losMutexDef_t*)(sem))

/**
 * @ingroup  semaphore
 * @brief destroy the semaphore at the address pointed to by sem.
 *
 * @par Description:
 * This API is used to destroy the semaphore.
 *
 * @attention
 * <ul>
 * <li> None. </li>
 * </ul>
 *
 * @param  sem     [IN] the semaphore to be acquired.
 *
 * @retval  -1     The semaphore fails to be destoryed.
 * @retval  0      The semaphore is successfully destoryed.
 * @par Dependency:
 * <ul>
 * <li>semaphore.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define sema_destory(sem)       _sema_destory((losMutexDef_t *)(sem))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_SEMAPHORE_H */
