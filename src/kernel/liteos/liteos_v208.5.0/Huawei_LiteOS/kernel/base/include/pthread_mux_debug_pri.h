/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Pthread Mutex Debug Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-07-23
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

#ifndef _PTHREAD_MUX_DEBUG_PRI_H
#define _PTHREAD_MUX_DEBUG_PRI_H

#include "los_mux_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern LOS_DL_LIST g_pthreadMuxList;
extern Atomic g_pthreadMutexAvailable;

#define NAME_BUFF_SIZE          12

/* Define the structure of pthread mux check */
typedef struct {
    LOS_DL_LIST  muxList;      /* insert mutex into global debug list */
    MuxBaseCB    *muxCB;       /* the mutex base object pointer */
    LosLockCheck deadLockDet;  /* deadlock detection entity */
} LosPthreadMuxCheck;

/* This Macro is used to obtain the pointer to a pthread mutex check entity using a corresponding parameter */
#define OS_PTHREAD_MUTEX_CHECK_FROM_DEADLOCK(ptr) LOS_DL_LIST_ENTRY(ptr, LosPthreadMuxCheck, deadLockDet)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PTHREAD_MUX_DEBUG_PRI_H */
