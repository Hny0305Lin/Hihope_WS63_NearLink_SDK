/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Spinlock
 * Author: Huawei LiteOS Team
 * Create: 2018-07-11
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

/**
 * @defgroup los_spinlock Spinlock
 * @ingroup kernel
 */

#ifndef _LOS_SPINLOCK_H
#define _LOS_SPINLOCK_H
#include "los_typedef.h"
#include "los_config.h"
#include "los_hwi.h"
#include "los_task.h"
#include "los_lockdep.h"
#include "arch/spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_KERNEL_SPINDEP
#define OS_SPINLOCK_FROM_SPINLOCK_CHECK(ptr) LOS_DL_LIST_ENTRY(ptr, SPIN_LOCK_S, lockCheck)
#define SPIN_LOCK_INITIALIZER(lockName) \
{0U, {LOCK_SPIN, 0, #lockName, 0xFFFFFFFF, NULL}}
#else
#define SPIN_LOCK_INITIALIZER(lockName) \
{                                       \
    .rawLock    = 0U,                   \
}
#endif

/**
 * @ingroup  los_spinlock
 * <ul>
 * <li>This macro is used to define the input parameter lock as a spinlock, and initialize the
 *     spinlock statically.</li>
 * <li>This macro has no return value.</li>
 * <li>Note that the input parameter lock does not need to be defined before calling this macro.
 *     Otherwise, the variable lock is repeatedly defined.</li>
 * <li>On Non-SMP (UP) mode, this macro has no effect.</li>
 * <li>The spinlock is advised to protect operation that take a short time. Otherwise, the overall system
 *     performance may be affected because the thread exits the waiting loop only after the spinlock is
 *     obtained. For time-consuming operation, the mutex lock can be used instead of spinlock.</li>
 * </ul>
 */
#define SPIN_LOCK_INIT(lock)  SPIN_LOCK_S lock = SPIN_LOCK_INITIALIZER(lock)

/**
 * @ingroup  los_spinlock
 * Define the structure of spinlock.
 */
struct Spinlock {
    size_t      rawLock;            /**< raw spinlock */
#ifdef LOSCFG_KERNEL_SPINDEP
    LosLockCheck lockCheck;         /**< lock check entity */
#endif
};
typedef struct Spinlock SPIN_LOCK_S;
#ifdef LOSCFG_KERNEL_SMP
/**
 * @ingroup  los_spinlock
 * @brief Lock the spinlock.
 *
 * @par Description:
 * This API is used to lock the spinlock. If the spinlock has been obtained by another thread,
 * the thread will wait cyclically until it can lock the spinlock successfully.
 *
 * @attention
 * <ul>
 * <li>The spinlock must be initialized before it is used. It should be initialized by #LOS_SpinInit
 *     or #SPIN_LOCK_INIT.</li>
 * <li>The parameter passed in should be a legal pointer.</li>
 * <li>A spinlock can not be locked for multiple times in a task. Otherwise, a deadlock occurs.</li>
 * <li>If the spinlock will be used in both task and interrupt, using #LOS_SpinLockSave instead of
 *     this API.</li>
 * <li>On Non-SMP (UP) mode, this function has no effect.</li>
 * </ul>
 *
 * @param  lock [IN] Type #SPIN_LOCK_S The pointer to spinlock.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinTrylock | LOS_SpinLockSave | LOS_SpinUnlock | LOS_SpinUnlockNoSched | SPIN_LOCK_INIT | LOS_SpinInit
 * @since Huawei LiteOS V200R003C00
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinLock(SPIN_LOCK_S *lock)
{
    /*
     * disable the scheduler, so it won't do schedule until
     * scheduler is reenabled. The LOS_TaskUnlock should not
     * be directly called along this critic area.
     */
    LOS_TaskLock();

    LOCKDEP_CHECK_IN(lock);
    ArchSpinLock(&lock->rawLock);
    LOCKDEP_RECORD(lock);
}

/**
 * @ingroup  los_spinlock
 * @brief Trying to lock the spinlock.
 *
 * @par Description:
 * This API is used to try to lock the spinlock. If the spinlock has been obtained by another thread,
 * this API will not waiting for the lock's owner to release the spinlock and return the failure directly.
 *
 * @attention
 * <ul>
 * <li>The spinlock must be initialized before it is used. It should be initialized by #LOS_SpinInit
 *     or #SPIN_LOCK_INIT.</li>
 * <li>The parameter passed in should be a legal pointer.</li>
 * <li>A spinlock can not be locked for multiple times in a task. Otherwise, a deadlock occurs.</li>
 * <li>On Non-SMP (UP) mode, this function has no effect.</li>
 * </ul>
 *
 * @param  lock [IN] Type #SPIN_LOCK_S The pointer to spinlock.
 *
 * @retval #LOS_OK   Got the spinlock.
 * @retval #LOS_NOK  Failed to get the spinlock.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinLock | LOS_SpinLockSave | LOS_SpinUnlock | LOS_SpinUnlockNoSched | SPIN_LOCK_INIT | LOS_SpinInit
 * @since Huawei LiteOS V200R003C00
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE INT32 LOS_SpinTrylock(SPIN_LOCK_S *lock)
{
    LOS_TaskLock();

    LOCKDEP_CHECK_IN(lock);
    INT32 ret = ArchSpinTrylock(&lock->rawLock);
    if (ret == LOS_OK) {
        LOCKDEP_RECORD(lock);
    }

    return ret;
}

/**
 * @ingroup  los_spinlock
 * @brief Unlock the spinlock.
 *
 * @par Description:
 * This API is used to unlock the spin lock.
 *
 * @attention
 * <ul>
 * <li>The parameter passed in should be a legal pointer.</li>
 * <li>On Non-SMP (UP) mode, this function has no effect. </li>
 * </ul>
 *
 * @param  lock [IN] Type #SPIN_LOCK_S The pointer to spinlock.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinLock | LOS_SpinTrylock | LOS_SpinLockSave | LOS_SpinUnlockRestore
 * @since Huawei LiteOS V200R003C00
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinUnlock(SPIN_LOCK_S *lock)
{
    LOCKDEP_CHECK_OUT(lock);
    ArchSpinUnlock(&lock->rawLock);

    /* Restore the scheduler flag. May cause task schedule. */
    LOS_TaskUnlock();
}

