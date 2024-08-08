/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS compat headfile for nuttx dirent
 * Author: Huawei LiteOS Team
 * Create: 2022-07-15
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

#ifndef __INCLUDE_NUTTX_DIRENT_H
#define __INCLUDE_NUTTX_DIRENT_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "fs/dirent_fs.h"
#include "nuttx/semaphore.h"
#include "stdlib.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DTYPE_UNKNOWN             0
#define DTYPE_FIFO                1
#define DTYPE_CHR                 2
#define DTYPE_SEM                 3
#define DTYPE_DIRECTORY           4
#define DTYPE_MQ                  5
#define DTYPE_BLK                 6
#define DTYPE_SHM                 7
#define DTYPE_FILE                8
#define DTYPE_MTD                 9
#define DTYPE_LINK                10
#define DTYPE_SOCK                12

#define DIRENT_MAGIC     0x11CBA828       /* Magic number to express the status of a dirent */

static inline bool vfs_dir_virify(struct fs_dirent_s *idir)
{
    return (idir->fd_status != DIRENT_MAGIC);
}

static inline void vfs_set_dir_verify(struct fs_dirent_s *idir)
{
    idir->fd_status = DIRENT_MAGIC;
}

static inline void vfs_reset_dir_verify(struct fs_dirent_s *idir)
{
    idir->fd_status = 0;
}

static inline void vfs_reset_dir_position(struct fs_dirent_s *idir)
{
    idir->fd_position = 0;
}

#endif /* __INCLUDE_NUTTX_DIRENT_H */
