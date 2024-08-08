/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS compat headfile for nuttx vfs_oflags
 * Author: Huawei LiteOS Team
 * Create: 2022-09-27
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

#ifndef __INCLUDE_NUTTX_VFS_OFLAGS_H
#define __INCLUDE_NUTTX_VFS_OFLAGS_H

#include "fcntl.h"
#include "stdbool.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

static inline bool vfs_posix_oflags_is_readonly(int oflags)
{
    return (((unsigned int)oflags & O_ACCMODE) == O_RDONLY);
}

static inline bool vfs_posix_oflags_is_writeonly(int oflags)
{
    return (((unsigned int)oflags & O_ACCMODE) == O_WRONLY);
}

static inline bool vfs_posix_oflags_with_truncate(int oflags)
{
    return (oflags & O_TRUNC);
}

static inline bool vfs_posix_oflags_with_append(int oflags)
{
    return (oflags & O_APPEND);
}

static inline bool vfs_posix_oflags_with_directory(int oflags)
{
    return (oflags & O_DIRECTORY);
}

#endif /* __INCLUDE_NUTTX_VFS_OFLAGS_H */
