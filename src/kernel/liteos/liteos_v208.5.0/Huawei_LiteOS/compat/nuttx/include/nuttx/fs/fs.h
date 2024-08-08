/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS compat headfile for nuttx fs
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

#ifndef __INCLUDE_NUTTX_FS_H
#define __INCLUDE_NUTTX_FS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "fs/fs.h"
#include "fs/file.h"
#include "los_tables.h"
#include "inode/inode.h"
#include "nuttx/vfs_oflags.h"
#include "nuttx/fs/ioctl.h"
#include "nuttx/drivers/drivers.h"
#include "limits.h"
#include "unistd.h"
#include "nuttx/net/net.h"
#ifdef LOSCFG_COMPAT_LINUX
#include "poll.h"
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define file_operations file_operations_vfs

#ifdef LOSCFG_COMPAT_LINUX
#define POLL_IN_SET (POLLIN | POLLRDNORM | POLLRDBAND | POLLHUP | POLLERR)
#define POLL_OUT_SET (POLLOUT | POLLWRBAND | POLLWRNORM | POLLERR)
#define POLL_EX_SET (POLLPRI)
#endif

#define DEV_PATH_LEN  5

#define FFCNTL      (FNONBLOCK | FNDELAY | FAPPEND | FFSYNC | FASYNC)

static inline bool vfs_driver_path_verify(const char *path)
{
    return (path == NULL || strlen(path) >= PATH_MAX || strncmp("/dev/", path, DEV_PATH_LEN) != 0);
}

static inline void vfs_release_fullpath(char *path)
{
    free(path);
}

static inline int vfs_error_get(void)
{
    return -errno;
}

static inline bool vfs_console_fd_verify(int fd)
{
    return ((fd <= STDERR_FILENO) && (fd >= STDIN_FILENO)); /* fd : [0,2] */
}

#ifdef LOSCFG_NET_LWIP_SACK
static inline bool vfs_invalid_fd_verify(int fd)
{
    return ((unsigned int)fd >= CONFIG_NFILE_DESCRIPTORS + CONFIG_NSOCKET_DESCRIPTORS);
}

static inline bool vfs_socket_fd_verify(int fd)
{
    return ((unsigned int)fd >= CONFIG_NFILE_DESCRIPTORS &&
           (unsigned int)fd < (CONFIG_NFILE_DESCRIPTORS + CONFIG_NSOCKET_DESCRIPTORS));
}
#else
static inline bool vfs_invalid_fd_verify(int fd)
{
    return ((unsigned int)fd >= CONFIG_NFILE_DESCRIPTORS);
}

static inline bool vfs_socket_fd_verify(int fd)
{
    return false;
}
#endif
static inline void vfs_inode_free(struct inode *node)
{
    (VOID)LOS_MemFree(m_aucSysMem0, node);
}

static inline bool tmpfs_path_len_verify(const char *path)
{
    if (strlen(path) == 0) {
        return false;
    }
    return true;
}

static inline bool tmpfs_param_negative_verify(off_t tmp)
{
    return tmp < 0;
}

static inline bool nfs_rpc_path_verify(size_t size)
{
    return (size < RPC_PATH_MAX);
}

#ifdef LOSCFG_COMPAT_LINUX
static inline pollevent_t vfs_pollfd_events_get(pollevent_t condition)
{
    if (condition == POLLIN) {
        return (POLLIN | POLLRDNORM);
    } else if (condition == POLLOUT) {
        return (POLLOUT | POLLWRNORM);
    }

    return OK;
}

static inline bool vfs_revents_pollset_check(pollevent_t revents, pollevent_t condition)
{
    switch (condition) {
        case (POLLIN):
            return (revents & POLL_IN_SET);
        case (POLLOUT):
            return (revents & POLL_OUT_SET);
        case (POLLERR):
            return (revents & POLL_EX_SET);
        default :
            return false;
    }
}
#endif
static inline bool vfs_seek_check(FAR struct file *filep)
{
    return (file_seek64(filep, 0, SEEK_END) < 0);
}

#endif /* __INCLUDE_NUTTX_FS_H */
