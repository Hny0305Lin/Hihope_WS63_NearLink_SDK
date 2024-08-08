/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: LiteOS VFS Private Headfile
 * Author: Huawei LiteOS Team
 * Create: 2023-1-31
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

#ifndef _VFS_EXTEND_H
#define _VFS_EXTEND_H

#include "los_fs.h"

#define MS_RDONLY 1
#define MS_NOSYNC 2

#define FALLOC_FL_KEEP_SIZE 1 /* extend size */

#ifdef LOSCFG_FS_FAT
/* Format options (3rd argument of format) */
#define FMT_FAT      0x01
#define FMT_FAT32    0x02
#define FMT_ANY      0x07
#define FMT_ERASE    0x08

/* system time flag for FAT */
#define FAT_SYSTEM_TIME_ENABLE   0x01
#define FAT_SYSTEM_TIME_DISABLE  0x00
#endif

#ifdef LOSCFG_FS_PROC
#define PROCFS_MOUNT_POINT      "/proc"
#define PROCFS_MOUNT_POINT_SIZE (sizeof(PROCFS_MOUNT_POINT) - 1)
#endif

#ifdef LOSCFG_FS_YAFFS
#define YAFFS_MOUNT_POINT       "/yaffs"
#define YAFFS_MOUNT_POINT_SIZE  (sizeof(YAFFS_MOUNT_POINT) - 1)
#endif

#ifdef LOSCFG_FS_RAMFS
#define RAMFS_MOUNT_POINT       "/ramfs"
#define RAMFS_MOUNT_POINT_SIZE  (sizeof(RAMFS_MOUNT_POINT) - 1)
#endif

/* yaffs and fatfs cache configur */
/* config block size for fat file system, only can be 0,32,64,128,256,512,1024 */

#define CONFIG_FS_FAT_SECTOR_PER_BLOCK  256
#define CONFIG_NFILE_DESCRIPTORS_PER_BLOCK CONFIG_NFILE_DESCRIPTORS
/* config block num for fat file system */

#ifdef LOSCFG_FS_FAT_BLOCK_NUMS
#define CONFIG_FS_FAT_BLOCK_NUMS        LOSCFG_FS_FAT_BLOCK_NUMS
#else
#define CONFIG_FS_FAT_BLOCK_NUMS        32
#endif

#ifdef LOSCFG_FS_FAT_CACHE_SYNC_THREAD

/* config the priority of sync task */

#define CONFIG_FS_FAT_SYNC_THREAD_PRIO  10

/* config dirty ratio of bcache for fat file system */

#define CONFIG_FS_FAT_DIRTY_RATIO       50

/* config time interval of sync thread for fat file system, in milliseconds */

#define CONFIG_FS_FAT_SYNC_INTERVAL     5000
#endif

#define CONFIG_FS_FLASH_BLOCK_NUM 1

/* page numbers of per yaffs block */

#define CONFIG_FS_YLIB_PAGE_PER_BLOCK (64 * CONFIG_FS_FLASH_BLOCK_NUM)

/* config erase size for yaffs file system */

#define CONFIG_FS_YAFFS_BLOCK_ERASE_SIZE (2048 * CONFIG_FS_YLIB_PAGE_PER_BLOCK)

/* config block size for yaffs file system */

#define CONFIG_FS_YAFFS_BLOCK_SIZE  ((2048 + 64) * CONFIG_FS_YLIB_PAGE_PER_BLOCK)

/* config cache size for yaffs file system */

#define CONFIG_FS_YAFFS_BLOCK_CACHE_MEMSIZE (16*CONFIG_FS_YAFFS_BLOCK_SIZE)

#endif /* _VFS_EXTEND_H */
