/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Lock Dependency Check Private HeadFile.
 * Author: Huawei LiteOS Team
 * Create: 2022-11-11
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

#ifndef _LOS_LOCKDEP_PRI_H
#define _LOS_LOCKDEP_PRI_H

#include "los_lockdep.h"
#include "los_mux.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern VOID LockDepMalloc(enum LockType type, UINT32 id);
#ifdef LOSCFG_KERNEL_MUXDEP
extern LosLockCheck *OsMuxLockDepGet(UINT32 id);
#define MUXDEP_CHECK_INIT(id) do {                              \
    LockDepMalloc(LOCK_MUTEX, id);                              \
    OsLockDepCheckInit(LOCK_MUTEX, OsMuxLockDepGet(id), id);    \
} while (0)

#define MUXDEP_CHECK_DEINIT(id)     OsLockDepCheckDeInit(LOCK_MUTEX, OsMuxLockDepGet(id), id)
#define MUXDEP_CHECK_IN(id)         OsLockDepCheckIn(LOCK_MUTEX, OsMuxLockDepGet(id), id)
#define MUXDEP_RECORD(id)           OsLockDepRecord(LOCK_MUTEX, OsMuxLockDepGet(id), id)
#define MUXDEP_CHECK_OUT(id)        OsLockDepCheckOut(LOCK_MUTEX, OsMuxLockDepGet(id), id)
#else
#define MUXDEP_CHECK_INIT(id)
#define MUXDEP_CHECK_DEINIT(id)
#define MUXDEP_CHECK_IN(id)
#define MUXDEP_RECORD(id)
#define MUXDEP_CHECK_OUT(id)
#endif

#ifdef LOSCFG_KERNEL_SEMDEP
extern LosLockCheck *OsSemLockDepGet(UINT32 id);
#define SEMDEP_CHECK_INIT(type, id) do {                            \
    if (type == OS_SEM_BINARY) {                                    \
        LockDepMalloc(LOCK_SEM, id);                                \
        OsLockDepCheckInit(LOCK_SEM, OsSemLockDepGet(id), id);      \
    }                                                               \
} while (0)

#define SEMDEP_CHECK_DEINIT(type, id) do {                          \
    if (type == OS_SEM_BINARY) {                                    \
        OsLockDepCheckDeInit(LOCK_SEM, OsSemLockDepGet(id), id);    \
    }                                                               \
} while (0)

#define SEMDEP_CHECK_IN(type, id)  do {                             \
    if (type == OS_SEM_BINARY) {                                    \
        OsLockDepCheckIn(LOCK_SEM, OsSemLockDepGet(id), id);        \
    }                                                               \
} while (0)

#define SEMDEP_RECORD(type, id) do {                                \
    if (type == OS_SEM_BINARY) {                                    \
        OsLockDepRecord(LOCK_SEM, OsSemLockDepGet(id), id);         \
    }                                                               \
} while (0)

#define SEMDEP_CHECK_OUT(type, id) do {                             \
    if (type == OS_SEM_BINARY) {                                    \
        OsLockDepCheckOut(LOCK_SEM, OsSemLockDepGet(id), id);       \
    }                                                               \
} while (0)
#else
#define SEMDEP_CHECK_INIT(type, id)
#define SEMDEP_CHECK_DEINIT(type, id)
#define SEMDEP_CHECK_IN(type, id)
#define SEMDEP_RECORD(type, id)
#define SEMDEP_CHECK_OUT(type, id)
#endif

#ifdef LOSCFG_PTHREAD_MUXDEP
extern VOID OsLockDepListInit(VOID);
extern VOID OsLockDepPthreadMuxInsert(MuxBaseCB *muxCB);
extern LosLockCheck *OsLockDepPthreadMuxGet(MuxBaseCB *muxCB);
extern VOID OsLockDepPthreadMuxDelete(MuxBaseCB *muxCB);

#define PTHREAD_MUXDEP_CHECK_INIT()         OsLockDepListInit()
#define PTHREAD_MUXDEP_CHECK_DEINIT(muxCB)  OsLockDepPthreadMuxDelete(muxCB)
#define PTHREAD_MUXDEP_CHECK_IN(muxCB)      OsLockDepCheckIn(LOCK_PTHREAD, OsLockDepPthreadMuxGet(muxCB), 0)
#define PTHREAD_MUXDEP_RECORD(muxCB)        OsLockDepRecord(LOCK_PTHREAD, OsLockDepPthreadMuxGet(muxCB), 0)
#define PTHREAD_MUXDEP_CHECK_OUT(muxCB)     OsLockDepCheckOut(LOCK_PTHREAD, OsLockDepPthreadMuxGet(muxCB), 0)
#define PTHREAD_MUXDEP_INSERT(muxCB)        OsLockDepPthreadMuxInsert(muxCB)
#else
#define PTHREAD_MUXDEP_CHECK_INIT(muxCB)
#define PTHREAD_MUXDEP_CHECK_DEINIT(muxCB)
#define PTHREAD_MUXDEP_CHECK_IN(muxCB)
#define PTHREAD_MUXDEP_RECORD(muxCB)
#define PTHREAD_MUXDEP_CHECK_OUT(muxCB)
#define PTHREAD_MUXDEP_INSERT(muxCB)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _LOS_LOCKDEP_PRI_H */
