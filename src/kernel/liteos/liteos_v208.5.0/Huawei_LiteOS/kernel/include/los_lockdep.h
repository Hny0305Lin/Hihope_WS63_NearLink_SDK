/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description: Lock Dependency Check.
 * Author: Huawei LiteOS Team
 * Create: 2018-10-18
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
 * --------------------------------------------------------------------------- */

/**
 * @defgroup los_lockdep lockdep
 * @ingroup kernel
 */

#ifndef _LOS_LOCKDEP_H
#define _LOS_LOCKDEP_H

#include "los_config.h"
#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

 /* lockdep supported lock type */
enum LockType {
#ifdef LOSCFG_KERNEL_SPINDEP
    LOCK_SPIN,
#endif
#ifdef LOSCFG_KERNEL_MUXDEP
    LOCK_MUTEX,
#endif
#ifdef LOSCFG_KERNEL_SEMDEP
    LOCK_SEM,
#endif
#ifdef LOSCFG_PTHREAD_MUXDEP
    LOCK_PTHREAD,
#endif
    LOCK_TYPE_MAX,
};

/* Define the structure of deadlock detection entity */
typedef struct {
    enum LockType   type;   /* lock type */
    UINT32          id;     /* semaphore  or mutex id */
    CHAR            *name;  /* spinlock name */
    UINT32          cpuId;  /* the cpu id when the lock is obtained. */
    VOID            *owner; /* the pointer to the lock owner's task control block. */
} LosLockCheck;

#ifdef LOSCFG_KERNEL_LOCKDEP

/* This Macro is used to obtain the pointer to a lock check entity using a corresponding parameter */
#define LOCKDEP_GET_ADDR(lockDep, index) ((lockDep)->heldLocks[(index)].lockAddr)

/* The max num that a task holding lock */
#define MAX_LOCK_DEPTH  16U

enum LockDepErrType {
    LOCKDEP_SUCCESS = 0,
    LOCKDEP_ERR_DOUBLE_LOCK,
    LOCKDEP_ERR_DEAD_LOCK,
    LOCKDEP_ERR_UNLOCK_WITHOUT_LOCK,
    LOCKDEP_ERR_OVERFLOW,
    LOCKDEP_ERR_MAX,
};

typedef struct {
    VOID *lockPtr;
    VOID *lockAddr;
    UINT64 waitTime;
    UINT64 holdTime;
} HeldLocks;

typedef struct {
    VOID *waitLock;
    INT32 lockDepth;
    HeldLocks heldLocks[MAX_LOCK_DEPTH];
} LockDep;
#endif

/* Note: The following functions are for internal use only. */

/* This API is used to init dead lock */
extern VOID OsLockDepCheckInit(enum LockType type, LosLockCheck *lock, UINT32 id);

/* This API is used to deinitialize dead lock */
extern VOID OsLockDepCheckDeInit(enum LockType type, LosLockCheck *lock, UINT32 id);

/* This API is used to check dead lock */
extern VOID OsLockDepCheckIn(enum LockType type, LosLockCheck *lock, UINT32 id);

/* This API is used to trace when a lock locked */
extern VOID OsLockDepRecord(enum LockType type, LosLockCheck *lock, UINT32 id);

/* This API is used to trace when a lock unlocked */
extern VOID OsLockDepCheckOut(enum LockType type, LosLockCheck *lock, UINT32 id);

#ifdef LOSCFG_KERNEL_SPINDEP
extern VOID OsLockdepClearSpinlocks(VOID);
#define LOCKDEP_CHECK_INIT(lock) do {                   \
    OsLockDepCheckInit(LOCK_SPIN, &lock->lockCheck, 0); \
    lock->lockCheck.name = "spinlock";                  \
} while (0)
#define LOCKDEP_CHECK_IN(lock)      OsLockDepCheckIn(LOCK_SPIN, &lock->lockCheck, 0)
#define LOCKDEP_RECORD(lock)        OsLockDepRecord(LOCK_SPIN, &lock->lockCheck, 0)
#define LOCKDEP_CHECK_OUT(lock)     OsLockDepCheckOut(LOCK_SPIN, &lock->lockCheck, 0)
#define LOCKDEP_CLEAR_LOCKS()       OsLockdepClearSpinlocks()
#else
#define LOCKDEP_CHECK_INIT(lock)
#define LOCKDEP_CHECK_IN(lock)
#define LOCKDEP_RECORD(lock)
#define LOCKDEP_CHECK_OUT(lock)
#define LOCKDEP_CLEAR_LOCKS()
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _LOS_LOCKDEP_H */
