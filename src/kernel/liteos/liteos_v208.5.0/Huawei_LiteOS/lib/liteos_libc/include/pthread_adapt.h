/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS pthread adapt
 * Author: Huawei LiteOS Team
 * Create: 2021-06-10
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

#ifndef _PTHREAD_ADAPT_H
#define _PTHREAD_ADAPT_H

#include "los_list.h"
#include "los_event.h"
#include "los_mux.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LITEOS_PTHREAD_MUX MuxBaseCB stLock
#define LITEOS_PTHREAD_EVENT EVENT_CB_S event
#define LITEOS_PTHREAD_EVENT_INITIALIZER { 0, { NULL, NULL } }
#define LITEOS_PTHREAD_INITIALIZER { { (struct LOS_DL_LIST *)NULL, (struct LOS_DL_LIST *)NULL }, NULL, 0 }
#define PTHREAD_COND_INITIALIZER   { { 0 }, -1, LITEOS_PTHREAD_EVENT_INITIALIZER, NULL, -1 }
#define PTHREAD_MUTEX_PRIORITY LOS_TASK_PRIORITY_LOWEST

#ifndef POSIX_MUTEX_DEFAULT_INHERIT
#define POSIX_MUTEX_DEFAULT_INHERIT
#if defined POSIX_MUTEX_DEFAULT_INHERIT
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
    { { PTHREAD_PRIO_INHERIT, PTHREAD_MUTEX_PRIORITY, PTHREAD_MUTEX_RECURSIVE_NP, 0 }, \
    LITEOS_PTHREAD_INITIALIZER }
#define PTHREAD_MUTEX_INITIALIZER \
    { { PTHREAD_PRIO_INHERIT, PTHREAD_MUTEX_PRIORITY, 0, 0 }, \
    LITEOS_PTHREAD_INITIALIZER }
#elif defined POSIX_MUTEX_DEFAULT_PROTECT
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
    { { PTHREAD_PRIO_PROTECT, PTHREAD_MUTEX_PRIORITY, PTHREAD_MUTEX_RECURSIVE_NP, 0 }, \
    LITEOS_PTHREAD_INITIALIZER }
#define PTHREAD_MUTEX_INITIALIZER \
    { { PTHREAD_PRIO_PROTECT, PTHREAD_MUTEX_PRIORITY, 0, 0 },  \
    LITEOS_PTHREAD_INITIALIZER }
#else
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
    { { PTHREAD_PRIO_NONE, PTHREAD_MUTEX_PRIORITY, PTHREAD_MUTEX_RECURSIVE_NP, 0 }, \
    LITEOS_PTHREAD_INITIALIZER }
#define PTHREAD_MUTEX_INITIALIZER \
    { { PTHREAD_PRIO_NONE, PTHREAD_MUTEX_PRIORITY, 0, 0 }, \
    LITEOS_PTHREAD_INITIALIZER }
#endif
#endif

#define PTHREAD_MUTEX_ERRORCHECK_NP 2
#define PTHREAD_MUTEX_RECURSIVE_NP  1

typedef struct pthread_mutexattr {
    UINT8 protocol;
    UINT8 prioceiling;
    UINT8 type;
    UINT8 reserved;
} pthread_mutexattr_t;

typedef struct pthread_mutex {
    pthread_mutexattr_t stAttr;
    LITEOS_PTHREAD_MUX;
} pthread_mutex_t;

typedef struct {
    clockid_t clock;
} pthread_condattr_t;

typedef struct pthread_cond {
    pthread_condattr_t stCondAttr;
    volatile int count;
    LITEOS_PTHREAD_EVENT;
    pthread_mutex_t* mutex;
    volatile int value;
} pthread_cond_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PTHREAD_ADAPT_H */
