/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS libc lock
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

#ifndef _LIBC_LOCK_ADAPT_H
#define _LIBC_LOCK_ADAPT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* init pthread_mutex for __fdopen and stdin stdout stderr in LiteOS & seLiteOS */
#define __INIT_LOCK_ATTR(_lock) \
        _lock.stAttr.protocol = PTHREAD_PRIO_INHERIT, \
        _lock.stAttr.prioceiling = LOS_TASK_PRIORITY_LOWEST, \
        _lock.stAttr.type = PTHREAD_MUTEX_RECURSIVE_NP, \
        _lock.stAttr.reserved = 0, \
        _lock.stLock.muxList.pstPrev = (struct LOS_DL_LIST *)NULL, \
        _lock.stLock.muxList.pstNext = (struct LOS_DL_LIST *)NULL, \
        _lock.stLock.owner = NULL, \
        _lock.stLock.muxCount = 0

extern int LibcLock(unsigned int *lock);
extern int LibcUnlock(const unsigned int *lock);
#define LIBC_LOCK_INITIALIZER UINT_MAX
#define LIBC_LOCK_INIT(x) unsigned int x = LIBC_LOCK_INITIALIZER
#define LIBC_LOCK(x) LibcLock(&(x))
#define LIBC_UNLOCK(x) LibcUnlock(&(x))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LIBC_LOCK_ADAPT_H */