/**
 * @ingroup  los_spinlock
 * @brief Unlock the spinlock and no scheduling is generated.
 *
 * @par Description:
 * This API is used to unlock the spin lock without scheduling.
 *
 * @attention
 * <ul>
 * <li>The parameter passed in should be a legal pointer.</li>
 * <li>On Non-SMP (UP) mode, this function has no effect.</li>
 * <li>Unlocking does not generate scheduling. Therefore, there is a delay for waiting tasks to be scheduled.
 * Users needs proactive scheduling to ensure timing.</li>
 * <li> For example, this function can be used in spin lock switching, make sure the first spin unlock not trigger
 * unnecessary scheduling. Then, the second spin lock invokes LOS_SpinUnlock to ensure scheduling.</li>
 * </ul>
 *
 * @param  lock [IN] Type #SPIN_LOCK_S The pointer to spinlock.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinLock | LOS_SpinTrylock | LOS_SpinLockSave
 * @since Huawei LiteOS 207.0.0
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinUnlockNoSched(SPIN_LOCK_S *lock)
{
    LOCKDEP_CHECK_OUT(lock);
    ArchSpinUnlock(&lock->rawLock);

    /* Restore the scheduler flag only. */
    LOS_TaskUnlockNoSched();
}

/**
 * @ingroup  los_spinlock
 * @brief Lock the spinlock and disable all interrupts.
 *
 * @par Description:
 * This API is used to lock the spinlock and disable all interrupts before locking. After
 * the interrupts are disabled, this API executes exactly the same as #LOS_SpinLock.
 * @attention
 * <ul>
 * <li>The spinlock must be initialized before it is used. It should be initialized by
 *     #LOS_SpinInit or #SPIN_LOCK_INIT.</li>
 * <li>The parameter passed in should be a legal pointer.</li>
 * <li>A spinlock can not be locked for multiple times in a task. Otherwise, a deadlock
*      occurs.</li>
 * <li>On Non-SMP (UP) mode, this function only disables all interrupts.</li>
 * </ul>
 *
 * @param  lock     [IN]    Type #SPIN_LOCK_S The pointer to spinlock.
 * @param  intSave  [OUT]   Type #UINT32 The pointer is used to save the interrupt flag
 *                                       before all interrupts are disabled. It will be
 *                                       used by #LOS_SpinUnlockRestore.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinLock | LOS_SpinTrylock | LOS_SpinUnlockRestore
 * @since Huawei LiteOS V200R003C00
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinLockSave(SPIN_LOCK_S *lock, UINT32 *intSave)
{
    *intSave = LOS_IntLock();
    LOS_SpinLock(lock);
}

/**
 * @ingroup  los_spinlock
 * @brief Unlock the spinlock and restore the interrupt flag.
 *
 * @par Description:
 * This API is used to unlock the spinlock and restore the interrupts by restoring the interrupt flag.
 * This API can be called only after calling #LOS_SpinLockSave, and the input parameter value should
 * be the parameter returned by #LOS_SpinLockSave.
 *
 * @attention
 * <ul>
 * <li>The parameter passed in should be a legal pointer.</li>
 * <li>On Non-SMP (UP) mode, this function only restore interrupt flag.</li>
 * </ul>
 *
 * @param  lock     [IN]    Type #SPIN_LOCK_S The pointer to spinlock.
 * @param  intSave  [IN]    Type #UINT32 The interrupt flag need to be restored.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinUnlock | LOS_SpinLockSave
 * @since Huawei LiteOS V200R003C00
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinUnlockRestore(SPIN_LOCK_S *lock, UINT32 intSave)
{
    LOS_SpinUnlock(lock);
    LOS_IntRestore(intSave);
}

/**
 * @ingroup  los_spinlock
 * @brief Check if holding the spinlock.
 *
 * @par Description:
 * This API is used to check if the spinlock is held or not.
 *
 * @attention
 * <ul>
 * <li>The parameter passed in should be a legal pointer.</li>
 * <li>On Non-SMP (UP) mode, this function always returns #TRUE.</li>
 * </ul>
 *
 * @param  lock     [IN]    Type #SPIN_LOCK_S The pointer to spinlock.
 *
 * @retval #TRUE   The spinlock is held.
 * @retval #FALSE  The spinlock is not held.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R003C00
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE BOOL LOS_SpinHeld(const SPIN_LOCK_S *lock)
{
    return (lock->rawLock != 0);
}

/**
 * @ingroup  los_spinlock
 * @brief Spinlock dynamic initialization.
 *
 * @par Description:
 * This API is used to initialize a spinlock dynamically.
 *
 * @attention
 * <ul>
 * <li>The spinlock is advised to protect operation that take a short time. Otherwise, the overall system
 *     performance may be affected because the thread exits the waiting loop only after the spinlock is
 *     obtained. For time-consuming operation, the mutex lock can be used instead of spinlock.</li>
 * <li>The parameter passed in should be a legal pointer.</li>
 * <li>On Non-SMP (UP) mode, this function has no effect.</li>
 * </ul>
 *
 * @param  lock     [IN/OUT]    Type #SPIN_LOCK_S The pointer to spinlock need to be initialized.
 *
 * @retval None.
 *
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R003C00
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinInit(SPIN_LOCK_S *lock)
{
    lock->rawLock = 0;
    LOCKDEP_CHECK_INIT(lock);
}

#else

/*
 * For Non-SMP system, these apis does not handle with spinlocks,
 * but for unifying the code of drivers, vendors and etc.
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinLock(SPIN_LOCK_S *lock)
{
    (VOID)lock;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE INT32 LOS_SpinTrylock(SPIN_LOCK_S *lock)
{
    (VOID)lock;
    return LOS_OK;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinUnlock(SPIN_LOCK_S *lock)
{
    (VOID)lock;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinUnlockNoSched(SPIN_LOCK_S *lock)
{
    (VOID)lock;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinLockSave(SPIN_LOCK_S *lock, UINT32 *intSave)
{
    (VOID)lock;
    *intSave = LOS_IntLock();
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinUnlockRestore(SPIN_LOCK_S *lock, UINT32 intSave)
{
    (VOID)lock;
    LOS_IntRestore(intSave);
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE BOOL LOS_SpinHeld(const SPIN_LOCK_S *lock)
{
    (VOID)lock;
    return TRUE;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinInit(SPIN_LOCK_S *lock)
{
    (VOID)lock;
}

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
